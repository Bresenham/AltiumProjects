// --- Global Variables ---
// (In Padauk Mini-C, 'BYTE' is a native keyword, no headers needed)
BYTE pwm_counter;
BYTE duty1;
BYTE duty2;
BYTE duty3;
BYTE adc_val;
BYTE temp;      // Used for math operations since we have no hardware multiplier

// --- Interrupt Service Routine for Software PWM ---
void Interrupt(void)
{
    // Save ALU and FLAG registers
    pushaf;

    // Check if Timer16 interrupt occurred (bit 2 of intrq)
    if (intrq.2)
    {
        intrq.2 = 0; // Clear the interrupt flag
        pwm_counter++;

        // PWM for LED 1 (PA4 - Pin 6)
        if (pwm_counter < duty1) {
            pa.4 = 1; // LED ON
        } else {
            pa.4 = 0; // LED OFF
        }

        // PWM for LED 2 (PA3 - Pin 5)
        if (pwm_counter < duty2) {
            pa.3 = 1; // LED ON
        } else {
            pa.3 = 0; // LED OFF
        }

        // PWM for LED 3 (PA7 - Pin 2)
        if (pwm_counter < duty3) {
            pa.7 = 1; // LED ON
        } else {
            pa.7 = 0; // LED OFF
        }
    }

    // Restore ALU and FLAG registers
    popaf;
}

// --- Main Program (Entry point for Padauk Core 0) ---
void FPPA0(void)
{
    // --- SYSTEM CLOCK CONFIGURATION ---
    // Calibrate the internal RC oscillator (IHRC) to 16MHz at 3.3V.
    // Set System Clock (SYSCLK) to IHRC / 2 = 8MHz.
    // Note: This macro also automatically disables the Watchdog Timer during startup.
    .ADJUST_IC SYSCLK=IHRC/2, IHRC=16MHz, VDD=3.3V;

    // Initialize Variables
    pwm_counter = 0;
    duty1 = 0;
    duty2 = 0;
    duty3 = 0;

    // --- SETUP IO PINS ---
    pac = 0b1001_1000;  // PA4, PA3, PA7 as outputs
    pa = 0x00;          // All LEDs OFF
    paph = 0x00;        // Disable internal pull-ups
    padier = 0b1111_1000; // Disable digital input buffer on PA0 (Bit 0 = 0)

    // --- INITIAL BLINK SEQUENCE ---
    // 8 MHz sysclk -> 1 cycle = 125ns -> 4,000,000 cycles = 500ms

    // LED 1 (Pin 6 = PA4) - blink once
    pa.4 = 1; 
    .delay 4000000; 
    pa.4 = 0; 
    .delay 4000000;
    pa.4 = 1; 
    .delay 4000000;

    // LED 2 (Pin 5 = PA3) - blink twice
    pa.3 = 1; 
    .delay 4000000; 
    pa.3 = 0; 
    .delay 4000000;
    pa.3 = 1; 
    .delay 4000000; 
    pa.3 = 0; 
    .delay 4000000;
    pa.3 = 1; 
    .delay 4000000;

    // LED 3 (Pin 2 = PA7) - blink three times
    pa.7 = 1; 
    .delay 4000000; 
    pa.7 = 0; 
    .delay 4000000;
    pa.7 = 1; 
    .delay 4000000; 
    pa.7 = 0; 
    .delay 4000000;
    pa.7 = 1; 
    .delay 4000000; 
    pa.7 = 0; 
    .delay 4000000;
    pa.7 = 1; 
    .delay 4000000;

	pa.4 = 0;
	pa.3 = 0;
	pa.7 = 0;

    // --- SETUP ADC MANUALLY ---
    // adcc = Enable(bit 7) | Process Control(bit 6) | Channel PA0(bits 5:2)
    // Channel PA0 is 1010. Therefore 1000_0000 | 0010_1000 = 0b1010_1000
    adcc = 0b1010_1000;
    // adcrgc = VDD reference (bit 7 = 0)
    adcrgc = 0x00;
    // adcm = Clock source SYSCLK/16 (bits 3:1 = 100)
    $ ADCM 8BIT, /16; 

    // --- SETUP TIMER16 MANUALLY ---
    // t16m = IHRC (bits 7:5 = 100) | /1 divider (bits 4:3 = 00) | Bit 8 interrupt (bits 2:0 = 000)
    t16m = 0b1000_0000; 
    
    intrq = 0;          // Clear pending interrupts
    inten.2 = 1;        // Enable Timer16 interrupt
    engint;             // Enable Global Interrupts

    // --- MAIN LOOP ---
    while (1)
    {
        // 1. Trigger ADC Conversion
		adcc |= 0b0100_0000;  // Set adcc.6 to 1 to start conversion

		nop; nop;             // Give hardware 2 cycles to pull the ready bit low internally
        
        // Wait for hardware to set bit 6 back to 1 (conversion done)
        while(!(adcc & 0b0100_0000)) 
        {
            // Waiting...
        }
        
        adc_val = adcr;       // Store 8-bit result

        // 2. Map 0-255 ADC value to three 0-255 PWM duty cycles
        if (adc_val <= 85)
        {
            duty1 = adc_val + adc_val + adc_val;
            duty2 = 0;
            duty3 = 0;
        }
        else if (adc_val <= 170)
        {
            duty1 = 255;
            temp = adc_val - 85;
            duty2 = temp + temp + temp;
            duty3 = 0;
        }
        else
        {
            duty1 = 255;
            duty2 = 255;
            temp = adc_val - 170;
            duty3 = temp + temp + temp;
        }
        
        // Brief pause before reading ADC again
        .delay 40; 
    }
}