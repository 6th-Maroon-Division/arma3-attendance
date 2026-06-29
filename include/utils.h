#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace Utils {
    // String utilities
    std::vector<std::string> SplitString(const std::string& str, char delimiter);
    std::string JoinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    std::string Trim(const std::string& str);
    
    // JSON escape utilities
    std::string JsonEscape(const std::string& str);
    
    // Build JSON string for player event
    std::string BuildPlayerEventJson(const std::string& steamId, const std::string& discordUserId, bool isJoin);
    
    // Time utilities - ISO 8601 timestamp
    std::string GetCurrentUTCTime();
}
