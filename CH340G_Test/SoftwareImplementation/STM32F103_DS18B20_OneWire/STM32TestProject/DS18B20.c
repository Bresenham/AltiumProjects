#include "stm32f10x.h"
#include "DS18B20.h"

#include "GlobalHelperFunctions.h"

void setReadMode() {
	/* Set Input with Pull-Up/Pull-Down */
	GPIOC->CRH |= GPIO_CRH_CNF14_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF14_0;
	
	/* Set Pull-Up According to Table 20 */
	GPIOC->ODR |= GPIO_ODR_ODR14;
}

void setWriteMode() {
	/* Set Open-Drain Output */
	GPIOC->CRH |= GPIO_CRH_CNF14_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF14_1;
	
	/* Set 50MHz Output-Mode */
	GPIOC->CRH |= ( GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1 );
}

bool startCommunication(struct DS18B20 *self) {
	
	setWriteMode();
	
	/* Begin Master Tx Reset Pulse By Pulling 1-Wire Line Low */
	GPIOC->ODR &= ~GPIO_ODR_ODR14;
	
	/* Wait For At Least 480µs */
	wait(480);
	
	/* End Master Tx Reset Pulse By Releasing 1-Wire Line */
	GPIOC->ODR |= GPIO_ODR_ODR14;
	
	setReadMode();
	
	/* Let DS18B20 Delay Time Elapse */
	wait(60);
	
	/* 1-Wire Should Definitely Be Low Now */
	return GPIOC->IDR & GPIO_IDR_IDR14;
}

void initDS18B20(struct DS18B20 *self) {
	self->startCommunication = &startCommunication;
}