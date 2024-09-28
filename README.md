# Sitina 1

![front](/assets/r0p11_front.jpg)

Open-source 35mm full-frame (36mm x 24mm) CCD mirror-less interchangable lens camera (MILC). The original goal of this project was to create a digital camera back to convert SLR cameras to DSLR cameras, but has since changed to build a full MILC.

I have made 2 videos about this project:

- First video in 2023 (r0.6 hardware): [https://www.youtube.com/watch?v=Ma9FrN5COIo](https://www.youtube.com/watch?v=Ma9FrN5COIo)
- Second video in 2024 (r0.11-B hardware): [https://www.youtube.com/watch?v=OkfzjmY9cF8](https://www.youtube.com/watch?v=OkfzjmY9cF8)

## Specifications

The following are preliminary.

### Hardware

- SoC: Xilinx Zynq 7010 (2x667MHz Cortex-A9, 28K logic cells FPGA)
- RAM: 512MB 16-bit DDR3 SDRAM
- AFE: Analog Devices AD9990
- Sensor: Kodak/ONsemi KAI-11000/KAI-11002M/CM
- Screen: 3.4" 480x480 High-Brightness DPI IPS LCD
- External Ports:
  - SD slot (HS, 25MB/s)
  - USB Type-C (5V3A power supply)
  - Flash sync terminal

### Camera Features

The following are possible with hardware, but not necessarily supported by the current software.

- Sensor Type: 35mm Interline CCD sensor
- Sensor Size: 36.0 x 24.0 mm
- Effective Pixels: 10.7MP
- Lens Mount: Active E mount
- Still Resolutions:
  - 4008x2672 10.7MP 3:2
  - 3563x2672 9.5MP 4:3 1.08X crop
  - 2672x2672 7.1MP 1:1 1.27X crop
  - 2782x1856 5.2MP 3:2 APS-C crop
  - 2676x2007 5.4MP 4:3 APS-C crop
  - 2365x2365 5.6MP 1:1 APS-C crop
- Still Format:
  - DNG (10/12/14-bit lossless compressed RAW)
  - JPEG
- Readout Speed:
  - 4008x2672 up to 5FPS
  - 4008x480 at 28FPS (line skipping live view, rescaled to 480x320 for output)
- Video recording is not supported
- ISO: 100 to 6400 (color version)

## Status

Hardware R0.11 tested and working. The hardware still need a lot of work and is not ready yet.

## Sample Photos

![sample_721725966846.jpg](/assets/sample_721725966846.jpg)

![sample_851725972700.jpg](/assets/sample_851725972700.jpg)

![sample_861725972700.jpg](/assets/sample_861725972700.jpg)

## Revision History

See [old-versions.md](/OLD_VERSIONS.md) for change logs, stories, and photos of old revisions!

## License

Unless otherwise stated, the hardware design is released under CERN-OHL-P, and the software/ firmware are released under MIT.

Most other 3D models are obtained from vendor website, refer to the corresponding vendor website for licensing details. They are provided for convience only.