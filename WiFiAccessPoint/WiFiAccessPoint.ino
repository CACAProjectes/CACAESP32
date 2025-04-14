/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)

  Reescrito por JJCP 21/03/2025
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include "esp_wifi.h"
// Pantalla
#include <Arduino.h>
#include <U8g2lib.h>
// GPIO
//#define LED_BUILTIN 4  // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#define SENSOR_LUZ      0
#define SENSOR_POS_TRA  1
#define SENSOR_POS_DEL  2
// Pantalla
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define SDA_PIN 5
#define SCL_PIN 6

// DATOS EN SERIE
const int buzzerPin       = 7;   // BUZZER
const int dataPin         = 8;   // SER
const int clockPin        = 9;   // SRCLK
const int latchPin        = 10;  // RCLK
// Set these to your desired credentials.
const char *ssid          = "CACA_ESP32_C3";
const char *password      = "12345678";
// Set VAR PAGINA HTML
String pagina_web;
String pagina_web_refresh;
String cabeceraHttp;
String paginaPrincipalGET = "GET /principal?";
String paginaPrincipal    = "/principal";
// Botones ACTIVOS
boolean bWarnings         = false;
boolean bIntermitenteIzq  = false;
boolean bIntermitenteDer  = false;
boolean bSirena           = false;
boolean bPosicion         = false;
boolean bCruce            = false;
boolean bCarretera        = false;
boolean bMarchaAtras      = false;
boolean bLuzTrasera       = false;
boolean bLuzTraseraAtras  = false;
boolean bLuzTraseraStop   = false;
boolean bIntermitenteIzqON= false;
boolean bIntermitenteDerON= false;
boolean bSensorLuz        = false;
boolean bBuzzer           = false;
//
int iUnidadPotencia       = 0;
int iGiroRuedas           = 50;         // [RECTO]
// Máscaras de LUCES
byte CTE_IntermDerON      = 0b10000000;	// U1 - QA
byte CTE_IntermIzqON      = 0b01000000;	// U1 - QB
byte CTE_SirenaBON  	    = 0b00100000;	// U1 - QC
byte CTE_SirenaGON  	    = 0b00010000;	// U1 - QD
byte CTE_SirenaRON  	    = 0b00001000;	// U1 - QE
//
byte CTE_LuzGirDerON      = 0b10000000;	// U2 - QA
byte CTE_LuzCarrON        = 0b01000000;	// U2 - QB
byte CTE_LuzCrucON  	    = 0b00100000;	// U2 - QC
byte CTE_LuzPosON  	      = 0b00010000;	// U2 - QD
byte CTE_LuzGirIzqON  	  = 0b00001000;	// U2 - QE
byte CTE_LuzTrasON  	    = 0b00000100;	// U2 - QF
byte CTE_LuzAtrON  	      = 0b00000010;	// U2 - QG
byte CTE_LuzStopON  	    = 0b00000001;	// U2 - QH

// String de salida
byte iRespuesta1          = 0b00000000;
byte iRespuesta2          = 0b00000000;
// BUCLE
int CTE_MIN_BUCLE         = 1;
int CTE_MAX_BUCLE         = 10;
int mContador             = CTE_MIN_BUCLE;
String currentLine        = "";
int iPosWebPrincipal      = 0;
int CTE_TEMPS_ESPERA_BUCLE= 150;
int CTE_TEMPS_REFRESH_HTML= 2;
int CTE_LIMITE_LUZ        = 750;
int CTE_LIMITE_POS_DEL    = 500;
int CTE_LIMITE_POS_TRA    = 500;
int CTE_FRECUENCIA_BUZZER = 2000;       // Send 1KHz sound signal...
// Network
NetworkServer server(80);
// Pantalla
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, 6, 5);
int width                 = 72; 
int height                = 40; 
int xOffset               = 30;         // = (132-w)/2 
int yOffset               = 12;         // = (64-h)/2 

