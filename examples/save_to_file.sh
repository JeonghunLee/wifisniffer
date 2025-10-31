#!/bin/bash
# Save captured packets to a file with timestamp

if [ -z "$1" ]; then
    echo "Usage: $0 <serial_port> [output_file]"
    echo "Example: $0 /dev/ttyACM0 packets.log"
    exit 1
fi

PORT=$1
OUTPUT=${2:-"packets_$(date +%Y%m%d_%H%M%S).log"}

# Check if port exists and is readable
if [ ! -e "$PORT" ]; then
    echo "Error: Serial port $PORT does not exist"
    exit 1
fi

if [ ! -r "$PORT" ]; then
    echo "Error: Cannot read from $PORT (permission denied)"
    echo "Try: sudo usermod -a -G dialout $USER"
    exit 1
fi

echo "Capturing packets from $PORT to $OUTPUT"
echo "Press Ctrl+C to stop..."

# Capture packets to file
cat "$PORT" | tee "$OUTPUT"
