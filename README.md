# Arma 3 Attendance Bot Extension

A C++ extension for Arma 3 that tracks player join/leave events and sends them to a bot API endpoint **asynchronously** using a queue system.

## Architecture

The extension uses a **queue-based architecture** for maximum reliability:

```
Arma 3 SQF Script
    │
    ▼ (callExtension - returns immediately)
┌─────────────────────────────────────┐
│      C++ Extension                   │
│  ┌─────────────────────────────────┐│
│  │      Event Queue (thread-safe)    ││  ← Events are queued here
│  └─────────────────────────────────┘│
│              │                        │
│              ▼                        │
│  ┌─────────────────────────────────┐│
│  │    Worker Thread                  ││  ← Processes queue in background
│  │  - HTTP POST to API               ││
│  │  - Handles retries on failure     ││
│  └─────────────────────────────────┘│
└─────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────┐
│         Your Bot API                   │
│  POST /api/bot/events                  │
│  Authorization: Bearer <token>         │
└─────────────────────────────────────┘
```

**Key Benefits:**
- ⚡ **Non-blocking**: Arma 3 game thread is never blocked by network calls
- 📦 **Reliable**: Events are queued and processed in the background
- 🔄 **Scalable**: Can handle bursts of events (many players joining at once)
- 🛡️ **Safe**: Thread-safe queue with proper synchronization

## Features

- Detects when players join or leave the Arma 3 server
- Sends HTTP POST requests to your bot API **asynchronously**
- Supports both Steam ID and Discord ID identification
- Works on both Linux (.so) and Windows (.dll) servers
- Built with GitHub Actions using GitHub secrets for API token
- Thread-safe event queue with background processing

## API Endpoint

The extension sends events to `POST /api/bot/events` with the following format:

### Join Event
```json
{
  "steamId": "76561198000000000",
  "isJoin": true,
  "timestamp": "2024-01-15T12:30:00Z"
}
```

### Leave Event
```json
{
  "steamId": "76561198000000000",
  "isJoin": false,
  "timestamp": "2024-01-15T15:00:00Z"
}
```

### Headers
- `Authorization: Bearer <BOT_API_TOKEN>`
- `Content-Type: application/json`

## Setup

### 1. GitHub Secrets

Set these secrets in your GitHub repository:
- `BOT_API_TOKEN` - The bearer token for your bot API
- `BOT_API_ENDPOINT` - The full URL to your API endpoint (e.g., `https://your-bot-api.com/api/bot/events`)

### 2. Build the Extension

The GitHub Actions workflow will automatically build the extension for Linux (32-bit and 64-bit) and Windows.

To trigger a build:
1. Push to `main` or `develop` branch
2. Create a pull request to `main`
3. Manually trigger via GitHub Actions

### 3. Install on Arma 3 Server

#### For Linux Server:

1. **Download the built .so file** from GitHub Actions artifacts
2. **Create mod folder structure:**
   ```bash
   @attendance_bot/
   ├── addons/
   │   └── attendance_bot/
   │       ├── config.cpp
   │       └── attendance_bot.so  # (or _64.so / _32.so)
   └── meta.cpp
   ```

3. **Set environment variables** on your server:
   ```bash
   export BOT_API_TOKEN="your_bot_token_here"
   export BOT_API_ENDPOINT="https://your-bot-api.com/api/bot/events"
   ```
   
   Add these to your server startup script or systemd service.

4. **Launch Arma 3 Server** with the mod:
   ```bash
   ./arma3server -mod=@attendance_bot -serverConfig=server.cfg -cfg=basic.cfg
   ```

#### For Windows Server:

1. Download `attendance_bot.dll` from artifacts
2. Place it in `@attendance_bot\addons\attendance_bot\`
3. Set environment variables in your server's batch file:
   ```batch
   set BOT_API_TOKEN=your_bot_token_here
   set BOT_API_ENDPOINT=https://your-bot-api.com/api/bot/events
   ```
4. Launch with `-mod=@attendance_bot`

### 4. Mission Setup

Add the init script to your mission:

1. Create a file `scripts/init.sqf` in your mission with the content from this repo's `scripts/init.sqf`
2. Include it in your mission's `init.sqf`:
   ```sqf
   [] execVM "scripts\init.sqf";
   ```

Or add to `description.ext`:
```cpp
class CfgFunctions
{
    class AttendanceBot
    {
        class init
        {
            file = "scripts\init.sqf";
            preInit = 1;
        };
    };
};
```

## Extension Commands

You can call these from SQF scripts:

```sqf
// Get extension version
callExtension "version";  // Returns "1.0.0"

