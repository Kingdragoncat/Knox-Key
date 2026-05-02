# Contributing to Knox Key

Thank you for your interest in contributing to the Knox Key project! This document outlines the development workflow and guidelines.

## Development Setup

### 1. Clone & Setup
```bash
git clone <repository-url>
cd "Knox Key Kicad"
```

### 2. Install KiCad 8.x
- Download from: https://www.kicad.org/download/
- Ensure version is 8.0 or later

### 3. Verify Project Opens
```bash
# Via command line:
kiutils open "Knox Key Kicad.kicad_pro"

# Or open KiCad GUI and use File > Open Project
```

## Development Workflow

### Before Making Changes
1. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. Open the project in KiCad:
   - Don't commit lock files (`.kicad_pcb.lck`, `.kicad_sch.lck`)
   - These are auto-generated and in `.gitignore`

### Making Changes

#### Schematic Changes
1. Edit `Knox Key Kicad.kicad_sch` in KiCad
2. Run ERC (Electrical Rules Check):
   - `Tools > Electrical Rules Checker`
   - Fix any violations
3. Save the file

#### PCB Layout Changes
1. Edit `Knox Key Kicad.kicad_pcb` in KiCad
2. Run DRC (Design Rules Check):
   - `Tools > Design Rules Checker`
   - Verify clearances and trace widths
3. Update renders if geometry changes significantly
4. Save the file

#### Adding Components
1. Update schematic first (`.kicad_sch`)
2. Run ERC to verify
3. Update PCB layout
4. Update BOM files if needed
5. Document in commit message

### Documentation Changes
- Update design renders if layout significantly changes
- Keep BOM.csv synchronized with project
- Update README.md for major changes

## Commit Guidelines

### Commit Message Format
```
<type>: <subject>

<body>

<footer>
```

### Types
- `feat`: New feature or component
- `fix`: Bug fix or design correction
- `docs`: Documentation updates
- `refactor`: Layout optimization, schematic reorganization
- `test`: Manufacturing verification, design checks
- `perf`: Performance improvements
- `chore`: Maintenance, project updates

### Examples
```bash
git commit -m "feat: add RGB LED support for status indication"

git commit -m "fix: correct ATECC608B pin spacing
- Updated footprint to 0.5mm pitch
- Re-routed connected traces
- Verified with DRC"

git commit -m "docs: update README with manufacturing steps"

git commit -m "refactor: reorganize power distribution planes
- Improved current paths
- Reduced EMI coupling
- DRC passed"
```

## Before Submitting a Pull Request

### Checklist
- [ ] Branch created from `main`
- [ ] Changes are on a feature branch
- [ ] ERC passes with no violations
- [ ] DRC passes with no violations
- [ ] Commit messages follow guidelines
- [ ] README/documentation updated if needed
- [ ] No lock files committed (`.lck` files)
- [ ] No backup files in commit (`.bak`, `~` files)
- [ ] Changes don't modify user-specific `.kicad_prl`

### Testing
1. Close and reopen project to verify it loads
2. Regenerate manufacturing files if needed:
   ```bash
   # In KiCad:
   # File > Fabrication Outputs > Gerbers
   # File > Fabrication Outputs > Component Placement
   # File > Export > Bill of Materials
   ```
3. Compare files in `manufacturing-kryptonite/` to verify changes

## Manufacturing Updates

If changes affect manufacturing:

1. Update manufacturing files in KiCad:
   - `File > Fabrication Outputs > Gerbers...` → `manufacturing-kryptonite/gerbers/`
   - `File > Fabrication Outputs > Component Placement...` → `manufacturing-kryptonite/`
   - `File > Export > Bill of Materials...` → Update `BOM.csv`

2. Verify JLCPCB compatibility:
   - Check all parts are in LCSC database
   - Verify component positions have no conflicts
   - Confirm copper clearances meet JLCPCB minimums (0.127mm)

3. Commit with clear message:
   ```bash
   git commit -m "chore: regenerate manufacturing files after schematic updates"
   ```

## Design Standards

### Electrical Rules
- All nets labeled (no floating nodes)
- All component values specified
- ERC should pass with zero violations
- Power distribution properly planned

### PCB Layout Rules
- DRC should pass with zero violations
- Trace width minimum: 0.254mm (follow JLCPCB standards)
- Via size: 0.3mm-0.6mm
- Minimum clearance: 0.127mm (JLCPCB spec)
- Ground planes on internal layers (if applicable)

### Component Placement
- Critical components (ATECC608B) placed centrally
- Decoupling capacitors close to power pins
- USB connector at board edge
- Consider thermal management

## Design Review Process

1. **Self Review**: Verify ERC/DRC, check for design errors
2. **Code Review**: Request review from team members
3. **Manufacturing Verification**: Test export to JLCPCB format
4. **Merge**: After approval, merge to main branch

## Updating Documentation & Images

### When to Update Images
- After major PCB layout changes
- After component repositioning
- Before release versions

### How to Update Renders
In KiCad:
1. **Schematic**: `File > Export > PDF/PNG` → `design-images/Schematic/`
2. **PCB Renders**: `View > 3D View` → Screenshot → `design-images/PCB/3D/`

## Questions or Issues?

- **Design Questions**: Open a GitHub issue with `question` label
- **Bug Reports**: Use `bug` label with detailed reproduction steps
- **Feature Requests**: Use `enhancement` label with use case

## Version Control Best Practices

### Do's ✅
- Commit often with clear messages
- Keep commits focused (one feature per commit)
- Pull before pushing to avoid conflicts
- Create branches for features/fixes
- Review diffs before committing

### Don'ts ❌
- Don't commit lock files (`.lck`)
- Don't commit user settings (`.kicad_prl`)
- Don't commit backup files (`.bak`)
- Don't mix multiple features in one commit
- Don't force push to `main` branch

## Release Process

1. Create release branch: `git checkout -b release/v1.x.x`
2. Update version references
3. Finalize documentation
4. Create annotated tag: `git tag -a v1.x.x -m "Release v1.x.x"`
5. Merge to main and develop
6. Push with tags: `git push origin main --tags`

## Resources

- **KiCad Documentation**: https://docs.kicad.org/
- **Git Guide**: https://git-scm.com/book/en/v2
- **GitHub Flow**: https://guides.github.com/introduction/flow/
- **JLCPCB Design Rules**: https://jlcpcb.com/help/article/PCB-Manufacturing-Capabilities-and-Limitations

---

Thank you for contributing to Knox Key! 🎉
