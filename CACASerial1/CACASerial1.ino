/*
  CACA Coche Teledirigido
 */
//
const int ledPinPrueba = 4;   // LED pin de la placa
// DATOS EN SERIE
const int pinSERIE_RST =  9;  // Pin SERIE RESET
const int pinSERIE_CLK =  10;  // Pin SERIE CLOCK
const int pinSERIE_DAT =  8;  // Pin SERIE DATA
//
int contadorGeneral = 0;  // Contador general de tiempos
int inByte = 0;

void setup() {
  // inicializar PINs IN-OUT
  pinMode(ledPinPrueba, OUTPUT);
  //
  pinMode(pinSERIE_RST, OUTPUT);
  pinMode(pinSERIE_CLK, OUTPUT);
  pinMode(pinSERIE_DAT, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // activar el SERIAL-DATA - RST=HIGH
  digitalWrite(pinSERIE_RST, HIGH);  
}

void loop() {
  // BLUE
  //digitalWrite(LED_BUILTIN, HIGH);
  shiftOut(pinSERIE_DAT, pinSERIE_CLK, LSBFIRST, B00100000); // send this binary value to the shift register
  delay(250); // Wait for 1000 millisecond(s)
  // VACIO
  shiftOut(pinSERIE_DAT, pinSERIE_CLK, LSBFIRST, B00000000); // send this binary value to the shift register
  delay(100); // Wait for 1000 millisecond(s)
  // BLUE
  shiftOut(pinSERIE_DAT, pinSERIE_CLK, LSBFIRST, B00100000); // send this binary value to the shift register
  delay(250); // Wait for 1000 millisecond(s)
  // RED
  //digitalWrite(LED_BUILTIN, LOW);
  shiftOut(pinSERIE_DAT, pinSERIE_CLK, LSBFIRST, B00001000); // send this binary value to the shift register
  delay(500); // Wait for 1000 millisecond(s)
  //
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
