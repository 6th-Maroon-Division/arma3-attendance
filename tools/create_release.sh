#!/bin/bash
# Create release package for Attendance Bot
# This should be run after building the extension

set -e

PACKAGE_NAME="attendance_bot"
VERSION="1.0.0"

echo "Creating release package v$VERSION..."

# Create release directory
RELEASE_DIR="release/$PACKAGE_NAME-v$VERSION"
mkdir -p "$RELEASE_DIR"

# Copy mod folder
cp -r @attendance_bot "$RELEASE_DIR/"

# Copy build artifacts (if they exist)
if [ -f "build64/attendance_bot.so" ]; then
    cp build64/attendance_bot.so "$RELEASE_DIR/@attendance_bot/attendance_bot_64.so"
    echo "Copied 64-bit Linux extension"
fi

if [ -f "build32/attendance_bot.so" ]; then
    cp build32/attendance_bot.so "$RELEASE_DIR/@attendance_bot/attendance_bot_32.so"
    echo "Copied 32-bit Linux extension"
fi

if [ -f "build/attendance_bot.dll" ]; then
    cp build/attendance_bot.dll "$RELEASE_DIR/@attendance_bot/attendance_bot.dll"
    echo "Copied Windows extension"
fi

# Create config examples in mod folder
cp tools/config.hpp.example "$RELEASE_DIR/@attendance_bot/config.hpp.example"
cp tools/config.json.example "$RELEASE_DIR/@attendance_bot/config.json.example"

# Create ZIP package
cd release
zip -r "$PACKAGE_NAME-v$VERSION.zip" "$PACKAGE_NAME-v$VERSION"
cd ..

echo "Release package created: release/$PACKAGE_NAME-v$VERSION.zip"
echo ""
echo "Contents:"
ls -la "release/$PACKAGE_NAME-v$VERSION/"
