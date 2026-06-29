// Arma 3 Extension Configuration
// This config declares the C++ extension to Arma 3

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
    };
};

// Declare the extension - Arma 3 will look for attendance_bot.so or attendance_bot.dll
// in the same directory as this config.cpp
class CfgExtensions
{
    // The name here is used in callExtension commands
    // The engine looks for a file named after this class
    class attendance_bot
    {
        // This should match the filename of your .so/.dll
        // On Linux: attendance_bot.so
        // On Windows: attendance_bot.dll
        extension = "attendance_bot";
    };
};

// Required for the extension to be loaded as an addon
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
        tooltip = "Player join/leave event tracker";
        tooltipOwned = "Player join/leave event tracker";
        author = "Your Name";
        action = "";
        hideName = 0;
        hidePicture = 0;
    };
};
