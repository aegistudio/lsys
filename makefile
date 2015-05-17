all: bin
	@echo "Making Boot Sector Image..."
	@make loader.image

bin:
	@mkdir bin

loader.code:
	@echo "Making Loader Binaries..."
	@nasm loader.asm -o bin/loader.bin

loader.image: boot.sector.image loader.code
	@echo "Writing Loader Into The Image"
	@sudo mount bin/boot.img /mnt
	@sudo cp bin/loader.bin /mnt
	@sudo umount /mnt

boot.sector.image: boot.sector.code boot.raw.image
	@echo "Writting Boot Sector Binaries To Image..."
	@dd if=bin/boot.bin of=bin/boot.img count=1 conv=notrunc

boot.sector.code:
	@echo "Making Boot Sector Binaries..."
	@nasm boot.asm -o bin/boot.bin

boot.raw.image:
	@echo "Creating Raw Image Of Boot Sector..."
	@bximage -fd -size=1.44 -q bin/boot.img

clean: bin
	@echo "Cleaning Up Legacy Output..."
	@rm -r bin

run: all
	@echo "Running..."
	@bochs
