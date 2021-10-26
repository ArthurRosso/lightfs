rm cdoispontos.bin
gcc -g -o main main.c filesystem.c  -std=c11
./main
hexdump -C cdoispontos.bin > dump.txt  
