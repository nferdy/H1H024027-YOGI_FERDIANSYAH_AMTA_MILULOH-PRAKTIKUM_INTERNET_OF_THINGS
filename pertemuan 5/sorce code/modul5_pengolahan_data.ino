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
        suhu += 8.0;
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

        Serial.print("Mentah: ");
        Serial.print(suhu);
        Serial.print(" | Filter: ");
        Serial.print(suhuFilter);
        
        // [BARIS TAMBAHAN] Menampilkan hasil ke Serial Monitor
        Serial.print(" | StdDev: "); 
        Serial.print(simpanganBaku); 

        Serial.print(" | Min: ");
        Serial.print(suhuMin);
        Serial.print(" | Max: ");
        Serial.println(suhuMax);
    }

    delay(2000);
}