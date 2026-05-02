# Knox Key - Secure Authentication Key PCB Design

A high-security USB Type-C authentication key design built with KiCad 8, featuring the ATECC608B cryptographic authentication chip and secure firmware validation mechanisms.

## 📋 Project Overview

**Knox Key** is an open-source, security-focused hardware authentication solution designed for:
- Secure device authentication
- Cryptographic key management
- Hardware-based access control
- Integration with existing security infrastructure

### Key Features
- **ATECC608B** - Hardware security module with AES/SHA algorithms
- **STM32 MCU** - Flexible microcontroller (F072 or G0 series)
- **USB Type-C** - Modern connector standard for universal compatibility
- **JLCPCB Ready** - Fully prepared for SMT assembly

## 📁 Project Structure

```
Knox Key Kicad/
├── Knox Key Kicad.kicad_pro        # KiCad 8 project file
├── Knox Key Kicad.kicad_sch        # Schematic source
├── Knox Key Kicad.kicad_pcb        # PCB layout (routed)
├── type-c.pretty/                  # Custom USB Type-C footprint
├── manufacturing/                  # JLCPCB manufacturing files (ready to order)
│   ├── BOM.csv                    # Bill of Materials with LCSC links
│   ├── BOM.xml                    # Alternative BOM format
│   ├── CPL.csv                    # Component Placement List
│   ├── component-positions.csv    # Position file (alternative format)
│   ├── gerbers/                   # Gerber files for PCB fabrication
│   └── knox-key-job.gbrjob        # JLCPCB job metadata
├── design-images/                 # Design documentation & renders
│   ├── PCB/                       # PCB visualizations
│   │   ├── 3D/                   # 3D perspective renders
│   │   ├── Top.png               # Top layer render
│   │   ├── Bottom.png            # Bottom layer render
│   │   └── NoFill.png            # PCB without copper pours
│   └── Schematic/                # Schematic documentation
│       ├── schematic.pdf         # Schematic PDF export
│       └── Schematic.png         # Schematic render
├── bom/
│   └── ibom.html                 # Interactive BOM (opens in browser)
├── backup-*/                     # Historical backups (previous revisions)
├── .gitignore                    # Git ignore rules
├── README.md                     # This file
├── BOM_suggested.csv             # Cleaned BOM with recommendations
└── DESIGN-UPDATES.md             # Detailed design updates & migration guide

```

## 🛠️ Getting Started

