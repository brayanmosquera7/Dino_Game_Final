# Dino_Game_Final
Dino Game para ESP32 + Pantalla ILI9488 + Bluetooth
Proyecto desarrollado para el curso **Microcontroladores – Universidad del Cauca**,  
consistente en una implementación del clásico *Dino Game* de Google Chrome,  
utilizando:

- ESP32
- Pantalla TFT ILI9488 480x320
- Conexión Bluetooth con celular (control por app)
- Buzzer para efectos de sonido

---

## 🎮 **Características del Proyecto**

✔ Control remoto vía Bluetooth  
✔ Animación del dinosaurio (salto, movimiento)  
✔ Obstáculos aleatorios  
✔ Detección de colisiones  
✔ Puntuación progresiva  
✔ Sonidos con buzzer  
✔ Redibujado optimizado (sin parpadeos)

---

## 🛠 **Tecnologías Utilizadas**

- Arduino IDE
- Biblioteca **TFT_eSPI**
- **BluetoothSerial** del ESP32
- Comunicación SPI
- Lenguaje C++ (Arduino)

---

 **Conexiones ESP32 – ILI9488**

| ESP32 | ILI9488 | Función        |
|-------|---------|----------------|
| 3.3V  | VCC     | Alimentación   |
| GND   | GND     | Tierra         |
| GPIO 23 | MOSI  | Datos SPI      |
| GPIO 18 | SCK   | Reloj SPI      |
| GPIO 5  | CS    | Chip Select    |
| GPIO 19 | DC    | Data/Command   |
| GPIO 22 | RST   | Reset          |
| GPIO 21 | T_CS  | Touch CS       |

Buzzer conectado a GPIO 25.
<img width="584" height="366" alt="image" src="https://github.com/user-attachments/assets/0830dfad-a693-4e43-abc8-0e5824954756" />

