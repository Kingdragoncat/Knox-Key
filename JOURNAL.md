---
title: "Knox Key"
author: "Kingdragoncat"
description: "A high-security USB Type-C authentication key design built with KiCad 8, featuring the ATECC608B cryptographic authentication chip and secure firmware validation mechanisms."
created_at: "2026-03-10"
---

# March 10, 2026: Project Initiation & Hardware Security Research

I decided to build my own hardware FIDO2 security key. I started by researching how hardware security keys actually work, reading up on FIDO2, U2F standards, and comparing existing commercial devices like YubiKeys and Titan keys. I got inspired after doing a talk on security keys to build one myself.

Next, I looked at open-source FIDO2 key implementations. I did a deep dive into the Lionkey project, analyzing their schematics, component choices, and repository structure. This gave me a good reference model.

With a general design in mind, I initialized the KiCad project and began searching for MCU options. I checked JLCPCB's assembly catalog for parts with USB device stacks and sufficient RAM/Flash for cryptographic operations. I compared STM32F072, STM32L433, and similar models, ultimately choosing the STM32F072 as the main MCU due to its excellent FIDO2 support, cost, and availability.

![Key Research](design-images/15_key_research.png)
![LionKey reference](design-images/10_lionkey_preview.png)
![Start Project](design-images/16_start_project.png)
![Nucleo reference](design-images/11_nucleo_reference.png)
![STM32F072 MCU](design-images/17_%20STM32F072.png)

Total time spent: 3 hours

# March 12, 2026: Component Selection & BOM Generation

With the MCU selected, I focused on choosing the rest of the bill of materials. I searched JLCPCB for voltage regulators capable of producing a stable 3.3V rail from the 5V USB line, selecting the MCP1700T-3302E in a compact SOT-23 package and noting its recommended decoupling capacitor configuration.

For hardware cryptography, I picked the ATECC608 cryptographic coprocessor from Microchip. This handles ECDSA signatures and other FIDO2 operations securely. I then selected a Type-C connector and the USBLC6-2SC6 ESD protection chip for the data lines. Finally, I gathered passive component packages (resistors, decoupling capacitors, and pull-ups) to ensure everything was single-side mountable for easier assembly, and ran the KiCad BOM export plugin to generate our clean BOM file.

![Regulator selection](design-images/22_regulator_datasheet.png)
![ATECC608 selection](design-images/23_atecc608_datasheet.png)
![USB and ESD parts](design-images/24_usb_esd_parts.png)
![Passives selection](design-images/25_passives_list.png)
![BOM generation](design-images/03_bom_layout.png)

Total time spent: 2 hours

# March 15, 2026: Drawing Main Power, USB, and MCU Connections

I started drafting the schematic in KiCad, beginning with the main power supply circuits. I placed the voltage regulator, its input/output decoupling capacitors, and set up the power bus.

Next, I wired up the USB Type-C connector. I configured the CC pins with 5.1k pull-down resistors to enable proper host detection, set up series impedance matching resistors on the D+/D- lines, and integrated the ESD protection chip. Finally, I added the STM32F072 microcontroller, wiring up its power pins, crystal oscillator, and local decoupling caps, making sure they were grouped logically near the MCU pins.

![Power schematic](design-images/06_power_reg.png)
![USB ESD schematic](design-images/05_usb_esd.png)
![Crystal oscillator schematic](design-images/09_crystal_caps.png)

Total time spent: 4.5 hours

# March 18, 2026: Crypto Chip integration, Debug Ports, and ERC Checks

I completed the remaining parts of the schematic. First, I placed the ATECC608 crypto coprocessor and connected its I2C bus (SDA and SCL lines) directly to the MCU, including pull-up resistors and a dedicated decoupling capacitor.

To make testing possible, I added an SWD debug header, connecting the SWDIO and SWCLK lines. I also added a status indicator RGB LED with appropriate series current-limiting resistors, as well as a tactile button for user presence detection. Once the design was fully wired, I cleaned up the layout, routed sheet symbols, and ran the Electrical Rules Checker (ERC), correcting a couple of missing pin connections.

![ATECC608 I2C schematic](design-images/13_atecc_i2c.png)
![SWD port schematic](design-images/08_swd_symbol.png)
![User button and LED schematic](design-images/07_user_button.png)
![Full schematic](design-images/21_schematic_full.png)
![ERC check report](design-images/27_erc_check.png)

