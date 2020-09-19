all: kernel launch

clean:
	rm -rf ./build/

kernel:
	rm -rf ./build/
	mkdir build
	cd build && cmake -DCROSS_COMPILER_PREFIX= -DCMAKE_TOOLCHAIN_FILE=../seL4/gcc.cmake -G Ninja -C ../configs/kernel_cfg.cmake ../seL4/
	cd build && ninja kernel.elf
	objcopy -I elf64-x86-64 -O elf32-i386 ./build/kernel.elf ./build/32.elf

launch:
	qemu-system-x86_64 -enable-kvm -cpu host -m 50M -kernel ./build/32.elf -serial stdio
