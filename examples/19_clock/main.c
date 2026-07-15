#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

int main() {
    stdio_init_all();
    sleep_ms(2000); // dar tiempo a que el USB CDC se enumere

    uint32_t f_sys  = clock_get_hz(clk_sys);
    uint32_t f_peri = clock_get_hz(clk_peri);
    uint32_t f_usb  = clock_get_hz(clk_usb);
    uint32_t f_adc  = clock_get_hz(clk_adc);
    uint32_t f_rtc  = clock_get_hz(clk_rtc);

    while (true) {
        printf("clk_sys  = %lu Hz\n", f_sys);
        printf("clk_peri = %lu Hz\n", f_peri);
        printf("clk_usb  = %lu Hz\n", f_usb);
        printf("clk_adc  = %lu Hz\n", f_adc);
        printf("clk_rtc  = %lu Hz\n", f_rtc);
        sleep_ms(3000);
    }
}