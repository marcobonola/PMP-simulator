#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

// Function pointer for "special" instructions
class cpu_core;
typedef void (*operation)(cpu_core *cpu);
void sysc_op(cpu_core *cpu);

// Information about each instruction in the cpu, including function pointer and a
// character description for debugging purposes.
typedef struct _instruction {
	char name[9];
	operation special_case; // if nonnull handled by some magical process.
	const bool register_write;

	const byte alu_operation;  // an integer which determines which operation the ALU should perform
	const byte alu_source;     // second arg source
	                           // 0: source from register.
	                           // 1: source from immediate.
	                           // 2: source from sum of immediate and register (addrcalc)

	const bool branch;
	const byte mem_write;
	const byte mem_read;
	const bool mem_to_register;
} instruction;

// opcode is the instruction's position in the array.
const instruction instructions [64] =

  {{ "    nop", NULL,     false, 0, 0, false, 0, 0, false }     // 00  OK
  ,{ "   addi", NULL,     true,  1, 1, false, 0, 0, false }     // 01  OK 
  ,{ "   beqz", NULL,     false, 0, 0, true,  0, 0, false }     // 02  TBC
  ,{ "    bge", NULL,     false, 0, 0, true,  0, 0, false }     // 03  TBC
  ,{ "    bne", NULL,     false, 0, 0, true,  0, 0, false }     // 04  TBC
  ,{ "     la", NULL,     true,  0, 1, false, 0, 0, false }     // 05  OK
  ,{ "     lw", NULL,     true,  0, 2, false, 0, 4, true  }     // 06  TBC
//  ,{ "     lw", NULL,     true,  0, 2, false, 0, 1, true  }     // 06  TBC
  ,{ "     li", NULL,     true,  0, 1, false, 0, 0, false }     // 07  OK
  ,{ "     lb", NULL,     true,  0, 2, false, 0, 1, true  }     // 08  TBC
  ,{ "   subi", NULL,     true,  2, 1, false, 0, 0, false }     // 09  OK
  ,{ "    add", NULL,     true,  1, 0, false, 0, 0, false }     // 0a  OK
  ,{ "syscall", &sysc_op, false, 0, 0, false, 0, 0, false }	// 0b  OK
  ,{ "     st", NULL,     false, 0, 2, false, 1, 0, false }     // 0c #NO SIM per bug pipeline
  ,{ "     ld", NULL,     true,  1, 2, false, 0, 4, true }     // 0d #NO SIM 
  ,{ "    sub", NULL,     true,  2, 0, false, 0, 0, false }     // 0e #NO SIM
  ,{ "   outb", NULL,     false, 0, 2, false, 1, 1, false }     // 0f TBC
  ,{ "   outh", NULL,     false, 0, 2, false, 2, 2, false }     // 10 TBC
  ,{ "   outw", NULL,     false, 0, 2, false, 4, 4, false }     // 11 TBC
  ,{ "   outd", NULL,     false, 0, 2, false, 8, 8, false }     // 12 TBC
  ,{ "   outq", NULL,     false, 0, 2, false, 16, 16, false }   // 13 TBC
  ,{ "   outl", NULL,     false, 0, 2, false, 17, 17, false }   // 14 TBC
  ,{ "  outla", NULL,     false, 0, 0, false, 17, 17, false }   // 15 TBC
  ,{ "    and", NULL,     true,  3, 0, false, 0, 0, false }     // 16 TBC
  ,{ "     or", NULL,     true,  4, 0, false, 0, 0, false }     // 17 TBC
  ,{ "    xor", NULL,     true,  5, 0, false, 0, 0, false }     // 18 TBC
  ,{ "   xnor", NULL,     true,  6, 0, false, 0, 0, false }     // 19 TBC
  ,{ "    slr", NULL,     true,  7, 0, false, 0, 0, false }     // 1A TBC
  ,{ "    adc", NULL,     true,  8, 0, false, 0, 0, false }     // 1B TBC
  ,{ "   andi", NULL,     true,  3, 1, false, 0, 0, false }     // 1C TBC
  ,{ "    stw", NULL,     false, 0, 2, false, 4, 0, false }     // 1D TBC
  ,{ "  outwh", NULL,     false, 0, 2, false, 6, 6, false }     // 1E TBC
  ,{ "    not", NULL,     true,  9, 0, false, 0, 0, false }     // 1F TBC
  ,{ "    sth", NULL,     false, 0, 2, false, 2, 0, false }     // 20 TBC
  }; 

#endif /* _INSTRUCTION_H_ */
