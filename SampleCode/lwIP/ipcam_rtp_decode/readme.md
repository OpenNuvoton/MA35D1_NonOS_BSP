# MA35D1 FreeRTOS lwIP IPCam RTP and AVTP/CVF H.264 Receiver

This sample runs on the MA35D1 Cortex-A35 platform with FreeRTOS, lwIP,
GMAC Ethernet, the VC8000 H.264 hardware decoder, the VC8000 post-processor,
and the MA35D1 display controller.

The decoded video is scaled by VC8000 PP and written directly to the display
framebuffer. The current LCD panel configuration in `main.c` is 1024x600.
The AVTP test stream below is generated as 1280x800 and is scaled to the
configured LCD output.

## Supported Input Paths

### RTP/H.264 over UDP

The original input path is:

```text
Ethernet -> IPv4 -> UDP port 50004 -> RTP/H.264 parser
                                   -> Annex-B ring buffer -> VC8000 H.264 decoder -> LCD
```

The receiver uses UDP port `50004`. The default sample configuration uses
`192.168.1.1/24`; the active `IPCAM_RTP` setting selects the camera-network
configuration.

### AVTP/CVF H.264

The AVTP input path is enabled by the GCC compiler define:

```text
HAVE_AVTP
```

The AVTP path is:

```text
Ethernet EtherType 0x22F0
       -> AVTP subtype CVF (0x03)
       -> CVF H.264 (format 0x02, subtype 0x01)
       -> H.264 payload extraction
       -> Annex-B ring buffer
       -> VC8000 H.264 decoder
       -> VC8000 PP
       -> framebuffer and LCD
```

AVTP media packets are Layer-2 Ethernet packets. They do not use an IPv4
address or UDP port. The current test stream uses:

```text
Destination MAC: ff:ff:ff:ff:ff:ff
EtherType:       0x22F0
Stream ID:       454d4f5389abcdef
```

Audio is not processed by this PoC. Non-CVF AVTP formats and AVTP audio are
ignored. The initial implementation does not require gPTP, SRP/MSRP, VLAN
priority, or a complete AVB/TSN stack.

## Build Configuration

Enable the following compiler define in the GCC project when AVTP support is
required:

```text
HAVE_AVTP
```

All AVTP-specific application code is guarded by `#ifdef HAVE_AVTP`. Without
this define, the original RTP/UDP path remains available.

The application links against:

```text
Library/VC8000Lib/libvc8000_RTOS.a
Library/DisplayLib/libdisp.a
```

## Generate an AVTP/CVF H.264 Test Stream

The following GStreamer command generates 500 test frames at 50 fps. This is
approximately 10 seconds of SMPTE test video. The input is converted to I420
(YUV 4:2:0), which avoids the High 4:4:4 H.264 profile that is not accepted
by the MA35D1 VC8000 configuration used by this sample.

```bash
gst-launch-1.0 -e \
       videotestsrc is-live=true pattern=smpte num-buffers=500 ! \
       video/x-raw,format=I420,width=1280,height=800,framerate=50/1 ! \
       x264enc tune=zerolatency speed-preset=ultrafast byte-stream=false ! \
       video/x-h264,stream-format=avc,alignment=au ! \
       h264parse ! \
       avtpcvfpay streamid=0x454d4f5389abcdef ! \
       filesink location=avtp_stream.bin
```

Important points:

- `videotestsrc` produces SMPTE color bars, not a camera image.
- `I420` keeps the encoded stream in a VC8000-compatible 4:2:0 format.
- `num-buffers=500` with `50/1` fps produces about 10 seconds of content.
- `avtpcvfpay` creates AVTP/CVF AVTPDUs.
- `filesink` writes concatenated AVTPDUs, not complete Ethernet frames.
- The output file must be replayed by a tool that adds the Ethernet header and
       EtherType `0x22F0`.

For a 30-second source, use approximately 1500 frames at 50 fps and change
the output name to `avtp_stream_30s.bin`.

## AVTP Replay on Windows 11

The Windows replay tool and `avtp_stream.bin` are kept together under the
`IPCam_Sim` directory. The replay tool requires Python, Scapy, and Npcap raw
packet support. Select the Npcap interface for the physical Ethernet adapter
connected to MA35D1.

Install Scapy if required:

```powershell
python -m pip install scapy
```

List Npcap interfaces:

```powershell
C:\Python314\python.exe `
       d:/Projects/SideProj/Tool/IPCam_AVTP_CVF_Log_Replay/replay_avtp_cvf_npcap.py `
       --list-interfaces
```

Replay at the normal 50 fps rate:

```powershell
C:\Python314\python.exe `
       d:/Projects/SideProj/Tool/IPCam_AVTP_CVF_Log_Replay/replay_avtp_cvf_npcap.py `
       --input "D:\Projects\SideProj\Tool\IPCam_AVTP_CVF_Log_Replay\avtp_stream.bin" `
       --iface "\Device\NPF_{AB471D02-85D3-4958-8664-5C209A22FD45}"
