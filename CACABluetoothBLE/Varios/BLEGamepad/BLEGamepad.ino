#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEHIDDevice.h>
#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>

BLEHIDDevice* hid;
BLECharacteristic* input;

void setup() {
  Serial.begin(115200);

  // Inicializar Bluetooth
  BLEDevice::init("ESP32-C3 Gamepad");
  BLEServer* pServer = BLEDevice::createServer();
  hid = new BLEHIDDevice(pServer);
  input = hid->inputReport(1); // Report ID 1

  // Configurar el dispositivo HID
  const uint8_t reportMap[] = {
      USAGE_PAGE(1), 0x01, // Generic Desktop Controls
      USAGE(1), 0x05,      // Gamepad
      COLLECTION(1), 0x01, // Application
      // Aquí se definen los botones y ejes
      END_COLLECTION(0)
  };

  hid->reportMap((uint8_t*)reportMap, sizeof(reportMap));
  hid->startServices();
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->start();

  Serial.println("Esperando conexión del mando...");
}

void loop() {
  // Aquí podrías leer los datos del mando
  if (input->getValue().length() > 0) {
    Serial.println("Datos recibidos del mando.");
  }
}