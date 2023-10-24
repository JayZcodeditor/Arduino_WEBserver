#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

const char* ssid = "JXYZ";
const char* password = "313326339";

const int ledPin = D6; // GPIO pin connected to the LED (D6)
const int dhtPin = D4; // GPIO pin connected to the DHT sensor (D4)

DHT dht(dhtPin, DHT11);
ESP8266WebServer server(80);

bool ledState = LOW; // Initialize LED state as off

void init_wifi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.printf("Connected to ");
  Serial.println(ssid);
  Serial.printf("IP address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  String html = "<html><body style='display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh;'>";
  html += "<h1 style='text-align: center;'>ESP8266 Web Server</h1>";
  html += "<form action='/led/on' method='get' style='display: inline;'><input type='submit' value='Turn LED On'></form>";
  html += "<form action='/led/off' method='get' style='display: inline;'><input type='submit' value='Turn LED Off'></form>";
  html += "<p style='text-align: center;'>LED Status: <span style='color: ";
  if (ledState == HIGH) {
    html += "green;'>On</span>";
  } else {
    html += "red;'>Off</span>";
  }
  html += "</p>";
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  html += "<p style='text-align: center;'>Temperature: " + String(temperature) + " &deg;C</p>";
  html += "<p style='text-align: center;'>Humidity: " + String(humidity) + " %</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleLEDOn() {
  digitalWrite(ledPin, HIGH);
  ledState = HIGH;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleLEDOff() {
  digitalWrite(ledPin, LOW);
  ledState = LOW;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  dht.begin();
  init_wifi(ssid, password);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/led/on", HTTP_GET, handleLEDOn);
  server.on("/led/off", HTTP_GET, handleLEDOff);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
