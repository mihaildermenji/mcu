import time
import serial
import matplotlib.pyplot as plt
import sys

def read_value(ser):
    """Читает строку телеметрии и возвращает напряжение и температуру"""
    while True:
        try:
            line = ser.readline().decode('ascii').strip()
            if line:
                v, t = map(float, line.split())
                return v, t
        except (ValueError, UnicodeDecodeError):
            continue

def main():
    # Получаем имя порта из аргументов командной строки
    if len(sys.argv) > 1:
        port = sys.argv[1]
    else:
        port = 'COM3'
        print(f"Используется порт по умолчанию: {port}")
        print("Укажите порт: python get_telemetry.py COM5")
    
    # Создаем объект Serial
    ser = serial.Serial(port=port, baudrate=115200, timeout=0.0)
    
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
    
    # Запускаем телеметрию на устройстве
    ser.write("tm_start\n".encode('ascii'))
    print("Telemetry started. Press Ctrl+C to stop...")
    print("-" * 50)
    
    try:
        # Бесконечный цикл сбора телеметрии
        while True:
            # Фиксируем время измерения
            ts = time.time() - start_ts
            
            # Читаем напряжение и температуру из одной строки телеметрии
            voltage_V, temp_C = read_value(ser)
            
            # Добавляем метку времени, напряжение и температуру в листы
            measure_ts.append(ts)
            measure_voltage_V.append(voltage_V)
            measure_temperature_C.append(temp_C)
            
            # Выводим отформатированные измерения
            print(f'{ts:.2f}s - {voltage_V:.3f} V, {temp_C:.1f} C')
            
    except KeyboardInterrupt:
        print("\n\nMeasurement stopped by user")
        
    finally:
        # Останавливаем телеметрию на устройстве
        ser.write("tm_stop\n".encode('ascii'))
        print("Telemetry stopped")
        
        # Закрываем последовательный порт
        ser.close()
        print(f"Port {port} closed")
        
        # Строим графики
        if measure_ts:
            plt.figure(figsize=(12, 8))
            
            # График напряжения
            plt.subplot(2, 1, 1)
            plt.plot(measure_ts, measure_voltage_V, 'b-', linewidth=2)
            plt.title('График зависимости напряжения от времени', fontsize=14)
            plt.xlabel('время, с', fontsize=12)
            plt.ylabel('напряжение, В', fontsize=12)
            plt.grid(True, linestyle='--', alpha=0.7)
            plt.xlim(min(measure_ts), max(measure_ts))
            
            # График температуры
            plt.subplot(2, 1, 2)
            plt.plot(measure_ts, measure_temperature_C, 'r-', linewidth=2)
            plt.title('График зависимости температуры от времени', fontsize=14)
            plt.xlabel('время, с', fontsize=12)
            plt.ylabel('температура, °C', fontsize=12)
            plt.grid(True, linestyle='--', alpha=0.7)
            plt.xlim(min(measure_ts), max(measure_ts))
            
            plt.tight_layout()
            plt.show()
            
            # Вывод статистики
            print(f"\n--- Статистика ---")
            print(f"Напряжение: среднее = {sum(measure_voltage_V)/len(measure_voltage_V):.3f} В")
            print(f"Напряжение: мин = {min(measure_voltage_V):.3f} В, макс = {max(measure_voltage_V):.3f} В")
            print(f"Температура: средняя = {sum(measure_temperature_C)/len(measure_temperature_C):.1f} °C")
            print(f"Температура: мин = {min(measure_temperature_C):.1f} °C, макс = {max(measure_temperature_C):.1f} °C")
            print(f"Всего измерений: {len(measure_ts)}")
            print(f"Общее время: {measure_ts[-1]:.2f} с")
            print(f"Частота измерений: {len(measure_ts) / measure_ts[-1]:.1f} Гц")
        else:
            print("No data collected")

if __name__ == "__main__":
    main()