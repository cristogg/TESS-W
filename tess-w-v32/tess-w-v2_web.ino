
//----------------------------------------------------------------------------------
void startWebServer() {
 
 
  if (settingMode) {            //No hay configuracion.
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    
    webServer.on("/settings", []() {
     String s = "<h2>TESS-W STARS4ALL<br>";
      s += "Wi-Fi & TESS Settings</h2><p>Please enter your password by selecting the SSID.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select> <br>Password: <input name=\"pass\" length=64 type=\"password\"><br>";
      s += "Name: <input name=\"tname\" length=32 type=\"text\"><br>";
      s += "ZP: <input name=\"cons\" length=8 type=\"text\"><br>";
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
      if (ssid.length() < 4) ssid = "unknow";
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
          s+= " <h2>STARS4ALL";
          s+= "<br>TESS-W Data</h2>";

          if (SDcardOK) {
             s+= "SD card detected ";             
             s+= "<br>File size " + String(tamano) + " Bytes" ;    
          } 
          if (HayRTC) {
            ahorita = Rtc.GetDateTime();
            printDateTime(ahorita);          
            //Serial.print(datestring);            
             s+= "<br>RTC:  ";
             s+= String(datestring); 
          } 
          if (HaySHT21) {
            s+= "<h3> HR :  " + String(humd21) + "%";
            s+= "<br> T amb. : " + String( temp21) + "&ordm;C";                     
          }   
          else if (BME280ok) {
            s+= "<h3> HR :  " + String(humedad) + "%";
            s+= "<br> T amb. : " + String( tempBME ) + "&ordm;C";  
            s+= "<br> Press. : " + String( pressure) + "Hp"; 
            s+= "<br> Wind : " + String( AnalogA0/11.1) + "Km/h";                               
          }
                    
          s+= "<h3> Mag.V :  " + String(Magnitud) + " mv/as2";
          s+= "<br> Frec. : " + String( Frecuencia) + " Hz";                     
          s+= "<br> T. IR :   " + String(TObjeto) + " &ordm;C";      
          s+= "<br> T. Sens:   " + String(TAmbiente) + " &ordm;C";      
          s+= "<br>";
          s+= "<br> Wifi :   " + String(rssi) + " dBm";
          s+= "<br>mqtt sec.: " + String(Numero)  + "</h3>";
          s+= "<p><a href=\"/config\">Show Settings</a></p>";     //----------->
     
           webServer.send(200, "text/html", makePage("STA mode", s));  
       });  
     
       webServer.on("/config" , []() {                   //Pagina muestra Configuracion
         String b = broker;
         b.replace(".", ",");
         String s = "<h2>TESS-W Settings.</h2>";   
         s+= "<a href=\"/erase\">Erase All Settings</a>";         
         s+= "<h4>Compiled: " + String(__DATE__) + vernum;           
         s+= "<br> MAC: " + macID;  
         s+= "<br> SSID: " + ssid;  
         s+= "<br> ZP: " + String( ConstI);
         s+= "<br> Tel.Port: " + Puerto; 
         s+= "<br> MQTT  "; 
         s+= "<br> Name : " + tname; 
         s+= "<br> Topic : " + rdtopic ;
         s+= "<br> Broker : " + String( mqtb);        
         s+= "<br> Period: " + Envio + "sec.</h4>";          
         s+= "<p><a href=\"/Const\">Edit ZP.</a></p>";  
         if (HayRTC) {
            s+= "<p><a href=\"/EditRTC\">Edit DATE TIME.</a></p>";  
         } 
         if (HayManetometro) {
            s+= "<p><a href=\"/EditMag\">Calibrate Magnetometer.</a></p>";  
         } 
         
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
          s+= " All Setings will be deleted !!!"; 
          s+= "<p><a href=\"/reset\">RESET</a></p>";      
          webServer.send(200, "text/html", makePage("Reset TESS Settings", s));
          Serial.println("Sending to Reset. ");
       } );


        webServer.on("/Const" , []() {                  // Pagina para introducir ZP y enviarla
          String s = "<h3> TESS-w Settings </h3>";
          s += "Edit Zero Point";
          s += "<br>Actual ZP:" + String( ConstI);
          s += "<form method=\"get\" action=\"setconst\" > ";         //---------->
          s += "<br>New I.C.: <input name=\"cons\" length=8 type=\"text\"><br>";
          s += "<input type=\"submit\"></form>";
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
    
       } );       

       webServer.on("/setconst" , []() {       //Pagina para cambiar ZP
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
            String s = "<h3>New ZP "; 
            s += cons;
            s += "</h3>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          } 
          else { 
            Serial.print("ZP out of range ");
            String s = "<p>ZP out of range</p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }    
       } );           

/*
      webServer.on("/EditMag" , []() {              //Pagina Magnetometro 
          String s = "<h4> Magnetometer constans. </h4>";
          s += "Max X Y Z: " + String(m_max.x)+ " " + String(m_max.y)+ " " + String(m_max.z);
          s += "<br>Min x y z:  " + String(m_min.x)+ " "+ String(m_min.y)+ " "+ String(m_min.z) ;                                
          s += "<form method=\"get\" action=\"SetMagnet\" > ";          //------------>         
          s += "<br>X max: <input name=\"nMaxX\" length=5 type=\"text\"><br>";
          s += "<br>Y max: <input name=\"nMaxY\" length=5 type=\"text\"><br>";
          s += "<br>Z max: <input name=\"nMaxZ\" length=5 type=\"text\"><br>";
          s += "<br>X min: <input name=\"nMinX\" length=5 type=\"text\"><br>";
          s += "<br>Y min: <input name=\"nMinY\" length=5 type=\"text\"><br>";
          s += "<br>Z min: <input name=\"nMinZ\" length=5 type=\"text\"><br>";
          s += "<br>";
          s += "<input type=\"submit\"></form>";
          webServer.send(200, "text/html", makePage("Magnet Settings", s));
    
       } );       
  
      webServer.on("/SetMagnet" , []() {               //Pagina cuando se ha cambiado Magnetome
         String s = "<h4> Magnetometer "; 
          cons = urlDecode(webServer.arg("nMaxX"));    //----                              
          int x = cons.toInt();
          if (x > 0) { //evita envió de 0s
            m_max.x = x;
            Serial.print(" +X: ");   Serial.print(m_max.x);     
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(134 + i, cons[i]);
            }                         
            cons = urlDecode(webServer.arg("nMaxY"));    //----                    
            m_max.y = cons.toInt();
            Serial.print(" +Y: ");   Serial.print(m_max.y);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(140 + i, cons[i]);
            }                                   
            cons = urlDecode(webServer.arg("nMaxZ"));    //----                    
            m_max.z = cons.toInt();
            Serial.print(" +Z: ");    Serial.println(m_max.z);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(146 + i, cons[i]);
            }                                   
            cons = urlDecode(webServer.arg("nMinX"));    //----                    
            m_min.x = cons.toInt();
            Serial.print("   ´-X: "); Serial.print(m_min.x);          
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(152 + i, cons[i]);
            }                                   
            cons = urlDecode(webServer.arg("nMinY"));    //----                    
            m_min.y = cons.toInt();
            Serial.print(" -Y: ");    Serial.print(m_min.y);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(158 + i, cons[i]);
            }                         
          
            cons = urlDecode(webServer.arg("nMinZ"));    //----                    
            m_min.z = cons.toInt();
            Serial.print(" -Z: ");    Serial.println(m_min.z);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(164 + i, cons[i]);                  
            }
            EEPROM.commit();                                
            Serial.println("Write EEPROM done!");
                                         
            s += " Constans OK. ";                                            
            s += "</h4>";
            s+= "<p><a href=\"/\">Return Main</a></p>";     

            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }
          else {
            s += " Invalid Constans. ";                                            
            s += "</h4>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Return Main</a></p>";                 
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }            
       } );     


      webServer.on("/EditRTC" , []() {              //Pagina para cambiar RTC 
          String s = "<h3> TESS-w RTC </h3>";
//          s += "<br>Format:" + String( datestring);          
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
              ahorita = Rtc.GetDateTime();
//RtcDateTime ahora2 = RtcDateTime(ahorita.Year(), ahorita.Month(), ahorita.Day(),ahorita.Hour(),ahorita.Minute(),ahorita.Second());
RtcDateTime ahora2 = RtcDateTime((uint16_t)nAn, (uint8_t)nMe, (uint8_t)nDi, (uint8_t)nHo, (uint8_t)nMi, (uint8_t)nSe);
              Rtc.SetDateTime(ahora2);                  
              printDateTime(ahora2);          
          Serial.print(datestring);                      
              s+= "New" + String(datestring); 
            }           
            s += "</h4>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Return Main</a></p>";     
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
       } );     
    */
    }
    else {                                             //NO hay Wifi pero SI Config
        Serial.print("Starting apdata at ");
        Serial.println(WiFi.softAPIP());

       webServer.on("/", []() {
         String s = "<META HTTP-EQUIV=\"Refresh\" Content= \"4\" >";
          s+= " <h2>STARS4ALL";
          s+= "<br>TESS-W AP Mode</h2>";

          if (SDcardOK) {
             s+= "SD card & RTC detected.<br>";
             s+= String(datestring); 
             s+= "<br>  File size " + String(tamano) + " Bytes" ;    
          }   

          if (HaySHT21) {
            s+= "<h3> HR :  " + String(humd21) + " %";
            s+= "<br> T amb. : " + String( temp21) + "ºC";                     
          }   
          else if (BME280ok) {
            s+= "<h3> HR :  " + String(humedad) + "%";
            s+= "<br> T amb. : " + String( tempBME ) + "&ordm;C";                     
            s+= "<br> Press. : " + String( pressure) + "Hp"; 
             s+= "<br> Wind : " + String( AnalogA0/11.1) + "Km/h"; 
                               
          }

          s+= "<h3> Mag.V :  " + String(Magnitud) + " mv/as2";
          s+= "<br> Frec. : " + String(Magnitud)  + " Hz";                     
          s+= "<br> T. IR :   " + String(TObjeto) + " &ordm;C";      
          s+= "<br> T. Sens:   " + String(TAmbiente) + " &ordm;C";      
          s+= "<br>";
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
          s+= "<br> Compiled: " + String(__DATE__) + vernum;                       
          s+= "<br> MAC: " + macID;                     
          s+= "<h4> SSID: " + ssid;  
          s+= "<br>ZP: " + String( ConstI);
          s+= "<br> Tel.Port: " + Puerto; 
          s+="<br><br> MQTT  "; 
          s+= "<br> Name : " + tname; 
          s+= "<br> Topic : " + rdtopic ;
          s+= "<br> Broker : " + String( mqtb);        
          s+= "<br> Period: " + Envio + "sec.</h4>";
              
         if (HayManetometro) {
            s+= "<p><a href=\"/EditMag\">Calibrate Magnetometer.</a></p>";  
         }                                
          s+= "<p><a href=\"/Const\">Edit Ins.Const.</a></p>";              // a edit ZP
          s+= "<p><a href=\"/settings\">Edit wifi.</a></p>";                // a edit SSID                            
          webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
       } );


      webServer.on("/erase" , []() {                 
          String s = "<h2>TESS-W FULL RESET</h2>";
          s+= " All Setings will be deleted !!!"; 
          s+= "<p><a href=\"/reset\">RESET</a></p>";
//        s += "<br> <input type=\"button\" onclick=\"alert()\">";     
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
            String s = "<h3>New Zero Point "; 
            s += cons;
            s += "</h3>";
//            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Retunr Main</a></p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          } 
          else { 
            Serial.print("ZP out of range ");
            String s = "<p>ZP out of range</p>";
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }    
       } );           

      webServer.on("/Const" , []() {      
          String s = "<h3> TESS-w Settings </h3>";
          s += "Edit Zero Point.";
          s += "<br>Actual ZP:" + String( ConstI);
          s += "<form method=\"get\" action=\"setconst\" > ";
          s += "<br>New ZP: <input name=\"cons\" length=8 type=\"text\"><br>";
          s += "<input type=\"submit\"></form>";
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
    
      webServer.on("/setap", []() {
          ssid = urlDecode(webServer.arg("ssid"));
          Serial.print("SSID: ");
          Serial.println(ssid);
          pass = urlDecode(webServer.arg("pass"));
          Serial.print("Password: ");
          Serial.println(pass);
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
      });
    }

    webServer.on("/EditMag" , []() {              //Pagina Magnetometro 
          String s = "<h4> Magnetometer constans. </h4>";
          s += "Max X Y Z: " + String(m_max.x)+ " " + String(m_max.y)+ " " + String(m_max.z);
          s += "<br>Min x y z:  " + String(m_min.x)+ " "+ String(m_min.y)+ " "+ String(m_min.z) ;                                
          s += "<form method=\"get\" action=\"SetMagnet\" > ";          //------------>         
          s += "<br>X max: <input name=\"nMaxX\" length=5 type=\"text\"><br>";
          s += "<br>Y max: <input name=\"nMaxY\" length=5 type=\"text\"><br>";
          s += "<br>Z max: <input name=\"nMaxZ\" length=5 type=\"text\"><br>";
          s += "<br>X min: <input name=\"nMinX\" length=5 type=\"text\"><br>";
          s += "<br>Y min: <input name=\"nMinY\" length=5 type=\"text\"><br>";
          s += "<br>Z min: <input name=\"nMinZ\" length=5 type=\"text\"><br>";
          s += "<br>";
          s += "<input type=\"submit\"></form>";
          webServer.send(200, "text/html", makePage("Magnet Settings", s));
    
       } );       
  
    webServer.on("/SetMagnet" , []() {               //Pagina cuando se ha cambiado Magnetome
         String s = "<h4> Magnetometer "; 
          cons = urlDecode(webServer.arg("nMaxX"));    //----                              
          int x = cons.toInt();
          if (x > 0) { //evita envió de 0s
            m_max.x = x;
            Serial.print(" +X: ");   Serial.print(m_max.x);     
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(134 + i, cons[i]);
            }                         
            cons = urlDecode(webServer.arg("nMaxY"));    //----                    
            m_max.y = cons.toInt();
            Serial.print(" +Y: ");   Serial.print(m_max.y);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(140 + i, cons[i]);
            }                                   
            cons = urlDecode(webServer.arg("nMaxZ"));    //----                    
            m_max.z = cons.toInt();
            Serial.print(" +Z: ");    Serial.println(m_max.z);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(146 + i, cons[i]);
            }                                   
            cons = urlDecode(webServer.arg("nMinX"));    //----                    
            m_min.x = cons.toInt();
            Serial.print("   ´-X: "); Serial.print(m_min.x);          
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(152 + i, cons[i]);
            }                                   
            cons = urlDecode(webServer.arg("nMinY"));    //----                    
            m_min.y = cons.toInt();
            Serial.print(" -Y: ");    Serial.print(m_min.y);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(158 + i, cons[i]);
            }                         
          
            cons = urlDecode(webServer.arg("nMinZ"));    //----                    
            m_min.z = cons.toInt();
            Serial.print(" -Z: ");    Serial.println(m_min.z);
            for (int i = 0; i < cons.length(); ++i) {
              EEPROM.write(164 + i, cons[i]);                  
            }
            EEPROM.commit();                                
            Serial.println("Write EEPROM done!");
                                         
            s += " Constans OK. ";                                            
            s += "</h4>";
            s+= "<p><a href=\"/\">Return Main</a></p>";     

            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }
          else {
            s += " Invalid Constans. ";                                            
            s += "</h4>";
            s+= "<p><a href=\"/config\">Show Settings</a></p>";
            s+= "<p><a href=\"/\">Return Main</a></p>";                 
            webServer.send(200, "text/html", makePage("Wi-Fi TESS Settings", s));
          }            
       } );     


    webServer.on("/EditRTC" , []() {              //Pagina para cambiar RTC 
          String s = "<h3> TESS-w RTC </h3>";
//          s += "<br>Format:" + String( datestring);          
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
              ahorita = Rtc.GetDateTime();
//RtcDateTime ahora2 = RtcDateTime(ahorita.Year(), ahorita.Month(), ahorita.Day(),ahorita.Hour(),ahorita.Minute(),ahorita.Second());
RtcDateTime ahora2 = RtcDateTime((uint16_t)nAn, (uint8_t)nMe, (uint8_t)nDi, (uint8_t)nHo, (uint8_t)nMi, (uint8_t)nSe);
              Rtc.SetDateTime(ahora2);                  
              printDateTime(ahora2);          
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

