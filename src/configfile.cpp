#include "configfile.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

// Trim whitespace from both ends
static std::string Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// Remove comments from a line (// and # style)
static std::string RemoveComments(const std::string& line) {
    size_t commentPos = line.find("//");
    if (commentPos != std::string::npos) {
        return line.substr(0, commentPos);
    }
    commentPos = line.find('#');
    if (commentPos != std::string::npos) {
        return line.substr(0, commentPos);
    }
    return line;
}

// Parse INI-style config (key = value)
static bool ParseIniLine(const std::string& line, std::string& key, std::string& value) {
    std::string trimmed = Trim(RemoveComments(line));
    if (trimmed.empty()) return false;
    
    size_t delimiterPos = trimmed.find('=');
    if (delimiterPos == std::string::npos) return false;
    
    key = Trim(trimmed.substr(0, delimiterPos));
    value = Trim(trimmed.substr(delimiterPos + 1));
    
    // Remove surrounding quotes if present
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
    }
    if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
        value = value.substr(1, value.size() - 2);
    }
    
    return !key.empty();
}

// Parse simple JSON (very basic parser, just for our needs)
static bool ParseJsonLine(const std::string& line, std::string& key, std::string& value) {
    std::string trimmed = Trim(RemoveComments(line));
    if (trimmed.empty()) return false;
    
    // Simple pattern: "key": "value"
    size_t colonPos = trimmed.find(':');
    if (colonPos == std::string::npos) return false;
    
    key = Trim(trimmed.substr(0, colonPos));
    value = Trim(trimmed.substr(colonPos + 1));
    
    // Remove quotes from key
    if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
        key = key.substr(1, key.size() - 2);
    }
    
    // Remove quotes from value
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
    }
    
    // Remove trailing comma
    if (!value.empty() && value.back() == ',') {
        value.pop_back();
    }
    
    return !key.empty();
}

bool ConfigFile::LoadFromIniFile(const std::string& path, std::string& apiToken, std::string& endpoint) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::string key, value;
        if (ParseIniLine(line, key, value)) {
            if (key == "apiToken" || key == "token" || key == "BOT_API_TOKEN") {
                apiToken = value;
            } else if (key == "endpoint" || key == "url" || key == "apiUrl" || key == "BOT_API_ENDPOINT") {
                endpoint = value;
            }
        }
    }
    
    return !apiToken.empty();
}

bool ConfigFile::LoadFromJsonFile(const std::string& path, std::string& apiToken, std::string& endpoint) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::string key, value;
        if (ParseJsonLine(line, key, value)) {
            if (key == "apiToken" || key == "token") {
                apiToken = value;
            } else if (key == "endpoint" || key == "url" || key == "apiUrl") {
                endpoint = value;
            }
        }
    }
    
    return !apiToken.empty();
}

bool ConfigFile::LoadFromFile(const std::string& path, std::string& apiToken, std::string& endpoint) {
    // Try INI format first
    if (LoadFromIniFile(path, apiToken, endpoint)) {
        return true;
    }
    
    // Reset and try JSON
    apiToken.clear();
    endpoint.clear();
    if (LoadFromJsonFile(path, apiToken, endpoint)) {
        return true;
    }
    
    return false;
}

std::string ConfigFile::FindConfigPath() {
    // Try multiple possible locations
    std::vector<std::string> paths = {
        // In the mod's addons directory
        "@attendance_bot/addons/attendance_bot/config.hpp",
        "@attendance_bot/config.hpp",
        // In the mod root
        "@attendance_bot/config/attendance_bot.hpp",
        "@attendance_bot/config/attendance_bot.json",
        // In the config directory
        "@attendance_bot/config.hpp",
        // Current directory
        "./config.hpp",
        "./attendance_bot_config.hpp",
        "./config/attendance_bot.hpp",
        // Absolute path for Linux server
        "/home/arma/arma3/@attendance_bot/config.hpp",
    };
    
    for (const auto& path : paths) {
        std::ifstream test(path);
        if (test.good()) {
            test.close();
            return path;
        }
    }
    
    return "";
}

bool ConfigFile::TryLoadConfig(std::string& apiToken, std::string& endpoint) {
    std::string configPath = FindConfigPath();
    if (!configPath.empty()) {
        return LoadFromFile(configPath, apiToken, endpoint);
    }
    return false;
}
