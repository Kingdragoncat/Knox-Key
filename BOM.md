# Bill of Materials — Knox Key

17 line items, 29 components total. Machine-readable version: [`BOM.csv`](BOM.csv). This is also mirrored in `manufacturing/bom.csv`, which is the exact file used for JLCPCB SMT assembly ordering (see [Manufacturing & Ordering](README.md#manufacturing--ordering) in the README).

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

\* R6/R7/R8 (the RGB LED current-limiting resistors) are still labeled with placeholder values (`R`/`G`/`B`) in the schematic rather than an actual resistance — confirm and update before ordering a board that needs them populated.

Parts without an LCSC number (D1, J3, U6) aren't in JLCPCB's basic/extended parts library under that footprint/value combo and need to be hand-placed or sourced separately.
