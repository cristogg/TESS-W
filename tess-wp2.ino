#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

//--- Arduino 1.6.5 ------
//SN   MAC addr.
//1->
//2->
//3->  18:FE:34:CF:E9:A3
//4->  18:FE:34:D3:48:CD 
//5->  5C:CF:7F:82:8E:FB
//6->  18:FE:34:D3:45:B9
//7->  5C:CF:7F:82:8D:7B
//8->  18:FE:34:D3:48:CD
//9->  18:FE:34:CF:EA:80
//10-> 18:FE:34:D3:47:77
//11-> 18:FE:34:D3:47:7F
//12-> 18:FE:34:CF:E8:84


/*
#define wifi_ssid "AstroHenares"
#define wifi_password "astrohenaresyngc457"
#define mqtt_server "192.168.0.101"
#define wifi_ssid "WLAN_3395"
#define wifi_password "47b3060084660517688a"
*/ 

#define mqtt_port 1883
#define mqtt_server_upm "rich.dia.fi.upm.es"  //DEBE leerse de broker, pero algo falla
#define mqtt_user_upm "guest"
#define mqtt_password_upm "ciclope"

#define mqtt_server_ucm "astrix.fis.ucm.es"  //147.96.67.45
#define mqtt_user_ucm "tess"
#define mqtt_password_ucm "k0L1vr1Sl0w"
//Para los TESS usuario: tess, passwd: xxxxxxxx
//Para el recolector de medidas usuario: tessdb passwd: yyyyyyy

#define mqtt_server_mosquito "test.mosquitto.org"
#define mqtt_server_192 "192.168.1.6"

//#define reads_topic "tess/coslada4/reads"
String rdtopic;
//#define status_topic  "tess/coslada4/status"
String sttopic;
#define TAMBUFF  200   //bufer rx serie medidas tess
#define EepromBuf 134  //espacio para variables de configuracion tess y AP
//#define TelnetPort 26


const IPAddress apIP(192, 168, 4, 1);
const char* apSSID = "TESS-W";
boolean settingMode;
String ssidList;

String ssid = "";
String pass = "";
String tname = "";
String cons = "";
String broker = "192.168.1.6";
String Envio = "";
String Puerto = "";
char mqtb[32];

int SegEnvio = 10;


boolean mqttcon;
boolean HayWifi;


void callback(const MQTT::Publish& pub) {
    //Serial.print(pub.payload_string());
}

WiFiClient espClient;
PubSubClient client(espClient, broker);

String CadTess;

String macID;
String Signal;
long rssi;
String jasonReads;
String jasonStatus;
String tess_channel = "pruebas";
String revision = "1";

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

float F_TAmbiente, TAmbiente;
float F_TObjeto, TObjeto;
float F_Magnitud, Magnitud;
float F_Frecuencia, Frecuencia;
bool abreparen, RecibidoTess, BufCompleto;
int posmeteo, CuentasEma, posBuf;
float  OfsetMV =0;
float ConstI = 20.0;
float OfsetTamb = 0;

float CuentaRxFrecuencia = 0;
float FrecuenciaM = 0;
float TAmbienteM = 0, TObjetoM = 0, MagnitudM = 0;
float CuentaRx_tA = 0, CuentaRx_tO = 0, CuentaRxMag = 0;

int mx, my, mz;
int tiemposer, tocaenviar;
int estado;
unsigned int Numero = 0;
unsigned int T5sg = 0;
unsigned int T30sg = 0;
unsigned int TryMqtt = 0;

int HeatPin = 14;                 // LED connected to digital pin 13

boolean modoAP = 0;

int TelnetPort = 23;


const char* ssid2     = "WLAN_42A7";
const char* password = "12345678";

//--------------

DNSServer dnsServer;
ESP8266WebServer webServer(80);
//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2
WiFiServer server(TelnetPort); //for send sockets to PC TESS program
WiFiClient serverClients[MAX_SRV_CLIENTS];

