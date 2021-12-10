#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <logging/log.h>
#include <zephyr.h>

#include "memfault/components.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

// Blink code taken from the zephyr/samples/basic/blinky example.
static void blink_forever(void) {
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0 DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0 ""
#define PIN 0
#define FLAGS 0
#endif

  bool led_is_on = true;

  const struct device *dev = device_get_binding(LED0);
  if (dev == NULL) {
    return;
  }

  int ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
  if (ret < 0) {
    return;
  }

  while (1) {
    gpio_pin_set(dev, PIN, (int)led_is_on);
    led_is_on = !led_is_on;
    k_msleep(SLEEP_TIME_MS);
  }
}

void memfault_platform_get_device_info(sMemfaultDeviceInfo *info) {
  *info = (sMemfaultDeviceInfo){
      .device_serial = "DEMOSERIAL",
      .software_type = "zephyr-app",
      .software_version = "1.0.0-dev",
      .hardware_version = CONFIG_BOARD,
  };
}

static void reset_flash(void) {
  // toggle the reset pin with a delay. MX25UM51345G datasheet specifies 10us
  // (see section "13. RESET")
  //
  // P2_12 on the EVK is the SPI flash reset pin (OR'd with a couple other
  // sources to produce nRESET_OSPI)
  const struct device *dev = device_get_binding("GPIO_2");
  gpio_pin_set(dev, 12, 0);
  k_busy_wait(1000 * 100);
  gpio_pin_set(dev, 12, 1);
}

// TODO: this reset sequence results in the chip falling over to USB DFU, which
// means the Flash configuration (0x400) or image header (0x1000) failed to load
// 😞
MEMFAULT_NORETURN
void memfault_platform_reboot(void) {
  memfault_platform_halt_if_debugging();

  // strobe reset pin on the SPI flash to set it back to 1-bit mode, which is
  // required for the ROM bootloader to successfully probe it.
  reset_flash();

  // Note: There is no header exposed for this zephyr function
  extern void sys_arch_reboot(int type);

  sys_arch_reboot(0);
  CODE_UNREACHABLE;
}

void main(void) {
  LOG_INF("Memfault Demo App! Board %s\n", CONFIG_BOARD);
  memfault_device_info_dump();
  blink_forever();
}
