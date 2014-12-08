crack: crack.c
	gcc crack.c -o crack -O3 -lcrypt -lpthread -lm dkh/md5.c
