/*
 * Configuration script for the Attendance Bot extension
 * 
 * This can be used to dynamically configure the extension at runtime
 */

// Set the API token
attendance_bot_setToken = {
    private ["_token"];
    _token = _this select 0;
    ["setToken", _token] call attendance_bot_callExtension
};

// Set the API endpoint
attendance_bot_setEndpoint = {
    private ["_endpoint"];
    _endpoint = _this select 0;
    ["setEndpoint", _endpoint] call attendance_bot_callExtension
};

// Test the connection
attendance_bot_testConnection = {
    private ["_result"];
    _result = callExtension "version";
    if (_result == "1.0.0") then {
        systemChat "Attendance Bot extension is working correctly";
        true
    } else {
        systemChat format ["Attendance Bot extension error: %1", _result];
        false
    }
};
