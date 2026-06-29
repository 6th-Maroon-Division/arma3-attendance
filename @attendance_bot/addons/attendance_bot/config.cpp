// Arma 3 Attendance Bot - PBO Configuration
// This config is packed into the PBO

class CfgPatches
{
    class attendance_bot
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.00;
        requiredAddons[] = {};
    };
};

// Declare the C++ extension
// The extension file (attendance_bot.so/.dll) is in the mod root folder
class CfgExtensions
{
    class attendance_bot
    {
        extension = "attendance_bot";
    };
};

// Auto-execute init script from PBO
class CfgFunctions
{
    class attendance_bot
    {
        class init
        {
            file = "scripts\init.sqf";
            preInit = 1;
            postInit = 1;
        };
    };
};
