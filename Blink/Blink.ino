/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/
/*
// LED_BUILTIN define fixups for Teensy, ChipKit, ESP8266, ESP32 cores
#if !defined(LED_BUILTIN)

#if defined(CORE_TEENSY)
#define LED_BUILTIN CORE_LED0_PIN

#elif defined(ARDUINO_ARCH_ESP8266)
#define LED_BUILTIN BUILTIN_LED

// this is for cores that incorrectly used BUILTIN_LED instead of LED_BUILTIN
// esp32 core does this, they have LED_BUILTIN but it is a const not a define
// this works around that.
#elif defined(BUILTIN_LED)
#define LED_BUILTIN BUILTIN_LED

// special check for pre 1.0.6 IDEs that didn't define LED_BUILTIN
#elif (ARDUINO <  106 ) && ( defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__))
#define LED_BUILTIN 13 // IDE bundled AVR boards use pin 13 for builtin led

#elif defined(PIN_LED1) // chipkit
#define LED_BUILTIN PIN_LED1
#endif
#endif
*/

#define LED_BUILTIN 8

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
