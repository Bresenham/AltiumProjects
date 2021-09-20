#include <SI_EFM8BB1_Defs.h>
#include <SI_EFM8BB1_Register_Enums.h>

#include <stdint.h>

void SiLabs_Startup (void) {
  // Disable the watchdog here
}

int main (void) {
  XBR2 = XBR2_XBARE__ENABLED;
  PRTDRV = PRTDRV_P1DRV__HIGH_DRIVE;
  P1MDIN = P1MDIN_B1__DIGITAL;
  P1MDOUT = P1MDOUT_B1__PUSH_PULL;
  P1 = P1_B1__LOW;
  while (1) {
      if(P1 & P1_B1__HIGH) {
          P1 = P1_B1__LOW;
      } else {
          P1 = P1_B1__HIGH;
      }
  }
}
