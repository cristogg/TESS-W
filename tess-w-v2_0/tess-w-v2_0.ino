// -------------CAMBIOS-----------------------------------------------------
//7/6/2017  Incluido Ficheros en SD, RTC y HTU21D.
    //   En el arranque se comprueba si esta presente nuestro SSID 
    //   para evitar intentos de conexion imposibles.

//12/8/2017 Es posible editar sin resetar todo el acceso wifi y RTC
    // Si hay SD el programa apaga el wifi.
    // Si No hay SD el programa funciona como siempre.

//21-8-2017 En la conexion mqtt se pone como cliente tname
  // es posible poner otro nombre de broker que contenga "tess", igual autentificacion que ucm

//22-8-2017 Sin conexion, el nombre del AP es del tipo: TESS-stars1 para permitir varios fotometros juntos.
  //Si no hay configuración el AP muestra como nombre TESS-W

//25/10/2017 Opción de no intentar mandar MQTT si en el broker ponemos off, <4 caracteres

//24/11/2017 
// - Broadcast UDP con cadena jason especifica.
// - Compilado con Arduino 1.8.3 pero librería MQTT antigua

//29/11/2017 Trato configurar desde la ap 2.0
//falta espaciar envio udp a 1 sg
//parpadea led en esp-07 

//5/12/2017 Se procesan los datos del acelerometro magnetometro, desde la web se ajusta el magnetometro
//10/12/2017 La funcion de Gestion Web se saca a un fichero aparte para facilitar la edicion
//5/1/2018  La multicast UDP funciona en cualquier red.
//8/1/2018 El calentador se activa siempre que la temperatura < 5
//14/01/2018 Corregido error en cambio de SSID y passw
//-------------------------------------------------------------------------------

#include <ESP8266WiFi.h>
//#include <DNSServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>

#include <SPI.h>
#include <SD.h>
//#include <pgmspace.h>
#include <Wire.h>  // 
#include <RtcDS1307.h>
#include "SparkFunHTU21D.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


//-------!!!----Cambiar HeatPin segun modulo------!!!!!--------------------
int HeatPin = 14;  //esp07, Incompatible con tarjeta SD, que la usa como clk
//int HeatPin = 0;     //esp12S, PIO para calefactor, en TESS-W v1.0 está en el 14.
int DonePin = 2;     //PIO DONE para nanotimer, es el LED azul.

//--- I2C ---
//pin 13 SDA    PIO5
//pin  14 SCL   PIO4
//-- SPI tarjeta SD ---
//pin 5 CLK     PIO14
//pin  6 MISO   PIO12
//pin  7 MOSI   PIO13
//pin  10 CS    PIO15
//--------------

File myFile;
RtcDS1307 Rtc;
HTU21D myHumidity;
float humd21;
float temp21;
bool HaySHT21;
bool HayRTC;

int nHo;
int nMi;
int nSe;
int nAn;
int nMe;
int nDi;


#define mqtt_port 1883
#define mqtt_server_upm "rich.dia.fi.upm.es"  //DEBE leerse de broker, pero algo falla
#define mqtt_user_upm "xxxxxx"
#define mqtt_password_upm "xxxxxx"

#define mqtt_server_ucm "astrix.fis.ucm.es"  //147.96.67.45
#define mqtt_user_ucm "xxxxxx"
#define mqtt_password_ucm "xxxxx"

#define mqtt_server_mosquito "test.mosquitto.org"
#define mqtt_server_192 "192.168.1.6"

#define mqtt_server_local "192.168.1.33"
#define mqtt_user_local "xxxxxx" 
#define mqtt_password_local "xxxxxx"
//#define reads_topic "tess/coslada4/reads"
String rdtopic;
//#define status_topic  "tess/coslada4/status"
String sttopic;
String ESP8266Client;
#define TAMBUFF  200  //bufer rx serie medidas tess
#define EepromBuf 134  //espacio para variables de configuracion tess y AP
//#define TelnetPort 26

//const IPAddress apIP(192, 168, 4, 1);
IPAddress apIP(192, 168, 4, 1);
const char* apSSID = "TESS-W";

IPAddress IPfija(192, 168, 1, 111);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(225, 225, 225, 0);

WiFiUDP Udp;
unsigned int localUdpPort = 2250; // escucha
unsigned int remoteUdpPort = 2255; // envia
char incomingPacket[500]; // buffer for incoming packets
char replyPacekt[] = "Soy el 21"; // a reply string to send back


boolean settingMode;
String ssidList;

String ssid = "";
String pass = "";
String tname = "";
String cons = "";
String rtcDT = "";
String broker = "";
String Envio = "";
String Puerto = "";
char mqtb[32];

int SegEnvio = 10;


boolean mqttcon;
boolean HayWifi;
WiFiClient espClient;
PubSubClient client(espClient);

uint8_t mactess[WL_MAC_ADDR_LENGTH];  //18FE349CB4C1 modulo ofi

String CadTess;

String macID;
String Signal;
long rssi;
String jasonReads;
String jasonStatus;
String jasonLocal;
String tess_channel = "pruebas";
String revision = "1";
String rev_udp = "1";

long lastMsg = 0;
long lastMsg2 = 0;
long now4 = 0;
float temp = 0.0;
float mag = 0.0;
float diff = 1.0;


long lastRd = 0;
int p_wr = 0;
int p_rd = 0;
int nd;
char buff_meteo[TAMBUFF];
long lastSearch;
char Meteo[200];
static char BufTess[200];

float TAmbienteM, TObjetoM, Frecuencia_F, MagnitudM, CuentaRx_tA, CuentaRx_tO, CuentaRxMag; //Para gráfico
float TAmbiente_F, TObjeto_F, Magnitud_F, CuentaRx_tA_F, CuentaRx_tO_F, CuentaRxMag_F; //para fichero

float F_TAmbiente, TAmbiente;
float F_TObjeto, TObjeto;
float F_Magnitud, Magnitud;
float F_Frecuencia, Frecuencia;
float CuentaRxFrecuencia_F, FrecuenciaM_F;
bool abreparen, RecibidoTess, BufCompleto;
int posmeteo, CuentasEma, posBuf;
float  OfsetMV =0;
float ConstI = 20.0;
//float OfsetTamb = 0;

