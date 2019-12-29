/*
 * returns 0 if gcc variable attribute pack works as expected
 * returns -1 else
 */
#include <stdint.h>

struct test {
	uint8_t one;
	uint16_t two;
} __attribute__ ((packed));

int main(int argc, char **argv) {
	return (sizeof(struct test) != 3) ? -1 : 0;
}
