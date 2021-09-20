#include <SI_EFM8BB1_Defs.h>
#include <SI_EFM8BB1_Register_Enums.h>

#include <stdint.h>
#include <stdbool.h>

void SiLabs_Startup (void) {
  WDTCN = 0xDE;
  WDTCN = 0xAD;
}


volatile bool timer0_irq = false;

SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn) {

  timer0_irq = true;

}

int main (void) {
  // Clock configuration
  CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_1 | CLKSEL_CLKSL__HFOSC;

  // Enable Crossbar 2
  XBR2 = XBR2_XBARE__ENABLED;

  // Configure P1.1 as digital push-pull output
  PRTDRV = PRTDRV_P1DRV__HIGH_DRIVE;
  P1MDIN = P1MDIN_B1__DIGITAL;
  P1MDOUT = P1MDOUT_B1__PUSH_PULL;
  P1 = P1_B1__LOW;

  // Configure Timer 0 to toggle P1.1 every second
  CKCON0 = CKCON0_T0M__SYSCLK | CKCON0_SCA__SYSCLK_DIV_48;
  TMOD = TMOD_GATE0__DISABLED | TMOD_CT0__TIMER | TMOD_T0M__MODE1;
  TL0 = 0;
  TH0 = 0;

  // Enable Timer 0 interrupt
  IE = IE_EA__ENABLED;
  IE |= IE_ET0__ENABLED;

  // Start Timer0
  TCON = TCON_TR0__RUN;

  while (1) {
      if( timer0_irq ) {
          P1_B1 ^= 1;
          timer0_irq = false;
      }
  }
}
