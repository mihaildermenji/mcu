#include "led-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdint.h"

const uint LED_PIN = 25; // uint - сокращение от unsigned int
uint LED_BLINK_PERIOD_US = 500000;

uint64_t led_ts;  // переменная для хранения метки времени 64 т.к. выделена память для метки 64 бита
led_state_t led_state;

void led_state_init(){
    gpio_init(LED_PIN); // функция gpio_init(uint gpio),  иницилизирует gpio пин с переданным номером LED_PIN (25- номер светодиода)
    gpio_set_dir(LED_PIN, true);
    /* функция gpio_set_dir(uint gpio, bool out),
     устанавливает направлениеи пина(он будет принимать данные или отдавать)
    out = true  - выход (output)
     out = false - вход (input) */
    led_ts = 0;
    led_state = LED_STATE_OFF;
}

void led_task_state_set(led_state_t state){ // функция устанавливает новое состояние и сразу его применяет
    led_state = state;
        switch (led_state)
    {
    case LED_STATE_OFF:
        gpio_put(LED_PIN, false); // изменить состояние пина. false = 0 В, true 3,3 В

        break;
    case LED_STATE_ON:
        gpio_put(LED_PIN, true);
        break;
    case LED_STATE_BLINK:
            if (time_us_64() > led_ts)
    	led_ts = time_us_64() + (LED_BLINK_PERIOD_US / 2);

        break;

    default:
        break;
    }
}

void led_task_handler() {
    switch (led_state)
    {
    case LED_STATE_OFF:
        gpio_put(LED_PIN, false); // изменить состояние пина. false = 0 В, true 3,3 В

        break;
    case LED_STATE_ON:
        gpio_put(LED_PIN, true);
        break;
    case LED_STATE_BLINK:
            if (time_us_64() > led_ts){
                    gpio_put(LED_PIN, !gpio_get(LED_PIN));
    /*  gpio_get(LED_PIN) читает текущее состояние указанного пина и возвращает его как логическое значение (bool).*/
                    led_ts = time_us_64() + (LED_BLINK_PERIOD_US / 2);
            }
        break;

    default:
        break;
    }
}
void led_task_set_blink_period_ms(uint32_t period_ms){
    LED_BLINK_PERIOD_US = period_ms*1000;
}