boolean restoreConfig();
boolean checkConnection();
void startWebServer();
void setupMode();
void ProcesarTess(unsigned int leidos);
float HzToMag(float HzTSL );
void reconnect();

//-----------------------------------------------------------
void setup() {
  
  Serial.begin(9600);
  delay(10);
  
  Serial.println();
  Serial.println();
  Serial.println("Connecting ");
    
 /* WiFi.begin(ssid2, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
*/

  EEPROM.begin(512);                    
  pinMode(HeatPin, OUTPUT);      // sets the digital pin as output
  
  if ( restoreConfig() ) {       //Si hay configuracion 
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int nn = WiFi.scanNetworks();
    delay(100);
    Serial.print("Num.Redes: "); 
    Serial.println(nn); 
//    setupMode(); 
    delay(100);
 
    WiFi.begin(ssid.c_str(), pass.c_str());
   // WiFi.begin(ssid2, password);
//    WiFi.mode(WIFI_STA); // Para quitar el AP en 192.168.4.1   
    settingMode = false;
    rdtopic = "STARS4ALL/" + tname + "/reading";
    sttopic = "STARS4ALL/register" ;
    //------------  
                                   
    if (checkConnection()) {                    //Si hay conexion wifi 
      HayWifi = 1;   
      Serial.print("Topic: ");
      Serial.println( rdtopic);
      Serial.print("Broker: ");
      Serial.println(broker);
//      client.setServer(broker.c_str(), mqtt_port);  //libreria anterior 
      client.set_callback(callback);
       
//      server.begin();                             //arranca Telenet
//      server.setNoDelay(true);
//      Serial.println("Ready for telnet ");
      modoAP = 0;
      startWebServer();  //crea las paginas
  
    }
    else {                                // no hay wifi pero si config
      HayWifi = false;
      settingMode = false;
      Serial.println(" AP data mode ");
   //   setupMode();            // Configura AP
      delay(100);
      WiFi.mode(WIFI_AP);
      modoAP = 1;
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
//      WiFi.softAP(apSSID);
      WiFi.softAP(tname.c_str());
      dnsServer.start(53, "*", apIP);
      delay(100);
      startWebServer();
//      Serial.println(WiFi.softAPIP());      
    } 

//WiFiServer server(TelnetPort); //for send sockets to PC TESS program
    server.begin();                             //arranca Telnet
    server.setNoDelay(true);
    Serial.println("Ready for telnet ");
       
  }
  else {                                        //no hay configuración
    Serial.println(" AP config mode");
    settingMode = true;     
    setupMode();            // Configura AP 
  }

  uint8_t mactess[WL_MAC_ADDR_LENGTH];  //18FE349CB4C1 modulo ofi
  WiFi.macAddress(mactess);
  macID = String(mactess[WL_MAC_ADDR_LENGTH - 6], HEX) + ":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 5], HEX)+":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 4], HEX) +":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 3], HEX)+":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 2], HEX) +":" +
          String(mactess[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
//  String AP_NameString = "Coslada " + macID;
  Serial.print("MAC: ");
  Serial.println(macID.c_str());

  Serial.print("Compiled ");
  Serial.print(__DATE__);
  Serial.print("  ");
  Serial.println(__TIME__);
lastSearch =  millis(); //si esta como AP evita reinicio 
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

  now2 = millis();  //cada 5 seg se comprueba si hay wifi por si se perdio
  if((settingMode==0) &&(modoAP == 0) && (now2 - lastMsg2 > 5000)) { 
    lastMsg2 = now2;      
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconectando Wifi");
      if (checkConnection())
        HayWifi = true;
        else {
          HayWifi = false;
          mqttcon = 0;
        }
    }
  }
  
  webServer.handleClient(); //mantiene la pagina web
// delay(100);
  dnsServer.processNextRequest();
    
  if (settingMode)
   ;       //dnsServer.processNextRequest();
  else if( (HayWifi) || (modoAP) ){  //conectado como STA 

    //check if there are any new clients telnet
    if (server.hasClient()){
lastSearch =  millis(); //si esta como AP evita reinicios para buscar wifi      
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()){
          if(serverClients[i]) serverClients[i].stop();
          serverClients[i] = server.available();
          Serial.print("New client: ");  Serial.println(i);
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
    if( BufCompleto) { //Si rx mensaje completo tess, se envia por telnet
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
      BufCompleto = 0;    
    }

    if (modoAP == false) { // hay wifi 
      now = millis();
      if (now - lastMsg > 1000) { //cada segundo se comprueba si hay dato valido 
         
        lastMsg = now;
        T5sg++;    
        if (T5sg == 2)
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
      //   client.publish(sttopic.c_str(), jasonStatus.c_str(), false);                  
            client.publish(MQTT::Publish(sttopic.c_str(), jasonStatus.c_str())
                  .set_qos(0)
                  .set_retain(true));
            delay(100);      
      
         Serial.println(jasonStatus);
        }
        else 
          reconnect();      
      }         
  		    if(T5sg >= SegEnvio) {
      	    if ( RecibidoTess) { // Envia cada n sg
        	T5sg = 0;
        	RecibidoTess = 0;  
          
       //   client.loop();  // mantiene enlace mqtt
        	
        	if (!client.connected())  //mantiene conexión mqtt
             reconnect();

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
          	//client.publish(rdtopic.c_str(), jasonReads.c_str(), true); //libreria anterior
            client.publish(MQTT::Publish(rdtopic.c_str(), jasonReads.c_str())
                  .set_qos(0)
                  .set_retain(true));
            delay(100);      
            Serial.println(" MQTT:");
          	Serial.println(jasonReads);
        	} 
        	else {
            if (HayWifi)
          	  Serial.println("Wait Broker"); 
        	} 
        // Serial.println(jasonReads); 
          //Serial.println(CadTess.c_str() );
      	}
      	    else {  //no llegan mensajes del modulo TESS     
      		    CadTess = "<WaitTESS>";
      		    BufCompleto = 1;
      		    T5sg = 0;
              Serial.println(CadTess);
      	    }
    	    }   
          if (T30sg < 15) {  //Se envia 3 veces en el arranque.
            T30sg++;
            T5sg = 0;
          }     
        }
     }
     else {  //es AP y tratamos de buscar wifi
        now3 = millis();
        if ((now3 - now4) > 5000) {
           now4 = now3;
           Serial.println((now3 - lastSearch) / 1000);
        }   
        
        if (now3 - lastSearch > 300000) {  //si estoy como AP cada 5 min
            lastSearch = now3;
            setup(); 
        }
        else if (now3 - lastSearch == 240000)   //si estoy como AP cada 5 min
           Serial.println("Searching Wifi in 60sg");
     }     
  }
      
  //------------------
    long ahora = millis();
    if (ahora - lastRd > 100) {  //read serial every 0.1 sg
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
          ProcesarTess(nd);        
        }
        else
            Serial.readBytes(BufRx, TAMBUFF); //si >200 hay un problema, se leen sin usarlos
    }
  }   // end read serial    
