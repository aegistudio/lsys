all: bin
	@echo "Making Boot Sector Image..."
	@make assemble.image

bin:
	@mkdir bin
	@mkdir bin/boot
	@mkdir bin/kernel
	@mkdir bin/lib
	@mkdir bin/driver
	@mkdir bin/api

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

driver.code:
	@echo "Compiling The Code Of Drivers..."
	@nasm driver/video/video_base.asm -o bin/driver/video_base.obj -f elf32
	@gcc -c driver/video/video.c -I include/ -o bin/driver/video.obj
	@nasm driver/interrupt_base.asm -o bin/driver/interrupt_base.obj -f elf32
	@gcc -c driver/interrupt.c -I include/ -o bin/driver/interrupt.obj -w
	@nasm driver/keyboard/keyboard_base.asm -o bin/driver/keyboard_base.obj -f elf32
	@gcc -c driver/keyboard/keyboard.c -I include -o bin/driver/keyboard.obj
	@nasm driver/clock/clock_base.asm -o bin/driver/clock_base.obj -f elf32
	@gcc -c driver/clock/clock.c -I include -o bin/driver/clock.obj

lib.code:
	@echo "Compiling The Code Of Libraries..."
	@gcc -c lib/segmentation.c -I include/ -o bin/lib/segmentation.obj
	@gcc -c lib/scheduler.c -I include/ -o bin/lib/scheduler.obj
	@gcc -c lib/semaphore.c -I include/ -o bin/lib/semaphore.obj
	@nasm lib/scheduler_base.asm -o bin/lib/scheduler_base.obj -f elf32

kernel.code: lib.code driver.code api.code
	@echo "Compiling The Code Of Kernel..."
	@nasm kernel/kernel.asm -o bin/kernel/kernel.obj -f elf32
	@gcc -c kernel/protect.c -I include/ -o bin/kernel/protect.obj
	@gcc -c kernel/video.c -I include/ -o bin/kernel/video.obj
	@gcc -c kernel/interrupt.c -I include/ -o bin/kernel/interrupt.obj
	@nasm kernel/test_main.asm -o bin/kernel/test_main.obj -f elf32
	@nasm kernel/barber.asm -o bin/kernel/barber.obj -f elf32
	@gcc -c kernel/test_main.c -I include/ -o bin/kernel/test_main_c.obj
	@ld bin/kernel/*.obj bin/lib/*.obj bin/driver/*.obj bin/api/*.obj -o bin/kernel/kernel.elf --oformat elf32-i386 -e kernel_main -Ttext 0x30400

api.code:
	@echo "Compiling The Code Of API..."
	@nasm api/stdout_base.asm -o bin/api/stdout_base.obj -f elf32
	@gcc -c api/stdout.c -I include/ -o bin/api/stdout.obj
	@nasm api/process_base.asm -o bin/api/process_base.obj -f elf32
	@gcc -c api/process.c -I include/ -o bin/api/process.obj
	@nasm api/semaphore_base.asm -o bin/api/semaphore_base.obj -f elf32
	@gcc -c api/semaphore.c -I include/ -o bin/api/semaphore.obj

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
