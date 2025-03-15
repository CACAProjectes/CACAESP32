/*
  CACA Serial Luces
 */
//
// DATOS EN SERIE
const int dataPin = 8;   // SER
const int clockPin = 9;  // SRCLK
const int latchPin = 10;  // RCLK
// Variables internas
int contadorGeneral = 0;  // Contador general de tiempos
char inSerial = 0;        // Entrada del Serial estándar
String currentLine = "";  // String de la entrada Serial estándar
int iPos = -1;            // Posición de los comandos en el String Serial Stándar
int iContador = 0;        // Contador de bucle
// BUCLE
int CTE_MIN_BUCLE = 1;
int CTE_MAX_BUCLE = 10;
// Botones ACTIVOS
boolean bIntermitenteIzq  = true;
boolean bIntermitenteDer  = true;
boolean bSirena           = true;
boolean bPosicion         = false;
boolean bCruce            = false;
boolean bCarretera        = false;
// Máscaras de LUCES
byte CTE_IntermDerON  = 0b10000000;	// U1 - QA
byte CTE_IntermIzqON  = 0b01000000;	// U1 - QB
byte CTE_SirenaBON  	= 0b00100000;	// U1 - QC
byte CTE_SirenaGON  	= 0b00010000;	// U1 - QD
byte CTE_SirenaRON  	= 0b00001000;	// U1 - QE
// String de salida
byte iRespuesta1      = 0b00000000;
byte iRespuesta2      = 0b00000000;

void setup() {
  //
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

void loop() {
  for (iContador=CTE_MIN_BUCLE;iContador<=CTE_MAX_BUCLE;iContador++) {
    ////////////////
    // Intermitentes
    ////////////////
    if (bIntermitenteIzq && iContador <= CTE_MAX_BUCLE/2) 
      // Intermitente Izq ON
      iRespuesta2 = iRespuesta2 | CTE_IntermIzqON;	
    if (bIntermitenteIzq && iContador > CTE_MAX_BUCLE/2) 
      // Intermitente Izq OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermIzqON;	
    if (bIntermitenteDer && iContador <= CTE_MAX_BUCLE/2) 
      // Intermitente Der ON
      iRespuesta2 = iRespuesta2 | CTE_IntermDerON;
    if (bIntermitenteDer && iContador > CTE_MAX_BUCLE/2) 
      // Intermitente Der OFF
      iRespuesta2 = iRespuesta2 & ~CTE_IntermDerON;	
    ////////////////
    // Sirena
    ////////////////
    if (bSirena && iContador >= CTE_MIN_BUCLE && iContador <= CTE_MIN_BUCLE + 2) {
      // Sirena B
      iRespuesta2 = iRespuesta2 | CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaRON;	
    }
    if (bSirena && iContador == CTE_MIN_BUCLE + 3) {
      // Sirena R
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 | CTE_SirenaRON;	
    }
    if (bSirena && iContador >= CTE_MIN_BUCLE + 4 && iContador <= CTE_MIN_BUCLE + 6) {
      // Sirena B
      iRespuesta2 = iRespuesta2 | CTE_SirenaBON;	
      iRespuesta2 = iRespuesta2 & ~CTE_SirenaRON;	
    }
    if (bSirena && iContador >= CTE_MIN_BUCLE + 7 && iContador <= CTE_MIN_BUCLE + 9) {
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
    if (!bIntermitenteIzq) {
      iRespuesta2 = iRespuesta2 & ~CTE_IntermIzqON;
    }
    if (!bIntermitenteDer) {
      iRespuesta2 = iRespuesta2 & ~CTE_IntermDerON;
    }      
    //  ENVIAR AL ESP32-SERIAL-595
    setRegister();
    //
    leerSerial();
    //
    delay(150);
  }
}

void leerSerial() {
  // read from port 0, send to port 1:
  if (Serial.available()) {
    currentLine = "";
    do {
      inSerial = Serial.read();
      currentLine += inSerial;
    } while (inSerial != '\n');
    ///////////////////////
    // Comprobar comandos
    ///////////////////////
    // INTERMITENTE - DERECHO - S/N
    iPos = currentLine.indexOf("INTD=");
    if (iPos >= 0) {
      bIntermitenteDer = (currentLine.substring(iPos+5, iPos+5+1) == "S");
    }
    // INTERMITENTE - IZQUIERDO - S/N
    iPos = currentLine.indexOf("INTI=");
    if (iPos >= 0) {
      bIntermitenteIzq = (currentLine.substring(iPos+5, iPos+5+1) == "S");
    }
    // INTERMITENTE - SIRENA - S/N
    iPos = currentLine.indexOf("SIRE=");
    if (iPos >= 0) {
      bSirena = (currentLine.substring(iPos+5, iPos+5+1) == "S");
    }
    //
    Serial.print("currentLine: " + currentLine);
  }
}

void setRegister() {
  // Bloquea el Serial 595, envía el byte1 y después el byte2 y desbloque el Serial 595
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, iRespuesta1); 
  shiftOut(dataPin, clockPin, LSBFIRST, iRespuesta2); 
  digitalWrite(latchPin, HIGH);
}

