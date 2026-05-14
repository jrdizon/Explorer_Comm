/*
 * Webserver
 * by Joseph Dizon rbdizon@yahoo.com
 *
 * with Adafruit_SSD1306
 * with GPIO
 * with servo
 * with ultrasonic
 *
 * Commands:
 *
 * Get Chip info  http://<ip address>/info
 * Set LED to 1   http://<ip address>/led_1
 * Set LED to 0   http://<ip address>/led_0
 * Flash LED      http://<ip address>/led_flash?n=<count>
 *
 * Print text     http://<ip address>/print?text=<text>&color=[0|1]&clear=[0|1]&size=[0|1]&line=<line number>
 * Set cursor     http://<ip address>/cursor?x=<pixels>&y=<pixels>
 * Draw Rectangle http://<ip address>/rect?x=<pixels>&y=<pixels>&h=<pixels>&w=<pixels>&color=[0|1]&fill=[0|1]
 *
 * Set GPIO mode  http://<ip address>/gpio?init=<pin number>&mode=[in|out]
 * Set GPIO value http://<ip address>/gpio?pin=<pin number>&val=[0|1]
 * Get GPIO value http://<ip address>/gpio?pin=<pin number>
 *
 * Set ADC pin    http://<ip address>/adc?init=<pin number>
 * Get ADC value  http://<ip address>/adc?pin=<pin number>
 *
 * Set servo pin      http://<ip address>/servo?init=<pin number>&init1=<pin>&...
 * Set servo degrees  http://<ip address>/servo?deg=<degrees>&deg1=<degress>&...
 *
 * Get ultrasonic distance http://<ip address>/ultrasonic
 *
*/

#include <WiFi.h>
//#include <NetworkClient.h>
#include <WebServer.h>
//#include <ESPmDNS.h>

//#include <SPI.h> //For SPI
#include <Wire.h> //For I2C
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <ESP32Servo.h>
#include <secret.h> //This contains ssid and password

//ESP32-C3
  #define ESP32C3_led 8
  #define ESP32C3_ip4 50
  #define ESP32C3_sda 7
  #define ESP32C3_scl 9

//ESP32
  #define ESP32_led 2
  #define ESP32_ip4 51
  //Custon pin for display
  #define ESP32_sda 21
  #define ESP32_scl 23

boolean isESP32C3;
int led;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define LINE_HEIGHT 8 //Character line is 8 pixels in height

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
//#define OLED_RESET     4 // FOR SPI. Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET     -1 // FOR I2C. Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //For I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define your static IP settings (replace with your actual network info!)
IPAddress local_IP(10, 0, 0, 50); // Choose an available IP in your network
IPAddress gateway(10,0, 0, 1);   // Your router's IP (often 192.168.1.1 or .0.1)
IPAddress subnet(255, 255, 255, 0);  // Your subnet mask (usually this)
IPAddress primaryDNS(8, 8, 8, 8);    // Google DNS (optional but recommended)
IPAddress secondaryDNS(8, 8, 4, 4);  // Google DNS (optional)

WebServer server(80);
String content_type = "text/plain";

String version = "WebServer 2026.1.3.0";

