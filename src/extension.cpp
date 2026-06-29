#include "extension.h"
#include "httprequest.h"
#include "config.h"
#include "utils.h"
#include "queue.h"

#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <cstdio>

// Static members
std::string Arma3Extension::LastError = "";

// Global instances (initialized once)
static Config g_Config;
static std::shared_ptr<EventQueue> g_EventQueue = nullptr;
static std::shared_ptr<HttpRequest> g_HttpRequest = nullptr;
static std::shared_ptr<EventProcessor> g_EventProcessor = nullptr;

// ---------------------------------------------------------------------------
// Configuration helpers
// ---------------------------------------------------------------------------

static void EnsureInfrastructure() {
    if (!g_HttpRequest) {
        // Try to load config from environment variables first
        // This is how GitHub secrets will be injected
        g_Config.LoadFromEnvironment();
        
        // If endpoint is still empty, use default
        if (g_Config.GetEndpoint().empty()) {
            g_Config.SetEndpoint("http://localhost:3000/api/bot/events");
        }
        
        g_HttpRequest = std::make_shared<HttpRequest>(
            g_Config.GetApiToken(),
            g_Config.GetEndpoint()
        );
        
        g_EventQueue = std::make_shared<EventQueue>();
        g_EventProcessor = std::make_shared<EventProcessor>(g_EventQueue, g_HttpRequest);
        g_EventProcessor->Start();
    }
}

static void ShutdownInfrastructure() {
    if (g_EventProcessor) {
        g_EventProcessor->Stop();
        g_EventProcessor.reset();
    }
    if (g_EventQueue) {
        g_EventQueue->Stop();
        g_EventQueue.reset();
    }
    g_HttpRequest.reset();
}

// ---------------------------------------------------------------------------
// Arma 3 Extension API Implementation
// ---------------------------------------------------------------------------

extern "C" {
    EXPORT void RVExtensionVersion(char* output, int outputSize) {
        const char* version = "1.0.0";
        strncpy(output, version, outputSize);
        output[outputSize - 1] = '\0';
    }
    
    EXPORT void RVExtension(char* output, int outputSize, const char* function) {
        Arma3Extension::HandleCommand(output, outputSize, function);
    }
}

// ---------------------------------------------------------------------------
// Arma3Extension class implementation
// ---------------------------------------------------------------------------

void Arma3Extension::Initialize() {
    EnsureInfrastructure();
}

void Arma3Extension::Shutdown() {
    ShutdownInfrastructure();
}

void Arma3Extension::HandleCommand(char* output, int outputSize, const char* function) {
    if (!function || strlen(function) == 0) {
        strncpy(output, "Error: No function specified", outputSize);
        output[outputSize - 1] = '\0';
        return;
    }
    
    // Parse the function call
    // Format: "functionName arg1 arg2 arg3 ..."
    std::string funcStr(function);
    std::vector<std::string> parts = Utils::SplitString(funcStr, ' ');
    
    if (parts.empty()) {
        strncpy(output, "Error: Invalid function call", outputSize);
        output[outputSize - 1] = '\0';
        return;
    }
    
    std::string command = parts[0];
    
    // Initialize the infrastructure if not already done
    EnsureInfrastructure();
    
    if (command == "playerConnected") {
        // Format: playerConnected <steamId> <discordUserId>
        if (parts.size() >= 2) {
            std::string steamId = parts[1];
            std::string discordUserId = (parts.size() >= 3) ? parts[2] : "";
            
            // Queue the event and return immediately
            PlayerEvent event;
            event.steamId = steamId;
            event.discordUserId = discordUserId;
            event.isJoin = true;
            event.timestamp = Utils::GetCurrentUTCTime();
            
            g_EventQueue->Push(event);
            
            strncpy(output, "OK", outputSize);
        } else {
            strncpy(output, "Error: playerConnected requires steamId", outputSize);
        }
    }
    else if (command == "playerDisconnected") {
        // Format: playerDisconnected <steamId> <discordUserId>
        if (parts.size() >= 2) {
            std::string steamId = parts[1];
            std::string discordUserId = (parts.size() >= 3) ? parts[2] : "";
            
            // Queue the event and return immediately
            PlayerEvent event;
            event.steamId = steamId;
            event.discordUserId = discordUserId;
            event.isJoin = false;
            event.timestamp = Utils::GetCurrentUTCTime();
            
            g_EventQueue->Push(event);
            
            strncpy(output, "OK", outputSize);
        } else {
            strncpy(output, "Error: playerDisconnected requires steamId", outputSize);
        }
    }
    else if (command == "version") {
        strncpy(output, "1.0.0", outputSize);
    }
    else if (command == "setToken") {
        if (parts.size() >= 2) {
            g_Config.SetApiToken(parts[1]);
            g_HttpRequest = std::make_shared<HttpRequest>(
                g_Config.GetApiToken(),
                g_Config.GetEndpoint()
            );
            strncpy(output, "OK", outputSize);
        } else {
            strncpy(output, "Error: setToken requires token", outputSize);
        }
    }
    else if (command == "setEndpoint") {
        if (parts.size() >= 2) {
            g_Config.SetEndpoint(parts[1]);
            g_HttpRequest = std::make_shared<HttpRequest>(
                g_Config.GetApiToken(),
                g_Config.GetEndpoint()
            );
            strncpy(output, "OK", outputSize);
        } else {
            strncpy(output, "Error: setEndpoint requires URL", outputSize);
        }
    }
    else if (command == "queueStatus") {
        if (g_EventQueue) {
            int count = g_EventQueue->Empty() ? 0 : 1; // Simple check
            snprintf(output, outputSize, "Queue: %d events", count);
        } else {
            strncpy(output, "Queue not initialized", outputSize);
        }
    }
    else {
        strncpy(output, ("Error: Unknown command: " + command).c_str(), outputSize);
    }
    
    output[outputSize - 1] = '\0';
}

void Arma3Extension::OnPlayerConnected(const std::string& steamId, const std::string& discordUserId) {
    if (!g_EventQueue) {
        LastError = "Event queue not initialized";
        return;
    }
    
    PlayerEvent event;
    event.steamId = steamId;
    event.discordUserId = discordUserId;
    event.isJoin = true;
    event.timestamp = Utils::GetCurrentUTCTime();
    
    g_EventQueue->Push(event);
}

void Arma3Extension::OnPlayerDisconnected(const std::string& steamId, const std::string& discordUserId) {
    if (!g_EventQueue) {
        LastError = "Event queue not initialized";
        return;
    }
    
    PlayerEvent event;
    event.steamId = steamId;
    event.discordUserId = discordUserId;
    event.isJoin = false;
    event.timestamp = Utils::GetCurrentUTCTime();
    
    g_EventQueue->Push(event);
}
