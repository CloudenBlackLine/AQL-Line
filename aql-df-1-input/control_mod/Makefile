SIM_CC = gcc
FW_CC  = arm-none-eabi-gcc

SIM_BUILD = ./build/sim
FW_BUILD  = ./build/fw
BIN_DIR   = ../bin

COMMON_SRC = \
	source/motion_sensor.c \
	source/i2c_bus.c \
	source/controller.c \
	source/timer.c \
	source/stab_output.c \
	source/pwm_output.c \
	cm_main.c

SIM_SRC = \
	$(COMMON_SRC) \
	simulator/source.c

FW_SRC = \
	$(COMMON_SRC)

SIM_OBJ = $(patsubst %.c,$(SIM_BUILD)/%.o,$(SIM_SRC))
FW_OBJ  = $(patsubst %.c,$(FW_BUILD)/%.o,$(FW_SRC))

SIM_CFLAGS = -O2 -Wall -Wextra -MMD -MP -DSIMULATOR_ON=1 -DAQL_DEBUG=1

FW_CFLAGS = \
	-Os \
	-MMD -MP \
	-Wall -Wextra \
	-mcpu=cortex-m4 \
	-mthumb \
	-ffunction-sections \
	-fdata-sections \
	-fno-common \
	-ffreestanding \
	-fno-builtin \
	-DSIMULATOR_ON=0 \
	-DAQL_DEBUG=0

FW_LDFLAGS = \
	-Wl,--gc-sections \
	-Wl,-Map=$(BIN_DIR)/aqldf1.map


sim: $(BIN_DIR)/aqldf1_sim

$(BIN_DIR)/aqldf1_sim: $(SIM_OBJ)
	mkdir -p $(BIN_DIR)
	$(SIM_CC) $(SIM_OBJ) -o $@

$(SIM_BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	$(SIM_CC) $(SIM_CFLAGS) -c $< -o $@


fw_objects: $(FW_OBJ)

$(FW_BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	$(FW_CC) $(FW_CFLAGS) -c $< -o $@


run: sim
	$(BIN_DIR)/aqldf1_sim


clean:
	rm -rf ./build
	rm -f $(BIN_DIR)/aqldf1_sim
	rm -f $(BIN_DIR)/aqldf1.elf
	rm -f $(BIN_DIR)/aqldf1.map


all: sim

.PHONY: all sim run fw_objects clean test

test: $(SIM_BUILD)/motion_sensor_test
	$(SIM_BUILD)/motion_sensor_test

$(SIM_BUILD)/motion_sensor_test: tests/motion_sensor_test.c source/motion_sensor.c include/motion_sensor.h include/i2c_bus.h include/timer.h ../global/aqldf1.h
	mkdir -p $(dir $@)
	$(SIM_CC) -O2 -Wall -Wextra -Werror -DSIMULATOR_ON=0 -DAQL_DEBUG=0 tests/motion_sensor_test.c source/motion_sensor.c -o $@

-include $(SIM_OBJ:.o=.d) $(FW_OBJ:.o=.d)
