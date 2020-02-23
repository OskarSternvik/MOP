/*
 * 	startup.c
 *
 */
 __attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
} 

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


