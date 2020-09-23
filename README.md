# Intro

This project is intended as a little playground to learn seL4 microkernel. Here I try to create minimal environment consisting of seL4 and some usermode utilities which just use kernel's syscall interface. I also try not to use CAmkES or cmake-tools early in this project.

# Update submodules

After cloning this repository submodules should be also cloned:

`git submodule update --init`

# How to build

```
mkdir build
cd build
cmake -DCROSS_COMPILER_PREFIX= -DCMAKE_TOOLCHAIN_FILE=../seL4/gcc.cmake -DCMAKE_C_FLAGS=-fno-stack-protector -G Ninja -C ../configs/cfg.cmake ..
ninja kernel.elf
ninja libsel4.a
ninja sel4runtime
python -c "with open('./kernel/kernel.elf', 'r+b') as f: f.seek(18); f.write(int(3).to_bytes(2, 'little'));"
gcc -c ../projects/main.c -o main.o -fno-stack-protector \
	-I ../seL4/libsel4/include/ \
	-I ../seL4/libsel4/arch_include/x86 \
	-I ../seL4/libsel4/sel4_arch_include/x86_64/ \
	-I ../seL4/libsel4/mode_include/64 \
	-I ../seL4/libsel4/sel4_plat_include/pc99/ \
	-I ./libsel4/autoconf/ \
	-I ./kernel/gen_config/ \
	-I ./libsel4/gen_config/ \
	-I ./libsel4/include/ \
	-I ./libsel4/arch_include/x86/ \
	-I ./libsel4/sel4_arch_include/x86_64/
ld sel4runtime/libsel4runtime.a main.o libsel4/libsel4.a -e _sel4_start -o rootserver
```

# How to launch

`qemu-system-x86_64 -enable-kvm -cpu host -m 50M -kernel kernel/kernel.elf -initrd ./rootserver -serial stdio`
