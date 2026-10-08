#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "iQ";
const char* password = "yttaajaa";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* topicPerintah = "kuda terbang";

const int ledPin = 4; // GPIO 4 (D2)

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }
  
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);
  
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* perintah = doc["perintah"];
  
  // Baris Tambahan: Mengambil nilai intensitas dari JSON
  // Jika tidak ada parameter "intensitas" yang dikirim, defaultnya diset ke 255 (menyala penuh)
  int intensitas = doc["intensitas"] | 255; 
  
  if (String(perintah) == "ON") {
    // Baris Tambahan: Menggunakan analogWrite untuk mengatur kecerahan (PWM)
    analogWrite(ledPin, intensitas); 
    
    Serial.print("Aktuator: ON, Intensitas: ");
    Serial.println(intensitas);
  } else if (String(perintah) == "OFF") {
    // Baris Tambahan: Mematikan LED dengan nilai PWM 0
    analogWrite(ledPin, 0); 
    
    Serial.println("Aktuator: OFF");
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      client.subscribe(topicPerintah);
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0); // Baris Modifikasi: Inisialisasi awal LED mati menggunakan PWM
  
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop(); 
}