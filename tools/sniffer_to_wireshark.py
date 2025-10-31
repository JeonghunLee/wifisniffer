#!/usr/bin/env python3
"""
WiFi Sniffer to Wireshark Bridge

This script reads pcap data from ESP32-S3 WiFi sniffer via serial port
and pipes it to Wireshark for real-time packet analysis.

Usage:
    python3 sniffer_to_wireshark.py <serial_port> [wireshark_path]

Examples:
    # Linux/macOS
    python3 sniffer_to_wireshark.py /dev/ttyUSB0
    python3 sniffer_to_wireshark.py /dev/ttyUSB0 /usr/bin/wireshark
    
    # Windows
    python3 sniffer_to_wireshark.py COM3
    python3 sniffer_to_wireshark.py COM3 "C:\\Program Files\\Wireshark\\Wireshark.exe"
"""

import sys
import serial
import subprocess
import os
import time

BAUDRATE = 921600
WIRESHARK_DEFAULT_PATHS = [
    "wireshark",  # Available in PATH
    "/usr/bin/wireshark",  # Linux
    "/usr/local/bin/wireshark",  # macOS (Homebrew)
    "/Applications/Wireshark.app/Contents/MacOS/Wireshark",  # macOS (App)
    "C:\\Program Files\\Wireshark\\Wireshark.exe",  # Windows default
]

def find_wireshark():
    """Try to find Wireshark executable"""
    for path in WIRESHARK_DEFAULT_PATHS:
        try:
            if os.path.exists(path):
                return path
            # Try running it if it's in PATH
            subprocess.run([path, "--version"], capture_output=True, timeout=2)
            return path
        except (subprocess.SubprocessError, OSError, FileNotFoundError):
            continue
    return None

def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    
    serial_port = sys.argv[1]
    
    # Find Wireshark
    if len(sys.argv) >= 3:
        wireshark_path = sys.argv[2]
    else:
        wireshark_path = find_wireshark()
        if not wireshark_path:
            print("Error: Could not find Wireshark.")
            print("Please specify the path to Wireshark as the second argument.")
            sys.exit(1)
    
    print(f"Opening serial port: {serial_port} at {BAUDRATE} baud")
    print(f"Using Wireshark: {wireshark_path}")
    print("Waiting for ESP32-S3 to initialize...")
    
    try:
        # Open serial port
        ser = serial.Serial(serial_port, BAUDRATE, timeout=1)
        time.sleep(2)  # Wait for ESP32 to boot
        
        # Clear any initial boot messages
        ser.reset_input_buffer()
        
        print("Starting Wireshark...")
        
        # Start Wireshark with stdin pipe
        wireshark = subprocess.Popen(
            [wireshark_path, "-k", "-i", "-"],
            stdin=subprocess.PIPE,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL
        )
        
        print("Capturing packets... Press Ctrl+C to stop.")
        
        try:
            # Read from serial and write to Wireshark
            while True:
                data = ser.read(1024)
                if data:
                    wireshark.stdin.write(data)
                    wireshark.stdin.flush()
                
                # Check if Wireshark is still running
                if wireshark.poll() is not None:
                    print("\nWireshark closed.")
                    break
                    
        except KeyboardInterrupt:
            print("\nStopping capture...")
        
        finally:
            # Cleanup
            if wireshark.poll() is None:
                wireshark.terminate()
                wireshark.wait(timeout=5)
            ser.close()
            
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        print("\nAvailable serial ports:")
        try:
            import serial.tools.list_ports
            for port in serial.tools.list_ports.comports():
                print(f"  {port.device} - {port.description}")
        except ImportError:
            print("  (Install pyserial to list available ports)")
        sys.exit(1)
    
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
    
    print("Done.")

if __name__ == "__main__":
    main()
