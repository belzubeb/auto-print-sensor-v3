# Auto Print Sensor V3 (Arduino)

Firmware **Arduino Uno** untuk mendeteksi objek yang lewat di jalur produksi, dipakai sebagai pemicu otomatis proses cetak label (auto print).

Sensor yang dipakai: **Autonics BEN300-DFR** (photoelectric, output PNP/NPN sesuai wiring di bawah).

---

## Wiring

| Kabel sensor | Warna | Tujuan |
| :--- | :--- | :--- |
| BN | Cokelat | (+) PSU 24V |
| BU | Biru | (-) PSU 24V |
| WH | Putih | GND Arduino |
| BK | Hitam | D2 Arduino |

Logika yang sudah teruji (bukan dibalik):

- `pinMode(D2, INPUT_PULLUP)`
- **standby = LOW (0)**, **ada objek = HIGH (1)**

---

## Cara kerja

Sketch memakai pola **arm / disarm**, bukan debounce biasa:

1. Saat sinyal naik ke `HIGH` **dan** sensor dalam kondisi *armed* → kirim `HIT` lewat Serial (9600 baud), lalu sensor **dikunci** (disarmed).
2. Sensor tidak akan mengirim `HIT` lagi sampai sinyal benar-benar kembali ke `LOW` **dan stabil** selama `REARM_LOW_MS` (150 ms).
3. Setelah stabil `LOW`, sensor *di-arm* ulang untuk menangkap objek berikutnya.

Kenapa begitu: transisi "objek datang" (`LOW → HIGH`) ditangkap **instan** supaya pulsa pendek tetap terhitung, sedangkan transisi "objek pergi" (`HIGH → LOW`) yang di-debounce supaya sinyal mentah yang sempat blip tidak dihitung sebagai objek baru.

---

## Parameter

| Konstanta | Nilai | Keterangan |
| :--- | :--- | :--- |
| `sensorPin` | `2` | Pin input sensor (D2) |
| `REARM_LOW_MS` | `150` | Waktu sinyal harus stabil LOW sebelum boleh hit lagi |

---

## Output

Setiap objek terdeteksi mengirim satu baris ke Serial:

```
HIT
```

---

## Upload

1. Buka `auto_print_sensorv3.ino` di **Arduino IDE**.
2. Pilih board **Arduino Uno** dan port yang sesuai.
3. Klik **Upload**.

## Riwayat revisi

- **Revisi 2**: ganti edge-trigger + cooldown 50 ms menjadi pola arm/disarm, karena cooldown pendek membuat sinyal blip pada objek yang sama terhitung sebagai hit baru.
