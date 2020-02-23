#include <stdio.h>

__attribute__( ( naked) ) unsigned int getPsrReg( void )
{
	__asm (
		" MRS R0, APSR\n"
		" BX LR\n"
		);
	
}

__attribute__( ( naked) ) void setPsrReg( unsigned int apsr )
{
	__asm (
	" MSR APSR,R0\n"
	" BX LR\n"
	);
}

int main(int argc, char **argv)
{
	unsigned int psr;
	psr = getPsrReg();
	setPsrReg(0);
	psr = getPsrReg();
	return 0;
}
