/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include "esp_wifi.h"

#define LED_BUILTIN 4  // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// DATOS EN SERIE
const int dataPin         = 8;   // SER
const int clockPin        = 9;   // SRCLK
const int latchPin        = 10;  // RCLK
// Set these to your desired credentials.
const char *ssid          = "CACA_ESP32_C3";
const char *password      = "12345678";
// Set VAR PAGINA HTML
String pagina_web;
String cabeceraHttp;
// Botones ACTIVOS
boolean bWarnings         = false;
boolean bIntermitenteIzq  = false;
boolean bIntermitenteDer  = false;
boolean bSirena           = false;
boolean bPosicion         = false;
boolean bCruce            = false;
boolean bCarretera        = false;
// Máscaras de LUCES
byte CTE_IntermDerON      = 0b10000000;	// U1 - QA
byte CTE_IntermIzqON      = 0b01000000;	// U1 - QB
byte CTE_SirenaBON  	    = 0b00100000;	// U1 - QC
byte CTE_SirenaGON  	    = 0b00010000;	// U1 - QD
byte CTE_SirenaRON  	    = 0b00001000;	// U1 - QE
// String de salida
byte iRespuesta1          = 0b00000000;
byte iRespuesta2          = 0b00000000;
// BUCLE
int CTE_MIN_BUCLE         = 1;
int CTE_MAX_BUCLE         = 10;
int mContador             = CTE_MIN_BUCLE;
// Network
NetworkServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

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
  Serial.print("LED: ");
  Serial.println("" + String(LED_BUILTIN));
  server.begin();
  Serial.println("Server started");
  //  Configuración Html
  pagina_web = getPaginaWeb();
  cabeceraHttp = getCabeceraHttp();
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients
  //
  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // Lee y gestiona las peticiones de la URL
            gestionarPeticiones(currentLine);
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.print(cabeceraHttp);
            // Intercambio de variables
            intercambioVariables();
            // Página Web
            client.print(pagina_web);
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        /*
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
        }
        */
        // Activar/Desactivar actuadores
        gestionarActuadores();
        //  ENVIAR AL ESP32-SERIAL-595
        setRegister();        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
