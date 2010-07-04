all:
	gcc -c ipl.S
	ld -T bootimage.ls -o bootimage.bin ipl.o

clean:
	rm *.o
	rm bootimage.bin

# sudo fdisk -l
# sudo dd if=bootimage.bin of=/dev/sdb
