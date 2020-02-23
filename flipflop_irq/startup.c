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


#define EXTI 0x40013C00
#define SYSCFG_EXTICR1 ((volatile unsigned int*)(0x40013808))
#define EXTI_IMR ((volatile unsigned int*)(EXTI + 0))
#define EXTI_RTSR ((volatile unsigned int*)(EXTI + 8))
#define EXTI_PR ((volatile unsigned int*) (EXTI + 0x14))
#define NVIC_ISERO ((volatile unsigned int*)(0xE000E100))

static volatile int count = 0;

void irq_handler(void){
	/* Om avbrott från EXTI3: inkrementera count, kvittera avbrott från EXTI3 */
	count++;
	if(*(EXTI_PR) & 1<<3){
		*EXTI_PR |= 1<<3;
	}
}

void app_init(void){
	/* Sätt upp PD0-7 som utport */
	*(GPIO_D_MODER) = 0x00005555;
	/* koppla upp PE3 till avbrottslina EXTI3 */
	*SYSCFG_EXTICR1 &= 0x0FFF;
	*SYSCFG_EXTICR1 |= 0x4000;
	/* Konfiguera EXTI3 för att generera avbrott */
	*EXTI_IMR |= 1<<3; 
	/* Konfiguera för avbrott på positiv flank */
	*EXTI_RTSR |= 1<<3;
	/* Sätt upp avbrottsvektor */
	*((void(**)(void)) 0x2001C064) = irq_handler;
	/* Konfiguera de bitar i NVIC som kontrollerar den avbrottslina som EXTI3 kopplas till */
	*NVIC_ISERO |= 1<<9;
}

void main(void)
{
	int count2 = count;
	app_init();
	while(1){
		//count2 = count;
		*GPIO_D_ODR_LOW = count2++;
		
	}
}

