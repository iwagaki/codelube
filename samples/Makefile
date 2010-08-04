# for amd64 users
# sudo apt-get install libc6-dev-i386

CFLAGS += -fno-stack-protector # __stack_chk_fail の除去
CFLAGS += -fno-exceptions # __gxx_personality_v0 の除去
CFLAGS += -fno-rtti
CFLAGS += -nostdlib

all:
	gcc -c -m32 ipl.S
	gcc -c -m32 kernel.S
	g++ -c -m32 -O3 ${CFLAGS} main.cc
	ld -melf_i386 -T bootimage.ls -o bootimage.bin ipl.o kernel.o main.o

clean:
	rm *.o
	rm bootimage.bin

# sudo fdisk -l
# sudo dd if=bootimage.bin of=/dev/sdb
