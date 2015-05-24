all: bin
	@echo "Making Boot Sector Image..."
	@make assemble.image

bin:
	@mkdir bin
	@mkdir bin/boot
	@mkdir bin/kernel

loader.code:
	@echo "Making Loader Binaries..."
	@nasm boot/loader.asm -o bin/boot/loader.bin

assemble.image: boot.sector.image loader.code kernel.code
	@echo "Writing Loader Into The Image..."
	@sudo mount bin/boot.img /mnt
	@sudo cp bin/boot/loader.bin /mnt
	@sudo cp bin/kernel/kernel.elf /mnt
	@sudo umount /mnt

kernel.code:
	@echo "Compiling The Code Of Loader..."
	@gcc -c kernel/kernel.c -o bin/kernel/kernel.obj
	@ld bin/kernel/kernel.obj -o bin/kernel/kernel.elf --oformat elf32-i386

boot.sector.image: boot.sector.code boot.raw.image
	@echo "Writting Boot Sector Binaries To Image..."
	@dd if=bin/boot/boot.bin of=bin/boot.img count=1 conv=notrunc

boot.sector.code:
	@echo "Making Boot Sector Binaries..."
	@nasm boot/boot.asm -o bin/boot/boot.bin

boot.raw.image:
	@echo "Creating Raw Image Of Boot Sector..."
	@bximage -fd -size=1.44 -q bin/boot.img

clean: bin
	@echo "Cleaning Up Legacy Output..."
	@rm -r bin

run: all
	@echo "Running..."
	@bochs
