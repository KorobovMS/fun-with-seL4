# Intro

This project is intended as a little playground to learn seL4 microkernel. Here I try to create minimal environment consisting of seL4 and some usermode utilities which just use kernel's syscall interface. I also try not to use CAmkES or cmake-tools early in this project.

# Update submodules

After cloning this repository submodules should be also cloned:

`git submodule update --init`

# How to build

```
mkdir build
cd build
cmake -DCROSS_COMPILER_PREFIX= -DCMAKE_TOOLCHAIN_FILE=../seL4/gcc.cmake -G Ninja -C ../configs/kernel_cfg.cmake ../seL4/
ninja kernel.elf
objcopy -I elf64-x86-64 -O elf32-i386 kernel.elf 32.elf
```

# How to launch

`qemu-system-x86_64 -enable-kvm -cpu host -m 50M -kernel 32.elf -serial stdio`

# Makefile

There is also a Makefile to shorten things a little bit.

Use `make kernel` to build kernel and use `make launch` to launch Qemu.
