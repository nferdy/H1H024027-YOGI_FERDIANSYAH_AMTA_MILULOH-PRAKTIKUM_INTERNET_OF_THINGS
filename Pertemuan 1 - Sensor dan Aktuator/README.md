**Pertemuan 1 - Sensor dan Aktuator**

**Penjelasan Code**

**Percobaan 1**
Berkas kode ditempatkan pada direktori `code/Percobaan1A.ino`. Praktikum ini mengoperasikan mikrokontroler ESP32 DevKit yang disandingkan dengan sensor DHT11. Pin komunikasi data sensor dihubungkan langsung ke GPIO 4. Dalam setiap siklus eksekusi, program akan merekam metrik suhu maupun kelembaban, lalu menampilkannya melalui *Serial Monitor*. Apabila proses akuisisi data tersebut gagal, sistem secara otomatis akan memunculkan peringatan *error*. Jika berhasil, parameter suhu dan kelembaban akan dicetak secara bersamaan dengan jeda waktu pembacaan setiap 3 detik.

**Percobaan 2**
Berkas program disimpan pada `code/Percobaan2A.ino`. Rangkaian ini melibatkan ESP32, sensor DHT11 di pin GPIO 4, serta perangkat keluaran (modul relay atau LED) pada pin GPIO 26. Nilai suhu hasil pembacaan akan diuji terhadap titik batas (*threshold*) yang ditetapkan pada 30.0 °C. Bila kondisi suhu lingkungan melampaui batas tersebut, aktuator dipicu untuk menyala (ON). Sebaliknya, bila suhu tercatat persis atau lebih rendah dari ambang batas, aliran daya ke aktuator diputus (OFF).

**Penjelasan Setiap Fungsi**

* **`setup()`**: Blok konfigurasi awal yang dieksekusi satu kali saat *board* dinyalakan. Digunakan untuk mengatur parameter *Serial Monitor*, mode pin relay, serta proses inisialisasi sensor.


* **`loop()`**: Blok pengerjaan utama yang beroperasi secara terus-menerus tanpa henti untuk mengolah fungsi pembacaan data dan instruksi logika kendali.


* **`dht.begin()`**: Perintah wajib untuk membuka dan mengaktifkan komunikasi dengan sensor keluarga DHT.


* **`dht.readTemperature()`**: Fungsi instruksional khusus untuk mengekstrak parameter nilai suhu dari sensor.


* **`dht.readHumidity()`**: Fungsi instruksional untuk mengambil nilai persentase kelembaban relatif.


* **`isnan()`**: Mekanisme validasi keluaran sensor untuk memeriksa apakah data yang ditangkap merupakan angka valid atau justru *error* / tidak valid.


* **`pinMode()`**: Menentukan perilaku suatu pin, contohnya mendeklarasikan pin aktuator sebagai jalur keluaran (`OUTPUT`).


* **`digitalWrite()`**: Memberikan tegangan elektrik spesifik (`HIGH` untuk 3.3V atau `LOW` untuk 0V) menuju pin aktuator.


* **`Serial.print()` & `Serial.println()**`: Menampilkan karakter teks dan isi variabel ke layar *Serial Monitor*.


* **`delay()`**: Menghentikan sementara alur pembacaan program sesuai nominal milidetik yang ditentukan.



**Penjelasan Percabangan atau Conditional**

Pada **Percobaan 1**, pengujian logika `isnan(kelembaban) || isnan(suhu)` diimplementasikan untuk mencegah anomali *output*. Jika minimal satu indikator gagal dikonversi, instruksi *if* akan mencetak notifikasi kegagalan membaca data sensor. Namun, jika data berhasil diakuisisi secara penuh, program masuk ke blok *else* guna mencetak angka suhu dan kelembaban aktual.

Pada **Percobaan 2**, program pertama-tama memverifikasi validitas data. Jika tidak ada *error*, nilai suhu dibandingkan menggunakan syarat `suhu > suhuThreshold`. Logika ini akan mengaktifkan relay saat suhu melewati angka 30.0 °C, dan mengembalikan status relay menjadi mati bila nilai suhu turun menyentuh atau berada di bawah angka 30.0 °C.

