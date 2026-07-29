#include "cpu.h"
#include <stdio.h>
#include <stdint.h>

int Parity(uint8_t value) {
	int count = 0;
	for (int i = 0; i < 8; i++) {
		if (value & (1 << i)) {
			count++;
		}
	}

	return (count % 2) == 0;
}

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
		uint8_t answer = state->b + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->b & 0x0F) == 0x0F);
			state->b = answer;
			break;
	case 0x05:
		uint8_t answer = state->b - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->b & 0x0F) == 0x0F);
			state->b = answer;
			break;
	case 0x06:
		state->b = opcode[1];
		state->pc++;
		break;
	case 0x0c:
		uint8_t answer = state->c + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->c & 0x0F) == 0x0F);
			state->c = answer;
	case 0x0d:
		uint8_t answer = state->c - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->c & 0x0F) == 0x0F);
			state->c = answer;
	case 0x0e:
		state->c = opcode[1];
		state->pc++;
		break;
	case 0x14:
		uint8_t answer = state->d + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->d & 0x0F) == 0x0F);
			state->d = answer;
	case 0x15:
		uint8_t answer = state->d - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->d & 0x0F) == 0x0F);
			state->d = answer;
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