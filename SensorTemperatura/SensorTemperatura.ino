/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorTemperatura = analogRead(2); // PIN 2
  int valorTemp = map(sensorTemperatura,0,4095,0,100);
  int sensorLuz = analogRead(3); // PIN 5
  int valorLuz = map(sensorLuz,0,4095,0,100);
  // print out the value you read:
  Serial.println("Temp: " + String(valorTemp) + "% - Luz: " + String(valorLuz) + "%");
  delay(1000);  // delay in between reads for stability
}
