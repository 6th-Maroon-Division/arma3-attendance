#include "config.h"

#include <cstdlib> // for getenv
#include <fstream>
#include <sstream>

Config::Config() : apiToken_(""), endpoint_("") {}

std::string Config::GetApiToken() const {
    return apiToken_;
}

std::string Config::GetEndpoint() const {
    return endpoint_;
}

void Config::SetApiToken(const std::string& token) {
    apiToken_ = token;
}

void Config::SetEndpoint(const std::string& endpoint) {
    endpoint_ = endpoint;
}

bool Config::LoadFromEnvironment() {
    // Try to load from environment variables
    // These will be set by GitHub Actions secrets
    const char* tokenEnv = std::getenv("BOT_API_TOKEN");
    const char* endpointEnv = std::getenv("BOT_API_ENDPOINT");
    
    if (tokenEnv) {
        apiToken_ = tokenEnv;
    }
    
    if (endpointEnv) {
        endpoint_ = endpointEnv;
    }
    
    // Return true if at least the token was loaded
    return !apiToken_.empty();
}

bool Config::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple key=value format
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            
            if (key == "apiToken") {
                apiToken_ = value;
            } else if (key == "endpoint") {
                endpoint_ = value;
            }
        }
    }
    
    return !apiToken_.empty();
}
