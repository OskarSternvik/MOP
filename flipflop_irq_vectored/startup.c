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

#define GPIO_E_BASE 0x40021000
#define GPIO_E_MODER ((volatile unsigned int*)(GPIO_E_BASE + 0))
#define GPIO_E_ODR_LOW ((volatile unsigned char*)(GPIO_E_BASE + 0x14))
#define GPIO_E_ODR_HIGH ((volatile unsigned char*)(GPIO_E_BASE + 0x15))

#define GPIO_D_BASE 0x40020C00
#define GPIO_D_MODER ((volatile unsigned int*)(GPIO_D_BASE + 0))
#define GPIO_D_ODR_LOW ((volatile unsigned char*)(GPIO_D_BASE + 0x14))
#define GPIO_D_ODR_HIGH ((volatile unsigned char*)(GPIO_D_BASE + 0x15))

#define EXTI 0x40013C00
#define EXTI_IMR ((volatile unsigned int*)(EXTI + 0))
#define EXTI_RTSR ((volatile unsigned int*)(EXTI + 8))
#define EXTI_FTSR ((volatile unsigned int*)(EXTI + 0xC))
#define EXTI_PR ((volatile unsigned int*) (EXTI + 0x14))

#define EXTI0_IRQ_BPOS (1<<0)
#define EXTI1_IRQ_BPOS (1<<1)
#define EXTI2_IRQ_BPOS (1<<2)
#define EXTI3_IRQ_BPOS (1<<3)

#define NVIC 0xE000E10
#define NVIC_ISERO ((volatile unsigned int*)(NVIC + 0))
#define NVIC_EXTI3_IRQ_BPOS 1<<9
#define NVIC_EXTI2_IRQ_BPOS 1<<8
#define NVIC_EXTI1_IRQ_BPOS 1<<7
#define NVIC_EXTI0_IRQ_BPOS 1<<6

#define SYSCFG_EXTICR1 ((volatile unsigned int*)(0x40013808))

#define EXTI2_IRQVEC ((void(**)(void)) 0x2001C060)
#define EXTI1_IRQVEC ((void(**)(void)) 0x2001C05C)
#define EXTI0_IRQVEC ((void(**)(void)) 0x2001C058)

static volatile int count = 0;

void irq_handler_EXTI0();
void irq_handler_EXTI1();
void irq_handler_EXTI2();

void app_init(void){
	*GPIO_E_MODER = 0;
	*GPIO_D_MODER &= 00005555;
	*GPIO_D_MODER |= 55555555;
	
	*GPIO_D_ODR_HIGH = 0;
	
	// Aktivera NVIC för EXTI
	*NVIC_ISERO |= NVIC_EXTI2_IRQ_BPOS;
	*NVIC_ISERO |= NVIC_EXTI1_IRQ_BPOS;
	*NVIC_ISERO |= NVIC_EXTI0_IRQ_BPOS;
	
	*EXTI0_IRQVEC = irq_handler_EXTI0;
	*EXTI1_IRQVEC = irq_handler_EXTI1;
	*EXTI2_IRQVEC = irq_handler_EXTI2;
	
	*SYSCFG_EXTICR1 &= 0xFFFFF000;
	
	*SYSCFG_EXTICR1 |= 0x00000444;
	
	*EXTI_IMR |= EXTI2_IRQ_BPOS;
	*EXTI_IMR |= EXTI1_IRQ_BPOS;
	*EXTI_IMR |= EXTI0_IRQ_BPOS;
	
	*EXTI_FTSR |= EXTI2_IRQ_BPOS;
	*EXTI_FTSR |= EXTI2_IRQ_BPOS;
	*EXTI_FTSR |= EXTI2_IRQ_BPOS;
	
	*EXTI_RTSR &= 0;
}

void irq_handler_EXTI0(void){
	*EXTI_PR |= EXTI0_IRQ_BPOS;
	count++;
}
void irq_handler_EXTI1(void){
	*EXTI_PR |= EXTI1_IRQ_BPOS;
	count = 0;
}

void irq_handler_EXTI2(void){
	*EXTI_PR |= EXTI2_IRQ_BPOS;
	*GPIO_D_ODR_HIGH = ~(*GPIO_D_ODR_HIGH);
}

void main(void)
{
	app_init();
	while(1){
		*GPIO_D_ODR_LOW = count;
	}
}

