#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define I2C_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Firebase credentials
#define FIREBASE_HOST "present-2690e-default-rtdb.firebaseio.com" // Your Firebase project name address
#define FIREBASE_AUTH "4MMpJZYJX2VCfVmPEyCJFvlpMkM9jA27w3jLdMyw" // Your Firebase authentication key

// RFID cards mapping (change these as per your cards)
const char* card1 = "Card1";
const char* card2 = "Card2";

bool displayAttendance = false; // Flag to control attendance display
unsigned long displayStartTime; // Variable to store the start time of display

void setup() {
  Wire.begin(D2, D1);
  Serial.begin(9600); // Initialize hardware serial communication
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID Reader");

  WiFi.begin("realme 8s 5G", "123qwert"); // Replace with your WiFi credentials

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(1000); // Wait for Firebase initialization
}

void loop() {
  if (Serial.available()) {
    String rfidData = Serial.readStringUntil('\n');

    // Check if the received data is a valid RFID tag (adjust as needed)
    if (rfidData.length() == 12) { // Change to match your RFID tag length
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RFID Tag:");

      const char* cardName = getCardName(rfidData); // Get the card name for the scanned RFID data
      lcd.setCursor(0, 1);
      lcd.print(cardName);
      delay(1000); // Display the card name for 1 second (adjust as needed)
      lcd.clear();

      // Send card name to Firebase
      sendToFirebase(cardName);
    }
  }

  // Check for changes in Firebase data
  String attendanceValue = Firebase.getString("/attendance");
  if (attendanceValue.length() > 0) {
    displayAttendance = true;
    displayStartTime = millis(); // Record the start time of display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Total Attendance:");
    lcd.setCursor(0, 1);
    lcd.print(attendanceValue);
  }

  // Control the duration of attendance display
  if (displayAttendance && (millis() - displayStartTime >= 2000)) {
    displayAttendance = false; // Reset the flag after 2 seconds
    lcd.clear();
  }

  // Your other code can go here.
}

void sendToFirebase(const char* cardName) {
  String path = "/scanned-cards"; // Specify the Firebase path where you want to store the card names
  String data = "{\"name\":\"";
  data += cardName;
  data += "\"}";

  Firebase.pushString(path, data);
  Serial.println("Data sent to Firebase");
}

const char* getCardName(String rfidData) {
  // Check the scanned RFID data against the stored card numbers and return the corresponding card name
  if (rfidData == "070064216321") {
    return card1;
  } else if (rfidData == "30005061F4F5") {
    return card2;
  } else {
    return "Unknown"; // Return default name for unknown cards
  }
}
