nasm -f bin boot.asm -o boot.bin
nasm -f bin prog.asm -o prog.bin
cat boot.bin prog.bin > test_disk.img
# qemu-system-i386 -drive format=raw,file=test_disk.img
qemu-system-x86_64 -drive format=raw,file=test_disk.img

