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
	@sudo mount lsys.img /mnt
	@sudo rm /mnt/loader.bin -f
	@sudo rm /mnt/kernel.elf -f
	@sudo cp bin/boot/loader.bin /mnt
	@sudo cp bin/kernel/kernel.elf /mnt
	@sudo umount /mnt

kernel.code:
	@echo "Compiling The Code Of Loader..."
	echo "@gcc -c kernel/kernel.c -o bin/kernel/kernel.obj"
	@nasm kernel/wrapper.asm -o bin/kernel/kernel.obj -f elf32
	@ld bin/kernel/kernel.obj -o bin/kernel/kernel.elf --oformat elf32-i386 -e kernel_main -Ttext 0x30400

boot.sector.image: boot.sector.code lsys.img
	@echo "Writting Boot Sector Binaries To Image..."
	@dd if=bin/boot/boot.bin of=lsys.img count=1 conv=notrunc

boot.sector.code:
	@echo "Making Boot Sector Binaries..."
	@nasm boot/boot.asm -o bin/boot/boot.bin

lsys.img:
	@echo "Creating Raw Image Of System Lsys..."
	@bximage -fd -size=1.44 -q lsys.img

clean: bin
	@echo "Cleaning Up Legacy Output..."
	@rm -r -f bin

run: all
	@echo "Running..."
	@bochs
