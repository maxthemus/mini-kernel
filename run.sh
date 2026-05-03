nasm -f bin boot.asm -o boot.bin

# Compiling kernel code and linking
nasm -f elf32 prog.asm -o prog.o
nasm -f elf32 proc.asm -o proc.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel.c -o kernel.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kheap.c -o kheap.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kprintf.c -o kprintf.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c keyboard.c -o keyboard.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kshell.c -o kshell.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c timer.c -o timer.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kproc.c -o kproc.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c mem.c -o mem.o

# Linking scripts together
ld -m elf_i386 -T linker.ld -o kernel.elf prog.o kernel.o kheap.o kprintf.o keyboard.o kshell.o timer.o kproc.o proc.o mem.o

# Building kernel bin
objcopy -O binary kernel.elf kernel.bin

# Building disk image
cat boot.bin kernel.bin >test_disk.img

# qemu-system-i386 -drive format=raw,file=test_disk.img
qemu-system-i386 -drive format=raw,file=test_disk.img
