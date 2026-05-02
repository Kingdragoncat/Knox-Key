Summary
- Imported Kryptonite schematic/PCB into this project and backed up originals to the `backup-kryptonite-*` folder.
- This document lists the cleaned BOM, recommended part/footprint updates, KiCad upgrade instructions, and optional RGB LED addition steps.

Cleaned BOM (suggested)
Reference,Value,Footprint,Qty,Manufacturer,Manufacturer Part,Suggested Replacement,Notes
C1,C11,C12,10u,Capacitor_SMD:C_0603_1608Metric,3,SANYEAR,C0603X5R106M160NT,OK,Keep 10uF 0603 X5R
C2,C3,C4,C5,C6,C7,C10,100n,Capacitor_SMD:C_0402_1005Metric,7,HRE,CGA0402X7R104K160GT,OK,100nF decoupling 0402
C8,C9,33p,Capacitor_SMD:C_0402_1005Metric,2,CCTC,TCC0402COG330J500AT,OK,33pF COG for crystal
D1,GR,LED_SMD:LED_0603_1608Metric_Pad1.05x0.95mm_HandSolder,1,XINGLIGHT,XL-1608SURC-06,Optional,Replace with RGB LED if adding
J1,Conn_01x04_Pin,Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical,1,CONNFLY,DS1021-1x4SF11-B,OK
P1,USB_C_Plug_USB2.0,type-c:TYPE-C-31-G-01,1,Korean Hroparts Elec,TYPE-C-31-G-01,OK,Verify footprint in KiCad libs
R1,R5,10k,Resistor_SMD:R_0402_1005Metric,2,YAGEO,RC0402FR-0710KL,OK
R2,R3,R4,4k7,Resistor_SMD:R_0402_1005Metric,3,UNI-ROYAL,0402WGJ0472TCE,OK
R6,68,Resistor_SMD:R_0402_1005Metric,1,FOJAN,FRC0402J680 TS,OK
SW1,SW_Push,Button_Switch_SMD:SW_Push_1P1T_XKB_TS-1187A,1,XKB,TS-1187A-B-A-B,OK
U1,STM32F072CBUx,QFN-48-1EP_7x7mm_P0.5mm_EP5.6x5.6mm,1,STMicroelectronics,STM32F072CBU6,Optional replace,Consider STM32G0 series (firmware porting required)
U2,ATECC608B-MAHDA,DFN-8-1EP_3x2mm_P0.5mm_EP1.3x1.5mm,1,Microchip,ATECC608B-MAHDA-S,OK
U3,MCP1700x-330xxTT,SOT-23,1,Microchip,MCP1700T-3302E/TT,OK
U4,USBLC6-2SC6,SOT-23-6,1,STMicroelectronics,USBLC6-2SC6,OK
Y1,8MHz,Crystal_SMD_3225-4Pin_3.2x2.5mm,1,Yajingxin,TAXM8M4RLBDDT2T,OK

Recommended updates
- KiCad version: Project is KiCad 8 format. Use latest KiCad 8.x (install newest stable) and open project; KiCad will suggest library upgrades.
- Footprints: Verify each footprint with official KiCad libraries and replace any vendor-specific footprints with official ones where possible (use Footprint Libraries > Manage Footprint Libraries).
- Manufacturer/MPN: For every part, confirm availability (LCSC, DigiKey) and update `Manufacturer Part` fields in the schematic to match your chosen vendor.
- MCU: Keep `STM32F072CBUx` if you want zero firmware changes; if you want newer features, consider `STM32G030K6` or `STM32G071` but expect firmware porting.
- ESD/USB parts: Confirm `USBLC6-2SC6` footprint and polarity for Type-C.
- Add test points for `VBUS`, `3V3`, `GND`, `SWDIO`, `SWCLK`, and `RESET`.

How to upgrade the project to latest KiCad
1. Install KiCad latest stable 8.x from https://kicad.org.
2. Open the project: File > Open Project > `Knox Key Kicad.kicad_pro`.
3. In Eeschema: Tools > Update Schematic Symbols (if prompted) and run ERC.
4. In Pcbnew: Tools > Update PCB from Schematic (CvPcb mapping must be correct). Resolve any missing footprints.
5. Use the Library Manager to update any custom footprint tables and set `fp-lib-table` if missing.
6. Re-run DRC and check for errors.

Adding an RGB LED (minimal, common-cathode)
Parts to add:
- RGB LED, 0603 SMD common-cathode (or common-anode if you prefer); example: `1608 RGB LED` (qty 1)
- Resistors RrgbR, RrgbG, RrgbB — 150R or 220R 0603 (3 pcs)

Suggested net/pin assignment (manual check required):
- Choose three free MCU GPIO pins (look for unused `PAx`/`PBx` nets in the schematic). I did not autowire pins to avoid collisions — pick pins that are not used for USB, I2C, or ATECC interface.
- Connect LED cathode to `GND` (common-cathode) and each anode through a resistor to the chosen MCU pins.
- In firmware, configure those pins as GPIO outputs with PWM (timer or software PWM) to drive colors.

Implementation steps (KiCad)
1. In Eeschema, place a `LED_RGB` symbol (or three `LED` symbols grouped) and three resistors.
2. Label nets for the three colors (e.g., `LED_R`, `LED_G`, `LED_B`).
3. Re-run Annotate and update PCB footprints via CvPcb.
4. Place footprints on PCB in a suitable location near existing `D1` or in a visible area; check mechanical enclosure clearance.
5. Update `BOM.csv` with chosen RGB LED MPN and resistor MPNs.

Files added
- `Knox Key Kicad/README-kryptonite-update.md` (this file)
- `Knox Key Kicad/BOM_suggested.csv` (cleaned CSV with suggested replacements)

Next steps I can take for you (choose one):
- I can add the RGB circuit to the schematic and place footprints on the PCB (requires confirming preferred MCU pins and RGB type).
- I can replace footprints in the PCB with official KiCad footprints automatically (I will update the `fp-lib-table` accordingly and run a footprint mapping pass).
- I can produce a final BOM with DigiKey/LCSC links for each chosen replacement and update `Knox Key Kicad.kicad_sch` fields.

If you want me to implement the RGB now, confirm: `common-cathode` vs `common-anode`, desired brightness (`150Ω` or `220Ω`), and whether I should pick free GPIOs automatically or wait for you to pick specific pins.
