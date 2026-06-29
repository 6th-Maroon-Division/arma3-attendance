#pragma once

#include <string>
#include <map>
#include <vector>

class ConfigFile {
public:
    // Config file path - will look in multiple locations
    static std::string FindConfigPath();
    
    // Load config from file
    static bool LoadFromFile(const std::string& path, std::string& apiToken, std::string& endpoint);
    
    // Load config from JSON file
    static bool LoadFromJsonFile(const std::string& path, std::string& apiToken, std::string& endpoint);
    
    // Load config from INI-style file (key = value)
    static bool LoadFromIniFile(const std::string& path, std::string& apiToken, std::string& endpoint);
    
    // Try all supported formats
    static bool TryLoadConfig(std::string& apiToken, std::string& endpoint);
    
private:
    // Get the mod directory path
    static std::string GetModDirectory();
};