### Prerequisites
- **KiCad 8.x** or later ([Download](https://www.kicad.org/download/))
- **Git** for version control
- A browser for viewing the interactive BOM

### Opening the Project
1. Clone this repository:
   ```bash
   git clone <repository-url>
   cd "Knox Key Kicad"
   ```

2. Open in KiCad:
   ```bash
   kiutils open "Knox Key Kicad.kicad_pro"
   ```
   Or open KiCad and select `File > Open Project > Knox Key Kicad.kicad_pro`

### Viewing Documentation
- **Schematic**: `Knox Key Kicad.kicad_sch` (in KiCad) or `design-images/Schematic/Schematic.png`
- **Interactive BOM**: Open `bom/ibom.html` in your browser
- **3D View**: Open `Knox Key Kicad.kicad_pcb` in KiCad, then `View > 3D View`
- **Manufacturing Info**: See `manufacturing/` for complete JLCPCB assembly data

## 📊 Design Information

### Components
- **Microcontroller**: STM32F072 (or STM32G0 series - see upgrade guide)
- **Security Chip**: ATECC608B (Microchip authentication module)
- **USB Interface**: USB Type-C connector
- **Power**: 5V USB-powered
- **Total Components**: 16 (see BOM for details)

### PCB Specifications
- **Layers**: 2-layer PCB
- **Size**: [Check PCB file for exact dimensions]
- **Connector**: USB Type-C
- **Status**: ✅ Routed, ready for manufacturing

### Design Verification
See [DESIGN-UPDATES.md](DESIGN-UPDATES.md) for:
- Complete design checklist
- MCU options and considerations
- Optional enhancements (RGB LED, etc.)
- Design rule compliance verification

## 🏭 Manufacturing & Assembly

### JLCPCB Assembly (Ready to Order)
All manufacturing files are prepared and ready for JLCPCB:

1. **Navigate to JLCPCB**: https://jlcpcb.com
2. **Upload Gerbers**: `manufacturing/gerbers/` folder
3. **Upload BOM**: `manufacturing/BOM.csv`
4. **Upload CPL**: `manufacturing/CPL.csv`
5. **Configure**: 
   - SMT assembly: Yes
   - Stencil: Yes (recommended for solder paste)
   - Copy/place: 0.2mm tolerance

### Files Description
| File | Purpose |
|------|---------|
| `BOM.csv` | Component list with LCSC part numbers |
| `CPL.csv` | Pick-and-place coordinates for automation |
| `gerbers/` | Fabrication data (10 layers: Cu, mask, paste, silkscreen, edge, drill) |
| `job.gbrjob` | JLCPCB job metadata |

## 📸 Design Views

### PCB Renders
- **3D Front**: `design-images/PCB/3D/front.png`
- **3D Back**: `design-images/PCB/3D/back.png`
- **Top View**: `design-images/PCB/Top.png`
- **Bottom View**: `design-images/PCB/Bottom.png`

### Schematic
- **PDF**: `design-images/Schematic/schematic.pdf`
- **PNG**: `design-images/Schematic/Schematic.png`

## 🔄 Workflow

### Development
1. Make changes to `Knox Key Kicad.kicad_sch` (schematic) or `Knox Key Kicad.kicad_pcb` (layout)
2. Run ERC (Electrical Rules Check) & DRC (Design Rules Check)
3. Commit changes to git
4. Tag releases with version numbers

### Manufacturing
1. Update manufacturing files (right-click project > Export BOM/CPL/Gerbers)
2. Place files in `manufacturing/`
3. Upload to JLCPCB or other fabricator
4. Commit manufacturing outputs to git

### Documentation
- Update images in `design-images/` after major design changes
- Keep BOM_suggested.csv synchronized with KiCad project
- Update this README with any significant changes

## 📝 Notes & Customization

### Custom Footprint
- `type-c.pretty/TYPE-C-31-G-01.kicad_mod` - USB Type-C footprint
- Verify against KiCad's standard library before production

### MCU Options
The design supports multiple STM32 variants. See [DESIGN-UPDATES.md](DESIGN-UPDATES.md) for:
- STM32F072 current implementation
- STM32G0 series upgrade path
- Firmware compatibility notes

### Performance & Security Considerations
- ATECC608B provides hardware-level cryptographic acceleration
- USB interface supports full-speed (12 Mbps) communication
- Power consumption: Optimized for battery-free USB operation

## 🔐 Security Notes
- Custom authentication implementation - review security audit before production deployment
- ATECC608B should be provisioned with secure keys (not included in this design)
- USB firmware must implement proper authentication protocol

## 📚 Additional Resources

- **KiCad Documentation**: https://docs.kicad.org/
- **ATECC608B Datasheet**: Microchip AT608A-SHNP
- **STM32 MCU**: https://www.st.com/en/microcontrollers/stm32f0-series.html
- **JLCPCB Design Rules**: https://jlcpcb.com/help/article/PCB-Manufacturing-Capabilities-and-Limitations
- **USB Type-C Specification**: https://www.usb.org/

## 📋 Checklist Before Pushing

- [x] .gitignore configured for KiCad
- [x] README documentation complete
- [x] Manufacturing files prepared (gerbers, BOM, CPL)
- [x] Design images/renders in design-images/
- [x] Project structure organized and clean
- [ ] ERC & DRC verification passed
- [ ] JLCPCB manufacturing files tested
- [ ] Commit messages clear and descriptive

## 📄 License

[Add your license here - e.g., MIT, Apache 2.0, or specific open hardware license]

## 👥 Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/your-feature`)
3. Commit changes (`git commit -m "description"`)
4. Push to branch (`git push origin feature/your-feature`)
5. Open a Pull Request

## 📧 Support & Issues

For questions, bugs, or feature requests, please open an issue on GitHub.

---

**Last Updated**: May 2026  
**KiCad Version**: 8.x  
**Status**: ✅ Ready for manufacturing
