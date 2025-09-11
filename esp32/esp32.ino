#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

#define RXD2 16
#define TXD2 17

FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();
    Serial.println("Received: " + data);

    if (Firebase.pushString(firebaseData, "/esp32/data", data)) {
      Serial.println("Data sent to Firebase successfully!");
    } else {
      Serial.println("Firebase push failed: " + firebaseData.errorReason());
    }
  }
}
