#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <DHT.h>

// -------- WiFi ----------
const char* ssid = "YOUR_WIFI_NAME";
const char* pass = "YOUR_WIFI_PASSWORD";

// -------- KiwisIoT Dashboard ----------
const char* topic = "YOUR_TOPIC_ID";

// -------- Relay ----------
#define RELAY1 D1

// -------- DHT11 ----------
#define DHT_PIN D5
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// -------- Create KiwisIoT object ----------
KiwisIoT kiwisiot(ssid, pass, topic);

// Receive Control from KiwisIoT
void onControl(String ch, String val) {

  Serial.print("Received from KiwisIoT -> ");
  Serial.print("Channel: ");
  Serial.print(ch);
  Serial.print(" | Value: ");
  Serial.println(val);


  // Channel 1 = Relay / Fan Control
  if (ch == "1") {

    // Value 1 = ON
    if (val == "1") {

      // Active LOW relay
      digitalWrite(RELAY1, LOW);

      Serial.println("Relay / Fan: ON");
    }

    // Value 0 = OFF
    else {

      digitalWrite(RELAY1, HIGH);

      Serial.println("Relay / Fan: OFF");
    }
  }
}


// SEND TEMPERATURE TO KIWISIOT
void sendTemperature() {

  float temperature = dht.readTemperature();


  // Check if DHT11 reading is valid
  if (isnan(temperature)) {

    Serial.println("Failed to read temperature from DHT11!");
    return;
  }


  // Display temperature
  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.println(" °C");


  // Send only temperature to Channel 0
  String tempData = String(temperature, 2);

  kiwisiot.send("0", tempData);

  Serial.print("Sent to KiwisIoT Channel 0: ");
  Serial.println(tempData);
}


// SETUP
void setup() {

  // Start Serial Monitor
  Serial.begin(115200);

  Serial.println();
  Serial.println("==============================");
  Serial.println(" SMART FAN + DHT11");
  Serial.println("==============================");


  // Relay setup
  pinMode(RELAY1, OUTPUT);

  // Keep fan OFF at startup
  digitalWrite(RELAY1, HIGH);

  Serial.println("Relay initialized: OFF");


  // Start DHT11
  dht.begin();

  Serial.println("DHT11 initialized");


  // Start KiwisIoT
  Serial.println("Connecting to KiwisIoT...");

  kiwisiot.begin();

  // Register receiving function
  kiwisiot.onReceive(onControl);

  Serial.println("KiwisIoT initialized");
  Serial.println("Waiting for commands...");
}


// LOOP
void loop() {

  // Keep communication with KiwisIoT running
  kiwisiot.run();


  // Send temperature every 2 seconds
  static unsigned long lastDHT = 0;

  if (millis() - lastDHT >= 2000) {

    lastDHT = millis();

    sendTemperature();
  }


  delay(100);
}