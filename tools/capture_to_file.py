#!/usr/bin/env python3
"""
WiFi Sniffer to File Capture

This script reads pcap data from ESP32-S3 WiFi sniffer via serial port
and saves it to a file that can be analyzed later with Wireshark.

Usage:
    python3 capture_to_file.py <serial_port> <output_file>

Examples:
    python3 capture_to_file.py /dev/ttyUSB0 capture.pcap
    python3 capture_to_file.py COM3 wifi_capture.pcap
"""

import sys
import serial
import time

BAUDRATE = 921600

def main():
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit(1)
    
    serial_port = sys.argv[1]
    output_file = sys.argv[2]
    
    print(f"Opening serial port: {serial_port} at {BAUDRATE} baud")
    print(f"Output file: {output_file}")
    print("Waiting for ESP32-S3 to initialize...")
    
    try:
        # Open serial port
        ser = serial.Serial(serial_port, BAUDRATE, timeout=1)
        time.sleep(2)  # Wait for ESP32 to boot
        
        # Clear any initial boot messages
        ser.reset_input_buffer()
        
        print("Capturing packets to file... Press Ctrl+C to stop.")
        
        packet_count = 0
        with open(output_file, 'wb') as f:
            try:
                while True:
                    data = ser.read(1024)
                    if data:
                        f.write(data)
                        packet_count += 1
                        if packet_count % 100 == 0:
                            print(f"Captured {packet_count} packets", end='\r')
                        
            except KeyboardInterrupt:
                print(f"\nCapture stopped. Total packets captured: {packet_count}")
        
        ser.close()
        print(f"Capture saved to {output_file}")
        print(f"You can open this file in Wireshark for analysis.")
        
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

if __name__ == "__main__":
    main()
