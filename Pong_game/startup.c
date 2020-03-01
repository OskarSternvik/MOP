/*
 * 	startup.c
 *
 */
 
#include "Pong.h"
 
__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}

void delay_250ns(void){
	#ifdef SIMULATOR
		return;
	#endif
		/* SystemCoreClock = 16800000 */
		*STK_CTRL = 0;
		*STK_LOAD = ((168/4)-1);
		*STK_VAL = 0;
		*STK_CTRL = 5;
		while((*STK_CTRL & 10000) == 0){}
		*STK_CTRL = 0;
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

 void ascii_ctrl_bit_set(unsigned char x){
	 CTRL_WR |= (B_SELECT | x);
 }
 
 void ascii_ctrl_bit_clear(unsigned char x){
	 CTRL_WR |= (B_SELECT | ~x);
 }
 
 void ascii_write_controller(unsigned char byte){
	 delay_250ns();
	 ascii_ctrl_bit_set(B_E);
	 DATA_WR = byte;
	 delay_250ns();
	 ascii_ctrl_bit_clear(B_E);
	 delay_250ns();
 }
 
 unsigned char ascii_read_controller(void){
	 unsigned char rv;
	 ascii_ctrl_bit_set(B_E);
	 delay_250ns();
	 delay_250ns();
	 rv = DATA_RD;
	 ascii_ctrl_bit_clear(B_E);
	 return rv;
 }
 
 unsigned char ascii_read_status(){
	 unsigned char rv;
	 *(GPIO_E_MODER) = 0x00005555;
	 ascii_ctrl_bit_clear(B_RS);
	 ascii_ctrl_bit_set(B_RW);
	 rv = ascii_read_controller();
	 *(GPIO_E_MODER) = 0x55555555;
	 return rv;
 }
 
 void ascii_write_cmd(unsigned char command){
	 ascii_ctrl_bit_clear(B_RS);
	 ascii_ctrl_bit_clear(B_RW);
	 ascii_write_controller(command);
 }
 
 void ascii_write_data(unsigned char data){
	 ascii_ctrl_bit_set(B_RS);
	 ascii_ctrl_bit_clear(B_RW);
	 ascii_write_controller(data);
 }
 
 void ascii_init(void){
	 while((ascii_read_status() & 0x80) == 0x80) {}
	 delay_mikro(8);
	 ascii_write_cmd(0x38);
	 delay_mikro(40);
	 while((ascii_read_status() & 0x80) == 0x80) {}
	 delay_mikro(8);
	 ascii_write_cmd(0x0C);
	 delay_mikro(40);
	 while((ascii_read_status() & 0x80) == 0x80) {}
	 delay_mikro(8);
	 ascii_write_cmd(0x01);
	 delay_milli(2);
	 while((ascii_read_status() & 0x80) == 0x80) {}
	 delay_mikro(8);
	 ascii_write_cmd(0x06);
	 delay_mikro(40);
 }
 
 void ascii_gotoxy(int x, int y){
	 unsigned char address;
	 address = x - 1;
	 if( y == 2) address = address | 0x40;
	 ascii_write_cmd(0x80 | address);
 }
 
 void ascii_write_char(unsigned char c){
	 while((ascii_read_status() & 0x80) == 0x80) {}
	 delay_mikro(8);
	 ascii_write_data(c);
	 delay_mikro(40);
 }
 
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
 
 void graphic_initialize(void){
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
 
 void graphic_draw_screen(void){
	 uint8_t i, j, controller, c;
	 unsigned int k = 0;
	 
	 for(c = 0; c < 2; c++){
		 controller = (c == 0) ? B_CS1 : B_CS2;
		 for(j = 0; j < 8; j++){
			 graphic_write_command(LCD_SET_PAGE | j, controller);
			 graphic_write_command(LCD_SET_ADD  | 0, controller);
			 for(i = 0; i <= 63; i++, k++){
				 graphic_write_data(backBuffer[k], controller);
			 }
		 }
	 }
 }
 
 /*void pixel(unsigned char x, unsigned char y, unsigned char set) {
// Kontrollera att x och y är i tilåtna intervall. 
	if(x > 128 || x < 1 || y > 64 || y < 1) return;
	if(set != 0 && set != 1) return;

	uint8_t index = (y - 1) / 8;
	uint8_t mask;
	mask = 1 << ( (y - 1) % 8);
	if(set == 0){
		mask = ~mask; // biten ska nollställas
	}

	uint8_t controller;

	// Bestäm fysiska koordinater och välj styrkrets
	if(x > 64){
		controller = B_CS2;
		x = x - 65;
	} else{
		controller = B_CS1;
		x = x - 1;
	 }
	graphic_write_command(LCD_SET_ADD | x, controller);
	graphic_write_command(LCD_SET_PAGE | index, controller);
	uint8_t temp = graphic_read_data(controller);
	graphic_write_command(LCD_SET_ADD | x, controller);

	if(set == 1){
	mask = mask | temp;
	} else{
	mask = mask & temp;
	}
	graphic_write_data(mask, controller); 
 } */
 
 void pixel (int x, int y) {
	 uint8_t mask;
	 int index = 0;
	 if( (x > 128) || (x < 1) || (y > 64) || (y < 1)) return;
	 
	 mask = 1 << ((y-1) % 8);
	 
	 if(x > 64){
		 x -= 65;
		 index = 512;
	 }
	 
	 index += x + ((y-1)/8)*64;
	 
	 backBuffer[index] |= mask;
 }

 void clear_backBuffer(){
	 int i;
	 for(i =0;i <1024;i++) backBuffer[i]=0;
 }
 
 unsigned char keyb(void) {
	unsigned char key[]={1,2,3,0xA,4,5,6,0xB,7,8,9,0xC,0xE,0,0xF,0xD};
	int row, col;
	for (row=1; row <=4 ; row++ ) {
		kbdActivate( row );
		if( (col = kbdGetCol () ) != 0 ){
			kbdActivate(row);
			return key [4*(row-1)+(col-1) ];
		}
			kbdActivate( row);
			return key[4*(row-1) + (col-1)];
	}
	
	*GPIO_D_ODR_HIGH = 0;
	return 0xFF;
	
	
}

void kbdActivate( unsigned int row ){
	switch( row ) {
	case 1: *GPIO_D_ODR_HIGH = 0x10 ; break;
	case 2: *GPIO_D_ODR_HIGH = 0x20 ; break;
	case 3: *GPIO_D_ODR_HIGH = 0x40 ; break;
	case 4: *GPIO_D_ODR_HIGH = 0x80 ; break;
	case 0: *GPIO_D_ODR_HIGH = 0x00; break;
	}
}

int kbdGetCol(void){
	unsigned char c;
	c = *GPIO_D_IDR_HIGH;
	if ( c & 0x8 ) return 4;
	if ( c & 0x4 ) return 3;
	if ( c & 0x2 ) return 2;
	if ( c & 0x1 ) return 1;
	return 0;
}

 void set_object_speed(POBJECT o, int speedx, int speedy){
	 o->dirx = speedx;
	 o->diry = speedy;
 }
 void draw_object(POBJECT o){
	 for(int i = 0; i < o->geo->numpoints; i++){
		 //pixel(o->posx + o->geo->px[i].x, o->posy + o->geo->px[i].y, 1);
		 pixel(o->posx + o->geo->px[i].x, o->posy + o->geo->px[i].y);
	 }
 }
 void clear_object(POBJECT o){
	 for(int i = 0; i< o->geo->numpoints; i++){
		 //pixel(o->posx + o->geo->px[i].x, o->posy + o->geo->px[i].y, 0);
		 pixel(o->posx + o->geo->px[i].x, o->posy + o->geo->px[i].y);
	 }
 }
 void move_ball(POBJECT o){
	 //clear_object(o);
	 o->posx += o->dirx;
	 o->posy += o->diry;
	 if(o->posx < 1){
		 o->dirx *= -1;
	 }
	 if(o->posx > 128){
		 o->dirx *= -1;
	 }
	 if(o->posy < 3){
		 o->diry *= -1;
	 }
	 if(o->posy > 64){
		 o->diry *= -1;
	 }
	 //draw_object(o);
 }
 
 void move_paddle(POBJECT o){
	 o->posy += 1;
 }

void main(void){
	char* s;
	char test1[] = "This is MD407";
	char test2[] = "Hello world!!!!";
	
	POBJECT p = &ball;
	POBJECT b1 = &bar_1;
	POBJECT b2 = &bar_2;
	init_app();
	graphic_initialize();
	#ifndef SIMULATOR
	graphic_clear_screen();
	#endif
	
	ascii_init();
	ascii_gotoxy(1,1);
	s = test1;
	while(*s) ascii_write_char(*s++);
	ascii_gotoxy(1,2);
	s = test2;
	while(*s) ascii_write_char( *s++ );
		
	p->set_speed(p,4,1);
	b1->set_speed(b1,0,0);
	b2->set_speed(b2,0,0);
	while(1)
	{
		//clear_backBuffer();
		p->clear(p);
		p->move(p);
		p->draw(p);
		
		b1->clear(b1);
		b1->move(b1);
		b1->draw(b1);
		
		b2->clear(b2);
		b2->move(b2);
		b2->draw(b2);
		//graphic_draw_screen();
		delay_milli(40);
	}
}

