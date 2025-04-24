#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Nombre del dispositivo BLE
#define DEVICE_NAME "ESP32-C3-BLE-Write"

// UUIDs para el servicio y las características
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-5678-90ab-cdef-1234567890ab"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// Callback para manejar escritura en la característica
class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      //std::string value = pCharacteristic->getValue();
      String value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("Datos recibidos:");
        for (int i = 0; i < value.length(); i++) {
          Serial.print(value[i]);
        }
        Serial.println();
      }
    }
};

// Callback para manejar conexiones y desconexiones
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

  // Crear una característica BLE con permisos de escritura
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  // Configurar el callback para manejar escritura
  pCharacteristic->setCallbacks(new MyCallbacks());

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
  // No es necesario hacer nada en el loop para este ejemplo
}