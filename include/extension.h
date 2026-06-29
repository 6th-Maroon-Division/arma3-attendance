#pragma once

#include <string>
#include <vector>
#include <memory>

// Arma 3 Extension API
#ifdef _WINDOWS
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((dllexport))
#endif

// Main extension class
class Arma3Extension {
public:
    static void Initialize();
    static void Shutdown();
    static void HandleCommand(char* output, int outputSize, const char* function);
    
    // Player event handlers
    static void OnPlayerConnected(const std::string& steamId);
    static void OnPlayerDisconnected(const std::string& steamId);
    
private:
    static std::string LastError;
};

// Export the required Arma 3 extension functions
extern "C" {
    EXPORT void RVExtensionVersion(char* output, int outputSize);
    EXPORT void RVExtension(char* output, int outputSize, const char* function);
}
