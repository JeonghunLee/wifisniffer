#!/usr/bin/env python3
"""
Advanced WiFi Sniffer with Statistics

This script provides enhanced features:
- Real-time packet statistics
- Filtering options
- Multiple output formats

Usage:
    python3 advanced_sniffer.py <serial_port> [options]

Options:
    -o, --output <file>     Save to pcap file instead of Wireshark
    -s, --stats             Show packet statistics
    -c, --count <n>         Stop after capturing n packets
    -h, --help              Show this help message

Examples:
    python3 advanced_sniffer.py /dev/ttyUSB0 -s
    python3 advanced_sniffer.py COM3 -o capture.pcap -c 1000
"""

import sys
import serial
import time
import argparse
import struct
from collections import defaultdict

BAUDRATE = 921600

# Pcap header constants
PCAP_MAGIC = 0xa1b2c3d4
PCAP_HEADER_SIZE = 24
PCAP_PACKET_HEADER_SIZE = 16

class PacketStats:
    def __init__(self):
        self.total_packets = 0
        self.total_bytes = 0
        self.frame_types = defaultdict(int)
        self.start_time = time.time()
    
    def update(self, packet_len):
        self.total_packets += 1
        self.total_bytes += packet_len
    
    def print_stats(self):
        elapsed = time.time() - self.start_time
        pps = self.total_packets / elapsed if elapsed > 0 else 0
        bps = (self.total_bytes * 8) / elapsed if elapsed > 0 else 0
        
        print(f"\n--- Packet Statistics ---")
        print(f"Total packets: {self.total_packets}")
        print(f"Total bytes: {self.total_bytes}")
        print(f"Duration: {elapsed:.1f}s")
        print(f"Packets/sec: {pps:.1f}")
        print(f"Bits/sec: {bps:.0f}")
        print(f"-------------------------\n")

def wait_for_pcap_header(ser):
    """Wait for and validate pcap file header"""
    print("Waiting for pcap header...")
    
    header = ser.read(PCAP_HEADER_SIZE)
    while len(header) < PCAP_HEADER_SIZE:
        header += ser.read(PCAP_HEADER_SIZE - len(header))
    
    magic = struct.unpack('<I', header[0:4])[0]
    # Check both little-endian (0xa1b2c3d4) and big-endian (0xd4c3b2a1) formats
    if magic != PCAP_MAGIC and magic != 0xd4c3b2a1:
        print(f"Warning: Invalid pcap magic number: 0x{magic:08x}")
        print("Expected: 0xa1b2c3d4 (little-endian) or 0xd4c3b2a1 (big-endian)")
        print("Continuing anyway...")
    else:
        print("Valid pcap header received")
    
    return header

def main():
    parser = argparse.ArgumentParser(description='Advanced WiFi Sniffer')
    parser.add_argument('serial_port', help='Serial port (e.g., /dev/ttyUSB0 or COM3)')
    parser.add_argument('-o', '--output', help='Output pcap file')
    parser.add_argument('-s', '--stats', action='store_true', help='Show statistics')
    parser.add_argument('-c', '--count', type=int, help='Stop after N packets')
    
    args = parser.parse_args()
    
    stats = PacketStats() if args.stats else None
    
    print(f"Opening serial port: {args.serial_port} at {BAUDRATE} baud")
    
    try:
        # Open serial port
        ser = serial.Serial(args.serial_port, BAUDRATE, timeout=1)
        time.sleep(2)  # Wait for ESP32 to boot
        
        # Clear any initial boot messages
        ser.reset_input_buffer()
        
        # Wait for pcap header
        pcap_header = wait_for_pcap_header(ser)
        
        # Open output
        if args.output:
            output = open(args.output, 'wb')
            output.write(pcap_header)
            print(f"Saving to: {args.output}")
        else:
            output = None
            print("No output file specified, displaying stats only")
        
        print("Capturing packets... Press Ctrl+C to stop.")
        
        packet_count = 0
        try:
            while True:
                # Read packet header
                pkt_header = ser.read(PCAP_PACKET_HEADER_SIZE)
                if len(pkt_header) < PCAP_PACKET_HEADER_SIZE:
                    continue
                
                # Parse packet header
                ts_sec, ts_usec, incl_len, orig_len = struct.unpack('<IIII', pkt_header)
                
                # Read packet data
                packet_data = ser.read(incl_len)
                while len(packet_data) < incl_len:
                    packet_data += ser.read(incl_len - len(packet_data))
                
                # Write to output if specified
                if output:
                    output.write(pkt_header)
                    output.write(packet_data)
                
                # Update statistics
                if stats:
                    stats.update(incl_len)
                
                packet_count += 1
                
                # Display progress
                if packet_count % 100 == 0:
                    print(f"Captured {packet_count} packets", end='\r')
                
                # Check if we've reached the count limit
                if args.count and packet_count >= args.count:
                    print(f"\nReached packet limit ({args.count})")
                    break
                    
        except KeyboardInterrupt:
            print(f"\nCapture stopped. Total packets: {packet_count}")
        
        finally:
            if output:
                output.close()
                print(f"Capture saved to {args.output}")
            
            if stats:
                stats.print_stats()
            
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
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