void setup(void) {
  isESP32C3 = String(ESP.getChipModel()) == "ESP32-C3";
  if (isESP32C3) {
    led = ESP32C3_led;
    IPAddress local_IP(10, 0, 0, ESP32C3_ip4); //Set statis IP address
    Wire.setPins(ESP32C3_sda, ESP32C3_scl); //Custon pin for display
  }
  else {
    led = ESP32_led;
    IPAddress local_IP(10, 0, 0, ESP32_ip4); //Set statis IP address
    Wire.setPins(ESP32_sda, ESP32_scl); //Custon pin for display
  }
  
  Serial.begin(115200);
  Serial.println(version);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.print(version); //display.print(F(version));
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  if (isESP32C3)
    display.println(F(" ESP32-C3"));
  else
    display.println(F(""));

  display.display();
  //delay(1000);

  String conStr;
  // --- KEY STEP: Configure static IP BEFORE connecting ---
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure"); // For ESP32/ESP8266
    printLine(4,"WiFi failed to configure");
    conStr = "Connect";
  }
  else {
    printLine(4,"WiFi static");
    conStr = "Static";
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (isESP32C3) {
    WiFi.setTxPower(WIFI_POWER_8_5dBm); // Reduces reflections [2, 8] -- for ESP32 C3 Supper Mini
    printLine(2,"ESP32-C3 Low wifi");
    Serial.println("ESP32-C3 Low wifi");
  }

  printLine(3,"Connecting...");
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(500);
    //Serial.print(".");
    display.print(".");
    display.display();
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //printLine(1,"Connected");
  //printLine(2,"IP:");
  //display.println(WiFi.localIP());
  //display.display();

  display.clearDisplay();
  display.setCursor(0,0);
  display.print(conStr + " IP: ");
  display.println(WiFi.localIP());
  display.display();

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  /*
    - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to http://esp32.local, you should see a response.
  */
  /*
  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  //server.begin();
  //Serial.println("TCP server started");

  // Add service to MDNS-SD
  //MDNS.addService("http", "tcp", 80);
  */

  setup_Server();
  setup_Display();
  setup_GPIO();
  setup_ADC();
  setup_servo();
  setup_ultrasonic();

  server.begin();
  Serial.println("HTTP server started");

  // Add service to MDNS-SD
  //MDNS.addService("http", "tcp", 80);
}

void loop(void) {
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}

int serverGetInt(String _Arg)
{
  if (server.hasArg(_Arg)) return server.arg(_Arg).toInt();
  else return -1;
}

void setup_Server()
{
  //Async
  /*server.on("/async", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasArg("x")) { // Check for the "value" argument
      String x = request->arg("x");
      String y = request->arg("y");
      String h = request->arg("h");
      String w = request->arg("w");

      int c;
      if (request->hasArg("color") && request->arg("color") == "0")
        c = SSD1306_BLACK;
      else
        c = SSD1306_WHITE;

      display.writeFillRect(x.toFloat(), y.toFloat(), w.toFloat(), h.toFloat(), c);
      display.display();
      request->send(200, content_type, "Async Rectangle");
    } else {
      request->send(400, content_type, "Invalid async request");
    }
  });*/

  /* Sample
  server.on("/get", HTTP_GET, [](){
    if (server.hasArg("input1")) { // Check if the "input1" argument exists
      String inputValue = server.arg("input1"); // Get the value of "input1"
      // Process the value (e.g., control a GPIO, save to a variable)
      server.send(200, content_type, "Value received: " + inputValue);
    } else {
      server.send(400, content_type, "Missing input1 argument");
    }
  });
  */

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.on("/info", HTTP_GET, [](){
    server.send(200, content_type, version + "\nChip Info: " + String(ESP.getChipModel()) + (isESP32C3 ? " (ESP32-C3)" : ""));
  });

  server.on("/reset", HTTP_GET, [](){
    server.send(200, content_type, version + "\nChip Info: " + String(ESP.getChipModel()) + (isESP32C3 ? " (ESP32-C3)" : "") + " -RESET");
    delay(200);
    ESP.restart();
  });

  server.on("/wifi-off", HTTP_GET, [](){
    server.send(200, content_type, "WiFi to turn off.");
    delay(1000);
    //httpd_stop(server); //Stop 
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    // Power down Wi-Fi radio
    //WiFi.forceSleepBegin();
    printLine(4, "WiFi is off");
  });

  server.on("/led_0", HTTP_GET, [](){
    digitalWrite(led, 0);
    server.send(200, content_type, "LED:0");
  });

  server.on("/led_1", HTTP_GET, [](){
    digitalWrite(led, 1);
    server.send(200, content_type, "LED:1");
  });

  server.on("/led_flash", HTTP_GET, [](){
    int cnt = 1;
    String sCnt;
    if (server.hasArg("n")) {
      sCnt = server.arg("n");
      cnt = sCnt.toInt();
    }

    for(int i=0; i<cnt; i++) {
      digitalWrite(led, 0);
      delay(200);
      digitalWrite(led, 1);
      delay(200);
    }
    server.send(200, content_type, "LED:FLASHED:" + sCnt);
  });
}

