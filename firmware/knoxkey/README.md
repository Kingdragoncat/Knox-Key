# Knox Key Firmware

A high-security, hardware-backed FIDO2 (CTAP2) firmware implementation designed specifically for the Knox Key hardware.

## Features

- **Hardware-Backed Entropy**: All secure random numbers are generated using the onboard ATECC608B TRNG via I2C, meeting stringent security and FIPS compliance standards.
- **Hybrid Cryptography**: Leverages CryptoAuthLib for hardware random number generation while utilizing highly optimized software routines for ECDSA and AES-256 (for CTAP credential storage).
- **Advanced User Presence**: Features a fully-debounced hardware button detection engine supporting Single, Double, and Long clicks for various authentication actions.
- **Visual Feedback**: RGB LED states provide clear feedback on the authenticator's status:
  - **White Wink**: Device locator / ping
  - **Green**: Success / Validated Presence
  - **Red**: Error / Timeout / Cancelled
  - **Blue/Yellow**: Awaiting User Presence Heartbeat
- **Platform**: Ported to run efficiently on the STM32L433 MCU using TinyUSB for communication.

## Building the Firmware

### Requirements
- **CMake** (v3.20+)
- **Arm GNU Toolchain** (`arm-none-eabi-gcc`)
- **Make** / **Ninja** (depending on your OS)

### Instructions

1. **Clone the Repository**
   Make sure to clone with submodules to include all dependencies:
   ```bash
   git clone --recurse-submodules https://github.com/Kingdragoncat/Knox-Key.git
   cd "Knox-Key/Knox Key Kicad/firmware/knoxkey"
   ```

2. **Configure the Build System**
   ```bash
   cmake -B build/stm32l433-release -S . --preset default -DBUILD_TARGET=stm32l433
   ```

3. **Compile the Firmware**
   ```bash
   cmake --build build/stm32l433-release -j 8
   ```

4. **Flash the Microcontroller**
   The final compiled binaries (`.elf`, `.hex`, and `.bin`) will be output to:
   `build/stm32l433-release/targets/stm32l433/`
   
   You can flash the `.hex` or `.bin` file to your Knox Key via STM32CubeProgrammer over USB DFU mode, or using an ST-Link via SWD.

## Development & Porting

This firmware is a heavy modification of the original LionKey CTAP core. It has been extensively refactored to support the hardware components unique to the Knox Key design:
- STM32L433 target integration (USB, I2C, Flash Storage mapping)
- Replaced the software pseudo-random number generator with the ATECC608B hardware interface via `CryptoAuthLib`.
- Overhauled user-presence loops to leverage hardware interrupt-driven RGB LED multiplexing.

## License

This project is licensed under the GNU General Public License v3.0.
