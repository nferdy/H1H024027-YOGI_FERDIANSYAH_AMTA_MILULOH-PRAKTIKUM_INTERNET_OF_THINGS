#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> 
#include <ArduinoJson.h>

const char* ssid = "iQOO Z9x 5G";
const char* password = "yttaajaa";
const char* serverUrl = "https://httpbin.org/post"; 

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Mengabaikan validasi sertifikat HTTPS
    
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    // Membuat objek data sensor dalam format JSON
    JsonDocument doc;
    doc["suhu"] = 28.5;       
    doc["kelembaban"] = 65.0; 
    
    // --- KODE YANG DITAMBAHKAN ---
    unsigned long waktuMili = millis();
    doc["waktu_ms"] = waktuMili;
    // -----------------------------
    
    String requestBody;
    serializeJson(doc, requestBody);
    
    Serial.print("\nMengirim data: ");
    Serial.println(requestBody);
    
    // Mengirim data melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);
    
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
  delay(10000); 
}