**Library atau Dependencies yang Diperlukan**

* Aplikasi Arduino IDE.


* Pustaka `DHT sensor library` (diintegrasikan dengan perintah `#include <DHT.h>`).


* Modul *board* ESP32 DevKit.


* Komponen pendeteksi DHT11.
* Aktuator berwujud Relay 1 *Channel* atau LED indikator.



**Jawaban Pertanyaan Praktikum yang Berkaitan dengan Code**

**Percobaan 1**

**1. Diagram Alur**
Berikut representasi alur untuk akuisisi data:
`[Masukkan Gambar Flowchart Percobaan 1]`

**2. Fungsi isnan()**
Perintah `isnan()` bertugas mendeteksi data cacat (*Not a Number*) akibat kegagalan sinyal konversi. Dengan menambahkan syarat penyaringan seperti `!isnan(suhu) && !isnan(kelembaban)`, program memastikan hanya data angka bulat atau desimal yang sah yang akan diproses lebih lanjut, misalnya untuk keperluan perhitungan rata-rata.

**3. Alasan Menggunakan delay()**
Sensor tipe DHT tidak didesain untuk merespons frekuensi pembacaan yang sangat ekstrem. Jeda interval sekitar 2 hingga 3 detik memberikan kompensasi waktu yang cukup bagi mikrokontroler internal sensor DHT untuk menyempurnakan penghitungan analitiknya. Jika dipaksa terlalu cepat, ESP32 berpotensi gagal merekam data (menghasilkan respons NaN) atau datanya menjadi sangat tidak stabil.

**4. Modifikasi Rata-rata Lima Pembacaan**
Berikut segmen program yang diubah untuk mengakumulasi lima data akurat sebelum mencetak rata-ratanya:

```cpp
float totalSuhu = 0;             // Penampung keseluruhan nilai suhu.
float totalKelembaban = 0;       // Penampung keseluruhan nilai kelembaban.
int pembacaanValid = 0;          // Penghitung total iterasi yang sukses.

for (int i = 0; i < 5; i++) {    // Mengulang siklus akuisisi lima kali berturut-turut.
    float suhu = dht.readTemperature();
    float kelembaban = dht.readHumidity();

    if (!isnan(suhu) && !isnan(kelembaban)) { // Blok filter khusus data valid.
        totalSuhu += suhu;                    // Akumulasi data suhu.
        totalKelembaban += kelembaban;        // Akumulasi data kelembaban.
        pembacaanValid++;                     // Tambah hitungan keberhasilan.
    }
    delay(2000);                              // Jeda aman antar pengukuran.
}

if (pembacaanValid > 0) {                     // Mencegah error pembagian dengan 0.
    float rataSuhu = totalSuhu / pembacaanValid;
    float rataKelembaban = totalKelembaban / pembacaanValid;
    Serial.print("Rata-rata Suhu: ");
    Serial.print(rataSuhu);
    Serial.print(" °C, Rata-rata Kelembaban: ");
    Serial.print(rataKelembaban);
    Serial.println(" %");
} else {
    Serial.println("Gagal membaca keseluruhan data sensor!");
}

```

Variabel `totalSuhu` dan `totalKelembaban` bertugas menghimpun nilai dari sensor secara akumulatif. Variabel `pembacaanValid` diterapkan sebagai angka penyebut pembagian, memastikan *error* (pembacaan yang gagal) tidak akan pernah merusak nilai rata-rata keseluruhan.

**Percobaan 2**

**1. Fungsi Nilai Threshold**
Ambang batas atau *threshold* difungsikan sebagai referensi baku yang menentukan kapan mikrokontroler mengambil alih peran mekanis. Berbekal parameter ini, ESP32 tidak hanya bertindak sebagai alat ukur semata, melainkan berevolusi menjadi instrumen otomasi yang responsif terhadap perubahan cuaca sekitarnya.

