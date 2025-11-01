import serial
import struct
import sys
import argparse
from datetime import datetime

SYNC_MAGIC = 0xDEADBEEF
PACKET_HEADER_MAGIC = 0xA5A5A5A5

# PCAP Global Header
def write_pcap_header(f):
    # magic_number, version_major, version_minor, thiszone, sigfigs, snaplen, network (802.11)
    pcap_header = struct.pack('<IHHIIII', 0xa1b2c3d4, 2, 4, 0, 0, 65535, 105)
    f.write(pcap_header)

# PCAP Packet Header
def write_pcap_packet(f, ts_sec, ts_usec, packet_data):
    length = len(packet_data)
    pcap_packet_header = struct.pack('<IIII', ts_sec, ts_usec, length, length)
    f.write(pcap_packet_header)
    f.write(packet_data)

def list_serial_ports():
    """List all available serial ports"""
    from serial.tools import list_ports
    ports = list(list_ports.comports())
    if not ports:
        print("No serial ports found!")
        return []
    
    print("\nAvailable serial ports:")
    for i, port in enumerate(ports, 1):
        print(f"  [{i}] {port.device}: {port.description}")
    return ports

def main():
    parser = argparse.ArgumentParser(
        description='WiFi Packet Sniffer - Capture packets from ESP32-S3 to PCAP file',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s -p COM3 -o capture.pcap
  %(prog)s --port COM5 --output wifi_packets.pcap
  %(prog)s --list                    (list available ports)
  %(prog)s -p COM3 --display-only    (don't save, just display)
        """
    )
    
    parser.add_argument('-p', '--port', 
                        help='Serial port (e.g., COM3, COM4)')
    parser.add_argument('-o', '--output', 
                        help='Output PCAP filename (default: capture_YYYYMMDD_HHMMSS.pcap)')
    parser.add_argument('-b', '--baud', 
                        type=int, 
                        default=115200,
                        help='Baud rate (default: 115200)')
    parser.add_argument('-l', '--list', 
                        action='store_true',
                        help='List available serial ports and exit')
    parser.add_argument('-d', '--display-only', 
                        action='store_true',
                        help='Display packets without saving to file')
    parser.add_argument('-v', '--verbose', 
                        action='store_true',
                        help='Verbose output (show packet hex data)')
    
    args = parser.parse_args()
    
    # List ports and exit
    if args.list:
        list_serial_ports()
        return
    
    # If no port specified, list ports and ask user
    if not args.port:
        ports = list_serial_ports()
        if not ports:
            sys.exit(1)
        
        try:
            choice = input("\nSelect port number (or 'q' to quit): ").strip()
            if choice.lower() == 'q':
                sys.exit(0)
            port_idx = int(choice) - 1
            if 0 <= port_idx < len(ports):
                args.port = ports[port_idx].device
            else:
                print("Invalid selection!")
                sys.exit(1)
        except (ValueError, KeyboardInterrupt):
            print("\nCancelled")
            sys.exit(1)
    
    # Generate default filename if not specified
    if not args.output and not args.display_only:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        args.output = f"capture_{timestamp}.pcap"
    
    # Open serial port
    try:
        ser = serial.Serial(args.port, args.baud, timeout=1)
        print(f"✓ Connected to {ser.port} @ {args.baud} baud")
    except serial.SerialException as e:
        print(f"✗ Error opening {args.port}: {e}")
        print("\nTry:")
        print("  1. Check Device Manager for correct COM port")
        print("  2. Close other programs using the port")
        print("  3. Run with --list to see available ports")
        sys.exit(1)
    
    # Open PCAP file if saving
    pcap_file = None
    if not args.display_only:
        try:
            pcap_file = open(args.output, 'wb')
            write_pcap_header(pcap_file)
            print(f"✓ Saving to: {args.output}")
        except IOError as e:
            print(f"✗ Error creating file {args.output}: {e}")
            ser.close()
            sys.exit(1)
    
    print("\n" + "="*70)
    print("Capturing WiFi packets... Press Ctrl+C to stop")
    print("="*70)
    
    packet_count = 0
    error_count = 0
    
    try:
        while True:
            # Read sync magic (4 bytes)
            sync = ser.read(4)
            if len(sync) < 4:
                continue
            
            sync_val = struct.unpack('<I', sync)[0]
            if sync_val != SYNC_MAGIC:
                error_count += 1
                if args.verbose:
                    print(f"⚠ Bad sync: 0x{sync_val:08X}")
                continue
            
            # Read record length (4 bytes)
            rec_len_bytes = ser.read(4)
            if len(rec_len_bytes) < 4:
                continue
            rec_len = struct.unpack('<I', rec_len_bytes)[0]
            
            # Sanity check
            if rec_len > 10000 or rec_len < 20:
                error_count += 1
                if args.verbose:
                    print(f"⚠ Invalid length: {rec_len}")
                continue
            
            # Read packet prefix (16 bytes) sniffer header
            prefix = ser.read(16)
            if len(prefix) < 16:
                continue
            
            # Unpack prefix
            magic, ts_sec, ts_usec, channel, rssi, pkt_len = struct.unpack('<IIIBbH', prefix)
            
            if magic != PACKET_HEADER_MAGIC:
                error_count += 1
                if args.verbose:
                    print(f"⚠ Bad packet magic: 0x{magic:08X}")
                continue
            
            # Read payload
            payload = ser.read(pkt_len)
            if len(payload) < pkt_len:
                continue
            
            packet_count += 1
            
            # Display packet info
            timestamp_str = datetime.fromtimestamp(ts_sec).strftime('%H:%M:%S')
            print(f"[{packet_count:5d}] {timestamp_str}.{ts_usec:06d} | "
                  f"Ch:{channel:2d} | RSSI:{rssi:3d}dBm | Len:{pkt_len:4d}B", 
                  end='')
            
            # Determine packet type from first byte
            if len(payload) > 0:
                frame_type = payload[0]
                type_str = "DATA" if (frame_type & 0x0C) == 0x08 else \
                          "MGMT" if (frame_type & 0x0C) == 0x00 else \
                          "CTRL" if (frame_type & 0x0C) == 0x04 else "UNKN"
                print(f" | {type_str}", end='')
            
            print()  # newline
            
            # Verbose: show hex data
            if args.verbose and len(payload) > 0:
                hex_preview = ' '.join(f'{b:02X}' for b in payload[:32])
                print(f"          {hex_preview}...")
            
            # Write to PCAP file
            if pcap_file:
                write_pcap_packet(pcap_file, ts_sec, ts_usec, payload)
                if packet_count % 100 == 0:
                    pcap_file.flush()  # Flush every 100 packets
            
    except KeyboardInterrupt:
        print("\n" + "="*70)
        print("✓ Capture stopped")
        print("="*70)
        print(f"  Total packets: {packet_count}")
        print(f"  Errors:        {error_count}")
        if pcap_file:
            print(f"  Saved to:      {args.output}")
            print(f"\n💡 Open with Wireshark: wireshark {args.output}")
    
    except Exception as e:
        print(f"\n✗ Error: {e}")
    
    finally:
        if pcap_file:
            pcap_file.close()
        ser.close()
        print("\n✓ Closed")

if __name__ == "__main__":
    main()