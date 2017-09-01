asm(".code16");
int test = 3;
int kernel_start(void) {
	return test;
}