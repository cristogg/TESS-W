
//----------------------------------------------------------------------------------
void startWebServer() {

  if (settingMode) {            //No hay configuracion.
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    
    webServer.on("/settings", []() {
     String s = "<h2>TESS-W STARS4ALL<br>";
      s += "Wi-Fi & TESS Settings </h2>" ;
      s += "Firmware v: "  + String(vernum);       
      s += "<br>MAC: " + macID;                           
      s += "<br><br>Selec SSID and enter password.";                  
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select> <br>Password: <input name=\"pass\" length=64 type=\"password\">";
      s += "<br>Sensor: <input name=\"tname\" length=32 type=\"text\">";
      s += "<br>Filter (def. 0): <input name=\"cons\" length=4 type=\"text\">";
      s += "<br>CI (def. 20.50): <input name=\"nZP1\" length=5 type=\"text\">";
      if(Ncanales > 0){          
          s += "<br>CI 2: <input name=\"nZP2\" length=5 type=\"text\">";
          s += "<br>CI 3: <input name=\"nZP3\" length=5 type=\"text\">";
          s += "<br>CI 4: <input name=\"nZP4\" length=5 type=\"text\">";
      }   
      s += "<br>Offset hz (def. 0): <input name=\"offmhz\" length=4 type=\"text\">";
      s += "<br>Seg (def. 60): <input name=\"Envio\" length=4 type=\"text\">";      
      s += "<br>Tel.Port (def. 23): <input name=\"Port\" length=4 type=\"text\">";                  
      s += "<br>Broker: <input name=\"broker\" length=16 type=\"text\">";      
      s += "<br><input type=\"submit\"></form>";
      
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
      
      cons = urlDecode(webServer.arg("nZP1"));    //----                    
            ZP1 = cons.toFloat();
            Serial.print(" CI1: ");   Serial.print(ZP1);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(134 + i, cons[i]);
            }  
      if(Ncanales > 0){             
        cons = urlDecode(webServer.arg("nZP2"));    //----                    
            ZP2 = cons.toFloat();
            Serial.print(" CI2: ");   Serial.print(ZP2);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(140 + i, cons[i]);
            }  
        cons = urlDecode(webServer.arg("nZP3"));    //----                    
            ZP3 = cons.toFloat();
            Serial.print(" CI3: ");   Serial.print(ZP3);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(146 + i, cons[i]);
            }  
        cons = urlDecode(webServer.arg("nZP4"));    //----                    
            ZP4 = cons.toFloat();
            Serial.print(" CI4: ");   Serial.print(ZP4);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(152 + i, cons[i]);
            }  
      }
      offmhz = urlDecode(webServer.arg("offmhz"));    //----
      Serial.print("Offset. ");
      Serial.println(offmhz);

      cons = urlDecode(webServer.arg("cons"));    //----
      Serial.print("Filter type: ");
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
      if (ssid.length() < 3) ssid = "unknow";
      for (int i = 0; i < ssid.length(); ++i) {
        EEPROM.write(200 + i, ssid[i]);
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

      Serial.println("Writing Offset to EEPROM..."); //--
      for (int i = 0; i < offmhz.length(); ++i) {
        EEPROM.write(170 + i, cons[i]);
      }
           
      
      EEPROM.commit();
      Serial.println("Write EEPROM done!");
      String s = "<h1>Setup complete.</h1><p> Connecting to \""; 
      s += ssid;
      s += "\" now.";
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
       webServer.on("/", []() {                      //Pagina Principal
          String s = "<META HTTP-EQUIV=\"Refresh\" Content= \"4\" >";
          s+= " <h2>TESS-W STARS4ALL ";
          s+= "<br> Sensor: " + tname;
          s+= "<br></h2>";

          if (SDcardOK) {
             s+= "SD card detected ";             
             s+= "<br>File size " + String(tamano) + " Bytes" ;    
          } 
          if (HayRTC) {
/*            ahorita = Rtc.GetDateTime();
            printDateTime(ahorita);          
*/            //Serial.print(datestring);            
             s+= "<br>RTC:  ";
             s+= String(datestring); 
          } 
          if (HaySHT21) {
            s+= "<h3> HR :  " + String(humd21) + "%";
            s+= "<br> T amb. : " + String( temp21) + "&ordm;C";                     
          }   
          else if (BME280ok) {
            s+= "<h4> HR :  " + String(humedad) + "%";
            s+= "<br> T amb. : " + String( tempBME ) + "&ordm;C";  
            s+= "<br> Press. : " + String( pressure) + "Hp";  
            s+= "<br> Wind : " + String( AnalogA0/11.1) + "Km/h";                  
          }

          if (MLXok) {
          s+= "<br><br> T. IR :   " + String(TObjeto) + " &ordm;C";      
          s+= "<br> T. Sens:   " + String(TAmbiente) + " &ordm;C";                 
          }
           
          if ( ConstI == 1 )
            s+= "<br> RGB filters.";
          else if ( ConstI == 2 )
            s+= "<br> Johnson filters.";
                
          s+= "<br> Mag.1 :  " + String(Magnitud) + " mv/as2";
          s+= " f: " + String( Frecuencia) + " Hz"; 
          if (Ncanales > 0) { 
            s+= "<br> Mag.2 :  " + String(MagnitudG) + " mv/as2";
            s+= "  f2 : " + String( frecG) + " Hz";                     
            s+= "<br> Mag.3 :  " + String(MagnitudR) + " mv/as2";
            s+= " f3 : " + String( frecR) + " Hz";                   
            s+= "<br> Mag.4 :  " + String(MagnitudB) + " mv/as2";
            s+= " f4 : " + String( frecB) + " Hz";                     
          }                    
          s+= "<br>";
          s+= "<br> Wifi :   " + String(rssi) + " dBm";
          s+= "<br>mqtt sec.: " + String(Numero)  + "</h4>";
          s+= "<p><a href=\"/config\">Show Settings</a></p>";     //----------->
     
           webServer.send(200, "text/html", makePage("STA mode", s));  
       });  
     
       webServer.on("/config" , []() {                   //Pagina muestra Configuracion, con Wifi
         String b = broker;
         b.replace(".", ",");
         String s = "<h2>TESS-W Settings.</h2>";   
         s+= "<a href=\"/erase\">Erase All Settings</a>";                  
         s+= "<br>Compiled: " + String(__DATE__) ; 
         s+= "<br> Firmware v: " + String(vernum);          
         s+= "<br> MAC: " + macID;  
         s+= "<h4> SSID: " + ssid;  
         s+= "<br> Tel.Port: " + Puerto; 
          s+= "<br> Offset Hz: " + String( OffsetHz);
          if (ConstI == 0)
            s+= "<br> Filters: single chanel."; 
          else if ( ConstI == 1 )
            s+= "<br> Filters: 4  RGB."; 
          else if ( ConstI == 2 )
            s+= "<br> Filters: 4 Johnson."; 
          else   
            s+= "<br> Filters: 4 Custom."; 
         
          s += "<br> CI 1: " + String(ZP1);
          if (Ncanales > 0) { 
            s += "<br> CI 2: " + String(ZP2);
            s += "<br> CI 3: " + String(ZP3);
            s += "<br> CI 4: " + String(ZP4);
          }
          
          s+= "<br><br>  MQTT config "; 
          s+= "<br> Name : " + tname; 
          s+= "<br> Topic : " + rdtopic ;
          s+= "<br> Broker : " + String( mqtb);        
          s+= "<br> Period: " + Envio + "sec.</h4>";          
          s+= "<p><a href=\"/Const\">Edit Filter Type.</a></p>";  
          s+= "<p><a href=\"/Offset\">EditOffset.</a></p>";
          
          if (HayRTC) {
            s+= "<p><a href=\"/EditRTC\">Edit DATE TIME.</a></p>";  
          } 
//         if (HayManetometro) {
          s+= "<p><a href=\"/EditZP\"> Edit CI.</a></p>";  
  //       } 
      //    s+= "<p><a href=\"/settings\">Edit wifi.</a></p>";                // a edit SSID   
       //   s+= "<p><a href=\"/\">Edit CI.</a></p>";                         
         
         s+= "<p><a href=\"/\">Return Main</a></p>";     
         webServer.send(200, "text/html", makePage("TESS-W Settings", s));
       } );

       webServer.on("/reset" , []() {                   // Pagina Confirmacion del RESET
          for (int i = 0; i < EepromBuf; ++i) {
             EEPROM.write(i, 0);
          }
          EEPROM.commit();
          String s = "<h1>Wi-Fi and TESS settings was reset.</h1><p>Please reset device.</p>";
          webServer.send(200, "text/html", makePage("Reset Wi-Fi TESS Settings", s));
          Serial.println("Config Erased. ");
       } );

       
       webServer.on("/erase" , []() {                     //Pagina avisando para RESET all   
          String s = "<h2>TESS-W FULL RESET</h2>";
          s+= " All Setings will be lost !!!"; 
          s+= "<br> Are you sure? ";
          s+= "<br> You must know the necessary data to set up the photometer again.";
          s+= "<br> The sensor name must not be changed whithout consulting STARS4ALL. ";
          s+= "<p><a href=\"/reset\">RESET</a></p>";     
          s+= "<p><a href=\"/\">Return Main</a></p>"; 
          webServer.send(200, "text/html", makePage("Reset TESS Settings", s));
          Serial.println("Sending to Reset. ");
       } );


        webServer.on("/Const" , []() {                  // Pagina para introducir CI y enviarla
          String s = "<h3> TESS-W Settings </h3>";
          s += "Edit Filter type: 0= One chanel, 1 = RGB, 2 = Johnson VBR, 3 others";
          s += "<br>Actual Type:" + String( ConstI);
          s += "<form method=\"get\" action=\"setconst\" > ";         //---------->
          s += "<br>New Filters: <input name=\"cons\" length=8 type=\"text\"><br>";
          s += "<input type=\"submit\"></form>";
          s += "<p><a href=\"/\">Return Main</a></p>"; 
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
    
       } );       

       webServer.on("/setconst" , []() {       //Pagina para cambiar ZP
          cons = urlDecode(webServer.arg("cons"));    //----
          int Cons = cons.toInt();
           Ncanales = Cons;
          if (( Cons < 4) && ( Cons >= 0)) {
            if (Cons == 0) Ncanales = 0; //Un solo canal = TESS-W
            ConstI = Cons;
            for (int i = 64; i < 80; ++i) {
              EEPROM.write(i, 0);
            }
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(64 + i, cons[i]);
            }
            EEPROM.commit();                      
            Serial.println("Write EEPROM done!");
            String s = "<h3>New Filter Type "; 
            s += cons;
            s += "</h3>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          } 
          else { 
            Serial.print("Invalid Filter type. Use 0 for ONE Chanel, 1 for RGB, 2 for Johnson or 3 for others. ");
            String s = "<p>Invalid type. Use 0 for ONE Chanel, 1 for RGB, 2 for Johnson or 3 for others.</p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }    
       } );           


        webServer.on("/Offset" , []() {                  // Pagina para introducir ZP y enviarla
          String s = "<h3> TESS-w Settings </h3>";
          s += "Edit Calibration Offset.";
          s += "<br>Actual Offset, Hz: " + String( OffsetHz);
          s += "<form method=\"get\" action=\"setOffset\" > ";         //---------->
          s += "<br>New Offset, Hz: <input name=\"Offset\" length=8 type=\"text\"><br>";
          s += "<input type=\"submit\"></form>";
          s+= "<p><a href=\"/\">Return Main</a></p>"; 
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
    
       } );             
       webServer.on("/setOffset" , []() {       //Pagina para cambiar Offset
          cons = urlDecode(webServer.arg("Offset"));    //----
          float Cons = cons.toFloat();
          if (( Cons > -1) && ( Cons < 1)) {
            OffsetHz = Cons;
           // for (int i = 64; i < 80; ++i) {
            for (int i = 170; i < 186; ++i) {  // cons 16B
              EEPROM.write(i, 0);
            }
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(170 + i, cons[i]);
            }
            EEPROM.commit();                      
            Serial.println("Write EEPROM done!");
            String s = "<h3>New Offset Hz: "; 
            s += cons;
            s += "</h3>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          } 
          else { 
            Serial.print("Offset out of range ");
            String s = "<p>Offset out of range.</p>";
             s+= "<p><a href=\"/\">Return Main</a></p>"; 
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }    
       } );         


//---------------Edit ZP1, 2 3 4----------------------
      webServer.on("/EditZP" , []() {              //Pagina ZPs
          String s = "<h4> Present CI values: </h4>";
          s += "<br>CI 1: " + String(ZP1);
          if (Ncanales > 0) {
            s += "<br>CI 2: " + String(ZP2);
            s += "<br>CI 3: " + String(ZP3);
            s += "<br>CI 4: " + String(ZP4);
          }
          s += "<br>Enter New values: ";
//          s += "<br>Min x y z:  " + String(m_min.x)+ " "+ String(m_min.y)+ " "+ String(m_min.z) ;                                
          s += "<form method=\"get\" action=\"SetZP\" > ";          //------------>         
          s += "<br>CI1: <input name=\"nZP1\" length=5 type=\"text\"><br>";
          if (Ncanales > 0) {
            s += "<br>CI2: <input name=\"nZP2\" length=5 type=\"text\"><br>";
            s += "<br>CI3: <input name=\"nZP3\" length=5 type=\"text\"><br>";
            s += "<br>CI4: <input name=\"nZP4\" length=5 type=\"text\"><br>";
          }
//          s += "<br>Y min: <input name=\"nMinY\" length=5 type=\"text\"><br>";
  //        s += "<br>Z min: <input name=\"nMinZ\" length=5 type=\"text\"><br>";
          s += "<br>";
          s += "<input type=\"submit\"></form>";
           s+= "<p><a href=\"/\">Return Main</a></p>"; 
          webServer.send(200, "text/html", makePage("Magnet Settings", s));
    
       } );       
  
      webServer.on("/SetZP" , []() {               //Pagina cuando se han cambiado ZPs
         String s = "<h4> 4 chanel constant "; 
          cons = urlDecode(webServer.arg("nZP1"));    //----                              
          float x = cons.toFloat();
          if (x > 0) { //evita envió de 0s
            ZP1 = x;
            Serial.print(" CI1: ");   Serial.print(ZP1);     
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(134 + i, cons[i]);
            }                         
            if (Ncanales > 0) {
              cons = urlDecode(webServer.arg("nZP2"));    //----                    
              ZP2 = cons.toFloat();
              Serial.print(" CI2: ");   Serial.print(ZP2);
              for (int i = 0; i < cons.length(); ++i) {
                EEPROM.write(140 + i, cons[i]);
              }                                   
              cons = urlDecode(webServer.arg("nZP3"));    //----                    
              ZP3 = cons.toFloat();
              Serial.print(" CI3: ");    Serial.println(ZP3);
              for (int i = 0; i < cons.length(); ++i) {
                EEPROM.write(146 + i, cons[i]);
              }                                   
              cons = urlDecode(webServer.arg("nZP4"));    //----                    
              ZP4 = cons.toFloat();
              Serial.print(" CI4: "); Serial.println(ZP4);          
              for (int i = 0; i < cons.length(); ++i) {
                EEPROM.write(152 + i, cons[i]);
              }                                
            }   
              
            EEPROM.commit();                                
            Serial.println("Write EEPROM done!");
                                         
            s += " Constans OK. ";                                            
            s += "</h4>";
            s+= "<p><a href=\"/\">Return Main</a></p>";     

            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }
          else {            
             s = "<h4> CI invalid values.</h4>";                                       
            
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Return Main</a></p>";                 
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }            
       } );     
//-------------------------------------------------------

    }  // Fin de Hay Wifi
    else {                                             //NO hay Wifi pero SI Config
        Serial.print("Starting apdata at ");
        Serial.println(WiFi.softAPIP());

       webServer.on("/", []() {
         String s = "<META HTTP-EQUIV=\"Refresh\" Content= \"4\" >";
          s+= " <h2>STARS4ALL";
 //         s+= "<br>TESS-W AP Mode</h2><h4>";
          s+= "<br>TESS-W AP Mode Sensor: " + tname;
          s+= "<br></h2>";

          if (SDcardOK) {
             s+= "SD card & RTC detected.<br>";
             s+= String(datestring); 
             s+= "<br>  File size " + String(tamano) + " Bytes" ;    
          }   

          if (HaySHT21) {
            s+= " HR :  " + String(humd21) + " %";
            s+= "<br> T amb. : " + String( temp21) + "ºC";                     
          }   
          else if (BME280ok) {
            s+= " HR :  " + String(humedad) + "%";
            s+= "<br> T amb. : " + String( tempBME ) + "&ordm;C";                     
            s+= "<br> Press. : " + String( pressure) + "Hp"; 
            s+= "<br> Wind : " + String( AnalogA0/11.1) + "Km/h";                    
          }
          if (HayRTC) {
             s+= "<br>RTC:  ";
             s+= String(datestring); 
          }          
          if (MLXok) {
          s+= "<br><br> T. IR :   " + String(TObjeto) + " &ordm;C";      
          s+= "<br> T. Sens:   " + String(TAmbiente) + " &ordm;C";                 
          }
          
          if ( ConstI == 1 )
            s+= "<br> RGB filters.";
          else if ( ConstI == 2 )
            s+= "<br> Johnson filters.";
          
          s+= "<br> Mag. :  " + String(Magnitud) + " mv/as2";
          s+= " f : " + String( Frecuencia) + " Hz";   
          if(Ncanales > 0){                  
            s+= "<br> Mag.2 :  " + String(MagnitudG) + " mv/as2";
            s+= " f2 : " + String( frecG) + " Hz";                     
            s+= "<br> Mag.3 :  " + String(MagnitudR) + " mv/as2";
            s+= " f3 : " + String( frecR) + " Hz";                   
            s+= "<br> Mag.4 :  " + String(MagnitudB) + " mv/as2";
            s+= " f4 : " + String( frecB) + " Hz";                     
          }
          
          s+= "<br></h4>";
//          s+= "<br> Wifi :   " + String(rssi) + " dBm";
  //        s+= "<br>mqtt sec.: " + String(Numero)  + "</h3>";
          s+= "<p><a href=\"/config\">Show Settings</a></p>";
     
           webServer.send(200, "text/html", makePage("AP mode", s));  
       });  

        webServer.on("/config" , []() {
          String b = broker;
          b.replace(".", ",");
          String s = "<h2>TESS-W Settings.</h2>"; 
          s+= " <p><a href=\"/erase\">Reset ALL Settings</a></p>";       // a RESET
          s+= "<br>Compiled: " + String(__DATE__); 
          s+= "<br> Firmware v: " + String(vernum);                      
          s+= "<br> MAC: " + macID;                     
          s+= "<h4> SSID: " + ssid;  
          s+= "<br> Tel.Port: " + Puerto;
          s+= "<br>Offset Hz: " + String( OffsetHz);          
          if (ConstI == 0)
            s+= "<br> Filters: single chanel."; 
          else if ( ConstI == 1 )
            s+= "<br> Filters: 4  RGB."; 
          else if ( ConstI == 2 )
            s+= "<br> Filters: 4 Johnson."; 
          else   
            s+= "<br> Filters: 4 Custom."; 
          
          s += "<br> CI 1: " + String(ZP1);
          if(Ncanales > 0){                            
            s += "<br> CI 2: " + String(ZP2);
            s += "<br> CI 3: " + String(ZP3);
            s += "<br> CI 4: " + String(ZP4);
          }
           
          s+="<br><br> MQTT  "; 
          s+= "<br> Name : " + tname; 
          s+= "<br> Topic : " + rdtopic ;
          s+= "<br> Broker : " + String( mqtb);        
          s+= "<br> Period: " + Envio + "sec.</h4>";
          
          s+= "<p><a href=\"/EditZP\">Edit CI.</a></p>";                 // a edit CI
          s+= "<p><a href=\"/Offset\">Edit Offset Hz.</a></p>";             // a edit Offset
          s+= "<p><a href=\"/Const\">Edit Filter Type.</a></p>";            // a edit Filtro
          if (HayRTC) {
            s+= "<p><a href=\"/EditRTC\">Edit DATE TIME.</a></p>";  
          } 
          s+= "<p><a href=\"/settings\">Edit wifi.</a></p>";                // a edit SSID   
          s+= "<p><a href=\"/\">Return Main</a></p>";                         
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
       } );


      webServer.on("/erase" , []() {                 
          String s = "<h2>TESS-W FULL RESET</h2>";
//          s+= " All Setings will be deleted !!!"; 
  //        s+= " Are you sure?";

          s+= " All Setings will be lost !!!"; 
          s+= "<br> Are you sure? ";
          s+= "<br> You must know the necessary data to set up the photometer again.";
          s+= "<br> The sensor name must not be changed whithout consulting STARS4ALL. ";

  
          s+= "<p><a href=\"/reset\">RESET</a></p>";
//        s += "<br> <input type=\"button\" onclick=\"alert()\">";     
          
          s+= "<p><a href=\"/\">Return Main</a></p>";
          
          webServer.send(200, "text/html", makePage("Reset TESS Settings", s));
          Serial.println("Sending to Reset Page. ");
       } );   


       webServer.on("/reset" , []() {      
          for (int i = 0; i < EepromBuf; ++i) {
             EEPROM.write(i, 0);
          }
          EEPROM.commit();
          String s = "<h1>Wi-Fi and TESS settings was reset.</h1><p>Please reset device.</p>";
//          webServer.send(200, "text/html", makePage("Reset Wi-Fi TESS Settings", s));
          Serial.println("Config Erased. ");
       } );
               
      webServer.on("/setconst" , []() {      
          cons = urlDecode(webServer.arg("cons"));    //----
          float Cons = cons.toFloat();
          if (( Cons >= 0) && ( Cons < 4)) {
            ConstI = Cons;
            Ncanales = ConstI;
            for (int i = 64; i < 80; ++i) {
              EEPROM.write(i, 0);
            }
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(64 + i, cons[i]);
            }
            EEPROM.commit();                      
            Serial.println("Write EEPROM done!");
            String s = "<h3>New Filter Type "; 
            s += cons;
            s += "</h3>";
//            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Retunr Main</a></p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          } 
          else { 
            Serial.print("Invalid Filter type. Use 1 for RGB, 2 for Johnson, 3 others.");
            String s = "<p>Invalid Filter Type.</p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }    
       } );           

      webServer.on("/Const" , []() {      
          String s = "<h3> TESS-W Settings </h3>";
          s += "Edit Filter Type.";
          s+= "<br>Actual Type: " + String( ConstI);  
          s+=  "<br>  (0: ONE chanel, 1: RGB, 2: Johnson, 3: others) "; 
          s += "<form method=\"get\" action=\"setconst\" > ";
          s += "<br>New Type: <input name=\"cons\" length=8 type=\"text\"><br>";
          s += "<input type=\"submit\"></form>";
          s+= "<p><a href=\"/\">Return Main</a></p>"; 
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
    
       } );       
      
      webServer.on("/settings", []() {
          String s = "<h2>TESS-W STARS4ALL<br>";
          s += "Wifi Settings</h2><p>Please enter your password by selecting the SSID.</p>";
          s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
          s += ssidList;
          s += "</select> <br>Password: <input name=\"pass\" length=64 type=\"password\"><br>";
          s += "<input type=\"submit\"></form>";      
          webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
      });


        webServer.on("/Offset" , []() {                  // Pagina para introducir Offset Hz enviarla
          String s = "<h3> TESS-W Settings </h3>";
          s += "Edit Calibration Offset.";
          s += "<br>Actual Offset, Hz: " + String( OffsetHz);
          s += "<form method=\"get\" action=\"setOffset\" > ";         //---------->
          s += "<br>New Offset: <input name=\"Offset\" length=8 type=\"text\"><br>";
          s += "<input type=\"submit\"></form>";
           s+= "<p><a href=\"/\">Return Main</a></p>"; 
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
    
       } );             
       webServer.on("/setOffset" , []() {       //Pagina para cambiar Offset
          cons = urlDecode(webServer.arg("Offset"));    //----
          float Cons = cons.toFloat();
          if (( Cons > -1) && ( Cons < 1)) {
            OffsetHz = Cons;
           // for (int i = 64; i < 80; ++i) {
            for (int i = 170; i < 186; ++i) {  // cons 16B
              EEPROM.write(i, 0);
            }
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(170 + i, cons[i]);
            }
            EEPROM.commit();                      
            Serial.println("Write EEPROM done!");
            String s = "<h3>New Offset Hz: "; 
            s += cons;
            s += "</h3>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          } 
          else { 
            Serial.print(Cons);
            Serial.print(" Offset out of range ");
            
            String s = "<p>Offset out of range</p>";
            s+= "<p><a href=\"/\">Return Main</a></p>"; 
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }    
       } );                 

      
    
      webServer.on("/setap", []() {
          ssid = urlDecode(webServer.arg("ssid"));
          Serial.print("SSID: ");
          Serial.println(ssid);
          pass = urlDecode(webServer.arg("pass"));
          Serial.print("Password: ");
          Serial.println(pass);

          if ( ssid.length() > 2) {  //
            EEPROM.begin(512); 
            for (int i = 0; i <48; ++i) {
              EEPROM.write(i, 0);
            }
            EEPROM.commit();        
            for (int i = 200; i <232; ++i) { //el ssid paso a la 200
              EEPROM.write(i, 0);
            }
            EEPROM.commit();        
            Serial.println("Writing new SSID to EEPROM...");
            for (int i = 0; i < ssid.length(); ++i) {
             // EEPROM.write(i, ssid[i]);
              EEPROM.write(200 + i, ssid[i]);
            }
            EEPROM.commit();
            Serial.println("Writing new Password to EEPROM...");
            for (int i = 0; i < pass.length(); ++i) {
              EEPROM.write(16 + i, pass[i]);
            }                 
            EEPROM.commit();
          
            Serial.println("Write EEPROM done!");
            String s = "<h1>Setup complete.</h1><p>device will be connected to \""; 
            s += ssid;
            s += "\" Restarting NOW.</p>";
           
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
            ESP.restart();
          }
          else {   //no se guarda wifi 
            Serial.println("SSID too short!");
            String s = "<h1>SSID name invalid.</h1><p>SSID name must use at least 3 characters \""; 
            s += ssid;
            s += "\" Try a longer name.</p>";           
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));            
          }          
      });
      
    }  //FIN de NO hay Wifi pero SI Config 



