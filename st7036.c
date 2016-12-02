#include "st7036.h"

const pin_t rs = {GPIOA, GPIO7};
const pin_t rw = {GPIOA, GPIO6};
const pin_t en = {GPIOA, GPIO5};
const pin_t db[] = {{GPIOA, GPIO4}, {GPIOA, GPIO3}, {GPIOA, GPIO2}, {GPIOA, GPIO1}};

/* Private interface */
static void commandWrite(int cmd);
static void senable(void);
static void delay(int delay_ms);
static void udelay(int delay_us);
	
/**
 * Public interface
 */

int st7036_Init(void)
{
	int i;

	/* Initialize pins */
	rcc_periph_clock_enable(RCC_GPIOA);

	gpio_set_mode(rs.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, rs.pin);
	gpio_set_mode(rw.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, rw.pin);
	gpio_set_mode(en.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, en.pin);

	for(i=0; i<4; i++)
		gpio_set_mode(db[i].port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, db[i].pin);

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

	/* Set Contrast lowest bits */
	commandWrite(7);
	commandWrite(1);
	delay(20);

	/* Set Contrast highest bits */
	commandWrite(7);
	commandWrite(0x7);
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

	st7036_Clear();
	st7036_Home();
	
	st7036_dataWrite('S');
	st7036_dataWrite('T');
	st7036_dataWrite('M');
	st7036_dataWrite('3');
	st7036_dataWrite('2');
	st7036_dataWrite('F');
	st7036_dataWrite('r');
	st7036_dataWrite('q');
	
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
	int divisor = 10000000;
	
	while(divisor){
		st7036_dataWrite('0' + ((data/divisor) % 10));
		divisor /= 10;
	}

	return 0;
}

/** Private interface
 */

/* Very poor delay function */
#define CYCLES_MS (72000/4)
#define CYCLES_uS (72/4)
void delay(int delay_ms)
{
	int i;
	for (i = 0; i < delay_ms * CYCLES_MS; i++)
		__asm__("nop");
}

void udelay(int delay_us)
{
	int i;
	for (i = 0; i < delay_us * CYCLES_uS; i++)
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

