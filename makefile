CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O2

all: encrypt decrypt_serial decryptParallel

encrypt: encrypt.c
	$(CC) $(CFLAGS) -o encrypt encrypt.c
decrypt_serial: decrypt_serial.c
	$(CC) $(CFLAGS) -o decrypt_serial decrypt_serial.c
decryptParallel: decryptParallel.c
	mpicc decryptParallel.c -o decryptParallel

clean:
	rm -f encrypt decrypt_serial decryptParallel