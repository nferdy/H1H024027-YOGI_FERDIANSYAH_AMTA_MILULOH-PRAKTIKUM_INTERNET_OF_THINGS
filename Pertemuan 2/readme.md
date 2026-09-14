
```markdown
# Praktikum IoT Module 2: Konfigurasi Jaringan

**Nama:** Yogi Ferdiansyah Amta Miluloh
**NIM:** H1H024027
**Modul:** Praktikum IoT 2 - Konfigurasi Jaringan
**Mikrokontroler:** ESP8266[cite: 1, 2]
**Topik:** Konfigurasi dan Koneksi Jaringan WiFi

## 1. Deskripsi Praktikum

Praktikum ini bertujuan untuk memahami penggunaan fitur konektivitas WiFi pada mikrokontroler[cite: 4]. Percobaan dilakukan menggunakan dua mode utama, yaitu Mode Station (STA) di mana perangkat bertindak sebagai *client* yang terhubung ke WiFi, dan Mode Access Point (AP) di mana perangkat memancarkan jaringannya sendiri[cite: 4]. Pembacaan dan analisis parameter jaringan seperti IP address, MAC address, jumlah *client*, dan kekuatan sinyal (RSSI) juga diimplementasikan melalui program.

## 2. Library dan Dependencies

### 2.1 Library yang Diperlukan

```cpp
#include <ESP8266WiFi.h>

```

Library `ESP8266WiFi.h` digunakan untuk mengatur seluruh komunikasi jaringan WiFi pada modul mikrokontroler ESP8266, baik saat beroperasi sebagai pemancar (AP) maupun penerima (STA).

## 3. Percobaan 2A: Mode Station (STA)

### 3.1 Tujuan

Memahami dan mengimplementasikan ESP8266 pada mode Station (STA) agar dapat terhubung ke jaringan WiFi yang sudah tersedia dan memonitor parameter koneksinya.

### 3.2 Kode Program (STA)

```cpp
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
  if (waktu <= 45) {
    
    Serial.print(waktu);
    if(waktu < 10) Serial.print(" "); 
    Serial.print("     | ");
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Terhubung | ");
      Serial.print(WiFi.localIP());
      Serial.print(" | ");
      Serial.print(WiFi.macAddress());
      Serial.print(" | ");
      Serial.print(WiFi.RSSI());
      Serial.print("  | ");
    } 
    else {
      Serial.print("Terputus  | -               | -                 | -    | ");
    }
    
    if (digitalRead(ledPin) == HIGH) {
      Serial.println("HIGH");
    } else {
      Serial.println("LOW");
    }
    
    waktu += 5; 
  }
  
  delay(5000); 
}

```

### 3.3 Hasil Pengamatan (Merujuk Tabel 1 Dapeng)

Berdasarkan *output Serial Monitor* pada gambar `IMG-20260909-WA00111.jpg`, diperoleh hasil observasi sebagai berikut:

| Waktu (s) | Status Koneksi | IP Address | MAC Address | RSSI (dBm) | Status LED |
| --- | --- | --- | --- | --- | --- |
| 0 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -37 | HIGH |
| 5 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -44 | HIGH |
| 10 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -43 | HIGH |
| 15 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -55 | HIGH |
| 20 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -53 | HIGH |
| 25 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -51 | HIGH |
| 30 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -35 | HIGH |
| 35 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -47 | HIGH |
| 40 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -54 | HIGH |
| 45 | Terhubung | 10.249.141.149 | 84:F3:EB:5A:73:66 | -49 | HIGH |

(Sesuai dengan format Tabel 1: Pengamatan Koneksi WiFi Mode Station (STA))

**Analisis:**
ESP8266 berhasil terhubung ke jaringan dengan SSID "hammed". Perangkat mendapatkan alokasi IP Address `10.249.141.149` dengan konektivitas yang stabil selama siklus 45 detik. Nilai RSSI berfluktuasi antara -35 dBm hingga -55 dBm, yang menunjukkan kualitas tangkapan sinyal berada dalam level yang baik dan sangat kuat.

---

## 4. Percobaan 2B: Mode Access Point (AP)

### 4.1 Tujuan

Mengimplementasikan konfigurasi ESP8266 sebagai penyedia jaringan atau *Access Point* mandiri yang mampu diakses langsung oleh perangkat lain tanpa memerlukan *router* eksternal.

### 4.2 Kode Program (AP)

```cpp
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
    
    Serial.print(waktu);
    if(waktu < 10) Serial.print(" "); 
    Serial.print("     | ");
    
    // Mengecek jumlah perangkat yang terhubung ke AP
    int jumlahClient = WiFi.softAPgetStationNum();
    
    Serial.print(jumlahClient);
    Serial.print("          | ");
    
    if (jumlahClient > 0) {
      Serial.print("Smartphone/Laptop   | ");
      Serial.println("Perangkat berhasil terkoneksi");
    } else {
      Serial.print("-                   | ");
      Serial.println("Menunggu perangkat terhubung...");
    }
    
    waktu += 5; 
  }
  
  delay(5000); 
}

