CFLAGS = -fno-stack-protector

all:
	gcc -c -m32 ipl.S
	gcc -c -m32 -O3 ${CFLAGS} main.c
	ld -melf_i386 -T bootimage.ls -o bootimage.bin ipl.o main.o

clean:
	rm *.o
	rm bootimage.bin

# sudo fdisk -l
# sudo dd if=bootimage.bin of=/dev/sdb
