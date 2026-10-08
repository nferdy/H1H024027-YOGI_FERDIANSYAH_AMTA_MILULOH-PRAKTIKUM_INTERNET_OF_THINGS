# Modul4 - Percobaan 
## Yogi Ferdiansyah Amta Miluloh
## H1H024027
## Shift (B)

## Library atau Dependencies yang Diperlukan
- Arduino IDE
- Board NodeMCU ESP8266
- Sensor Suhu & Kelembaban DHT11
- Aktuator: LED dan Buzzer
- Kabel Jumper dan Breadboard
- Library `ESP8266WiFi`: Untuk mengelola koneksi jaringan nirkabel pada modul ESP8266.
- Library `PubSubClient`: Untuk implementasi klien protokol komunikasi MQTT (Publish dan Subscribe).
- Library `ArduinoJson`: Untuk serialisasi (membentuk) dan deserialisasi (mengurai) format data JSON.
- Library `DHT sensor library`: Untuk membaca antarmuka data dari sensor DHT11.
- Aplikasi Client MQTT Explorer: Untuk memantau data yang masuk (subscribe) dan mengirim perintah kendali (publish) dari sisi komputer/PC.

## Penjelasan Code
### Kode Percobaan 4A (Subscribe dan Deserialisasi JSON)
Program ini mengatur ESP8266 agar mendaftarkan diri secara pasif (subscribe) ke sebuah topic spesifik pada broker MQTT publik HiveMQ. Saat pesan masuk terdeteksi, fungsi callback akan menangkap payload mentah, mengonversinya menjadi teks string, lalu menggunakan pustaka ArduinoJson untuk melakukan deserialisasi (parsing) teks tersebut menjadi objek JSON yang terstruktur. Nilai dari kunci (key) "perintah" kemudian diekstrak untuk menghidupkan (ON) atau mematikan (OFF) aktuator secara instan.

### Kode Percobaan 4B (Komunikasi Full Duplex)
Program ini merupakan implementasi sistem IoT pertukaran data dua arah (bidirectional) murni. ESP8266 bertindak ganda sebagai publisher sekaligus subscriber tanpa saling memblokir (non-blocking). Di satu sisi, mikrokontroler membaca suhu dari sensor DHT11 dan mempublikasikan data tersebut dalam format JSON setiap 5 detik menggunakan referensi fungsi waktu millis(). Di sisi lain, fungsi client.loop() secara kontinu terus mendengarkan jalur masuk pada topic perintah. Apabila ada perintah JSON masuk, ESP8266 akan langsung mengeksekusinya tanpa harus menunggu jeda proses pengiriman data suhu selesai.

## Penjelasan Setiap Fungsi
- `deserializeJson(doc, pesan)`: Menerjemahkan (parsing) teks berformat JSON yang diterima dari broker menjadi struktur objek/dokumen di memori mikrokontroler agar nilainya bisa dipanggil berdasarkan kunci (misalnya doc["perintah"]).
- `client.setCallback(callback)`: Mendaftarkan sebuah fungsi kustom yang akan otomatis dieksekusi secara asinkron oleh sistem setiap kali ada pesan baru yang masuk pada topic yang sedang didengarkan.
- `client.subscribe(topic)`: Mendaftarkan klien ESP8266 ke broker untuk mulai menerima lalu lintas pesan pada alamat topic tertentu.
- `client.loop()`: Fungsi esensial yang dipanggil secara terus-menerus di dalam loop() utama untuk mempertahankan koneksi (keep-alive) dengan broker serta memproses antrean pesan yang masuk dan keluar.
- `millis()`: Mengembalikan nilai waktu (dalam milidetik) sejak mikrokontroler pertama kali menyala, digunakan sebagai instrumen penjadwalan tugas non-blocking pengganti delay().
- `analogWrite(pin, intensitas)`: Menghasilkan sinyal PWM (Pulse Width Modulation) pada pin GPIO ESP8266 untuk mengatur variasi tingkat kecerahan aktuator LED.

## Penjelasan Percabangan/Conditional
**Perulangan (Looping):**
- `while (!client.connected())`: Menahan eksekusi program jika ESP8266 kehilangan koneksi dengan broker MQTT, dan memaksanya untuk melakukan proses reconnect berulang kali hingga statusnya kembali tersambung, lalu mendaftarkan ulang subscribe.

