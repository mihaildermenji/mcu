#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdio.h"
#include "stdlib.h"
const uint LED_PIN = 25;
main(){
    stdio_init_all();
    gpio_init(LED_PIN);
     gpio_set_dir(LED_PIN, GPIO_OUT);
    while(1)
    {
         
        char symbol = getchar();
        printf("received char: %c [ ASCII code: %d ]\n", symbol, symbol);
        switch(symbol)
        {
        case 'e':
            gpio_put(LED_PIN, true);
            printf("led enable done\n");
            break;
        case 'd':
            gpio_put(LED_PIN, false);
            printf("led disable done\n");
            break;
        case 'v':
            printf("Device name: My pico device; firmware version: v0.0.1\n");
            break;
        default:
            break;
        }
    } 
}