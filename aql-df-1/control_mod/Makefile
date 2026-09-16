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

SIM_CFLAGS = -O2 -Wall -Wextra -DSIMULATOR_ON=1 -DAQL_DEBUG=1

FW_CFLAGS = \
	-Os \
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