**Percabangan (If-Else):**
- `if (error)`: Mengevaluasi status hasil operasi deserializeJson(). Jika gagal (format bukan JSON yang valid), program mencetak status error dan menggunakan perintah return untuk langsung keluar dari fungsi callback guna mencegah malfungsi lanjutan.
- `if (millis() - waktuTerakhirPublish >= intervalPublish)`: Fondasi arsitektur non-blocking. Program hanya akan masuk ke dalam blok pengiriman data sensor apabila selisih waktu saat ini dengan waktu pengiriman terakhir sudah melampaui interval 5000 ms.
- `if (String(topic) == String(topicPerintah))` : Menyeleksi rute pemrosesan dengan membandingkan alamat topic asal pesan, sehingga mikrokontroler bisa menentukan arah kendali ke pin keras yang tepat (seperti LED atau Buzzer) tanpa tumpang tindih.

## Penjelasan Singkat Mengenai Detail Percobaan
Pada Percobaan 4A, sistem berhasil mendemonstrasikan bagaimana mikrokontroler membedah paket data (JSON) dari server luar untuk menggerakkan komponen keras secara lokal. Pada Percobaan 4B, sistem sukses menerapkan kemampuan multitasking (full duplex), membuktikan bahwa dengan menghindari sintaks blocking, mikrokontroler mampu membaca sensor, mempublikasikan metrik, serta merespons kendali eksternal secara simultan dan real-time dengan koneksi yang stabil.

---

## Jawaban Pertanyaan Praktikum yang Berkaitan dengan Code

### 1. Modifikasi Percobaan 4A (Pertanyaan 4.5.4 No. 4 - Kontrol Intensitas PWM)
Berikut adalah modifikasi program penerimaan JSON yang telah ditambahkan fitur pembacaan parameter "intensitas" guna mengatur kecerahan LED melalui sinyal PWM, disesuaikan persis dengan basis kode yang telah dikembangkan.

~~~cpp
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
~~~

**Penjelasan Penambahan Kode:**
- `int intensitas = doc["intensitas"] | 255;`: Baris ini mendeklarasikan variabel baru untuk menampung ekstrak nilai numerik dari key "intensitas". Penggunaan operator logika `| 255` bertindak sebagai nilai pengaman (fallback) yang otomatis mengatur kecerahan ke tingkat maksimum jika pengirim publish MQTT tidak menyertakan variabel "intensitas" di dalam body JSON-nya.
- `analogWrite(ledPin, intensitas);`: Fungsi ini ditambahkan untuk menggantikan digitalWrite(). Ia membangkitkan sinyal PWM pada ESP8266 di mana lebar pulsanya bervariasi sesuai dengan rentang angka variabel intensitas, memampukan LED untuk meredup atau menyala terang secara dinamis.
- `analogWrite(ledPin, 0);` pada setup dan blok OFF: Digunakan untuk memastikan perangkat keras benar-benar mati dengan memutus total lebar pulsa PWM ke nilai nol.

### 2. Modifikasi Percobaan 4B (Pertanyaan 4.6.4 No. 4 - Penambahan Topic Buzzer)
Berikut adalah modifikasi kode komunikasi full duplex dengan integrasi aktuator kedua (Buzzer), menggunakan topic MQTT yang terpisah dan merombak logika fungsi callback().

~~~cpp
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
~~~

**Penjelasan Penambahan Kode:**
- `const char* topicPerintahBuzzer = "kuda mabur/buzzer";`: Mendeklarasikan variabel memori alamat topic sekunder. Jalur ini didedikasikan sepenuhnya untuk mengisolasi instruksi perintah yang masuk khusus untuk mengontrol Buzzer.
- `const int buzzerPin = 14;`: Mengonfigurasi pin fisik GPIO 14 (D5) sebagai jalur tegangan untuk perangkat Buzzer.
- `if (String(topic) == String(topicPerintah)) { ... } else if (String(topic) == String(topicPerintahBuzzer)) { ... }`: Menambahkan struktur kontrol logika tingkat atas di dalam callback. Mikrokontroler membandingkan string nama topic pesan masuk dengan string topic perangkat. Jika rutenya cocok dengan LED, arus digitalWrite dialihkan ke GPIO 5. Jika rutenya cocok dengan Buzzer, dieksekusi ke GPIO 14.
- `client.subscribe(topicPerintahBuzzer);`: Baris ini diinjeksikan pada blok prosedur reconnect agar modul ESP8266 mendaftarkan pendengaran asinkron ganda ke broker, memastikan ia memantau pesan masuk dari kedua topic tersebut secara simultan.
- `pinMode(buzzerPin, OUTPUT);` dan `digitalWrite(buzzerPin, LOW);`: Inisialisasi awal pada fungsi setup() untuk memastikan pin Buzzer berfungsi sebagai jalur keluaran (output) dan berada dalam kondisi mati saat pertama kali modul dinyalakan.
