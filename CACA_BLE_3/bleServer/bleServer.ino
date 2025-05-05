/*
    Simple sketch to control a led with BLE protocol by
    Daniel Carrasco (https://www.electrosoftcloud.com)
*/
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "3feb1e8a-3981-4045-ad39-b225135013a0"
#define CONTROL_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
//#define LED 23
#define LED 8         // 8 - led azul de la placa ESP32-C3
char ledStatus = 48;  // 0 in ASCII
// New characteristic with object to manage it
BLECharacteristic controlCharacteristic(
  CONTROL_CHARACTERISTIC_UUID,
  BLECharacteristic::PROPERTY_READ |
  BLECharacteristic::PROPERTY_WRITE
);
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE!");
  Serial.println("Initializing device");
  BLEDevice::init("Led control electrosoftcloud"); // Initializing the device with its name
  Serial.println("Creating server");
  BLEServer *pServer = BLEDevice::createServer(); // Create the server
  Serial.println("Adding service UUID");
  BLEService *pService = pServer->createService(SERVICE_UUID); // Creating a new service into server
  
  // Adding a characteristic with the object name (official UUID), without object (this characteristic will not change)
  Serial.println("Adding name characteristic");
  BLECharacteristic *nameCharacteristic = pService->createCharacteristic(
                                         BLEUUID((uint16_t)0x2A00),
                                         BLECharacteristic::PROPERTY_READ
                                       );
  nameCharacteristic->setValue("Led");
  // Adding a characteristic to control the led with 0 and 1
  Serial.println("Adding control characteristic");
  pService->addCharacteristic(&controlCharacteristic);
  controlCharacteristic.setValue(&ledStatus); // Value uint8_t with length 1
  Serial.println("Starting...");
  pService->start();
  Serial.println("Creating advertising");
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  pinMode (LED, OUTPUT); // Set the LED pin as OUTPUT
}
void loop() {
  //std::string controlValue = controlCharacteristic.getValue();
  String controlValue = controlCharacteristic.getValue();
  //if (controlValue[0] != ledStatus) {
  if (controlValue.charAt(0) != ledStatus) {
    Serial.print("Value changed... new value: ");
    Serial.println(controlValue.charAt(0));
    ledStatus = controlValue.charAt(0);
    if (ledStatus == 48) {
      digitalWrite(LED, LOW); // LED Off
    }
    else if (ledStatus == 49) {
      digitalWrite(LED, HIGH); // LED On
    }
  }
}