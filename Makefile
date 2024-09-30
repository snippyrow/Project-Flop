CFLAGS = -ffreestanding -m32 -g

all: kernel asm link run

kernel:
	/usr/local/i386elfgcc/bin/i386-elf-gcc $(CFLAGS) -c Source/kernel.cpp -o Binaries/kernel.o

asm:
	nasm -felf32 "Source/boot.s" -f bin -o "Binaries/boot.bin"
	nasm "Source/kernel_entry.s" -f elf -o "Binaries/kernel_entry.o"
link:
	/usr/local/i386elfgcc/bin/i386-elf-ld -o "Binaries/full.bin" -Ttext 0x7e00 "Binaries/kernel_entry.o" "Binaries/kernel.o" --oformat binary
	cat "Binaries/boot.bin" "Binaries/full.bin" "Binaries/zeroes.bin" > "Binaries/game.bin"
run:
	dd if=Binaries/game.bin of=game.img bs=512 count=63
	# Now attach assets
	dd if=font.bin of=game.img bs=512 seek=63
	dd if=rage.bin of=game.img bs=512 seek=104
	qemu-system-x86_64 \
    -drive file=game.img,format=raw,index=0,if=ide \
    -m 128M \
    -d int \
    -no-reboot

# sudo dd if=game.img of=/dev/sda bs=4M status=progress && sync