void setup_I2C()
{
  server.on("/i2c", HTTP_GET, [](){

    int wid = serverGetInt("width");
    int hei = serverGetInt("height");
    int sda = serverGetInt("sda");
    int scl = serverGetInt("scl");

    if (wid == 0) wid = SCREEN_WIDTH;
    if (hei == 0) hei = SCREEN_HEIGHT;
    if (sda == 0) sda = isESP32C3 ? ESP32C3_sda : ESP32_sda;
    if (scl == 0) scl = isESP32C3 ? ESP32C3_scl : ESP32_scl;    
  
    server.send(200, content_type, "I2C initialized with width: " + String(wid) + ", height: " + String(hei) + ", SDA: " + String(sda) + ", SCL: " + String(scl));

    //I2C is initialized by the display library, but you can also initialize it here if needed
    //Wire.begin(); // For default SDA and SCL pins
    Wire.setPins(sda, scl); //Custom pin for display
    Adafruit_SSD1306 display(wid, hei, &Wire, OLED_RESET);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.display();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.print("Static IP: ");
    display.println(WiFi.localIP());
    display.display();
  });
}

//Shape Parameters
int xS = 0;
int yS = 0;
int x2S = 0;
int y2S = 0;
int hS = 0;
int wS = 0;
int roundS = 0;
int colorS;
int fillS;
void setup_Display()
{
  server.on("/print", HTTP_GET, [](){
    String resp = serve_Print();
    server.send(200, content_type, resp);
  });

//Draw shape
  server.on("/draw", HTTP_GET, [](){

    String resp = "Shape:";

    for (uint8_t i = 0; i < server.args(); i++) {
      String argName = server.argName(i);
      String argVal = server.arg(i);
      int intVal = argVal.toInt();

      if (argName == "wait") {
        int millisWait = argVal.toInt();
        if (millisWait > 0) {
          delay(millisWait);
        }
      }

      else if (argName == "color") {
        if (argVal == "0")
          colorS = SSD1306_BLACK;
        else
          colorS = SSD1306_WHITE;
      }

      else if (argName == "fill") {
        fillS = intVal;
      }

      else if (argName == "r") {
        roundS = intVal;
      }

      else if (argName == "h") {
        hS = intVal;
      }

      else if (argName == "w") {
        wS = intVal;
      }

      else if (argName == "x") {
        xS = intVal;
      }

      else if (argName == "y") {
        yS = intVal;
      }

      else if (argName == "x2") {
        x2S = intVal;
      }

      else if (argName == "y2") {
        y2S = intVal;
      }

      else if (argName == "rect") {
          if (fillS > 0) {
            if (roundS > 0) display.fillRoundRect(xS, yS, wS, hS, roundS, colorS);
            else display.fillRect(xS, yS, wS, hS, colorS);
          }
          else {
            if (roundS > 0) display.drawRoundRect(xS, yS, wS, hS, roundS, colorS);
            else display.drawRect(xS, yS, wS, hS, colorS);
          }
          if (intVal > 0)
            display.display();
        }

      else if (argName == "line") {
        display.drawLine(xS, yS, x2S, y2S, colorS);
        if (intVal > 0)
          display.display();
      }
      
      else if (argName == "circle") {
        if (fillS > 0) display.fillCircle(xS, yS, wS, colorS);
        else display.drawCircle(xS, yS, wS, colorS);
        if (intVal > 0)
          display.display();
      }

      //else if (argName == "ellipse") {
      //  if (fillS > 0) display.fillEllipse(xS, yS, wS, hS, colorS);
      //  else display.drawEllipse(xS, yS, wS, hS, colorS);
      //  if (intVal > 0)
      //    display.display();
      //}

      else if (argName == "triangle") {
        if (fillS > 0) display.fillTriangle(xS, yS, x2S, y2S, x2S+wS, y2S, colorS);
        else display.drawTriangle(xS, yS, x2S, y2S, x2S+wS, y2S, colorS);
        if (intVal > 0)
          display.display();
      }

      else if (argName == "clear") {
        display.clearDisplay();
        if (intVal > 0)
          display.display();
      }

      else if (argName == "cursor") {
        display.setCursor(xS, yS);
        if (intVal > 0)
          display.display();
      }

      else if (argName == "text") {
        display.print(argVal);
      }

      else if (argName == "print") {
        display.display();
      }

      else
        resp += " ?:" + argName + ":" + argVal;
    } 
    server.send(200, content_type, resp);
  });
}

