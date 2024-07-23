#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

#define SWITCH1_PIN 2  // Connect switch/button pins
#define SWITCH2_PIN 3
#define SWITCH3_PIN 4

#define BUZZER_PIN 5   // Connect buzzer pin

// Define party names
#define PARTY1 "BJP"
#define PARTY2 "Congress"
#define PARTY3 "NCP"

int bjpVotes = 0;
int congressVotes = 0;
int ncpVotes = 0;

bool voted = false;  // Flag to track whether voter has already voted

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522

  pinMode(SWITCH1_PIN, INPUT_PULLUP);  // Set switch pins as inputs with pull-up resistors
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SWITCH3_PIN, INPUT_PULLUP);
  
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Read RFID tag
    String tag = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      tag += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      tag += String(mfrc522.uid.uidByte[i], HEX);
    }
    tag.toUpperCase();

    Serial.print("RFID Tag: ");
    Serial.println(tag);

    // Check if voter already voted
    if (!isVoterRegistered(tag)) {
      // Read button press and cast vote
      int buttonPressed = readButtonPress();
      if (buttonPressed != -1) {
        castVote(buttonPressed);
        // Provide feedback using buzzer
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);
        delay(1000);
        voted = true;  // Set voted flag to true
      }
    } else {
      if (voted) {
        Serial.println("Not Applicable - Already voted with this RFID tag.");
        voted = false;  // Reset voted flag
      } else {
        Serial.println("You have already voted with this RFID tag. Please use a different tag.");
      }
    }
  }

  // Print total votes and winner
  printTotalVotes();
  printWinner();
  delay(5000); // Delay to reduce serial output frequency
}

bool isVoterRegistered(String tag) {
  // Check if voter is registered (for simplicity, assuming no registration process in this code)
  // Implement actual registration logic if required
  return voted;  // Return voted flag value
}

int readButtonPress() {
  // Read button presses
  if (digitalRead(SWITCH1_PIN) == LOW) {
    return 1; // Button 1 pressed
  } else if (digitalRead(SWITCH2_PIN) == LOW) {
    return 2; // Button 2 pressed
  } else if (digitalRead(SWITCH3_PIN) == LOW) {
    return 3; // Button 3 pressed
  }
  return -1; // No button pressed
}

void castVote(int button) {
  // Cast vote based on button pressed
  switch (button) {
    case 1:
      Serial.println("Vote cast for BJP");
      bjpVotes++;
      break;
    case 2:
      Serial.println("Vote cast for Congress");
      congressVotes++;
      break;
    case 3:
      Serial.println("Vote cast for NCP");
      ncpVotes++;
      break;
  }
}

void printTotalVotes() {
  Serial.println("Total Votes:");
  Serial.print("BJP: ");
  Serial.println(bjpVotes);
  Serial.print("Congress: ");
  Serial.println(congressVotes);
  Serial.print("NCP: ");
  Serial.println(ncpVotes);
}

void printWinner() {
  int maxVotes = max(bjpVotes, max(congressVotes, ncpVotes));
  Serial.print("Winner: ");
  if (maxVotes == bjpVotes) {
    Serial.println(PARTY1);
  } else if (maxVotes == congressVotes) {
    Serial.println(PARTY2);
  } else {
    Serial.println(PARTY3);
  }
}
