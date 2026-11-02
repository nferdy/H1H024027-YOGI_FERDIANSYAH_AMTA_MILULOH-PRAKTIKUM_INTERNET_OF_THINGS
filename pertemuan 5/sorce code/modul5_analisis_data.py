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