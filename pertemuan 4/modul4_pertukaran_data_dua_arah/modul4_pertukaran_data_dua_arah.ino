#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid          = "iQ";
const char* password      = "yttaajaa";
const char* mqttServer    = "broker.hivemq.com";
const int mqttPort        = 1883;

// Topic MQTT untuk Publish dan Subscribe
const char* topicData           = "kuda mabur/data";
const char* topicPerintah       = "kuda mabur/perintah";
const char* topicPerintahBuzzer = "kuda mabur/buzzer"; // Tambahan Topic Baru

#define DHTPIN            4     // Pin D2
#define DHTTYPE           DHT11
const int ledPin          = 5;  // Pin D1
const int buzzerPin       = 14; // Tambahan: Pin D5 untuk Buzzer

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; 

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char) payload[i];
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);
  if (error) {
    return;
  }

  const char* perintah = doc["perintah"];
  
  // Modifikasi Callback: Membedakan aksi berdasarkan topic yang menerima pesan
  if (String(topic) == String(topicPerintah)) {
    if (String(perintah) == "ON") {
      digitalWrite(ledPin, HIGH);
      Serial.println("Perintah LED diterima -> Aktuator LED: ON");
    } else if (String(perintah) == "OFF") {
      digitalWrite(ledPin, LOW);
      Serial.println("Perintah LED diterima -> Aktuator LED: OFF");
    }
  } 
  else if (String(topic) == String(topicPerintahBuzzer)) {
    if (String(perintah) == "ON") {
      digitalWrite(buzzerPin, HIGH);
      Serial.println("Perintah Buzzer diterima -> Aktuator Buzzer: ON");
    } else if (String(perintah) == "OFF") {
      digitalWrite(buzzerPin, LOW);
      Serial.println("Perintah Buzzer diterima -> Aktuator Buzzer: OFF");
    }
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
      
      // Subscribe ke kedua topic
      client.subscribe(topicPerintah);
      client.subscribe(topicPerintahBuzzer); // Tambahan Subscribe baru
      
      Serial.println("Berhasil Subscribe ke topic LED dan Buzzer");
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
  pinMode(buzzerPin, OUTPUT); // Tambahan inisialisasi pin buzzer
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW); // Tambahan inisialisasi status awal buzzer
  
  dht.begin();
  hubungkanWiFi();
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop(); 

  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuTerakhirPublish >= intervalPublish) {
    waktuTerakhirPublish = waktuSekarang;

    float suhu = dht.readTemperature();
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;

      char buffer[128];
      serializeJson(doc, buffer);
      
      client.publish(topicData, buffer);
      Serial.print("Data terkirim ke topic ");
      Serial.print(topicData);
      Serial.print(": ");
      Serial.println(buffer);
    }
  }
}