//---------------Edit ZP1, 2 3 4----------------------
      webServer.on("/EditZP" , []() {              //Pagina ZPs
          String s = "<h4> Present CI values:. </h4>";
          s += "CI 1: " + String(ZP1)  + "<br>";
          if (Ncanales > 0) {          
            s += "CI 2: " + String(ZP2) + "<br>";
            s += "CI 3: " + String(ZP3) + "<br>";
            s += "CI 4: " + String(ZP4) + "<br>";
          }          
          s += "<br>Enter New values: ";
//          s += "<br>Min x y z:  " + String(m_min.x)+ " "+ String(m_min.y)+ " "+ String(m_min.z) ;                                
          s += "<form method=\"get\" action=\"SetZP\" > ";          //------------>         
          s += "<br>CI 1: <input name=\"nZP1\" length=5 type=\"text\"><br>";
          if (Ncanales > 0) {          
            s += "<br>CI 2: <input name=\"nZP2\" length=5 type=\"text\"><br>";
            s += "<br>CI 3: <input name=\"nZP3\" length=5 type=\"text\"><br>";
            s += "<br>CI 4: <input name=\"nZP4\" length=5 type=\"text\"><br>";
          }
//          s += "<br>Y min: <input name=\"nMinY\" length=5 type=\"text\"><br>";
  //        s += "<br>Z min: <input name=\"nMinZ\" length=5 type=\"text\"><br>";
          s += "<br>";
          s += "<input type=\"submit\"></form>";
          
           s+= "<p><a href=\"/\">Return Main</a></p>"; 
           
          webServer.send(200, "text/html", makePage("Magnet Settings", s));
    
       } );       
  
      webServer.on("/SetZP" , []() {               //Pagina cuando se han cambiado ZPs
         String s = "<h4> CI 4 chanels: "; 
          cons = urlDecode(webServer.arg("nZP1"));    //----                              
          float x = cons.toFloat();
          if (x > 0) { //evita envió de 0s
            ZP1 = x;
            Serial.print(" CI1: ");   Serial.print(ZP1);     
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(134 + i, cons[i]);
            }                   
            if (Ncanales > 0) {      
              cons = urlDecode(webServer.arg("nZP2"));    //----                    
              ZP2 = cons.toFloat();
              Serial.print(" CI2: ");   Serial.print(ZP2);
              for (int i = 0; i < cons.length(); ++i) {
                EEPROM.write(140 + i, cons[i]);
              }                                   
              cons = urlDecode(webServer.arg("nZP3"));    //----                    
              ZP3 = cons.toFloat();
              Serial.print(" CI3: ");    Serial.println(ZP3);
              for (int i = 0; i < cons.length(); ++i) {
                EEPROM.write(146 + i, cons[i]);
              }                                   
              cons = urlDecode(webServer.arg("nZP4"));    //----                    
              ZP4 = cons.toFloat();
              Serial.print(" CI4: "); Serial.println(ZP4);          
              for (int i = 0; i < cons.length(); ++i) {
                EEPROM.write(152 + i, cons[i]);
              }                                   
            }
            
            EEPROM.commit();                                
            Serial.println("Write EEPROM done!");
                                         
            s += " Constans OK. ";                                            
            s += "</h4>";
            s+= "<p><a href=\"/\">Return Main</a></p>";     

            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }
          else {
             s = "<h4> CI invalid values.</h4>"; 
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Return Main</a></p>";                 
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }            
       } );     