void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int hr = sec / 3600;
  int min = (sec / 60) % 60;
  sec = sec % 60;

  snprintf(
    temp, 400,

    "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

    hr, min, sec
  );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void printServer()
{
  printLine(3, "Uri: "+server.uri());
  printLine(4, "Args:" + String(server.args()));
}

void handleNotFound() {
  digitalWrite(led, 1);
  printServer();
  String message = "Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, content_type, message);
  digitalWrite(led, 0);
}

/*
/print?color=[0|1]&clear=[1]&size=[1|2]&line=[1|..|4]&text=<text>
*/
int x = 0;
int y = 0;
int size = 1;
int line = 1;
String serve_Print()
{
  String resp = "Print:";

  for (uint8_t i = 0; i < server.args(); i++) {
    String argName = server.argName(i);
    String argVal = server.arg(i);

    if (argName == "wait") {
      int millisWait = argVal.toInt();
      if (millisWait > 0) {
        delay(millisWait);
      }
    }

    else if (argName == "clear") {
      display.clearDisplay();
      display.setCursor(0,0);
      display.display();
      }

    else if (argName == "color") {
      if (argVal == "0")
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      else
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }

    else if (argName == "size") {
      size = argVal.toInt();
      if (size != 1)
        display.setTextSize(2);
      else
        display.setTextSize(1);
    }

    else if (argName == "line") {
      line = argVal.toInt();
      if (size != 1) {
        printLine(line+1, "");
      }
      else {
        printLine(line, "");
      }
    }

    else if (argName == "x") {
      x = argVal.toInt();
      display.setCursor(x, y);
    }

    else if (argName == "y") {
      y = argVal.toInt();
      display.setCursor(x, y);
    }

    else if (argName == "text") {
      display.print(argVal);
    }

    else if (argName == "print") {
      display.display();
    }

    else
        resp += " ?:" + argName + ":" + argVal;
  }

  return resp;
}

void printLine(int line, String str)
{
  int y = (line-1) * LINE_HEIGHT;
  display.fillRect(0, y, SCREEN_WIDTH, LINE_HEIGHT, SSD1306_BLACK);
  display.setCursor(0,y);
  display.print(str);
  display.display();
}

/*void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}*/

void setup_GPIO()
{
  server.on("/gpio", HTTP_GET, [](){

    if (!server.hasArg("pin")) { //Initialize servo to pin
      server.send(200, content_type, "GPIO missing pin");
      return;
    }

    String spin = server.arg("pin");
    int pin = spin.toInt();
    String strReturn = "GPIO pin: " + spin + " :";

    if (server.hasArg("mode")) { // mode=[in|out]
      String setType;
      if (server.arg("mode") == "out") {
        pinMode(pin, OUTPUT); // Set as output
        setType = "OUTPUT";
      }
      else {
        pinMode(pin, INPUT); // Set as input
        setType = "INPUT";
      }
      //server.send(200, content_type, strReturn + "set=" + setType);
      //return;
      strReturn += " mode=" + setType;
    }
    else if (server.hasArg("val")) { // val=[1|0]
      String sVal = server.arg("val");
      digitalWrite(pin, sVal.toInt()); // Write to pin
      //server.send(200, content_type, strReturn + "val=" + sVal);
      //return;
      strReturn += " val=" + sVal;
    }
    else {
      //strReturn += " mode=" + pinModes[pin];
    }

    int pinStat = digitalRead(pin); // Read the pin
    server.send(200, content_type, strReturn + " status=" + pinStat);
  });
}


void setup_ADC()
{
  server.on("/adc", HTTP_GET, [](){
    if (server.hasArg("init")) { //Initialize servo to pin
      String pin = server.arg("init");
      pinMode(pin.toInt(), INPUT); // Set as input
      server.send(200, content_type, "ADC Initialized to pin: " + pin);
    }
    else if (server.hasArg("pin")) { // Check if the "input1" argument exists
      String pin = server.arg("pin");
      int val = analogRead(pin.toInt());
      server.send(200, content_type, "ADC(" + pin + "): " + val);
    } else {
      server.send(400, content_type, "Invalid adc request");
    }
  });
}

//#define servoCount 8
/*struct sServo { 
  Servo myServo;
  int deg;
};
struct sServo myServos[servoCount];*/

