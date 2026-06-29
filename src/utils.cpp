#include "utils.h"

#include <cctype>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Utils {
    std::vector<std::string> SplitString(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        
        while (std::getline(tokenStream, token, delimiter)) {
            token = Trim(token);
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        
        return tokens;
    }
    
    std::string JoinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
        if (strings.empty()) {
            return "";
        }
        
        std::ostringstream oss;
        for (size_t i = 0; i < strings.size(); ++i) {
            if (i > 0) {
                oss << delimiter;
            }
            oss << strings[i];
        }
        
        return oss.str();
    }
    
    std::string Trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            return "";
        }
        
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
    
    std::string JsonEscape(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (c >= 0 && c < 32) {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        oss << c;
                    }
                    break;
            }
        }
        return oss.str();
    }
    
    std::string BuildPlayerEventJson(
        const std::string& steamId, 
        const std::string& discordUserId, 
        bool isJoin
    ) {
        std::string timestamp = GetCurrentUTCTime();
        
        // Prefer steamId if available, otherwise use discordUserId
        if (!steamId.empty()) {
            return "{\"steamId\":\"" + JsonEscape(steamId) + 
                   "\",\"isJoin\":" + (isJoin ? "true" : "false") +
                   "\",\"timestamp\":\"" + timestamp + "\"}";
        } else if (!discordUserId.empty()) {
            return "{\"discordUserId\":\"" + JsonEscape(discordUserId) + 
                   "\",\"isJoin\":" + (isJoin ? "true" : "false") +
                   "\",\"timestamp\":\"" + timestamp + "\"}";
        }
        
        // If neither is available, this is an error case
        // But we still return valid JSON
        return "{\"isJoin\":" + (isJoin ? "true" : "false") +
               "\",\"timestamp\":\"" + timestamp + "\"}";
    }
    
    std::string GetCurrentUTCTime() {
        // Get current time in UTC
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        
        std::tm tm_utc;
        #ifdef _WINDOWS
            gmtime_s(&tm_utc, &now_time);
        #else
            gmtime_r(&now_time, &tm_utc);
        #endif
        
        // Format as ISO 8601: YYYY-MM-DDTHH:MM:SSZ
        std::ostringstream oss;
        oss << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
        
        return oss.str();
    }
}
