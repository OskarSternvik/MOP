
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
 

 #define STK_CTRL ((volatile unsigned int*)(0xE000E010))
 #define STK_LOAD ((volatile unsigned int*)(0xE000E014))
 #define STK_VAL  ((volatile unsigned int*)(0xE000E018))
 
 #define MAX_POINTS 53
 
//#define SIMULATOR
#define CTRL_RD *(GPIO_E_IDR_LOW)
#define CTRL_WR *(GPIO_E_ODR_LOW)
#define DATA_RD *(GPIO_E_IDR_HIGH)
#define DATA_WR *(GPIO_E_ODR_HIGH)

typedef unsigned int uint32_t;
 
 #define GPIO_D_BASE 0x40020C00
 #define GPIO_D_MODER    ((volatile unsigned int* )  (GPIO_D_BASE+0x00))
 #define GPIO_D_OTYPER   ((volatile unsigned short* )(GPIO_D_BASE+0x04))
 #define GPIO_D_OSPEEDR  ((volatile unsigned int* )  (GPIO_D_BASE+0x08))
 #define GPIO_D_PUPDR    ((volatile unsigned int* )  (GPIO_D_BASE+0x0C))
 
 #define GPIO_D_IDR_LOW  ((volatile unsigned char* ) (GPIO_D_BASE+0x10))
 #define GPIO_D_IDR_HIGH ((volatile unsigned char* ) (GPIO_D_BASE+0x11))
 #define GPIO_D_ODR_LOW  ((volatile unsigned char* ) (GPIO_D_BASE+0x14))
 #define GPIO_D_ODR_HIGH ((volatile unsigned char* ) (GPIO_D_BASE+0x15))
 
 
void delay_250ns(void);
void delay_500ns(void);
void delay_mikro(unsigned int us);
void delay_milli(unsigned int ms);

	typedef unsigned char uint8_t;

void ascii_ctrl_bit_set(unsigned char x);

void ascii_ctrl_bit_clear(unsigned char x);

void ascii_write_controller(unsigned char byte);

unsigned char ascii_read_controller(void);

unsigned char ascii_read_status();

void ascii_write_cmd(unsigned char command);

void ascii_write_data(unsigned char data);

void ascii_init(void);

void ascii_gotoxy(int x, int y);

void ascii_write_char(unsigned char c);



void graphic_ctrl_bit_set(uint8_t x);

void graphic_ctrl_bit_clear(uint8_t x);

static void select_controller(uint8_t controller);

void graphic_wait_ready(void);

uint8_t graphic_read(uint8_t controller);

void graphic_write(uint8_t value, uint8_t controller);

void graphic_write_command(uint8_t command, uint8_t controller);

void graphic_write_data(uint8_t data, uint8_t controller);

unsigned char graphic_read_data(unsigned char controller);

void init_app();

void graphic_initialize(void);

void graphic_clear_screen(void);

void graphic_draw_screen(void);

//void pixel(unsigned char x, unsigned char y, unsigned char set);

void pixel(int x, int y);

	uint8_t backBuffer[1024];// 128 * 64 / 8

void clear_backBuffer();

unsigned char keyb(void);

void kbdActivate( unsigned int row );

int kbdGetCol ( void );



 typedef struct tPoint{
	 unsigned char x;
	 unsigned char y;
 } POINT;
 
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
 
 GEOMETRY bar_1_geometry ={
	.numpoints = 52,
	.sizex = 2,
	.sizey = 26,
	{
		{0,0},{1,0},
		{0,1},{1,1},
		{0,2},{1,2},
		{0,3},{1,3},
		{0,4},{1,4},
		{0,5},{1,5},
		{0,6},{1,6},
		{0,7},{1,7},
		{0,8},{1,8},
		{0,9},{1,9},
		{0,10},{1,10},
		{0,11},{1,11},
		{0,12},{1,12},
		{0,13},{1,13},
		{0,14},{1,14},
		{0,15},{1,15},
		{0,16},{1,16},
		{0,17},{1,17},
		{0,18},{1,18},
		{0,19},{1,19},
		{0,20},{1,20},
		{0,21},{1,21},
		{0,22},{1,22},
		{0,23},{1,23},
		{0,24},{1,24},
		{0,25},{1,25}
	}
};

/*GEOMETRY bar_2_geometry ={
	.numpoints = 52,
	.sizex = 2,
	.sizey = 26,
	{
		{126,0},{127,0},
		{126,1},{127,1},
		{126,2},{127,2},
		{126,3},{127,3},
		{126,4},{127,4},
		{126,5},{127,5},
		{126,6},{127,6},
		{126,7},{127,7},
		{126,8},{127,8},
		{126,9},{127,9},
		{126,10},{127,10},
		{126,11},{127,11},
		{126,12},{127,12},
		{126,13},{127,13},
		{126,14},{127,14},
		{126,15},{127,15},
		{126,16},{127,16},
		{126,17},{127,17},
		{126,18},{127,18},
		{126,19},{127,19},
		{126,20},{127,20},
		{126,21},{127,21},
		{126,22},{127,22},
		{126,23},{127,23},
		{126,24},{127,24},
		{126,25},{127,25}
	}
}; */
 
 typedef struct tObj{
	 PGEOMETRY geo;
	 int dirx,diry;
	 int posx,posy;
	 void (*draw) (struct tObj *);
	 void (*clear) (struct tObj *);
	 void (*move) (struct tObj *);
	 void (*set_speed)(struct tObj *, int, int);
 } OBJECT, *POBJECT;
 
 void set_object_speed(POBJECT o, int speedx, int speedy);
 
 void draw_object(POBJECT o);
 
 void clear_object(POBJECT o);
 
 void move_ball(POBJECT o);
 
 void move_paddle(POBJECT o);
 
 static OBJECT ball =
 {
	 &ball_geometry,
	 0,0,
	 1,1,
	 draw_object,
	 clear_object,
	 move_ball,
	 set_object_speed
 };
 
 static OBJECT bar_1 =
 {
	 &bar_1_geometry,
	 0,0,
	 10,0,
	 draw_object,
	 clear_object,
	 move_paddle,
	 set_object_speed
 };
 
  static OBJECT bar_2 =
 {
	 &bar_1_geometry,
	 0,0,
	 115,0,
	 draw_object,
	 clear_object,
	 move_paddle,
	 set_object_speed
 };