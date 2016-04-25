/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"
#include <SoftwareSerial.h>
#include <Servo.h>
Servo myservo; 

SoftwareSerial mySerial(0, 1);
int pos = 0;
/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];

// Saves last known lon/lat
float last_lon = 0.0;
float last_lat = 0.0;

// For button pressing
int pin1 = 3;
volatile int state = LOW;

long lastDebounceTime = 0;
long debounceDelay = 300;
int lastButton = LOW;
int buttonState;

long lastButtonPress = 0;


/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
//  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial1.begin(115200); 
  
  

//  Serial.println(F("Adafruit Bluefruit App Controller Example"));
//  Serial.println(F("-----------------------------------------"));

  /* Initialise the module */
//  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
//  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
//    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }


  /* Disable command echo from Bluefruit */
  ble.echo(false);

//  Serial.println("Requesting Bluefruit info:");
//  /* Print Bluefruit information */
//  ble.info();

//  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
//  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
//  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

//  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
//    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set Bluefruit to DATA mode
//  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

//  Serial.println(F("******************************"));

  pinMode(pin1, INPUT);
  digitalWrite(pin1, HIGH);
// 
//  myservo.write(180);
//  delay(20);
//  myservo.write(90);
}

//int servoPos = 0;

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*
/**************************************************************************/
void loop(void)
{
  /* Wait for new data to arrive */

//  Serial.println(digitalRead(pin1));
  if (digitalRead(pin1) != 1 && (millis() - lastDebounceTime) > debounceDelay) {
        long pressed = millis();
        // transfer lat lon
        Serial1.println(last_lon,7);
        Serial1.println(last_lat,7);
        Serial.println(last_lon,7);
        Serial.println("Button pressed");
        lastDebounceTime = millis();
//        myservo.write(0);
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(180);              // tell servo to go to position in variable 'pos'
////    delay(15);                       // waits 15ms for the servo to reach the position
//  }
    myservo.attach(9); 
    myservo.write(30);
    delay(2000);
    myservo.detach();
//  myservo.write(90);
//  delay(15);
//  myservo.write(90);
//  delay(500);

  }
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  /* Got a packet! */
  // printHex(packetbuffer, len);

  // Color
//  if (packetbuffer[1] == 'C') {
//    uint8_t red = packetbuffer[2];
//    uint8_t green = packetbuffer[3];
//    uint8_t blue = packetbuffer[4];
//    Serial.print ("RGB #");
//    if (red < 0x10) Serial.print("0");
//    Serial.print(red, HEX);
//    if (green < 0x10) Serial.print("0");
//    Serial.print(green, HEX);
//    if (blue < 0x10) Serial.print("0");
//    Serial.println(blue, HEX);
//  }
//
//  // Buttons
//  if (packetbuffer[1] == 'B') {
//    uint8_t buttnum = packetbuffer[2] - '0';
//    boolean pressed = packetbuffer[3] - '0';
//    Serial.print ("Button "); Serial.print(buttnum);
//    if (pressed) {
//      Serial.println(" pressed");
//    } else {
//      Serial.println(" released");
//    }
//  }

  // GPS Location
  if (packetbuffer[1] == 'L') {
    float lat, lon, alt;
    lat = parsefloat(packetbuffer+2);
    lon = parsefloat(packetbuffer+6);
    alt = parsefloat(packetbuffer+10);
    last_lat = lat;
    
    last_lon = lon;
//    Serial.print("GPS Location\t");
//    Serial.print("Lat: "); Serial.print(lat, 4); // 4 digits of precision!
//    Serial.print('\t');
//    Serial.print("Lon: "); Serial.print(lon, 4); // 4 digits of precision!
//    Serial.print('\t');
//    Serial.print(alt, 4); Serial.println(" meters");
  }

  // Accelerometer
  if (packetbuffer[1] == 'A') {
    float x, y, z;
    x = parsefloat(packetbuffer+2);
    y = parsefloat(packetbuffer+6);
    z = parsefloat(packetbuffer+10);
//    Serial.print("Accel\t");
//    Serial.print(x); Serial.print('\t');
//    Serial.print(y); Serial.print('\t');
//    Serial.print(z); Serial.println();
  }

//  // Magnetometer
//  if (packetbuffer[1] == 'M') {
//    float x, y, z;
//    x = parsefloat(packetbuffer+2);
//    y = parsefloat(packetbuffer+6);
//    z = parsefloat(packetbuffer+10);
//    Serial.print("Mag\t");
//    Serial.print(x); Serial.print('\t');
//    Serial.print(y); Serial.print('\t');
//    Serial.print(z); Serial.println();
//  }
//
//  // Gyroscope
//  if (packetbuffer[1] == 'G') {
//    float x, y, z;
//    x = parsefloat(packetbuffer+2);
//    y = parsefloat(packetbuffer+6);
//    z = parsefloat(packetbuffer+10);
//    Serial.print("Gyro\t");
//    Serial.print(x); Serial.print('\t');
//    Serial.print(y); Serial.print('\t');
//    Serial.print(z); Serial.println();
//  }
//
//  // Quaternions
//  if (packetbuffer[1] == 'Q') {
//    float x, y, z, w;
//    x = parsefloat(packetbuffer+2);
//    y = parsefloat(packetbuffer+6);
//    z = parsefloat(packetbuffer+10);
//    w = parsefloat(packetbuffer+14);
//    Serial.print("Quat\t");
//    Serial.print(x); Serial.print('\t');
//    Serial.print(y); Serial.print('\t');
//    Serial.print(z); Serial.print('\t');
//    Serial.print(w); Serial.println();
//  }
}

