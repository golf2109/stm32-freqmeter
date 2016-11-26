/**
 * st7036 simple display Driver
 * Uses 4-bit interface. Display is powered with Vcc=5V, signals are 3V
 * Pinout

 Signal     Display Pin    STM32F4 GPIO
 RS         39             PA1
 RW         37             PA2
 EN         36             PA3
 D4         31             PA4
 D5         30             PA5
 D6         29             PA6
 D7         28             PA7
*/

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/usb/usbd.h>

typedef struct pin_t
{
	uint32_t port;
	uint16_t pin;
}pin_t;

int st7036_Init(void);
void st7036_Clear(void);
void st7036_Home(void);
int st7036_dataWrite(unsigned int data);
int st7036_integerWrite(int data);
