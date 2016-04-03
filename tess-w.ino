#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

//--- Arduino 1.6.5 ------

/*
#define wifi_ssid "AstroHenares"
#define wifi_password "astrohenaresyngc457"
#define mqtt_server "192.168.0.101"
#define wifi_ssid "WLAN_3395"
#define wifi_password "47b3060084660517688a"
*/

#define mqtt_server "astrix.fis.ucm.es"  //147.96.67.45
//Para los TESS usuario: tess, passwd: xxxxxxxx
//Para el recolector de medidas usuario: tessdb passwd: yyyyyyy
#define mqtt_user "tess"
#define mqtt_password "xxxxxxx"
//#define mqtt_server "test.mosquitto.org"
//#define mqtt_server "192.168.1.44"

//#define reads_topic "tess/coslada4/reads"
String rdtopic;
//#define status_topic  "tess/coslada4/status"
String sttopic;
#define TAMBUFF  200  //bufer rx serie medidas tess
#define EepromBuf 112  //espacio para variables de configuracion tess y AP

const IPAddress apIP(192, 168, 4, 1);
const char* apSSID = "TESS_SETUP";
boolean settingMode;
String ssidList;

String ssid = "";
String pass = "";
String tname = "";
String gain = "";
String broker = "";
char mqtb[32];

boolean mqttcon;

WiFiClient espClient;
PubSubClient client(espClient);

String CadTess;

String macID;
String Signal;
long rssi;
String jasonReads;
String jasonStatus;
String tess_channel = "pruebas";
String revision = "1";

long lastMsg = 0;
float temp = 0.0;
float mag = 0.0;
float diff = 1.0;

long lastRd = 0;
int p_wr = 0;
int p_rd = 0;
int nd;
char buff_meteo[TAMBUFF];

char Meteo[200];
static char BufTess[200];

float F_TAmbiente, TAmbiente;
float F_TObjeto, TObjeto;
float F_Magnitud, Magnitud;
float F_Frecuencia, Frecuencia;
bool abreparen, RecibidoTess, BufCompleto;
int posmeteo, CuentasEma, posBuf;
float  OfsetMV =0;
float GainMV = 1.0;
float OfsetTamb = 0;

float CuentaRxFrecuencia, FrecuenciaM;
float TAmbienteM, TObjetoM, MagnitudM, CuentaRx_tA, CuentaRx_tO, CuentaRxMag;

int mx, my, mz;
int tiemposer, tocaenviar;
int estado;
unsigned int Numero = 0;
unsigned int T5sg = 0;
unsigned int T30sg = 0;
unsigned int TryMqtt = 0;
//--------------

