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

void INX (uint8_t* high, uint8_t* low) {
	uint16_t value = (*high << 8) | *low;
	value++;
	*high = (value >> 8) & 0xff;
	*low = value & 0xff;
}

void RRC (State8080 *state) {
	uint8_t bit0 = state->a & 1;
	state->a = (state->a >> 1) | (bit0 << 7);
	state->cc.cy = bit0;
}

void SBB (State8080 *state, uint8_t value) {
	uint8_t carry = state->cc.cy;
	uint16_t answer = state->a - value - carry;

	state->cc.z = ((answer & 0xff) == 0);
	state->cc.s = ((answer & 0x80) != 0);
	state->cc.p = Parity(answer & 0xff);
	state->cc.ac = (((state->a & 0x0F) < (value & 0x0F) + carry));
	state->cc.cy = (answer > 0xff);
	state->a = (uint8_t)(answer & 0xff);
}

void ADC (State8080 *state, uint8_t value) {
	uint16_t answer = state->a + value + state->cc.cy;

	state->cc.z = ((answer & 0xff) == 0);
	state->cc.s = ((answer & 0x80) != 0);
	state->cc.p = Parity(answer & 0xff);
	state->cc.ac = (((state->a & 0x0F) + (value & 0x0F) + state->cc.cy) > 0x0F);
	state->cc.cy = (answer > 0xff);
	state->a = (uint8_t)(answer & 0xff);
}

void ADD (State8080 *state, uint8_t value) {
	uint16_t answer = state->a + value;

	state->cc.z = ((answer & 0xff) == 0);
	state->cc.s = ((answer & 0x80) != 0);
	state->cc.p = Parity(answer & 0xff);
	state->cc.ac = (((state->a & 0x0F) + (value & 0x0F)) > 0x0F);
	state->cc.cy = (answer > 0xff);
	state->a = (uint8_t)(answer & 0xff);
}

void SUB (State8080 *state, uint8_t value) {
	uint16_t answer = state->a - value;

	state->cc.z = ((answer & 0xff) == 0);
	state->cc.s = ((answer & 0x80) != 0);
	state->cc.p = Parity(answer & 0xff);
	state->cc.ac = (((state->a & 0x0F) < (value & 0x0F)));
	state->cc.cy = (answer > 0xff);
	state->a = (uint8_t)(answer & 0xff);
}