float CuentaRxFrecuencia = 0;
float FrecuenciaM = 0;
//float TAmbienteM = 0, TObjetoM = 0, MagnitudM = 0;
//float CuentaRx_tA = 0, CuentaRx_tO = 0, CuentaRxMag = 0;
int nube;

int tiemposer, tocaenviar;
int estado;
unsigned int Numero = 0;
unsigned int NumeroUdp = 0;
unsigned int T1sg = 0;
unsigned int T30sg = 0;
unsigned int TryMqtt = 0;
unsigned int T01sg = 0;
unsigned int T5sg = 0;



boolean modoAP = 0;
int TelnetPort = 23;
boolean EepromOK;

const char* ssid2     = "WLAN_42A7";
const char* password = "12345678";

long now5;
char datestring[20];
  
bool ssidOk;
bool SDcardOK;
bool NuevaMedia;
RtcDateTime ahorita;
int tamano; 

const byte DNS_PORT = 53;


//DNSServer dnsServer;
ESP8266WebServer webServer(80);
//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2
WiFiServer server(TelnetPort); //for send sockets to PC TESS program
WiFiClient serverClients[MAX_SRV_CLIENTS];

boolean restoreConfig();
boolean checkConnection();
void startWebServer();
void setupMode();
//void ProcesarTess(unsigned int leidos);
void ProcesarTessFull(unsigned int leidos);
float HzToMag(float HzTSL );
void reconnect();
void CalculaMedias();
void IniciaSD();
void IniciaRTC();
void printDateTime(const RtcDateTime& dt);
boolean GuardaLog();
void PrintMAC();
void GestionRed();
void RefrescaOled();
void EnviaUdp();    
void ConfigDefault();
void RestoreMagnetrometro();

//-----------------------------------------------------------
//-----calculos acelerometro---------------------------------
float roll, pitch, Acimut, heading, Arect, Decl, HSLgrados;
float pitchM, CuentaRx_pitch, headingM;
float F_roll, F_pitch, F_Acimut, F_heading,  F_Arect, F_Decl, F_HSLgrados;
int mx, my, mz;
//int mxs, Pxs;
int RecAM; //recibidos del acelerometro magnetometro, al llegar mz debe ser 6
typedef struct vector
 {
   float x, y, z;
 } vector;

vector p = {0, 1, 0};  //segun la colocacion del sensor
                      //valores por defecto si no hay fichero
vector m_max = {500, 500, 500};
vector m_min = {-500, -500, -500};
vector m_avg = {0,0,0};
vector MagMax, MagMin;
bool HayManetometro;
vector a_max = {1024, 1024, 1024};
vector a_min = {-1024, -1024, -1024};
vector a_avg = {0,0,0};
vector AceMax, AceMin;

enum calibration_mode {CAL_NONE, CAL_X, CAL_Y, CAL_Z};
void vector_cross(const vector *a, const vector *b, vector *out);
float vector_dot(const vector *a, const vector *b);
void vector_normalize(vector *a);
int get_heading(const vector *a, const vector *m, const vector *p);
//---------------------------------------------------------------

//--------------

//-----------------------------------------------------------
void setup() {
  
  pinMode(HeatPin, OUTPUT);      // sets the digital pin as output
  digitalWrite(HeatPin,LOW);
  pinMode(DonePin, OUTPUT);      // sets the digital pin as output
  digitalWrite(DonePin,HIGH);

  Serial.begin(9600);
  delay(10);
  Serial.println("-----------------------------------------------");
  Serial.print("Compiled ");
  Serial.print(__DATE__);
  Serial.print("  ");
  Serial.println(__TIME__);
  Serial.println("-----------------------------------------------");  

  EEPROM.begin(512); 
  Wire.begin(4,5); //Inicia puerto I2C(SCl,SDA):  SCL=PIO5=D1; SDA= PIO4=D2    
  IniciaRTC();
  myHumidity.begin(); //Inicia SHT21
  delay(100);    
  humd21 = myHumidity.readHumidity();  // Prueba si hay sensor SHT21
 
  if ((humd21 < 99) && (humd21 > 0)) { // Humedad dentro de margen
         temp21 = myHumidity.readTemperature();
         HaySHT21 = 1;
         Serial.println("SHT21 found:");
         Serial.print("T amb. ");
         Serial.println(temp21);
         Serial.print("HR ");
         Serial.println(humd21);
  }
  else 
    HaySHT21 = 0;
    
//---borra config y limpia memoria-------------
/*  for (int i = 0; i < EepromBuf; ++i) {
        EEPROM.write(i, 0);
  }
  EEPROM.commit();
//----------------------------
 */   
  if ( restoreConfig() ) {   //Lee configuracion EEPROM
       Serial.println("Config Found OK ");
       EepromOK = 1;
  }     
  else {
      ConfigDefault();
      EepromOK = 0;
//      GestionRed();         //se configura el wifi
  //    ESP8266Client = tname;
      settingMode = true;     
    }       
  
 // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done 
  // Clear the buffer.
  display.clearDisplay();
// text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,5);
  display.println("TESS");
  
  display.setTextSize(1);  
  display.print("ZP ");  
  display.println(ConstI);  
  display.print("vers.  ");
  display.println(__DATE__);
  
  display.display();
//  delay(2000);
 
  
  Serial.println(" ");
  WiFi.mode(WIFI_STA);    
  delay(1);             
  PrintMAC();   //saca mac y la deja en String macID       

  
  IniciaSD();  
//SDcardOK = 0;
  if (SDcardOK) { // Si hay tarjeta SD        
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();    //Desactiva completamente la wifi, <10mA
    delay(1);             
    Serial.println("--- Logger Mode -----------------------------------");  
  }
  else {   // NO hay SD, se configura la red para funcionamiento normal
    pinMode(HeatPin, OUTPUT);      //Calentador, pin as output
    digitalWrite(HeatPin,LOW);    // que es desconfigurado por IniciaSD
    
    Serial.println("--- WIFI Mode -------------------------------------");      
    if ( EepromOK ) {       //Si hay configuracion general
      GestionRed();         //se configura el wifi
      ESP8266Client = tname;
    }
    else {                                        //NO hay configuración
      Serial.println(" AP config mode");
      settingMode = true;     
      setupMode();            // Configura AP 
      Serial.println("-----------------------------------------------");        
    }
//    Udp.begin(localUdpPort);  //para escuchar, en principio no necesario
//    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);     
  } //fin de no hay SD 

  lastSearch =  millis(); //si esta como AP evita reinicio 
  now5 = millis();
}


