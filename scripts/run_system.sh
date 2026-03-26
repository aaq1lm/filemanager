#!/bin/bash

PROJECT_DIR=$(pwd)
LOG_FILE="$PROJECT_DIR/logs/report.txt"

echo "Cleaning previous build..."
make clean

echo "Compiling project..."
make

if [ $? -eq 0 ]; then
    mkdir -p logs
    echo "=== SYSTEM MONITOR REPORT ===" > "$LOG_FILE"
    ./monitor ./sample_files >> "$LOG_FILE"
    echo ""
    echo "Build and execution successful!"
    echo "Log saved to: $LOG_FILE"
else
    echo "Build FAILED. Check compiler errors above."
    exit 1
fi
