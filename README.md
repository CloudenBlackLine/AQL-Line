# AQL-Line

AQL-DF-1 controller code and host simulator.

## Project layout

- `aql-df-1/`: original C controller implementation and host simulator.

## Build

The host simulator requires GCC and Make. From the repository root:

```sh
make -C aql-df-1/control_mod sim
make -C aql-df-1/control_mod run
```

Compiling firmware objects requires `arm-none-eabi-gcc`:

```sh
make -C aql-df-1/control_mod fw_objects
```

Firmware object compilation does not produce a flashable firmware image; startup code, a linker script, and hardware integration remain necessary.

## Repository contents

Source code is versioned. Generated binaries, object files, source dumps, and local tools are excluded.
