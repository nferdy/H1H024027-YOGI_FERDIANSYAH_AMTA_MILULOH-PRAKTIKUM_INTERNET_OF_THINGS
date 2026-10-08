# Library atau dependencies yang diperlukan
- Arduino IDE
- NodeMCU ESP8266
- Kabel Micro USB 
- Library `ESP8266WiFi` 
- Library `ESP8266HTTPClient` 
- Library `WiFiClientSecure` 
- Library `ArduinoJson` 
- Library `PubSubClient` 
- Aplikasi client MQTT Explorer

# Penjelasan kode
## Kode Percobaan 3A (HTTP POST)
Program ini mengatur ESP8266 sebagai klien jaringan yang mengirimkan data ke server publik menggunakan protokol HTTP POST. Karena endpoint tujuan (`https://httpbin.org/post`) menggunakan keamanan HTTPS, program memanfaatkan `WiFiClientSecure` untuk mengabaikan validasi sertifikat. Nilai sensor (suhu, kelembaban, dan waktu uptime mikrokontroler) dibungkus ke dalam objek memori JSON menggunakan pustaka `ArduinoJson`. Data tersebut kemudian diserialisasi menjadi teks dan dikirimkan ke server setiap 10 detik.

## Kode Percobaan 3B (MQTT)
Program ini mengonfigurasi ESP8266 untuk berkomunikasi dengan broker MQTT publik (`broker.hivemq.com` pada port 1883). Setelah koneksi jaringan dan broker berhasil diinisialisasi, perangkat bertindak sebagai publisher. Di dalam loop utama, program terus-menerus merakit pesan sensor berformat JSON dan mempublikasikannya ke dalam topic unik (`kuda lari`) secara persisten setiap 5 detik tanpa memutus koneksi.

# Penjelasan setiap fungsi
- `WiFi.begin(ssid, password)`: Memulai proses koneksi ESP8266 ke jaringan Wi-Fi eksternal.
- `client.setInsecure()`: Memerintahkan `WiFiClientSecure` pada ESP8266 untuk mengabaikan proses validasi sertifikat SSL/TLS saat mengakses website HTTPS.
- `http.begin(client, serverUrl)`: Membuka jalur komunikasi menuju URL server tujuan sebelum data dikirim.
- `http.addHeader()`: Menyisipkan parameter metadata (seperti `Content-Type: application/json`) agar server mengetahui format teks yang sedang dikirimkan.
- `serializeJson(doc, requestBody)`: Mengonversi/menerjemahkan objek JSON dari dalam memori RAM menjadi sebuah String teks murni agar bisa ditransmisikan melintasi jaringan.
- `http.POST(requestBody)`: Mengeksekusi pengiriman data (payload) ke server dan mengembalikan kode status pengiriman.
- `client.setServer(mqttServer, mqttPort)`: Menetapkan alamat IP/URL dari broker MQTT beserta port yang akan digunakan.
- `client.connect(clientId)`: Membuka sesi koneksi aktual dengan broker MQTT menggunakan identitas klien yang unik.
- `client.loop()`: Memerintahkan ESP8266 untuk mempertahankan koneksi MQTT di latar belakang (keep-alive) dan memproses aliran paket data.
- `client.publish(mqttTopic, buffer)`: Mempublikasikan data teks JSON ke dalam topic spesifik pada broker MQTT.

# Penjelasan percabangan/conditional
**Perulangan:**
- `while (WiFi.status() != WL_CONNECTED)`: Menahan laju eksekusi program dan mencetak titik-titik (.) hingga mikrokontroler benar-benar sukses mendapatkan alamat IP dari router.
- `while (!client.connected())`: Digunakan pada percobaan MQTT untuk memastikan broker terhubung. Jika koneksi terputus, perulangan ini akan mengurung program dan terus melakukan percobaan reconnect hingga koneksi kembali pulih.

**Percabangan:**
- `if (httpResponseCode > 0) ... else ...`: Mengevaluasi hasil dari fungsi HTTP POST. Jika nilainya lebih besar dari 0 (misal: 200), berarti pengiriman sukses dan program akan mencetak balasan server. Jika tidak (masuk ke blok else), berarti terjadi error jaringan sehingga program akan mencetak kode error negatif.

# Penjelasan singkat mengenai detail percobaan
### Percobaan 3A (HTTP POST dengan Format JSON)
Mengonfigurasi ESP8266 untuk merakit paket data terstruktur berformat JSON dan mengirimkannya sekali jalan (request-response) ke server publik (`httpbin.org`). Percobaan ini membuktikan bahwa server menerima format dengan benar melalui pantulan (echo) response body di Serial Monitor.
### Percobaan 3B (Komunikasi MQTT)
Mendemonstrasikan kelancaran pertukaran data ringan yang berkelanjutan (publish-subscribe). ESP8266 secara konsisten mengirimkan metrik sensor ke broker publik HiveMQ tanpa memutus koneksi jaringan, yang kemudian berhasil ditangkap (subscribe) secara real-time oleh aplikasi PC MQTT Explorer.

# Jawaban pertanyaan praktikum yang berkaitan dengan code
## Pertanyaan 3.5.4 No 4 (Modifikasi Percobaan 3A - Penambahan millis)
Berikut adalah modifikasi source code Percobaan HTTP POST menggunakan arsitektur ESP8266 dengan tambahan injeksi parameter waktu uptime (`millis()`) ke dalam struktur JSON.

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> 
#include <ArduinoJson.h>

const char* ssid = "hammed";
const char* password = "kudalari";
const char* serverUrl = "[https://httpbin.org/post](https://httpbin.org/post)"; 

void setup() {
  Serial.begin(115200);
  delay(1000);
  
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
    client.setInsecure(); // ESP8266: Mengabaikan validasi sertifikat HTTPS
    
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    // Membuat objek data sensor dalam format JSON
    JsonDocument doc;
    doc["suhu"] = 28.5;       
    doc["kelembaban"] = 65.0; 
    
    // --- MODIFIKASI: PENAMBAHAN DATA WAKTU MILLIS() ---
    unsigned long waktuMili = millis();
    doc["waktu_ms"] = waktuMili;
    // --------------------------------------------------
    
    String requestBody;
    serializeJson(doc, requestBody);
    
    Serial.print("\nMengirim data: ");
    Serial.println(requestBody);
    
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

# Penjelasan Penambahan Kode
unsigned long waktuMili = millis(); : Mendeklarasikan variabel baru untuk menampung fungsi bawaan millis(), yaitu jumlah waktu (dalam milidetik) yang telah berlalu semenjak board ESP8266 pertama kali dinyalakan.