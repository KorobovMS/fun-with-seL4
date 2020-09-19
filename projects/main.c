void seL4_DebugPutChar(char c);
int main()
{
	char hello[] = "Hello, world!\n";
	char *p = hello;
	while (*p) {
		seL4_DebugPutChar(*p);
		++p;
	}
	return 0;
}