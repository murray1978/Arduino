// tabless.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef void(*__opcode_handler_t)(void);

typedef struct opcode_table{
	__opcode_handler_t opcode_handler;
} opcode_table_t;

void opcodeOne( void ){
	printf("Opcode1");
}

void opcode2( void ){
	printf("\n");
}

void third( void){
	printf("33");
}

opcode_table_t opcode_table[] = {
	{opcodeOne},
	{opcode2},
	{third}
};

__opcode_handler_t handler[] = {
	opcodeOne, opcode2, third
};
int _tmain(int argc, _TCHAR* argv[])
{
	opcode_table[0].opcode_handler();
	opcode_table[1].opcode_handler();
	opcode_table[2].opcode_handler();
	handler[0]();
	return 0;
}

