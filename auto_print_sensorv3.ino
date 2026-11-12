// ===================================================================
// Auto Print Sensor -- Arduino Uno + Autonics BEN300-DFR
// ===================================================================
// Wiring (final, sesuai brief):
//   Cokelat (BN) -> (+) PSU 24V
//   Biru    (BU) -> (-) PSU 24V
//   Putih   (WH) -> GND Arduino
//   Hitam   (BK) -> D2 Arduino
//
// Logic yang udah dites & benar (BUKAN dibalik):
//   pinMode(D2, INPUT_PULLUP) -> standby = LOW (0), ada objek = HIGH (1)
//
// ===================================================================
// REVISI KE-2 -- versi sebelumnya (edge-trigger + cooldown pendek 50ms)
// berhasil nangkep pulsa pendek dari objek asli, TAPI kebablasan ke arah
// lain: sinyal mentah sensor kadang nge-blip/goyang balik ke LOW sebentar
// terus HIGH lagi walau objeknya SAMA (belum bener-bener pergi) -- gap-nya
// lebih dari 50ms jadi lolos cooldown, kehitung hit baru padahal bukan.
//
// Fix: pola "arm/disarm". Setelah sekali HIT, sensor DIKUNCI (disarmed)
// -- gak akan ngirim HIT lagi sampai sinyalnya beneran balik ke LOW DAN
// stabil di situ selama REARM_LOW_MS (bukan cuma sekilas). Begitu udah
// stabil LOW, baru "dipersenjatai ulang" (armed) buat siap nangkep objek
// BERIKUTNYA. Ini beda dari debounce biasa: yang di-debounce di sini
// cuma transisi "objek pergi", bukan transisi "objek dateng" (itu tetep
// instant, biar pulsa pendek dari objek asli tetep ketangkep).
// ===================================================================

const int sensorPin = 2;                 // Kabel hitam (BK) sensor -> pin D2
const unsigned long REARM_LOW_MS = 150;  // sinyal harus stabil LOW selama ini dulu sebelum boleh hit lagi

int armed = true;                // true = siap ngirim HIT berikutnya
int lastRawState = LOW;
unsigned long lowSince = 0;      // kapan mulai LOW stabil (buat itung REARM_LOW_MS)

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT_PULLUP);

  lastRawState = digitalRead(sensorPin);
  lowSince = millis();
  armed = (lastRawState == LOW); // kalau pas start udah HIGH, jangan langsung armed (tunggu LOW dulu)
}

void loop() {
  int raw = digitalRead(sensorPin);
  unsigned long now = millis();

  if (raw == LOW) {
    if (lastRawState != LOW) {
      // Baru aja balik ke LOW -- mulai hitung dari sini, jangan langsung
      // percaya, siapa tau ini cuma blip sesaat juga.
      lowSince = now;
    }
    if (!armed && (now - lowSince) >= REARM_LOW_MS) {
      // Udah LOW stabil cukup lama -- baru dipercaya "objek beneran udah
      // pergi", siap nangkep objek berikutnya.
      armed = true;
    }
  }

  if (raw == HIGH && armed) {
    // Transisi ke HIGH ditangkep INSTAN (gak nunggu stabil), biar pulsa
    // pendek dari objek asli tetep kehitung -- tapi cuma kalau lagi
    // armed (belum pernah hit buat objek yang sama ini).
    Serial.println("HIT");
    armed = false; // langsung dikunci, gak bakal hit lagi sampai LOW stabil dulu
  }

  lastRawState = raw;
}
