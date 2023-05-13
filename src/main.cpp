#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <iostream>
#include <string>



TFT_eSPI tft = TFT_eSPI();
int count = 0;

String stringCount = "not initialized";

// webServer
const char* ssid = "lucmx2";
const char* password = "l30nc1t4";
WiFiServer server(80);

//http response
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;



void SetupWifi(){
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();


  Serial.println("RRSI: ");
  Serial.println(WiFi.RSSI());
}


void setup() {
  
  Serial.begin(9600);
  SetupWifi();

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Hello World!", 30, 30, 2); 

  //String ip = WiFi.localIP().toString();
  tft.drawString(WiFi.localIP().toString(), 30, 150, 2);

}

std::string toString(int number) {
  std::string strNumber = std::to_string(number); 
  return strNumber;  
}

void printCounter(){
  
  count++;

  stringCount = toString(count).c_str();

  char const *charArray = toString(count).c_str();

  tft.drawString(charArray, 50,50, 2);   

  String log = "Contador incrementado: " + stringCount;
  Serial.println(log); 
}

void printString(String texto){

  tft.drawString(texto, 50,50,2);
  Serial.println(texto);
}


void wifiMonitor(){

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            

            // exec command if received
            if (header.indexOf("GET /count/increment") >= 0) {
              printCounter();
            }

            if (header.indexOf("GET /text/spec/") >= 0){

              int p_init = header.indexOf("GET /text/spec/") + 15;
              int p_fin = header.indexOf ("HTTP/1.1");

              String texto = header.substring(p_init, p_fin);
              printString(texto);
            }
            
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"; charset=utf-8>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current count
            client.println("<p>El contador actual es: " + stringCount + "</p>");
            
            
            client.println("<p><a href=\"/count/increment\"><button class=\"button\">Incrementar</button></a></p>");           
               
            client.println("<p>Para solicitar imprimir otro texto, envía comando GET /text/spec/{{texto a imprimir}}");
           
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");

  }
}

void loop(){
  // put your main code here, to run repeatedly:

  //printCounter();

  wifiMonitor();

  // std::string str = "Se imprimió contador "+ toString(count);
  // Serial.println(str.c_str());
  delay(5000);  
}




