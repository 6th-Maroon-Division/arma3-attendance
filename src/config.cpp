#include "config.h"
#include "configfile.h"

#include <cstdlib> // for getenv

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

bool Config::Load() {
    // Try config file first
    std::string fileToken, fileEndpoint;
    if (ConfigFile::TryLoadConfig(fileToken, fileEndpoint)) {
        apiToken_ = fileToken;
        endpoint_ = fileEndpoint;
        return !apiToken_.empty();
    }
    
    // Fall back to environment variables
    return LoadFromEnvironment();
}
