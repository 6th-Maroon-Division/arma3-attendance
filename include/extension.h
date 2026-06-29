#pragma once

#include <string>
#include <vector>
#include <memory>

// Arma 3 Extension API
// For Linux: shared object (.so) with these exported functions
// For Windows: DLL with these exported functions

#ifdef _WINDOWS
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// Main extension class
class Arma3Extension {
public:
    static void Initialize();
    static void Shutdown();
    static void HandleCommand(char* output, int outputSize, const char* function);
    
    // Player event handlers - called from SQF
    static void OnPlayerConnected(const std::string& steamId, const std::string& discordUserId);
    static void OnPlayerDisconnected(const std::string& steamId, const std::string& discordUserId);
    
private:
    static std::string LastError;
};

// Export the required Arma 3 extension functions
extern "C" {
    // Required: Called by Arma 3 to get extension version (since v1.70)
    EXPORT void RVExtensionVersion(char* output, int outputSize);
    
    // Required: Main extension function called by Arma 3
    // Signature: void RVExtension(char* output, int outputSize, const char* function)
    EXPORT void RVExtension(char* output, int outputSize, const char* function);
}
