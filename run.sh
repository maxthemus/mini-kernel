nasm -f bin boot.asm -o boot.bin

# Compiling kernel code and linking
nasm -f elf32 prog.asm -o prog.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel.c -o kernel.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kheap.c -o kheap.o

# Linking scripts together
ld -m elf_i386 -T linker.ld -o kernel.elf prog.o kernel.o kheap.o

# Building kernel bin
objcopy -O binary kernel.elf kernel.bin

# Building disk image
cat boot.bin kernel.bin >test_disk.img

# qemu-system-i386 -drive format=raw,file=test_disk.img
qemu-system-i386 -drive format=raw,file=test_disk.img
