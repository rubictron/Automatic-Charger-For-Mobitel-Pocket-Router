#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";


String serverName = "http://mobile.router/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;


unsigned long timerDelay = 1000;

int btryp;

#define RELLAY_PIN D0

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELLAY_PIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RELLAY_PIN, HIGH);

  delay(500);


  Serial.begin(115200);
  delay(500);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  delay(500);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "mark_title.w.xml";

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();



        int firstListItem = payload.indexOf("<batt_p>") + 8;
        int secondListItem = payload.indexOf("</batt_p>");
        String btrystr = payload.substring(firstListItem, secondListItem);
        btryp = btrystr.toInt();
        Serial.println(btryp);

        if (btryp < 8) {
          timerDelay = 50000;
        } else if (btryp < 12) {
          timerDelay = 15000;
        } else if (btryp < 20) {
          timerDelay = 50000;
        } else {
          timerDelay = 600000;
        }

        if (btryp < 7 ) {

          Serial.println("Charger On ");
          digitalWrite(RELLAY_PIN, LOW);

          digitalWrite(LED_BUILTIN, HIGH);
          delay(500);
          digitalWrite(LED_BUILTIN, LOW );

        } else if (btryp > 99) {
          Serial.println("Charger Off");
          digitalWrite(RELLAY_PIN, HIGH );

          digitalWrite(LED_BUILTIN, LOW );
          delay(500);
          digitalWrite(LED_BUILTIN, HIGH);

        }



      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      WiFi.begin(ssid, password);
      Serial.println("Connecting");
      delay(500);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      timerDelay = 1000;
    }
    lastTime = millis();
  }
}
