echo "Building kernel" 
echo "Building boot.asm"
nasm -f elf32 boot.asm -o boot.o
echo "Building kernel.c"
gcc -m32 -c kernel.c -o kernel.o -ffreestanding -O2 -Wall -Wextra -Wchar-subscripts
echo "Linking"
gcc -m32 -T linker.ld -o myos.bin boot.o kernel.o -ffreestanding -O2 -nostdlib -Wchar-subscripts
echo "Verify multiboot"
if grub-file --is-x86-multiboot myos.bin; then
	echo "Multiboot confirmed"
	echo "Creating ISODIR" 
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	echo "Creating ISO"
	grub-mkrescue -o myos.iso isodir
	echo "YAY, Ready!"
else
	echo "FATAL: not multiboot"
fi