//--------------------------------------------------------------
//--------------------------------------------------------------
void loop() {
  bool jpreparado = 0;
  char BufRx[TAMBUFF];
  char msg[100];
	char au[32];
  long now, now2, now3;
  uint8_t i;
  static int con, esperaWifi;
static bool pio;

//  if  ( RecibidoTess && SDcardOK ) { //Si hay SD miro para guardar datos
  if  ( SDcardOK ) { //Si hay SD miro para guardar datos      
      now2 = millis();
      if ( (now2 - now5) >= SegEnvio*100 ) {  
        RecibidoTess = 0; 
        now5 = millis();
        CalculaMedias();
        if (HaySHT21) {
          humd21 = myHumidity.readHumidity();
          if ((humd21 < 99) && (humd21 > 0))
            temp21 = myHumidity.readTemperature();
        }
        if ( GuardaLog() ) {  
          ahorita = Rtc.GetDateTime();
          printDateTime(ahorita);          
          Serial.print(datestring);
          Serial.println("  Done");
          delay(100);      
        }
        digitalWrite(DonePin,LOW);
      }    
  }   
  else {  //------------- No Hay SD ----------------
    now2 = millis();  //cada 5 seg se comprueba si hay wifi por si se perdio
    if((settingMode==0) &&(modoAP == 0) && (now2 - lastMsg2 > 5000)) {    
      if (HaySHT21) {
        humd21 = myHumidity.readHumidity();
        if ((humd21 < 99) && (humd21 > 0))
          temp21 = myHumidity.readTemperature();
      }    
      lastMsg2 = now2;      
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconectando Wifi");
        if (checkConnection())   //intenta conexión wifi
          HayWifi = true;
        else {
          HayWifi = false;
          mqttcon = 0;
        }
      }           
    }
/*    int noBytes = Udp.parsePacket(); //rx por UDP
    String received = "";
    if (noBytes) { 
      Serial.print("rx_udp: ");
      Serial.println(noBytes);
      Udp.read(incomingPacket, noBytes);
    }
*/    
    webServer.handleClient(); //mantiene la pagina web
//    dnsServer.processNextRequest();
    
    if (settingMode)
      ;       //dnsServer.processNextRequest();
    else if( (HayWifi) || (modoAP) ){  //conectado como STA  o como AP
      if (server.hasClient()){        //check if there are any new clients telnet
        lastSearch =  millis();       //si esta como AP evita reinicios para buscar wifi      
        for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
          if (!serverClients[i] || !serverClients[i].connected()){
            if(serverClients[i]) serverClients[i].stop();
            serverClients[i] = server.available();
            Serial.print("New client: ");  
            Serial.println(i);
            continue;
          }
        }
        //no free/disconnected spot so reject
        WiFiClient serverClient = server.available();
        serverClient.stop();
      }
        //check clients for data
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        if (serverClients[i] && serverClients[i].connected()){
          if(serverClients[i].available()){
            //get data from the telnet client and push it to the UART
            while(serverClients[i].available()) Serial.write(serverClients[i].read());
          }
        }
      }    
                //check UART for data
      if( BufCompleto) {       //Si rx mensaje completo tess, se envia por telnet
      BufCompleto = 0;         
        for( i = 0; i < MAX_SRV_CLIENTS; i++){
          if (serverClients[i] && serverClients[i].connected()){
            con++;
            if (con >2) {
              con = 0;
              if (mqttcon == 0)
                CadTess = CadTess + "<WaitBrok>" ; 
            }
            serverClients[i].write(CadTess.c_str(), CadTess.length());
            delay(1);
            lastSearch =  millis(); //si esta como AP evita reinicios para buscar wifi      
          }
        }
        
        if (T01sg >=10) { //Timer 1sg
          T01sg = 0;
       //   CalculaMedias();
          RefrescaOled();         
          EnviaUdp();           
//          Serial.print("Heading ");   //acimut
  //        Serial.print(heading);
        }
      }

      if (modoAP == false) {        // hay wifi 
        now = millis();
        if (now - lastMsg > 1000) { //cada segundo se comprueba si hay dato valido 
          lastMsg = now;
          T1sg++;    
          if (T1sg == 2)
            rssi = WiFi.RSSI();
          if ((T30sg == 3) || (T30sg == 8) || (T30sg == 13) ) {
            rssi = WiFi.RSSI(); 
            jasonStatus =  "{\"name\":\"" + tname +"\""
                   + ", \"rev\":" + revision
                   + ", \"mac\":\"" + macID +"\"" 
                   + ", \"chan\":\"" + tess_channel +"\""
                   + ", \"calib\":" + cons 
                   + ", \"wdBm\":" + String(rssi)                   
                   
                   + "}"; 
            if ( mqttcon == 1) {  //si hay conexion mqtt   
              Numero++;                  
              client.publish(sttopic.c_str(), jasonStatus.c_str(), false);                  
    //client.publish("outTopic","hello world");
              Serial.println(jasonStatus);
            }
            else 
              reconnect();      
          }         
  		    if(T1sg >= SegEnvio) {
      	    if ( RecibidoTess) { // Envia cada n sg
              CalculaMedias();             
          	  T1sg = 0;
        	    RecibidoTess = 0;                   	
        	    if (!client.connected())  //mantiene conexión mqtt
                reconnect();          
        	    jasonReads = "{\"seq\":" + String(Numero) 
                  + ", \"rev\":" + revision
                  + ", \"name\":\"" + tname +"\""
                  + ", \"freq\":" + String(F_Frecuencia)                    
                  + ", \"mag\":" + String(F_Magnitud)                    
                  + ", \"tamb\":" + String(F_TAmbiente) 
                  + ", \"tsky\":" + String(F_TObjeto) 
                  + ", \"wdBm\":" + String(rssi)
                  + "}";               
        	    if ( mqttcon == 1) {  //si hay conexion mqtt
          	    Numero++; 
          	    client.publish(rdtopic.c_str(), jasonReads.c_str(), true);
//                client.publish("outTopic","hello world");
                
                Serial.println(" MQTT:");
          	    Serial.println(jasonReads);
        	    } 
        	    else {
                if (HayWifi)
          	      Serial.println("Wait Broker"); 
        	    } 
      	    }
      	    else {  //no llegan mensajes del modulo TESS     
      		    CadTess = "<WaitTESS>";
      		    BufCompleto = 1;
      		    T1sg = 0;
              Serial.println(CadTess);
               EnviaUdp(); 
      	    }
    	    }   
          if (T30sg < 15) {  //Se envia 3 veces en el arranque.
            T30sg++;
            T1sg = 0;
          }     
        }
      }
      else {  //es AP y tratamos de buscar wifi
        now3 = millis();
        if ((now3 - now4) > 5000) {
           now4 = now3;
           Serial.println((now3 - lastSearch) / 1000);
//CadTess = "<Wait Config>";
        }           
        if (now3 - lastSearch > 300000) {  //si estoy como AP cada 5 min
            lastSearch = now3;
            //setup(); 
            GestionRed();
        }
        else if (now3 - lastSearch == 240000)   //si estoy como AP cada 5 min
           Serial.println("Searching Wifi in 60sg");
      }     
    }      
  } //fin sin SD
