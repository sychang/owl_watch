#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif
#include <Adafruit_WINC1500.h>
#ifndef ARDUINO_ARCH_AVR
#include <avr/dtostrf.h>
#endif
// Define the WINC1500 board connections below.
// If you're following the Adafruit WINC1500 board
// guide you don't need to modify these:
#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2     // or, tie EN to VCC and comment this out
// The SPI pins of the WINC1500 (SCK, MOSI, MISO) should be
// connected to the hardware SPI port of the Arduino.
// On an Uno or compatible these are SCK = #13, MISO = #12, MOSI = #11.
// On an Arduino Zero use the 6-pin ICSP header, see:
//   https://www.arduino.cc/en/Reference/SPI

// Setup the WINC1500 connection with the pins above and the default hardware SPI.
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);

// Or just use hardware SPI (SCK/MOSI/MISO) and defaults, SS -> #10, INT -> #7, RST -> #5, EN -> 3-5V
//Adafruit_WINC1500 WiFi;

char ssid[] = "s's iPhone";     //  your network SSID (name)
char pass[] = "6i4qtcgx3vjub";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(141,101,112,175);  // numeric IP for test page (no DNS)
char server[] = "radar-cructches.herokuapp.com";    // domain name for test page (using DNS)
#define webpage "/add_marker"  // path to test page


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
Adafruit_WINC1500Client client;

void setup() {
  // put your setup code here, to run once:
#ifdef WINC_EN
  pinMode(WINC_EN, OUTPUT);
  digitalWrite(WINC_EN, HIGH);
#endif
  Serial.begin(115200);
  Serial.print("Listening for lat,long");
  Serial1.begin(115200);

  Serial.println("WINC1500 Web client test");

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    uint8_t timeout = 10;
    while (timeout && (WiFi.status() != WL_CONNECTED)) {
      timeout--;
      delay(1000);
    }
  }

  Serial.println("Connected to wifi");
  printWifiStatus();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

int incoming = 0;

String serial_read = "";
float lat = 0.0f;
float lon = 0.0f;
void loop() {
    while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // put your main code here, to run repeatedly:
   incoming = Serial1.available();
//    Serial.println("test");
   while (incoming != 0) 
   {
     
     serial_read = Serial1.readString();
     int split = serial_read.indexOf("\n");
     String lon_str = serial_read.substring(0,split);
     String lat_str = serial_read.substring(split+1, serial_read.length() - 1);
     lat = lat_str.toFloat();
     lon = lon_str.toFloat();
     Serial.println(lat);
     Serial.println("\nStarting connection to server...");
      // if you get a connection, report back via serial:
      post();
     
     
     incoming = Serial1.available();
   }
//   Serial.print(lat);
//   Serial.println(lon);
}

void post()
{

        if (client.connect(server, 80)) {
        Serial.println("connected to server");
        // Make a HTTP request:
        client.print("GET ");
//        client.print(webpage);
        client.print("/add_marker?lat=");
        client.print(lat,9);
        client.print("&lon=");
        client.print(lon,9); 
        client.println(" HTTP/1.1");
        client.print("Host: "); 

//        ?var1=value1&var2=value2&var3=value3
        client.println(server);
        client.println("Connection: close");
        client.println();
      }

//  String PostData = "{\"lat\":";
//  char buffer[5];
//  String s = dtostrf(lat, 5, 4, buffer);
//  PostData = PostData + s;
//  PostData = PostData + ",\"lng\":";
//  PostData = PostData + String(lon);
//  PostData = PostData +"}";
//  Serial.println(PostData);
//  String PostData = "{\"test\":1}";
//
//  if (client.connect(server, 80)) {
//    Serial.println("connected");
//    client.println("POST / HTTP/1.1");
//    client.println("Host:  http://radar-cructches.herokuapp.com");
//    client.println("User-Agent: Arduino/1.0");
//    client.println("Connection: close");
//    client.println("Content-Type: application/x-www-form-urlencoded;");
//    client.print("Content-Length: ");
////    client.println(PostData.length()+10);
//    client.println(PostData.length());
//    client.println();
////    client.print("{\"lat\":");
////    client.print(lat);
////    client.print(",\"lng\":");
////    client.print(lon);
////    client.println("}");
//    client.println(PostData);
   else {
    Serial.println("connection failed");
  }
}

