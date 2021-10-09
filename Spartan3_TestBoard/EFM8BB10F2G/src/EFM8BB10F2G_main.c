#include <SI_EFM8BB1_Defs.h>
#include <SI_EFM8BB1_Register_Enums.h>

#include <stdint.h>
#include <stdbool.h>

#define SYSCLK                  24500000UL
#define SPI0CLK                 1000000UL
#define UART_BAUD_RATE          19200UL

#define SPI0CKR_VAL             ( ( SYSCLK / (2 * SPI0CLK) ) - 0.5 )
#define UART_TIMER1_RELOAD_VAL  ( 0xFF - ( SYSCLK / (2 * 48 * UART_BAUD_RATE) ) )

void SiLabs_Startup (void) {
  WDTCN = 0xDE;
  WDTCN = 0xAD;
}

volatile uint8_t scaler = 0;
volatile bool timer0_irq = false;

SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn) {

  scaler += 1;
  if( scaler == 8 ) {
      timer0_irq = true;
      scaler = 0;
  }

}

int main (void) {

  uint8_t device_id = 0x00;
  volatile uint8_t tim = UART_TIMER1_RELOAD_VAL;

  // Clock configuration
  CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_1 | CLKSEL_CLKSL__HFOSC;

  // Enable UART pins TX=0.4, RX=0.5
  XBR0 = XBR0_URT0E__ENABLED;

  // Enable SPI0
  XBR0 |= XBR0_SPI0E__ENABLED;

  // Configure SPI0
  SPI0CFG = SPI0CFG_MSTEN__MASTER_ENABLED | SPI0CFG_CKPHA__DATA_CENTERED_FIRST | SPI0CFG_CKPOL__IDLE_LOW;
  SPI0CN0 = SPI0CN0_NSSMD__4_WIRE_MASTER_NSS_HIGH;
  SPI0CKR = SPI0CKR_VAL;
  SPI0CN0 |= SPI0CN0_SPIEN__ENABLED;

  // Enable Crossbar 2
  XBR2 = XBR2_XBARE__ENABLED;

  // Configure UART pins
  PRTDRV = PRTDRV_P0DRV__HIGH_DRIVE | PRTDRV_P1DRV__HIGH_DRIVE;
  P0MDIN = P0MDIN_B4__DIGITAL | P0MDIN_B5__DIGITAL;
  P0MDOUT = P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__PUSH_PULL;

  // Configure P1.1 as digital push-pull output
  P1MDIN = P1MDIN_B1__DIGITAL;
  P1MDOUT = P1MDOUT_B1__PUSH_PULL;
  P1 = P1_B1__LOW;

  // Configure Timer 0 to toggle P1.1 every second
  CKCON0 = CKCON0_T0M__PRESCALE | CKCON0_T1M__PRESCALE | CKCON0_SCA__SYSCLK_DIV_48;
  TMOD = TMOD_GATE0__DISABLED | TMOD_GATE1__DISABLED | TMOD_CT0__TIMER | TMOD_T0M__MODE1 | TMOD_CT1__TIMER | TMOD_T1M__MODE2;

  TL0 = 0;
  TH0 = 0;

  TL1 = 0;
  TH1 = UART_TIMER1_RELOAD_VAL;

  // Enable interrupt
  IE = IE_ET0__ENABLED | IE_EA__ENABLED;

  // Start Timer0 & Timer 1
  TCON = TCON_TR0__RUN | TCON_TR1__RUN;


  SPI0DAT = 0xAB;
  while( (SPI0CN0 & SPI0CN0_SPIF__BMASK) == 0 ) { ; }
  SPI0CN0 &= ~(SPI0CN0_SPIF__BMASK);
  device_id = SPI0DAT;

  SPI0DAT = 0x00;
  while( (SPI0CN0 & SPI0CN0_SPIF__BMASK) == 0 ) { ; }
  SPI0CN0 &= ~(SPI0CN0_SPIF__BMASK);
  device_id = SPI0DAT;

  SPI0DAT = 0x00;
  while( (SPI0CN0 & SPI0CN0_SPIF__BMASK) == 0 ) { ; }
  SPI0CN0 &= ~(SPI0CN0_SPIF__BMASK);
  device_id = SPI0DAT;

  SPI0DAT = 0x00;
  while( (SPI0CN0 & SPI0CN0_SPIF__BMASK) == 0 ) { ; }
  SPI0CN0 &= ~(SPI0CN0_SPIF__BMASK);
  device_id = SPI0DAT;

  SPI0DAT = 0x00;
  while( (SPI0CN0 & SPI0CN0_SPIF__BMASK) == 0 ) { ; }
  SPI0CN0 &= ~(SPI0CN0_SPIF__BMASK);
  device_id = SPI0DAT;


  while (1) {
      if( timer0_irq ) {
          P1_B1 ^= 1;
          SBUF0 = device_id;
          timer0_irq = false;
      }
  }
}
