/*
 * Arma 3 Attendance Bot - Player Event Tracker
 * 
 * This script sets up event handlers to detect when players join and leave
 * the server, and sends events to the C++ extension which forwards them
 * to the bot API.
 * 
 * Usage:
 * 1. Place this in your mission's init.sqf or call it from there
 * 2. Ensure the extension DLL/SO is loaded
 * 3. Configure the API token and endpoint
 */

// ============================================================================
// Configuration
// ============================================================================

// API Configuration - these can also be set via the extension commands
// If you set them here, they'll be used before the extension loads from env vars
ATTENDANCE_BOT_API_TOKEN = "";      // Will be loaded from BOT_API_TOKEN env var
ATTENDANCE_BOT_ENDPOINT = "";       // Will be loaded from BOT_API_ENDPOINT env var

// ============================================================================
// Extension Communication
// ============================================================================

// Helper function to call the extension
attendance_bot_callExtension = {
    private ["_fnc_callExtension", "_result", "_cmd"];
    _cmd = format ["%1 %2", _this select 0, _this select 1];
    _result = callExtension _cmd;
    _result
};

// Configure the extension with API token and endpoint
attendance_bot_configure = {
    if (ATTENDANCE_BOT_API_TOKEN != "") then {
        ["setToken", ATTENDANCE_BOT_API_TOKEN] call attendance_bot_callExtension;
    };
    
    if (ATTENDANCE_BOT_ENDPOINT != "") then {
        ["setEndpoint", ATTENDANCE_BOT_ENDPOINT] call attendance_bot_callExtension;
    };
};

// ============================================================================
// Player Event Handlers
// ============================================================================

// Get player's Steam ID
attendance_bot_getSteamID = {
    private ["_player", "_steamID"];
    _player = _this select 0;
    _steamID = getPlayerUID _player;
    
    // Arma 3 returns Steam ID as a string like "76561198000000000"
    // This is the 64-bit Steam ID format
    if (_steamID == "") then {
        // Try alternative method
        _steamID = profileName _player;
    };
    _steamID
};

// Get player's Discord ID (if available via Discord integration mod)
attendance_bot_getDiscordID = {
    private ["_player", "_discordID"];
    _player = _this select 0;
    _discordID = "";
    
    // Check if Discord integration is available
    // This depends on the Discord mod you're using
    // For example, with Dwarden's Discord Integration:
    if (isNil "BIS_Discord_loaded") then {
        // Try to get Discord ID from variable
        _discordID = _player getVariable ["discordUserId", ""];
    };
    
    _discordID
};

// Handle player connected
attendance_bot_onPlayerConnected = {
    private ["_player", "_steamID", "_discordID", "_result"];
    _player = _this select 0;
    
    // Get identifiers
    _steamID = [_player] call attendance_bot_getSteamID;
    _discordID = [_player] call attendance_bot_getDiscordID;
    
    // Send to extension
    _result = ["playerConnected", _steamID, _discordID] call attendance_bot_callExtension;
    
    // Log for debugging
    // systemChat format ["Player connected: %1 (Steam: %2, Discord: %3)", name _player, _steamID, _discordID];
    
    _result
};

// Handle player disconnected
attendance_bot_onPlayerDisconnected = {
    private ["_player", "_steamID", "_discordID", "_result"];
    _player = _this select 0;
    
    // Get identifiers
    _steamID = [_player] call attendance_bot_getSteamID;
    _discordID = [_player] call attendance_bot_getDiscordID;
    
    // Send to extension
    _result = ["playerDisconnected", _steamID, _discordID] call attendance_bot_callExtension;
    
    // Log for debugging
    // systemChat format ["Player disconnected: %1 (Steam: %2, Discord: %3)", name _player, _steamID, _discordID];
    
    _result
};

// ============================================================================
// Initialize the tracking system
// ============================================================================

// Wait for the extension to load
waitUntil {!
    (callExtension "version" == "") 
    && {(callExtension "version") != "Error: No function specified"}
};

// Configure the extension
[] call attendance_bot_configure;

// Set up mission event handlers
["PlayerConnected", {_this call attendance_bot_onPlayerConnected}] call addMissionEventHandler;
["PlayerDisconnected", {_this call attendance_bot_onPlayerDisconnected}] call addMissionEventHandler;

// Also handle existing players at mission start
{
    if (isPlayer _x && !isNull _x && alive _x) then {
        [_x] call attendance_bot_onPlayerConnected;
    };
} forEach playableUnits;

// Log initialization
systemChat "Attendance Bot initialized - player tracking active";
