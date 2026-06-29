# Arma 3 Attendance Bot Extension

**A self-contained mod that automatically tracks player join/leave events and sends them to your bot API - NO MISSION CHANGES REQUIRED!**

## 🎯 Key Features

✅ **Self-contained** - No mission changes required  
✅ **Config file support** - INI-style or JSON configuration  
✅ **Environment variables fallback** - Still supports env vars for compatibility  
✅ **Async queue** - Non-blocking HTTP requests  
✅ **Thread-safe** - Proper synchronization  
✅ **Cross-platform** - Linux (.so) and Windows (.dll)  
✅ **GitHub Actions** - Automatic builds with secrets  

## 🏗️ Architecture

```
Arma 3 Server + Mod
    │
    ▼
┌─────────────────────────────────────────────────────────────┐
│                    @attendance_bot Mod                           │
│                                                                  │
│  ┌─────────────────┐      ┌──────────────────────────────┐ │
│  │  config.cpp      │      │  init.sqf (auto-executed)      │ │
│  │  - Declares      │      │  - addMissionEventHandler      │ │
│  │    extension     │──────▶│  - Registers callbacks         │ │
│  └─────────────────┘      │    PlayerConnected              │ │
│                         │    PlayerDisconnected            │ │
│                         └──────────────┬─────────────────┘ │
│                                                  │              │
│                                                  ▼              │
│                         ┌──────────────────────────────┐ │
│                         │  attendance_bot.so/.dll       │ │
│                         │  - Reads config.hpp          │ │
│                         │  - Thread-safe Event Queue    │ │
│                         │  - Background Worker Thread   │ │
│                         │  - HTTP POST to API          │ │
│                         └──────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────────────────────┐
│                         Your Bot API                              │
│                    POST /api/bot/events                           │
└─────────────────────────────────────────────────────────────┘
```

## 📝 Configuration

The extension supports **multiple configuration methods** with this priority:

1. **Config file** (INI or JSON format) - Checked first
2. **Environment variables** - Fallback if no config file found

### Option 1: Config File (Recommended)

Create a config file in the mod directory:

**INI-style (`@attendance_bot/addons/attendance_bot/config.hpp`):**
```ini
// Attendance Bot Configuration
apiToken = your_bot_api_token_here
endpoint = https://your-bot-api.com/api/bot/events
```

**JSON format (`@attendance_bot/addons/attendance_bot/config.json`):**
```json
{
    "apiToken": "your_bot_api_token_here",
    "endpoint": "https://your-bot-api.com/api/bot/events"
}
```

Example files are provided:
- `config.hpp.example`
- `config.json.example`

### Option 2: Environment Variables

Set on your server:
```bash
export BOT_API_TOKEN="your_bot_api_token_here"
export BOT_API_ENDPOINT="https://your-bot-api.com/api/bot/events"
```

### Supported Key Names

Both methods support multiple key names:
- `apiToken`, `token`, `BOT_API_TOKEN`
- `endpoint`, `url`, `apiUrl`, `BOT_API_ENDPOINT`

## 📡 API Endpoint

Sends to `POST /api/bot/events`:

**Join Event:**
```json
{
  "steamId": "76561198000000000",
  "isJoin": true,
  "timestamp": "2024-01-15T12:30:00Z"
}
```

**Leave Event:**
```json
{
  "steamId": "76561198000000000",
  "isJoin": false,
  "timestamp": "2024-01-15T15:00:00Z"
}
```

**Headers:**
- `Authorization: Bearer <BOT_API_TOKEN>`
- `Content-Type: application/json`

## ⚡ Setup

### 1. Build the Extension

**GitHub Actions** will build automatically. Or build locally:

```bash
# Linux
sudo apt install build-essential cmake libcurl4-openssl-dev
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
# Output: attendance_bot.so

# Windows
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
# Output: attendance_bot.dll
```

### 2. Install on Arma 3 Server

```bash
# Copy the built extension to mod directory
cp build/attendance_bot.so @attendance_bot/addons/attendance_bot/

# Option A: Create config file (recommended)
cp @attendance_bot/addons/attendance_bot/config.hpp.example \
   @attendance_bot/addons/attendance_bot/config.hpp
# Edit config.hpp with your token and endpoint

# Option B: Use environment variables
# (Set in your server startup script)

# Launch server
./arma3server -mod=@attendance_bot ...
```

### 3. That's It!

The mod automatically:
- Loads the extension
- Reads config from file (or env vars)
- Executes `init.sqf` via CfgFunctions
- Registers event handlers
- Starts tracking players asynchronously

## 🎛️ Commands

From debug console or SQF:

```sqf
callExtension "version";           // "1.0.0"
callExtension "queueStatus";       // "Queue: X events"
callExtension "reloadConfig";      // Reload config file without restart
callExtension "setToken new_token"; // Set token at runtime
callExtension "setEndpoint new_url"; // Set endpoint at runtime
```

