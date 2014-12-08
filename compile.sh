#gcc -finput-charset=UTF-8 -o main main.c
gcc -finput-charset=UTF-8  -D__KERNEL__ -pg -g -lm -O4 -o main main.c
./main
