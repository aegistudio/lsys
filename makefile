all: bin
	@echo "Making Boot Sector Image..."
	@make assemble.image

bin:
	@mkdir bin
	@mkdir bin/boot
	@mkdir bin/kernel
	@mkdir bin/lib

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

lib.code:
	@echo "Compiling The Code Of Library..."
	@gcc -c lib/segmentation.c -I include/ -o bin/lib/segmentation.obj
	@nasm lib/video_base.asm -o bin/lib/video_base.obj -f elf32
	@gcc -c lib/video.c -I include/	-o bin/lib/video.obj
	@nasm lib/interrupt_base.asm -o bin/lib/interrupt_base.obj -f elf32
	@gcc -c lib/interrupt.c -I include/ -o bin/lib/interrupt.obj -w

kernel.code: lib.code
	@echo "Compiling The Code Of Kernel..."
	@nasm kernel/kernel.asm -o bin/kernel/kernel.obj -f elf32
	@gcc -c kernel/protect.c -I include/ -o bin/kernel/protect.obj
	@gcc -c kernel/video.c -I include/ -o bin/kernel/video.obj
	@gcc -c kernel/interrupt.c -I include/ -o bin/kernel/interrupt.obj
	@ld bin/kernel/*.obj bin/lib/*.obj -o bin/kernel/kernel.elf --oformat elf32-i386 -e kernel_main -Ttext 0x30400

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