## 🔄 Config Reloading

You can reload the configuration without restarting the server:

```sqf
callExtension "reloadConfig";
```

Or edit the config file and the extension will automatically detect changes on next event (depending on implementation).

## 📁 File Structure

```
arma3-attendance/ (development)
├── @attendance_bot/                      # ← Copy this to Arma 3
│   ├── addons/
│   │   └── attendance_bot/
│   │       ├── config.cpp               # Arma config + auto-execution
│   │       ├── config.hpp.example        # Config file template (INI)
│   │       ├── config.json.example       # Config file template (JSON)
│   │       ├── init.sqf                 # Auto-registers event handlers
│   │       └── attendance_bot.so/.dll     # Compiled extension
│   └── keys/
│       └── readme.txt                   # For mod signing
├── .github/
│   └── workflows/
│       └── build.yml                     # CI/CD pipeline
├── include/                              # C++ headers
│   ├── extension.h
│   ├── httprequest.h
│   ├── config.h
│   ├── configfile.h                     # Config file reader
│   ├── utils.h
│   └── queue.h
├── src/                                 # C++ sources
│   ├── extension.cpp
│   ├── httprequest.cpp
│   ├── config.cpp
│   ├── configfile.cpp                   # Config file implementation
│   ├── utils.cpp
│   └── queue.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 🚀 How It Works

### Mod Loading Flow:

1. **Arma 3 starts** with `-mod=@attendance_bot`
2. **Engine loads** `config.cpp` from the mod
3. **Engine sees** `CfgFunctions` with `preInit = 1`
4. **Engine executes** `\@attendance_bot\addons\attendance_bot\init.sqf`
5. **Extension loads** and reads config from file
6. **init.sqf runs** and registers event handlers
7. **When players join/leave**, handlers fire
8. **Handlers call** `callExtension` to the C++ extension
9. **C++ extension queues** events and returns "OK" immediately
10. **Worker thread** processes queue in background, sends HTTP POST

### Event Queue System:

```
SQF Handler → callExtension → [Thread-safe Queue] → Worker Thread → HTTP POST
         ↑                                  ↓
         └────────── "OK" (immediate) ────┘
```

## 🧪 Testing

The mod works automatically. To verify:

1. Check your API server receives events
2. Look in Arma 3 server RPT log for any errors
3. Test with `callExtension "queueStatus"` in debug console
4. Try `callExtension "reloadConfig"` after changing config

## 🐛 Troubleshooting

### Config not loading?
- ✅ Check file is named `config.hpp` or `config.json`
- ✅ Verify file is in `@attendance_bot/addons/attendance_bot/`
- ✅ Check file permissions: `chmod 644 config.hpp`
- ✅ Check for typos in the config file
- ✅ Try environment variables as fallback

### Mod not working?
- ✅ Verify `-mod=@attendance_bot` in server startup
- ✅ Check file permissions: `chmod -R 755 @attendance_bot`
- ✅ Verify .so/.dll architecture matches server (32-bit vs 64-bit)
- ✅ Check RPT log for extension load errors

### Events not being sent?
- ✅ Verify token is correct in config file
- ✅ Check endpoint URL is correct
- ✅ Test API with curl:
  ```bash
  curl -X POST https://your-api.com/api/bot/events \
    -H "Authorization: Bearer YOUR_TOKEN" \
    -H "Content-Type: application/json" \
    -d '{"steamId":"76561198000000000","isJoin":true,"timestamp":"2024-01-15T12:30:00Z"}'
  ```

### Thread issues?
- Extension spawns a background thread on first use
- Requires pthread on Linux (installed by default)
- Errors logged to stderr (visible in server console)

## 📄 Implementation Details

### Config Loading Priority

```
1. Check for config.hpp in mod directory
2. Check for config.json in mod directory
3. Check for config.hpp in various standard locations
4. Fall back to BOT_API_TOKEN environment variable
5. Fall back to BOT_API_ENDPOINT environment variable
```

### Supported Config Keys

All these key names work in both config files and environment variables:
- `apiToken`, `token`, `BOT_API_TOKEN`
- `endpoint`, `url`, `apiUrl`, `BOT_API_ENDPOINT`

### Auto-Execution (config.cpp)
```cpp
class CfgFunctions
{
    class attendance_bot
    {
        class init {
            file = "\@attendance_bot\addons\attendance_bot\init.sqf";
            preInit = 1;  // Runs before mission starts
            postInit = 1; // Also runs for JIP players
        };
    };
};
```

### Async Queue (C++)
- Thread-safe std::queue with mutex
- Condition variable for efficient waiting
- Background std::thread for processing
- Atomic flags for clean shutdown

## 📜 License

MIT License - Free to use, modify, distribute.

## 🤝 Contributing

1. Fork repository
2. Create feature branch
3. Commit changes
4. Submit pull request

GitHub Actions will build and test automatically.