//-------------------------------------------------------------------------
  long ahora = millis();
  if ((ahora - lastRd) > 100) {  //read serial every 0.1 sg
    T01sg++;
 //   T1sg++;
    T5sg++;
    lastRd = ahora;
    if(Serial.available()){      
      size_t len = Serial.available();
       if(len <200) {  //si hay datos, no ruido.
          Serial.readBytes(BufRx, len);
          BufRx[len] = 0;
          for(int k = 0; k < len; k++)
          {
              buff_meteo[p_wr] = BufRx[k];
              p_wr++;
              if(p_wr >= TAMBUFF)
              p_wr = 0;
          }
          nd += len;
       //   ProcesarTess(nd); 
          ProcesarTessFull(nd); 
      }
      else
        Serial.readBytes(BufRx, TAMBUFF); //si >200 hay un problema, se leen sin usarlos
    }
  }   // end read serial    
}



//-----------------------------------------------------------------------
void CalculaMedias(){
  if (CuentaRxFrecuencia > 0)
               F_Frecuencia = FrecuenciaM/CuentaRxFrecuencia;
            else F_Frecuencia = 0;
  FrecuenciaM = 0; CuentaRxFrecuencia = 0;
  if (CuentaRxMag > 0)  
              F_Magnitud = MagnitudM /CuentaRxMag;
            else  F_Magnitud = 0;
  MagnitudM = 0; CuentaRxMag = 0;
  if (CuentaRx_tA > 0)  
              F_TAmbiente = TAmbienteM /CuentaRx_tA;
            else F_TAmbiente = 0;
  TAmbienteM = 0; CuentaRx_tA = 0;
  if ( CuentaRx_tO > 0)  
              F_TObjeto = TObjetoM /CuentaRx_tO;             
            else F_TObjeto = 0;
  TObjetoM = 0; CuentaRx_tO = 0; 

//   F_roll = roll;
//   F_pitch = pitch;
//   F_heading = heading;
  if (CuentaRx_pitch > 0)  {
     F_pitch = pitchM /CuentaRx_pitch;    
     F_heading = headingM /CuentaRx_pitch;             
  }
  else {
     F_pitch = 0;
     F_heading = 0;
  }
  pitchM = 0;            
  headingM = 0; 
 
  CuentaRx_pitch = 0;
}

/*  
//---borra config y limpia memoria-------------
void EraseEEprom() {
for (int i = 0; i < EepromBuf; ++i) {
        EEPROM.write(i, 0);
  }
  EEPROM.commit();
}
//----------------------------
*/ 

//----------------------------------------------
boolean restoreConfig() {
  char auxi;
  
  Serial.println("Reading EEPROM...");

  ssid = "";
  pass = "";
  tname = "";
  cons = "";
  broker = "";
  Envio = "";
  Puerto = "";
  
  if ((EEPROM.read(0) != 0) && (EEPROM.read(0) <124)){ //no esta limpia ni caracter raro 
//  if (EEPROM.read(0) != 0) {
    for (int i = 0; i < 16; ++i) {  // SSID  16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)          
        ssid += auxi;
    }
    Serial.print("SSID: ");
    Serial.println(ssid.c_str());
    
    for (int i = 16; i < 48; ++i) {  // Password 32B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)          
        pass += auxi;
    }    
    Serial.print("Password: ");
    Serial.println(pass);
    if (pass.length() < 4)
    {
      Serial.println("Config error.");
      return false;
    } 

    for (int i = 48; i < 64; ++i) {  // Nombre 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        tname += auxi;
    }
    Serial.print("Name: ");
    Serial.println(tname);
    if (tname.length() < 5)
    {
      Serial.println("Config error.");
      return false;
    }
    rdtopic = "STARS4ALL/" + tname + "/reading";
    sttopic = "STARS4ALL/register" ;
    
    for (int i = 64; i < 80; ++i) {  // cons 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        cons += auxi;           
    }
    ConstI = cons.toFloat();
    if (( ConstI < 2) || ( ConstI > 40))
       ConstI = 20;
    Serial.print("Const: ");
    Serial.println(ConstI);
    
//    for (int i = 80; i < EepromBuf; ++i) {  // Broker 32B
    for (int i = 80; i < 112; ++i) {  // Broker 32B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0) {
        broker += auxi;
        mqtb[i-80] = auxi;
   //   broker = "rich.dia.fi.upm.es"; 
      }
    }
    Serial.print("Broker: ");
    Serial.println(broker);
    if (broker.length() < 2)
    {
      Serial.println("Config error.");
      return false;
    }
    
    
    for (int i = 112; i < 128; ++i) {  // Seg. Envio 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        Envio += auxi;           
    }
    SegEnvio = Envio.toInt();
    if ((SegEnvio < 5) || (SegEnvio > 600)) {
      SegEnvio = 10;
      Envio = "10";
    }
    Serial.print("Seg.: ");
    Serial.println(Envio);

    for (int i = 128; i < 134; ++i) {  // Puerto Telenet 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        Puerto += auxi;           
    }
    TelnetPort = Puerto.toInt();
    if ((TelnetPort < 20) || (TelnetPort > 8080)) {
      TelnetPort = 26;
      Puerto = "26";
    }
    Serial.print("Port.: ");
    Serial.println(TelnetPort);

    RestoreMagnetometro();
    
    Serial.println("End Readig ...");
        
//    WiFi.begin(ssid.c_str(), pass.c_str());
    return true;
  }
  else {
    Serial.println("Config not found.");
    return false;
  }
}

