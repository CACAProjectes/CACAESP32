#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Nombre del dispositivo BLE
#define DEVICE_NAME "ESP32-C3-BLE"

// UUIDs para el servicio y las características
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-5678-90ab-cdef-1234567890ab"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// Función de callback para manejar las conexiones
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Dispositivo conectado");
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Dispositivo desconectado");
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando BLE...");

  // Inicializar el dispositivo BLE
  BLEDevice::init(DEVICE_NAME);

  // Crear un servidor BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Crear un servicio BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Crear una característica BLE
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Configurar un valor inicial
  pCharacteristic->setValue("Hola desde ESP32-C3");

  // Iniciar el servicio
  pService->start();

  // Iniciar la publicidad BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Opcional
  pAdvertising->setMaxPreferred(0x12); // Opcional
  BLEDevice::startAdvertising();
  Serial.println("Dispositivo BLE listo para conectar.");
}

void loop() {
  // Si un dispositivo está conectado, enviar notificaciones periódicas
  if (deviceConnected) {
    static unsigned long lastMillis = 0;
    if (millis() - lastMillis > 1000) {
      lastMillis = millis();
      pCharacteristic->setValue("Tiempo: " + String(millis() / 1000));
      pCharacteristic->notify(); // Enviar notificación al cliente conectado
      Serial.println("Notificación enviada");
    }
  }
}