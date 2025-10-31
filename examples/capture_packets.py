#!/usr/bin/env python3
"""
Simple Python script to capture and display WiFi packets from the sniffer.

Usage:
    python3 capture_packets.py /dev/ttyACM0
    python3 capture_packets.py COM3  # Windows
"""

import sys
import serial
import time

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 capture_packets.py <serial_port>")
        print("Example: python3 capture_packets.py /dev/ttyACM0")
        sys.exit(1)

    port = sys.argv[1]
    baudrate = 115200

    try:
        print(f"Connecting to {port}...")
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Connected! Capturing packets (Ctrl+C to stop)...")
        print("-" * 80)

        packet_count = 0
        while True:
            if ser.in_waiting:
                line = ser.readline()
                try:
                    decoded = line.decode('utf-8', errors='ignore').strip()
                    if decoded:
                        print(decoded)
                        packet_count += 1
                        
                        # Print statistics every 100 packets
                        if packet_count % 100 == 0:
                            print(f"\n[Stats] Total packets captured: {packet_count}\n")
                except Exception as e:
                    print(f"Decode error: {e}")
            else:
                time.sleep(0.01)

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {port}")
        print(f"Details: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print(f"\n\nCapture stopped. Total packets: {packet_count}")
        ser.close()
        sys.exit(0)

if __name__ == "__main__":
    main()
