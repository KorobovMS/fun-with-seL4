all: kernel launch

clean:
	rm -rf ./build/

kernel:
	rm -rf ./build/
	mkdir build
	cd build && cmake -DCROSS_COMPILER_PREFIX= -DCMAKE_TOOLCHAIN_FILE=../seL4/gcc.cmake -G Ninja -C ../configs/kernel_cfg.cmake ../seL4/
	cd build && ninja kernel.elf
	python -c "with open('./build/kernel.elf', 'r+b') as f: f.seek(18); f.write(b'\x03');"

launch:
	qemu-system-x86_64 -enable-kvm -cpu host -m 50M -kernel ./build/kernel.elf -serial stdio
