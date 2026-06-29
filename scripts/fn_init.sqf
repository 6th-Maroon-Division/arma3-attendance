/*
 * Arma 3 Attendance Bot - Player Event Tracker
 * This is the pre-init function that runs before mission start
 */

// Configure the extension with environment variables
// This will be called automatically by Arma 3

// Wait for extension to be available
waitUntil {!(callExtension "version" == "")};

// Extension is loaded, now set up event handlers
["PlayerConnected", {
    private ["_player", "_steamID", "_discordID"];
    _player = _this select 0;
    _steamID = getPlayerUID _player;
    _discordID = _player getVariable ["discordUserId", ""];
    callExtension format ["playerConnected %1 %2", _steamID, _discordID];
}] call addMissionEventHandler;

["PlayerDisconnected", {
    private ["_player", "_steamID", "_discordID"];
    _player = _this select 0;
    _steamID = getPlayerUID _player;
    _discordID = _player getVariable ["discordUserId", ""];
    callExtension format ["playerDisconnected %1 %2", _steamID, _discordID];
}] call addMissionEventHandler;

// Handle already connected players
{
    if (isPlayer _x && !isNull _x && alive _x) then {
        private ["_steamID", "_discordID"];
        _steamID = getPlayerUID _x;
        _discordID = _x getVariable ["discordUserId", ""];
        callExtension format ["playerConnected %1 %2", _steamID, _discordID];
    };
} forEach playableUnits;
