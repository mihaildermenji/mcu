import time
import serial
import matplotlib.pyplot as plt

def read_value(ser):
    while True:
        try:
            line = ser.readline().decode('ascii')
            value = float(line)
            return value
        except ValueError:
            continue

def main():
    # Создаем объект Serial для работы с последовательным портом
    # Замените 'COM3' на ваш реальный COM-порт
    ser = serial.Serial(port='COM3', baudrate=115200, timeout=0.0)
    
    # Проверка, открылся ли порт
    if ser.is_open:
        print(f"Port {ser.name} opened")
    else:
        print(f"Port {ser.name} closed")
        return
    
    # Листы для хранения результатов измерений и меток времени
    measure_temperature_C = []
    measure_voltage_V = []
    measure_ts = []
    
    # Фиксируем время старта измерений
    start_ts = time.time()
    
    try:
        # Бесконечный цикл
        while True:
            # Фиксируем время измерения
            ts = time.time() - start_ts
            
            # Отправляем команду get_adc
            ser.write("get_adc\n".encode('ascii'))
            voltage_V = read_value(ser)
            
            # Отправляем команду get_temp
            ser.write("get_temp\n".encode('ascii'))
            temp_C = read_value(ser)
            
            # Добавляем метку времени, напряжение и температуру в листы
            measure_ts.append(ts)
            measure_voltage_V.append(voltage_V)
            measure_temperature_C.append(temp_C)
            
            # Выводим отформатированные измерения
            print(f'{voltage_V:.3f} V - {temp_C:.1f}C - {ts:.2f}s')
            
            # Задержка для ограничения скорости работы бесконечного цикла
            time.sleep(0.1)
            
    except KeyboardInterrupt:
        print("\n\nMeasurement stopped by user")
        
    finally:
        # Закрываем последовательный порт
        ser.close()
        print("Port closed")
        
        # Строим графики
        plt.figure(figsize=(10, 8))
        
        plt.subplot(2, 1, 1)
        plt.plot(measure_ts, measure_voltage_V, 'b-', linewidth=2)
        plt.title('График зависимости напряжения от времени', fontsize=14)
        plt.xlabel('время, с', fontsize=12)
        plt.ylabel('напряжение, В', fontsize=12)
        plt.grid(True, linestyle='--', alpha=0.7)
        
        plt.subplot(2, 1, 2)
        plt.plot(measure_ts, measure_temperature_C, 'r-', linewidth=2)
        plt.title('График зависимости температуры от времени', fontsize=14)
        plt.xlabel('время, с', fontsize=12)
        plt.ylabel('температура, °C', fontsize=12)
        plt.grid(True, linestyle='--', alpha=0.7)
        
        plt.tight_layout()
        plt.show()
        
        # Вывод статистики
        if measure_voltage_V:
            print(f"\n--- Статистика ---")
            print(f"Напряжение: среднее = {sum(measure_voltage_V)/len(measure_voltage_V):.3f} В")
            print(f"Температура: средняя = {sum(measure_temperature_C)/len(measure_temperature_C):.1f} °C")
            print(f"Всего измерений: {len(measure_ts)}")
            print(f"Общее время: {measure_ts[-1]:.2f} с")

if __name__ == "__main__":
    main()