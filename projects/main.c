#include <sel4/sel4.h>

seL4_BootInfo *sel4runtime_bootinfo(void);

// This is a kludge to avoid crash while CRT is initializing and
// is calling ELF constructors.
// sel4runtime provides _init function and the only thing it does
// is opening a stack frame. It doesn't 'ret', but this function is 'call'ed.
// This needs some futher exploration on how gcc/ld work.
// Right now main.o must be after libsel4runtime.a in ld arguments list.
__attribute__ ((section (".init")))
void init()
{
	asm volatile("pop %rbp\n");
}

// __stack_chk_fail, __assert_fail and strcpy are implemented here
// so there is not need in linking with libc library.
void __stack_chk_fail()
{
	seL4_DebugPutChar('s');
	seL4_DebugPutChar('c');
	seL4_DebugPutChar('f');
	seL4_DebugPutChar('\n');
	while (1);
}

void __assert_fail(const char * assertion, const char * file, int line, const char * function)
{
	seL4_DebugPutChar('a');
	seL4_DebugPutChar('f');
	seL4_DebugPutChar('\n');
	while (1);
}

char *strcpy(char *strDest, const char *strSrc)
{
    char *temp = strDest;
    while (*strDest++ = *strSrc++);
    return temp;
}

// This is needed for rootserver tls data storage.
unsigned int _tdata_start[128];
unsigned int _tdata_end[128];
unsigned int _tbss_end[128];

void print(const char *str)
{
	const char *p = str;
	while (*p) {
		seL4_DebugPutChar(*p);
		++p;
	}
}

void print_dec_rec(unsigned long long num)
{
	if (num != 0) {
		print_dec_rec(num / 10);
		seL4_DebugPutChar('0' + (num % 10));
	}
}

void print_dec(unsigned long long num)
{
	if (num == 0)
		seL4_DebugPutChar('0');
	else
		print_dec_rec(num);
}

void print_hex_digit(int num)
{
	if (0 <= num && num <= 9)
		seL4_DebugPutChar('0' + num);
	else if (0xa <= num && num <= 0xf)
		seL4_DebugPutChar('A' - 0xa + num);
}

void print_hex_rec(unsigned long long num)
{
	if (num != 0) {
		print_hex_rec(num / 0x10);
		print_hex_digit(num % 0x10);
	}
}

void print_hex(unsigned long long num)
{
	if (num == 0) {
		print("0x0");
	} else {
		print("0x");
		print_hex_rec(num);
	}
}

void print_cap_type(int cap)
{
    if (cap == 0) { print("cap_null_cap"); return; }
    if (cap == 2) { print("cap_untyped_cap"); return; }
    if (cap == 4) { print("cap_endpoint_cap"); return; }
    if (cap == 6) { print("cap_notification_cap"); return; }
    if (cap == 8) { print("cap_reply_cap"); return; }
    if (cap == 10) { print("cap_cnode_cap"); return; }
    if (cap == 12) { print("cap_thread_cap"); return; }
    if (cap == 14) { print("cap_irq_control_cap"); return; }
    if (cap == 16) { print("cap_irq_handler_cap"); return; }
    if (cap == 18) { print("cap_zombie_cap"); return; }
    if (cap == 20) { print("cap_domain_cap"); return; }
    if (cap == 1) { print("cap_frame_cap"); return; }
    if (cap == 3) { print("cap_page_table_cap"); return; }
    if (cap == 5) { print("cap_page_directory_cap"); return; }
    if (cap == 7) { print("cap_pdpt_cap"); return; }
    if (cap == 9) { print("cap_pml4_cap"); return; }
    if (cap == 11) { print("cap_asid_control_cap"); return; }
    if (cap == 13) { print("cap_asid_pool_cap"); return; }
    if (cap == 15) { print("cap_io_space_cap"); return; }
    if (cap == 17) { print("cap_io_page_table_cap"); return; }
    if (cap == 19) { print("cap_io_port_cap"); return; }
    if (cap == 31) { print("cap_io_port_control_cap"); return; }
    print("?????");
}

void panic(const char *str)
{
	print("Panic!!!\n");
	print(str);
	while (1);
}

unsigned char thread1_stack[0x1000];
unsigned char thread1_ipc_buffer[32];

void thread1_proc()
{
	print("hello from thread #1\n");
	while (1) {
		seL4_Yield();
	}
}

unsigned char thread2_stack[0x1000];
unsigned char thread2_ipc_buffer[32];

void print_vga(char c, char fg, char bg, int x, int y)
{
	seL4_Word vga_vaddr = 0x4f0000;
	unsigned char *pos = (unsigned char *)vga_vaddr + 2*(x*80 + y);
	*pos = c;
	*(pos + 1) = (bg << 4) | fg;
}

