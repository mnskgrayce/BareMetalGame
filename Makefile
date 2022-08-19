# Specify the build and source folder
BUILD_DIR = ./build
SRC_DIR = ./src
SCRIPT_DIR = ./script

# Code files are all .c files inside SRC_DIR
CFILES = $(wildcard $(SRC_DIR)/*.c)

# Object files are the .o files inside BUILD_DIR with the same name as .c files
OFILES = $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Flags for building
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib
LDFLAGS = -nostdlib

# Run the "clean" and "kernel.img" commands
all: clean kernel8.img

# Make boot.o from the boot.S inside SRC_DIR
$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S
	aarch64-elf-gcc $(GCCFLAGS) -c $< -o $@

# Make other .o files from .c files inside SRC_DIR
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	aarch64-elf-gcc $(GCCFLAGS) -c $< -o $@

# Run the boot.o inside BUILD_DIR
kernel8.img: $(BUILD_DIR)/boot.o $(OFILES)
	aarch64-elf-ld $(LDFLAGS) $(BUILD_DIR)/boot.o $(OFILES) -T $(SCRIPT_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	aarch64-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/kernel8.img

# Delete the image file and stuff inside BUILD_DIR
clean:
	rm -f *.img $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/kernel8.img $(BUILD_DIR)/*.o

# Run the simulation on QEMU
run:
	qemu-system-aarch64 -M raspi3b -kernel $(BUILD_DIR)/kernel8.img -serial null -serial stdio

# Run the "all" and "run" commands
test: all run