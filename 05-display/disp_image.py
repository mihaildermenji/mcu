import serial
from PIL import Image
import time

# Открываем изображение и конвертируем в RGB
image = Image.open(r'C:\Users\Ovarl\Downloads\i (4).webp').convert('RGB')
width, height = image.size

# Открываем Serial-порт
ser = serial.Serial('COM11', 115200)

try:
    # Отправляем пиксели
    for y in range(height):
        for x in range(width):
            r, g, b = image.getpixel((x, y))
            # Отправляем цвет в HEX формате (6 символов)
            color_hex = f"{r:02X}{g:02X}{b:02X}"
            ser.write(f"disp_px {x} {y} {color_hex}\n".encode())
finally:
    # Пауза перед закрытием порта
    time.sleep(0.1)
    ser.close()