#define numServos 9
Servo servos[numServos];
//int servoPins[numServos] = {2, 4, 5, 12, 13, 14, 15, 16, 17}; //Available pins for ESP32
int servoDegs[numServos] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; //Current degrees for each servo
//int servoTimers[numServos] = {-1, -1, -1, -1, -1, -1, -1, -1, -1}; //Timer allocated for each servo
int servoDests[numServos] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; //Destination degrees for each servo
int servoMax[numServos] = {180, 180, 180, 180, 180, 180, 180, 180, 180}; //Max degrees for each servo
int servoMin[numServos] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; //Min degrees for each servo

boolean servoIsTimed = true;
int servoSteps[numServos] = {1, 1, 1, 1, 1, 1, 1, 1, 1}; //Degrees to move per step when timed
int servoTemp[numServos] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; //Degrees to move per step when timed
int servoDelay = 20; //Delay in ms between steps when timed
String respServo;
//Note: Servo available pins: 2, 4, 5, 12-19, 21-23, 25-27, 32-33 (ESP32) and 0-9 (ESP32-C3)

void setup_servo()
{
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  server.on("/servo", HTTP_GET, [](){

    respServo = "";
    String argName;
    String argVal;

    try {
    for (uint8_t i = 0; i < server.args(); i++) {
      argName = server.argName(i);
      argVal = server.arg(i);

      char idxStr = argName[argName.length() - 1]; // Get the last character for index
      int idx = 0;
      if (idxStr >= '0' && idxStr <= '9') {
        idx = idxStr - '0'; // Convert char to int (assuming single digit index)
        if (idx >= numServos) {
          respServo += " ?" + argName;
          continue; // Skip invalid index
        }
      }

      //Set destination degrees for servo (0-180)
      if (argName.startsWith("deg")) {
          int deg = argVal.toInt();
          if (deg < servoMin[idx]) deg = servoMin[idx];
          if (deg > servoMax[idx]) deg = servoMax[idx];
          servoDests[idx] = deg; //Set destination for this servo
          servoTemp[i] = 0; //Reset timer for this servo to move immediately to new destination

          /*if (servoIsTimed) {
            //servoTimers[idx] = millis(); //Start timer for this servo
            servoDests[idx] = deg; //Set destination for this servo
          }
          else {
            servos[idx].write(deg); //Move immediately to position
            servoDegs[idx] = deg; //Update current position
          }*/
        respServo += " D"+String(idx);
      }

      //Add degrees to current destination for servo
      else if (argName.startsWith("add")) {
        int degAdd = argVal.toInt();
        servoDests[idx] += degAdd; //Add to destination for this servo
        if (servoDests[idx] < servoMin[idx]) servoDests[idx] = servoMin[idx];
        if (servoDests[idx] > servoMax[idx]) servoDests[idx] = servoMax[idx];
        servoTemp[i] = 0; //Reset timer for this servo to move immediately to new destination
        respServo += " A"+String(idx);
      }

      else if (argName.startsWith("min")) {
        int degMin = argVal.toInt();
        servoMin[idx] = degMin; //Set minimum for this servo
        if (servoDests[idx] < servoMin[idx]) servoDests[idx] = servoMin[idx];
        respServo += " Min"+String(idx)+":"+String(degMin);
      }

      else if (argName.startsWith("max")) {
        int degMax = argVal.toInt();
        servoMax[idx] = degMax; //Set maximum for this servo
        if (servoDests[idx] > servoMax[idx]) servoDests[idx] = servoMax[idx];
        respServo += " Max"+String(idx)+":"+String(degMax);
      }

      //Set step size for timed movement
      else if (argName.startsWith("steps")) {
        int steps = argVal.toInt();
          servoSteps[idx] = steps;
          respServo += " Steps:" + String(idxStr) + ":" + String(steps);
      }

      //Set delay between steps for timed movement
      else if (argName == "delay") {
        servoDelay = argVal.toInt();
        //if (servoDelay < 10) servoDelay = 10;
        respServo += " Delay:" + String(servoDelay) + "ms";
      }

      //Set whether to move servos in timed steps or immediately
      else if (argName == "timed") {
        servoIsTimed = argVal.toInt() > 0;
        respServo += " Timed:" + String(servoIsTimed);
      }

      //Move servos to their destinations
      else if (argName == "go") {
        loop_servo(argVal.toInt() > 0); //Move servos to their destinations
        respServo += " Go";
      }

      //Wait for a specified time before next arg (allows time for servos to move if in timed mode)
      else if (argName == "wait") {
        int millisWait = argVal.toInt();
        if (millisWait > 0) {
          delay(millisWait);
          respServo += " Wait:" + String(millisWait) + "ms";
        }
      }

      //Initialize servo to pin
      else if (argName.startsWith("pin")) {
          if (argVal.equals("x")) {
            servos[idx].detach();
            respServo += " S"+String(idx)+":x";
          }
          else {
            respServo += " I"+String(idx);
            servos[idx].setPeriodHertz(50); // Standard 50hz servo
            int reserved = servos[idx].attach(argVal.toInt(), 500, 2400); // Attach with min/max pulse widths
            if (reserved == 0) {
              respServo += ": Failed to attach servo" + String(idx) + " to pin " + argVal + "\n";
            }
            else {
              respServo += ":" + String(reserved); // Reserved timer number
            }
            servoDegs[idx] = 90; // Initialize current degree
            servoDests[idx] = 90; // Initialize destination degree
          }
      }

      else {
        respServo += " ?:" + argName;
      }
    }

    loop_servo(servoIsTimed); //Update servo positions if in timed mode

    server.send(200, content_type, "Servo:"+respServo);
  }
  catch (const std::exception& e) {
    Serial.println("Error initializing servo: " + String(e.what()));
    respServo += "\nError: " + String(e.what());
    respServo += "\n";
    server.send(200, content_type, "Error: "+argName+":"+argVal+":"+respServo);
  }
  });
}

