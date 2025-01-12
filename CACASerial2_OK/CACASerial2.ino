/*
  CACA Serial Luces
 */
//
const int ledPinPrueba = 4;     // LED pin de la placa
// DATOS EN SERIE
/*
const int pinSERIE_RST =  9;    // Pin SERIE RESET
const int pinSERIE_CLK =  10;   // Pin SERIE CLOCK
const int pinSERIE_DAT =  8;    // Pin SERIE DATA
*/
const int dataPin = 8;   // SER
const int clockPin = 9;  // SRCLK
const int latchPin = 10;  // RCLK
//
int contadorGeneral = 0;  // Contador general de tiempos
int inByte = 0;

void setup() {
  // inicializar PINs IN-OUT
  pinMode(ledPinPrueba, OUTPUT);
  //
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  // activar el SERIAL-DATA - RST=HIGH
  //digitalWrite(pinSERIE_RST, HIGH);  
}

void loop() {
  /*
  //blinkLed();
  // read from port 0, send to port 1:
  if (Serial.available()) {
    inByte = Serial.read();
    Serial.write(inByte);
    contadorGeneral = 1;
  }
  //  
  //shiftOut(pinSERIE_DAT, pinSERIE_CLK, LSBFIRST, B10101010); // send this binary value to the shift register
  if (contadorGeneral == 1 && inByte != 10) {
    //shiftOut(pinSERIE_DAT, pinSERIE_CLK, MSBFIRST, inByte); // send this binary value to the shift register
    shiftOut(pinSERIE_DAT, pinSERIE_CLK, MSBFIRST, B10101010); // send this binary value to the shift register
    shiftOut(pinSERIE_DAT, pinSERIE_CLK, MSBFIRST, B10101010); // send this binary value to the shift register
    Serial.write("Enviado!");
    contadorGeneral = 0;
  }
  /*
  for (int j = 0; j < 256; j++) {
    //ground latchPin and hold low for as long as you are transmitting
    //digitalWrite(latchPin, LOW);
    shiftOut(pinSERIE_DAT, pinSERIE_CLK, MSBFIRST, j);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    //digitalWrite(latchPin, HIGH);
    delay(1000);
  }
  *
  //
  delay(1000);
  */
  byte leds = 0b00100000; // BLUE
  setRegister(leds);
  delay(500);
  leds = 0b00001000;  // RED
  setRegister(leds);
  delay(250);
  leds = 0b00100000; // BLUE
  setRegister(leds);
  delay(100);
  leds = 0b00001000;  // RED
  setRegister(leds);
  delay(250);
  leds = 0b00100000; // BLUE
  setRegister(leds);
  delay(500);
  //for (int i = 0; i < 8; i++) {
  //  bitSet(leds, i);
  //  setRegister(leds);
  //}
}

void setRegister(byte val) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, val); 
  digitalWrite(latchPin, HIGH);
}

void blinkLed() {
  contadorGeneral++;
  if (contadorGeneral >= 0 && contadorGeneral < 50)
    digitalWrite(ledPinPrueba, HIGH);   // turn the LED on (HIGH is the voltage level)
  else if (contadorGeneral >= 50 &&  contadorGeneral < 100)
    digitalWrite(ledPinPrueba, LOW);    // turn the LED off by making the voltage LOW
  else 
    contadorGeneral = 0;
}
