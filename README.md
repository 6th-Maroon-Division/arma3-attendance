# Arma 3 Attendance Bot Extension

**A self-contained mod that automatically tracks player join/leave events and sends them to your bot API - NO MISSION CHANGES REQUIRED!**

## 🎯 Key Feature: Zero Mission Configuration

✅ **Just install the mod - it works automatically!**

The mod uses **CfgFunctions with preInit** to automatically execute its tracking code when loaded. No need to modify any missions or add scripts.

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
│  │  - CfgFunctions  │      │  - Handles PlayerConnected     │ │
│  └─────────────────┘      │    PlayerDisconnected          │ │
│                         └──────────────┬─────────────────┘ │
│                                                  │              │
│                                                  ▼              │
│                         ┌──────────────────────────────┐ │
│                         │  attendance_bot.so/.dll       │ │
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

## 🚀 Features

- ✅ **Self-contained** - No mission changes required
- ✅ **Automatic** - Event handlers registered automatically
- ✅ **Async queue** - Non-blocking HTTP requests
- ✅ **Thread-safe** - Proper synchronization
- ✅ **Cross-platform** - Linux (.so) and Windows (.dll)
- ✅ **GitHub Actions** - Automatic builds with secrets

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

### 1. GitHub Secrets

Set in your repository:
- `BOT_API_TOKEN` - Bearer token for your API
- `BOT_API_ENDPOINT` - Full URL (e.g., `https://your-api.com/api/bot/events`)

### 2. Build & Install

**GitHub Actions** will build automatically. Download the artifact and:

```bash
# Linux server
mkdir -p /path/to/arma3/@attendance_bot/addons/attendance_bot
cp attendance_bot.so /path/to/arma3/@attendance_bot/addons/attendance_bot/

# Set environment variables (in your server startup script)
export BOT_API_TOKEN="your_token_here"
export BOT_API_ENDPOINT="https://your-api.com/api/bot/events"

# Launch server
./arma3server -mod=@attendance_bot ...
```

**Windows server:** Similar, just use `.dll` instead.

### 3. That's It!

The mod automatically:
- Loads the extension
- Executes `init.sqf` via CfgFunctions
- Registers event handlers
- Starts tracking players

## 📁 File Structure

```
arma3-attendance/
├── @attendance_bot/                      # ← Copy this to your Arma 3 directory
│   ├── addons/
│   │   └── attendance_bot/
│   │       ├── config.cpp               # Arma config + auto-execution
│   │       ├── init.sqf                 # Auto-executed SQF script
│   │       └── attendance_bot.so         # Compiled extension (Linux)
│   │       └── attendance_bot.dll        # Compiled extension (Windows)
│   └── keys/
│       └── readme.txt                   # For mod signing
├── .github/
│   └── workflows/
│       └── build.yml                     # CI/CD pipeline
├── include/                              # C++ headers
│   ├── extension.h
│   ├── httprequest.h
│   ├── config.h
│   ├── utils.h
│   └── queue.h
├── src/                                 # C++ sources
│   ├── extension.cpp
│   ├── httprequest.cpp
│   ├── config.cpp
│   ├── utils.cpp
│   └── queue.cpp
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## 🎛️ How It Works

### Mod Loading Flow:

1. **Arma 3 starts** with `-mod=@attendance_bot`
2. **Engine loads** `config.cpp` from the mod
3. **Engine sees** `CfgFunctions` with `preInit = 1`
4. **Engine executes** `\@attendance_bot\addons\attendance_bot\init.sqf`
5. **init.sqf runs** and registers event handlers
6. **When players join/leave**, handlers fire
7. **Handlers call** `callExtension` to the C++ extension
8. **C++ extension queues** events and returns "OK" immediately
9. **Worker thread** processes queue in background, sends HTTP POST

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

## 🔧 Commands

From debug console or SQF:

```sqf
callExtension "version";           // "1.0.0"
callExtension "queueStatus";       // "Queue: X events"
callExtension "setToken new_token"; // Change token at runtime
callExtension "setEndpoint new_url"; // Change endpoint at runtime
```

## 🛠️ Building Locally

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

## 🐛 Troubleshooting

### Mod not working?
- ✅ Verify `-mod=@attendance_bot` in server startup
- ✅ Check file permissions: `chmod -R 755 @attendance_bot`
- ✅ Verify .so/.dll architecture matches server (32-bit vs 64-bit)
- ✅ Check RPT log for extension load errors

### Events not being sent?
- ✅ Verify `BOT_API_TOKEN` environment variable is set
- ✅ Check `BOT_API_ENDPOINT` is correct
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

### Event Handling (init.sqf)
```sqf
["PlayerConnected", attendance_bot_onPlayerConnected] call addMissionEventHandler;
["PlayerDisconnected", attendance_bot_onPlayerDisconnected] call addMissionEventHandler;
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