void clear_screen()
{
	for (int x = 0; x < 25; ++x) {
		for (int y = 0; y < 80; ++y) {
			print_vga(' ', 0, 0, x, y);
		}
	}
}

void thread2_proc()
{
	char str[] = "rainbow";
	char fgs[] = { 15, 15, 0, 0, 0, 15, 15 };
	char bgs[] = { 4, 6, 14, 2, 11, 1, 5 };
	clear_screen();
	for (int i = 0; i < sizeof(str) - 1; ++i)
		print_vga(str[i], fgs[i], bgs[i], 0, i);
	while (1);
}

void print_system_info(seL4_BootInfo *bootinfo)
{
	print("Untyped [");
	print_dec(bootinfo->untyped.start);
	print(", ");
	print_dec(bootinfo->untyped.end);
	print(")\n");
	print("Untyped list\n");
	for (int i = 0; i < bootinfo->untyped.end - bootinfo->untyped.start; ++i) {
		print_dec(i);
		print("\t");
		print_hex(bootinfo->untypedList[i].paddr);
		print("\t");
		print("2^");
		print_dec(bootinfo->untypedList[i].sizeBits);
		print("\t");
		if (bootinfo->untypedList[i].isDevice) {
			print("dev");
		} else {
			print("mem");
		}
		print("\n");
	}
	print("Cap info\n");
	for (int i = 0; i < 200; ++i) {
		print_dec(i);
		print("\t");
		print_cap_type(seL4_DebugCapIdentify(i));
		print("\n");
	}
}

int main(int argc, char **argv)
{
	seL4_UserContext ctx = { 0 };
	seL4_BootInfo *bootinfo = sel4runtime_bootinfo();
	seL4_Word thread1 = bootinfo->untyped.end;
	seL4_Word thread2 = bootinfo->untyped.end + 1;
	seL4_Word first_pages_caps = bootinfo->untyped.end + 2;
	seL4_Word vga_page_cap = first_pages_caps + 0xb8000 / 0x1000;
	seL4_Word vga_vaddr = 0x4f0000;

	if (seL4_Untyped_Retype(
			bootinfo->untyped.end - 1,
			seL4_TCBObject,
			seL4_TCBBits,
			seL4_CapInitThreadCNode,
			0,
			0,
			bootinfo->untyped.end,
			2))
		panic("cannot retype\n");

	if (seL4_Untyped_Retype(
			bootinfo->untyped.start,
			seL4_X86_4K,
			seL4_PageBits,
			seL4_CapInitThreadCNode,
			0,
			0,
			first_pages_caps,
			256))
		panic("cannot retype memory\n");
	if (seL4_X86_Page_Map(
			vga_page_cap,
			seL4_CapInitThreadVSpace,
			vga_vaddr,
			seL4_CapRights_new(0, 0, 1, 1),
			seL4_X86_Default_VMAttributes))
		panic("cannot map\n");

	print_system_info(bootinfo);

	if (seL4_TCB_Configure(
			thread1,
			0,
			seL4_CapInitThreadCNode,
			0,
			seL4_CapInitThreadVSpace,
			0,
			0,
			0))
		panic("cannot configure 1\n");
	if (seL4_TCB_Configure(
			thread2,
			0,
			seL4_CapInitThreadCNode,
			0,
			seL4_CapInitThreadVSpace,
			0,
			0,
			0))
		panic("cannot configure 2\n");

	ctx.rip = (seL4_Word)(thread1_proc);
	ctx.rsp = (seL4_Word)(thread1_stack + sizeof(thread1_stack));
	if (seL4_TCB_WriteRegisters(
			thread1,
			0,
			0,
			2,
			&ctx))
		panic("cannot write registers 1\n");
	ctx.rip = (seL4_Word)(thread2_proc);
	ctx.rsp = (seL4_Word)(thread2_stack + sizeof(thread2_stack));
	if (seL4_TCB_WriteRegisters(
			thread2,
			0,
			0,
			2,
			&ctx))
		panic("cannot write registers 2\n");
	if (seL4_TCB_SetPriority(
			thread1,
			seL4_CapInitThreadTCB,
			255))
		panic("cannot set priority 1\n");
	if (seL4_TCB_SetPriority(
			thread2,
			seL4_CapInitThreadTCB,
			255))
		panic("cannot set priority 2\n");

	if (seL4_TCB_SetAffinity(
			thread1,
			0))
		panic("cannot set affinity 1\n");
	if (seL4_TCB_SetAffinity(
			thread2,
			0))
		panic("cannot set affinity 2\n");

	seL4_DebugDumpScheduler();

	if (seL4_TCB_Resume(thread1))
		panic("cannot resume 1\n");
	if (seL4_TCB_Resume(thread2))
		panic("cannot resume 2\n");

	print("hello from rootserver thread\n");

	seL4_DebugDumpScheduler();

	while (1) {
		seL4_Yield();
	}
	return 0;
}
