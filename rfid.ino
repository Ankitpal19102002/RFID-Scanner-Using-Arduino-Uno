#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// RFID pins
#define RST_PIN 9
#define SS_PIN 10

// Define pins for LED and Servo
#define GREEN_LED_PIN 7
#define SERVO_PIN 3

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Servo myServo;  // Create Servo instance

// Known RFID tag UID (replace with your own tag's UID)
byte knownUID[] = {0x63, 0xAF, 0x1F, 0xFB}; // Example UID

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  // Set pin modes
  pinMode(GREEN_LED_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Initial position (locked)

  Serial.println("Place your RFID tag near the reader...");

  // Check if the RFID reader is properly initialized
  if (!mfrc522.PCD_PerformSelfTest()) {
    Serial.println("RFID reader initialization failed!");
    while (true);
  } else {
    Serial.println("RFID reader initialized successfully.");
  }
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Show UID on serial monitor
  Serial.print("UID tag: ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();

  // Print the UID in a single line
  Serial.print("Token ID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) {
      Serial.print(":");
    }
  }
  Serial.println();

  // Check if the read UID matches the known UID
  bool authorized = true;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != knownUID[i]) {
      authorized = false;
      break;
    }
  }

  // If authorized, unlock the door
  if (authorized) {
    Serial.println("Access granted.");
    digitalWrite(GREEN_LED_PIN, HIGH); // Turn on the green LED
    myServo.write(90); // Unlock position
    delay(3000); // Keep the door unlocked for 3 seconds
    myServo.write(0); // Lock position
    digitalWrite(GREEN_LED_PIN, LOW); // Turn off the green LED
  } else {
    Serial.println("Access denied.");
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}