**2. Jika suhuThreshold Diturunkan Menjadi 20.0**
Lampu atau relai otomatis akan lebih konstan menyala. Hal ini disebabkan kondisi suhu ruangan tropis normal rata-rata selalu berada jauh di atas level 20.0 °C. Syarat komparasi akan senantiasa menunjukkan nilai *True*, sehingga status OFF hampir mustahil tercapai kecuali sistem diuji di ruangan bersuhu es.

**3. Perbedaan Kendali Tunggal dan Histerisis**
Kendali berbatas tunggal mengakibatkan perangkat beralih status secara drastis saat memotong satu titik referensi. Kekurangannya, jika temperatur berayun sedikit saja di dekat angka batas tersebut, lampu akan kedap-kedip berulang kali secara tidak terkontrol (efek *flickering*).
Strategi histerisis menyelesaikan ini dengan menetapkan margin batas ganda. Perangkat baru diaktifkan di suhu puncak (contoh 30 °C) dan dipadamkan pada suhu dasar (contoh 28 °C). Terdapat jeda penyangga (*deadband*) di antara angka 28-30 °C yang membuat usia pakai relay jauh lebih awet karena tidak terus-menerus disakelar.

**4. Modifikasi Program dengan Histerisis**
Berikut potongan logika program peredam *flickering* dengan pendekatan *deadband*:

```cpp
const float ambangAtas = 30.0;     // Suhu kritis untuk menyalakan beban listrik.
const float ambangBawah = 28.0;    // Suhu aman untuk mematikan beban listrik.
bool statusRelay = false;          // Perekam histori status sebelumnya.

float suhu = dht.readTemperature();

if (isnan(suhu)) {
    Serial.println("Kesalahan baca data sensor!");
} else {
    if (suhu > ambangAtas) {           // Bila melebihi puncak kritis.
        statusRelay = true;
    } else if (suhu < ambangBawah) {   // Bila kembali menyentuh batas bawah.
        statusRelay = false;
    }

    digitalWrite(RELAYPIN, statusRelay ? HIGH : LOW); // Update keluaran fisik pin.

    if (statusRelay) {
        Serial.println("Aktuator: Status ON");
    } else {
        Serial.println("Aktuator: Status OFF");
    }
}
delay(2000);                           // Kompensasi pembacaan hardware.

```

Fokus utama adalah pada variabel `statusRelay` yang bertugas merekam status listrik yang beroperasi saat itu. Di dalam zona penyangga suhu (28 hingga 30 °C), kondisi `if` maupun `else if` tidak ada yang memicu tindakan eksekusi baru, otomatis mikrokontroler menahan riwayat konfigurasi lampu yang sebelumnya sudah berjalan.

**Penjelasan Singkat Detail Percobaan**

Dalam Percobaan 1, proses akuisisi direpresentasikan oleh rangkaian fisik antara DHT11 dan ESP32. Begitu *script* dikompilasi ulang dan diunggah, perubahan metrik suhu terekam *real-time* ke antarmuka terminal.
Untuk Percobaan 2, fokus praktikum bergeser ke ranah pengendalian eksternal. Besaran parameter lingkungan dialihfungsikan menjadi acuan matematis yang secara otomatis memutuskan kapan ESP32 perlu menyuntikkan aliran listrik pada jalur aktuator relay.

**Skematik atau Diagram Rangkaian**

**Percobaan 1**

```
ESP32 DevKit          DHT11
3.3V / 3V3 ----------- VCC
GND ------------------ GND
GPIO 4 --------------- DATA

```

**Percobaan 2**

```
ESP32 DevKit          DHT11
3.3V / 3V3 ----------- VCC
GND ------------------ GND
GPIO 4 --------------- DATA

ESP32 DevKit          Relay / LED
5V / VIN ------------- VCC (Jika menggunakan Relay)
GND ------------------ GND
GPIO 26 -------------- IN / Anoda (melalui resistor)

```

Pemetaan pin konfigurasi di atas sejalan dengan inisialisasi di baris awal program. Rangkaian pasokan daya bisa sedikit bervariasi bergantung apakah praktikan menggunakan lampu LED atau modul Relay bertegangan 5V saat sesi berjalan.

**Foto Proses Praktikum atau Perangkaian**
`[Tambahkan tautan foto/video demonstrasi praktik di laboratorium]`