const uint8_t epd_bitmap_luz_posicion[] = 
{   
  0b01000000,0b00100000, 
  0b10000000,0b00010110, 
  0b00010000,0b10001111, 
  0b10100000,0b01011001, 
  0b11000000,0b00110000, 
  0b01100000,0b01100000, 
  0b01100000,0b01100000, 
  0b11000000,0b00110000, 
  0b10100000,0b01011001, 
  0b00010000,0b10001111, 
  0b10000000,0b00010110, 
  0b01000000,0b00100000
};

const uint8_t epd_bitmap_luz_cruce[] = 
{   
  0b00000000,0b00001100,
  0b00000000,0b00011110,
  0b00000000,0b00111101,
  0b10000000,0b01111100,
  0b01000000,0b01101110,
  0b00000000,0b11101101,
  0b10000000,0b11101100,
  0b01000000,0b01101110,
  0b00000000,0b01111101,
  0b10000000,0b00111100,
  0b01000000,0b00011110,
  0b00000000,0b00001101
};

const uint8_t epd_bitmap_luz_carretera[] = 
{   
  0b00000000,0b00001100,
  0b11000000,0b00011111,
  0b00000000,0b00111100,
  0b00000000,0b01111100,
  0b11000000,0b01101111,
  0b00000000,0b11001100,
  0b00000000,0b11001100,
  0b11000000,0b01101111,
  0b00000000,0b01111100,
  0b00000000,0b00111100,
  0b11000000,0b00011111,
  0b00000000,0b00001100
};

const uint8_t epd_bitmap_int_der[] = 
{
  0b00000100,
  0b00001100,
  0b00011100,
  0b00111000,
  0b01110000,
  0b11111110,
  0b11111110,
  0b01110000,
  0b00111000,
  0b00011100,
  0b00001100,
  0b00000100
};

const uint8_t epd_bitmap_int_izq[] = 
{
  0b00100000,
  0b00110000,
  0b00111000,
  0b00011100,
  0b00001110,
  0b01111111,
  0b01111111,
  0b00001110,
  0b00011100,
  0b00111000,
  0b00110000,
  0b00100000
};

