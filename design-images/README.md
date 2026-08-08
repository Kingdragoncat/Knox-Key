design-images/

This folder holds two kinds of images, kept separate on purpose:

**Build journal screenshots** — flat files at the root of this folder, numbered `01`–`40` (e.g. `04_hkdf_code.png`, `21_schematic_full.png`). These are embedded chronologically in [`JOURNAL.md`](../JOURNAL.md) and shouldn't be renamed or moved; the journal links to them by exact filename.

**Current renders** — kept up to date with the latest hardware revision and embedded in the [Renders section of the README](../README.md#renders):
- `Schematic/Schematic.png` and `Schematic/schematic.pdf` — full schematic export.
- `PCB/Top.png`, `PCB/Bottom.png`, `PCB/NoFill.png`, `PCB/All.png` — 2D board layer views.
- `PCB/3D/front.png` and `PCB/3D/back.png` — 3D renders.

See [`CONTRIBUTING.md`](../CONTRIBUTING.md#how-to-update-renders) for how to regenerate the current renders after a layout change.
