# Arma 3 Attendance Bot - Server Side Mod

**A self-contained server-side mod that automatically tracks player join/leave events using Steam ID and sends them to your bot API. NO MISSION CHANGES REQUIRED!**

## 🎯 Key Features

✅ **Server-side only** - No client files, no mission changes needed  
✅ **Self-contained** - Just install the mod, it works automatically  
✅ **Steam ID tracking** - Uses `getPlayerUID` for player identification  
✅ **Config file support** - INI-style (`config.hpp`) or JSON (`config.json`)  
✅ **Environment variables fallback** - Supports env vars for compatibility  
✅ **Async queue** - Non-blocking HTTP requests with background processing  
✅ **Thread-safe** - Proper mutex/condition variable synchronization  
✅ **GitHub Releases** - Automated builds published to GitHub Releases  

## 📁 Mod Structure

```
@attendance_bot/                    # ← Copy this folder to your Arma 3 server
├── attendance_bot.so/.dll          # ← C++ extension (in mod root)
├── mod.cpp                        # Mod metadata
├── logo.paa                       # Mod logo (placeholder)
├── config.hpp.example             # INI config template
├── config.json.example            # JSON config template
├── addons/
│   └── attendance_bot.pbo         # ← PBO with SQF scripts
└── keys/
    └── readme.txt                 # For mod signing
```

**The PBO contains:**
- `config.cpp` - Arma 3 extension declaration + CfgFunctions for auto-execution
- `scripts/init.sqf` - SQF script that registers event handlers

## 🏗️ Architecture

```
Arma 3 Server
    │
    ▼
┌─────────────────────────────────────────────────────────────┐
│                    @attendance_bot Mod                           │
│                                                                  │
│  Extension: @attendance_bot/attendance_bot.so                │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │  PBO: @attendance_bot/addons/attendance_bot.pbo          │ │
│  │  ┌─────────────────┐  ┌──────────────────────────────┐ │ │
│  │  │  config.cpp      │  │  scripts/init.sqf            │ │ │
│  │  │  - CfgExtensions  │──▶│  - addMissionEventHandler    │ │ │
│  │  │  - CfgFunctions   │  │    PlayerConnected          │ │ │
│  │  └─────────────────┘  │    PlayerDisconnected        │ │ │
│  │                        └──────────────┬───────────────┘ │ │
│  │                                         │                 │ │
│  │                                         ▼                 │ │
│  │                                callExtension "playerConnected"│ │
│  └─────────────────────────────────────────────────────────┘ │
│                                  │                                 │
│                                  ▼                                 │
│                         [Thread-safe Queue]                       │
│                                  │                                 │
│                                  ▼                                 │
│                         [Worker Thread]                          │
│                         HTTP POST to API                          │
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

Create `@attendance_bot/config.hpp` (INI-style):
```ini
// Attendance Bot Configuration
apiToken = your_bot_api_token_here
endpoint = https://your-bot-api.com/api/bot/events
```

Or `@attendance_bot/config.json` (JSON):
```json
{
    "apiToken": "your_bot_api_token_here",
    "endpoint": "https://your-bot-api.com/api/bot/events"
}
```

### Option 2: Environment Variables

Set in your server startup script:
```bash
export BOT_API_TOKEN="your_bot_api_token_here"
export BOT_API_ENDPOINT="https://your-bot-api.com/api/bot/events"
```

### Supported Key Names

All these work in both config files and environment variables:
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

### 1. Download

Get the latest release from:
- **Stable releases**: [GitHub Releases](https://github.com/your-repo/releases) (tagged versions like `v1.0.0`)
- **Latest build**: [GitHub Releases - Latest](https://github.com/your-repo/releases/tag/latest) (from main branch)

### 2. Install on Arma 3 Server

```bash
# Extract the downloaded ZIP
unzip attendance_bot-v*.zip

# Copy the mod folder to your Arma 3 server
cp -r @attendance_bot /path/to/arma3/

# For 64-bit server, use the 64-bit extension
ln -sf @attendance_bot/attendance_bot_64.so @attendance_bot/attendance_bot.so

# For 32-bit server, use the 32-bit extension
ln -sf @attendance_bot/attendance_bot_32.so @attendance_bot/attendance_bot.so

# Create config file
cp @attendance_bot/config.hpp.example @attendance_bot/config.hpp
# Edit config.hpp with your token and endpoint

