#pragma once

#include <string>
#include <map>

class HttpRequest {
public:
    struct Response {
        int statusCode;
        std::string body;
        std::string error;
    };
    
    HttpRequest(const std::string& apiToken, const std::string& endpoint);
    
    // Send player join/leave event to the API
    Response SendPlayerEvent(const std::string& steamId, const std::string& discordUserId, bool isJoin);
    
private:
    std::string apiToken_;
    std::string endpoint_;
    
    // Internal helper to send POST request
    Response PostRequest(const std::string& url, const std::string& jsonBody, 
                         const std::map<std::string, std::string>& headers);
    
    // Generate ISO 8601 timestamp
    std::string GetCurrentTimestamp();
};
