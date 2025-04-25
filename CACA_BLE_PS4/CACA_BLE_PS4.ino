#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);

  // Inicializar la conexión BLE con PS4
  //if (!PS4.begin("xx:xx:xx:xx:xx:xx")) { // Reemplaza con la dirección MAC del ESP32-C3
  if (!PS4.begin("E4:B3:23:C0:B6:EE")) { // Reemplaza con la dirección MAC del ESP32-C3
    Serial.println("No se pudo inicializar la conexión BLE");
    return;
  }

  Serial.println("Esperando conexión del mando PS4...");
}

void loop() {
  // Comprobar si el mando está conectado
  if (PS4.isConnected()) {
    Serial.println("Mando PS4 conectado");

    // Leer los botones y los joysticks
    if (PS4.event.button_down.up) {
      Serial.println("Se presionó el botón 'Arriba'");
    }
    if (PS4.event.button_down.down) {
      Serial.println("Se presionó el botón 'Abajo'");
    }
    if (PS4.event.button_down.cross) {
      Serial.println("Se presionó el botón 'X'");
    }

    // Leer valores de los joysticks
    int16_t joystickX = PS4.data.analog.stick.lx;
    int16_t joystickY = PS4.data.analog.stick.ly;
    Serial.printf("Joystick Izquierdo: X=%d, Y=%d\n", joystickX, joystickY);

    delay(100); // Esperar un poco para evitar saturar el puerto serie
  }
}