/**
 * @file hc12_morse_init.ino
 * @brief HC-12 Morse Code Transceiver with Connection Check and Startup Melody
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#define HC12_RX_PIN 11      ///< RX pin of the HC-12 module connected to Arduino Pin 11
#define HC12_TX_PIN 12      ///< TX pin of the HC-12 module connected to Arduino Pin 12
#define HC12_SET_PIN 10     ///< SET pin of the HC-12 module connected to Arduino Pin 10

SoftwareSerial morseCode(HC12_TX_PIN, HC12_RX_PIN); ///< SoftwareSerial for HC-12

#define BUZZER_PIN A0        ///< Pin for the active buzzer
#define LED_PIN 8           ///< Pin for the LED
#define BUTTON_PIN 7        ///< Pin for the input button

#define OUTPUT_DASH_DURATION 1000  ///< Dash duration in milliseconds
#define OUTPUT_DOT_DURATION 500    ///< Dot duration in milliseconds
#define INPUT_DASH_DURATION 1000   ///< Input threshold for distinguishing dash vs. dot

//#define HC12_TEST_MODE_INITIATOR // Uncomment for test mode initiator
//#define HC12_TEST_MODE_RESPONDER // Uncomment for test mode responder

/**
 * @brief Generates a beep on active buzzer.
 * 
 * @param duration Duration in milliseconds
 */
void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

/**
 * @brief Enters AT command mode on HC-12.
 */
void enterAtMode() {
  digitalWrite(HC12_SET_PIN, LOW);
  delay(1000);
}

/**
 * @brief Exits AT command mode on HC-12.
 */
void exitAtMode() {
  digitalWrite(HC12_SET_PIN, HIGH);
  delay(1000);
}

/**
 * @brief Verifies if the HC-12 module is connected.
 * 
 * @return true if "OK" is received from the module, false otherwise.
 */
bool isConnected() {
  morseCode.println("AT");
  delay(1000);

  if (morseCode.available()) {
    String response = morseCode.readStringUntil('\n');
    Serial.print("HC-12 Response: ");
    Serial.println(response);
    return response.startsWith("OK");
  }

  return false;
}

/**
 * @brief Initializes HC-12 module and verifies connection.
 */
void setupHc12() {
  pinMode(HC12_SET_PIN, OUTPUT);
  enterAtMode();

  morseCode.begin(9600);
  Serial.println("Initializing HC-12 module...");

  if (isConnected()) {
    Serial.println("✅ HC-12 module is connected and responsive.");
  } else {
    Serial.println("❌ HC-12 module did not respond. Check wiring and power.");
    beep(3000); // Beep to indicate failure
  }

  exitAtMode();
}

/**
 * @brief Sets up pins for I/O components.3
 */
void setupPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(HC12_SET_PIN, OUTPUT);
}

/**
 * @brief Plays a startup melody on the active buzzer.
 */
void systemInit() {
  beep(100); delay(100);
  beep(100); delay(100);
  beep(100); delay(300);

  beep(200); delay(200);
  beep(100); delay(100);
  beep(300);

  Serial.println("Welcome to the HC-12 Morse Code Communication Transceiver!");
}

/**
 * @brief Arduino setup function.
 */
void setup() {
  Serial.begin(9600);
  morseCode.begin(9600);

  setupPins();
  systemInit();
  setupHc12();
}

/**
 * @brief Arduino loop function.
 */
void loop() {
  // Future code for Morse code transmission/reception goes here.
}