void loop_servo(boolean _IsTimed) {
  boolean anyMoving = true;
  while (anyMoving)
  {
    anyMoving = false;
    for (int i = 0; i < numServos; i++) {
      if (servos[i].attached()) {
        int currentDeg = servoDegs[i];
        int destDeg = servoDests[i];
        if (!_IsTimed) {
          currentDeg = destDeg;
          servos[i].write(currentDeg);
          servoDegs[i] = currentDeg;
        }
        else if (currentDeg != destDeg) {
          if (servoTemp[i] > 0) {
            servoTemp[i]--;
          }
          else {
            int step = 1;
            if (currentDeg < destDeg) {
              currentDeg += step;
              if (currentDeg > destDeg) currentDeg = destDeg;
            }
            else {
              currentDeg -= step;
              if (currentDeg < destDeg) currentDeg = destDeg;
            }
            servos[i].write(currentDeg);
            servoDegs[i] = currentDeg;
            servoTemp[i] = servoSteps[i]; //Reset timer for this servo
          }
          anyMoving = true;
        }
      }
    }
    if(anyMoving)
      delay(servoDelay); // Small delay to allow servo movement and reduce CPU usage
  }
}

/*void loop_servo(boolean _IsTimed) {
  boolean anyMoving = true;
  while (anyMoving)
  {
    anyMoving = false;
    for (int i = 0; i < numServos; i++) {
      if (servos[i].attached()) {
        int currentDeg = servoDegs[i];
        int destDeg = servoDests[i];
        if (!_IsTimed) {
          currentDeg = destDeg;
          servos[i].write(currentDeg);
          servoDegs[i] = currentDeg;
        }
        else if (currentDeg != destDeg) {
          int step = servoSteps[i];
          if (currentDeg < destDeg) {
            currentDeg += step;
            if (currentDeg > destDeg) currentDeg = destDeg;
          }
          else {
            currentDeg -= step;
            if (currentDeg < destDeg) currentDeg = destDeg;
          }
          servos[i].write(currentDeg);
          servoDegs[i] = currentDeg;
          anyMoving = true;
        }
      }
    }
    if(anyMoving)
      delay(servoDelay); // Small delay to allow servo movement and reduce CPU usage
  }
}*/


//Ultrasonic Sensor
const int trigPin = 5;
const int echoPin = 18;
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
float distanceInch;

void setup_ultrasonic()
{
  //Ultrasonic Sensor
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  server.on("/ultrasonic", HTTP_GET, [](){
    ultrasonic();
    server.send(200, content_type, String(distanceCm));
  });

}

void ultrasonic()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  // Convert to inches
  //distanceInch = distanceCm * CM_TO_INCH;
}
