/*    *codebuffer is a valid pointer to 8080 assembly code    
    pc is the current offset into the code    

    returns the number of bytes of the op

    step 1: read code into buffer
    step 2: get a pointer to the beginning of the buffer
    step 3: use the byte at the pointer to determine the opcode
    step 4: print out the name of the opcode
    step 5: advance the counter by the bytes used by the instruction
    go to step 3 if not at the end of the buffer 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct ConditionCodes{
	uint8_t    z:1;
	uint8_t    s:1;
	uint8_t    p:1;
	uint8_t    cy:1;
	uint8_t    ac:1;
	uint8_t    pad:3;
}ConditionCodes;

typedef struct State8080{
	uint8_t    a;    //registers
    uint8_t    b;    
    uint8_t    c;    
    uint8_t    d;    
    uint8_t    e;    
    uint8_t    h;    
    uint8_t    l;    8
    uint16_t    sp;    
    uint16_t    pc;    //program counter
    uint8_t     *memory;    // simulate ram
    struct      ConditionCodes      cc;    
    uint8_t     int_enable; 
}State8080;

void UnimplementedInstruction(State8080* state){
	//pc will have advanced one, so undo
	printf("Error: Unimplemented instruction\n");
	exit(1);
}

int Emulate8080Op(State8080* state){
	unsigned char *opcode = &state->memory[state->pc];

	switch(*opcode){
		case 0x00:  break;
		case 0x01:	state->c = opcode[1];
					state->b = opcode[2];
					state->pc += 2; break;
		case 0x02:	uint16_t bc = state->b << 8 | state->c;
					state->memory[bc] = state->a;
					 break;
		case 0x03:	uint16_t bc = state->b << 8 | state->c;
					bc += 1;
					state->b = (bc & 0xffff) >> 8;
					state->c = (bc & 0xff);
					break;
		case 0x04: UnimplementedInstruction(state); break;
		case 0x05: 							//DCR    B
			{
			uint8_t res = state->b - 1;
			state->cc.z = (res == 0);
			state->cc.s = (0x80 == (res & 0x80));
			state->cc.p = parity(res, 8);
			state->b = res;
			}
			break;
		case 0x06: 							//MVI B,byte
			state->b = opcode[1];
			state->pc++;
			break;
		case 0x07: UnimplementedInstruction(state); break;
		case 0x08: UnimplementedInstruction(state); break;
		case 0x09: 							//DAD B
			{
			uint32_t hl = (state->h << 8) | state->l;
			uint32_t bc = (state->b << 8) | state->c;
			uint32_t res = hl + bc;
			state->h = (res & 0xff00) >> 8;
			state->l = res & 0xff;
			state->cc.cy = ((res & 0xffff0000) > 0);
			}
			break;
		case 0x0a: UnimplementedInstruction(state); break;
		case 0x0b: UnimplementedInstruction(state); break;
		case 0x0c: UnimplementedInstruction(state); break;
		case 0x0d: 							//DCR    C
			{
			uint8_t res = state->c - 1;
			state->cc.z = (res == 0);
			state->cc.s = (0x80 == (res & 0x80));
			state->cc.p = parity(res, 8);
			state->c = res;
			}
			break;
		case 0x0e: 							//MVI C,byte
			state->c = opcode[1];
			state->pc++;
			break;
		case 0x0f: 							//RRC
			{
				uint8_t x = state->a;
				state->a = ((x & 1) << 7) | (x >> 1);
				state->cc.cy = (1 == (x&1));
			}
			break;
		case 0x10: UnimplementedInstruction(state); break;
		case 0x11: 							//LXI	D,word
			state->e = opcode[1];
			state->d = opcode[2];
			state->pc += 2;
			break;
		case 0x12: UnimplementedInstruction(state); break;
		case 0x13: 							//INX    D
			state->e++;
			if (state->e == 0)
				state->d++;
			break;		
		case 0x14: UnimplementedInstruction(state); break;
		case 0x15: UnimplementedInstruction(state); break;
		case 0x16: UnimplementedInstruction(state); break;
		case 0x17: UnimplementedInstruction(state); break;
		case 0x18: UnimplementedInstruction(state); break;
		case 0x19: 							//DAD    D
			{
			uint32_t hl = (state->h << 8) | state->l;
			uint32_t de = (state->d << 8) | state->e;
			uint32_t res = hl + de;
			state->h = (res & 0xff00) >> 8;
			state->l = res & 0xff;
			state->cc.cy = ((res & 0xffff0000) != 0);
			}
			break;
		case 0x1a: 							//LDAX	D
			{
			uint16_t offset=(state->d<<8) | state->e;
			state->a = state->memory[offset];
			}
			break;
		case 0x1b: UnimplementedInstruction(state); break;
		case 0x1c: UnimplementedInstruction(state); break;
		case 0x1d: UnimplementedInstruction(state); break;
		case 0x1e: UnimplementedInstruction(state); break;
		case 0x1f: UnimplementedInstruction(state); break;
		case 0x20: UnimplementedInstruction(state); break;
		case 0x21: 							//LXI	H,word
			state->l = opcode[1];
			state->h = opcode[2];
			state->pc += 2;
			break;
		case 0x22: UnimplementedInstruction(state); break;
		case 0x23: 							//INX    H
			state->l++;
			if (state->l == 0)
				state->h++;
			break;		
		case 0x24: UnimplementedInstruction(state); break;
		case 0x25: UnimplementedInstruction(state); break;
		case 0x26:  							//MVI H,byte
			state->h = opcode[1];
			state->pc++;
			break;
		case 0x27: UnimplementedInstruction(state); break;
		case 0x28: UnimplementedInstruction(state); break;
		case 0x29: 								//DAD    H
			{
			uint32_t hl = (state->h << 8) | state->l;
			uint32_t res = hl + hl;
			state->h = (res & 0xff00) >> 8;
			state->l = res & 0xff;
			state->cc.cy = ((res & 0xffff0000) != 0);
			}
			break;
		case 0x2a: UnimplementedInstruction(state); break;
		case 0x2b: UnimplementedInstruction(state); break;
		case 0x2c: UnimplementedInstruction(state); break;
		case 0x2d: UnimplementedInstruction(state); break;
		case 0x2e: UnimplementedInstruction(state); break;
		case 0x2f: UnimplementedInstruction(state); break;
		case 0x30: UnimplementedInstruction(state); break;
		case 0x31: 							//LXI	SP,word
			state->sp = (opcode[2]<<8) | opcode[1];
			state->pc += 2;
			break;
		case 0x32: 							//STA    (word)
			{
			uint16_t offset = (opcode[2]<<8) | (opcode[1]);
			state->memory[offset] = state->a;
			state->pc += 2;
			}
			break;
		case 0x33: UnimplementedInstruction(state); break;
		case 0x34: UnimplementedInstruction(state); break;
		case 0x35: UnimplementedInstruction(state); break;
		case 0x36: 							//MVI	M,byte
			{					
			//AC set if lower nibble of h was zero prior to dec
			uint16_t offset = (state->h<<8) | state->l;
			state->memory[offset] = opcode[1];
			state->pc++;
			}
			break;
		case 0x37: UnimplementedInstruction(state); break;
		case 0x38: UnimplementedInstruction(state); break;
		case 0x39: UnimplementedInstruction(state); break;
		case 0x3a: 							//LDA    (word)
			{
			uint16_t offset = (opcode[2]<<8) | (opcode[1]);
			state->a = state->memory[offset];
			state->pc+=2;
			}
			break;
		case 0x3b: UnimplementedInstruction(state); break;
		case 0x3c: UnimplementedInstruction(state); break;
		case 0x3d: UnimplementedInstruction(state); break;
		case 0x3e: 							//MVI    A,byte
			state->a = opcode[1];
			state->pc++;
			break;
		case 0x3f: UnimplementedInstruction(state); break;
		case 0x40: UnimplementedInstruction(state); break;
		case 0x41: UnimplementedInstruction(state); break;
		case 0x42: UnimplementedInstruction(state); break;
		case 0x43: UnimplementedInstruction(state); break;
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
		case 0x56: 							//MOV D,M
			{
			uint16_t offset = (state->h<<8) | (state->l);
			state->d = state->memory[offset];
			}
			break;
		case 0x57: UnimplementedInstruction(state); break;
		case 0x58: UnimplementedInstruction(state); break;
		case 0x59: UnimplementedInstruction(state); break;
		case 0x5a: UnimplementedInstruction(state); break;
		case 0x5b: UnimplementedInstruction(state); break;
		case 0x5c: UnimplementedInstruction(state); break;
		case 0x5d: UnimplementedInstruction(state); break;
		case 0x5e: 							//MOV E,M
			{
			uint16_t offset = (state->h<<8) | (state->l);
			state->e = state->memory[offset];
			}
			break;
		case 0x5f: UnimplementedInstruction(state); break;
		case 0x60: UnimplementedInstruction(state); break;
		case 0x61: UnimplementedInstruction(state); break;
		case 0x62: UnimplementedInstruction(state); break;
		case 0x63: UnimplementedInstruction(state); break;
		case 0x64: UnimplementedInstruction(state); break;
		case 0x65: UnimplementedInstruction(state); break;
		case 0x66: 							//MOV H,M
			{
			uint16_t offset = (state->h<<8) | (state->l);
			state->h = state->memory[offset];
			}
			break;
		case 0x67: UnimplementedInstruction(state); break;
		case 0x68: UnimplementedInstruction(state); break;
		case 0x69: UnimplementedInstruction(state); break;
		case 0x6a: UnimplementedInstruction(state); break;
		case 0x6b: UnimplementedInstruction(state); break;
		case 0x6c: UnimplementedInstruction(state); break;
		case 0x6d: UnimplementedInstruction(state); break;
		case 0x6e: UnimplementedInstruction(state); break;
		case 0x6f: state->l = state->a; break; //MOV L,A
		case 0x70: UnimplementedInstruction(state); break;
		case 0x71: UnimplementedInstruction(state); break;
		case 0x72: UnimplementedInstruction(state); break;
		case 0x73: UnimplementedInstruction(state); break;
		case 0x74: UnimplementedInstruction(state); break;
		case 0x75: UnimplementedInstruction(state); break;
		case 0x76: UnimplementedInstruction(state); break;
		case 0x77: 							//MOV    M,A
			{
			uint16_t offset = (state->h<<8) | (state->l);
			state->memory[offset] = state->a;
			}
			break;
		case 0x78: UnimplementedInstruction(state); break;
		case 0x79: UnimplementedInstruction(state); break;
		case 0x7a: state->a  = state->d;  break;	//MOV D,A
		case 0x7b: state->a  = state->e;  break;	//MOV E,A
		case 0x7c: state->a  = state->h;  break;	//MOV H,A
		case 0x7d: UnimplementedInstruction(state); break;
		case 0x7e: 							//MOV A,M
			{
			uint16_t offset = (state->h<<8) | (state->l);
			state->a = state->memory[offset];
			}
			break;
		case 0x7f: UnimplementedInstruction(state); break;
		case 0x80: UnimplementedInstruction(state); break;
		case 0x81: UnimplementedInstruction(state); break;
		case 0x82: UnimplementedInstruction(state); break;
		case 0x83: UnimplementedInstruction(state); break;
		case 0x84: UnimplementedInstruction(state); break;
		case 0x85: UnimplementedInstruction(state); break;
		case 0x86: UnimplementedInstruction(state); break;
		case 0x87: UnimplementedInstruction(state); break;
		case 0x88: UnimplementedInstruction(state); break;
		case 0x89: UnimplementedInstruction(state); break;
		case 0x8a: UnimplementedInstruction(state); break;
		case 0x8b: UnimplementedInstruction(state); break;
		case 0x8c: UnimplementedInstruction(state); break;
		case 0x8d: UnimplementedInstruction(state); break;
		case 0x8e: UnimplementedInstruction(state); break;
		case 0x8f: UnimplementedInstruction(state); break;
		case 0x90: UnimplementedInstruction(state); break;
		case 0x91: UnimplementedInstruction(state); break;
		case 0x92: UnimplementedInstruction(state); break;
		case 0x93: UnimplementedInstruction(state); break;
		case 0x94: UnimplementedInstruction(state); break;
		case 0x95: UnimplementedInstruction(state); break;
		case 0x96: UnimplementedInstruction(state); break;
		case 0x97: UnimplementedInstruction(state); break;
		case 0x98: UnimplementedInstruction(state); break;
		case 0x99: UnimplementedInstruction(state); break;
		case 0x9a: UnimplementedInstruction(state); break;
		case 0x9b: UnimplementedInstruction(state); break;
		case 0x9c: UnimplementedInstruction(state); break;
		case 0x9d: UnimplementedInstruction(state); break;
		case 0x9e: UnimplementedInstruction(state); break;
		case 0x9f: UnimplementedInstruction(state); break;
		case 0xa0: UnimplementedInstruction(state); break;
		case 0xa1: UnimplementedInstruction(state); break;
		case 0xa2: UnimplementedInstruction(state); break;
		case 0xa3: UnimplementedInstruction(state); break;
		case 0xa4: UnimplementedInstruction(state); break;
		case 0xa5: UnimplementedInstruction(state); break;
		case 0xa6: UnimplementedInstruction(state); break;
		case 0xa7: state->a = state->a & state->a; LogicFlagsA(state);	break; //ANA A
		case 0xa8: UnimplementedInstruction(state); break;
		case 0xa9: UnimplementedInstruction(state); break;
		case 0xaa: UnimplementedInstruction(state); break;
		case 0xab: UnimplementedInstruction(state); break;
		case 0xac: UnimplementedInstruction(state); break;
		case 0xad: UnimplementedInstruction(state); break;
		case 0xae: UnimplementedInstruction(state); break;
		case 0xaf: state->a = state->a ^ state->a; LogicFlagsA(state);	break; //XRA A
		case 0xb0: UnimplementedInstruction(state); break;
		case 0xb1: UnimplementedInstruction(state); break;
		case 0xb2: UnimplementedInstruction(state); break;
		case 0xb3: UnimplementedInstruction(state); break;
		case 0xb4: UnimplementedInstruction(state); break;
		case 0xb5: UnimplementedInstruction(state); break;
		case 0xb6: UnimplementedInstruction(state); break;
		case 0xb7: UnimplementedInstruction(state); break;
		case 0xb8: UnimplementedInstruction(state); break;
		case 0xb9: UnimplementedInstruction(state); break;
		case 0xba: UnimplementedInstruction(state); break;
		case 0xbb: UnimplementedInstruction(state); break;
		case 0xbc: UnimplementedInstruction(state); break;
		case 0xbd: UnimplementedInstruction(state); break;
		case 0xbe: UnimplementedInstruction(state); break;
		case 0xbf: UnimplementedInstruction(state); break;
		case 0xc0: UnimplementedInstruction(state); break;
		case 0xc1: 						//POP    B
			{
				state->c = state->memory[state->sp];
				state->b = state->memory[state->sp+1];
				state->sp += 2;
			}
			break;
		case 0xc2: 						//JNZ address
			if (0 == state->cc.z)
				state->pc = (opcode[2] << 8) | opcode[1];
			else
				state->pc += 2;
			break;
		case 0xc3:						//JMP address
			state->pc = (opcode[2] << 8) | opcode[1];
			break;
		case 0xc4: UnimplementedInstruction(state); break;
		case 0xc5: 						//PUSH   B
			{
			state->memory[state->sp-1] = state->b;
			state->memory[state->sp-2] = state->c;
			state->sp = state->sp - 2;
			}
			break;
		case 0xc6: 						//ADI    byte
			{
			uint16_t x = (uint16_t) state->a + (uint16_t) opcode[1];
			state->cc.z = ((x & 0xff) == 0);
			state->cc.s = (0x80 == (x & 0x80));
			state->cc.p = parity((x&0xff), 8);
			state->cc.cy = (x > 0xff);
			state->a = (uint8_t) x;
			state->pc++;
			}
			break;
		case 0xc7: UnimplementedInstruction(state); break;
		case 0xc8: UnimplementedInstruction(state); break;
		case 0xc9: 						//RET
			state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
			state->sp += 2;
			break;
		case 0xca: UnimplementedInstruction(state); break;
		case 0xcb: UnimplementedInstruction(state); break;
		case 0xcc: UnimplementedInstruction(state); break;
		case 0xcd: 						//CALL adr
			{
			uint16_t	ret = state->pc+2;
			state->memory[state->sp-1] = (ret >> 8) & 0xff;
			state->memory[state->sp-2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (opcode[2] << 8) | opcode[1];
			}
 			break;
		case 0xce: UnimplementedInstruction(state); break;
		case 0xcf: UnimplementedInstruction(state); break;
		case 0xd0: UnimplementedInstruction(state); break;
		case 0xd1: 						//POP    D
			{
				state->e = state->memory[state->sp];
				state->d = state->memory[state->sp+1];
				state->sp += 2;
			}
			break;
		case 0xd2: UnimplementedInstruction(state); break;
		case 0xd3: 
			//Don't know what to do here (yet)
			state->pc++;
			break;
		case 0xd4: UnimplementedInstruction(state); break;
		case 0xd5: 						//PUSH   D
			{
			state->memory[state->sp-1] = state->d;
			state->memory[state->sp-2] = state->e;
			state->sp = state->sp - 2;
			}
			break;
		case 0xd6: UnimplementedInstruction(state); break;
		case 0xd7: UnimplementedInstruction(state); break;
		case 0xd8: UnimplementedInstruction(state); break;
		case 0xd9: UnimplementedInstruction(state); break;
		case 0xda: UnimplementedInstruction(state); break;
		case 0xdb: UnimplementedInstruction(state); break;
		case 0xdc: UnimplementedInstruction(state); break;
		case 0xdd: UnimplementedInstruction(state); break;
		case 0xde: UnimplementedInstruction(state); break;
		case 0xdf: UnimplementedInstruction(state); break;
		case 0xe0: UnimplementedInstruction(state); break;
		case 0xe1: 					//POP    H
			{
				state->l = state->memory[state->sp];
				state->h = state->memory[state->sp+1];
				state->sp += 2;
			}
			break;
		case 0xe2: UnimplementedInstruction(state); break;
		case 0xe3: UnimplementedInstruction(state); break;
		case 0xe4: UnimplementedInstruction(state); break;
		case 0xe5: 						//PUSH   H
			{
			state->memory[state->sp-1] = state->h;
			state->memory[state->sp-2] = state->l;
			state->sp = state->sp - 2;
			}
			break;
		case 0xe6: 						//ANI    byte
			{
			state->a = state->a & opcode[1];
			LogicFlagsA(state);
			state->pc++;
			}
			break;
		case 0xe7: UnimplementedInstruction(state); break;
		case 0xe8: UnimplementedInstruction(state); break;
		case 0xe9: UnimplementedInstruction(state); break;
		case 0xea: UnimplementedInstruction(state); break;
		case 0xeb: 					//XCHG
			{
				uint8_t save1 = state->d;
				uint8_t save2 = state->e;
				state->d = state->h;
				state->e = state->l;
				state->h = save1;
				state->l = save2;
			}
			break;
		case 0xec: UnimplementedInstruction(state); break;
		case 0xed: UnimplementedInstruction(state); break;
		case 0xee: UnimplementedInstruction(state); break;
		case 0xef: UnimplementedInstruction(state); break;
		case 0xf0: UnimplementedInstruction(state); break;
		case 0xf1: 					//POP PSW
			{
				state->a = state->memory[state->sp+1];
				uint8_t psw = state->memory[state->sp];
				state->cc.z  = (0x01 == (psw & 0x01));
				state->cc.s  = (0x02 == (psw & 0x02));
				state->cc.p  = (0x04 == (psw & 0x04));
				state->cc.cy = (0x05 == (psw & 0x08));
				state->cc.ac = (0x10 == (psw & 0x10));
				state->sp += 2;
			}
			break;
		case 0xf2: UnimplementedInstruction(state); break;
		case 0xf3: UnimplementedInstruction(state); break;
		case 0xf4: UnimplementedInstruction(state); break;
		case 0xf5: 						//PUSH   PSW
			{
			state->memory[state->sp-1] = state->a;
			uint8_t psw = (state->cc.z |
							state->cc.s << 1 |
							state->cc.p << 2 |
							state->cc.cy << 3 |
							state->cc.ac << 4 );
			state->memory[state->sp-2] = psw;
			state->sp = state->sp - 2;
			}
			break;
		case 0xf6: UnimplementedInstruction(state); break;
		case 0xf7: UnimplementedInstruction(state); break;
		case 0xf8: UnimplementedInstruction(state); break;
		case 0xf9: UnimplementedInstruction(state); break;
		case 0xfa: UnimplementedInstruction(state); break;
		case 0xfb: state->int_enable = 1;  break;	//EI
		case 0xfc: UnimplementedInstruction(state); break;
		case 0xfd: UnimplementedInstruction(state); break;
		case 0xfe: 						//CPI  byte
			{
			uint8_t x = state->a - opcode[1];
			state->cc.z = (x == 0);
			state->cc.s = (0x80 == (x & 0x80));
			state->cc.p = parity(x, 8);
			state->cc.cy = (state->a < opcode[1]);
			state->pc++;
			}
			break;
		case 0xff: UnimplementedInstruction(state); break;
	}
	state->pc += 1;

 }

int Disassembler8080Op(unsigned char *codebuffer, int pc){

    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf ("%04x ", pc);
    switch(*code){
        case 0x00: printf("NOP\n"); break;
        case 0x01: printf("LXI      B,#$%02x%02x \n", code[2], code[1]); opbytes=3; break;
        case 0x02: printf("STAX     B\n"); break;
        case 0x03: printf("INX      B\n"); break;
        case 0x04: printf("INR      B\n"); break;
        case 0x05: printf("DCR      B\n"); break;
        case 0x06: printf("MVI      B,#$%02x \n", code[1]); opbytes=2; break;
        case 0x07: printf("RLC\n"); break;
        case 0x08: printf("NOP\n"); break;
        case 0x09: printf("DAD      B\n"); break;
        case 0x0a: printf("LDAX     B\n"); break;
        case 0x0b: printf("DCX      B\n"); break;
        case 0x0c: printf("INR      C\n"); break;
        case 0x0d: printf("DCR    C\n"); break;
		case 0x0e: printf("MVI    C,#$%02x \n", code[1]); opbytes = 2;	break;
		case 0x0f: printf("RRC\n"); break;
			
		case 0x10: printf("NOP\n"); break;
		case 0x11: printf("LXI    D,#$%02x%02x \n", code[2], code[1]); opbytes=3; break;
		case 0x12: printf("STAX   D\n"); break;
		case 0x13: printf("INX    D\n"); break;
		case 0x14: printf("INR    D\n"); break;
		case 0x15: printf("DCR    D\n"); break;
		case 0x16: printf("MVI    D,#$%02x \n", code[1]); opbytes=2; break;
		case 0x17: printf("RAL\n"); break;
		case 0x18: printf("NOP\n"); break;
		case 0x19: printf("DAD    D\n"); break;
		case 0x1a: printf("LDAX   D\n"); break;
		case 0x1b: printf("DCX    D\n"); break;
		case 0x1c: printf("INR    E\n"); break;
		case 0x1d: printf("DCR    E\n"); break;
		case 0x1e: printf("MVI    E,#$%02x \n", code[1]); opbytes = 2; break;
		case 0x1f: printf("RAR\n"); break;
			
		case 0x20: printf("NOP\n"); break;
		case 0x21: printf("LXI    H,#$%02x%02x \n", code[2], code[1]); opbytes=3; break;
		case 0x22: printf("SHLD   $%02x%02x \n", code[2], code[1]); opbytes=3; break;
		case 0x23: printf("INX    H\n"); break;
		case 0x24: printf("INR    H\n"); break;
		case 0x25: printf("DCR    H\n"); break;
		case 0x26: printf("MVI    H,#$%02x \n", code[1]); opbytes=2; break;
		case 0x27: printf("DAA\n"); break;
		case 0x28: printf("NOP\n"); break;
		case 0x29: printf("DAD    H\n"); break;
		case 0x2a: printf("LHLD   $%02x%02x \n", code[2], code[1]); opbytes=3; break;
		case 0x2b: printf("DCX    H\n"); break;
		case 0x2c: printf("INR    L\n"); break;
		case 0x2d: printf("DCR    L\n"); break;
		case 0x2e: printf("MVI    L,#$%02x \n", code[1]); opbytes = 2; break;
		case 0x2f: printf("CMA\n"); break;
			
		case 0x30: printf("NOP\n"); break;
		case 0x31: printf("LXI    SP,#$%02x%02x\n", code[2], code[1]); opbytes=3; break;
		case 0x32: printf("STA    $%02x%02x\n", code[2], code[1]); opbytes=3; break;
		case 0x33: printf("INX    SP\n"); break;
		case 0x34: printf("INR    M\n"); break;
		case 0x35: printf("DCR    M\n"); break;
		case 0x36: printf("MVI    M,#$%02x\n", code[1]); opbytes=2; break;
		case 0x37: printf("STC\n"); break;
		case 0x38: printf("NOP\n"); break;
		case 0x39: printf("DAD    SP\n"); break;
		case 0x3a: printf("LDA    $%02x%02x\n", code[2], code[1]); opbytes=3; break;
		case 0x3b: printf("DCX    SP\n"); break;
		case 0x3c: printf("INR    A\n"); break;
		case 0x3d: printf("DCR    A\n"); break;
		case 0x3e: printf("MVI    A,#$%02x\n", code[1]); opbytes = 2; break;
		case 0x3f: printf("CMC\n"); break;
			
		case 0x40: printf("MOV    B,B\n"); break;
		case 0x41: printf("MOV    B,C\n"); break;
		case 0x42: printf("MOV    B,D\n"); break;
		case 0x43: printf("MOV    B,E\n"); break;
		case 0x44: printf("MOV    B,H\n"); break;
		case 0x45: printf("MOV    B,L\n"); break;
		case 0x46: printf("MOV    B,M\n"); break;
		case 0x47: printf("MOV    B,A\n"); break;
		case 0x48: printf("MOV    C,B\n"); break;
		case 0x49: printf("MOV    C,C\n"); break;
		case 0x4a: printf("MOV    C,D\n"); break;
		case 0x4b: printf("MOV    C,E\n"); break;
		case 0x4c: printf("MOV    C,H\n"); break;
		case 0x4d: printf("MOV    C,L\n"); break;
		case 0x4e: printf("MOV    C,M\n"); break;
		case 0x4f: printf("MOV    C,A\n"); break;
		
		case 0x50: printf("MOV    D,B\n"); break;
		case 0x51: printf("MOV    D,C\n"); break;
		case 0x52: printf("MOV    D,D\n"); break;
		case 0x53: printf("MOV    D.E\n"); break;
		case 0x54: printf("MOV    D,H\n"); break;
		case 0x55: printf("MOV    D,L\n"); break;
		case 0x56: printf("MOV    D,M\n"); break;
		case 0x57: printf("MOV    D,A\n"); break;
		case 0x58: printf("MOV    E,B\n"); break;
		case 0x59: printf("MOV    E,C\n"); break;
		case 0x5a: printf("MOV    E,D\n"); break;
		case 0x5b: printf("MOV    E,E\n"); break;
		case 0x5c: printf("MOV    E,H\n"); break;
		case 0x5d: printf("MOV    E,L\n"); break;
		case 0x5e: printf("MOV    E,M\n"); break;
		case 0x5f: printf("MOV    E,A\n"); break;

		case 0x60: printf("MOV    H,B\n"); break;
		case 0x61: printf("MOV    H,C\n"); break;
		case 0x62: printf("MOV    H,D\n"); break;
		case 0x63: printf("MOV    H.E\n"); break;
		case 0x64: printf("MOV    H,H\n"); break;
		case 0x65: printf("MOV    H,L\n"); break;
		case 0x66: printf("MOV    H,M\n"); break;
		case 0x67: printf("MOV    H,A\n"); break;
		case 0x68: printf("MOV    L,B\n"); break;
		case 0x69: printf("MOV    L,C\n"); break;
		case 0x6a: printf("MOV    L,D\n"); break;
		case 0x6b: printf("MOV    L,E\n"); break;
		case 0x6c: printf("MOV    L,H\n"); break;
		case 0x6d: printf("MOV    L,L\n"); break;
		case 0x6e: printf("MOV    L,M\n"); break;
		case 0x6f: printf("MOV    L,A\n"); break;

		case 0x70: printf("MOV    M,B\n"); break;
		case 0x71: printf("MOV    M,C\n"); break;
		case 0x72: printf("MOV    M,D\n"); break;
		case 0x73: printf("MOV    M.E\n"); break;
		case 0x74: printf("MOV    M,H\n"); break;
		case 0x75: printf("MOV    M,L\n"); break;
		case 0x76: printf("HLT\n");        break;
		case 0x77: printf("MOV    M,A\n"); break;
		case 0x78: printf("MOV    A,B\n"); break;
		case 0x79: printf("MOV    A,C\n"); break;
		case 0x7a: printf("MOV    A,D\n"); break;
		case 0x7b: printf("MOV    A,E\n"); break;
		case 0x7c: printf("MOV    A,H\n"); break;
		case 0x7d: printf("MOV    A,L\n"); break;
		case 0x7e: printf("MOV    A,M\n"); break;
		case 0x7f: printf("MOV    A,A\n"); break;

		case 0x80: printf("ADD    B\n"); break;
		case 0x81: printf("ADD    C\n"); break;
		case 0x82: printf("ADD    D\n"); break;
		case 0x83: printf("ADD    E\n"); break;
		case 0x84: printf("ADD    H\n"); break;
		case 0x85: printf("ADD    L\n"); break;
		case 0x86: printf("ADD    M\n"); break;
		case 0x87: printf("ADD    A\n"); break;
		case 0x88: printf("ADC    B\n"); break;
		case 0x89: printf("ADC    C\n"); break;
		case 0x8a: printf("ADC    D\n"); break;
		case 0x8b: printf("ADC    E\n"); break;
		case 0x8c: printf("ADC    H\n"); break;
		case 0x8d: printf("ADC    L\n"); break;
		case 0x8e: printf("ADC    M\n"); break;
		case 0x8f: printf("ADC    A\n"); break;

		case 0x90: printf("SUB    B\n"); break;
		case 0x91: printf("SUB    C\n"); break;
		case 0x92: printf("SUB    D\n"); break;
		case 0x93: printf("SUB    E\n"); break;
		case 0x94: printf("SUB    H\n"); break;
		case 0x95: printf("SUB    L\n"); break;
		case 0x96: printf("SUB    M\n"); break;
		case 0x97: printf("SUB    A\n"); break;
		case 0x98: printf("SBB    B\n"); break;
		case 0x99: printf("SBB    C\n"); break;
		case 0x9a: printf("SBB    D\n"); break;
		case 0x9b: printf("SBB    E\n"); break;
		case 0x9c: printf("SBB    H\n"); break;
		case 0x9d: printf("SBB    L\n"); break;
		case 0x9e: printf("SBB    M\n"); break;
		case 0x9f: printf("SBB    A\n"); break;

		case 0xa0: printf("ANA    B\n"); break;
		case 0xa1: printf("ANA    C\n"); break;
		case 0xa2: printf("ANA    D\n"); break;
		case 0xa3: printf("ANA    E\n"); break;
		case 0xa4: printf("ANA    H\n"); break;
		case 0xa5: printf("ANA    L\n"); break;
		case 0xa6: printf("ANA    M\n"); break;
		case 0xa7: printf("ANA    A\n"); break;
		case 0xa8: printf("XRA    B\n"); break;
		case 0xa9: printf("XRA    C\n"); break;
		case 0xaa: printf("XRA    D\n"); break;
		case 0xab: printf("XRA    E\n"); break;
		case 0xac: printf("XRA    H\n"); break;
		case 0xad: printf("XRA    L\n"); break;
		case 0xae: printf("XRA    M\n"); break;
		case 0xaf: printf("XRA    A\n"); break;

		case 0xb0: printf("ORA    B\n"); break;
		case 0xb1: printf("ORA    C\n"); break;
		case 0xb2: printf("ORA    D\n"); break;
		case 0xb3: printf("ORA    E\n"); break;
		case 0xb4: printf("ORA    H\n"); break;
		case 0xb5: printf("ORA    L\n"); break;
		case 0xb6: printf("ORA    M\n"); break;
		case 0xb7: printf("ORA    A\n"); break;
		case 0xb8: printf("CMP    B\n"); break;
		case 0xb9: printf("CMP    C\n"); break;
		case 0xba: printf("CMP    D\n"); break;
		case 0xbb: printf("CMP    E\n"); break;
		case 0xbc: printf("CMP    H\n"); break;
		case 0xbd: printf("CMP    L\n"); break;
		case 0xbe: printf("CMP    M\n"); break;
		case 0xbf: printf("CMP    A\n"); break;

		case 0xc0: printf("RNZ\n"); break;
		case 0xc1: printf("POP    B\n"); break;
		case 0xc2: printf("JNZ    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xc3: printf("JMP    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xc4: printf("CNZ    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xc5: printf("PUSH   B\n"); break;
		case 0xc6: printf("ADI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xc7: printf("RST    0\n"); break;
		case 0xc8: printf("RZ\n"); break;
		case 0xc9: printf("RET\n"); break;
		case 0xca: printf("JZ     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xcb: printf("JMP    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xcc: printf("CZ     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xcd: printf("CALL   $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xce: printf("ACI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xcf: printf("RST    1\n"); break;

		case 0xd0: printf("RNC\n"); break;
		case 0xd1: printf("POP    D\n"); break;
		case 0xd2: printf("JNC    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xd3: printf("OUT    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xd4: printf("CNC    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xd5: printf("PUSH   D\n"); break;
		case 0xd6: printf("SUI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xd7: printf("RST    2\n"); break;
		case 0xd8: printf("RC\n");  break;
		case 0xd9: printf("RET\n"); break;
		case 0xda: printf("JC     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xdb: printf("IN     #$%02x\n",code[1]); opbytes = 2; break;
		case 0xdc: printf("CC     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xdd: printf("CALL   $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xde: printf("SBI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xdf: printf("RST    3\n"); break;

		case 0xe0: printf("RPO\n"); break;
		case 0xe1: printf("POP    H\n"); break;
		case 0xe2: printf("JPO    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xe3: printf("XTHL\n");break;
		case 0xe4: printf("CPO    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xe5: printf("PUSH   H\n"); break;
		case 0xe6: printf("ANI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xe7: printf("RST    4\n"); break;
		case 0xe8: printf("RPE\n"); break;
		case 0xe9: printf("PCHL\n");break;
		case 0xea: printf("JPE    $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xeb: printf("XCHG\n"); break;
		case 0xec: printf("CPE     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xed: printf("CALL   $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xee: printf("XRI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xef: printf("RST    5\n"); break;

		case 0xf0: printf("RP\n");  break;
		case 0xf1: printf("POP    PSW\n"); break;
		case 0xf2: printf("JP     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xf3: printf("DI\n");  break;
		case 0xf4: printf("CP     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xf5: printf("PUSH   PSW\n"); break;
		case 0xf6: printf("ORI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xf7: printf("RST    6\n"); break;
		case 0xf8: printf("RM\n");  break;
		case 0xf9: printf("SPHL\n");break;
		case 0xfa: printf("JM     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xfb: printf("EI\n");  break;
		case 0xfc: printf("CM     $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xfd: printf("CALL   $%02x%02x\n",code[2],code[1]); opbytes = 3; break;
		case 0xfe: printf("CPI    #$%02x\n",code[1]); opbytes = 2; break;
		case 0xff: printf("RST    7\n"); break;
    }

    return opbytes;
}

int main( int argc, char**argv){
    FILE *f = fopen(argv[1], "rb");
	if (f == NULL){
		printf("error: couldn't open file %s\n", argv[1]);
	}
	fseek(f, 0L, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);

	unsigned char *buffer = malloc(fsize);
	fread(buffer, fsize, 1 , f);
	fclose(f);

	int pc = 0;

	while (pc< fsize){
		pc += Disassembler8080Op(buffer,pc);
	}
	return 0;
	}