void DAD (State8080 *state, uint16_t value) {
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
	 disassemble8080op(state->memory, state->pc);

	switch(*opcode) {
	case 0x00: break; //NOP
	case 0x01:
		state->c = opcode[1];
		state->b = opcode[2];
		state->pc += 2;
		break;
	case 0x02: UnimplementedInstruction(state); break;
	case 0x03: UnimplementedInstruction(state); break;
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
	case 0x07: UninplementedInstruction(state); break;
	case 0x08: UnimplementedInstruction(state); break;
	case 0x09: { 
		uint32_t bc = (state->b << 8) | (state->c);
		DAD(state, bc);
		break;
	}
	case 0x0a: UnimplementedInstruction(state); break;
	case 0x0b: UnimplementedInstruction(state); break;
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
	case 0x0f:
		RRC(state);
		break;
	case 0x10: UnimplementedInstruction(state); break;
	case 0x11: {
		state->e = opcode[1];
		state->d = opcode[2];
		state->pc += 2;
		break;
	}
	case 0x12: UnimplementedInstruction(state); break;
	case 0x13: {
		INX(&state->d, &state->e);
		break;
	}
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
	case 0x16: UnimplementedInstruction(state); break;
	case 0x17: UnimplementedInstruction(state); break;
	case 0x18: UnimplementedInstruction(state); break;
	case 0x19: {
		uint32_t de = (state->d << 8) | (state->e);
		DAD(state, de);
		break;
	}
	case 0x1a: {
		uint16_t address = (state->d << 8) | (state->e);
		state->a = state->memory[address];
		break;
	}
	case 0x1b: UninplementedInstruction(state); break;
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
	case 0x1e: UnimplementedInstruction(state); break;
	case 0x1f: UnimplementedInstruction(state); break;
	case 0x20: UnimplementedInstruction(state); break;
	case 0x21: {
		state->l = opcode[1];
		state->h = opcode[2];
		state->pc += 2;
		break;
	}
	case 0x23: {
		INX(&state->h, &state->l);
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
	case 0x26: {
		state->h = opcode[1];
		state->pc++;
		break;
	}
	case 0x29: {
		uint32_t hl = (state->h << 8) | (state->l);
		DAD(state, hl);
		break;
	}
	case 0x2a: UnimplementedInstruction(state); break;
	case 0x2b: UnimplementedInstruction(state); break;
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
	case 0x2e: UnimplementedInstruction(state); break;
	case 0x2f: UnimplementedInstruction(state); break;
	case 0x30: UnimplementedInstruction(state); break;
	case 0x31: {
		state->sp = (opcode[2] << 8) | opcode[1];
		state->pc += 2;
		break;
	}
	case 0x32: {
		uint16_t address = (opcode[2] << 8) | opcode[1];
		state->memory[address] = state->a;
		state->pc += 2;
		break;
	}
	case 0x33: UnimplementedInstruction(state); break;
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
	case 0x36: {
		uint16_t offset = hl_address(state);
		state->memory[offset] = opcode[1];
		state->pc++;
		break;
	}
	case 0x37: UnimplementedInstruction(state); break;
	case 0x38: UnimplementedInstruction(state); break;
	case 0x39: {
		uint32_t sp = state->sp;
		DAD(state, sp);
		break;
	}
	case 0x3a: {
		uint16_t address = (opcode[2] << 8) | opcode[1];
		state->a = state->memory[address];
		state->pc += 2;
		break;
	}
	case 0x3b: UnimplementedInstruction(state); break;
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
	case 0x3e: { 
		state->a = opcode[1];
		state->pc++;
		break;
	}
	case 0x3f: UnimplementedInstruction(state); break;
	case 0x40: UnimplementedInstruction(state); break;
	case 0x41:
		state->b = state->c;
		break;
	case 0x42:
		state->b = state->d;
		break;
	case 0x43:
		state->b = state->e;
		break;
	case 0x44: UnimplementedInstruction(state); break;
	case 0x45: UnimplementedInstruction(state); break;
	case 0x46: UnimplementedInstruction(state); break;
	case 0x47: UnimplementedInstruction(state); break;
	case 0x48: UnimplementedInstruction(state); break;
	case 0x49: UnimplementedInstruction(state); break;
	case 0x4a: UnimplementedInstruction(state); break;
	case 0x4b: UnimplementedInstruction(state); break;
	case 0x4c: UnimplementedInstruction(state); break;
	case 0x4d: UnimplementedInstruction(state); break;
	case 0x4e: UnimplementedInstruction(state); break;
	case 0x4f: UnimplementedInstruction(state); break;
	case 0x50: UnimplementedInstruction(state); break;
	case 0x51: UnimplementedInstruction(state); break;
	case 0x52: UnimplementedInstruction(state); break;
	case 0x53: UnimplementedInstruction(state); break;
	case 0x54: UnimplementedInstruction(state); break;
	case 0x55: UnimplementedInstruction(state); break;
	case 0x56: {
		uint16_t offset = (state->h << 8) | (state->l);
		state->d = state->memory[offset];
		break;
	}
	case 0x57: UnimplementedInstruction(state); break;
	case 0x58: UnimplementedInstruction(state); break;
	case 0x59: UnimplementedInstruction(state); break;
	case 0x5a: UnimplementedInstruction(state); break;
	case 0x5b: UnimplementedInstruction(state); break;
	case 0x5c: UnimplementedInstruction(state); break;
	case 0x5d: UnimplementedInstruction(state); break;
	case 0x5e: {
		uint16_t offset = (state->h << 8) | (state->l);
		state->e = state->memory[offset];
		break;
	}
	case 0x5f: UnimplementedInstruction(state); break;
	case 0x60: UnimplementedInstruction(state); break;
	case 0x61: UnimplementedInstruction(state); break;
	case 0x62: UnimplementedInstruction(state); break;
	case 0x63: UnimplementedInstruction(state); break;
	case 0x64: UnimplementedInstruction(state); break;
	case 0x65: UnimplementedInstruction(state); break;
	case 0x66: {
		uint16_t offset = (state->h << 8) | (state->l);
		state->h = state->memory[offset];
		break;
	}
	case 0x67: UnimplementedInstruction(state); break;
	case 0x68: UnimplementedInstruction(state); break;
	case 0x69: UnimplementedInstruction(state); break;
	case 0x6a: UnimplementedInstruction(state); break;
	case 0x6b: UnimplementedInstruction(state); break;
	case 0x6c: UnimplementedInstruction(state); break;
	case 0x6d: UnimplementedInstruction(state); break;
	case 0x6e: UnimplementedInstruction(state); break;
	case 0x6f: {
		uint16_t offset = (state->h << 8) | (state->l);
		state->l = state->memory[offset];
		break;
	}
	case 0x70: UnimplementedInstruction(state); break;
	case 0x71: UnimplementedInstruction(state); break;
	case 0x72: UnimplementedInstruction(state); break;
	case 0x73: UnimplementedInstruction(state); break;
	case 0x74: UnimplementedInstruction(state); break;
	case 0x75: UnimplementedInstruction(state); break;
	case 0x76: UnimplementedInstruction(state); break;
	case 0x77: {
		uint16_t offset = (state->h << 8) | (state->l);
		state->memory[offset] = state->a;
		break;
	}
	case 0x80:
		ADD(state, state->b);
		break;
	case 0x81:
		ADD(state, state->c);
		break;
	case 0x82:
		ADD(state, state->d);
		break;
	case 0x83:
		ADD(state, state->e);
		break;
	case 0x84:
		ADD(state, state->h);
		break;
	case 0x85:
		ADD(state, state->l);
		break;
	case 0x86: {
		uint16_t offset = hl_address(state);
		ADD(state, state->memory[offset]);
		break;
	}
	case 0x87:
		ADD(state, state->a);
		break;
	case 0x88:
		ADC(state, state->b);
		break;
	case 0x89:
		ADC(state, state->c);
		break;
	case 0x8a:
		ADC(state, state->d);
		break;
	case 0x8b:
		ADC(state, state->e);
		break;
	case 0x8c:
		ADC(state, state->h);
		break;
	case 0x8d:
		ADC(state, state->l);
		break;
	case 0x8e: {
		uint16_t offset = hl_address(state);
		ADC(state, state->memory[offset]);
		break;
	}
	case 0x8f:
		ADC(state, state->a);
		break;
	case 0x90:
		SUB(state, state->b);
		break;
	case 0x91:
		SUB(state, state->c);
		break;
	case 0x92:
		SUB(state, state->d);
		break;
	case 0x93:
		SUB(state, state->e);
		break;
	case 0x94:
		SUB(state, state->h);
		break;
	case 0x95:
		SUB(state, state->l);
		break;
	case 0x96: {
		uint16_t offset = hl_address(state);
		SUB(state, state->memory[offset]);
		break;
	}
	case 0x97:
		SUB(state, state->a);
		break;
	case 0x98:
		SBB(state, state->b);
		break;
	case 0x99:
		SBB(state, state->c);
		break;
	case 0x9a:
		SBB(state, state->d);
		break;
	case 0x9b:
		SBB(state, state->e);
		break;
	case 0x9c:
		SBB(state, state->h);
		break;
	case 0x9d:
		SBB(state, state->l);
		break;
	case 0x9e: {
		uint16_t offset = hl_address(state);
		SBB(state, state->memory[offset]);
		break;
	}
	case 0x9f:
		SBB(state, state->a);
		break;
	}
	  printf("\tC=%d,P=%d,S=%d,Z=%d\n", state->cc.cy, state->cc.p,    
           state->cc.s, state->cc.z);    
       printf("\tA $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n",    
           state->a, state->b, state->c, state->d,    
           state->e, state->h, state->l, state->sp);  
	state->pc+=1;
	return 4;
	
}