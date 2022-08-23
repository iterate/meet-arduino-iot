/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>

// OLED FeatherWing buttons map to different pins depending on board.
// The I2C (Wire) bus may also be different.
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define WIRE Wire
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
  #define WIRE Wire
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
  #define WIRE Wire
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
  #define WIRE Wire
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  #define BUTTON_A  9
  #define BUTTON_B  8
  #define BUTTON_C  7
  #define WIRE Wire1
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#endif

ESP8266WiFiMulti WiFiMulti;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

uint8_t atou8(const char *s)
{
  uint8_t v = 0;
      while (*s) { v = (v << 1) + (v << 3) + (*(s++) - '0'); }
      return v;
}

const char * headerKeys[] = {"x-api-key: XtCmKi8obdXfXu8Prjq4wPVfowaKFZCf"};
const size_t numberOfHeaders = 1;

void setup() {

  //const uint8_t converted_value = atou8(my_string[0]);

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println("");
  Serial.println("");

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  Serial.println("OLED begun");

  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("IterateG17Guest", "woopwoop");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    //client.setCACert(root_ca);
    //client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://meet-the-backend.app.iterate.no/api/v1/esp/desk/sloyden_desk_2")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      https.addHeader("x-api-key", "XtCmKi8obdXfXu8Prjq4wPVfowaKFZCf");
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          //Serial.println(https);
          display.setTextSize(2);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(0,0);
          display.clearDisplay();
          delay(200);
          display.println(payload);
          //display.setCursor(0,0);
          display.display(); // actually display all of the above
          display.startscrollright(0x00, 0x0F);
          delay(2000);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  Serial.println("Wait 10s before next round...");
  delay(10000);
}
