#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <memory>

// Forward declarations
class HttpRequest;

// Event data structure - Steam ID only
struct PlayerEvent {
    std::string steamId;
    bool isJoin;
    std::string timestamp;
};

// Thread-safe queue for player events
class EventQueue {
public:
    EventQueue();
    ~EventQueue();
    
    // Add an event to the queue
    void Push(const PlayerEvent& event);
    
    // Get the next event (blocks if queue is empty)
    PlayerEvent Pop();
    
    // Try to get the next event (non-blocking)
    bool TryPop(PlayerEvent& event);
    
    // Check if queue is empty
    bool Empty() const;
    
    // Stop the processing thread
    void Stop();
    
    // Check if stopped
    bool IsStopped() const;
    
    // For EventProcessor: Check if empty and not stopped
    bool HasWork() const;
    
private:
    friend class EventProcessor;  // Allow EventProcessor to access private members
    
    std::queue<PlayerEvent> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condVar_;
    std::atomic<bool> stopped_{false};
};

// Worker thread for processing events
class EventProcessor {
public:
    EventProcessor(std::shared_ptr<EventQueue> queue, std::shared_ptr<HttpRequest> http);
    ~EventProcessor();
    
    void Start();
    void Stop();
    
private:
    void ProcessLoop();
    
    std::shared_ptr<EventQueue> queue_;
    std::shared_ptr<HttpRequest> http_;
    std::unique_ptr<std::thread> thread_;
    std::atomic<bool> running_{false};
};
