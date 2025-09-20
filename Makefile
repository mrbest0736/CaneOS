# CaneOS Makefile (placeholder)
# Add build rules for bootloader, kernel, drivers, filesystem, libraries, and user programs.


# CaneOS Makefile
# Build rules for bootloader, kernel, drivers, filesystem, libraries, and user programs

ASM=nasm
ASMFLAGS=-f bin

BOOTLOADER=boot/bootloader.asm
BOOTLOADER_BIN=bootloader.bin

all: $(BOOTLOADER_BIN)

$(BOOTLOADER_BIN): $(BOOTLOADER)
	$(ASM) $(ASMFLAGS) $< -o $@

# Stubs for future expansion
kernel:
	@echo "Add kernel build rules here."

drivers:
	@echo "Add drivers build rules here."

filesystem:
	@echo "Add filesystem build rules here."

libraries:
	@echo "Add libraries build rules here."

user:
	@echo "Add user program build rules here."

clean:
	del /Q $(BOOTLOADER_BIN)
