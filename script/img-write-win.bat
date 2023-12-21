set DISK1_NAME=disk1.vhd

dd if=boot.bin of=%DISK1_NAME% bs=512 conv=notrunc count=1

dd if=loader.bin of=%DISK1_NAME% bs=512 conv=notrunc seek=1

dd if=kernel.elf of=%DISK1_NAME% bs=512 conv=notrunc count=200 seek=100