void RestoreMagnetometro() {   //-------------------------------MAGNETOMETRO------------
 char auxi;
 String auxi2;
   
    for (int i = 134; i < 140; ++i) {  // Max X 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    m_max.x = auxi2.toInt();
    auxi2 = "";
    if ((m_max.x < 100) || (m_max.x > 1000)) {
      m_max.x = 500;
    }
    Serial.print("Max X: ");
    Serial.println(m_max.x);
//------------------------
    for (int i = 140; i < 146; ++i) {  // Max Z 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    m_max.y = auxi2.toInt();
    auxi2 = "";
    if ((m_max.y < 100) || (m_max.y > 1000)) {
      m_max.y = 500;
    }
    Serial.print("Max Y: ");
    Serial.println(m_max.y);
//------------------------
    for (int i = 146; i < 152; ++i) {  // Max Z 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    m_max.z = auxi2.toInt();
    auxi2 = "";
    if ((m_max.z < 100) || (m_max.z > 1000)) {
      m_max.z = 500;
    }
    Serial.print("Max Z: ");
    Serial.println(m_max.z);
//--------------------------------------------------------MINIMOS--------
    for (int i = 152; i < 158; ++i) {  // Max X 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    m_min.x = auxi2.toInt();
    auxi2 = "";
    if ((m_min.x < -1000) || (m_min.x > -100)) {
      m_min.x = -500;
    }
    Serial.print("Min X: ");
    Serial.println(m_min.x);
//------------------------
    for (int i = 158; i < 164; ++i) {  // Max Z 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    m_min.y = auxi2.toInt();
    auxi2 = "";
    if ((m_min.y < -1000) || (m_min.y > -100)) {
      m_min.y = -500;
    }
    Serial.print("Min Y: ");
    Serial.println(m_min.y);
//------------------------
    for (int i = 164; i < 170; ++i) {  // Max Z 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
       auxi2 += auxi;           
    }
    m_min.z = auxi2.toInt();
    auxi2 = "";
    if ((m_min.z < -1000) || (m_min.z > -100)) {
      m_min.z = -500;
    }
    Serial.print("Min Z: ");
    Serial.println(m_min.z);
//-----------------------
}


void ConfigDefault(){
  
  Serial.println("Conf not found. Defaul values.");

  ssid = "unknow";
  pass = "1234";
  tname = "unconf";  
  cons = "20.5";
  ConstI = 20.5;
  broker = "broker.unconfig";
  Envio = "10";
  SegEnvio = 10;
  Puerto = "23";
  TelnetPort = 23;

    Serial.print("SSID: ");
    Serial.println(ssid.c_str());
    Serial.print("Password: ");
    Serial.println(pass);
    Serial.print("Name: ");
    Serial.println(tname);
    rdtopic = "STARS4ALL/" + tname + "/reading";
    sttopic = "STARS4ALL/register" ;
    Serial.print("Const: ");
    Serial.println(ConstI);
    Serial.print("Broker: ");
    Serial.println(broker);    
    Serial.print("Seg.: ");
    Serial.println(Envio);
    Serial.print("Port.: ");
    Serial.println(TelnetPort);
}

boolean checkConnection() {
  int count = 0;
 
  Serial.print("Waiting for Wi-Fi connection");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return (true);
    }
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("Timed out.");
  return false;      
}

//---------------------------------------------------------------------------
void setupMode() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  Serial.println("");
  for (int i = 0; i < n; ++i) {
    ssidList += "<option value=\"";
    ssidList += WiFi.SSID(i);
    ssidList += "\">";
    ssidList += WiFi.SSID(i);
    ssidList += "</option>";
  }
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID);
//  dnsServer.start(53, "*", apIP);
  startWebServer();

  if(HayWifi == 0){
    Serial.print("Starting AP at ");
    Serial.println(WiFi.softAPIP());
  }
  else 
  {
    Serial.print("Starting Web at \"");
    Serial.print(apSSID);   
  }
  Serial.println("\"");
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
//  s += "<META HTTP-EQUIV=\"Refresh\" Content= \"7\" >";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += contents;
  s += "</body></html>";
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}

//--------Reconnect MQTT--------
void reconnect() {
  if (broker.length() < 4 ) return;
  if(HayWifi == 1 ){  //si el nombre del broker es corto no se envía.
    if (!client.connected()) {
      Serial.print("Conecting MQTT: ");
  //   Serial.print(mqtt_server); //Conecting MQTT: rich.dia.fi.upm.es   port: 1883
      Serial.print(mqtb);
      Serial.print("-port: ");
      Serial.println(mqtt_port);
    
      // Attempt to connect                  //broker  mosquito, sin pasword
//      if( (broker.indexOf("168") > 0) || (broker.indexOf("mosqui") > 0) ) {   
      if( broker.indexOf("mosqui") > 0) {   
        if (client.connect(ESP8266Client.c_str()) ) {  //como cliente se pone el nombre del sensor
          Serial.println("connected");
          mqttcon = 1; 
        }         
      }
      else if( broker.indexOf("upm") > 0) {   //broker politecnica
        if (client.connect( ESP8266Client.c_str(), mqtt_user_upm, mqtt_password_upm)) {
          Serial.println("connected upm");
          mqttcon = 1; 
        }         
      }     
      else if( broker.indexOf("ucm") > 0) {  //broker complutense
//        if (client.connect("ESP8266Client", mqtt_user_ucm, mqtt_password_ucm)) {
        if (client.connect( ESP8266Client.c_str(), mqtt_user_ucm, mqtt_password_ucm)) {
          Serial.println("connected ucm");
          mqttcon = 1; 
        } 
      } 
      else if( broker.indexOf("tess") > 0) {   //broker  con tess en la URL
//        if (client.connect("ESP8266Client", mqtt_user_upm, mqtt_password_upm)) {
        if (client.connect( ESP8266Client.c_str(), mqtt_user_ucm, mqtt_password_ucm)) {
          Serial.println("connected ucm");
          mqttcon = 1; 
        }         
      }     
      else if( broker.indexOf("168") > 0) {   //broker  local
//        if (client.connect("ESP8266Client", mqtt_user_upm, mqtt_password_upm)) {
        if (client.connect( ESP8266Client.c_str(), mqtt_user_local, mqtt_password_local)) {
          Serial.println("connected to local mosquitto");
          mqttcon = 1; 
        }         
      }     
      
      else Serial.println("Broker unknow");
      
      if(mqttcon != 1)
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in seconds");
      // Wait 5 seconds before retrying----> Se espera en loop principal
      // delay(5000);
        mqttcon = 0;
      }
   }
  }
  else
   ;// Serial.println("Esperando Wifi");
}


//-----recibe Hz TSL y retorna magnitud visual -------------------
float HzToMag(float HzTSL )
{
  float mv;
   if (HzTSL > 0)
     mv = ConstI - 2.5*log10(HzTSL);
   else
     mv = 0;  
     
    return mv;
}
//---------------------------------------------------------------------------


//------------------------------------------------------
//<fm 12345><tA +2312><tO -0312><aX +2312><aY +2312><aZ +2312><mX +2312><mY +2312><mZ +2312>
//{"seq":10308, "name":TESS-Madrid-01, "freq":7384.00, "mag":11.29, "tamb":23.51, "tsky":23.53, "wifidBm":-70}

//-------Analiza cadena recibida----------------------

//------------------------------------------------------------
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}


