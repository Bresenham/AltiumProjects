#include "GlobalHelperFunctions.h"
#include "stm32f10x.h"

void wait(const uint32_t us) {
	
	/* Measured with Logic Analyzer */
	volatile uint32_t amount_of_loops = ( sysclk_in_mhz * us ) / 13;
	
	while(amount_of_loops > 0) {
		amount_of_loops--;
	}
}
