#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

// Убираем SIO_BASE, т.к. он уже определен в SDK
// Используем готовые определения из SDK
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"

#define DEVICE_NAME "My Pico Device"
#define DEVICE_VRSN "1.0.0"

// ПРОТОТИПЫ ВСЕХ ФУНКЦИЙ
void version_callback(const char* args);
void led_on_callback(const char* args);
void led_off_callback(const char* args);
void led_blink_callback(const char* args);
void led_set_period_callback(const char* args);
void help_callback(const char* args);
void mem_callback(const char* args);
void wmem_callback(const char* args);

// ========== РЕАЛИЗАЦИЯ КОЛБЭКОВ ==========

void version_callback(const char* args)
{
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
    led_task_state_set(LED_STATE_ON);
    printf("LED turned ON\n");
}

void led_off_callback(const char* args)
{
    led_task_state_set(LED_STATE_OFF);
    printf("LED turned OFF\n");
}

void led_blink_callback(const char* args)
{
    led_task_state_set(LED_STATE_BLINK);
    printf("LED blinking started\n");
}

void led_set_period_callback(const char* args)
{
    uint32_t period_ms = 0;
    sscanf(args, "%u", &period_ms);
    
    if (period_ms == 0)
    {
        printf("ERROR: Period must be greater than 0 ms\n");
        return;
    }
    
    led_task_set_blink_period_ms(period_ms);
    printf("LED blink period set to %u ms\n", period_ms);
}

void help_callback(const char* args)
{
    printf("Available commands:\n");
    printf("  version           - get device name and firmware version\n");
    printf("  on                - switch on led\n");
    printf("  off               - switch off led\n");
    printf("  blink             - provide unblocking blinking\n");
    printf("  set_period        - set blinking period in ms\n");
    printf("  mem               - read memory at address (hex)\n");
    printf("  wmem              - write memory at address (hex)\n");
    printf("  help              - print commands description\n");
}

void mem_callback(const char* args)
{
    uint32_t addr = 0;
    
    // Парсим адрес как hex
    if (sscanf(args, "%x", &addr) != 1 && sscanf(args, "0x%x", &addr) != 1)
    {
        printf("ERROR: Invalid address format. Use: mem <hex_address>\n");
        printf("Example: mem 20000000 or mem 0x20000000\n");
        return;
    }
    
    uint32_t value = *(volatile uint32_t*)addr;
    printf("0x%08X = 0x%08X (%u)\n", addr, value, value);
}

void wmem_callback(const char* args)
{
    uint32_t addr = 0;
    uint32_t value = 0;
    
    // Парсим адрес и значение как hex
    if (sscanf(args, "%x %x", &addr, &value) != 2 && 
        sscanf(args, "0x%x 0x%x", &addr, &value) != 2)
    {
        printf("ERROR: Invalid arguments. Use: wmem <hex_address> <hex_value>\n");
        printf("Example: wmem D0000014 02000000\n");
        return;
    }
    
    *(volatile uint32_t*)addr = value;
    printf("OK: 0x%08X <- 0x%08X\n", addr, value);
}

// ========== МАССИВ КОМАНД API ==========

api_t device_api[] =
{
    {"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "switch on led"},
    {"off", led_off_callback, "switch off led"},
    {"blink", led_blink_callback, "provide unblocking blinking"},
    {"set_period", led_set_period_callback, "set blinking period in ms"},
    {"mem", mem_callback, "read memory at address (hex)"},
    {"wmem", wmem_callback, "write memory at address (hex)"},
    {"help", help_callback, "print commands description"},
    {NULL, NULL, NULL},
};

// ========== MAIN ==========

int main()
{
    led_task_init();
    
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    
    printf("\n=== Device Ready ===\n");
    printf("Commands: version, on, off, blink, set_period, mem, wmem, help\n\n");
    
    // Используем регистры из SDK
    printf("GPIO_OUT_SET address: 0x%08X\n", SIO_BASE + SIO_GPIO_OUT_SET_OFFSET);
    printf("GPIO_OUT_CLR address: 0x%08X\n", SIO_BASE + SIO_GPIO_OUT_CLR_OFFSET);
    printf("To turn ON LED: wmem 0xD0000014 0x02000000\n");
    printf("To turn OFF LED: wmem 0xD0000018 0x02000000\n\n");
  
    while(1)
    {
        led_task_handler();
        
        char* cmd = stdio_task_handle();
        if (cmd != NULL) {
            protocol_task_handle(cmd);
        }
        
        sleep_us(100);
    }
    
    return 0;
}