//--------------------  
}


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
      if (auxi != 0)          
        ssid += auxi;
    }
    Serial.print("SSID: ");
    Serial.println(ssid.c_str());
    
    for (int i = 16; i < 48; ++i) {  // Password 32B
      auxi = EEPROM.read(i);
      if (auxi != 0)          
        pass += auxi;
    }    
    Serial.print("Password: ");
    Serial.println(pass);

    for (int i = 48; i < 64; ++i) {  // Nombre 16B
      auxi = EEPROM.read(i);
      if (auxi != 0)
        tname += auxi;
    }
    Serial.print("Name: ");
    Serial.println(tname);

    for (int i = 64; i < 80; ++i) {  // cons 16B
      auxi = EEPROM.read(i);
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
      if (auxi != 0) {
        broker += auxi;
        mqtb[i-80] = auxi;
   //   broker = "rich.dia.fi.upm.es"; 
      }
    }
    Serial.print("Broker: ");
    Serial.println(broker);
    
    for (int i = 112; i < 128; ++i) {  // Seg. Envio 16B
      auxi = EEPROM.read(i);
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
    
    Serial.println("End Readig ...");
        
//    WiFi.begin(ssid.c_str(), pass.c_str());
    return true;
  }
  else {
    Serial.println("Config not found.");
    return false;
  }
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

