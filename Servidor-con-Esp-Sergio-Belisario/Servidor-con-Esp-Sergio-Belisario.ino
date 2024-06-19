#include <WiFi.h>
#include <max6675.h>

//red wifi nombre y contraseña
const char* ssid = "nombre_red";
const char* password = "aqui_la_clave";  // aqui escribis la clave para hacer la conexion 

//numero de puerto 
WiFiServer server(80);

//almacen de http si es on o off
String header;

//estados de salida
String output26State = "off";
String output27State = "off";
String output12State = "off";
String output13State = "off";

// asignar variables de salidas de rele
const int output12 = 12;
const int output13 = 13;
const int output26 = 26;
const int output27 = 27;

// tiempo transcurrido
unsigned long currentTime = millis();
// tiempo anterior 
unsigned long previousTime = 0;
// define el tiempo milisegundos (ejemplo: 2000ms = 2s)
const long timeoutTime = 2000;

//asignar variables del max6675

int8_t espPin25 = 25;
int8_t espPin33 = 33;
int8_t espPin32 = 32;
MAX6675 sensorMAX6675(espPin25, espPin33, espPin32);

void setup() {
  Serial.begin(115200);
  Serial.println("Ing sergio belisarion temperatura MAX6675");
  
  // iniciar las salidas
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  pinMode(output12, OUTPUT);
  pinMode(output13, OUTPUT);
  // setear salidas en low "bajo"
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);
  digitalWrite(output12, LOW);
  digitalWrite(output13, LOW);
  // conectando al wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // imprimiendo la IP local
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  //mostrar la temperatura 
  double tempC = sensorMAX6675.readCelsius();
  String tempStr = String(tempC);
  delay(1000);


  WiFiClient client = server.available();  

  if (client) {                            
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");         
    String currentLine = "";               
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();            
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          
          if (currentLine.length() == 0) {
          
          
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

           
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            } else if (header.indexOf("GET /12/on") >= 0) {
              Serial.println("GPIO 12 on");
              output12State = "on";
              digitalWrite(output12, HIGH);
            } else if (header.indexOf("GET /12/off") >= 0) {
              Serial.println("GPIO 12 off");
              output12State = "off";
              digitalWrite(output12, LOW);
            } else if (header.indexOf("GET /13/on") >= 0) {
              Serial.println("GPIO 13 on");
              output13State = "on";
              digitalWrite(output13, HIGH);
            } else if (header.indexOf("GET /13/off") >= 0) {
              Serial.println("GPIO 13 off");
              output13State = "off";
              digitalWrite(output13, LOW);
            }

            // pantala html pagina web
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
           

            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // encabezado de pagina
            client.println("<body><h1>ESP32 INGENIERO SERGIO BELISARIO</h1>");

            client.println("<h1>La temperatura es :" + tempStr + " </h1>");

          
            client.println("<p>GPIO 12 - State " + output12State + "</p>");
         
            if (output12State == "off") {
              client.println("<p><a href=\"/12/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
          
            client.println("<p>GPIO 13 - State " + output13State + "</p>");
         
            if (output13State == "off") {
              client.println("<p><a href=\"/13/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/13/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

          
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
         
            if (output26State == "off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

           
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
           
            if (output27State == "off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
           
            client.println();
          
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
  
    header = "";
   
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
