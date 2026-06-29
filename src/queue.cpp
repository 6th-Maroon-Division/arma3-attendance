#include "queue.h"
#include "httprequest.h"

#include <chrono>

// ============================================================================
// EventQueue implementation
// ============================================================================

EventQueue::EventQueue() {}

EventQueue::~EventQueue() {
    Stop();
}

void EventQueue::Push(const PlayerEvent& event) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(event);
    }
    condVar_.notify_one();
}

PlayerEvent EventQueue::Pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    condVar_.wait(lock, [this] { 
        return !queue_.empty() || stopped_.load(); 
    });
    
    if (stopped_.load() && queue_.empty()) {
        return PlayerEvent{"", false, ""};
    }
    
    PlayerEvent event = queue_.front();
    queue_.pop();
    return event;
}

bool EventQueue::TryPop(PlayerEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return false;
    }
    event = queue_.front();
    queue_.pop();
    return true;
}

bool EventQueue::Empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

void EventQueue::Stop() {
    stopped_.store(true);
    condVar_.notify_all();
}

bool EventQueue::IsStopped() const {
    return stopped_.load();
}

bool EventQueue::HasWork() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !queue_.empty() || stopped_.load();
}

// ============================================================================
// EventProcessor implementation
// ============================================================================

EventProcessor::EventProcessor(std::shared_ptr<EventQueue> queue, std::shared_ptr<HttpRequest> http)
    : queue_(queue), http_(http) {}

EventProcessor::~EventProcessor() {
    Stop();
}

void EventProcessor::Start() {
    if (running_.load()) {
        return;
    }
    running_.store(true);
    thread_ = std::make_unique<std::thread>(&EventProcessor::ProcessLoop, this);
}

void EventProcessor::Stop() {
    if (!running_.load()) {
        return;
    }
    running_.store(false);
    queue_->Stop();
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

void EventProcessor::ProcessLoop() {
    while (running_.load()) {
        PlayerEvent event;
        
        // Use the public Pop() method which handles waiting
        event = queue_->Pop();
        
        // Check if we should stop
        if (queue_->IsStopped() && event.steamId.empty()) {
            break;
        }
        
        if (!event.steamId.empty()) {
            HttpRequest::Response response = http_->SendPlayerEvent(
                event.steamId, 
                event.isJoin
            );
            
            if (response.statusCode != 200 && response.statusCode != 201) {
                if (!response.error.empty()) {
                    fprintf(stderr, "Failed to send event: %s\n", response.error.c_str());
                }
            }
        }
    }
}
