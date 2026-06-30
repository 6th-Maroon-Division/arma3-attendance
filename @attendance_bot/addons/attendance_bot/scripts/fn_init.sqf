/*
 * Attendance Bot - Auto-initialization Script
 * 
 * This script runs automatically when the PBO loads via CfgFunctions.
 * It registers event handlers for player join/leave events.
 */

// Wait for extension and BIS functions to be available
waitUntil {
    !isNil "BIS_fnc_init" && 
    ("attendance_bot" callExtension "version" != "") && 
    ("attendance_bot" callExtension "version" != "Error: No function specified")
};

uiSleep 0.5;

// Helper to call extension
attendance_bot_call = {
    private ["_cmd", "_result"];
    _cmd = _this;
    if (isNil "_cmd") exitWith {"Error: No command"};
    if (typeName _cmd != typeName []) then {
        _cmd = [_cmd];
    };
    _cmd = _cmd joinString " ";
    _result = "attendance_bot" callExtension _cmd;
    _result
};

// Get player's Steam ID
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

// Event handlers
attendance_bot_onPlayerConnected = {
    private ["_player", "_steamID"];
    _player = _this select 0;
    _steamID = [_player] call attendance_bot_getSteamID;
    ["playerConnected", _steamID] call attendance_bot_call;
};

attendance_bot_onPlayerDisconnected = {
    private ["_player", "_steamID"];
    _player = _this select 0;
    _steamID = [_player] call attendance_bot_getSteamID;
    ["playerDisconnected", _steamID] call attendance_bot_call;
};

// Register event handlers
["PlayerConnected", attendance_bot_onPlayerConnected] call addMissionEventHandler;
["PlayerDisconnected", attendance_bot_onPlayerDisconnected] call addMissionEventHandler;

// Handle existing players
{
    if (isPlayer _x && !isNull _x && alive _x) then {
        [_x] call attendance_bot_onPlayerConnected;
    };
} forEach playableUnits;