//-------------------------------------------------------

    webServer.on("/EditRTC" , []() {              //Pagina para cambiar RTC 
          String s = "<h3> TESS-W RTC </h3>";
          s += "<br>Actual DT: " + String( datestring);          
          s += "<form method=\"get\" action=\"setRTC\" > ";          //------------>         

          s += "<br>Hour: <input name=\"nHora\" length=5 type=\"text\"><br>";
          s += "<br>Min: <input name=\"nMinuto\" length=5 type=\"text\"><br>";
          s += "<br>Sec: <input name=\"nSegundo\" length=5 type=\"text\"><br>";
          s += "<br>Year: <input name=\"nYear\" length=5 type=\"text\"><br>";
          s += "<br>Month: <input name=\"nMes\" length=5 type=\"text\"><br>";
          s += "<br>Day: <input name=\"nDia\" length=5 type=\"text\"><br>";
          s += "<br>";
          s += "<input type=\"submit\"></form>";
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));    
       } );       


  
    webServer.on("/setRTC" , []() {               //Pagina cuando se ha cambiado RTC
          cons = urlDecode(webServer.arg("nHora"));    //----                    
          nHo = cons.toInt();
          Serial.print(" Hr: ");
          Serial.print(nHo);          
          cons = urlDecode(webServer.arg("nMinuto"));    //----                    
          nMi = cons.toInt();
          Serial.print(" Mn: ");
          Serial.print(nMi);
          cons = urlDecode(webServer.arg("nSegundo"));    //----                    
          nSe = cons.toInt();
          Serial.print(" Sg: ");
          Serial.println(nSe);
          cons = urlDecode(webServer.arg("nYear"));    //----                    
          nAn = cons.toInt();
          Serial.print("      Ano: ");
          Serial.print(nAn);          
          cons = urlDecode(webServer.arg("nMes"));    //----                    
          nMe = cons.toInt();
          Serial.print(" Month: ");
          Serial.print(nMe);
          cons = urlDecode(webServer.arg("nDia"));    //----                    
          nDi = cons.toInt();
          Serial.print(" Day: ");
          Serial.println(nDi);
                    
          String s = "<h4>New DT: ";                                   
          if (HayRTC) {
                // Set date
            dt.tm_mday = (uint8_t)nDi;
            dt.tm_mon = (uint8_t)nMe -1; //resta 1 porque enero = 0
            dt.tm_year = (uint16_t)nAn - 1900;
            // Calculate day of the week
            t = mktime(&dt);
            dt.tm_wday = localtime(&t)->tm_wday;
            dt.tm_hour = (uint8_t)nHo;
            dt.tm_min = (uint8_t)nMi;
            dt.tm_sec = (uint8_t)nSe;
                // Set new date time
            Serial.print(F("Set RTC date time..."));
            if (!rtc.write(&dt)) {
                Serial.println(F("FAILED"));
            } else {
              Serial.println(F("OK"));
            }
                // Print build date/time
            Serial.print(F("Build date time: "));
            Serial.println(asctime(&dt));

          Serial.print(datestring);                      
              s+= "New" + String(datestring); 
              
            }           
            s += "</h4>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Return Main</a></p>";     
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
       } );     
    
    
  }
  webServer.begin();
}