void setup() {
  // PIN_MODE
  //pinMode(LED_BUILTIN, OUTPUT);   // Led interno ESP32C3  
  //
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);       // Set buzzer - pin 9 as an output
  //
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  WiFi.mode(WIFI_AP);
  esp_wifi_set_max_tx_power(40); 

  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //Serial.print("LED: ");
  //Serial.println("" + String(LED_BUILTIN));
  server.begin();
  Serial.println("Server started");
  //  Configuración Html
  cabeceraHttp = getCabeceraHttp();
  // Pantalla
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.setContrast(255); // maximum contrast 
  u8g2.setBusClock(400000);   // 400kHz I2C 
  u8g2.setFont(u8g2_font_ncenB10_tr); 
  // Primera pantalla
  mostrarPantallaBienvenida();
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients
  //
  if (client) {                     // if you get a client,
    Serial.println("New Client");  // print a message out the serial port
    currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        if (c == '\n') {            // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() > 0) {
            iPosWebPrincipal = currentLine.indexOf(paginaPrincipalGET); // Comprueba que sea la página web principal GET
            if (iPosWebPrincipal >= 0) {
              // Lee y gestiona las peticiones de la URL
              gestionarPeticiones(currentLine);
              // Inicializar pagina web cada vez para los {xx}
              pagina_web = getPaginaWeb();
              // Intercambio de variables
              intercambioVariables();                          
              // Activar/Desactivar actuadores fijos, luces, etc.
              gestionarActuadores();
              // Mostrar en pantalla
              mostrarPantalla();

              /* CLIENT INI */
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.print(cabeceraHttp);
              // Página Web
              client.print(pagina_web);
              // The HTTP response ends with another blank line:
              client.println();
              /* CLIENT FIN */

              // break out of the while loop:  
              break;
            }
            // No es una petición CIENT de GET
            /* CLIENT INI - REFRESH*/
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.print(cabeceraHttp);
            // Página Web REFRESH
            pagina_web_refresh = getPaginaWebRefresh();
            client.print(pagina_web_refresh);
            // The HTTP response ends with another blank line:
            client.println();
            /* CLIENT FIN */
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }     
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  else {
      delay(CTE_TEMPS_ESPERA_BUCLE);  // delay si no atiende a CLIENT
  }
  //  Actuadores secuenciales, intermitentes, sirena
  gestionarActuadoresSecuenciales();
  //  Mostrar intermitentes en Pantalla
  if (bWarnings || bIntermitenteIzq || bIntermitenteDer || bSensorLuz) {
    mostrarPantalla();
  }
  //  ENVIAR AL ESP32-SERIAL-595
  setRegister();   
}
void mostrarPantallaBienvenida() {
  u8g2.clearBuffer();                     // clear the internal memory
  u8g2.drawFrame(xOffset, yOffset, 72, 40);
  u8g2.setCursor(xOffset + 5, yOffset + 25); 
  u8g2.printf("%s", "HiCACA!");           // write something to the internal memory
  u8g2.sendBuffer();                      // transfer internal memory to the display  
}
void mostrarPantalla() {
  u8g2.clearBuffer();                     // clear the internal memory
    
  u8g2.drawFrame(xOffset, yOffset, 72, 40);
  // Intermitente izquierdo
  if ((bWarnings || bIntermitenteIzq) && bIntermitenteIzqON)
    u8g2.drawXBMP(xOffset + 3, yOffset + 4, 8, 12, epd_bitmap_int_izq);
  // Luces de POSICION
  if (bPosicion || bSensorLuz)
    u8g2.drawXBMP(xOffset + 10, yOffset + 4, 16, 12, epd_bitmap_luz_posicion);
  // Luces de CRUCE
  if (bCruce || bSensorLuz)
    u8g2.drawXBMP(xOffset + 26, yOffset + 4, 16, 12, epd_bitmap_luz_cruce);
  // Luces de CARRETERA
  if (bCarretera)
    u8g2.drawXBMP(xOffset + 42, yOffset + 4, 16, 12, epd_bitmap_luz_carretera);
  // Intermitente derecho
  if ((bWarnings || bIntermitenteDer) && bIntermitenteDerON)
    u8g2.drawXBMP(xOffset + 61, yOffset + 4, 8, 12, epd_bitmap_int_der);
  //  Velocidad Unidad de Potencia
  u8g2.setCursor(xOffset + 25, yOffset + 35); 
  if (iUnidadPotencia<10)
    u8g2.printf("0%d", iUnidadPotencia);    // write something to the internal memory
  else
    u8g2.printf("%d", iUnidadPotencia);     // write something to the internal memory

  u8g2.sendBuffer();                        // transfer internal memory to the display
}
void setRegister() {
  // Bloquea el Serial 595, envía el byte1 y después el byte2 y desbloque el Serial 595
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, iRespuesta1); 
  shiftOut(dataPin, clockPin, LSBFIRST, iRespuesta2); 
  digitalWrite(latchPin, HIGH);
}
void gestionarActuadoresSecuenciales() {
    // BUZZER
    if (!bBuzzer && mContador > CTE_MAX_BUCLE/2) {
      noTone(buzzerPin);                      // Stop sound...
    }
    if (bBuzzer && mContador <= CTE_MAX_BUCLE/2) {
      tone(buzzerPin, CTE_FRECUENCIA_BUZZER); // Send 1KHz sound signal...
      bBuzzer = false;
    }
    // Warnings
    if (bWarnings) {
      bIntermitenteDer = false;                           // DER -> FALSE
      bIntermitenteIzq = false;                           // IZQ -> FALSE
    }
    ////////////////
    // Intermitentes
    ////////////////
    if ((bWarnings || bIntermitenteIzq) && mContador <= CTE_MAX_BUCLE/2) {
      // Intermitente Izq ON
      iRespuesta2 = iRespuesta2 | CTE_IntermIzqON;	      // ON
      bIntermitenteIzqON = true;
    }
    if ((bWarnings || bIntermitenteIzq) && mContador > CTE_MAX_BUCLE/2)  {
      // Intermitente Izq OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermIzqON;	      // OFF
      bIntermitenteIzqON = false;
    }
    if (!bWarnings && !bIntermitenteIzq)  {
      // Intermitente Izq OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermIzqON;	      // OFF
      bIntermitenteIzqON = false;
    }
    //
    if ((bWarnings || bIntermitenteDer) && mContador <= CTE_MAX_BUCLE/2) {
      // Intermitente Der ON
      iRespuesta2 = iRespuesta2 | CTE_IntermDerON;        // ON
      bIntermitenteDerON = true;
    }
    if ((bWarnings || bIntermitenteDer) && mContador > CTE_MAX_BUCLE/2) {
      // Intermitente Der OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermDerON;	      // OFF
      bIntermitenteDerON = false;
    }
    if (!bWarnings && !bIntermitenteDer) {
      // Intermitente Der OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermDerON;	      // OFF
      bIntermitenteDerON = false;
    }
    ////////////////
    // Sirena
    ////////////////
    if (bSirena && mContador >= CTE_MIN_BUCLE && mContador <= CTE_MIN_BUCLE + 2) {
      // Sirena B
      iRespuesta2 = iRespuesta2 | CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaRON;	
    }
    if (bSirena && mContador == CTE_MIN_BUCLE + 3) {
      // Sirena R
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 | CTE_SirenaRON;	
    }
    if (bSirena && mContador >= CTE_MIN_BUCLE + 4 && mContador <= CTE_MIN_BUCLE + 6) {
      // Sirena B
      iRespuesta2 = iRespuesta2 | CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaRON;	
    }
    if (bSirena && mContador >= CTE_MIN_BUCLE + 7 && mContador <= CTE_MIN_BUCLE + 9) {
      // Sirena R
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 | CTE_SirenaRON;	
    }
    //  APAGADO GENERAL SI SE SOLICITA
    if (!bSirena) {
      // Sirena OFF
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaRON;				
    }	
    ////////////////
    //  CONTADOR
    ////////////////
    if (mContador++ > CTE_MAX_BUCLE-1)
      mContador = CTE_MIN_BUCLE;
}