//----------------------------------------------------------------------------------
void startWebServer() {
  if (settingMode) {
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    webServer.on("/settings", []() {
      String s = "<h2>TESS-W STARS4ALL<br>";
      s += "Wi-Fi & TESS Settings</h2><p>Please enter your password by selecting the SSID.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select> <br>Password: <input name=\"pass\" length=64 type=\"password\"><br>";
      s += "Name: <input name=\"tname\" length=32 type=\"text\"><br>";
      s += "Cons.: <input name=\"cons\" length=8 type=\"text\"><br>";
      s += "Seg: <input name=\"Envio\" length=8 type=\"text\"><br>";      
      s += "Tel.Port: <input name=\"Port\" length=8 type=\"text\"><br>";                  
      s += "Broker: <input name=\"broker\" length=8 type=\"text\"><br>";      
      s += "<input type=\"submit\"></form>";
      
      webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
    });
    
    webServer.on("/setap", []() {
      for (int i = 0; i < EepromBuf; ++i) {
        EEPROM.write(i, 0);
      }
      ssid = urlDecode(webServer.arg("ssid"));
      Serial.print("SSID: ");
      Serial.println(ssid);
      pass = urlDecode(webServer.arg("pass"));
      Serial.print("Password: ");
      Serial.println(pass);
      tname = urlDecode(webServer.arg("tname")); //---
      Serial.print("Name: ");
      Serial.println(tname);
      cons = urlDecode(webServer.arg("cons"));    //----
      Serial.print("Const. ");
      Serial.println(cons);
      Envio = urlDecode(webServer.arg("Envio"));    //----
      Serial.print("Envio: ");
      Serial.println(Envio);      
      Puerto = urlDecode(webServer.arg("Port"));    //----
      Serial.print("Port: ");
      Serial.println(Puerto);      

      
      broker = urlDecode(webServer.arg("broker")); //----
      Serial.print("Broker: ");
      Serial.println(broker);      
      
      Serial.println("Writing SSID to EEPROM...");
      for (int i = 0; i < ssid.length(); ++i) {
        EEPROM.write(i, ssid[i]);
      }
      Serial.println("Writing Password to EEPROM...");
      for (int i = 0; i < pass.length(); ++i) {
        EEPROM.write(16 + i, pass[i]);
      }
      Serial.println("Writing Name to EEPROM..."); //-
      for (int i = 0; i < tname.length(); ++i) {
        EEPROM.write(48 + i, tname[i]);
      }
      Serial.println("Writing Const to EEPROM..."); //--
      for (int i = 0; i < cons.length(); ++i) {
        EEPROM.write(64 + i, cons[i]);
      }
     
       Serial.println("Writing Broker to EEPROM..."); //---
      for (int i = 0; i < broker.length(); ++i) {
        EEPROM.write( 80 + i, broker[i]);
      }
      Serial.println("Writing Seg. to EEPROM..."); //--
      for (int i = 0; i < Envio.length(); ++i) {
        EEPROM.write(112 + i, Envio[i]);
      }
      Serial.println("Writing Port to EEPROM..."); //--
      for (int i = 0; i < Puerto.length(); ++i) {
        EEPROM.write(128 + i, Puerto[i]);
      }
      
      
      EEPROM.commit();
      Serial.println("Write EEPROM done!");
      String s = "<h1>Setup complete.</h1><p>device will be connected to \""; 
      s += ssid;
      s += "\" after the restart.";
      webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
      ESP.restart();
    });
    
    webServer.onNotFound([]() {
      String s = "<h1>TESS-W AP mode</h1><p><a href=\"/settings\">Wi-Fi TESS Settings</a></p>";
      webServer.send(200, "text/html", makePage("TESS AP mode", s));
    });
  }
  else {  //Hay configuracion
    if(HayWifi) {
       webServer.on("/", []() {
          String s = "<META HTTP-EQUIV=\"Refresh\" Content= \"4\" >";
          s+= " <h2>STARS4ALL";
          s+= "<br>TESS-W Data</h2>";
          s+= "<h3> Mag.V :  " + String(Magnitud) + " mv/as2";
          s+= "<br> Frec. : " + String( Frecuencia) + " Hz";                     
          s+= "<br> T. IR :   " + String(TObjeto) + " &ordm;C";      
          s+= "<br> T. Sens:   " + String(TAmbiente) + " &ordm;C";      
          s+= "<br>";
          s+= "<br> Wifi :   " + String(rssi) + " dBm";
          s+= "<br>mqtt sec.: " + String(Numero)  + "</h3>";
          s+= "<p><a href=\"/config\">Show Settings</a></p>";
     
           webServer.send(200, "text/html", makePage("STA mode", s));  
       });  
     
       webServer.on("/reset" , []() {      
          for (int i = 0; i < EepromBuf; ++i) {
             EEPROM.write(i, 0);
          }
          EEPROM.commit();
          String s = "<h1>Wi-Fi and TESS settings was reset.</h1><p>Please reset device.</p>";
          webServer.send(200, "text/html", makePage("Reset Wi-Fi TESS Settings", s));
          Serial.println("Config Erased. ");
       } );

       webServer.on("/config" , []() {
         String b = broker;
         b.replace(".", ",");
         String s = "<h2>TESS-W Settings.</h2>";   
         s+= "<a href=\"/erase\">Erase All Settings</a>";         
         s+= "<h4> <br> Compiled: " + String(__DATE__);           
         s+= "<br> MAC: " + macID;  
         s+= "<br> SSID: " + ssid;  
         s+= "<br> Ins.Const.: " + String( ConstI);
         s+= "<br> Tel.Port: " + Puerto; 
         s+= "<br> MQTT  "; 
         s+= "<br> Name : " + tname; 
         s+= "<br> Topic : " + rdtopic ;
         s+= "<br> Broker : " + String( mqtb);        
         s+= "<br> Period: " + Envio + "sec.</h4>"; 
         
         s+= "<p><a href=\"/Const\">Edit Ins.Const.</a></p>";                         
     
         webServer.send(200, "text/html", makePage("TESS-W Settings", s));
       } );

       webServer.on("/erase" , []() {                 
          String s = "<h2>TESS-W FULL RESET</h2>";
          s+= " All Setings will be deleted !!!"; 
          s+= "<p><a href=\"/reset\">RESET</a></p>";
      
          webServer.send(200, "text/html", makePage("Reset TESS Settings", s));
          Serial.println("Sending to Reset. ");
       } );


        webServer.on("/Const" , []() {      
          String s = "<h3> TESS-w Settings </h3>";
          s += "Edit Instrumental Constant.";
          s += "<br>Actual I.C.:" + String( ConstI);
          s += "<form method=\"get\" action=\"setconst\" > ";
          s += "<br>New I.C.: <input name=\"cons\" length=8 type=\"text\"><br>";
          s += "<input type=\"submit\"></form>";
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
    
       } );       

      webServer.on("/setconst" , []() {      
          cons = urlDecode(webServer.arg("cons"));    //----
          float Cons = cons.toFloat();
          if (( Cons > 10) && ( Cons < 30)) {
            ConstI = Cons;
            for (int i = 64; i < 80; ++i) {
              EEPROM.write(i, 0);
            }
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(64 + i, cons[i]);
            }
            EEPROM.commit();                      
            Serial.println("Write EEPROM done!");
            String s = "<h3>New I.Const. "; 
            s += cons;
            s += "</h3>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          } 
          else { 
            Serial.print("Const. out of range ");
            String s = "<p>Const. out of range</p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }    
       } );           
    
    }
    else {                              //hay Config pero no Wifi
        Serial.print("Starting apdata at ");
        Serial.println(WiFi.softAPIP());
        webServer.onNotFound([]() {
          String s = "<META HTTP-EQUIV=\"Refresh\" Content= \"4\" >";
          s+= "<h2> STARS4ALL ";
          s+= "<br> TESS-W AP Mode </h2>";
      //    s+= "<h1>  TESS-W AP Mode</h1>"; 
          s+= "<h3> Mag.V :  " + String(Magnitud) + " mv/as2"; 
          s+= "<br> Frec. : " + String( Frecuencia) + " Hz";        
          s+= "<br> T. Sens:   " + String(TAmbiente) + " &ordm;C";      
          s+= "<br> T. IR :   " + String(TObjeto) + " &ordm;C </h3>";      
          s+= "<br> No Wifi detected. For new wifi search, reset divice.";
          s+= "<p><a href=\"/config\">Show Settings</a></p>";          
          webServer.send(200, "text/html", makePage("AP mode", s));
        });        
        
        webServer.on("/config" , []() {
         String b = broker;
          b.replace(".", ",");
          String s = "<h2>TESS-W Settings.</h2>"; 
          s+= "<br> Compiled: " + String(__DATE__);                       
          s+= "<br> MAC: " + macID;           
          s+= "<h4> SSID: " + ssid;  
          s+= "<br>Const.: " + String( ConstI);
          s+= "<br> Tel.Port: " + Puerto; 
          s+="<br><br> MQTT  "; 
          s+= "<br> Name : " + tname; 
          s+= "<br> Topic : " + rdtopic ;
          s+= "<br> Broker : " + String( mqtb);        
          s+= "<br> Period: " + Envio + "sec.</h4>";
     
          s+= " <p><a href=\"/erase\">Reset TESS-W Settings</a></p>";                                
                                            
         webServer.send(200, "text/html", makePage("TESS-W Settings", s));
       } );

      webServer.on("/erase" , []() {                 
          String s = "<h2>TESS-W FULL RESET</h2>";
          s+= " All Setings will be deleted !!!"; 
          s+= "<p><a href=\"/reset\">RESET</a></p>";
//        s += "<br> <input type=\"button\" onclick=\"alert()\">";
      
          webServer.send(200, "text/html", makePage("Reset TESS Settings", s));
          Serial.println("Sending to Reset. ");
       } );   

       webServer.on("/reset" , []() {      
          for (int i = 0; i < EepromBuf; ++i) {
             EEPROM.write(i, 0);
          }
          EEPROM.commit();
          String s = "<h1>Wi-Fi and TESS settings was reset.</h1><p>Please reset device.</p>";
 //           webServer.send(200, "text/html", makePage("Reset Wi-Fi TESS Settings", s));
          Serial.println("Config Erased. ");
       } );
               
        
     }
  }
  webServer.begin();
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
  dnsServer.start(53, "*", apIP);
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
  if(HayWifi == 1){
    if (!client.connected()) {
      Serial.print("Conecting MQTT: ");
  //   Serial.print(mqtt_server); //Conecting MQTT: rich.dia.fi.upm.es   port: 1883
      Serial.print(mqtb);
      Serial.print(" port: ");
      Serial.println(mqtt_port);
    
      // Attempt to connect                  //broker local o mosquito, sin pasword
      if( (broker.indexOf("168") > 0) || (broker.indexOf("mosqui") > 0) ) {   
        if (client.connect("TesClient")) { // libreria anterior
          Serial.println("connected");
          mqttcon = 1; 
        }         
      }
//WiFiClient espClient;
//PubSubClient client(espClient, broker);
      
      else if( broker.indexOf("upm") > 0) {   //broker politecnica
//        if (client.connect("ESP8266Client", mqtt_user_upm, mqtt_password_upm)) { //libreria anterior
        if (client.connect(MQTT::Connect("TessClient")
          .set_auth(mqtt_user_upm, mqtt_password_upm))) {
          Serial.println("connected upm");
          mqttcon = 1; 
        }         
      }     
      else if( broker.indexOf("ucm") > 0) {  //broker complutense
//        if (client.connect("ESP8266Client", mqtt_user_ucm, mqtt_password_ucm)) { //libreria anterior
      if (client.connect(MQTT::Connect("TessClient")
       .set_auth(mqtt_user_ucm, mqtt_password_ucm))) {
          Serial.println("connected ucm");
          mqttcon = 1; 
        } 
      } 
      else Serial.println("Broker unknow");

//Serial.println(broker.c_str());
      
      if(mqttcon != 1)
      {
   //    Serial.print("failed, rc=");
   //     Serial.print(client.state());
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
void ProcesarTess(unsigned int leidos)
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
       if ((buff_meteo[p_rd] == '>' ) && (posmeteo <= 6)  ) {
          Meteo[posmeteo]= 0;
          CadMeteo = Meteo;
          posmeteo = 0;
       }

       if ((buff_meteo[p_rd] == '>' )   )  // ha terminado un mensaje de comandos
       {
          Meteo[posmeteo]= 0;
          CadMeteo = Meteo;
          posmeteo = 0;        
          Cad = CadMeteo.substring(4,9);         //se toma el valor  
          if   (CadMeteo.indexOf("<tA") >= 0) {  //y se busca a que corresponde        
             CuentaRx_tA++;
             au = atof(Cad.c_str())/100.0 + OfsetTamb;
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
          }
          else if  (CadMeteo.indexOf("<f") >= 0)  {
            if(Meteo[2] == 'H') {   // H son hercios, mucha luz             
               if(Meteo[3] != '-'){ //con "-" el micro indica fuera de rango
                 Frecuencia =  atoi(Cad.c_str());
               //  FrecuenciaM += Frecuencia;
               //  CuentaRxFrecuencia++;

                 Magnitud = HzToMag( Frecuencia);               
               }
               else {
                 Magnitud = 0;
                 Frecuencia =0;
               }
            }
            else if (Meteo[2]== 'm') {  //m son milihercios, cielo oscuro 
                 Frecuencia = atof(Cad.c_str())/1000.0;
             //    FrecuenciaM += Frecuencia;
             //    CuentaRxFrecuencia++;

              Magnitud = HzToMag( Frecuencia);
              //   mv = 100 * (HzToMag(ConstI * Frecuencia)) ;
              //   Magnitud = (mv/100.0) + OfsetMV;
            }
            FrecuenciaM += Frecuencia;
            CuentaRxFrecuencia++;            
            MagnitudM += Magnitud;
            CuentaRxMag++;

            if (( Magnitud > 14 ) && (TAmbiente < 10)) // Control Heater
                digitalWrite(HeatPin, HIGH );
            else
                digitalWrite(HeatPin,LOW);
          }
          else if  (CadMeteo.indexOf("<mZ") >= 0) { //ultimo dato en llegar
          //   F_TAmbiente = TAmbiente;
          //   F_TObjeto = TObjeto;
       /*      if ( F_TObjeto > 27 ) // Prueba Control Heater
                digitalWrite(HeatPin, HIGH );
              else
                digitalWrite(HeatPin,LOW);
         */
           //  F_Magnitud = Magnitud;
           //  F_Frecuencia = Frecuencia;
             
             BufTess[posBuf] = 0;
            CadTess = BufTess; 
            Serial.print(CadTess.c_str() );
             CadTess = CadTess + "\r\n";           
             posBuf = 0; //cadena grande completa, reinicio cadena
             BufCompleto = 1;             
             RecibidoTess = 1;
             }
          }
          p_rd++;
         if(p_rd >= TAMBUFF)
           p_rd = 0;
         leidos--;
         nd--;
    }
}