//-----------------------------------------------------
void IniciaSD() {
  File root;
  String cab, fich;
 
  Serial.print("Initializing SD card...");  
  // see if the card is present and can be initialized:
  if (!SD.begin(15)) {
    Serial.println("Card failed, or not present");
    SDcardOK = false;
    // don't do anything more:
    return;
  }
  SDcardOK = true;
  Serial.println("card initialized.");
  
  root = SD.open("/");
  printDirectory(root, 0);

//  myFile = SD.open("tessrec.txt", FILE_WRITE);
  fich = tname;
  fich+= ".txt";
  myFile = SD.open(fich, FILE_WRITE);

  if (myFile) {
    tamano = myFile.size();    
    if(tamano < 10) {
       Serial.print("Writing header...");
//        pp = "TESS ID: " & Main.NumSerie & " Gain: " & Round2(Main.ConsIns, 2) & CRLF
//        comienzo = "Date " & TAB & "Time" & TAB & "T IR" & TAB & "T Amb" & TAB & "Mag V " & TAB & " Frecuencia"  & TAB & " Nubes " & TAB & "Latitud " & TAB & "Longitud" & TAB & "Altura" & CRLF
      cab = String("TESS-W ID: ");
      cab+= "\t";      
      cab+= tname;
      cab+= "\tMAC: ";
      cab+= macID;
      cab+= "\tCI: ";
      cab+=  ConstI;
      myFile.println(cab);      
      Serial.print(cab);       
      cab= "\nDate \tTime \tT IR \tT Amb \tMag V \tFrec \tHR \tTA\n ";
      Serial.print(cab);
      myFile.println(cab);
      myFile.close();
      Serial.println("done.");
    }  
    else {
      Serial.print(" File exist, size: ");
      Serial.println(tamano);
    }  
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }
}
//------------------------------------------------------------------

void IniciaRTC() 
{
    //--------RTC SETUP ------------
    Serial.println("Iniciando RTC.");    
    Rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);  //Mira fecha de compilacion para comparar
    printDateTime(compiled);
    Serial.println(" ");
    HayRTC = 0;
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
        Serial.println("RTC lost confidence in the DateTime!");
        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue
        Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
         printDateTime(now);
         Serial.println(datestring);
         HayRTC = 1;
    }
    else if (now == compiled)     
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 
    
    delay(200);
    now = Rtc.GetDateTime();
    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("RTC MISING!");
         HayRTC = 0;
    }
}
//-------------------------------------------------------------------

//-------------------------------------------------------------
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
{
//    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
  //  Serial.print(datestring);
}

//----------------------------------------------------------------------
boolean GuardaLog()
{
   String dataStr = "";
   String fich;
   File root;

 //  Serial.println(CadTess.c_str() );  //Saca ultima cadena recibida

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC is out!");
        return false;      
    }

      ahorita = Rtc.GetDateTime();
      printDateTime(ahorita);
//     Serial.println();  

      dataStr = datestring;

      dataStr += "\t";  
      dataStr += String(F_TObjeto);     
      dataStr += "\t";  
      dataStr += String(F_TAmbiente); 
      dataStr += "\t";  
      dataStr += String(F_Magnitud);
      dataStr += "\t";  
      dataStr += String(F_Frecuencia);

      if (HaySHT21){
        dataStr += "\t";  
        dataStr += String(humd21);  
        dataStr += "\t";       
        dataStr += String(temp21);
      }   
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      fich = tname;
      fich+= ".txt";
      Serial.println(fich);
      root = SD.open("/");      
      File dataFile = SD.open(fich, FILE_WRITE);
      tamano = dataFile.size();    
      Serial.print("Size ");
      Serial.println(tamano);
      
    // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(dataStr);
        dataFile.close();
        Serial.println(dataStr);
      }
      else 
      Serial.println("Error opening file");  
    return true;
}

void PrintMAC()
{      
    uint8_t mactess[WL_MAC_ADDR_LENGTH];  //18FE349CB4C1 modulo ofi
    WiFi.macAddress(mactess);
    macID = String(mactess[WL_MAC_ADDR_LENGTH - 6], HEX) + ":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 5], HEX)+ ":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 4], HEX) + ":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 3], HEX)+ ":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 2], HEX) + ":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
//  String AP_NameString = "Coslada " + macID;
//    Serial.print("MAC: ");
    Serial.println(macID.c_str());
}

