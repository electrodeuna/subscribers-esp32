#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

///------- Reemplaza lo siguiente! --------///
char ssid[] = "Nombre-Wifi";       // Nombre Wifi
char password[] = "Password-Wifi";  // Password Wifi
String API_KEY = "TU-API-KEY";  // Token API
String CHANNEL_ID = "URL-YOUTBE"; // Url de Canal de Youtube
//------- ---------------------- ------

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RST -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

unsigned long timeBetweenRequests = 60000;
unsigned long nextRunTime;

long suscriptores;

static const uint8_t suscriptores32[128] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x18, 0x18, 0x00, 
    0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x0f, 0x18, 0x18, 0xf0, 
    0x1f, 0x9c, 0x39, 0xf8, 0x39, 0xcf, 0xf3, 0x9c, 0x30, 0xdf, 0xfb, 0x0c, 0x30, 0xf8, 0x1f, 0x0c, 0x39, 0xe0, 0x07, 0x9c, 
    0x1f, 0xc0, 0x03, 0xf8, 0x3f, 0x80, 0x01, 0xfc, 0x71, 0x80, 0x01, 0x8e, 0x63, 0x00, 0x00, 0xc6, 0xc3, 0x00, 0x00, 0xc3, 
    0xc3, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t icon_youtube[32] = {
  0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x20, 0x04, 0x40, 0x02, 0x42, 0x02, 0x43, 0x82, 0x43, 0xe2,
  0x43, 0x82, 0x42, 0x02, 0x40, 0x02, 0x20, 0x04, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);  //TEXTO EN COLOR BLANCO
  
  display.display();
}

void loop() {

  if (millis() > nextRunTime)  {
    HTTPClient https;
      
    https.begin("https://www.googleapis.com/youtube/v3/channels?part=statistics&id=" + CHANNEL_ID + "&key=" + API_KEY);
    int httpCode = https.GET();
    
    Serial.print("HttpCode: ");
    Serial.println(httpCode);

    if (httpCode == 200)
    {
      DynamicJsonDocument doc(512);
      String payload = https.getString();
      
      deserializeJson(doc, payload);
      JsonObject itemStatistics = doc["items"][0]["statistics"];

			suscriptores = itemStatistics["subscriberCount"].as<long>();

      display.clearDisplay();
      display.drawBitmap(34,6, icon_youtube, 16, 16, 1);
      
      display.setTextSize(1.8);
      display.setCursor(55, 10);
      display.println("YouTube");
  
      display.drawBitmap(25, 20, suscriptores32, 32, 32, 1);

      display.setTextSize(2);
      display.setCursor(65, 30);
      display.println(suscriptores);

      display.setTextSize(1.2);
      display.setCursor(14, 55);
      display.println("Electronica De Una"); // Nombre de tu Canal de Youtube

      display.display();
      delay(200);
    }
    https.end();
    nextRunTime = millis() + timeBetweenRequests;
  }
}
