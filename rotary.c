/*
 * rotary.c
 *
 *  Created on: Oct 15, 2016
 *      Author: Kenny Zhou
 */

#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include "driverlib/adc.h"
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include "launchpad.h"
#include "rotary.h"

// initialize the port connection to the rotary angle sensor and set the output as an input to the ADC module
void
rotaryInit()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	//GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);                            // Configuring PE1 as ADC input (Jumper J9)
}

int
ADCinit()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); // enable ADC module 0 of the microcontroller

	uint32_t  digitalOutput = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1); //

	// Enable the first sample sequencer to capture the value of channel 0 when
	// the processor trigger occurs.
	//
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH2); // ch 2(AIN2) corresponds to portE pin 1
	ADCSequenceEnable(ADC0_BASE, 0);
	//
	// Trigger the sample sequence.
	//
	ADCProcessorTrigger(ADC0_BASE, 0);
	//
	// Wait until the sample sequence has completed.
	//
	while(!ADCIntStatus(ADC0_BASE, 0, false))
	{
	}
	//
	// Read the value from the ADC.
	//
	ADCSequenceDataGet(ADC0_BASE, 0, &digitalOutput);

	return digitalOutput;
}