void GestionRed()
{
   int nn,i;
   String ak;
      Serial.println("--- Searching Wifi -------------------- "); 
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      Serial.print("STA MAC: ");
      PrintMAC();   //saca mac y queda en el String macID         
      nn = WiFi.scanNetworks();
      delay(100);
      Serial.print("Num.Redes: "); 
      Serial.println(nn); 
//    setupMode(); 
      delay(100);
//----
      for (int i = 0; i < nn; ++i) {    //Crea lista de redes.
        ssidList += "<option value=\"";
        ssidList += WiFi.SSID(i);
        ssidList += "\">";
        ssidList += WiFi.SSID(i);
        ssidList += "</option>";
      }
      delay(100);
//---   
//Buscamos si en la lista de redes esta la que buscamos    
      if(nn > 0)   {
        ssidOk = 0;
        for ( i = 0; i < nn; i++) {
          Serial.print(i+1); 
          Serial.print(": "); 
          Serial.print(WiFi.SSID(i)); 
          Serial.print(" ("); 
          Serial.print(WiFi.RSSI(i)); 
          Serial.println(")"); 
          if( WiFi.SSID(i) == ssid ) {
             Serial.println("SIDD presente");
             ssidOk = 1;
          }
        }  
        if( ssidOk ) 
          ;//Serial.println("SIDD presente");
        else     
          Serial.println("SIDD NO presente");      
         delay(10); 
      }   

      if(ssidOk) {                //el Punto Wifi está presente
   //     WiFi.config(IPfija, gateway, subnet);   //-----Para una IP FIJA- pero deja de funcionar UDP----
        WiFi.begin(ssid.c_str(), pass.c_str()); //trata de conectar
        settingMode = false;
        if (checkConnection()) {                    //Si hay conexion wifi 
          HayWifi = 1;   
          Serial.print("Topic: ");
          Serial.println( rdtopic);
          Serial.print("Broker: ");
          Serial.println(broker);
          client.setServer(broker.c_str(), mqtt_port);    
           modoAP = 0;
           startWebServer();                           //crea las paginas 
           delay(100);
        }
        else {  
          HayWifi = false;
          modoAP = 1;          
          settingMode = false;
 //         rdtopic = "STARS4ALL/" + tname + "/reading";
          Serial.println("----- SSID present but not accesible. Going to  AP data mode -----");
   //   setupMode();            // Configura AP
          delay(100);
          WiFi.mode(WIFI_AP);
          modoAP = 1;
          WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
//          WiFi.softAP("TESS");
        ak = "TESS-"+ tname;
        WiFi.softAP(ak.c_str());

          delay(100);
          startWebServer();         
        }
      }
      else {                                // NO hay Conexion con el wifi pero si config
        HayWifi = false;
        settingMode = false;
        
        Serial.println(" AP data mode ");
   //   setupMode();            // Configura AP
        delay(100);
        WiFi.mode(WIFI_AP);
        modoAP = 1;
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        ak = "TESS-"+ tname;
        WiFi.softAP(ak.c_str());
        delay(100);
        startWebServer();
      }     
      server.begin();                             //arranca Telnet
      server.setNoDelay(true);
      Serial.println("Ready for telnet ");
      Serial.println("-----------------------------------------------");   
}

void RefrescaOled() {
// ahorita = Rtc.GetDateTime();
//  printDateTime(ahorita); 
 // CalculaMedias();
  display.clearDisplay();
// text display tests
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
//  display.print(" -- MV --");
  if (HayWifi)  display.println("-- MV --+");
    else display.println(" -- MV --");
  display.setTextSize(2);
  display.println( Magnitud);

  display.setTextSize(1);  
display.println("----------");   
  display.print("TAmb:"); 
  display.println( TAmbiente);
  
  display.print("TIR:"); 
  display.println( TObjeto);
  display.display();
//  delay(1000);          
}


void EnviaUdp(void) {  
  String raiz;

          if (HayRTC) {
            ahorita = Rtc.GetDateTime();
            printDateTime(ahorita);          
          }
          jasonLocal = "{\"udp\":" + String(NumeroUdp) 
               + ", \"rev\":" + rev_udp
               + ", \"name\":\"" + tname +"\""
               + ", \"freq\":" + String(Frecuencia)                    
               + ", \"mag\":" + String(Magnitud)                    
               + ", \"tamb\":" + String(TAmbiente) 
               + ", \"tsky\":" + String(TObjeto) 
               + ", \"alt\":" + String(pitch) 
               + ", \"azi\":" + String(heading)                
               + ", \"wdBm\":" + String(rssi)
               + ", \"ZP\":" + String(ConstI)
          //     + ", \"RTC\":\"" + datestring  +"\""
               + "}";     
  if (modoAP)    
    Udp.beginPacket("192.168.4.255", remoteUdpPort);
  else {
     raiz =  WiFi.localIP().toString();
     int posi = raiz.lastIndexOf(".");
     String nuevaIP = raiz.substring(0,posi + 1);
     nuevaIP = nuevaIP + "255";
     Udp.beginPacket(nuevaIP.c_str(), remoteUdpPort);
  }  
  
  Udp.write(jasonLocal.c_str());
  Udp.endPacket(); 
  NumeroUdp++;
}


void vector_cross(const vector *a, const vector *b, vector *out)
{
   out->x = a->y * b->z - a->z * b->y;
   out->y = a->z * b->x - a->x * b->z;
   out->z = a->x * b->y - a->y * b->x;
}

float vector_dot(const vector *a, const vector *b)
{
   return a->x * b->x + a->y * b->y + a->z * b->z;
}

void vector_normalize(vector *a)
{
   float mag = sqrt(vector_dot(a, a));
   if(mag!=0){
     a->x /= mag;
     a->y /= mag;
     a->z /= mag;
   }
   else {
  // TESS->Memo1->Lines->Add("Error Acelerometro/Magnetometro");
     a->x = 0.1;
     a->y = 0.1;
     a->z = 0.1;
   }
}

//--------------------------------------------------------------------
// Returns a heading (in degrees)
// given an acceleration vector a due to gravity, a magnetic vector m, and a facing vector p.
//--------------------------------------------------------------------
int get_heading(const vector *a, const vector *m, const vector *p)
{
  vector E;
  vector N;
  float  k;

 // cross magnetic vector (magnetic north + inclination) with "down" (acceleration vector)
 //  to produce "east"
  vector_cross(m, a, &E);
  vector_normalize(&E);

 // cross "down" with "east" to produce "north" (parallel to the ground)
  vector_cross(a, &E, &N);
  vector_normalize(&N);

  //Calcula Roll
  k = pow(a->y, 2) + pow(a->z, 2);
  if(k != 0)
    ;
  else
    k = 0.0001;
  roll =  atan(a->x/sqrt(k));
  roll = roll*180/3.1416;

  //Calcula Altura
  k = pow(a->x, 2) + pow(a->z, 2);
  if( k!= 0)
    ;
  else
    k = 0.0001;
  pitch =  atan(a->y/sqrt(k));
  pitch = pitch*180/3.1416;

  //Calcula heading
  k = vector_dot(&N, p);
  if (k != 0)
    ;
  else {
  //  TESS->Memo1->Lines->Add("atan2");
    k = 0.00001; //proximo a cero
  }
  heading = atan2(vector_dot(&E, p), k) * 180.0 / M_PI;
  if (heading < 0)
     heading += 360;

  return heading;
}

