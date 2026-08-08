# Knox Key

This is a custom USB-C FIDO2 security key. The repository contains the KiCad 9 hardware files (schematics, PCB layout, fab outputs) and the authenticator firmware. 

The hardware pairs an STM32L433 microcontroller with an ATECC608B secure element. The project started out as an STM32F072 design (originally named "kryptonite") but was later reworked to use the L433.

## Hardware Details
* **MCU:** STM32L433CB (LQFP-48) with an 8 MHz crystal and internal USB PHY.
* **Crypto:** ATECC608B secure element on I2C (PB6/PB7) for hardware TRNG.
* **Port:** USB Type-C running USB 2.0 full speed.
* **Feedback:** Single RGB LED. Green is success, red is error, blue/yellow indicates waiting for user presence.
* **Button:** Tactile button for user presence verification (supports single, double, and long presses).
* **Power:** USB bus-powered via a 3.3V MCP1700 LDO.
* **PCB:** 2-layer layout.

## Working with KiCad
Open `Knox Key Kicad.kicad_pro` in KiCad 9. Make sure to run ERC and DRC before exporting manufacturing outputs. If you are hand-assembling the board, open `bom/ibom.html` in a web browser for the interactive BOM.

## Firmware
The firmware is in `firmware/knoxkey/` and implements a FIDO2 / CTAP2 authenticator. Random numbers are pulled from the ATECC608B over I2C, while credential wrapping uses software AES. User presence is verified via the hardware button.

To build the target for the STM32L433:
```bash
cd firmware/knoxkey
cmake -B build/stm32l433-release -S . --preset default -DBUILD_TARGET=stm32l433
cmake --build build/stm32l433-release -j 8
```
Binaries (`.elf`, `.hex`, `.bin`) will compile into `build/stm32l433-release/targets/stm32l433/`. Flash them using an ST-Link or via USB DFU mode.

## Bill of Materials
17 line items, 29 components total. Machine-readable versions: [`BOM.csv`](BOM.csv) and [`BOM.md`](BOM.md). `manufacturing/bom.csv` has the same data in the exact format JLCPCB's assembly service expects.

| Designator | Qty | Value | Footprint | Description | LCSC Part # |
|---|---|---|---|---|---|
| C1, C11, C12 | 3 | 10u | 0603 | Bulk/decoupling capacitor | [C466781](https://www.lcsc.com/product-detail/C466781.html) |
| C10, C2, C3, C4, C5, C6, C7 | 7 | 100n | 0402 | Decoupling capacitor | [C6119799](https://www.lcsc.com/product-detail/C6119799.html) |
| C8, C9 | 2 | 33p | 0402 | Crystal load capacitor | [C696910](https://www.lcsc.com/product-detail/C696910.html) |
| D1 | 1 | ASMB-MTB1-0A3A2 | LED_Avago_PLCC4_3.2x2.8mm_CW | RGB status LED | — |
| J3 | 1 | Conn_01x05 | PinHeader_1x05_P2.54mm_Vertical | SWD debug header | — |
| P1 | 1 | USB_C_Plug_USB2.0 | TYPE-C-31-G-01 | USB Type-C receptacle | [C530605](https://www.lcsc.com/product-detail/C530605.html) |
| R1, R5 | 2 | 10k | 0402 | Pull-up resistor | [C60490](https://www.lcsc.com/product-detail/C60490.html) |
| R2, R3, R4 | 3 | 4k7 | 0402 | USB CC pull-down / series resistor | [C25940](https://www.lcsc.com/product-detail/C25940.html) |
| R6 | 1 | R* | 0402 | RGB LED red channel current-limiting resistor | — |
| R7 | 1 | G* | 0402 | RGB LED green channel current-limiting resistor | — |
| R8 | 1 | B* | 0402 | RGB LED blue channel current-limiting resistor | — |
| SW1 | 1 | SW_Push | SW_Push_1P1T_XKB_TS-1187A | User presence tactile button | [C318884](https://www.lcsc.com/product-detail/C318884.html) |
| U2 | 1 | ATECC608B-MAHDA | DFN-8-1EP_3x2mm_P0.5mm_EP1.3x1.5mm | Cryptographic secure element | [C1518773](https://www.lcsc.com/product-detail/C1518773.html) |
| U3 | 1 | MCP1700x-330xxTT | SOT-23 | 3.3V LDO regulator | [C39051](https://www.lcsc.com/product-detail/C39051.html) |
| U4 | 1 | USBLC6-2SC6 | SOT-23-6 | USB D+/D- ESD protection | [C7519](https://www.lcsc.com/product-detail/C7519.html) |
| U6 | 1 | STM32L433CBTx | LQFP-48_7x7mm_P0.5mm | Main microcontroller | — |
| Y1 | 1 | 8MHz | Crystal_SMD_3225-4Pin_3.2x2.5mm | Crystal oscillator | [C720915](https://www.lcsc.com/product-detail/C720915.html) |

\* R6/R7/R8 are still labeled with placeholder values (`R`/`G`/`B`) in the schematic rather than an actual resistance — confirm before ordering a board that needs them populated.

## Manufacturing & Ordering
All files needed to manufacture the board are inside `manufacturing/`:
* `gerbers/` and `knox-key-gerbers.zip` — Bare-board fabrication files.
* `bom.csv` — Bill of Materials (with LCSC part numbers).
* `positions.csv` — Pick-and-place component coordinates (CPL).
* `designators.csv` — Reference designator mappings.
* `netlist.ipc` — IPC-D-356 netlist.

For JLCPCB: Upload `knox-key-gerbers.zip` for the PCB, and use `bom.csv` and `positions.csv` for SMT assembly. Add a solder paste stencil to your order if you plan to solder manually. Don't forget to regenerate these from KiCad if you modify the layout!

## Renders
![3D front](design-images/PCB/3D/front.png)
![3D back](design-images/PCB/3D/back.png)
![PCB layout](design-images/PCB/Top.png)
![Schematic](design-images/Schematic/Schematic.png)

## Security Warning
This is a hobbyist project and has not undergone any formal security audit. The ATECC608B chips ship unprovisioned, meaning you must load your own keys onto the secure element. Do not use this for production environments without reviewing the firmware and provisioning process yourself.

## License
GPLv3. See `LICENSE` for details.
