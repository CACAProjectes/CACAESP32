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

#ifndef LED_BUILTIN
#define LED_BUILTIN 4  // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#endif

// Set these to your desired credentials.
const char *ssid = "CACA_ESP32_C3";
const char *password = "12345678";
//int LED_BUILTIN = 4;

// Set VAR PAGINA HTML
String pagina_web;

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
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

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
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            //client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            //client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
            pagina_web = getPaginaWeb();
            //
            pagina_web.replace("{0}", "11,3");        // Sensor proximidad DELANTERO
            pagina_web.replace("{1}", "8,5");         // Sensor proximidad TRASERO
            pagina_web.replace("{2}", "ENCENDIDA");   // Marcha atrás
            pagina_web.replace("{3}", "APAGADA");     // Freno-STOP
            pagina_web.replace("{4}", "86");          // Unidad de potencia
            //
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

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
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
  
  strPaginaWeb += String("<table border=0>");
  strPaginaWeb += String("<tr><td>LUCES POSICI&Oacute;N</a></td><td><a href=\"./LPO=S\">ENCENDER</a> / <a href=\"./LPO=N\">APAGAR</a></td></tr>");
  strPaginaWeb += String("<tr><td>LUCES CRUCE</a></td><td><a href=\"./LCR=S\">ENCENDER</a> / <a href=\"./LCR=N\">APAGAR</a></td></tr>");
  strPaginaWeb += String("<tr><td>LUCES CARRETERA</a></td><td><a href=\"./LCA=S\">ENCENDER</a> / <a href=\"./LCA=N\">APAGAR</a></td></tr>");
  strPaginaWeb += String("<tr><td>WARNINGS</a></td><td><a href=\"./WAR=S\">ENCENDER</a> / <a href=\"./WAR=N\">APAGAR</a></td></tr>");
  strPaginaWeb += String("<tr><td>SIRENA</a></td><td><a href=\"./SIR=S\">ENCENDER</a> / <a href=\"./SIR=N\">APAGAR</a></td></tr>");
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
 
  strPaginaWeb += String("</body>");
  strPaginaWeb += String("</html>");
  return strPaginaWeb;
}