```

Replay without pacing to measure maximum receive/decode throughput:

```powershell
C:\Python314\python.exe `
       d:/Projects/SideProj/Tool/IPCam_AVTP_CVF_Log_Replay/replay_avtp_cvf_npcap.py `
       --input "D:\Projects\SideProj\Tool\IPCam_AVTP_CVF_Log_Replay\avtp_stream.bin" `
       --iface "\Device\NPF_{AB471D02-85D3-4958-8664-5C209A22FD45}" `
       --no-pacing
```

`--unpaced` is an alias for `--no-pacing`. Normal mode waits between Marker
frames according to `--fps` (default 50). Unpaced mode sends continuously and
does not represent original camera timing.

Validate the input without transmitting:

```powershell
C:\Python314\python.exe `
       d:/Projects/SideProj/Tool/IPCam_AVTP_CVF_Log_Replay/replay_avtp_cvf_npcap.py `
       --input "D:\Projects\SideProj\Tool\IPCam_AVTP_CVF_Log_Replay\avtp_stream.bin" `
       --dry-run
```

The tool validates AVTPDU length, CVF subtype, Stream ID, and H.264 payload
boundaries. It adds the following Ethernet header before transmission:

```text
Destination MAC: ff:ff:ff:ff:ff:ff
Source MAC:      selected Npcap interface MAC
EtherType:       0x22F0
```

## Network Topology

For AVTP Layer-2 testing, the Windows Ethernet adapter connected to MA35D1
must be selected in Npcap. A Linux VM must use a VirtualBox Bridged Adapter
bound to the physical Ethernet adapter. NAT can route ping/IPv4 traffic but is
not a reliable path for arbitrary Layer-2 EtherType `0x22F0` frames.

## Runtime Data Flow

### RTP mode

```text
GMAC RX -> lwIP ethernetif_input0() -> IPv4/UDP
                            -> RTP netconn receiver -> RTP/H.264 depacketizer
                            -> Annex-B ring -> VC8000 decode/PP -> LCD framebuffer
```

### AVTP mode

```text
GMAC RX -> ethernetif_input0() -> EtherType 0x22F0
                            -> CVF/H.264 validation -> Single-NAL/FU-A conversion
                            -> Annex-B ring -> VC8000 decode/PP -> LCD framebuffer
```

The shared H.264 ring buffer is 16 MiB with a 128 KiB linearization backup
area. Decoder processing starts after the ring reaches the 128 KiB trigger
level.

## Expected UART Output

With AVTP enabled, low-frequency statistics look similar to:

```text
AVTP stats: packets=1000 valid=1000 invalid=0 bytes=... h264_packets=1000 h264=... frames=...
VC8000 decode FPS: 50
```

AVTP statistics confirm Ethernet reception and CVF parsing. The VC8000 FPS is
based on completed decoded pictures, not decoder API call count.

## Troubleshooting

### No AVTP packets received

- Confirm the Npcap interface is the physical adapter connected to MA35D1.
- Confirm Wireshark sees EtherType `0x22F0` frames.
- Confirm the MA35D1 build contains `HAVE_AVTP`.

### AVTP packets received but no video

- Confirm `valid` equals the packet count and `invalid=0`.
- Confirm `ring_err=0`.
- Confirm the H.264 stream uses I420/YUV420 and a VC8000-compatible profile.
- Avoid High 4:4:4 streams; an SPS beginning with `67 F4` indicates the wrong
       profile for this tested VC8000 configuration.
- Confirm VC8000 reports width/height and PP initialization.

## Original RTP Test

To test the original RTP path with FFmpeg, send an H.264 RTP stream to the
MA35D1 address on UDP port `50004`:

```bash
ffmpeg -stream_loop -1 -re -i 1024600.mp4 \
                      -map 0:v:0 \
                      -an \
                      -c:v libx264 \
                      -preset ultrafast \
                      -tune zerolatency \
                      -f rtp \
                      -sdp_file stream.sdp \
                      rtp://192.168.1.1:50004
```

Adjust the destination IP to match `main.c` and the active `IPCAM_RTP`
configuration.

## Release Checklist

- Build the application with the intended `HAVE_AVTP` setting.
- Keep the matching `libvc8000_RTOS.a` with the release build.
- Use a VC8000-compatible H.264 profile and I420/YUV420 input.
- Remove or disable temporary debug instrumentation before release.
- Verify AVTP packet count, invalid count, ring errors, and decoded FPS.
- Verify the intended LCD resolution and framebuffer configuration.
- Keep the Windows replay tool and AVTPDU test files outside the firmware
       image unless required for validation.

