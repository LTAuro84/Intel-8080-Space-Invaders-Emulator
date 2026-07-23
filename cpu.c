#include "cpu.h"
#include <stdio.h>
#include <stdint.h>

void UnimplementedInstruction(State8080 *state) {
	printf("Error: Unimplemented instruction\n");
	exit(1);
}

int Emulate8080Op(State8080 *state)