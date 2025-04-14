#include "SoftwareSerial.h"
#include "BluetoothSerial.h"

/*BLUETOOTH*/
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
//#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
/*BLUETOOTH*/


SoftwareSerial mySoftwareSerial(16, 17); // RX, TX

# define ACTIVATED LOW

/*RELEVADOR*/
#define pinOut 12

/*RELEVADOR*/

void setup()
{

  mySoftwareSerial.begin(9600);  // speed, type, RX, TX
  Serial.begin(115200);


  /*RELEVADOR*/
  SerialBT.begin("MyESP"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode (pinOut, OUTPUT);
  digitalWrite (pinOut, LOW);
  delay (20);
  /*RELEVADOR*/
}

void loop()
{
  /*BLUETOOTH*/
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  
  if (SerialBT.available())
  {   
    char data = ((byte) SerialBT.read());
    switch (data)
    {
      case 'P':
        digitalWrite (pinOut, HIGH);
        //Hacemos un delay de 1 segundo
        delay (1000);
        digitalWrite (pinOut, LOW);
        break;

      case 'b':
        digitalWrite(maleta, HIGH);
        delay(800);
        digitalWrite(maleta, LOW);
        break;
      default : break;
    }
    Serial.println(data);
  }
  delay(20);
  /*BLUETOOTH*/
  
}
