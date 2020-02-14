

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

 #define STK_CTRL ((volatile unsigned int*)(0xE000E010))
 #define STK_LOAD ((volatile unsigned int*)(0xE000E014))
 #define STK_VAL  ((volatile unsigned int*)(0xE000E018))
 
 #define SIMULATOR

void delay_250ns(void){
	#ifdef SIMULATOR
		return;
	#endif
		/* SystemCoreClock = 16800000 */
		*STK_CTRL = 0;
		*STK_LOAD = ((168/4-1));
		*STK_VAL;
		*STK_CTRL = 5;
		while((*STK_CTRL & 10000) == 0){}
}
void delay_500ns(void){
	delay_250ns();
	delay_250ns();
}

void delay_mikro(unsigned int us){
	while(us--){
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
	}

}

void delay_milli(unsigned int ms){
	#ifdef SIMULATOR
		ms = ms/1000;
		ms++;
	#endif
	while(ms--){
		delay_mikro(1000);
	}
}

 #define GPIO_E_BASE 0x40021000
 #define GPIO_E_MODER    ((volatile unsigned int* )  (GPIO_E_BASE+0x00))
 #define GPIO_E_OTYPER   ((volatile unsigned short* )(GPIO_E_BASE+0x04))
 #define GPIO_E_OSPEEDR  ((volatile unsigned int* )  (GPIO_E_BASE+0x08))
 #define GPIO_E_PUPDR    ((volatile unsigned int* )  (GPIO_E_BASE+0x0C))
 
 #define GPIO_E_IDR_LOW  ((volatile unsigned char* ) (GPIO_E_BASE+0x10))
 #define GPIO_E_IDR_HIGH ((volatile unsigned char* ) (GPIO_E_BASE+0x11))
 #define GPIO_E_ODR_LOW  ((volatile unsigned char* ) (GPIO_E_BASE+0x14))
 #define GPIO_E_ODR_HIGH ((volatile unsigned char* ) (GPIO_E_BASE+0x15))
 
 #define B_E 0x40
 #define B_SELECT 4
 #define B_RW 2
 #define B_RS 1
 
 #define B_RST 0x20
 #define B_CS2 0x10
 #define B_CS1 8
 
 #define LCD_ON         0x3F
 #define LCD_OFF        0x3E
 #define LCD_SET_ADD    0x40
 #define LCD_SET_PAGE   0xB8
 #define LCD_DISP_START 0xC0
 #define LCD_BUSY       0x80
 
 typedef unsigned char uint8_t;
 
 void graphic_ctrl_bit_set(uint8_t x){
	 uint8_t c;
	 c = *GPIO_E_ODR_LOW;
	 c &= ~B_SELECT;
	 c |= (~B_SELECT & x);
	 *GPIO_E_ODR_LOW = c;
 }
 void graphic_ctrl_bit_clear(uint8_t x){
	 uint8_t c;
	 c = *GPIO_E_ODR_LOW;
	 c &= ~B_SELECT;
	 c &= ~x;
	 *GPIO_E_ODR_LOW = c;
 }
 
 static void select_controller(uint8_t controller){
	 switch(controller){
		 case 0:
			 graphic_ctrl_bit_clear(B_CS1|B_CS2);
			 break;
		 case B_CS1: 
			 graphic_ctrl_bit_set(B_CS1);
			 graphic_ctrl_bit_clear(B_CS2);
			 break;
		 case B_CS2:
			 graphic_ctrl_bit_set(B_CS2);
			 graphic_ctrl_bit_clear(B_CS1);
			 break;
		 case B_CS1|B_CS2:
			 graphic_ctrl_bit_set(B_CS1);
			 graphic_ctrl_bit_set(B_CS2);
			 break;
	 }
 }
 void graphic_wait_ready(void){
	 uint8_t c;
	 graphic_ctrl_bit_clear(B_E);
	 *GPIO_E_MODER = 0x00005555;
	 graphic_ctrl_bit_clear(B_RS);
	 graphic_ctrl_bit_set(B_RW);
	 delay_500ns();
	 
	 while(1){
		 graphic_ctrl_bit_set(B_E);
		 delay_500ns();
		 c = *GPIO_E_IDR_HIGH & LCD_BUSY;
		 graphic_ctrl_bit_clear(B_E);
		 delay_500ns();
		 if(c == 0){
			break;
		 }
	 }
	 //graphic_ctrl_bit_set(B_E); //är med i boken
	 *GPIO_E_MODER = 0x55555555;
	 }
 
 uint8_t graphic_read(uint8_t controller){
	 uint8_t c;
	 graphic_ctrl_bit_clear(B_E);
	 *GPIO_E_MODER = 0x00005555;
	 graphic_ctrl_bit_set(B_RS);
	 graphic_ctrl_bit_set(B_RW);
	 select_controller(controller);
	 delay_500ns();
	 graphic_ctrl_bit_set(B_E);
	 delay_500ns();
	 c = *GPIO_E_IDR_HIGH;
	 graphic_ctrl_bit_clear(B_E);
	 *GPIO_E_MODER = 0x55555555;
	 if(controller & B_CS1){
		 select_controller(B_CS1);
		 graphic_wait_ready();
	 }
	 if(controller & B_CS2){
		 select_controller(B_CS2);
		 graphic_wait_ready();
	 }
	 return c;
 }
 
 void graphic_write(uint8_t value, uint8_t controller){
	 *GPIO_E_ODR_HIGH = value;
	 select_controller(controller);
	 delay_500ns();
	 graphic_ctrl_bit_set(B_E);
	 delay_500ns();
	 graphic_ctrl_bit_clear(B_E);
	 
	 if(controller & B_CS1){
		 select_controller(B_CS1);
		 graphic_wait_ready();
	 }
	 if(controller & B_CS2){
		 select_controller(B_CS2);
		 graphic_wait_ready();
	 }
	  *GPIO_E_ODR_HIGH &= 0;
	 graphic_ctrl_bit_set(B_E);
	 select_controller(0); 
 }
 
 void graphic_write_command(uint8_t command, uint8_t controller){
	 graphic_ctrl_bit_clear(B_E);
	 select_controller(controller);
	 graphic_ctrl_bit_clear(B_RS);
	 graphic_ctrl_bit_clear(B_RW);
	 graphic_write(command, controller);
 }
 
 void graphic_write_data(uint8_t data, uint8_t controller){
	 graphic_ctrl_bit_clear(B_E);
	 select_controller(controller);
	 graphic_ctrl_bit_set(B_RS);
	 graphic_ctrl_bit_clear(B_RW);
	 graphic_write(data, controller);
 }
 
 unsigned char graphic_read_data(unsigned char controller){
	 (void) graphic_read(controller);
	 return graphic_read(controller);
 }
 

 
 void init_app(){
	 *(GPIO_E_MODER) = 0x55555555;
 }
 
 void graphic_initalize(void){
	graphic_ctrl_bit_set(B_E);
	delay_mikro(10);
	graphic_ctrl_bit_clear(B_CS1);
	graphic_ctrl_bit_clear(B_CS2);
	graphic_ctrl_bit_clear(B_RST);
	graphic_ctrl_bit_clear(B_E);
	delay_milli(30);
	graphic_ctrl_bit_set(B_RST);
	delay_milli(100); //kan vara onödig
	graphic_write_command(LCD_OFF,        B_CS1|B_CS2);
	graphic_write_command(LCD_ON,         B_CS1|B_CS2);
	graphic_write_command(LCD_DISP_START, B_CS1|B_CS2);
	graphic_write_command(LCD_SET_ADD,    B_CS1|B_CS2);
	graphic_write_command(LCD_SET_PAGE,   B_CS1|B_CS2);
	select_controller(0);
 }
 
 void graphic_clear_screen(void){
	 uint8_t page, add;
	 
	 for(page = 0; page<8;page++){
		 graphic_write_command(LCD_SET_PAGE|page, B_CS1|B_CS2);
		 graphic_write_command(LCD_SET_ADD|0, B_CS1|B_CS2);
		 for(add = 0; add<64; add++){
			 graphic_write_data(0, B_CS1|B_CS2);
		 }
	 }
 }
 
 void pixel(unsigned x, unsigned y, unsigned set){
	 uint8_t mask, temp, controller;
	 int index = ((y-1)/8);
	 
	 if((x < 1) || (y < 1) || (x > 128) || (y > 64)) return;
	 
	 switch((y-1)%8){
		case 0:mask = 1;break;
		case 1:mask = 2;break;
		case 2:mask = 4;break;
		case 3:mask = 8;break;
		case 4:mask = 0x10;break;
		case 5:mask = 0x20;break;
		case 6:mask = 0x40;break;
		case 7:mask = 0x80;break;
	 }
	 if(set == 0){
		 mask = ~mask;
	 }
	 if(x > 64){
		 controller = B_CS2;
		 x = x-65;
	 } else{
		 controller = B_CS1;
		 x = x-1;
	 }
	 graphic_write_command(LCD_SET_ADD | x, controller);
	 graphic_write_command(LCD_SET_PAGE | index, controller);
	 temp = graphic_read_data(controller);
	 graphic_write_command(LCD_SET_ADD|x, controller);
	 if(set){
		 mask |= temp;
	 } else{
		 mask &= temp;
	 }
	 graphic_write_data(mask, controller);
 }
 
 typedef struct tPoint{
	 unsigned char x;
	 unsigned char y;
 }POINT;
 
 #define MAX_POINTS 20
 
 typedef struct tGeometry{
	 int numpoints;
	 int sizex;
	 int sizey;
	 POINT px[MAX_POINTS];
 } GEOMETRY, *PGEOMETRY;
 
 GEOMETRY ball_geometry =
 {
	 12,
	 4,4,
	 {
			   {1,0}, {2,0}, 
		{0,1}, {1,1}, {2,1}, {3,1}, 
		{0,2}, {1,2}, {2,2}, {3,2}, 
			   {1,3}, {2,3}
	 }
 };
 
 typedef struct tObj{
	 PGEOMETRY geo;
	 int dirx,diry;
	 int posx,posy;
	 void (*draw) (struct tObj *);
	 void (*clear) (struct tObj *);
	 void (*move) (struct tObj *);
	 void (*set_speed)(struct tObj *, int, int);
 } OBJECT, *POBJECT;
 
 void set_object_speed(POBJECT o, int speedx, int speedy){
	 o->dirx = speedx;
	 o->diry = speedy;
 }
 void draw_object(POBJECT o){
	 for(int i = 0; i < o->geo->numpoints; i++){
		 pixel(o->posx + o->geo->px[i].x, o->posy + o->geo->px[i].y, 1);
	 }
 }
 void clear_object(POBJECT o){
	 for(int i = 0; i< o->geo->numpoints; i++){
		 pixel(o->posx + o->geo->px[i].x, o->posy + o->geo->px[i].y, 0);
	 }
 }
 void move_object(POBJECT o){
	 clear_object(o);
	 o->posx += o->dirx;
	 o->posy += o->diry;
	 if(o->posx < 1){
		 o->dirx *= -1;
	 }
	 if(o->posx > 128){
		 o->dirx *= -1;
	 }
	 if(o->posy < 1){
		 o->diry *= -1;
	 }
	 if(o->posy > 64){
		 o->posy *= -1;
	 }
	 draw_object(o);
 }
 
 static OBJECT ball =
 {
	 &ball_geometry,
	 0,0,
	 1,1,
	 draw_object,
	 clear_object,
	 move_object,
	 set_object_speed
 };
 
void main(void)
{
	 POBJECT p = &ball;
	 init_app();
	 graphic_initalize();
 #ifndef SIMULATOR
	 graphic_clear_screen();
 #endif
	 p->set_speed(p, 4, 1);
	 while(1){
		 p->move(p);
		 delay_milli(40); /*25 "bilder/sekund" */
	 }
}

