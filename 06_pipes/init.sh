mkfifo urls
export FIFONAME="urls"
gcc main.c -o prog
./prog
rm urls