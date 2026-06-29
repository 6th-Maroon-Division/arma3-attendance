#pragma once

#include <string>

class Config {
public:
    Config();
    
    std::string GetApiToken() const;
    std::string GetEndpoint() const;
    
    void SetApiToken(const std::string& token);
    void SetEndpoint(const std::string& endpoint);
    
    // Load configuration from all sources (file > env vars)
    bool Load();
    
    // Load from environment variables only
    bool LoadFromEnvironment();
    
private:
    std::string apiToken_;
    std::string endpoint_;
};
