#pragma once

#include <string>

class Config {
public:
    Config();
    
    std::string GetApiToken() const;
    std::string GetEndpoint() const;
    
    void SetApiToken(const std::string& token);
    void SetEndpoint(const std::string& endpoint);
    
    bool LoadFromEnvironment();
    bool LoadFromFile(const std::string& path);
    
private:
    std::string apiToken_;
    std::string endpoint_;
};
