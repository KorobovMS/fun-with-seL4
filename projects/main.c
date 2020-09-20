void seL4_DebugPutChar(char c);

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

void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function)
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

int main(int argc, char **argv)
{
	print("Hello, world!\n");
	while (1);
	return 0;
}
