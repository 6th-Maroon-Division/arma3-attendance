/*
 * Attendance Bot - Auto-initialization Script
 * 
 * This script runs automatically when the mod loads, setting up
 * player join/leave event tracking without requiring mission changes.
 * 
 * This is executed via CfgFunctions in the mod's config.cpp
 */

// ============================================================================
// Wait for extension to be loaded
// ============================================================================

waitUntil {
    !isNil "BIS_fnc_init" && 
    (callExtension "version" != "") && 
    (callExtension "version" != "Error: No function specified")
};

// Give it a bit more time just to be safe
uiSleep 0.5;

// ============================================================================
// Extension Communication Helper
// ============================================================================

attendance_bot_call = {
    private ["_cmd", "_result"];
    _cmd = _this;
    if (isNil "_cmd") exitWith {"Error: No command"};
    if (typeName _cmd != typeName []) then {
        _cmd = [_cmd];
    };
    _cmd = _cmd joinString " ";
    _result = callExtension _cmd;
    _result
};

// ============================================================================
// Player ID Extraction (Steam ID only)
// ============================================================================

attendance_bot_getSteamID = {
    private ["_player", "_uid"];
    _player = _this;
    if (isNull _player) exitWith { "" };
    _uid = getPlayerUID _player;
    if (_uid == "" || _uid == "0") then {
        _uid = profileNameSpace getVariable ["BIS_PlayerID", ""];
    };
    _uid
};

// ============================================================================
// Event Handlers (Steam ID only)
// ============================================================================

attendance_bot_onPlayerConnected = {
    private ["_player", "_steamID"];
    _player = _this select 0;
    
    _steamID = [_player] call attendance_bot_getSteamID;
    
    // Send to extension - returns immediately, queued internally
    ["playerConnected", _steamID] call attendance_bot_call;
};

attendance_bot_onPlayerDisconnected = {
    private ["_player", "_steamID"];
    _player = _this select 0;
    
    _steamID = [_player] call attendance_bot_getSteamID;
    
    // Send to extension - returns immediately, queued internally
    ["playerDisconnected", _steamID] call attendance_bot_call;
};

// ============================================================================
// Setup Event Handlers
// ============================================================================

// Register mission event handlers
["PlayerConnected", attendance_bot_onPlayerConnected] call addMissionEventHandler;
["PlayerDisconnected", attendance_bot_onPlayerDisconnected] call addMissionEventHandler;

// Also handle existing players at mission start
{
    if (isPlayer _x && !isNull _x && alive _x) then {
        [_x] call attendance_bot_onPlayerConnected;
    };
} forEach playableUnits;

// Log to RPT file (not visible to players)
// diag_log "Attendance Bot: Player tracking initialized";
