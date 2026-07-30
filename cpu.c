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

void DAD (State8080 *state, uint32_t value) {
	uint32_t hl = (state->h << 8) | state->l;
	uint32_t answer = hl + value;

	state->cc.cy = ((answer & 0xffff0000) > 0);

	state->h = (answer & 0xff00) >> 8;
	state->l = (answer & 0xff);
}

uint16_t hl_address(State8080 *state) {
	return (state->h << 8) | state->l;
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
	case 0x04: { 
		uint8_t answer = state->b + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->b & 0x0F) == 0x0F);
			state->b = answer;
			break;
	}
	case 0x05: { 
		uint8_t answer = state->b - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->b & 0x0F) == 0x0F);
			state->b = answer;
			break;
	}
	case 0x06:
		state->b = opcode[1];
		state->pc++;
		break;
	case 0x09: { 
		uint32_t bc = (state->b << 8) | (state->c);
		DAD(state, bc);
		break;
	}
	case 0x0c: { 
		uint8_t answer = state->c + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->c & 0x0F) == 0x0F);
			state->c = answer;
			break;
	}
	case 0x0d: { 
		uint8_t answer = state->c - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->c & 0x0F) == 0x0F);
			state->c = answer;
			break;
	}
	case 0x0e:
		state->c = opcode[1];
		state->pc++;
		break;
	case 0x14: { 
		uint8_t answer = state->d + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->d & 0x0F) == 0x0F);
			state->d = answer;
			break;
	}
	case 0x15: { 
		uint8_t answer = state->d - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->d & 0x0F) == 0x0F);
			state->d = answer;
			break;
	}
	case 0x1c: { 
		uint8_t answer = state->e + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->e & 0x0F) == 0x0F);
			state->e = answer;
			break;
	}
	case 0x1d: {
		uint8_t answer = state->e - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->e & 0x0F) == 0x0F);
			state->e = answer;
			break;
	}
	case 0x24: {
		uint8_t answer = state->h + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->h & 0x0F) == 0x0F);
			state->h = answer;
			break;
	}
	case 0x25: {
		uint8_t answer = state->h - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->h & 0x0F) == 0x0F);
			state->h = answer;
			break;
	}
	case 0x2c: { 
		uint8_t answer = state->l + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->l & 0x0F) == 0x0F);
			state->l = answer;
			break;
	}
	case 0x2d: { 
		uint8_t answer = state->l - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->l & 0x0F) == 0x0F);
			state->l = answer;
			break;
	}
	case 0x34: {
		uint16_t offset = hl_address(state);
		uint8_t answer = state->memory[offset] + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->memory[offset] & 0x0F) == 0x0F);
			state->memory[offset] = answer;
			break;
	}
	case 0x35: {
		uint16_t offset = hl_address(state);
		uint8_t answer = state->memory[offset] - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->memory[offset] & 0x0F) == 0x0F);
			state->memory[offset] = answer;
			break;
	}
	case 0x3c: { 
		uint8_t answer = state->a + 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->a & 0x0F) == 0x0F);
			state->a = answer;
			break;
	}
	case 0x3d: {
		uint8_t answer = state->a - 1;
			state->cc.z = ((answer & 0xff) == 0);
			state->cc.s = ((answer & 0x80) != 0);
			state->cc.p = Parity(answer&0xff);
			state->cc.ac = ((state->a & 0x0F) == 0x0F);
			state->a = answer;
			break;
	}
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
	return 4;
}