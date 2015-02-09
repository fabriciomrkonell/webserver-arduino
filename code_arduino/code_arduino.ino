 #include <SPI.h>  
 #include <Ethernet.h>  
 #include <SD.h>
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  
IPAddress ip(192,168,0,2);
EthernetServer server(80);
File webFile;  
int imprimir = 1;
int toogle = 1;
 
 void setup() {  
   Ethernet.begin(mac, ip);   
   server.begin();        
   Serial.begin(9600);
   if (!SD.begin(4)) {  
     Serial.println("Erro ao iniciar o cartão...");  
     return;    
   }  
   
   // Relés
   pinMode(2, OUTPUT);   
 }  
 
 void loop() {
   EthernetClient client = server.available();
   if (client) {   
     toogle = 1;
     boolean currentLineIsBlank = true;  
     while (client.connected()) {  
       if (client.available()) {    
         char c = client.read();            
         if(c == '?'){           
           if(toogle == 1){
             toogle = 0;          
             char pinRele = client.read();
             char statusRele = client.read();           
             String _statusRele = String(statusRele);
             String _pinRele = String(pinRele);
             digitalWrite(_pinRele.toInt(), _statusRele.toInt());
           }
         }
         if (c == '\n' && currentLineIsBlank) {  
           client.println("HTTP/1.1 200 OK");  
           client.println("Content-Type: text/html");  
           client.println("Connection: close");  
           client.println();  
           webFile = SD.open("index.htm");      
           if (webFile) {  
             while(webFile.available()) {               
               char var = webFile.read();
               if(var == '['){
                 char pinRele = webFile.read();
                 char statusRele = webFile.read();
                 String _statusRele = String(statusRele);
                 String _pinRele = String(pinRele);
                 if(digitalRead(_pinRele.toInt()) ==_statusRele.toInt()){                  
                   imprimir = 1;
                 }else{
                   imprimir = 0;
                 }
               }
               if(imprimir == 1){
                 if(var != '[' && var != ']'){
                   client.write(var);   
                 }
               }
               if(var == ']'){
                imprimir = 1;
               }               
             }  
             webFile.close();  
           }  
           break;  
         }  
         if (c == '\n') {  
           currentLineIsBlank = true;  
         }   
         else if (c != '\r') {  
           currentLineIsBlank = false;  
         }  
       }   
     }   
     delay(1);     
     client.stop();   
   }   
 }  
