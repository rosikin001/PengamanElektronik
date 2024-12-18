#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D3   // Pin reset untuk modul RFID
#define SS_PIN D4    // Pin Slave Select untuk modul RFID
#define RELAY_PIN D1 // Pin untuk relay

MFRC522 rfid(SS_PIN, RST_PIN); // Inisialisasi objek RFID
MFRC522::MIFARE_Key key;

String allowedUID = "931E2D13"; // Ganti dengan UID kartu RFID yang diizinkan
bool relayStatus = false;          // Status relay (false = OFF, true = ON)

void setup() {
  Serial.begin(115200); // Memulai komunikasi serial
  SPI.begin();          // Memulai SPI
  rfid.PCD_Init();      // Inisialisasi modul RFID
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Pastikan relay dalam keadaan OFF

  Serial.println("System Ready!");
  Serial.println("Scan your RFID card...");
}

void loop() {
  // Periksa apakah ada kartu RFID yang dekat
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return; // Jika tidak ada kartu, lanjutkan loop
  }

  // Membaca UID kartu
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX); // Konversi ke format heksadesimal
  }
  uid.toUpperCase(); // Ubah ke huruf besar untuk konsistensi

  // Tampilkan UID di Serial Monitor
  Serial.print("UID: ");
  Serial.println(uid);

  // Periksa apakah UID sesuai dengan yang diizinkan
  if (uid == allowedUID) {
    if (!relayStatus) { // Jika relay OFF, nyalakan
      relayStatus = true;
      digitalWrite(RELAY_PIN, HIGH); // Aktifkan relay
      Serial.println("Access Granted! Relay ON");
    } else { // Jika relay ON, matikan
      relayStatus = false;
      digitalWrite(RELAY_PIN, LOW); // Matikan relay
      Serial.println("Relay OFF");
    }
  } else {
    Serial.println("Access Denied!");
  }

  // Hentikan komunikasi dengan kartu
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
