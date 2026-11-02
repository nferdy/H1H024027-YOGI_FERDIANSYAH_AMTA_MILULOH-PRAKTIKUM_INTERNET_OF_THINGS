# Laporan Praktikum Internet of Things — Modul 5: Pengolahan dan Analisis Data IoT
**Pengolahan Data Edge (ESP8266) dan Server Processing (Python, MQTT, Moving Average, Z-Score, & Visualisasi)**

## Informasi Praktikan

| | |
|---|---|
| **Nama** | Yogi Ferdiansyah Amta Miluloh |
| **NIM** | H1H024027 |
| **Mata Kuliah** | Praktikum Internet Of Things (TK245005) |
| **Tahun / Semester** | 2026 / 5 (Genap) |
| **Modul** | Modul 5 — Pengolahan dan Analisis Data IoT |
| **Nama Asisten / NIM** | Athallah Tsany Satriyaji / H1D023013 |
| **Program Studi** | S1 Teknik Komputer |
| **Fakultas / Universitas** | Fakultas Teknik, Universitas Jenderal Soedirman |

## Daftar Isi

1. [Detail dan Ringkasan Percobaan](#1-detail-dan-ringkasan-percobaan)
   - 1.1 Tujuan Praktikum
   - 1.2 Dasar Teori
   - 1.3 Alat dan Bahan
2. [Library dan Dependencies yang Diperlukan](#2-library-dan-dependencies-yang-diperlukan)
3. [Percobaan 5A: Pengolahan Data Sensor pada ESP8266 (Moving Average & Statistik)](#3-percobaan-5a-pengolahan-data-sensor-pada-esp8266-moving-average--statistik)
   - 3.1 Penjelasan Kode Base Percobaan 5A
   - 3.2 Jawaban Pertanyaan Praktikum 5A
   - 3.3 Modifikasi Kode Percobaan 5A (Simpangan Baku / Standard Deviation)
   - 3.4 Penjelasan Baris demi Baris Kode Modifikasi 5A
   - 3.5 Penjelasan Fungsi yang Digunakan pada Percobaan 5A
   - 3.6 Penjelasan Percabangan / Conditional Logic pada Percobaan 5A
4. [Percobaan 5B: Akuisisi, Analisis, dan Visualisasi Data Suhu (MQTT & Server Processing)](#4-percobaan-5b-akuisisi-analisis-dan-visualisasi-data-suhu-mqtt--server-processing)
   - 4.1 Penjelasan Kode Base Percobaan 5B
   - 4.2 Ulasan Alur Pemrosesan Data pada Python
   - 4.3 Jawaban Pertanyaan Praktikum 5B
   - 4.4 Modifikasi Kode Percobaan 5B (Moving Average Sebelum Deteksi Z-Score)
   - 4.5 Penjelasan Baris demi Baris Kode Modifikasi 5B
   - 4.6 Penjelasan Fungsi yang Digunakan pada Percobaan 5B
   - 4.7 Penjelasan Percabangan / Conditional Logic pada Percobaan 5B
5. [Jawaban Pertanyaan Analisis](#5-jawaban-pertanyaan-analisis-bagian-vii-buku-catatan-praktikum)
6. [Catatan Kendala dan Solusi](#6-catatan-kendala-dan-solusi)
7. [Dokumentasi Praktikum](#7-dokumentasi-praktikum)

---

## 1. Detail dan Ringkasan Percobaan

### 1.1 Tujuan Praktikum

Berdasarkan modul praktikum dan Buku Catatan/Log Praktikum (BCP), tujuan praktikum Modul 5 adalah:

1. Memahami tahapan pengolahan dan analisis data pada sistem IoT (akuisisi, pra-pemrosesan, analisis, visualisasi).
2. Memahami perbedaan pengolahan data di sisi perangkat (*edge processing*) dan di sisi server/komputer (*server processing*).
3. Mengimplementasikan filter **Moving Average** dan perhitungan statistik sederhana (minimum, maksimum, simpangan baku) pada mikrokontroler.
4. Mengimplementasikan pengambilan data IoT melalui **MQTT** menggunakan Python dan menyimpannya ke file **CSV**.
5. Menganalisis dan memvisualisasikan data sensor serta mendeteksi anomali menggunakan metode **Z-Score**.

### 1.2 Dasar Teori

Data mentah dari sensor pada sistem IoT umumnya mengandung derau (*noise*), fluktuasi acak, dan terkadang lonjakan sesaat (*spike*). Data baru bernilai apabila diolah sehingga dapat dipakai untuk pengambilan keputusan, misalnya menentukan kondisi lingkungan atau memicu aktuator.

**A. Tahapan Pengolahan Data**

1. **Akuisisi data**: pengambilan data mentah dari sensor.
2. **Pra-pemrosesan**: pembersihan data (misalnya membuang NaN) dan penyaringan derau dengan filter.
3. **Pengolahan & analisis**: statistik deskriptif (rata-rata, min/max, simpangan baku) dan deteksi anomali.
4. **Visualisasi**: penyajian data dalam bentuk grafik agar tren dan anomali mudah diamati.

**B. Edge Processing vs Server Processing**

- **Edge Processing**: pengolahan dilakukan langsung pada mikrokontroler (ESP8266). Keunggulan: latensi rendah, menghemat bandwidth, tetap berjalan saat jaringan terputus. Keterbatasan: RAM dan daya komputasi kecil.
- **Server Processing**: pengolahan terpusat pada komputer/cloud (Python). Mampu menjalankan analisis kompleks, menyimpan data historis (CSV/SQL), dan membuat visualisasi.

**C. Filter Moving Average & Circular Buffer**

Moving Average (MA) menghitung rata-rata dari *N* sampel terakhir:

```
y[k] = (1/N) * Σ x[k-i]   ,  i = 0 ... N-1
```

Data disimpan pada **circular buffer** dengan operasi `(indeks + 1) % N` sehingga hemat memori dan setiap pembaruan berkompleksitas konstan O(1) untuk penulisan. Semakin besar *N*, data makin halus tetapi respons terhadap perubahan nyata makin lambat.

**D. Deteksi Anomali dengan Z-Score**

```
z = (x - μ) / σ
```

dengan μ = rata-rata sampel dan σ = simpangan baku sampel. Pada praktikum ini data dengan **|z| > 2** dianggap anomali. Berdasarkan *Empirical Rule* distribusi normal, ±2σ mencakup ±95% data, sehingga titik di luar batas tersebut hanya berpeluang muncul acak kurang dari 5%.

### 1.3 Alat dan Bahan

- **Hardware**: 1 unit board ESP8266 (NodeMCU / Wemos D1 Mini) dan 1 kabel USB (Micro-USB ke USB-A).
- **Software IDE**: Arduino IDE dengan paket board ESP8266.
- **Software Analisis**: Laptop/PC dengan Python 3 (pustaka `paho-mqtt`, `pandas`, `matplotlib`).
- **Jaringan**: WiFi terhubung ke internet.
- **Broker MQTT**: `broker.hivemq.com` (port 1883), topik `unsoed/tk245004/javier/data`.
- **Sumber data**: mock function `bacaSuhuMock()` sebagai pengganti sensor DHT22 (sensor fisik tidak tersedia), dipublikasikan oleh program Percobaan 2A Modul 4.

---

## 2. Library dan Dependencies yang Diperlukan

### A. Sisi Mikrokontroler (Arduino IDE)

| Library | Kegunaan |
|---|---|
| `<ESP8266WiFi.h>` | Pustaka dasar board ESP8266 (bawaan template). |
| `<math.h>` (bawaan) | Fungsi `sin()`, `pow()`, dan `sqrt()`. |

### B. Sisi Server / Komputer (Python)

Instalasi:

```bash
pip install paho-mqtt pandas matplotlib
```

| Library | Peran dan Kegunaan Teknis |
|---|---|
| `paho-mqtt` | Klien MQTT untuk *subscribe* data suhu dari broker HiveMQ. |
| `pandas` | DataFrame untuk manipulasi data tabular, ekspor/impor CSV, statistik (mean, std), dan `rolling()`. |
| `matplotlib` | Pembuatan grafik runtun waktu dan penandaan titik anomali. |
| `json`, `time` (bawaan) | Parsing payload JSON dan pencatatan waktu penerimaan. |

---

## 3. Percobaan 5A: Pengolahan Data Sensor pada ESP8266 (Moving Average & Statistik)

### 3.1 Penjelasan Kode Base Percobaan 5A

File implementasi: `modul5_pengolahan_data.ino`

Program membaca suhu dari mock function setiap 2 detik, menyaringnya dengan Moving Average N = 5, mencatat suhu minimum dan maksimum sejak program berjalan, lalu menampilkan semuanya di Serial Monitor (115200 baud).

```cpp
const int N = 5;
float buffer[N];
int indeks = 0;
int jumlah = 0;

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Mock function pengganti sensor DHT22
float bacaSuhuMock() {
  static unsigned long jumlahBaca = 0;
  jumlahBaca++;
  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
  float noise = random(-5, 6) * 0.1;
  float suhu = suhuDasar + noise;
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;            // simulasi anomali
  }
  return suhu;
}

// Fungsi penghalus data (Moving Average)
float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  if (jumlah < N) jumlah++;

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  return total / jumlah;
}
```

Penjelasan ringkas:

- `bacaSuhuMock()` membangkitkan suhu sintetis: gelombang sinus (28 ± 2 °C) + noise acak ±0,5 °C + lonjakan +8 °C setiap pembacaan kelipatan 25.
- `hitungMovingAverage()` menulis nilai baru ke `buffer[]` secara melingkar, lalu merata-ratakan `jumlah` data yang sudah terisi.
- `loop()` memanggil kedua fungsi, memperbarui `suhuMin`/`suhuMax`, dan mencetak hasil.

### 3.2 Jawaban Pertanyaan Praktikum 5A

**1. Diagram alur (flowchart) proses perhitungan Moving Average pada `hitungMovingAverage()`**

```
 [ Mulai ]
     │
 [ Terima nilaiBaru ]
     │
 [ buffer[indeks] = nilaiBaru ]
     │
 [ indeks = (indeks + 1) % N ]
     │
    / \
   /   \  Ya
  < jumlah < N ? > ───────────┐
   \   /                      │
    \ / Tidak                 ▼
     │                   [ jumlah++ ]
     │                        │
     └────────────────────────┴──► [ total = 0 ]
                                       │
                                   [ Loop i = 0 s.d. jumlah-1 ]
                                   [ total += buffer[i]       ]
                                       │
                                   [ rata-rata = total / jumlah ]
                                       │
                                   [ Return rata-rata ]
```

**2. Fungsi operasi `(indeks + 1) % N`**

Operasi modulo membuat array `buffer` bekerja sebagai **circular buffer**. Dengan N = 5, `indeks` berjalan 0, 1, 2, 3, 4, lalu kembali ke 0 karena (4 + 1) % 5 = 0. Akibatnya indeks tidak pernah keluar batas array (*out of bounds*) dan data terbaru otomatis menimpa data paling lama tanpa perlu menggeser seluruh isi array.

**3. Dampak ukuran jendela N (N = 20 vs N = 2)**

Lonjakan anomali pada mock function sebesar +8 °C. Pada MA, satu lonjakan menaikkan rata-rata sebesar 8/N:

| Ukuran N | Cakupan waktu (interval 2 s) | Kenaikan MA akibat spike | Karakteristik |
|---|---|---|---|
| N = 2 | 4 detik | ≈ 4,0 °C | Respons sangat cepat, tetapi derau dan anomali nyaris tidak teredam. |
| N = 5 (praktikum) | 10 detik | ≈ 1,6 °C | Kompromi antara kehalusan dan kecepatan respons. |
| N = 20 | 40 detik | ≈ 0,4 °C | Kurva sangat halus, tetapi *lagging* (keterlambatan respons) tinggi terhadap perubahan suhu nyata. |

### 3.3 Modifikasi Kode Percobaan 5A (Simpangan Baku / Standard Deviation)

**Soal:** *Modifikasi program agar ESP8266 juga menghitung simpangan baku dari data pada buffer.*

File implementasi: `modul5_pengolahan_data.ino` (bagian bertanda `[BARIS TAMBAHAN]`)

```cpp
// [BARIS TAMBAHAN] Fungsi untuk menghitung simpangan baku (Standard Deviation)
float hitungSimpanganBaku(float rataRata) {
  float totalVarians = 0;
  for (int i = 0; i < jumlah; i++) {
    totalVarians += pow(buffer[i] - rataRata, 2);
  }
  return sqrt(totalVarians / jumlah);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Memulai simulasi pembacaan suhu pada ESP8266...");
}

void loop() {
  float suhu = bacaSuhuMock();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu);

    // [BARIS TAMBAHAN] Memanggil fungsi hitung simpangan baku
    float simpanganBaku = hitungSimpanganBaku(suhuFilter);

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");   Serial.print(suhu);
    Serial.print(" | Filter: "); Serial.print(suhuFilter);

    // [BARIS TAMBAHAN] Menampilkan hasil ke Serial Monitor
    Serial.print(" | StdDev: "); Serial.print(simpanganBaku);

    Serial.print(" | Min: ");    Serial.print(suhuMin);
    Serial.print(" | Max: ");    Serial.println(suhuMax);
  }

  delay(2000);
}
```

### 3.4 Penjelasan Baris demi Baris Kode Modifikasi 5A

| Baris | Penjelasan |
|---|---|
| `float hitungSimpanganBaku(float rataRata) {` | Mendeklarasikan fungsi baru yang menerima rata-rata buffer (hasil Moving Average) dan mengembalikan simpangan baku bertipe `float`. |
| `float totalVarians = 0;` | Variabel akumulator untuk menjumlahkan kuadrat selisih tiap data terhadap rata-rata. |
| `for (int i = 0; i < jumlah; i++) {` | Mengiterasi hanya sebanyak `jumlah` data yang sudah terisi, sehingga benar saat buffer belum penuh. |
| `totalVarians += pow(buffer[i] - rataRata, 2);` | Menghitung selisih data ke-*i* terhadap rata-rata, mengkuadratkannya dengan `pow()`, lalu menambahkannya ke akumulator. |
| `return sqrt(totalVarians / jumlah);` | Membagi total dengan `jumlah` (varians populasi), lalu mengambil akar kuadratnya dengan `sqrt()` sehingga diperoleh simpangan baku σ. |
| `float simpanganBaku = hitungSimpanganBaku(suhuFilter);` | Di `loop()`, memanggil fungsi di atas dengan `suhuFilter` (rata-rata buffer) sebagai parameter. Harus dipanggil **setelah** `hitungMovingAverage()` agar buffer sudah diperbarui. |
| `Serial.print(" \| StdDev: "); Serial.print(simpanganBaku);` | Mencetak label dan nilai simpangan baku ke Serial Monitor di antara kolom Filter dan Min. |

> **Catatan:** pada pembacaan pertama `jumlah = 1`, sehingga simpangan baku = 0. Fungsi memakai pembagi `jumlah` (varians populasi), sedangkan `pandas.std()` pada Python memakai pembagi `n-1` (varians sampel), sehingga nilainya sedikit berbeda untuk data yang sama.

### 3.5 Penjelasan Fungsi yang Digunakan pada Percobaan 5A

| Nama Fungsi | Parameter | Return | Kegunaan |
|---|---|---|---|
| `bacaSuhuMock()` | – | `float` | Menghasilkan sinyal suhu sintetis (sinus + noise acak + anomali tiap kelipatan 25). |
| `hitungMovingAverage()` | `float nilaiBaru` | `float` | Memperbarui circular buffer dan mengembalikan rata-rata bergerak N = 5. |
| `hitungSimpanganBaku()` | `float rataRata` | `float` | Menghitung simpangan baku data pada buffer (fungsi tambahan modifikasi). |
| `sin()` | `double` | `double` | Nilai gelombang sinus untuk suhu dasar. |
| `random(min, max)` | `long, long` | `long` | Angka acak dari `min` sampai `max-1` untuk noise. |
| `pow()` | `double, double` | `double` | Memangkatkan selisih (kuadrat) pada perhitungan varians. |
| `sqrt()` | `double` | `double` | Akar kuadrat dari varians. |
| `isnan()` | `float` | `bool` | Memeriksa apakah pembacaan tidak valid (NaN). |

### 3.6 Penjelasan Percabangan / Conditional Logic pada Percobaan 5A

- `if (jumlahBaca % 25 == 0)`: menyuntikkan lonjakan +8 °C pada setiap pembacaan kelipatan 25 (simulasi anomali).
- `if (jumlah < N) jumlah++;`: menjamin rata-rata awal memakai jumlah data aktual sebelum buffer terisi penuh 5 elemen.
- `if (isnan(suhu)) { ... } else { ... }`: bila pembacaan gagal, data diabaikan dan tidak masuk ke filter/statistik; bila valid, diproses.
- `if (suhu < suhuMin) suhuMin = suhu;` dan `if (suhu > suhuMax) suhuMax = suhu;`: memperbarui nilai ekstrem global dari suhu mentah terbaru.

### 3.7 Hasil Pengamatan Percobaan 5A

**Tabel 5.1. Hasil Pengolahan Data Suhu pada ESP8266 (cuplikan 10 data awal)**

| No | Waktu Pengiriman (s) | Suhu Mentah (°C) | Moving Average (°C) | Suhu Min (°C) | Suhu Max (°C) |
|---|---|---|---|---|---|
| 1 | 2 | 28.73 | 28.52 | 27.86 | 28.82 |
| 2 | 4 | 28.78 | 28.57 | 27.86 | 28.82 |
| 3 | 6 | 28.62 | 28.73 | 27.86 | 28.82 |
| 4 | 8 | 29.36 | 28.83 | 27.86 | 29.36 |
| 5 | 10 | 29.49 | 29.00 | 27.86 | 29.49 |
| 6 | 12 | 29.82 | 29.22 | 27.86 | 29.82 |
| 7 | 14 | 29.83 | 29.43 | 27.86 | 29.83 |
| 8 | 16 | 29.24 | 29.55 | 27.86 | 29.83 |
| 9 | 18 | 29.24 | 29.53 | 27.86 | 29.83 |
| 10 | 20 | 29.22 | 29.47 | 27.86 | 29.83 |

Pengamatan: nilai Moving Average bergerak lebih halus dibanding suhu mentah, dan nilai Min/Max hanya berubah saat muncul nilai mentah yang melampaui ekstrem sebelumnya (contoh: Max naik dari 28.82 menjadi 29.36 pada data ke-4). Hasil sesuai spesifikasi percobaan.

---

## 4. Percobaan 5B: Akuisisi, Analisis, dan Visualisasi Data Suhu (MQTT & Server Processing)

### 4.1 Penjelasan Kode Base Percobaan 5B

File implementasi: `modul5_analisis_data.py`

Program Python *subscribe* ke topik `unsoed/tk245004/javier/data`, mengumpulkan 30 sampel suhu beserta waktu penerimaannya, menyimpannya ke `data_suhu.csv`, menghitung statistik dan Z-score, lalu membuat grafik dengan penanda anomali. Kode base (sebelum modifikasi) berikut mengikuti modul:

```python
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "unsoed/tk245004/javier/data"
JUMLAH_SAMPEL = 30

data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
        data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
        print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")
        if len(data) >= JUMLAH_SAMPEL:
            client.disconnect()
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

# -- Tahap 1: akuisisi data dari MQTT
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# -- Tahap 2: simpan ke CSV
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# -- Tahap 3: analisis statistik dan deteksi anomali (z-score)
rata2 = df["suhu"].mean()
std = df["suhu"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std
df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata : {rata2:.2f}")
print(f"Std deviasi: {std:.2f}")
print(f"Minimum : {df['suhu'].min():.2f}")
print(f"Maksimum : {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")

# -- Tahap 4: visualisasi
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", label="Suhu")
anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu dan Deteksi Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png")
plt.show()
```

### 4.2 Ulasan Alur Pemrosesan Data pada Python

1. **Akuisisi MQTT**: `on_connect` men-*subscribe* topik, `on_message` mem-*parse* payload JSON, mengambil field `suhu`, dan menyimpannya bersama stempel waktu penerimaan. Setelah 30 sampel terkumpul, koneksi diputus (`client.disconnect()`) sehingga `loop_forever()` berhenti.
2. **Penyimpanan tabular**: list `data` diubah menjadi `DataFrame` dan diekspor ke `data_suhu.csv`.
3. **Analisis statistik & Z-Score**: menghitung mean dan standard deviation, lalu Z-score tiap baris. Baris dengan |z| > 2 diberi flag `anomali = True`.
4. **Visualisasi**: suhu digambar sebagai grafik garis, garis putus-putus abu-abu menunjukkan rata-rata, dan titik anomali ditandai lingkaran merah besar (`s=100`).

**Hasil eksekusi pertama (data base, grafik `grafik_suhu.png`):**

![Grafik Suhu dan Deteksi Anomali](grafik_suhu.png)

Pada eksekusi ini terdeteksi **2 anomali**: data ke-3 (37,82 °C, Z = +3,87) dan satu lonjakan lain di sekitar data ke-28 (±34,1 °C). Keduanya berasal dari injeksi lonjakan +8 °C oleh mock function.

### 4.3 Jawaban Pertanyaan Praktikum 5B

**1. Fungsi blok `try-except` pada `on_message()` dan akibat jika dihilangkan**

- **Fungsi:** menangani galat saat memproses pesan MQTT. Bila payload bukan JSON valid (`json.loads` melempar `JSONDecodeError`, turunan dari `ValueError`), tidak memiliki key `"suhu"` (`KeyError`), atau nilainya tidak bisa dikonversi ke `float` (`ValueError`), program hanya mencetak "Data tidak valid, diabaikan" dan melanjutkan menunggu pesan berikutnya.
- **Jika dihilangkan:** satu pesan rusak/tidak sesuai format akan memicu *unhandled exception* di dalam callback, sehingga program Python berhenti sebelum 30 sampel terkumpul dan data yang sudah diterima tidak tersimpan ke CSV.

**2. Arti nilai Z-score dan alasan batas |z| > 2**

- **Arti:** Z-score menyatakan seberapa jauh suatu data menyimpang dari rata-rata dalam satuan simpangan baku. Z = +3,87 berarti data berada 3,87σ di atas rata-rata.
- **Alasan batas 2:** pada distribusi normal, sekitar 95% data berada di rentang −2 ≤ z ≤ +2. Data di luar rentang ini hanya berpeluang muncul acak kurang dari 5%, sehingga secara statistik tidak wajar dan dikategorikan sebagai anomali. Batas ini juga kompromi yang sederhana: lebih kecil (misal 1) terlalu banyak *false positive*, lebih besar (misal 3) berisiko melewatkan anomali.

**3. Kelemahan Z-score pada data dengan tren naik/turun yang tajam**

Z-score membandingkan setiap nilai terhadap mean dan σ **global** seluruh data, sehingga:

- Data normal di awal (rendah) dan di akhir (tinggi) yang terpaut jauh dari rata-rata global dapat keliru ditandai anomali (*false positive*), padahal itu hanya bagian dari tren.
- Tren yang lebar memperbesar σ global, sehingga anomali lokal berskala kecil justru lolos (*false negative*).
- Mean dan σ sendiri sensitif terhadap nilai ekstrem (efek *masking*): pada data kita, lonjakan 38,02 °C ikut menaikkan σ yang dipakai untuk menilai dirinya sendiri.

Solusinya antara lain memakai Z-score pada jendela bergerak (*rolling z-score*), *detrending* terlebih dahulu, atau metode yang lebih kokoh seperti median/MAD.

### 4.4 Modifikasi Kode Percobaan 5B (Moving Average Sebelum Deteksi Z-Score)

**Soal:** *Modifikasi program agar menggunakan metode moving average (jendela 5 data) pada data hasil CSV sebelum dihitung z-score-nya, kemudian bandingkan jumlah anomali yang terdeteksi.*

File implementasi: `modul5_analisis_data.py` (bagian bertanda `[BARIS TAMBAHAN]`), menghasilkan `grafik_suhu_ma.png`.

```python
# -- Tahap 2: simpan ke CSV
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# [BARIS TAMBAHAN 1] Membaca kembali data dari file CSV yang telah disimpan
df = pd.read_csv("data_suhu.csv")

# [BARIS TAMBAHAN 2] Menghitung Moving Average dengan jendela 5 data
df["suhu_ma"] = df["suhu"].rolling(window=5, min_periods=1).mean()

# -- Tahap 3: analisis statistik dan deteksi anomali (z-score) pada data Moving Average
# [BARIS TAMBAHAN 3] Menghitung rata-rata dan simpangan baku dari kolom hasil Moving Average
rata2 = df["suhu_ma"].mean()
std = df["suhu_ma"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    # [BARIS TAMBAHAN 4] Menghitung nilai Z-score menggunakan data suhu_ma
    df["zscore"] = (df["suhu_ma"] - rata2) / std

df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata (MA-5): {rata2:.2f}")
print(f"Std deviasi (MA-5): {std:.2f}")
print(f"Minimum (MA-5): {df['suhu_ma'].min():.2f}")
print(f"Maksimum (MA-5): {df['suhu_ma'].max():.2f}")
print(f"Jumlah anomali (MA-5): {df['anomali'].sum()}")

# -- Tahap 4: visualisasi
plt.figure(figsize=(10, 4))
# [BARIS TAMBAHAN 5] Memplot perbandingan suhu mentah dan suhu hasil Moving Average
plt.plot(df.index, df["suhu"], marker="o", alpha=0.4, label="Suhu Mentah")
plt.plot(df.index, df["suhu_ma"], marker="s", color="blue", label="Moving Average (N=5)")

anom = df[df["anomali"]]
# [BARIS TAMBAHAN 6] Menandai titik anomali pada kurva Moving Average
plt.scatter(anom.index, anom["suhu_ma"], color="red", s=100, label="Anomali (MA-5)", zorder=3)

plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata (MA-5)")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu Moving Average (N=5) dan Deteksi Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu_ma.png")
plt.show()
```

![Grafik Moving Average dan Deteksi Anomali](grafik_suhu_ma.png)

**Perbandingan jumlah anomali (dihitung dari `data_suhu.csv`, 30 sampel):**

| Parameter | Z-score pada data mentah | Z-score setelah Moving Average (N=5) |
|---|---|---|
| Rata-rata | 29.64 °C | 29.72 °C |
| Simpangan baku | 1.74 °C | 0.98 °C |
| Minimum / Maksimum | 27.80 / 38.02 °C | 28.18 / 31.51 °C |
| |Z| maksimum | 4.82 | 1.83 |
| **Jumlah anomali (|z| > 2)** | **1** (data ke-15, 38.02 °C) | **0** |

Lonjakan 38,02 °C teredam menjadi sekitar 31,4–31,5 °C pada MA (naik ±1,6 °C ≈ 8/5), sehingga tidak lagi melewati ambang |z| > 2. Efek sampingnya, lonjakan sesaat itu "menyebar" menjadi dataran tinggi selama 5 sampel berturut-turut (data ke-15 sampai ke-19) pada kurva MA.

### 4.5 Penjelasan Baris demi Baris Kode Modifikasi 5B

| Baris | Penjelasan |
|---|---|
| `df = pd.read_csv("data_suhu.csv")` **[1]** | Membaca kembali data dari file CSV yang baru disimpan, sehingga analisis dilakukan pada data hasil CSV sesuai soal. |
| `df["suhu_ma"] = df["suhu"].rolling(window=5, min_periods=1).mean()` **[2]** | Membuat kolom baru berisi rata-rata bergerak jendela 5 data. `min_periods=1` membuat 4 data awal tetap dihitung dari data yang tersedia (tidak menjadi NaN). |
| `rata2 = df["suhu_ma"].mean()` dan `std = df["suhu_ma"].std()` **[3]** | Rata-rata dan simpangan baku kini dihitung dari kolom `suhu_ma`, bukan data mentah. |
| `df["zscore"] = (df["suhu_ma"] - rata2) / std` **[4]** | Z-score dihitung dari data yang sudah difilter. Percabangan `if std == 0` tetap mencegah pembagian dengan nol. |
| `plt.plot(df.index, df["suhu"], ..., alpha=0.4, ...)` dan `plt.plot(df.index, df["suhu_ma"], ..., color="blue", ...)` **[5]** | Menggambar suhu mentah (transparan) dan kurva Moving Average (biru) pada satu grafik agar efek penyaringan terlihat. |
| `plt.scatter(anom.index, anom["suhu_ma"], color="red", ...)` **[6]** | Menandai titik anomali pada kurva MA. Karena tidak ada anomali, tidak ada titik merah yang tergambar (hanya muncul di legenda). |

### 4.6 Penjelasan Fungsi yang Digunakan pada Percobaan 5B

| Nama Fungsi | Pustaka | Kegunaan |
|---|---|---|
| `mqtt.Client()` | paho-mqtt | Membuat objek klien MQTT (callback API versi 2). |
| `client.connect()` / `client.subscribe()` | paho-mqtt | Terhubung ke broker dan berlangganan topik data. |
| `client.loop_forever()` / `client.disconnect()` | paho-mqtt | Menjalankan loop jaringan secara blocking hingga koneksi diputus setelah 30 sampel. |
| `json.loads()` | json | Mengubah payload JSON menjadi dictionary Python. |
| `time.strftime()` | time | Memformat waktu penerimaan data. |
| `pd.DataFrame()` | pandas | Mengubah list of dict menjadi tabel dua dimensi. |
| `df.to_csv()` / `pd.read_csv()` | pandas | Menyimpan dan membaca kembali data dari file CSV. |
| `df["col"].mean()` / `.std()` | pandas | Rata-rata dan simpangan baku sampel (pembagi n−1). |
| `df["col"].rolling(window).mean()` | pandas | Moving Average pada deret waktu. |
| `plt.plot()` / `plt.scatter()` / `plt.axhline()` | matplotlib | Menggambar garis data, titik anomali, dan garis rata-rata. |
| `plt.savefig()` / `plt.show()` | matplotlib | Menyimpan grafik ke PNG dan menampilkannya. |

### 4.7 Penjelasan Percabangan / Conditional Logic pada Percobaan 5B

- `try ... except (ValueError, KeyError)` pada `on_message()`: melewati pesan yang tidak valid tanpa menghentikan program.
- `if len(data) >= JUMLAH_SAMPEL: client.disconnect()`: menghentikan akuisisi setelah 30 sampel terkumpul.
- `if std == 0: ... else: ...`: mencegah *division by zero* bila semua data identik.
- `df["zscore"].abs() > 2`: menghasilkan flag boolean per baris untuk kolom `anomali`.
- `anom = df[df["anomali"]]`: memfilter baris anomali agar hanya titik tersebut yang digambar merah.

### 4.8 Hasil Pengamatan Percobaan 5B

**Tabel 5.2. Hasil Akuisisi dan Analisis Data Suhu melalui MQTT (20 data, eksekusi pertama)**

| No | Waktu Penerimaan | Suhu (°C) | Z-score | Status Anomali | Status Penyimpanan CSV |
|---|---|---|---|---|---|
| 1 | 2026-10-21 11:55:14 | 29.93 | 0.51 | Tidak | Berhasil |
| 2 | 2026-10-21 11:55:16 | 29.88 | 0.49 | Tidak | Berhasil |
| 3 | 2026-10-21 11:55:18 | 37.82 | 3.87 | **Ya** | Berhasil |
| 4 | 2026-10-21 11:55:20 | 29.35 | 0.27 | Tidak | Berhasil |
| 5 | 2026-10-21 11:55:22 | 29.96 | 0.53 | Tidak | Berhasil |
| 6 | 2026-10-21 11:55:24 | 29.37 | 0.28 | Tidak | Berhasil |
| 7 | 2026-10-21 11:55:26 | 29.16 | 0.19 | Tidak | Berhasil |
| 8 | 2026-10-21 11:55:28 | 29.75 | 0.44 | Tidak | Berhasil |
| 9 | 2026-10-21 11:55:30 | 29.43 | 0.30 | Tidak | Berhasil |
| 10 | 2026-10-21 11:55:32 | 29.60 | 0.37 | Tidak | Berhasil |
| 11 | 2026-10-21 11:55:34 | 28.76 | 0.02 | Tidak | Berhasil |
| 12 | 2026-10-21 11:55:36 | 29.22 | 0.21 | Tidak | Berhasil |
| 13 | 2026-10-21 11:55:38 | 28.77 | 0.02 | Tidak | Berhasil |
| 14 | 2026-10-21 11:55:40 | 28.02 | -0.30 | Tidak | Berhasil |
| 15 | 2026-10-21 11:55:42 | 28.16 | -0.24 | Tidak | Berhasil |
| 16 | 2026-10-21 11:55:44 | 28.10 | -0.26 | Tidak | Berhasil |
| 17 | 2026-10-21 11:55:46 | 27.64 | -0.46 | Tidak | Berhasil |
| 18 | 2026-10-21 11:55:48 | 27.78 | -0.40 | Tidak | Berhasil |
| 19 | 2026-10-21 11:55:50 | 28.22 | -0.21 | Tidak | Berhasil |
| 20 | 2026-10-21 11:55:52 | 27.87 | -0.36 | Tidak | Berhasil |

> Tabel 5.2 dan grafik `grafik_suhu.png` berasal dari eksekusi pertama (pukul 11:55). Eksekusi kedua (pukul 12:09) menghasilkan `data_suhu.csv` yang dipakai pada modifikasi Moving Average (`grafik_suhu_ma.png`), sehingga nilai sampelnya berbeda karena lonjakan muncul pada posisi data yang berbeda.

---

## 5. Jawaban Pertanyaan Analisis (Bagian VII Buku Catatan Praktikum)

**1. Uraian hasil tugas pada praktikum yang telah dilakukan**

- **Percobaan 5A:** ESP8266 berhasil membaca data suhu dari mock function setiap 2 detik, menyaringnya dengan Moving Average N = 5, mencatat Min/Max, dan (pada modifikasi) menghitung simpangan baku buffer. Serial Monitor menampilkan data mentah, hasil filter, StdDev, Min, dan Max sesuai spesifikasi. Hasil filter terlihat lebih halus daripada data mentah.
- **Percobaan 5B:** Python berhasil menerima 30 sampel suhu via MQTT dari broker HiveMQ, menyimpannya ke `data_suhu.csv` beserta waktu penerimaan, menghitung statistik, mendeteksi anomali dengan Z-score, dan menghasilkan grafik. Pada eksekusi pertama terdeteksi 2 anomali (Z maksimum +3,87). Pada modifikasi dengan Moving Average (N = 5), Z maksimum turun menjadi 1,83 sehingga jumlah anomali menjadi 0.

**2. Komparasi Edge Processing (5A) vs Server Processing (5B)**

| Parameter | Edge Processing (ESP8266 / 5A) | Server Processing (Python / 5B) |
|---|---|---|
| Lokasi komputasi | Mikrokontroler lokal | Komputer lokal / cloud |
| Kecepatan / latensi | Sangat cepat, tanpa latensi jaringan, independen dari koneksi | Bergantung pada latensi jaringan dan broker (data diterima tiap ±2 detik) |
| Kompleksitas analisis | Terbatas (Moving Average, Min/Max, StdDev sederhana) | Tinggi (Z-score, `rolling`, visualisasi, dapat diperluas ke machine learning) |
| Kebutuhan sumber daya | RAM dan flash sangat terbatas | RAM gigabyte dan penyimpanan besar |
| Penyimpanan data | Tidak ada penyimpanan historis | Dapat disimpan jangka panjang (CSV/database) |
| Konsumsi bandwidth | Hemat bila hanya mengirim data terfilter | Lebih besar bila semua data mentah dikirim |

**3. Pengaruh ukuran jendela Moving Average terhadap kualitas data dan respons aktuator**

- **Jendela besar (misal N = 20):** data sangat halus dan stabil terhadap derau, tetapi respons terhadap perubahan nyata lambat (*lag*). Pada sistem seperti alarm kebakaran atau pendingin, aktuator terlambat bereaksi.
- **Jendela kecil (misal N = 2):** respons cepat, tetapi derau dan lonjakan sesaat langsung terbawa ke keluaran sehingga aktuator bisa sering hidup-mati (*chattering*) dan mempercepat keausan perangkat.
- **Bukti dari data praktikum:** dengan N = 5, lonjakan 38,02 °C hanya menaikkan MA menjadi ±31,5 °C dan efeknya bertahan selama 5 sampel. Artinya filter meredam spike, namun aktuator yang berbasis MA tetap "melihat" sisa lonjakan selama ±10 detik.
- **Kesimpulan:** N = 5 merupakan kompromi yang wajar; pemilihan N harus disesuaikan dengan seberapa cepat sistem harus merespons.

**4. Pemanfaatan analisis anomali untuk meningkatkan keandalan sistem IoT**

- **Smart farming:** membedakan suhu yang benar-benar naik (misal perlu menyalakan penyiram) dari lonjakan sesaat akibat error sensor atau noise, sehingga aktuator tidak aktif secara keliru; juga dapat menandai sensor kelembaban yang rusak atau saluran irigasi bermasalah.
- **Smart home:** mendeteksi kenaikan suhu atau lonjakan arus yang tidak wajar untuk peringatan dini kebakaran, dan memutus saklar utama secara otomatis.

---

## 6. Catatan Kendala dan Solusi

**Ketiadaan sensor fisik DHT22**
- *Kendala:* sensor DHT22 fisik tidak tersedia di laboratorium.
- *Solusi:* memakai mock function `bacaSuhuMock()` (sinus + noise acak + injeksi anomali) pada ESP8266 sesuai arahan asisten praktikum.

**Sinkronisasi topik MQTT**
- *Kendala:* topik pada publisher (Arduino) dan subscriber (Python) harus sama persis agar data diterima.
- *Solusi:* menyamakan nilai `TOPIC` menjadi `unsoed/tk245004/javier/data` pada kedua sisi, sehingga 30 sampel tercatat lengkap di `data_suhu.csv`.

**Nilai tidak valid / noise berlebih**
- *Kendala:* pembacaan sensor bisa gagal (NaN) atau mengandung lonjakan.
- *Solusi:* pengecekan `isnan()` pada mikrokontroler, filter Moving Average pada sisi edge, serta `try-except` dan Z-score pada sisi Python.

**Data CSV tertimpa pada eksekusi ulang**
- *Kendala:* `data_suhu.csv` ditulis ulang setiap program dijalankan, sehingga data eksekusi pertama (untuk Tabel 5.2) tidak sama dengan data eksekusi kedua (untuk modifikasi MA).
- *Solusi:* hasil eksekusi pertama didokumentasikan lewat `grafik_suhu.png` dan Tabel 5.2, sedangkan CSV final dipakai sebagai dasar perbandingan pada bagian 4.4.

---


```

Tautan dokumentasi foto dan video (Google Drive): *(isi tautan di sini)*
