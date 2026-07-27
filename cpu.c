#include "cpu.h"
#include <stdio.h>
#include <stdint.h>

void UnimplementedInstruction(State8080 *state) {
	printf("Error: Unimplemented instruction\n");
	exit(1);
}

int Emulate8080Op(State8080 *state) {
	
	unsigned char *opcode = &state->memory[state->pc];

	switch(*opcode) {
	case 0x00: break; //NOP
	case 0x01:
		state->c = opcode[1];
		state->b = opcode[2];
		state->pc += 2;
		break;
	case 0x04:
		state->b++;
		if (state->b == 0) {
			state->cc.z = 1;
		}
		else {
			state->cc.z = 0;
		}
	case 0x06:
		state->b = opcode[1];
		state->pc++;
		break;
	case 0x0e:
		state->c = opcode[1];
		state->pc++;
		break;
	case 0x41:
		state->b = state->c;
		break;
	case 0x42:
		state->b = state->d;
		break;
	case 0x43:
		state->b = state->e;
		break;
	}
	state->pc+=1;
}