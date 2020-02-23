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

#define GPIO_D_BASE 0x40020C00
#define GPIO_D_MODER ((volatile unsigned int*)(GPIO_D_BASE + 0))
#define GPIO_D_ODR_LOW ((volatile unsigned char*)(GPIO_D_BASE + 0x14))
#define GPIO_D_ODR_HIGH ((volatile unsigned char*)(GPIO_D_BASE + 0x15))

#define System_Timer 0xE000E010
#define STK_CTRL ((volatile unsigned int*)(System_Timer + 0))
#define STK_LOAD ((volatile unsigned int*)(System_Timer + 4))
#define STK_VAL  ((volatile unsigned int*)(System_Timer + 8))


void delay_1mikro(void){
	/* SystemCOREClock = 168000000 */
	*STK_CTRL = 0;
	*STK_LOAD =(168-1);
	*STK_VAL = 0;
	*STK_CTRL = 7;
}


#ifdef SIMULATOR
#define DELAY_COUNT 100
#else
#define DELAY_COUNT 1000000
#endif

static volatile int systick_flag;
static volatile int delay_count;


void delay(unsigned int count){
	if( count == 0){
		return;
	}
	delay_count = count;
	systick_flag = 0;
	delay_1mikro();
}

void systick_irq_handler(void){
	*STK_CTRL = 0;
	delay_count--;
	if(delay_count > 0){
		delay_1mikro();
	} else {
		systick_flag = 1;
	}
}

void init_app(void){
	*(GPIO_D_MODER) = 0x00005555;
	*((void(**)(void) )0x2001C03C) = systick_irq_handler;
	systick_flag = 0;
}



void main(void)
{
	init_app();
	*GPIO_D_ODR_LOW = 0;
	delay(DELAY_COUNT);
	*GPIO_D_ODR_LOW = 0xFF;
	while(1){
		if(systick_flag) break;
		/* "Parallell kod" */
	}
	*GPIO_D_ODR_LOW = 0;
}