void setRegister() {
  // Bloquea el Serial 595, envía el byte1 y después el byte2 y desbloque el Serial 595
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, iRespuesta1); 
  shiftOut(dataPin, clockPin, LSBFIRST, iRespuesta2); 
  digitalWrite(latchPin, HIGH);
}
void gestionarActuadores() {
    ////////////////
    // Intermitentes
    ////////////////
    if (bIntermitenteIzq && mContador <= CTE_MAX_BUCLE/2) 
      // Intermitente Izq ON
      iRespuesta2 = iRespuesta2 | CTE_IntermIzqON;	
    if (bIntermitenteIzq && mContador > CTE_MAX_BUCLE/2) 
      // Intermitente Izq OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermIzqON;	
    if (bIntermitenteDer && mContador <= CTE_MAX_BUCLE/2) 
      // Intermitente Der ON
      iRespuesta2 = iRespuesta2 | CTE_IntermDerON;
    if (bIntermitenteDer && mContador > CTE_MAX_BUCLE/2) 
      // Intermitente Der OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermDerON;	
    ////////////////
    //  CONTADOR
    ////////////////
    if (mContador++ > CTE_MAX_BUCLE)
      mContador = CTE_MIN_BUCLE;

}
void gestionarPeticiones(String pCurrentLine) {
    // INTERMITENTE - DERECHO - S/N
    int iPos = pCurrentLine.indexOf("IND=");
    if (iPos >= 0) {
      bIntermitenteDer = (pCurrentLine.substring(iPos+5, iPos+5+1) == "S");
    }
    // INTERMITENTE - IZQUIERDO - S/N
    iPos = pCurrentLine.indexOf("INI=");
    if (iPos >= 0) {
      bIntermitenteIzq = (pCurrentLine.substring(iPos+5, iPos+5+1) == "S");
    }
    // WARNINGS - S/N
    iPos = pCurrentLine.indexOf("WAR=");
    if (iPos >= 0) {
      bWarnings = (pCurrentLine.substring(iPos+5, iPos+5+1) == "S");
    }
    // SIRENA - S/N
    iPos = pCurrentLine.indexOf("SIR=");
    if (iPos >= 0) {
      bSirena = (pCurrentLine.substring(iPos+5, iPos+5+1) == "S");
    }
}
void intercambioVariables() {
  // Luces de POSICION
  if (bPosicion) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{10}", "<a href=\"./LPO=N\">APAGAR</a>");
  }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{10}", "<a href=\"./LPO=S\">ENCENDER</a>");
  }
  // Luces de CRUCE
  if (bCruce) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{11}", "<a href=\"./LCR=N\">APAGAR</a>");
    }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{11}", "<a href=\"./LCR=S\">ENCENDER</a>");
  }
  // Luces de CARRETERA
  if (bCarretera) {
    // Enciende las luces y muestra el enlace de APAGAR
    pagina_web.replace("{12}", "<a href=\"./LCA=N\">APAGAR</a>");
    }
  else {
    // Apaga las luces y muestra el enlace de ENCENDER
    pagina_web.replace("{12}", "<a href=\"./LCA=S\">ENCENDER</a>");
  }
  if (bWarnings) {
    pagina_web.replace("{13}", "<a href=\"./WAR=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{13}", "<a href=\"./WAR=S\">ENCENDER</a>");
  }
  if (bSirena) {
    pagina_web.replace("{14}", "<a href=\"./SIR=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{14}", "<a href=\"./SIR=S\">ENCENDER</a>");
  }  
  if (bIntermitenteIzq) {
    pagina_web.replace("{15}", "<a href=\"./INI=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{15}", "<a href=\"./INI=S\">ENCENDER</a>");
  }  
  if (bIntermitenteDer) {
    pagina_web.replace("{16}", "<a href=\"./DER=N\">APAGAR</a>");
  }
  else {
    pagina_web.replace("{16}", "<a href=\"./DER=S\">ENCENDER</a>");
  }  
  // Sensor proximidad DELANTERO
  pagina_web.replace("{0}", "11,3");        // Sensor proximidad DELANTERO
  // Sensor proximidad TRASERO
  pagina_web.replace("{1}", "8,5");         // Sensor proximidad TRASERO
  // Marcha atrás
  pagina_web.replace("{2}", "ENCENDIDA");   // Marcha atrás
  // Freno-STOP
  pagina_web.replace("{3}", "APAGADA");     // Freno-STOP
  // Unidad de potencia
  pagina_web.replace("{4}", "86");          // Unidad de potencia
}
String getCabeceraHttp() {
  String strCabHttp = String("HTTP/1.1 200 OK\n");
  strCabHttp += String("Content-type:text/html\n\n");
  return strCabHttp;
}
// Set VAR PAGINA HTML
String getPaginaWeb() {
  String strPaginaWeb = String("<!doctype html>");
  strPaginaWeb += String("<html lang=\"es\">");
  strPaginaWeb += String("<head>");
  strPaginaWeb += String("<title>CACA_ESP32_C3 - C a r</title>");
  strPaginaWeb += String("<meta charset=\"UTF-8\">");
  strPaginaWeb += String("<meta http-equiv=\"refresh\" content=\"100; url=./K\">");
  strPaginaWeb += String("</head>");
  strPaginaWeb += String("<html>");
  strPaginaWeb += String("<body>");
  //
  strPaginaWeb += String("<table border=0>");
  strPaginaWeb += String("<tr><td>LUCES POSICI&Oacute;N</a></td><td>{10}</td></tr>");
  strPaginaWeb += String("<tr><td>LUCES CRUCE</a></td><td>{11}</td></tr>");
  strPaginaWeb += String("<tr><td>LUCES CARRETERA</a></td>{12}<td></td></tr>");
  strPaginaWeb += String("<tr><td>WARNINGS</a></td><td>{13}</td></tr>");
  strPaginaWeb += String("<tr><td>SIRENA</a></td><td>{14}</td></tr>");
  strPaginaWeb += String("<tr><td>INTERMITENTE IZQUIERDO</a></td><td>{15}</td></tr>");
  strPaginaWeb += String("<tr><td>INTERMITENTE DERECHO</a></td><td>{16}</td></tr>");
  strPaginaWeb += String("<tr><td colspan=2>&nbsp;</td></tr>");
  strPaginaWeb += String("<tr><td>Sensor proximidad DELANTERO</td><td>{0}&percnt;</td></tr>");
  strPaginaWeb += String("<tr><td>Sensor proximidad TRASERO</td><td>{1}&percnt;</td></tr>");
  strPaginaWeb += String("<tr><td>Marcha atrás</td><td>{2}</td></tr>");
  strPaginaWeb += String("<tr><td>Freno-STOP</td><td>{3}</td></tr>");
  strPaginaWeb += String("<tr><td colspan=2>&nbsp;</td></tr>");
  strPaginaWeb += String("<tr><td>Unidad de potencia</td><td>{4}&percnt;</td></tr>");
  strPaginaWeb += String("<tr>");
	strPaginaWeb += String("<td  colspan=2 align=\"center\">");
	strPaginaWeb += String("&lsqb;<a href=\"./UPO=00\">PARAR</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=10\">10&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=20\">20&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=30\">30&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=40\">40&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=50\">50&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=60\">60&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=70\">70&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=80\">80&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=90\">90&percnt;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./UPO=99\">MAX VELOCIDAD</a>&rsqb;");
  strPaginaWeb += String("</td>");
  strPaginaWeb += String("</tr>");
  strPaginaWeb += String("<tr><td colspan=2>&nbsp;</td></tr>");
  strPaginaWeb += String("<tr><td>Giro ruedas delanteras</td><td>45&deg;</td></tr>");
  strPaginaWeb += String("<tr>");
	strPaginaWeb += String("<td  colspan=2 align=\"center\">");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=10\">-40&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=20\">-30&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=30\">-20&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=40\">-10&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=50\">RECTO</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=60\">+10&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=70\">+20&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=80\">+30&deg;</a>&rsqb;");
  strPaginaWeb += String("&lsqb;<a href=\"./GRU=90\">+40&deg;</a>&rsqb;");
  strPaginaWeb += String("</td>");
  strPaginaWeb += String("</tr>");
  strPaginaWeb += String("</table>");
  //
  strPaginaWeb += String("</body>");
  strPaginaWeb += String("</html>");
  return strPaginaWeb;
}