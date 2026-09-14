#include <ESP8266WiFi.h> // Library khusus untuk ESP8266

const char* ap_ssid = "ESP punya yogi"; 
const char* ap_password = "12345678"; // Password minimal 8 karakter

int waktu = 0; // Variabel pencatat waktu (detik)

void setup() {
  Serial.begin(115200);
  delay(1000); // Trik menghilangkan karakter bootloader
  Serial.println();
  
  // Set mode WiFi menjadi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress apIP = WiFi.softAPIP();
  
  Serial.println("Access Point aktif!");
  Serial.print("SSID       : ");
  Serial.println(ap_ssid);
  Serial.print("IP Address : ");
  Serial.println(apIP);
  
  // Mencetak header Tabel 4 agar rapi di Serial Monitor
  Serial.println("\n=========================================================================");
  Serial.println("Wkt(s) | Jml Client | Perangkat Terhubung | Keterangan");
  Serial.println("=========================================================================");
}

void loop() {
  // Hanya mencetak data sampai batas 45 detik (sesuai jumlah baris di tabel dapeng)
  if (waktu <= 45) {
    
    // 1. Kolom Waktu
    Serial.print(waktu);
    if(waktu < 10) Serial.print(" "); // Biar spasinya rata untuk angka satuan
    Serial.print("     | ");
    
    // Mengecek jumlah perangkat yang terhubung ke AP
    int jumlahClient = WiFi.softAPgetStationNum();
    
    // 2. Kolom Jumlah Client
    Serial.print(jumlahClient);
    Serial.print("          | ");
    
    // 3 & 4. Kolom Perangkat Terhubung & Keterangan
    if (jumlahClient > 0) {
      Serial.print("Smartphone/Laptop   | ");
      Serial.println("Perangkat berhasil terkoneksi");
    } else {
      Serial.print("-                   | ");
      Serial.println("Menunggu perangkat terhubung...");
    }
    
    // Tambah waktu 5 detik untuk putaran selanjutnya
    waktu += 5; 
  }
  
  delay(5000); // Tunggu benar-benar 5 detik sebelum mengulang loop
}