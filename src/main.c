#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"

int main() {
    stdio_init_all();

	printf("hello, world!\n");
	while(1) {
		printf("1s\n");
		sleep_ms(1000);
	}
}