void gestionarActuadores() {
   // Sensor de Luz
    int iLuz = getSensorLuzInt() ;
    bSensorLuz = (iLuz < CTE_LIMITE_LUZ);
    ////////////////
    // Luces
    ////////////////
    if (bCarretera) 
      // Luces de carretera ON
      iRespuesta1 = iRespuesta1 | CTE_LuzCarrON | CTE_LuzTrasON;	        // ON
      else
      iRespuesta1 = iRespuesta1 & ~CTE_LuzCarrON & ~CTE_LuzTrasON;	        // OFF
    if (bCruce || bSensorLuz) 
      // Luces de cruce ON
      iRespuesta1 = iRespuesta1 | CTE_LuzCrucON | CTE_LuzTrasON;	        // ON
      else
      iRespuesta1 = iRespuesta1 & ~CTE_LuzCrucON & ~CTE_LuzTrasON;	        // OFF
    if (bPosicion || bSensorLuz) 
      // Luces de posición ON
      iRespuesta1 = iRespuesta1 | CTE_LuzPosON | CTE_LuzTrasON;		        // ON
      else
      iRespuesta1 = iRespuesta1 & ~CTE_LuzPosON & ~CTE_LuzTrasON;	        // OFF
    // Luz trasera de posición, cruce y carretera
    if (bCarretera || bCruce || bPosicion || bSensorLuz) {
      iRespuesta1 = iRespuesta1 | CTE_LuzTrasON;          // ON  
    }
    else {
      iRespuesta1 = iRespuesta1 & ~CTE_LuzTrasON;         // OFF
    }
    //  GIRO DE RUEDAS
    if (iGiroRuedas < 40)
      // Luz Giro Iquierdo
      iRespuesta1 = iRespuesta1 | CTE_LuzGirIzqON;	      // ON
     else
      iRespuesta1 = iRespuesta1 & ~CTE_LuzGirIzqON;	      // OFF
    if (iGiroRuedas > 60)
      // Luz Giro Derecha
      iRespuesta1 = iRespuesta1 | CTE_LuzGirDerON;	      // ON
    else
      iRespuesta1 = iRespuesta1 & ~CTE_LuzGirDerON;	      // OFF
    // LUCES TRASERAS
    if (bLuzTraseraAtras) 
      // Luces de marcha atrás
      iRespuesta1 = iRespuesta1 | CTE_LuzAtrON;		        // ON
      else
      iRespuesta1 = iRespuesta1 & ~CTE_LuzAtrON;	        // OFF
    if (bLuzTraseraStop) 
      // Luz trasera Freno/STOP
      iRespuesta1 = iRespuesta1 | CTE_LuzStopON;          // ON
      else
      iRespuesta1 = iRespuesta1 & ~CTE_LuzStopON;	        // OFF
}
void gestionarPeticiones(String pCurrentLine) {
    // INTERMITENTE - DERECHO - S/N
    int iPos = pCurrentLine.indexOf("IND=");
    if (iPos >= 0) {
      bIntermitenteDer = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
      bIntermitenteIzq = false;
    }
    // INTERMITENTE - IZQUIERDO - S/N
    iPos = pCurrentLine.indexOf("INI=");
    if (iPos >= 0) {
      bIntermitenteIzq = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
      bIntermitenteDer = false;
    }
    // WARNINGS - S/N
    iPos = pCurrentLine.indexOf("WAR=");
    if (iPos >= 0) {
      bWarnings = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }
    // SIRENA - S/N
    iPos = pCurrentLine.indexOf("SIR=");
    if (iPos >= 0) {
      bSirena = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }
    // CLAXON - S/N
    iPos = pCurrentLine.indexOf("CLA=");
    if (iPos >= 0) {
      bBuzzer = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }
    // LUCES DE POSICION - S/N
    iPos = pCurrentLine.indexOf("LPO=");
    if (iPos >= 0) {
      bPosicion = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }
    // LUCES DE CRUCE - S/N
    iPos = pCurrentLine.indexOf("LCR=");
    if (iPos >= 0) {
      bCruce = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }
    // LUCES DE CARRETERA - S/N
    iPos = pCurrentLine.indexOf("LCA=");
    if (iPos >= 0) {
      bCarretera = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }
    // UNIDAD DE POTENCIA - 00,10,20,...,90,99
    iPos = pCurrentLine.indexOf("UPO=");
    if (iPos >= 0) {
      String vStr = pCurrentLine.substring(iPos+4, iPos+4+2);
      iUnidadPotencia = vStr.toInt();
    }
    // GIRO RUEDAS - 10,20,...,80,90
    iPos = pCurrentLine.indexOf("GRU=");
    if (iPos >= 0) {
      String vStr = pCurrentLine.substring(iPos+4, iPos+4+2);
      iGiroRuedas = vStr.toInt();
    }
    // LUZ MARCHA ATRÁS - S/N
    iPos = pCurrentLine.indexOf("MAT=");
    if (iPos >= 0) {
      bLuzTraseraAtras = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }
    // LUZ TRASERA FRENO/STOP - S/N
    iPos = pCurrentLine.indexOf("FRE=");
    if (iPos >= 0) {
      bLuzTraseraStop = (pCurrentLine.substring(iPos+4, iPos+4+1) == "S");
    }

}
int getSensorPosicionDel() {
  // Sensor de posición delantero
  int sensorPosDel = analogRead(SENSOR_POS_DEL);        // PIN 2
  int valor = map(sensorPosDel,0,4095,0,1000); 
  return valor;  
}
int getSensorPosicionTra() {
  // Sensor de posición delantero
  int sensorPosTra = analogRead(SENSOR_POS_TRA);        // PIN 5
  int valor = map(sensorPosTra,0,4095,0,1000); 
  return valor;  
}
int getSensorLuzInt() {
  // Sensor de LUZ
  int sensorLuz = analogRead(SENSOR_LUZ);               // PIN 3
  int valorLuz = map(sensorLuz,0,4095,0,1000); 
  return valorLuz;  
}
String convertSensor2String(int pValor) {
  // Sensor de LUZ
  int iNum = pValor / 10;
  int iDec = pValor % 10;  
  return String(iNum) + "." + String(iDec);
}

