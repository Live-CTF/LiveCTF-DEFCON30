// gcc nopcoding.c -O0 -no-pie -o nopcoding -lcapstone -m32

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <capstone/capstone.h>

void win(void) {
	system("/bin/sh");
}

int main(void) {
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	char data[0x1000];
	printf("Enter up to 0x%zx bytes of x86_64 nop instructions:\n", (size_t)(sizeof(data) / sizeof(char)));
	fgets(data, 0xfff, stdin);
	size_t length = strcspn(data, "\n");
	data[length] = 0;

	csh handle;
	cs_insn *insn;
	size_t count;

	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
		return -1;
	count = cs_disasm(handle, (const uint8_t *)data, length, 0xfff, 0, &insn);
	if (count > 0) {
		size_t j;
		for (j = 0; j < count; j++) {
			printf("%s %s\n", insn[j].mnemonic, insn[j].op_str);
			if (strcmp(insn[j].mnemonic, "nop") != 0) {
				printf("You provided something that wasn't a nop!\n");
				return -1;
			}
		}

		cs_free(insn, count);
	} else {
		printf("Failed to disassemble!\n");
		return -1;
	}

	cs_close(&handle);

	printf("Running your nops! Nothing should happen!\n");

	data[length] = 0xc3;

	void *ptr = mmap(0, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (ptr == MAP_FAILED) {
		printf("mmap failed\n");
		return -1;
	}
	memcpy(ptr, data, 0x1000);

	((void(*)())ptr)();

	printf("Finished running nops! Hopefully nothing happened!\n");

	return 0;
}