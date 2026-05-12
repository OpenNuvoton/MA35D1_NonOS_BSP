# NOTE: This script is tested under Python 3.x
# -*- coding: utf-8 -*-

"""
USB Video Sink Test Utility
Copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
"""

__version__ = "v1.0.2"

import os
import sys
import argparse
import usb.core
import usb.util
import time

# USB Device configuration
TARGET_VID = 0x0416
TARGET_PID = 0x5963
ENDPOINT_OUT = 0x01  # Endpoint 1 BULK OUT
CHUNK_SIZE = 16 * 1024  # 16 KB

class UsbManager:
    """
    Handles single USB device detection and initialization using libusb/WinUSB backend.
    """
    def __init__(self, vid, pid):
        self.vid = vid
        self.pid = pid
        self.device = None
        self.find_and_open()

    def find_and_open(self):
        """
        Search for the specific device and set its configuration.
        """
        try:
            dev = usb.core.find(idVendor=self.vid, idProduct=self.pid)
            
            if dev is None:
                print(f"Device not found (VID: {hex(self.vid)}, PID: {hex(self.pid)}).")
                return

            try:
                dev.set_configuration()
                self.device = dev
            except usb.core.USBError as e:
                print(f"USB Error: Unable to open device at address {dev.address}: {str(e)}")
            except NotImplementedError:
                print("Backend operation not implemented.")

        except usb.core.NoBackendError:
            print("Error: libusb-1.0.dll or WinUSB driver not found.")
            sys.exit(1)

    def get_device(self):
        return self.device

def do_attach(h264_file_path) -> int:
    """
    Streams data to USB and calculates real-time KB/s.
    """
    if not os.path.exists(h264_file_path):
        print(f"Error: File not found at {h264_file_path}")
        return 1

    usb_mgr = UsbManager(TARGET_VID, TARGET_PID)
    device = usb_mgr.get_device()

    if not device:
        print("USB device initialization failed.")
        return 2

    print(f"Input File: {os.path.basename(h264_file_path)}")
    print(f"Streaming to EP1... Press Ctrl+C to stop.\n")

    total_bytes_sent = 0
    interval_bytes_sent = 0
    start_time = time.time()
    last_report_time = start_time
    
    try:
        with open(h264_file_path, 'rb') as f:
            while True:
                data = f.read(CHUNK_SIZE)
                
                if not data:
                    print("\n[Loop] Re-seeking to the beginning...")
                    f.seek(0)
                    continue
                
                try:
                    sent = device.write(ENDPOINT_OUT, data)
                    total_bytes_sent += sent
                    interval_bytes_sent += sent
                    
                    # Calculate and report every 1 second
                    current_time = time.time()
                    elapsed = current_time - last_report_time
                    
                    if elapsed >= 1.0:
                        kb_per_sec = (interval_bytes_sent / 1024) / elapsed
                        total_kb = total_bytes_sent // 1024
                        
                        sys.stdout.write(f"\rTotal: {total_kb} KB | Speed: {kb_per_sec:.2f} KB/s")
                        sys.stdout.flush()
                        
                        # Reset interval counter
                        interval_bytes_sent = 0
                        last_report_time = current_time
                        
                except usb.core.USBError as e:
                    print(f"\nUSB Transfer Error: {e}")
                    break

    except KeyboardInterrupt:
        print("\nStreaming stopped by user.")
    finally:
        usb.util.dispose_resources(device)
    
    return 0

def main():
    parser = argparse.ArgumentParser(description="Nuvoton USB Video Sink Test Utility (GPL v2.0)")
    parser.add_argument("FILE", help="Full path to the source H.264 file", type=str)
    parser.add_argument("-v", "--version", action='version', version=f'%(prog)s {__version__}')

    args = parser.parse_args()
    exit_code = do_attach(args.FILE)
    sys.exit(exit_code)

if __name__ == "__main__":
    main()