DNSServer dnsServer;
ESP8266WebServer webServer(80);

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
WiFiServer server(26);
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
  EEPROM.begin(512);
  delay(10);
 
  if (restoreConfig()) {       //Mira si hay configuracion y hace WiFi.begin
    if (checkConnection()) {   //Espera la conexion
      settingMode = false;
         WiFi.mode(WIFI_STA); // + para quitar el AP
      startWebServer();
      
      rdtopic = "STARS4ALL/" + tname + "/reading";
      sttopic = "STARS4ALL/" + tname + "/status";
      Serial.print("Topic: ");
      Serial.println( rdtopic);
      Serial.print("Broker: ");
      Serial.println(mqtt_server);  //mosquitto
      client.setServer(mqtt_server, 1883);   //arranca MQTT  
 //    Serial.println(broker); 
 //    client.setServer(broker.c_str(), 1883);
//     client.setServer(mqtb, 1883);

      server.begin();           //arranca servidor para Telenet
      server.setNoDelay(true);
      Serial.print("Ready! Use 'telnet ");
      return;
    }
  }
  settingMode = true;
  setupMode();
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void loop() {
  bool jpreparado = 0;
  char BufRx[TAMBUFF];
  char msg[100];
	char au[32];
  long now;
 uint8_t i;
  static int con;
  
  webServer.handleClient(); //mantiene la pagina web
  
  if (settingMode)
    dnsServer.processNextRequest();
  else {  //conectado como STA

    //check if there are any new clients telnet
    if (server.hasClient()){
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()){
          if(serverClients[i]) serverClients[i].stop();
          serverClients[i] = server.available();
          Serial.print("New client: "); Serial.print(i);
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
              CadTess = CadTess + " NO BROKER " ; 
          }
          serverClients[i].write(CadTess.c_str(), CadTess.length());
          delay(1);
        }
      }
      BufCompleto = 0;    
    }
        
    
  //  client.loop();  // mantiene enlace mqtt
     
    now = millis();
    if (now - lastMsg > 1000) { //cada segundo se comprueba si hay dato valido    
      lastMsg = now;
      T5sg++;    
      if (T5sg == 2)
         rssi = WiFi.RSSI();
         
      if ( (RecibidoTess) & (T5sg > 9) ){
        T5sg = 0;
        RecibidoTess = 0;  
 client.loop();  // mantiene enlace mqtt
        if (!client.connected())  //mantiene conexión mqtt
             reconnect();
                                 
        Numero++;                    
        jasonReads = "{\"seq\":" + String(Numero) 
               + ", \"rev\":" + revision
               + ", \"name\":\"" + tname +"\""
               + ", \"freq\":" + String(Frecuencia)                    
                + ", \"mag\":" + String(F_Magnitud)                    
               + ", \"tamb\":" + String(F_TAmbiente) 
               + ", \"tsky\":" + String(F_TObjeto) 
               + ", \"wdBm\":" + String(rssi)
               + "}";
               
        if ( mqttcon == 1) {  //si hay conexion mqtt
          client.publish(rdtopic.c_str(), jasonReads.c_str(), true);
          Serial.println(jasonReads);
        } 
        else
          Serial.println("Sin broker"); 
      }
         
      if (T30sg < 15)  //Se envia 3 veces en el arranque.
           T30sg++;
        if ((T30sg == 3) || (T30sg == 8) || (T30sg == 13) ) {
           rssi = WiFi.RSSI(); 

        jasonStatus =  "{\"name\":\"" + tname +"\""
                   + ", \"rev\":" + revision
                   + ", \"mac\":\"" + macID +"\"" 
                   + ", \"cal\":" + gain 
                   + ", \"wdBm\":" + String(rssi)                   
                   + ", \"chan\":" + tess_channel
                   + "}";                     
         client.publish(sttopic.c_str(), jasonStatus.c_str(), false);                  
         Serial.println(jasonStatus);
      }
        
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
      } 	  	
    }
  }
}


