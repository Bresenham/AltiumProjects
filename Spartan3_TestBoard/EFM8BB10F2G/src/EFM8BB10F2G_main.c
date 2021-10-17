#include <SI_EFM8BB1_Defs.h>
#include <SI_EFM8BB1_Register_Enums.h>

#include <stdint.h>
#include <stdbool.h>

#include "SPI/SPI.h"
#include "UART/UART.h"
#include "W25Q32JV/W25Q32JV.h"

#define SYSCLK                  24500000UL
#define SPI0CLK                 1000000UL
#define UART_BAUD_RATE          19200UL
#define SYSCLK_DIV              48UL

#define UART_TIMER1_RELOAD_VAL  ( 0xFF - ( SYSCLK / (2 * SYSCLK_DIV * UART_BAUD_RATE) ) )
#define SPI0CKR_VAL             ( ( SYSCLK / (2 * SPI0CLK) ) - 0.5 )

#define AMOUNT_OF_TRANSFERS     5

volatile uint8_t transfer_idx = 0;
uint8_t idata trans_mem[AMOUNT_OF_TRANSFERS * TRANSFER_MAX_LEN];
uint8_t idata recv_mem[AMOUNT_OF_TRANSFERS * TRANSFER_MAX_LEN];
struct TRANSFER transfers[AMOUNT_OF_TRANSFERS];

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

void uart_on_transmit_finished_callback(struct TRANSFER *trans) {
  return;
}

void uart_on_receive_finished_callback(struct TRANSFER *recv) {
  return;
}

void w25q32jv_request_finished(struct TRANSFER *trans) {

  if( trans->type == W25Q32JV_READ_FROM_ADDR ) {
      trans->data_trans = trans->data_recv;
      trans->data_idx = 0;
      uart_transmit_start(trans);
  } else if( transfer_idx < 4 ) {
      w25q32jv_execute_transfer(&transfers[transfer_idx++]);
  }
}

int main (void) {

  uint8_t i;
  for(i = 0; i < AMOUNT_OF_TRANSFERS; ++i) {
      transfers[i].data_recv = &recv_mem[i * TRANSFER_MAX_LEN];
      transfers[i].data_trans = &trans_mem[i * TRANSFER_MAX_LEN];
  }

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

  w25q32jv_write_enable(&transfers[0]);
  w25q32jv_sector_erase(0x00000000, &transfers[1]);
  w25q32jv_write_enable(&transfers[2]);
  w25q32jv_write_byte_to_addr(0x00000000, 0x5A, &transfers[3]);
  w25q32jv_read_byte_from_addr(0x00000000, &transfers[4]);

  w25q32jv_execute_transfer(&transfers[transfer_idx++]);

  while (1) {
      if( timer0_irq ) {
          P1_B1 ^= 1;
          w25q32jv_execute_transfer(&transfers[4]);
          timer0_irq = false;
      }
  }
}
