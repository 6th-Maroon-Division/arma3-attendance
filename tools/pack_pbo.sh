#!/bin/bash
# Pack PBO for Arma 3 Attendance Bot
# Run this after building the extension

set -e

echo "Packing PBO for Attendance Bot..."

# Create temporary directory for PBO contents
PBO_DIR="@attendance_bot/addons/attendance_bot_pbo"
PBO_NAME="attendance_bot"

# Clean up
rm -rf "$PBO_DIR"
mkdir -p "$PBO_DIR"

# Copy files for PBO
cp @attendance_bot/addons/attendance_bot/config.cpp "$PBO_DIR/"
mkdir -p "$PBO_DIR/scripts"
cp @attendance_bot/addons/attendance_bot/scripts/init.sqf "$PBO_DIR/scripts/"

# Create PBO using binarize
# Note: You need to have binarize tool from Arma 3 Tools
if command -v binarize &> /dev/null; then
    echo "Packing with binarize..."
    cd "$PBO_DIR"
    binarize -pack "$PBO_NAME.pbo" .
    cd ../..
    mv "$PBO_DIR/$PBO_NAME.pbo" "@attendance_bot/addons/"
    echo "PBO created: @attendance_bot/addons/$PBO_NAME.pbo"
else
    echo "WARNING: binarize not found. Cannot create PBO."
    echo "You need Arma 3 Tools installed to create PBO files."
    echo "Alternative: Manually create PBO or use unpbo tool."
    echo "PBO contents are in: $PBO_DIR"
    exit 1
fi

# Clean up
rm -rf "$PBO_DIR"

echo "PBO packing complete!"
