#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#define CPU_CLOCK		80000000UL
#define DEC_CLOCK_VALUE 300000UL
#define TIME_1SEC		CPU_CLOCK
#define TIME_20MS		CPU_CLOCK / 50
#define TIME_1MS		CPU_CLOCK / 1000
#define TIME_10US		CPU_CLOCK / 100000
#define ON				0x01UL
#define OFF				0x00UL
#define HIGH			0x01UL
#define LOW				0x00UL

volatile unsigned long ulServoTime[] = {
		TIME_1MS * 19, TIME_1MS
};
volatile long ulTickTime = 0;

void PinOut(unsigned long ulLevel)
{
	if(ulLevel)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0x04);
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x10);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0x00);
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x00);
	}
}

void Timer0IntHandler(void)
{

	static unsigned long ulTimePeriod = 0;
	ulTickTime++;

	// Clear the Timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	TimerLoadSet(TIMER0_BASE, TIMER_A, ulServoTime[ulTimePeriod]);

	if(ulTimePeriod == 0)
	{
		PinOut(LOW);
		ulTimePeriod = 1;
	}
	else
	{
		PinOut(HIGH);
		ulTimePeriod = 0;
	}
}

int main(void)
{
	unsigned long ulPeriod;

	// Set main clock speed 80MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// Set GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF | SYSCTL_PERIPH_GPIOA | SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);


	// Set Timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);

	//ulPeriod = SysCtlClockGet() / 2 / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, TIME_1MS);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	volatile unsigned long ulTempServoTime = ulServoTime[1];

	while(1)
	{
		if(ulTickTime > 0)
		{
			ulTempServoTime = ulTempServoTime + (0.001 * TIME_1MS);
			ulTickTime = 0;
		}

		if(ulTempServoTime > (TIME_1MS * 2 + 0.5 * TIME_1MS))
		{
			ulTempServoTime = TIME_1MS;
			ulTickTime = -500;
		}

		if((ulTickTime == 0) || (ulTickTime == -500))
		{
			ulServoTime[0] = TIME_20MS - ulTempServoTime;
			ulServoTime[1] = ulTempServoTime;
		}
	}
}

