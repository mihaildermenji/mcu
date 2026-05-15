#include "stdio-task/stdio-task.h"
#include "stdio.h"
#include "stdlib.h"
#include "pico/stdlib.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

#include "hardware/spi.h"

#include "ili9341-driver.h"

#include "ili9341-display.h"
#include "ili9341-font.h"
//Дисплей

static ili9341_display_t ili9341_display = { 0 };
#define ILI9341_PIN_MISO 4
#define ILI9341_PIN_CS 10
#define ILI9341_PIN_SCK 6
#define ILI9341_PIN_MOSI 7
#define ILI9341_PIN_DC 8
#define ILI9341_PIN_RESET 9
// #define PIN_LED -> 3.3V



typedef void (*ili9341_spi_write)(const uint8_t* data, uint32_t size);
typedef void (*ili9341_spi_read)(uint8_t* buffer, uint32_t length);
typedef void (*ili9341_gpio_cs_write)(bool level);
typedef void (*ili9341_gpio_dc_write)(bool level);
typedef void (*ili9341_gpio_reset_write)(bool level);
typedef void (*ili9341_delay_ms)(uint32_t ms);

void rp2040_spi_write(const uint8_t* data, uint32_t size)
{
    spi_write_blocking(spi0, data, size);
}

void rp2040_spi_read(uint8_t* buffer, uint32_t length)
{
    spi_read_blocking(spi0, 0, buffer, length);
}

void rp2040_gpio_cs_write(bool level)
{
    gpio_put(ILI9341_PIN_CS, level);
}

void rp2040_gpio_dc_write(bool level)
{
    gpio_put(ILI9341_PIN_DC, level);
}

void rp2040_gpio_reset_write(bool level)
{
    gpio_put(ILI9341_PIN_RESET, level);
}

void rp2040_delay_ms(uint32_t ms) {
    sleep_ms(ms);
}

















#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

api_t device_api[];

void version_callback(const char* args);
void led_on_callback(const char* args);
void led_off_callback(const char* args);
void led_blink_callback(const char* args);
void led_blink_set_period_ms_callback(const char* args);
void help_callback(const char* args);











void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}
void led_on_callback(const char* args){
    led_task_state_set(LED_STATE_ON);
}
void led_off_callback(const char* args){
    led_task_state_set(LED_STATE_OFF);
}
void led_blink_callback(const char* args){
    led_task_state_set(LED_STATE_BLINK);
}
void led_blink_set_period_ms_callback(const char* args){
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);
    if (period_ms == 0){
        printf("Error! Function led_blink_set_period_ms_callback received no argument ");
        return;
    }
    led_task_set_blink_period_ms(period_ms);
    /*Функция int sscanf(const char *str, const char *format, ...) String Scan Formatted (сканирование строки по формату).
    Первым аргументом мы передаем указатель на строку, которую будем сканировать,
    Вторым аргументом мы передаем сроку формата, которая определяет какие типы искать, их может быть указано несколько:
    "%u"     // искать беззнаковое целое число
    "%d"     // искать целое число со знаком
    "%s"     // искать строку (до пробела)
    "%f"     // искать число с плавающей точкой
    "%u %s"  // искать число и строку
    Оставшимися аргументами идут указатели на переменные, куда скалдывать найденый формат.
     Если поиск был по нескольким форматам, можно указать указатели на переменые в том же порядке.*/
}


void help_callback(const char* args){
    for (uint i = 0; device_api[i].command_help != NULL; i++) {
        printf("%s: %s\n",device_api[i].command_name, device_api[i].command_help);

    }
}
void mem_callback(const char* hex_str) {
    uint32_t addr = strtoul(hex_str, NULL, 16); 
    /* unsigned long strtoul(const char* str, char** endptr, int base); (PS: strtoul = string to unsigned long)
    Читает строку str и преобразует её в число типа unsigned long с учётом системы счисления base.
    str — строка для преобразования
    endptr — указатель, куда будет записан адрес первого непрочитанного символа (можно NULL, если не нужно)
    base — система счисления (2..36, или 0 для автоопределения)
    */
    uint32_t value = *(volatile uint32_t*)addr; // 
    printf("Value: 0x%08X (%u)\n", value, value); // 0x%08X формат 16 системы.
}

