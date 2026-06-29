#include "extension.h"
#include "httprequest.h"
#include "config.h"
#include "utils.h"

#include <cstring>
#include <string>
#include <vector>
#include <memory>

// Static members
std::string Arma3Extension::LastError = "";

// Global instances (initialized once)
static Config g_Config;
static std::unique_ptr<HttpRequest> g_HttpRequest = nullptr;

// ---------------------------------------------------------------------------
// Configuration helpers
// ---------------------------------------------------------------------------

static void EnsureHttpRequest() {
    if (!g_HttpRequest) {
        // Try to load config from environment variables first
        // This is how GitHub secrets will be injected
        g_Config.LoadFromEnvironment();
        
        // If endpoint is still empty, use default
        if (g_Config.GetEndpoint().empty()) {
            g_Config.SetEndpoint("http://localhost:3000/api/bot/events");
        }
        
        g_HttpRequest = std::make_unique<HttpRequest>(
            g_Config.GetApiToken(),
            g_Config.GetEndpoint()
        );
    }
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
    // Initialize the HTTP request handler
    EnsureHttpRequest();
}

void Arma3Extension::Shutdown() {
    g_HttpRequest.reset();
}

void Arma3Extension::HandleCommand(char* output, int outputSize, const char* function) {
    if (!function || strlen(function) == 0) {
        strncpy(output, "Error: No function specified", outputSize);
        output[outputSize - 1] = '\0';
        return;
    }
    
    // Parse the function call
    // Format: "functionName arg1 arg2 arg3 ..."
    // Arma 3 sends the function name and arguments as a single string
    std::string funcStr(function);
    std::vector<std::string> parts = Utils::SplitString(funcStr, ' ');
    
    if (parts.empty()) {
        strncpy(output, "Error: Invalid function call", outputSize);
        output[outputSize - 1] = '\0';
        return;
    }
    
    std::string command = parts[0];
    
    // Initialize the HTTP client if not already done
    EnsureHttpRequest();
    
    if (command == "playerConnected") {
        // Format: playerConnected <steamId> <discordUserId>
        if (parts.size() >= 2) {
            std::string steamId = parts[1];
            std::string discordUserId = (parts.size() >= 3) ? parts[2] : "";
            OnPlayerConnected(steamId, discordUserId);
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
            OnPlayerDisconnected(steamId, discordUserId);
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
            g_HttpRequest = std::make_unique<HttpRequest>(
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
            g_HttpRequest = std::make_unique<HttpRequest>(
                g_Config.GetApiToken(),
                g_Config.GetEndpoint()
            );
            strncpy(output, "OK", outputSize);
        } else {
            strncpy(output, "Error: setEndpoint requires URL", outputSize);
        }
    }
    else {
        strncpy(output, ("Error: Unknown command: " + command).c_str(), outputSize);
    }
    
    output[outputSize - 1] = '\0';
}

void Arma3Extension::OnPlayerConnected(const std::string& steamId, const std::string& discordUserId) {
    if (!g_HttpRequest) {
        LastError = "HTTP request handler not initialized";
        return;
    }
    
    HttpRequest::Response response = g_HttpRequest->SendPlayerEvent(steamId, discordUserId, true);
    
    if (response.statusCode != 200 && response.statusCode != 201) {
        LastError = "Failed to send player connected event: " + response.error;
    }
}

void Arma3Extension::OnPlayerDisconnected(const std::string& steamId, const std::string& discordUserId) {
    if (!g_HttpRequest) {
        LastError = "HTTP request handler not initialized";
        return;
    }
    
    HttpRequest::Response response = g_HttpRequest->SendPlayerEvent(steamId, discordUserId, false);
    
    if (response.statusCode != 200 && response.statusCode != 201) {
        LastError = "Failed to send player disconnected event: " + response.error;
    }
}
