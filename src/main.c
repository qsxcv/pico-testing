#include "rp2040.h"
#include "usb.h"
#include "usb_std.h"
#include "usb_hid.h"

static void sys_init(void)
{
  // Enable XOSC
  XOSC->CTRL     = (XOSC_CTRL_FREQ_RANGE_1_15MHZ << XOSC_CTRL_FREQ_RANGE_Pos);
  XOSC->STARTUP  = 47; // ~1 ms @ 12 MHz
  XOSC_SET->CTRL = (XOSC_CTRL_ENABLE_ENABLE << XOSC_CTRL_ENABLE_Pos);
  while (0 == (XOSC->STATUS & XOSC_STATUS_STABLE_Msk));

  // Setup SYS PLL for 12 MHz * 40 / 4 / 1 = 120 MHz
  RESETS_CLR->RESET = RESETS_RESET_pll_sys_Msk;
  while (0 == RESETS->RESET_DONE_b.pll_sys);

  PLL_SYS->CS = (1 << PLL_SYS_CS_REFDIV_Pos);
  PLL_SYS->FBDIV_INT = 40;
  PLL_SYS->PRIM = (4 << PLL_SYS_PRIM_POSTDIV1_Pos) | (1 << PLL_SYS_PRIM_POSTDIV2_Pos);

  PLL_SYS_CLR->PWR = PLL_SYS_PWR_VCOPD_Msk | PLL_SYS_PWR_PD_Msk;
  while (0 == PLL_SYS->CS_b.LOCK);

  PLL_SYS_CLR->PWR = PLL_SYS_PWR_POSTDIVPD_Msk;

  // Setup USB PLL for 12 MHz * 40 / 5 / 2 = 48 MHz
  RESETS_CLR->RESET = RESETS_RESET_pll_usb_Msk;
  while (0 == RESETS->RESET_DONE_b.pll_usb);

  PLL_USB->CS = (1 << PLL_SYS_CS_REFDIV_Pos);
  PLL_USB->FBDIV_INT = 40;
  PLL_USB->PRIM = (5 << PLL_SYS_PRIM_POSTDIV1_Pos) | (2 << PLL_SYS_PRIM_POSTDIV2_Pos);

  PLL_USB_CLR->PWR = PLL_SYS_PWR_VCOPD_Msk | PLL_SYS_PWR_PD_Msk;
  while (0 == PLL_USB->CS_b.LOCK);

  PLL_USB_CLR->PWR = PLL_SYS_PWR_POSTDIVPD_Msk;

  // Switch clocks to their final socurces
  CLOCKS->CLK_REF_CTRL = (CLOCKS_CLK_REF_CTRL_SRC_xosc_clksrc << CLOCKS_CLK_REF_CTRL_SRC_Pos);

  CLOCKS->CLK_SYS_CTRL = (CLOCKS_CLK_SYS_CTRL_AUXSRC_clksrc_pll_sys << CLOCKS_CLK_SYS_CTRL_AUXSRC_Pos);
  CLOCKS_SET->CLK_SYS_CTRL = (CLOCKS_CLK_SYS_CTRL_SRC_clksrc_clk_sys_aux << CLOCKS_CLK_SYS_CTRL_SRC_Pos);

  CLOCKS->CLK_PERI_CTRL = CLOCKS_CLK_PERI_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_PERI_CTRL_AUXSRC_clk_sys << CLOCKS_CLK_PERI_CTRL_AUXSRC_Pos);

  CLOCKS->CLK_USB_CTRL = CLOCKS_CLK_USB_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_USB_CTRL_AUXSRC_clksrc_pll_usb << CLOCKS_CLK_USB_CTRL_AUXSRC_Pos);

  CLOCKS->CLK_ADC_CTRL = CLOCKS_CLK_ADC_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_ADC_CTRL_AUXSRC_clksrc_pll_usb << CLOCKS_CLK_ADC_CTRL_AUXSRC_Pos);

  CLOCKS->CLK_RTC_DIV = (256 << CLOCKS_CLK_RTC_DIV_INT_Pos); // 12MHz / 256 = 46875 Hz
  CLOCKS->CLK_RTC_CTRL = CLOCKS_CLK_RTC_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_RTC_CTRL_AUXSRC_xosc_clksrc << CLOCKS_CLK_ADC_CTRL_AUXSRC_Pos);

  // Configure 1 us tick for watchdog and timer
  WATCHDOG->TICK = (12 << WATCHDOG_TICK_CYCLES_Pos) | WATCHDOG_TICK_ENABLE_Msk;

  // Enable GPIOs
  RESETS_CLR->RESET = RESETS_RESET_io_bank0_Msk | RESETS_RESET_pads_bank0_Msk;
  while (0 == RESETS->RESET_DONE_b.io_bank0 || 0 == RESETS->RESET_DONE_b.pads_bank0);
}

static uint64_t get_time(void)
{
  const uint32_t lo = TIMER->TIMELR;
  const uint32_t hi = TIMER->TIMEHR;
  return ((uint64_t)hi << 32) | lo;
}

int main(void)
{
  sys_init();
  usb_init();
  usb_hid_init();

  const int led_pin = 25;
  IO_BANK0->GPIO25_CTRL = IO_BANK0_GPIO25_CTRL_FUNCSEL_sio_25;
  SIO->GPIO_OE_SET = 1 << led_pin;

  union {
    struct __attribute__((packed)) { // ordered as in report descriptor
      uint8_t btn;
      int8_t dwhl;
      int16_t dx, dy;
    };
    uint8_t u8[6];
  } usb_packet = {0};
  usb_packet.dx = 12;

  while (1)
  {
    while (USBCTRL_REGS->INTS != 0 || usb_config == 0)
      usb_task();

    // wait for SOF
    while (USBCTRL_REGS->INTR_b.DEV_SOF == 0);
    // read to clear
    volatile uint32_t dummy = USBCTRL_REGS->SOF_RD;
    (void)dummy;

    // 500us delay
    uint64_t t_next = get_time() + 500;
    while (get_time() < t_next);

    usb_packet.dx = -usb_packet.dx;
    usb_hid_send(usb_packet.u8, 6);

    SIO->GPIO_OUT_XOR = 1 << led_pin;
  }
  return 0;
}
