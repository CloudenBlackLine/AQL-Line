# AQL-Line

AQL-DF-1 controller code and mechanical CAD project.

## Project layout

- `aql-df-1/`: original C controller implementation and host simulator.
- `aql-df-1-input/`: separate controller source copy with MPU9250 input updates and input tests. See its [README](aql-df-1-input/README.md) for hardware assumptions and implementation status.
- `cad/`: editable FreeCAD and OpenSCAD designs, STEP/STL exports, dimensions, and a Cura project. See the [CAD notes](cad/README.txt) for the approximate visual models and their limitations.

Both controller source versions are retained so their differences can be reviewed.

## Build

The host simulator requires GCC and Make. From the repository root:

```sh
make -C aql-df-1/control_mod sim
make -C aql-df-1/control_mod run
```

For the updated sensor input version:

```sh
make -C aql-df-1-input/control_mod sim
make -C aql-df-1-input/control_mod test
```

Compiling firmware objects requires `arm-none-eabi-gcc`:

```sh
make -C aql-df-1-input/control_mod fw_objects
```

Firmware object compilation does not produce a flashable firmware image; startup code, a linker script, and hardware integration remain necessary. See the input version's README for details.

## Repository contents

Source code, editable CAD documents, and useful model exports are versioned. Generated binaries, object files, source dumps, FreeCAD backups, application installers, and the duplicate CAD ZIP are excluded. Install development and CAD applications separately.