void intercambioVariables() {
  // Unidad de potencia
  pagina_web.replace("{18}", String(iUnidadPotencia));
  // Giro de ruedas
  pagina_web.replace("{19}", String(iGiroRuedas));
  // Sensor de LUZ
  pagina_web.replace("{17}", convertSensor2String(getSensorLuzInt()));
  // Sensor proximidad DELANTERO
  pagina_web.replace("{0}", convertSensor2String(getSensorPosicionDel()));  // Sensor proximidad DELANTERO
  // Sensor proximidad TRASERO
  pagina_web.replace("{1}", convertSensor2String(getSensorPosicionTra()));  // Sensor proximidad TRASERO
  // CLAXON
  if (bBuzzer) {
    // Toca el CLAXON
    pagina_web.replace("{20}", "<a href=\"."+paginaPrincipal+"?CLA=S\">CLAXON</a>");
  }  
  else {
    // Toca el CLAXON
    pagina_web.replace("{20}", "<a href=\"."+paginaPrincipal+"?CLA=S\">CLAXON</a>");
  }  
  // Luces de POSICION
  if (bPosicion) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{10}", "<a href=\"."+paginaPrincipal+"?LPO=N\">APAGAR</a>");
  }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{10}", "<a href=\"."+paginaPrincipal+"?LPO=S\">ENCENDER</a>");
  }
  // Luces de CRUCE
  if (bCruce) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{11}", "<a href=\"."+paginaPrincipal+"?LCR=N\">APAGAR</a>");
    }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{11}", "<a href=\"."+paginaPrincipal+"?LCR=S\">ENCENDER</a>");
  }
  // Luces de CARRETERA
  if (bCarretera) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{12}", "<a href=\"."+paginaPrincipal+"?LCA=N\">APAGAR</a>");
    }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{12}", "<a href=\"."+paginaPrincipal+"?LCA=S\">ENCENDER</a>");
  }
  if (bWarnings) {
    pagina_web.replace("{13}", "<a href=\"."+paginaPrincipal+"?WAR=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{13}", "<a href=\"."+paginaPrincipal+"?WAR=S\">ENCENDER</a>");
  }
  if (bSirena) {
    pagina_web.replace("{14}", "<a href=\"."+paginaPrincipal+"?SIR=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{14}", "<a href=\"."+paginaPrincipal+"?SIR=S\">ENCENDER</a>");
  }  
  if (bIntermitenteIzq) {
    pagina_web.replace("{15}", "<a href=\"."+paginaPrincipal+"?INI=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{15}", "<a href=\"."+paginaPrincipal+"?INI=S\">ENCENDER</a>");
  }  
  if (bIntermitenteDer) {
    pagina_web.replace("{16}", "<a href=\"."+paginaPrincipal+"?IND=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{16}", "<a href=\"."+paginaPrincipal+"?IND=S\">ENCENDER</a>");
  }  
  // Marcha atrás
  if (bLuzTraseraAtras) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{2}", "<a href=\"."+paginaPrincipal+"?MAT=N\">QUITAR</a>");
  }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{2}", "<a href=\"."+paginaPrincipal+"?MAT=S\">PONER</a>");
  }
  // Freno-STOP
  if (bLuzTraseraStop) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{3}", "<a href=\"."+paginaPrincipal+"?FRE=N\">SOLTAR</a>");
  }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{3}", "<a href=\"."+paginaPrincipal+"?FRE=S\">FRENAR</a>");
  }
}
String getCabeceraHttp() {
  String strCabHttp = String("HTTP/1.1 200 OK\n");
  strCabHttp += String("Content-type:text/html\n\n");
  return strCabHttp;
}
// Get
String getPaginaWebRefresh() {
  String strPaginaWeb = String("<!doctype html>");
  strPaginaWeb += String("<html lang=\"es\">");
  strPaginaWeb += String("<head>");
  strPaginaWeb += String("<title>CACA_ESP32_C3 - C a r</title>");
  strPaginaWeb += String("<meta charset=\"UTF-8\">");
  strPaginaWeb += String("</head>");
  strPaginaWeb += String("<html>");
  strPaginaWeb += String("<body>");

  strPaginaWeb += String("Página REFRESH");
 
  strPaginaWeb += String("</body>");
  strPaginaWeb += String("</html>");
  return strPaginaWeb;
}
// Get VAR PAGINA HTML
String getPaginaWeb() {
  String strPaginaWeb = String("<!doctype html>");
  strPaginaWeb += String("<html lang=\"es\">");
  strPaginaWeb += String("<head>");
  strPaginaWeb += String("<title>CACA_ESP32_C3 - C a r</title>");
  strPaginaWeb += String("<meta charset=\"UTF-8\">");
  strPaginaWeb += String("<meta http-equiv=\"refresh\" content=\""+String(CTE_TEMPS_REFRESH_HTML)+"; url=."+paginaPrincipal+"?REFRESH\">");
  strPaginaWeb += String("</head>");
  strPaginaWeb += String("<html>");
  strPaginaWeb += String("<body>");
  //
  strPaginaWeb += String("<table border=0>");
  strPaginaWeb += String("<tr><td colspan=\"2\" align=\"center\">C A R</td></tr>");
  strPaginaWeb += String("<tr><td>CLAXON</a></td><td>{20}</td></tr>");
  strPaginaWeb += String("<tr><td>LUCES POSICI&Oacute;N</a></td><td>{10}</td></tr>");
  strPaginaWeb += String("<tr><td>LUCES CRUCE</a></td><td>{11}</td></tr>");
  strPaginaWeb += String("<tr><td>LUCES CARRETERA</a></td><td>{12}</td></tr>");
  strPaginaWeb += String("<tr><td>WARNINGS</a></td><td>{13}</td></tr>");
  strPaginaWeb += String("<tr><td>SIRENA</a></td><td>{14}</td></tr>");
  strPaginaWeb += String("<tr><td>INTERMITENTE IZQUIERDO</a></td><td>{15}</td></tr>");
  strPaginaWeb += String("<tr><td>INTERMITENTE DERECHO</a></td><td>{16}</td></tr>");

  strPaginaWeb += String("<tr><td colspan=2>&nbsp;</td></tr>");  
  strPaginaWeb += String("<tr><td>Sensor proximidad DELANTERO</td><td>{0}&percnt;</td></tr>");
  strPaginaWeb += String("<tr><td>Sensor proximidad TRASERO</td><td>{1}&percnt;</td></tr>");
  strPaginaWeb += String("<tr><td>Sensor Luz</td><td>{17}&percnt;</td></tr>");

  strPaginaWeb += String("<tr><td>Marcha atrás</td><td>{2}</td></tr>");
  strPaginaWeb += String("<tr><td>Freno-STOP</td><td>{3}</td></tr>");
  strPaginaWeb += String("<tr><td colspan=2>&nbsp;</td></tr>");
  strPaginaWeb += String("<tr><td>Unidad de potencia</td><td>{18}&percnt;</td></tr>");
  strPaginaWeb += String("<tr>");
	strPaginaWeb += String("<td  colspan=2 align=\"center\">");
	strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=00\">PARAR</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=10\">10&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=20\">20&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=30\">30&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=40\">40&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=50\">50&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=60\">60&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=70\">70&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=80\">80&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=90\">90&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?UPO=99\">MAX VELOCIDAD</a>&rsqb;");
  strPaginaWeb += String("</td>");
  strPaginaWeb += String("</tr>");
  strPaginaWeb += String("<tr><td colspan=2>&nbsp;</td></tr>");
  strPaginaWeb += String("<tr><td>Giro ruedas delanteras</td><td>{19}&deg;</td></tr>");
  strPaginaWeb += String("<tr>");
	strPaginaWeb += String("<td  colspan=2 align=\"center\">");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=10\">-40&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=20\">-30&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=30\">-20&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=40\">-10&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=50\">RECTO</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=60\">+10&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=70\">+20&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=80\">+30&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"."+paginaPrincipal+"?GRU=90\">+40&deg;</a>&rsqb;");
  strPaginaWeb += String("</td>");
  strPaginaWeb += String("</tr>");
  strPaginaWeb += String("</table>");
  //
  strPaginaWeb += String("</body>");
  strPaginaWeb += String("</html>");
  return strPaginaWeb;
}
