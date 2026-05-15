#include "stdio-task/stdio-task.h"
#include "stdio.h"
#include "stdlib.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "led-task/led-task.h"

#include "protocol-task.h"
#include "bme280-driver.h"
#include "bme280-regs.h"

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

    if (sscanf(hex_str, "%x %x", &addr, &value) == 2) { // функция считывает строку hex_str и, с учетом формата "%x %x", записывает полученные строки по указаным адресам
        // оба в шеснацеричной системе исчесления 
    }
    else {
        printf("Format error! In the wmem function\n");
        return;
    }
    *(volatile uint32_t*)addr = value;

}

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
    i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
    i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}







void read_regs_callback(const char* args) {
    uint8_t addr;
    uint8_t N;
    if (!(sscanf(args, "%hhx %hhx", &addr, &N) == 2)) { // функция считывает строку args и, с учетом формата "%hhx %hhx", записывает полученные строки по указаным адресам. hhx формат для uint8_t
        printf("Format error! In the read_reg function\n");
        return;
    }
    if (addr > 0xFF || N > 0xFF || N + addr > 0x100) {
        printf("Error! In the read_regs function, values passed exceed the possible range\n");
        return;
    }
    uint8_t buffer[256] = { 0 };
    bme280_read_regs(addr, buffer, N);

    for (int i = 0; i < N; i++)
    {
        printf("bme280 register [0x%X] = 0x%X\n", addr + i, buffer[i]);
    }

}

void write_reg_callback(const char* args) {
    uint8_t addr;
    uint8_t val;
    if (!(sscanf(args, "%hhx %hhx", &addr, &val) == 2)) { // функция считывает строку args и, с учетом формата "%hhx %hhx", записывает полученные строки по указаным адресам. hhx формат для uint8_t
        printf("Format error! In the read_reg function\n");
        return;
    }
    if (addr > 0xFF || val > 0xFF ) {
        printf("Error! In the write_reg function, values passed exceed the possible range\n");
        return;
    }
    bme280_write_reg(addr, val);
}


void temp_raw_callback(const char* args) {
    int32_t value;
    value = bme280_read_temp_raw();
    printf("Raw temperature: %ld (0x%05lX)\n", value, value);
}

void hum_raw_callback(const char* args) {
    uint32_t value;
    value = bme280_read_hum_raw();
    printf("Raw humidity: %u (0x%04X)\n", value, value);
}

void pres_raw_callback(const char* args) {
    int32_t value;
    value = bme280_read_pres_raw();
    printf("Raw pressure: %ld (0x%05lX)\n", value, value);
}

void temp_callback(const char* args)
{
    BME280_S32_t adc_T = bme280_read_temp_raw();
    double temperature = BME280_compensate_T_double(adc_T);
    printf("Temperature: %.2f °C\n", temperature);
}

void pres_callback(const char* args)
{
    BME280_S32_t adc_P = bme280_read_pres_raw();
    double pressure = BME280_compensate_P_double(adc_P);
    printf("Pressure: %.2f Pa (%.2f hPa)\n", pressure, pressure / 100.0);
}

void hum_callback(const char* args)
{
    // Обновляем t_fine
    BME280_S32_t adc_T = bme280_read_temp_raw();
    BME280_compensate_T_double(adc_T);

    // Используем целочисленную формулу как у товарища
    float humidity = bme280_read_humidity_int();

    printf("Humidity: %.2f %%\n", humidity);
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
    {"wmem", wmem_callback, "This function writes the specified value to the given address"},
    {"read_regs", read_regs_callback, "Reads bme280 registers.First arg : start register address, second arg : number of registers to read."},
    {"write_reg", write_reg_callback, "Writes value to BME280 register. Arguments: <reg_addr_hex> <value_hex>"},
    {"temp_raw", temp_raw_callback, "Read raw temperature value from BME280 sensor (16-bit)"},
    {"pres_raw", pres_raw_callback, "Read raw pressure value from BME280 sensor (16-bit)"},
    {"hum_raw", hum_raw_callback, "Read raw humidity value from BME280 sensor (16-bit)"},
    {"temp", temp_callback, "Read temperature value from BME280"  },
    {"pres", pres_callback, "Read pressure value from BME280"  },
    {"hum", hum_callback, "Read humidity value from BME280"  },
    {"dia",diagnose_bme280," diagnose"},
    {NULL, NULL, NULL},
};





int main(){
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_state_init();
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);

    bme280_read_calibration();

    while (1){

    protocol_task_handle(stdio_task_handle());
    led_task_handler();
    }








    return 0;
}
