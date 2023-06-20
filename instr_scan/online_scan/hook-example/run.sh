
scp -P 6543 -r lqj@10.208.130.12:/home/lqj/instr_scan/online_scan /home/ubuntu/lqj
cd /home/ubuntu/lqj/online_scan/hook-example
make
echo "123" | sudo insmod example.ko

cd /home/ubuntu/lqj/online_scan/hook-example/tests
gcc hello.c -o hello.spec -Wl,--rpath=/home/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/lqj/glibc-install/lib/ld-linux-aarch64.so.1

gcc hello.c -o hello.spec -Wl,--rpath=/home/ubuntu/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/ubuntu/lqj/glibc-install/lib/ld-linux-aarch64.so.1
gcc shm_write.c -o shm_write.spec -Wl,--rpath=/home/ubuntu/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/ubuntu/lqj/glibc-install/lib/ld-linux-aarch64.so.1
gcc shm_exec.c -o shm_exec.spec -Wl,--rpath=/home/ubuntu/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/ubuntu/lqj/glibc-install/lib/ld-linux-aarch64.so.1

./hello.spec &
./shm_write.spec &
./shm_exec.spec &

# cd /home/ubuntu/lqj/online_scan/hook-example/
# cat /proc/$(ps aux | grep "hello.spec" | head -n 1 | awk '{print $2}')/maps

# cat /proc/$(ps aux | grep "shm_write.spec" | head -n 1 | awk '{print $2}')/maps
# cat /proc/$(ps aux | grep "shm_exec.spec" | head -n 1 | awk '{print $2}')/maps

kill $(ps aux | grep "hello.spec" | head -n 1 | awk '{print $2}')
kill $(ps aux | grep "shm_write.spec" | head -n 1 | awk '{print $2}')
kill $(ps aux | grep "shm_exec.spec" | head -n 1 | awk '{print $2}')

echo "123" | sudo rmmod example
echo "123" | sudo dmesg -c

# ipcrm -m $(ipcs -m | head -n 5 | awk '$3=="ubuntu" {print $2}') 