Total time spent: 2.25 hours

# March 22, 2026: PCB Placement & Critical Trace Routing

With a clean schematic, I imported the netlist into the PCB Editor. I spent a long time arranging the components to minimize the board's footprint, placing the USB Type-C connector on the edge, the MCU in the center, and the crypto chip and passives in compact clusters.

Once the layout was solid, I routed the copper traces. I prioritized the critical USB differential pair, ensuring the D+ and D- lines were routed with matched lengths and controlled impedance. I kept these high-speed lines away from noisier components and routing paths, making multiple optimization passes to keep trace lengths short.

![PCB placement](design-images/28_initial_placement.png)
![USB trace routing](design-images/02_usb_pads_closeup.png)

Total time spent: 6.5 hours

# March 25, 2026: Ground Pouring & Design Rule Checks

I worked on improving noise immunity and power distribution. First, I added copper ground planes to the top and bottom layers of the board, along with 3.3V power rails to distribute current cleanly. 

To improve performance, I replaced my original discrete wired ground connections with a unified ground pour/splash. Removing the old ground traces and stitching the planes together with vias took about 40 minutes. Finally, I ran the Design Rules Checker (DRC), correcting a few spacing violations and narrow trace segments.

![Board trace views](design-images/14_board_routing.png)
![Ground pour](design-images/30_ground_pour_step.png)
![DRC check report](design-images/31_drc_dialog.png)

Total time spent: 3.2 hours

# March 28, 2026: Modifying USB Connector and Crash Recovery

I attempted to modify the USB Type-C connector to a more robust version with a metal shell for extra durability. This required changing the schematic symbol, switching the footprint, and rerouting the critical USB data lines. 

Halfway through the layout rework, KiCad crashed unexpectedly. I lost about 30 minutes of work because of the crash. Luckily, I was able to recover most of it from the automatic backups in my project folder. I restored the project, re-completed the PCB modifications, and fixed the DRC clearance issues again.

![USB footprint rework](design-images/32_usb_shroud_fail.png)
![KiCad recovery folder](design-images/33_kicad_recovery.png)
![PCB layout state](design-images/20_pcb_layout.png)

Total time spent: 4 hours

# April 02, 2026: Simplification, Slack Feedback, & Manufacturing Prep

After reviewing the complex metal-shrouded connector, I realized it added unnecessary complexity and routing congestion. I decided to revert to the original, simpler USB Type-C design. 

I posted the design on the Hack Club Slack to get feedback. Someone pointed out that the RGB LED was wired to GND instead of 3.3V, so I corrected the schematic and rerouted that section. I then exported our production-ready Gerber files, drill files, and component placement coordinates (CPL). I did a final check using the 3D viewer to make sure component clearances and alignments looked correct.

![Reverted USB layout](design-images/34_original_usb_layout.png)
![LED wiring fix](design-images/35_led_wiring_fix.png)
![Generated gerbers list](design-images/36_manufacturing_files.png)
![3D model check](design-images/19_3d_front.png)

Total time spent: 3.5 hours

# April 10, 2026: Firmware Build System & USB Setup

With the hardware finalized, I transitioned to software. I set up the firmware build environment using CMake and the `arm-none-eabi-gcc` toolchain. I configured the build settings for the ARM Cortex-M0 core and defined the compiler flags and linker script mappings.

Once the build system compiled successfully, I configured the STM32 HAL USB device stack. I defined the basic USB descriptors, set up endpoint configurations, and confirmed that the device successfully enumerated on my PC as an unrecognized USB device.

![CMake setup](design-images/37_cmake_setup.png)
![USB stack setup](design-images/38_usb_device_stack.png)

Total time spent: 2.5 hours

# April 15, 2026: Implementing FIDO2 & Command Parsing

I began implementing the FIDO2 protocol layer. I set up the USB Human Interface Device (HID) descriptors so the key could communicate with the host operating system. I also implemented key derivation logic utilizing HKDF for security.

Finally, I wrote the basic packet parsing state machine. This extracts incoming FIDO2 command packets from raw USB HID reports and routes them to the appropriate handler functions. The key can now parse packets and prepare response packets to send back to the computer.

![FIDO2 HKDF implementation](design-images/04_hkdf_code.png)
![Command parsing code](design-images/40_command_parsing_code.png)

Total time spent: 1.25 hours
