#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    stdio_init_all();

    const uint GPIO_28 = 26;
    const uint GPIO_29 = 27;

    gpio_init(GPIO_28);
    gpio_set_dir(GPIO_28, GPIO_OUT);
    gpio_put(GPIO_28, 1);

    gpio_init(GPIO_29);
    gpio_set_dir(GPIO_29, GPIO_OUT);
    gpio_put(GPIO_29, 1);

    while (true) {
        tight_loop_contents();
    }
}