# Launch server
./arma3server -mod=@attendance_bot ...
```

### 3. That's It!

The mod automatically:
- Loads the extension from `@attendance_bot/attendance_bot.so`
- Executes scripts from the PBO
- Registers event handlers
- Starts tracking players asynchronously

## 🎛️ Commands

From Arma 3 debug console or SQF:

```sqf
callExtension "version";           // Get version ("1.0.0")
callExtension "queueStatus";       // Check queue ("Queue: X events")
callExtension "reloadConfig";      // Reload config file
callExtension "setToken new";      // Set token at runtime
callExtension "setEndpoint url";   // Set endpoint at runtime
```

## 🔄 Config Reloading

Reload configuration without restarting:
```sqf
callExtension "reloadConfig";
```

## 📁 Development Structure

```
arma3-attendance/ (development repository)
├── @attendance_bot/                    # Mod folder (copied to release)
│   ├── mod.cpp                        # Mod metadata
│   ├── logo.paa                       # Logo placeholder
│   └── keys/
│       └── readme.txt                 # Signing info
│
├── @attendance_bot/addons/           # PBO source (packed into .pbo)
│   └── attendance_bot/                # PBO contents
│       ├── config.cpp                 # Arma config + auto-execution
│       └── scripts/
│           └── init.sqf               # Event handlers
│
├── .github/
│   └── workflows/
│       └── release.yml               # CI/CD workflow
│
├── include/                          # C++ headers
│   ├── extension.h
│   ├── httprequest.h
│   ├── config.h
│   ├── configfile.h
│   ├── queue.h
│   └── utils.h
│
├── src/                             # C++ sources
│   ├── extension.cpp
│   ├── httprequest.cpp
│   ├── config.cpp
│   ├── configfile.cpp
│   ├── utils.cpp
│   └── queue.cpp
│
├── tools/                           # Helper scripts
│   ├── pack_pbo.sh                   # Manual PBO packing
│   ├── create_release.sh             # Manual release creation
│   ├── config.hpp.example           # Config template
│   └── config.json.example          # Config template
│
├── CMakeLists.txt                    # CMake build
├── README.md
└── .gitignore
```

## 🚀 Building from Source

### Prerequisites

**Linux:**
```bash
sudo apt install build-essential cmake libcurl4-openssl-dev
```

**Windows:**
- Visual Studio 2022 with C++ support
- CMake
- vcpkg (for libcurl)

### Build

```bash
# Linux
mkdir build64 && cd build64
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
# Output: attendance_bot.so

# Windows
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
# Output: attendance_bot.dll
```

### Pack PBO

```bash
# Requires binarize tool from Arma 3 Tools
# Or use rapid-pbo: https://github.com/ArmaLife/rapid-pbo

# Manual packing
./tools/pack_pbo.sh

# Or manually:
cd @attendance_bot/addons
binarize -pack attendance_bot.pbo attendance_bot/
```

## 🐛 Troubleshooting

### PBO not loading?
- ✅ Verify `-mod=@attendance_bot` in server startup
- ✅ Check `@attendance_bot/addons/attendance_bot.pbo` exists
- ✅ Check file permissions: `chmod -R 755 @attendance_bot`

### Extension not loading?
- ✅ Verify `attendance_bot.so` (Linux) or `attendance_bot.dll` (Windows) exists in `@attendance_bot/`
- ✅ Check architecture matches server (32-bit vs 64-bit)
- ✅ Check RPT log for extension errors

### Config not loading?
- ✅ Create `@attendance_bot/config.hpp` or `config.json`
- ✅ Check file permissions: `chmod 644 config.hpp`
- ✅ Verify correct key names

### Events not being sent?
- ✅ Verify token and endpoint are correct
- ✅ Check API with curl:
  ```bash
  curl -X POST https://your-api.com/api/bot/events \
    -H "Authorization: Bearer YOUR_TOKEN" \
    -H "Content-Type: application/json" \
    -d '{"steamId":"76561198000000000","isJoin":true,"timestamp":"2024-01-15T12:30:00Z"}'
  ```

## 📄 GitHub Actions

The workflow automatically:
1. **On push to main**: Builds and publishes to `latest` release
2. **On tag push (v*)**: Builds and publishes to tagged release
3. **Creates ZIP** with complete mod structure
4. **Packs PBO** using rapid-pbo tool

### Triggering a Release

```bash
# Create and push a tag
git tag v1.0.0
git push origin v1.0.0

# Or push to main (auto-publishes to latest)
git push origin main
```

## 📜 License

MIT License - Free to use, modify, distribute.

## 🤝 Contributing

1. Fork repository
2. Create feature branch
3. Commit changes
4. Submit pull request

GitHub Actions will build and test automatically.
