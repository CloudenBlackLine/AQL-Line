# AQL-DF-1 input update

Separate source copy of `../aql-df-1`, with MPU9250 input changes. The original source and CAD are untouched. Generated binaries, object files, and the old source dump were not copied.

The existing file layout, integer typedefs, function names where possible, and register-based C style are retained. No HAL or additional library is required.

## What changed

- `i2c_bus.c`: returns `I2c_Status` separately from data; checks NACK, bus errors, and timeouts. Supports single-byte reads and bursts of three or more bytes. Failed transfers reset the STM32 I2C peripheral. A physically stuck bus is reported, not automatically repaired.
- `motion_sensor.c`: verifies identity, resets/wakes the IMU, waits for startup, writes and reads back the configuration. Each update reads the status byte plus the 14 measurement bytes in one burst. Only successful, data-ready samples replace the caller's values.
- `motion_sensor.h`: adds `Imu_Status`, `motion_sensor_stale()`, and debugger-visible `imu_status` / `imu_whoami`. `motion_sensor_get()` is replaced by the output argument to `motion_sensor_update()`.
- `timer.c`: TIM2 now counts milliseconds continuously for delays, timeouts, and sample age. The hardware loop still polls at 100 Hz. Initialize the timer before the IMU.
- `controller.c`: consumes the new input API. Hardware control calculations/output are gated off while real angle estimation is unfinished. Simulation keeps its existing controller and display.

## Hardware assumptions

Target: STM32F401CCU6, MPU9250, I2C1 on PB6 (SCL) and PB7 (SDA), 100 kHz. Use a common ground and appropriate external pull-ups to 3.3 V (often already on the IMU module); the enabled internal pull-ups are not a substitute for checking the bus wiring. MPU9250 nCS must be high for I2C. AD0 low selects the default address 0x68; for AD0 high, change `MOTION_SENSOR_ADDR` to 0x69.

This version expects the reset HSI clock configuration: 16 MHz system/AHB/APB1, without prescalers. I2C initialization returns `IMU_CLOCK_ERROR` if the clock selection/dividers differ. Do not change clocks after initialization. TIM2 is reserved for this timer, and must run before any I2C calls. The driver is polling, single-controller, and must not be called concurrently from interrupts.

Sensor settings: 100 Hz, gyro and accelerometer filters enabled (configuration value 3), +/-2 g and +/-250 degrees/second. Outputs remain signed raw counts: 16384 counts/g and 131 counts/(degree/second). No magnetometer, calibration, angle estimation, or physical plausibility rejection is added. `IMU_OK` means a completed data-ready transfer, not proof that the measurement is physically correct. These initial ranges need review against the craft's actual motion.

## Using the input

```c
timer_init();
Imu_Status status = motion_sensor_init();

/* In the polling loop, after initialization succeeds: */
Motion_Sensor sample;
status = motion_sensor_update(&sample);
if(status == IMU_OK)
{
    /* sample contains a complete raw measurement. */
}
```

`IMU_NOT_READY` means no fresh sample yet. Other statuses identify initialization or transfer failures. On failure, the caller's sample stays unchanged. `motion_sensor_stale()` is true before the first sample or after 100 ms without a successful fresh sample. This is an input diagnostic threshold, not a finished flight-control failsafe. After a failed initialization or sensor power cycle, explicitly call `motion_sensor_init()` again; the loop does not repeatedly reset the IMU.

## Build and verify

From this directory:

```sh
make -C control_mod sim
make -C control_mod fw_objects
make -C control_mod test
make -C control_mod run
```

`test` uses a mock I2C bus to check identity/configuration failures, signed decoding, incomplete transfers, unchanged outputs on failure, freshness, recovery, and timer rollover. It does not emulate the STM32 peripheral or verify electrical timing. Simulation bypasses the real IMU driver; low-level simulated I2C reads return `I2C_NOT_READY` rather than inventing successful hardware reads.

`fw_objects` compiles Cortex-M4 objects only. As in the original, there is no startup/vector table, linker script, flash target, or implemented hardware PWM. This is not yet a flashable firmware image. Board testing is still required: confirm WHO_AM_I=0x71, inspect stationary/moving raw readings, and check disconnected-sensor errors and burst timing with a debugger/logic analyzer.

References: [STM32F401 reference manual, I2C receive sequencing](https://www.st.com/resource/en/reference_manual/rm0368-stm32f401xbc-and-stm32f401xde-advanced-armbased-32bit-mcus-stmicroelectronics.pdf), [MPU9250 register map](https://invensense.tdk.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf).
