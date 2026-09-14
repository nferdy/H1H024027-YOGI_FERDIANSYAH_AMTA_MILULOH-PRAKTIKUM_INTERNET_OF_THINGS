#include <ESP8266WiFi.h>

const char* ssid = "hammed";
const char* password = "anakpphmpp";

const int ledPin = 2; // Pin ESP D4
int waktu = 0; // Variabel pencatat waktu (detik)

void setup() {
  Serial.begin(115200);
  delay(1000); 
  Serial.println();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); 
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi berhasil terhubung!");
  digitalWrite(ledPin, HIGH); // Nyalakan LED
  
  // Mencetak header tabel agar rapi di Serial Monitor
  Serial.println("=======================================================================");
  Serial.println("Wkt(s) | Status    | IP Address      | MAC Address       | RSSI | LED");
  Serial.println("=======================================================================");
}

void loop() {
  // Hanya mencetak data sampai batas 45 detik (sesuai jumlah baris di tabel dapeng)
  if (waktu <= 45) {
    
    // 1. Kolom Waktu
    Serial.print(waktu);
    if(waktu < 10) Serial.print(" "); // Biar spasinya rata untuk angka satuan
    Serial.print("     | ");
    
    // Jika WiFi terhubung, cetak semua data
    if (WiFi.status() == WL_CONNECTED) {
      // 2. Kolom Status
      Serial.print("Terhubung | ");
      
      // 3. Kolom IP Address
      Serial.print(WiFi.localIP());
      Serial.print(" | ");
      
      // 4. Kolom MAC Address
      Serial.print(WiFi.macAddress());
      Serial.print(" | ");
      
      // 5. Kolom RSSI
      Serial.print(WiFi.RSSI());
      Serial.print("  | ");
    } 
    // Jika WiFi terputus, kosongkan datanya dengan tanda strip
    else {
      Serial.print("Terputus  | -               | -                 | -    | ");
    }
    
    // 6. Kolom Status LED
    if (digitalRead(ledPin) == HIGH) {
      Serial.println("HIGH");
    } else {
      Serial.println("LOW");
    }
    
    // Tambah waktu 5 detik untuk putaran selanjutnya
    waktu += 5; 
  }
  
  delay(5000); // Tunggu benar-benar 5 detik sebelum mengulang loop
}