```

### 4.3 Pengamatan Access Point (Merujuk Tabel 3 Dapeng)

| No. | Parameter | Nilai Konfigurasi | Hasil Pengamatan |
| --- | --- | --- | --- |
| 1 | SSID | ESP punya yogi | ESP punya yogi |
| 2 | Password | 12345678 | Berhasil digunakan |
| 3 | IP Address AP | 192.168.4.1 | 192.168.4.1 |
| 4 | Status Access Point | Aktif | Aktif |
| 5 | SSID terdeteksi pada client | Ya | Ya |
| 6 | Perangkat berhasil terhubung | - | Ya, terkoneksi hingga 3 *client* pada pengujian |

(Sesuai dengan format Tabel 3: Pengamatan Access Point (AP))

### 4.4 Pengamatan Jumlah Perangkat Terhubung (Merujuk Tabel 4 Dapeng)

Berdasarkan *output Serial Monitor* pada gambar `IMG-20260909-WA00131.jpg`:

| Waktu (s) | Jumlah Client | Perangkat Terhubung | Keterangan |
| --- | --- | --- | --- |
| 0 | 0 | - | Menunggu perangkat terhubung... |
| 5 | 0 | - | Menunggu perangkat terhubung... |
| 10 | 0 | - | Menunggu perangkat terhubung... |
| 15 | 0 | - | Menunggu perangkat terhubung... |
| 20 | 0 | - | Menunggu perangkat terhubung... |
| 25 | 1 | Smartphone/Laptop | Perangkat berhasil terkoneksi |
| 30 | 1 | Smartphone/Laptop | Perangkat berhasil terkoneksi |
| 35 | 2 | Smartphone/Laptop | Perangkat berhasil terkoneksi |
| 40 | 3 | Smartphone/Laptop | Perangkat berhasil terkoneksi |
| 45 | 3 | Smartphone/Laptop | Perangkat berhasil terkoneksi |

(Sesuai dengan format Tabel 4: Pengamatan Jumlah Perangkat Terhubung ke AP)

**Analisis:**
ESP8266 berhasil bertindak sebagai server/penyedia jaringan mandiri dengan SSID "ESP punya yogi". Pada rentang waktu awal (0-20 detik), belum ada perangkat yang terkoneksi. Modul kemudian berhasil merespons dan mengakomodasi sambungan dari *client* (Smartphone/Laptop) yang terus bertambah dari 1 perangkat pada detik ke-25 hingga akhirnya mencapai 3 perangkat pada detik ke-40.

---

## 5. Pertanyaan Praktikum

**1. Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!**
Pada Percobaan STA, ESP8266 berhasil tersambung sebagai klien ke SSID "hammed" dan secara otomatis mendapatkan IP address `10.249.141.149` beserta data MAC dan RSSI *real-time*. Pada percobaan AP, mikrokontroler membuktikan fungsionalitasnya sebagai penyedia jaringan dengan memancarkan SSID "ESP punya yogi" yang valid dan berhasil diakses oleh hingga 3 perangkat klien sekaligus.

**2. Bagaimana pengaruh kekuatan sinyal (RSSI) terhadap kestabilan koneksi WiFi pada perangkat IoT?**
Nilai RSSI (ditampilkan dalam dBm) mencerminkan kualitas sinyal WiFi yang diterima oleh modul ESP8266. Jika nilainya semakin mendekati angka 0 (seperti rentang -35 hingga -55 dBm pada hasil percobaan), sinyal tersebut tergolong sangat kuat, yang berarti koneksi antarperangkat berjalan secara stabil tanpa adanya potensi komunikasi terputus atau tingkat kehilangan data (*packet loss*) yang tinggi.

**3. Bagaimana cara kerja ESP8266 dalam membedakan peran sebagai klien (Station) dan sebagai penyedia jaringan (Access Point)?**
Pembedaan fungsi ini sepenuhnya diatur melalui deklarasi `WiFi.mode()` yang ada di library `<ESP8266WiFi.h>`. Ketika fungsi yang dipanggil adalah `WiFi.mode(WIFI_STA)`, perangkat difungsikan sebagai penerima untuk mengikatkan dirinya pada *router* yang ada. Sebaliknya, jika `WiFi.mode(WIFI_AP)` dipanggil, maka ESP8266 akan membangkitkan IP lokalnya sendiri dan melakukan penyiaran SSID *(broadcasting)* agar gawai lain dapat masuk ke dalam jaringannya.

**4. Bagaimana kombinasi mode Station dan Access Point (AP+STA) dapat dimanfaatkan dalam skenario nyata sistem IoT?**
Fitur hibrida AP+STA sangat ideal diterapkan pada proses *provisioning* awal sebuah produk IoT. ESP8266 dapat menyalakan mode AP untuk menyajikan *web server* lokal yang bisa diakses pengguna melalui HP untuk merotasi input kredensial WiFi. Secara bersamaan, dengan fungsi STA yang menyala, ESP8266 menggunakan data yang baru di-input oleh pengguna tersebut untuk melakukan *bridging* ke jaringan internet rumah tanpa harus menulis-ulang (*hardcode*) algoritma di aplikasi *source code*.

```

```
