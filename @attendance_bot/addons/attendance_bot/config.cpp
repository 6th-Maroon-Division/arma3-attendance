// Arma 3 Extension Configuration
// This config declares the C++ extension to Arma 3 and sets up auto-execution

class CfgPatches
{
    class attendance_bot_extension
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.00;
        requiredAddons[] = {};
        author = "Your Name";
        name = "Attendance Bot Extension";
        
        // Version info
        version = "1.0.0";
        versionStr = "1.0.0";
        versionAr[] = {1,0,0};
    };
};

// Declare the C++ extension - Arma 3 will look for attendance_bot.so or attendance_bot.dll
class CfgExtensions
{
    class attendance_bot
    {
        // This should match the filename of your .so/.dll
        // On Linux: attendance_bot.so
        // On Windows: attendance_bot.dll
        extension = "attendance_bot";
        
        // Optional: full path (usually not needed if in same directory as config.cpp)
        // extension = "\@attendance_bot\addons\attendance_bot\attendance_bot";
    };
};

// Auto-execute our init script when the mod loads
// This runs automatically without requiring mission changes
class CfgFunctions
{
    class attendance_bot
    {
        class init
        {
            // This file will be automatically executed when the mod loads
            // preInit = 1 means it runs before mission starts
            file = "\@attendance_bot\addons\attendance_bot\init.sqf";
            preInit = 1;
            
            // Optional: also run during mission (for JIP players)
            postInit = 1;
        };
    };
};

// Mod metadata
class CfgMods
{
    class Mod_Base;
    class attendance_bot: Mod_Base
    {
        dir = "@attendance_bot";
        name = "Attendance Bot Extension";
        picture = "\@attendance_bot\logo.paa";
        logo = "\@attendance_bot\logo.paa";
        logoOver = "\@attendance_bot\logo.paa";
        logoSmall = "\@attendance_bot\logo.paa";
        tooltip = "Automatically tracks player join/leave events";
        tooltipOwned = "Player join/leave event tracker - no mission changes required";
        author = "Your Name";
        action = "";
        hideName = 0;
        hidePicture = 0;
    };
};
