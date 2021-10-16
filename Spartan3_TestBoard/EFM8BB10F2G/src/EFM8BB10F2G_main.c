#include <SI_EFM8BB1_Defs.h>
#include <SI_EFM8BB1_Register_Enums.h>

#include <stdint.h>
#include <stdbool.h>

#include "SPI/SPI.h"
#include "UART/UART.h"

#define SYSCLK                  24500000UL
#define SPI0CLK                 1000000UL
#define UART_BAUD_RATE          19200UL
#define SYSCLK_DIV              48UL

#define UART_TIMER1_RELOAD_VAL  ( 0xFF - ( SYSCLK / (2 * SYSCLK_DIV * UART_BAUD_RATE) ) )
#define SPI0CKR_VAL             ( ( SYSCLK / (2 * SPI0CLK) ) - 0.5 )


const uint8_t CMD_RELEASE_PW_DOWN_ID[] = { 0x9F, 0x00, 0x00, 0x00 };

void SiLabs_Startup (void) {
  WDTCN = 0xDE;
  WDTCN = 0xAD;
}


volatile struct TRANSFER spi_transfer;


volatile uint8_t scaler = 0;
volatile bool timer0_irq = false;
SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn) {

  scaler += 1;
  if( scaler == 8 ) {
      timer0_irq = true;
      scaler = 0;
  }
}

SI_INTERRUPT (UART_ISR, UART0_IRQn) {

  /* Transmit completed interrupt */
  if( SCON0 & SCON0_TI__BMASK ) {

      SCON0 &= ~SCON0_TI__BMASK;

      uart_transmit_handle_irq();
  }
}

void uart_on_transmit_finished_callback(struct TRANSFER *trans) {
  return;
}

void uart_on_receive_finished_callback(struct TRANSFER *recv) {
  return;
}

SI_INTERRUPT (SPI_ISR, SPI0_IRQn) {

  if( SPI0CN0 & SPI0CN0_SPIF__BMASK ) {
      spi_transmit_receive_handle_irq();
  }
}

void spi_on_transmit_receive_finished_callback(struct TRANSFER *transfer) {

  transfer->data_transmit = transfer->data_recv;
  transfer->data_idx = 0;
  uart_transmit_start(transfer);
}


int main (void) {

  uint8_t recv[10];

  spi_transfer.data_idx = 0;
  spi_transfer.data_len = sizeof(CMD_RELEASE_PW_DOWN_ID);
  spi_transfer.data_recv = recv;
  spi_transfer.data_transmit = CMD_RELEASE_PW_DOWN_ID;

  // Clock configuration
  CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_1 | CLKSEL_CLKSL__HFOSC;

  // Crossbar peripheral configuration
  XBR0 = XBR0_URT0E__ENABLED | XBR0_SPI0E__ENABLED | XBR0_SMB0E__DISABLED
      | XBR0_CP0E__DISABLED | XBR0_CP0AE__DISABLED | XBR0_CP1E__DISABLED
      | XBR0_CP1AE__DISABLED | XBR0_SYSCKE__DISABLED;

  // Enable Crossbar 2
  XBR2 = XBR2_XBARE__ENABLED;

  // Configure SPI0 pins
  PRTDRV = PRTDRV_P0DRV__HIGH_DRIVE | PRTDRV_P1DRV__HIGH_DRIVE;
  P0MDIN = P0MDIN_B0__DIGITAL | P0MDIN_B1__DIGITAL | P0MDIN_B2__DIGITAL | P0MDIN_B3__DIGITAL;
  P0MDOUT = P0MDOUT_B0__PUSH_PULL | P0MDOUT_B1__OPEN_DRAIN | P0MDOUT_B2__PUSH_PULL | P0MDOUT_B3__PUSH_PULL;

  // Configure SPI0
  SPI0CFG |= SPI0CFG_MSTEN__MASTER_ENABLED;
  SPI0CKR = SPI0CKR_VAL;
  SPI0CN0 |= SPI0CN0_SPIEN__ENABLED | SPI0CN0_NSSMD__4_WIRE_MASTER_NSS_HIGH;

  // Configure UART pins
  P0MDIN |= P0MDIN_B4__DIGITAL | P0MDIN_B5__DIGITAL;
  P0MDOUT |= P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__PUSH_PULL;

  // Configure P1.1 as digital push-pull output
  P1MDIN = P1MDIN_B1__DIGITAL;
  P1MDOUT = P1MDOUT_B1__PUSH_PULL;
  P1 = P1_B1__LOW;

  // Configure Timer 0 to toggle P1.1 every second
  CKCON0 = CKCON0_T0M__PRESCALE | CKCON0_T1M__PRESCALE | CKCON0_SCA__SYSCLK_DIV_48;
  TMOD = TMOD_GATE0__DISABLED | TMOD_GATE1__DISABLED | TMOD_CT0__TIMER | TMOD_T0M__MODE1 | TMOD_CT1__TIMER | TMOD_T1M__MODE2;

  TL0 = 0;
  TH0 = 0;

  // UART Timer
  TL1 = 0;
  TH1 = UART_TIMER1_RELOAD_VAL;

  // Enable Timer 0, UART and SPI0 interrupt
  IE = IE_ESPI0__ENABLED | IE_ET0__ENABLED | IE_ES0__ENABLED;

  // Enable all interrupts
  IE |= IE_EA__ENABLED;

  // Start Timer0 & Timer 1
  TCON = TCON_TR0__RUN | TCON_TR1__RUN;

  while (1) {
      if( timer0_irq ) {
          P1_B1 ^= 1;
          spi_transmit_receive_start(&spi_transfer);
          timer0_irq = false;
      }
  }
}
