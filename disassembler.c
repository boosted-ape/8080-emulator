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

int Disassembler8080Op(unsigned char *codebuffer, int pc){
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf ("%04x ", pc);
    switch(*code){
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI      B,#$%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x02: printf("STAX     B"); break;
        case 0x03: printf("INX      B"); break;
        case 0x04: printf("INR      B"); break;
        case 0x05: printf("DCR      B"); break;
        case 0x06: printf("MVI      B,#$%02x", code[1]); opbytes=2; break;
        case 0x07: printf("RLC"); break;
        case 0x08: printf("NOP"); break;
        //tbc
    }
}