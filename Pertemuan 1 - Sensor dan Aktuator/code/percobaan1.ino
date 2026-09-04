#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Memulai DHT11...");

  dht.begin();
  delay(2000);
}

void loop() {
  float kelembaban = dht.readHumidity();
  float suhu = dht.readTemperature();

  if (isnan(kelembaban) || isnan(suhu)) {
    Serial.println("Gagal membaca data dari sensor DHT11!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C | Kelembaban: ");
    Serial.print(kelembaban);
    Serial.println(" %");
  }

  delay(3000);
}