// Set API token at runtime
callExtension "setToken your_api_token_here";

// Set API endpoint at runtime
callExtension "setEndpoint https://your-api.com/api/bot/events";

// Queue a player connected event (returns "OK" immediately)
callExtension "playerConnected 76561198000000000";

// Queue a player disconnected event (returns "OK" immediately)
callExtension "playerDisconnected 76561198000000000";

// Check queue status
callExtension "queueStatus";  // Returns "Queue: X events"
```

## Building Locally

### Linux

```bash
# Install dependencies
sudo apt-get install build-essential cmake libcurl4-openssl-dev

# Build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

# Output: attendance_bot.so
```

### Windows

Use Visual Studio with CMake:
```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release

# Output: attendance_bot.dll
```

## Testing

The extension processes events **asynchronously**, so you won't see immediate HTTP calls:

```sqf
// This returns "OK" immediately - the HTTP request happens in background
_result = callExtension "playerConnected 76561198000000000";
// _result will be "OK" right away

// You can check the queue status
callExtension "queueStatus";
```

To verify events are being sent, check your API server logs or the Arma 3 server console output (errors are logged to stderr).

## Troubleshooting

### Extension not loading
- Ensure the .so/.dll file is in the correct location
- Check that the file architecture matches your Arma 3 server (32-bit vs 64-bit)
- Verify file permissions on Linux: `chmod 644 attendance_bot.so`

### API calls failing
- Check that `BOT_API_TOKEN` environment variable is set
- Verify the endpoint URL is correct
- Test with a simple curl command to confirm the API is working:
  ```bash
  curl -X POST https://your-api.com/api/bot/events \
    -H "Authorization: Bearer your_token" \
    -H "Content-Type: application/json" \
    -d '{"steamId":"76561198000000000","isJoin":true,"timestamp":"2024-01-15T12:30:00Z"}'
  ```

### Events not being queued
- Check that the SQF event handlers are registered correctly
- Ensure `addMissionEventHandler` is called after the extension loads
- Look for errors in the Arma 3 server RPT log file

### Thread-related issues
- The extension spawns a background thread on first use
- Make sure your system has pthread support (it should on any modern Linux)
- On Windows, ensure you're using a recent compiler with C++17 support

## Directory Structure

```
arma3-attendance/
├── @attendance_bot/                      # Arma 3 mod folder
│   ├── addons/
│   │   └── attendance_bot/
│   │       ├── config.cpp               # Arma 3 extension declaration
│   │       └── attendance_bot.so          # Built extension (Linux)
│   └── meta.cpp                          # Mod metadata
├── .github/
│   └── workflows/
│       └── build.yml                     # GitHub Actions workflow
├── include/                              # C++ headers
│   ├── extension.h
│   ├── httprequest.h
│   ├── config.h
│   ├── utils.h
│   └── queue.h                           # Queue system
├── scripts/                              # SQF scripts
│   ├── config.sqf
│   ├── fn_init.sqf
│   └── init.sqf
├── src/                                 # C++ sources
│   ├── extension.cpp
│   ├── httprequest.cpp
│   ├── config.cpp
│   ├── utils.cpp
│   └── queue.cpp                         # Queue implementation
├── CMakeLists.txt                        # CMake build configuration
├── README.md
└── .gitignore
```

## Implementation Details

### Queue System

The extension uses a producer-consumer pattern:

1. **Producer** (Main thread / Arma 3 game thread):
   - Receives events from SQF via `callExtension`
   - Pushes events to thread-safe queue
   - Returns "OK" immediately

2. **Consumer** (Worker thread):
   - Waits for events on the queue
   - Processes each event by sending HTTP POST
   - Handles errors and could implement retry logic
   - Runs independently of the game thread

### Thread Safety

- All queue operations are protected by mutex
- Condition variables are used for efficient waiting
- Atomic flags for thread control
- Proper cleanup on extension unload

## License

MIT License - Feel free to use, modify, and distribute.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

The GitHub Actions workflow will automatically build and test your changes.