void wmem_callback(const char* hex_str) {
    uint32_t addr;
    uint32_t value;

    if (sscanf(hex_str, "%x:%x", &addr, &value) == 2) { // функция считывает строку hex_str и, с учетом формата "%x:%x", записывает полученные строки по указаным адресам
        // оба в шеснацеричной системе исчесления 
    }
    else {
        printf("Format error\n");
        return;
    }
    *(volatile uint32_t*)addr = value;

}

void disp_screen_callback(const char* args)
{
    uint32_t c = 0;
    int result = sscanf(args, "%x", &c);

    uint16_t color = COLOR_BLACK;

    if (result == 1)
    {
        color = RGB888_2_RGB565(c);
    }

    ili9341_fill_screen(&ili9341_display, color);
}
void disp_px_callback(const char* args)
{
    uint16_t x, y;
    uint32_t color_32;
    if (sscanf(args, "%hu %hu %x", &x, &y, &color_32) == 3) {
        uint16_t color = RGB888_2_RGB565(color_32);
        ili9341_draw_pixel(&ili9341_display, x, y, color);
    }
    else {
        printf("Error: Invalid format. Use: disp_px <uint16> <uint16> <uint32>\n");
    }
}
void disp_line_callback(const char* args) {
    uint16_t x1, y1, x2, y2;
    uint32_t color_32;
    if (sscanf(args, "%hu %hu %hu %hu  %x", &x1, &y1 ,&x2, &y2, &color_32) == 5) {
        uint16_t color = RGB888_2_RGB565(color_32);
        ili9341_draw_line(&ili9341_display, x1, y1,x2, y2, color);
    }
    else {
        printf("Error: Invalid format. Use: disp_line <x1> <y1> <x2> <y2> <color>\n");
    }
}
void disp_rect_callback(const char* args) {
    uint16_t x1, y1, width, height;
    uint32_t color_32;
    if (sscanf(args, "%hu %hu %hu %hu %x", &x1, &y1, &width, &height, &color_32) == 5) {
        uint16_t color = RGB888_2_RGB565(color_32);
        ili9341_draw_rect(&ili9341_display, x1, y1, width, height, color);
    }
    else {
        printf("Error: Invalid format. Use: disp_rect <x> <y> <width> <height> <color>\n");
    }
}
void disp_frect_callback(const char* args) {
    uint16_t x1, y1, width, height;
    uint32_t color_32;
    if (sscanf(args, "%hu %hu %hu %hu %x", &x1, &y1, &width, &height, &color_32) == 5) {
        uint16_t color = RGB888_2_RGB565(color_32);
        ili9341_draw_filled_rect(&ili9341_display, x1, y1, width, height, color);
    }
    else {
        printf("Error: Invalid format. Use: disp_frect <x> <y> <width> <height> <color>\n");
    }
}
void disp_text_callback(const char* args) {
    uint16_t x, y;
    uint32_t color_32, bg_color_32;
    char text_buffer[256];
    if (sscanf(args, "%hu %hu %x %x %[^\n]", &x, &y, &color_32, &bg_color_32, &text_buffer) == 5) {
        uint16_t color = RGB888_2_RGB565(color_32);
        uint16_t bg_color = RGB888_2_RGB565(bg_color_32);
        ili9341_draw_text(&ili9341_display, x, y, text_buffer, &jetbrains_font, color, bg_color);
    }
    else if (sscanf(args, "%hu %hu %x %[^\n]", &x, &y, &color_32, &text_buffer) == 4){
        uint16_t color = RGB888_2_RGB565(color_32);
        ili9341_draw_text(&ili9341_display, x, y, text_buffer, &jetbrains_font, color, color);
    }
    else if (sscanf(args, "%hu %hu %[^\n]", &x, &y, &text_buffer) == 3) {
        ili9341_draw_text(&ili9341_display, x, y, text_buffer, &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    } else {
        printf("Error: Invalid format.\n");
        printf("Usage examples:\n");
        printf("  disp_text 10 20 Hello             - White text at (10,20)\n");
        printf("  disp_text 10 20 FF0000 Hello      - Red text at (10,20)\n");
        printf("  disp_text 10 20 FF0000 000000 Hello - Red text on black bg\n");
        printf("  For text with spaces, use quotes: disp_text 10 20 \"Hello World\"\n");
    }

}

api_t device_api[] =
{
    {"version", version_callback, "get device name and firmware version"},
    {"led_off",led_off_callback , "The function turns off the LED."},
    {"led_on",led_on_callback, "The function turns on the LED."},
    {"led_blink",led_blink_callback, "The function makes the LED blink."},
    {"set_period",led_blink_set_period_ms_callback, "set LED blink period in milliseconds (e.g., 'set_period 500')"},
    {"help", help_callback, "show this help message or help for a specific command"},
    {"mem",mem_callback,  "This function returns the value stored at the address passed as an argument"},
    {"wmem", wmem_callback, "This function writes the specified value to the given address. Signature: wmem addres:value"},
    {"disp_screen", disp_screen_callback , "Fill screen with color (name or hex)"},
    {"disp_px", disp_px_callback, "Draws a single pixel at specified coordinates with given color. Usage: disp_px <x> <y> <color>"},
    {"disp_line", disp_line_callback, "Draws a line from (x1,y1) to (x2,y2) with specified color. Usage: disp_line <x1> <y1> <x2> <y2> <color>"},
    {"disp_rect", disp_rect_callback, "Draws a rectangle outline at (x,y) with width, height and color. Usage: disp_rect <x> <y> <width> <height> <color>"},
    {"disp_frect", disp_frect_callback, "Draws a filled rectangle at (x,y) with width, height and color. Usage: disp_frect <x> <y> <width> <height> <color>"},
    {"disp_text", disp_text_callback, "Draw text on display. Usage: disp_text <x> <y> [color] [bg_color] <text>. "
 "Colors in RGB888 hex (e.g., FF0000). Use quotes for multi-word text."},
    {NULL, NULL, NULL},
};





int main(){
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_state_init();


    spi_init(spi0, 62500000);
    gpio_set_function(ILI9341_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_SCK, GPIO_FUNC_SPI);
    gpio_init(ILI9341_PIN_CS);
    gpio_set_dir(ILI9341_PIN_CS, true);
    gpio_put(ILI9341_PIN_CS, true );
    gpio_init(ILI9341_PIN_DC);
    gpio_set_dir(ILI9341_PIN_DC, true);
    gpio_put(ILI9341_PIN_DC, false);
    gpio_init(ILI9341_PIN_RESET);
    gpio_set_dir(ILI9341_PIN_RESET, true);
    gpio_put(ILI9341_PIN_RESET, false);

    ili9341_hal_t ili9341_hal = { 0 };
    ili9341_hal.spi_write = rp2040_spi_write;
    ili9341_hal.spi_read = rp2040_spi_read;
    ili9341_hal.gpio_cs_write = rp2040_gpio_cs_write;
    ili9341_hal.gpio_dc_write = rp2040_gpio_dc_write;

    ili9341_hal.gpio_reset_write = rp2040_gpio_reset_write;
    ili9341_hal.delay_ms = rp2040_delay_ms;

    ili9341_init(&ili9341_display, &ili9341_hal);

    ili9341_set_rotation(&ili9341_display, ILI9341_ROTATION_90);

    ili9341_fill_screen(&ili9341_display, COLOR_BLACK);
    sleep_ms(300);

    /* 2. Coloured rectangles */
    ili9341_draw_filled_rect(&ili9341_display, 10, 10, 100, 60, COLOR_MY);
    ili9341_draw_filled_rect(&ili9341_display, 120, 10, 100, 60, COLOR_GREEN);
    ili9341_draw_filled_rect(&ili9341_display, 230, 10, 80, 60, COLOR_BLUE);
   
    /* 3. Hollow rectangle outline */
    ili9341_draw_rect(&ili9341_display, 10, 90, 300, 80, COLOR_WHITE);

    /* 4. Diagonal lines */
    ili9341_draw_line(&ili9341_display, 0, 0, 319, 239, COLOR_YELLOW);
    ili9341_draw_line(&ili9341_display, 319, 0, 0, 239, COLOR_CYAN);

    ili9341_draw_text(&ili9341_display, 20, 100, "Hello, ILI9341!", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(&ili9341_display, 20, 116, "RP2040 / Pico SDK", &jetbrains_font, COLOR_YELLOW, COLOR_BLACK);

    
    while (1){

    protocol_task_handle(stdio_task_handle());
    led_task_handler();
    }








    return 0;
}
