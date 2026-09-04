#include <DHT.h>
#define DHTPIN D4 // pin data DHT11 terhubung ke D4 pada NodeMCU
#define DHTTYPE DHT11
#define RELAYPIN D1 // pin kendali relay/LED indikator

DHT dht(DHTPIN, DHTTYPE);
const float suhuThreshold = 30.0; 

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
}
void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" C -> ");
  
    if (suhu > suhuThreshold) {
      digitalWrite(RELAYPIN, HIGH); 
      Serial.println("Aktuator: ON");
    } else {
      digitalWrite(RELAYPIN, LOW);
      Serial.println("Aktuator: OFF");
    }
  }
}
