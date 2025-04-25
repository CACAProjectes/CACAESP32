#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Nombre del dispositivo BLE
#define DEVICE_NAME "ESP32C3_BLE"

// UUIDs para el servicio y la característica
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

// Crear un puntero para la característica BLE
BLECharacteristic *pCharacteristic;

// Callback para conexión y desconexión
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Dispositivo conectado");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Dispositivo desconectado");
  }
};

void setup() {
  // Inicializar Serial
  Serial.begin(115200);
  Serial.println("Iniciando BLE...");

  // Inicializar el dispositivo BLE
  BLEDevice::init(DEVICE_NAME);

  // Crear el servidor BLE
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Crear un servicio BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Crear una característica BLE
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Agregar un descriptor a la característica
  pCharacteristic->addDescriptor(new BLE2902());

  // Establecer un valor por defecto
  pCharacteristic->setValue("Hola desde ESP32C3");

  // Iniciar el servicio BLE
  pService->start();

  // Iniciar la publicidad BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Opcional
  pAdvertising->setMinPreferred(0x12);  // Opcional
  BLEDevice::startAdvertising();
  Serial.println("Esperando conexiones...");
}

void loop() {
  // Enviar datos a través de la característica BLE (opcional)
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 2000) { // Cada 2 segundos
    lastTime = millis();
    String message = "Contador: " + String(millis() / 1000);
    pCharacteristic->setValue(message.c_str());
    pCharacteristic->notify(); // Notificar a los dispositivos conectados
    Serial.println("Notificación enviada: " + message);
  }
}