//----------------------------------------------
boolean restoreConfig() {
  char auxi;
  Serial.println("Reading EEPROM...");
  
  if (EEPROM.read(0) != 0) {
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

    for (int i = 64; i < 80; ++i) {  // Gain 16B
      auxi = EEPROM.read(i);
      if (auxi != 0)
        gain += auxi;           
    }
    Serial.print("Gain: ");
    Serial.println(gain);

    for (int i = 80; i < EepromBuf; ++i) {  // Broker 32B
      auxi = EEPROM.read(i);
      if (auxi != 0)
        broker += auxi;
      mqtb[i-80] = auxi;
    }
    Serial.print("Broker: ");
    Serial.println(broker);
        
    WiFi.begin(ssid.c_str(), pass.c_str());
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
      uint8_t mactess[WL_MAC_ADDR_LENGTH];  //18FE349CB4C1 modulo ofi
  		WiFi.macAddress(mactess);
  		macID = String(mactess[WL_MAC_ADDR_LENGTH - 6], HEX) + ":" +
                 String(mactess[WL_MAC_ADDR_LENGTH - 5], HEX)+":" +
                 String(mactess[WL_MAC_ADDR_LENGTH - 4], HEX) +":" +
                 String(mactess[WL_MAC_ADDR_LENGTH - 3], HEX)+":" +
                 String(mactess[WL_MAC_ADDR_LENGTH - 2], HEX) +":" +
                 String(mactess[WL_MAC_ADDR_LENGTH - 1], HEX);
  		macID.toUpperCase();
  		String AP_NameString = "Coslada " + macID;
  		Serial.println(macID.c_str());                  
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
      s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\"><br>";
      s += "Name: <input name=\"tname\" length=32 type=\"text\"><br>";
      s += "Gain: <input name=\"gain\" length=8 type=\"text\"><br>";
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
      gain = urlDecode(webServer.arg("gain"));    //----
      Serial.print("Gain: ");
      Serial.println(gain);
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
      Serial.println("Writing Gain to EEPROM..."); //--
      for (int i = 0; i < gain.length(); ++i) {
        EEPROM.write(64 + i, gain[i]);
      }
       Serial.println("Writing Broker to EEPROM..."); //---
      for (int i = 0; i < broker.length(); ++i) {
        EEPROM.write( 80 + i, broker[i]);
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
  else {
  //  WiFi.mode(WIFI_STA); // + para quitar el AP
  //WiFi.disconnect();   
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.localIP());
    webServer.on("/", []() {
      String s = "<META HTTP-EQUIV=\"Refresh\" Content= \"7\" >";
      s+= "<h2>TESS-W STARS4ALL </h2><p><a href=\"/reset\">Reset Wi-Fi TESS Settings</a></p>";
      s+= "<h4> Topic : " + rdtopic + "</h4>"; //String(reads_topic);             
      s+= "<h2> Name : " + tname;             
      s+= "<br> Broker : " + broker;  
      s+= "<br> T. Amb:   " + String(TAmbiente) + " &ordm;C";      
      s+= "<br> T. IR :   " + String(F_TObjeto) + " &ordm;C";      
      s+= "<br> Mag.V :  " + String(F_Magnitud) + " mv/arcsg2";
      s+= "<br> Wifi :   " + String(rssi) + " dBm";
      s+= "<br> Sec. n&ordm;: " + String(Numero) +"</h2>";

      webServer.send(200, "text/html", makePage("STA mode", s));
    });
    webServer.on("/reset", []() {
      for (int i = 0; i < EepromBuf; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
      String s = "<h1>Wi-Fi and TESS settings was reset.</h1><p>Please reset device.</p>";
      webServer.send(200, "text/html", makePage("Reset Wi-Fi TESS Settings", s));
      Serial.print("Config Erased. \"");
    });
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
  Serial.print("Starting Access Point at \"");
  Serial.print(apSSID);
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
  // Loop until we're reconnected
//  while (!client.connected()) {
    if (!client.connected()) {
      Serial.print("Conecting MQTT...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
 //    if (client.connect("ESP8266Client")) {
     if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
   //   if (client.connect("ESP8266Client")) {
        Serial.println("connected");
        mqttcon = 1; 
      } 
      else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in seconds");
      // Wait 5 seconds before retrying----> Se espera en loop principal
      // delay(5000);
        mqttcon = 0;
    }
 }
}


//-----recibe Hz TSL y retorna magnitud visual -------------------
float HzToMag(float HzTSL )
{
  float mv;
    mv = HzTSL/230.0;   // Iradiancia en (uW/cm2)/10
    if (mv>0){
       mv = mv * 0.000001; //irradiancia en W/cm2
        //  mv = mv * 0.01; //irradiancia en W/m2
       mv = -1*(log10(mv)/log10(2.5)); //log en base 2.5 de la irradiancia.
                                          //log10(2.5) = 0.398   1/log10(2.5) = 2.5
    //  mv = 22 - 2.5*log10(HzTSL); //Frequency to magnitudes/arcSecond2 formula generica
        if (mv < 0) mv = 24;
     }
     else mv = 24;
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
                 Frecuencia = GainMV * atoi(Cad.c_str());
                 FrecuenciaM += Frecuencia;
                 CuentaRxFrecuencia++;

                 Magnitud = HzToMag(Frecuencia)  + OfsetMV ;
               }
               else {
                 Magnitud = 0;
                 Frecuencia =0;
               }

            }
            else if (Meteo[2]== 'm') {  //m son milihercios, cielo oscuro 
                 Frecuencia = GainMV * atof(Cad.c_str())/1000.0;
                 mv = 100 * (HzToMag(Frecuencia)) ;
                 Magnitud = (mv/100.0) + OfsetMV;
            }

            MagnitudM += Magnitud;
            CuentaRxMag++;
          }
          else if  (CadMeteo.indexOf("<mZ") >= 0) { //ultimo dato en llegar
             F_TAmbiente = TAmbiente;
             F_TObjeto = TObjeto;
             F_Magnitud = Magnitud;
             F_Frecuencia = Frecuencia;
             
             BufTess[posBuf] = 0;
            CadTess = BufTess;             
             posBuf = 0; //cadena grande completa, reinicio cadera
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

