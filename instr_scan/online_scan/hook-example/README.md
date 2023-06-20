# KernelApp_arm64
## Requirements
* Linux 

## Run
1. make & install
```shell
cd hook-example
make
sudo insmod example.ko
```

2. run a testcase with '.spec' as its suffix
```shell
cd tests
gcc ./hello.c -o hello.spec
./hello.spec
```

