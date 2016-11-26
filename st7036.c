#include "st7036.h"

const pin_t rs = {GPIOA, GPIO1};
const pin_t rw = {GPIOA, GPIO2};
const pin_t en = {GPIOA, GPIO3};
const pin_t db[] = {{GPIOA, GPIO4}, {GPIOA, GPIO5}, {GPIOA, GPIO6}, {GPIOA, GPIO7}};

/* Private interface */
static void commandWrite(int cmd);
static void senable(void);
static void delay(int delay_ms);

/**
 * Public interface
 */

int st7036_Init(void)
{
	int i;

	/* Initialize pins */
	//rcc_periph_clock_enable(RCC_GPIOA);

	gpio_set_mode(rs.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, rs.pin);
	gpio_set_mode(rw.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, rw.pin);
	gpio_set_mode(en.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, en.pin);

	for(i=0; i<4; i++)
		gpio_set_mode(db[i].port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, db[i].pin);

	//return 0; //ok
	
	/* Initialize display */
	gpio_clear(rw.port, rw.pin);

	commandWrite(0x3);
	delay(64);
	commandWrite(0x3);
	delay(50);
	commandWrite(0x3);
	delay(20);
	commandWrite(2);
	delay(20);
	commandWrite(2);
	delay(20);
	commandWrite(1);
	delay(20);
	commandWrite(1);
	delay(20);
	commandWrite(4);
	delay(20);

	//return 0;//ok
	
	/* Set Contrast lowest bits */
	commandWrite(7);
	commandWrite(1);
	delay(20);

	/* Set Contrast highest bits */
	commandWrite(5);
	commandWrite(0xe);
	delay(20);

	/* Power on */
	commandWrite(6);
	delay(20);
	commandWrite(0xa);
	delay(20);
	commandWrite(0);
	delay(100);
	commandWrite(0xc);
	delay(100);
	commandWrite(0);
	delay(100);
	commandWrite(1);
	delay(100);
	commandWrite(0);
	delay(100);
	commandWrite(6);
	delay(20);
	
	//return 0; ok but before USB enumeration

	st7036_Clear();
	st7036_Home();

	//return 0;//nok
	
	st7036_dataWrite('H');
	st7036_dataWrite('e');
	st7036_dataWrite('l');
	st7036_dataWrite('l');
	st7036_dataWrite('o');
	
	return 0;
}


void st7036_Clear(void)
{
	commandWrite(0x00);
	commandWrite(0x01);
}


void st7036_Home(void)
{
	commandWrite(0x00);
	commandWrite(0x02);
}


int st7036_dataWrite(unsigned int data)
{
	int i;
	uint8_t hinibble, lownibble;
	
	gpio_set(rs.port, rs.pin);
	gpio_clear(rw.port, rw.pin);

	hinibble = data >> 4;
	lownibble = data & 0xf;

	for(i=0; i<4; i++){
		if(hinibble & 1)
			gpio_set(db[i].port, db[i].pin);
		else
			gpio_clear(db[i].port, db[i].pin);

		hinibble >>= 1;		
	}

	senable();

	
	for(i=0; i<4; i++){
		if(lownibble & 1)
			gpio_set(db[i].port, db[i].pin);
		else
			gpio_clear(db[i].port, db[i].pin);

		lownibble >>= 1;		
	}

	senable();
	
	return 0;
}


int st7036_integerWrite(int data)
{
	return 0;
}

/** Private interface
 */

/* Very poor delay function */
#define CYCLES_MS 72000
void delay(int delay_ms)
{
	int i;
	for (i = 0; i < delay_ms * CYCLES_MS; i++)
		__asm__("nop");
}

static void commandWrite(int cmd)
{
	int i;

	gpio_clear(rs.port, rs.pin);
	gpio_clear(rw.port, rw.pin);

	for(i=0; i<4; i++){
		if(cmd & 1)
			gpio_set(db[i].port, db[i].pin);
		else
			gpio_clear(db[i].port, db[i].pin);
		cmd >>= 1;
	}
	
	senable();
}


static void senable(void)
{
	gpio_set(en.port, en.pin);
	delay(1);
	gpio_clear(en.port, en.pin);
	delay(1);
	gpio_set(en.port, en.pin);
	delay(1);
}


