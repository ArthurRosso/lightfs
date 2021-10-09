rm cdoispontos.bin
gcc -o main main.c filesystem.c cluster.c -std=c11
./main
hexdump -C cdoispontos.bin > dump.txt  