//--------------------------------------------------------------

//-------Analiza cadena recibida con Acelerometro ------------------
void ProcesarTessFull(unsigned int leidos)
{
 String Cad, CadMeteo;
 char aux[200], auxi[20];
 int mv;
 float au;
 
    while (leidos > 0)      //mientras haya datos en el buffer
    {
       if (buff_meteo[p_rd] == '<' ) {         // Empieza cadena
          abreparen = 1;
          posmeteo = 0;
          for (int kon = 0; kon < 200; kon++) //limpia cadena
           Meteo[kon]= 0;
       }
       if (abreparen == 1) {
          Meteo[posmeteo] = buff_meteo[p_rd];
          posmeteo++;
          if (posmeteo > 200)
            posmeteo = 0;

          BufTess[posBuf] = buff_meteo[p_rd];
          posBuf++;
          if (posBuf > 200)
            posBuf = 0;
       }

       if ((buff_meteo[p_rd] == '>' )   )  // ha terminado un mensaje 
       {
          Meteo[posmeteo]= 0;
          CadMeteo = Meteo;
          posmeteo = 0;
          Cad = CadMeteo.substring(4,9);         //se toma el valor  
          if  (CadMeteo.indexOf("<tA") >= 0) {            
             CuentaRx_tA++;
             au = atof(Cad.c_str())/100.0;// + OfsetTamb;
             if((au > -50) && (au < 80))
               TAmbiente = au;
             TAmbienteM += TAmbiente;
          }
          else if  ( CadMeteo.indexOf("<tO") >= 0) {
             CuentaRx_tO++;
             au = atof(Cad.c_str())/100.0;
             if((au > -50) && (au < 50))
                TObjeto = au;
             TObjetoM += TObjeto;
             nube = (100 - 3*(TAmbiente - TObjeto) );
             if (nube > 100) nube = 100;
                 else if (nube < 0) nube = 0;
//----------------------------------------------------------
          }
          else if  ( CadMeteo.indexOf("<f") >= 0) {
            if(Meteo[2] == 'H') {   // H son hercios, mucha luz
               if(Meteo[3] != '-'){ //con "-" el micro indica fuera de rango
                 Frecuencia =  atoi(Cad.c_str());
                 Magnitud = HzToMag(Frecuencia) ;// + ConstIns ;
               }
               else {
                 Magnitud = 0;
                 Frecuencia =50000;
               }
            }
            else if (Meteo[2]== 'm') {  //m son milihercios, cielo oscuro
                 Frecuencia =  atof(Cad.c_str())/1000.0;
                 Magnitud = HzToMag(Frecuencia) ;
            }
            MagnitudM += Magnitud;
            CuentaRxMag++;
            FrecuenciaM += Frecuencia;
            CuentaRxFrecuencia++;

            if (( Magnitud > 14 ) && (TAmbiente < 10) || (TAmbiente < 5)){ // Control Heater
                digitalWrite(HeatPin, HIGH );
            }    
            else {
                digitalWrite(HeatPin,LOW);
            }    

          }
//--------------------------------------------------------------
          else if  ( CadMeteo.indexOf("<aY") >= 0) {
             a_avg.x = atof(Cad.c_str());
             RecAM++;             
          }
          else if  ( CadMeteo.indexOf("<aX") >= 0) {
             a_avg.y = -1.0 * atof(Cad.c_str());
             RecAM++;             
          }
           else if  ( CadMeteo.indexOf("<aZ") >= 0) {
              a_avg.z = atof(Cad.c_str());
              RecAM++;             
          }
//--------------------------------------------------------------
          else if  ( CadMeteo.indexOf("<mY") >= 0) {
             m_avg.x = atof(Cad.c_str());
             mx= m_avg.x;
              RecAM++;             
          }
         else if  ( CadMeteo.indexOf("<mX") >= 0) {
             m_avg.y = -1.0 *  atof(Cad.c_str());
             my= m_avg.y;
              RecAM++;             
          }
           else if  ( CadMeteo.indexOf("<mZ") >= 0) {  
             m_avg.z = atof(Cad.c_str());
             mz= m_avg.z;
             HayManetometro = 1;
             BufTess[posBuf] = 0;
             CadTess = BufTess; 
            if (!SDcardOK) {             //si hay SD solo saco cuando se escribe en SD
                if (T5sg >5) { //evita sacar por serie los primeros 5sg para evitar raton
                  Serial.println(CadTess.c_str() );
                  T5sg = 51;
                }
            }  
             CadTess = CadTess + "\r\n";           
             posBuf = 0;                //cadena grande completa, reinicio cadena
             BufCompleto = 1;             
             RecibidoTess = 1;              
             RecAM++;             
             if ( mx==0 && my==0 && mz==0 )
                ;   //si todas las lecturas =0 es que no hay sensor
             else if (RecAM == 6) //Asegura haber recibido los 6 parametros.
             {            
           //    MaxMin(); // Extraccion de valores maximos y minimos para posible calibración
               m_avg.x = (m_avg.x - m_min.x) / (m_max.x - m_min.x) * 2 - 1.0;  //Normaliza entre -1 y +1
               m_avg.y = (m_avg.y - m_min.y) / (m_max.y - m_min.y) * 2 - 1.0;
               m_avg.z = (m_avg.z - m_min.z) / (m_max.z - m_min.z) * 2 - 1.0;
               a_avg.x = (a_avg.x - a_min.x) / (a_max.x - a_min.x) * 2 - 1.0;
               a_avg.y = (a_avg.y - a_min.y) / (a_max.y - a_min.y) * 2 - 1.0;
               a_avg.z = (a_avg.z - a_min.z) / (a_max.z - a_min.z) * 2 - 1.0;
               heading = get_heading(&a_avg, &m_avg, &p); // Calcula orientacion de cada lectura               
 //              CalculoARDEC();

               pitchM += pitch;               
               headingM += heading;            
               CuentaRx_pitch++;               
             }
             else {
               Serial.print(RecAM);
               Serial.println("  RecAM corto o no ace ");   //
             }
             RecAM = 0;
          }
       }
       p_rd++;
       if(p_rd >= TAMBUFF)
          p_rd = 0;
       leidos--;
       nd--;
    }
}

