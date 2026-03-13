/*
 * Webserver 2026
 * By Joseph Dizon
 *
 * For ESP32 and ESP32-C3 Super Mini
 * with Adafruit_SSD1306
 * with servo
 * with ultrasonic
 *
 * Download to ESP32 using Arduino IDE
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

//ESP32-C3
#define ESP32C3_led 2
#define ESP32C3_ip4 50

//ESP32
#define ESP32_led 13
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


const char *ssid = "<ssid>";
const char *password = "<password>";

// Define your static IP settings (replace with your actual network info!)
IPAddress local_IP(10, 0, 0, 50); // Choose an available IP in your network
IPAddress gateway(10,0, 0, 1);   // Your router's IP (often 192.168.1.1 or .0.1)
IPAddress subnet(255, 255, 255, 0);  // Your subnet mask (usually this)
IPAddress primaryDNS(8, 8, 8, 8);    // Google DNS (optional but recommended)
IPAddress secondaryDNS(8, 8, 4, 4);  // Google DNS (optional)

WebServer server(80);
String content_type = "text/plain";

String version = "WebServer 2026.1.1";

void setup(void) {
  isESP32C3 = String(ESP.getChipModel()) == "ESP32-C3";
  if (isESP32C3) {
    led = ESP32C3_led;
    IPAddress local_IP(10, 0, 0, ESP32C3_ip4); //Set statis IP address
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
  setup_GPIO();
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


void setup_Server()
{
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

  server.on("/wifi-off", HTTP_GET, [](){
    server.send(200, content_type, "WiFi to turn off.");
    delay(1000);
    //httpd_stop(server); //Stop the httpd server
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    // Power down Wi-Fi radio
    //WiFi.forceSleepBegin();
    printLine(4, "WiFi is off");
  });

  server.on("/info", HTTP_GET, [](){
    server.send(200, content_type, version + "\nChip Info: " + String(ESP.getChipModel()) + (isESP32C3 ? " (ESP32-C3)" : ""));
  });

  server.on("/led_on", HTTP_GET, [](){
    digitalWrite(led, 0);
    server.send(200, content_type, "LED:ON");
  });

  server.on("/led_off", HTTP_GET, [](){
    digitalWrite(led, 1);
    server.send(200, content_type, "LED:OFF");
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

  server.on("/print", HTTP_GET, [](){
    serve_Print();
  });

  //Set Cursor
  server.on("/cursor", HTTP_GET, [](){
    String x = server.arg("x");
    String y = server.arg("y");
    display.setCursor(x.toInt(), y.toInt());
    server.send(200, content_type, "Cursor: " + x + ":" + y);
  });

  //Draw Rectangle
  server.on("/rect", HTTP_GET, [](){
    String x = server.arg("x");
    String y = server.arg("y");
    String h = server.arg("h");
    String w = server.arg("w");

    int c;
    if (server.hasArg("color") && server.arg("color") == "0")
      c = SSD1306_BLACK;
    else
      c = SSD1306_WHITE;

    display.writeFillRect(x.toFloat(), y.toFloat(), w.toFloat(), h.toFloat(), c);
    display.display();
    server.send(200, content_type, "Rectangle ");
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
void serve_Print()
{
  int size;
  String lns;
  String tx = server.arg("text");

  if (server.hasArg("color") && server.arg("color") != "0")
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  else
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

  if (server.hasArg("clear") && server.arg("clear") != "0") {
    display.clearDisplay();
    display.setCursor(0,0);
  }

  if (server.hasArg("size"))
    size = server.arg("size").toInt();
  else
    size = 1;

  if (server.hasArg("line")) { // Check if the "input1" argument exists
    lns = server.arg("line"); // Get the value of "input1"
    int ln = lns.toInt();
    display.setTextSize(1);
    if (size != 1) {
      printLine(ln+1, "");
      display.setTextSize(2);
    }
    printLine(ln, tx);
  } else {
    lns = "";
    if (size != 1)
      display.setTextSize(2);
    else
      display.setTextSize(1);

    display.print(tx);
    display.display();
  }
  server.send(200, content_type, "Print2: " + lns + ": " + tx);
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

Servo myServo;
int servoPin = 19;

void setup_servo()
{
  server.on("/servo", HTTP_GET, [](){
    if (server.hasArg("init")) { //Initialize servo to pin
      String pin = server.arg("init");
      init_servo(pin.toInt());
      server.send(200, content_type, "Servo Initialized to pin: " + pin);
    }
    else if (server.hasArg("deg")) { // Check if the "input1" argument exists
      String deg = server.arg("deg");
      //printLine(3, "");
      //printLine(4, "Servo (deg): " + deg);

      myServo.write(deg.toInt());
      server.send(200, content_type, "Servo (deg): " + deg);
    } else {
      server.send(400, content_type, "Invalid servo request");
    }
  });
}

void init_servo(int _Pin)
{
  myServo.attach(_Pin);
  myServo.write(0); //0-170 degrees
}


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
