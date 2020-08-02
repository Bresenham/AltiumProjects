#include "stm32f10x.h"
#include <stdbool.h>
#include <stdint.h>
#include "DS18B20.h"
#include "GlobalHelperFunctions.h"

void init_led();
void init_timer();
void led_on();
void led_off();

struct DS18B20 ds18b20;

static bool is_led_on = true;
uint32_t sysclk_in_mhz;

void init_led() {
		/* Enable Clock for GPIO Port C */
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
	/* GPIO Port C Pin 13 Push-Pull */
	GPIOC->CRH &= ~( GPIO_CRH_CNF13_0 | GPIO_CRH_CNF13_1 );
	
	/* GPIO Port C Pin 13 50MHz Output Mode */
	GPIOC->CRH |= ( GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1 );
}

void init_timer() {
	/* Enable Clock for Timer 2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	/* Set Timer 2 Prescaler Value */
	TIM2->PSC = 4096;
	
	/* Set Timer 2 Auto Reload Value */
	TIM2->ARR = 1000*17;
	
	/* Timer 2 Update Interrupt Enable */
	TIM2->DIER |= TIM_DIER_UIE;
	
	/* Halt Timer 2 while Debugging */
	// DBGMCU->CR |= DBGMCU_CR_DBG_TIM2_STOP;
	
	/* Enable Timer 2 Interrupt */
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable Timer 2 */
	TIM2->CR1 |= TIM_CR1_CEN;

}

void led_on() {
	GPIOC->ODR &= (uint16_t)( ~GPIO_ODR_ODR13 );
}

void led_off() {
	GPIOC->ODR |= GPIO_ODR_ODR13;
}

void TIM2_IRQHandler() {
	TIM2->SR &= ~TIM_SR_UIF;
}

int main(void) {
	
	init_led();
	init_timer();
	
	sysclk_in_mhz = SystemCoreClock / 1000000;
	
	initDS18B20(&ds18b20);
	
	const uint64_t ds18b20_device_id = ds18b20.readDeviceID();
		
	while(true) {
		wait(60);
		GPIOC->ODR ^= GPIO_ODR_ODR13;
		wait(60);
		GPIOC->ODR ^= GPIO_ODR_ODR13;
	}
}
