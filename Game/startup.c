/*
 * 	startup.c
 *
 */
 
 #include "tRex.xbm"
 
__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
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
 
 
 void graphic_write_command(uint8_t command, uint8_t controller);
 void graphic_clear_screen(void);
 void pixel(unsigned x, unsigned y, unsigned set);
 
 
 typedef struct tSprite{
	 unsigned char width;
	 unsigned char height;
	 unsigned char* data;
 } sprite;
 
  typedef struct tObj{
	 PGEOMETRY geo;
	 int dirx,diry;
	 int posx,posy;
	 void (*draw) (struct tObj *);
	 void (*clear) (struct tObj *);
	 void (*move) (struct tObj *);
	 void (*set_speed)(struct tObj *, int, int);
 } OBJECT, *POBJECT;
 
 static void load_sprite(sprite* s, unsigned char* data, int width, int height);
 void draw_sprite(sprite* s, int x, int y, int set);
 void init_app(void);
 
 
 
 
 
 void graphic_write_command(uint8_t command, uint8_t controller){
	 graphic_ctrl_bit_clear(B_E);
	 select_controller(controller);
	 graphic_ctrl_bit_clear(B_RS);
	 graphic_ctrl_bit_clear(B_RW);
	 graphic_write(command, controller);
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
 
 static void load_sprite(sprite* s, unsigned char* data, int width, int height){
	 s->width = width;
	 s->height = height;
	 s->data = data;
 }
 
 void draw_sprite(sprite* s, int x, int y, int set) {
	 int width_in_bytes;
	 
	 if(s->width % 8 == 0){
		 width_in_bytes = s->width /8;
	 } else{
		 width_in_bytes = s->width / 8 + 1; 
	 }
	 for(int i = 0; i < s->height; i++){
		 for(int j = 0; j < width_in_bytes; j++){
			 unsigned char byte = s->data[i * width_in_bytes + j];
			 for(int k = 0; k < 8; k++){
				 if(byte & (1<<k)){
					 
				 }
			 }
		 }
	 }
 }
 

 
 init_app(void){
	 *GPIO_E_MODER = 0x55555555;
 }

void main(void)
{
	 init_app();
	 graphic_initalize();
	 graphic_clear_screen();
	 draw_sprite();
	 
	 while(1){
		 
	 }
}

