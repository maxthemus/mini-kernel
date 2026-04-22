asemble boot.asm
```bash
nasm -f bin <a>.asm -o <a>.bin
```


### Appending files to bootable file
```bash
cat boot.asm ... > disk.img
```

if you run (must be 512 bytes):
```bash
ls -l boot.bin
```


Running in qemu
```bash
qemu-system-x86_64 -drive format=raw,file=disk.img
```

Great example:
https://github.com/chuang76/x86-bootloader/blob/main/switch.asm



Cheat sheet for x86
https://ggbaker.ca/295/x86.html

Calling conventions between C and ASM
-  cdecl
https://gist.github.com/HACKE-RC/6303360bf9cbbb77992e4db0e1b672cd

Inorder to call a asm from C you must:
- Declare function def.
AND
- have label "global <name>" in asm file.

