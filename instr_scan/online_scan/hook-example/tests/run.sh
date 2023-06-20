## please create a new cgroup called "my" and change its own to the user
#cgroup v2: by root execute: cd /sys/fs/cgroup && mkdir my && chown gjghfd -R my
#cgroup v1: by root execute: cd /sys/fs/cgroup/memory && mkdir my && chown gjghfd -R my
echo 1G > /sys/fs/cgroup/memory/my/memory.limit_in_bytes    # cgroup v1
# echo 1G > /sys/fs/cgroup/my/memory.max  #cgroup v2
# gcc hello.c -o hello.spec
gcc hello.c -o hello.spec -Wl,--rpath=/home/ubuntu/lqj/glibc-install/lib \
		-Wl,--dynamic-linker=/home/ubuntu/lqj/glibc-install/lib/ld-linux-aarch64.so.1
./hello.spec &
# cgroup v1
sudo echo $(ps aux | grep "hello.spec" | head -n 1 | awk '{print $2}') > /sys/fs/cgroup/memory/my/cgroup.procs
# cgroup v2
# sudo echo $(ps aux | grep "test_cgroup" | head -n 1 | awk '{print $2}') > /sys/fs/cgroup/my/cgroup.procs