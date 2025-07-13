#include <Arduino.h>
#include <SoftwareSerial.h>

const byte BUTTON_PIN = 2;
const byte LED_PIN = 4;
const byte BUZZER_PIN = 6;

const byte HC12_SET_PIN = 8;
const byte HC12_TX_PIN = 10;
const byte HC12_RX_PIN = 12;

SoftwareSerial morse(HC12_TX_PIN, HC12_RX_PIN); //Arduino RX, Arduino TX

bool testMode = false; // Set to true to enable HC-12 configuration mode
bool isInitiator = false; // Set to true if this device is the initiator of the communication
const bool  toTestBuzzerLedAndButton = false; // Set to true to test buzzer, LED and button functionality
const bool hcTestMode = false; // Set to true to enable HC-12 configuration mode
int hc12TestValue = 0;

int morseToSend = 0; // Variable to hold the value to send via HC-12
int morseReceived = 0; // Variable to hold the received value from HC-12

unsigned long dashDuration = 600; // Duration for dash in milliseconds
unsigned long dotDuration = 200; // Duration for dot in milliseconds
unsigned long morseInterval = 1000; // Interval between dots and dashes in milliseconds

unsigned long buttonDotPressDuration = 500; // Duration for button press in milliseconds
unsigned long buttonDashPressDuration = 1000; // Duration for button press for dash in milliseconds

unsigned long lastButtonPressTime = 0; // Variable to hold the last button press time


/*
@brief Function to beep the buzzer
@param _times Number of times to beep
@param _duration Duration of each beep in milliseconds
*/
void beep (int _times, int _duration) {
  for (int i = 0; i < _times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(_duration);
    digitalWrite(BUZZER_PIN, LOW);
    delay(_duration);
  }
}

/*
*@brief Function to test buzzer, LED and button functionality
*@details When the button is pressed, the LED and buzzer will blink for 500 ms.
*@note This function uses a while loop to keep the LED and buzzer toggle on and off until the button is released.
*@note If the button is pressed and the LED and buzzer doesnt toggle, check the wiring and ensure the button is functioning correctly.
*@note If only the LED or buzzer is not toggling, check the wiring for that specific component.
*/
void testBuzzerLedAndButton() {
  //Blink LED and Buzzer when button is pressed using while loop
  if (digitalRead(BUTTON_PIN) == LOW) {
    while (digitalRead(BUTTON_PIN) == LOW) {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500); // Keep LED and Buzzer on for 500 ms
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
      delay(500); // Keep them off for 500 ms
    }
    //Ensure to turn off LED and Buzzer after button release
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
} 

void loopBuzzerLedAndButtonTest() {
  if (toTestBuzzerLedAndButton) {
    testBuzzerLedAndButton();
  }
}

void setupHcTestMode() {
  if (!hcTestMode){
    Serial.println("HC-12 is in normal mode.");
  } else {
    Serial.println("HC-12 is in configuration mode. Please set the parameters as needed.");
    if (isInitiator) {
      delay(1000); // Wait for HC-12 to initialize
      Serial.println("This device is the initiator of the communication.");
      morse.println("1"); // Send a message to the other device
      delay(1000); // Wait for a second before sending the next message
    } else {
      Serial.println("This device is not the initiator of the communication.");
    }
  }
}

void loopHcTestMode() {
  if (hcTestMode) {
    if (morse.available()) {
      String message = morse.readStringUntil('\n');
      hc12TestValue = message.toInt();

      Serial.print("Received: ");
      Serial.print(hc12TestValue);

      if (hc12TestValue > 0) {
        int replyValue = hc12TestValue + 1;
        Serial.print("\tSent: ");
        Serial.println(replyValue);
        delay(500); // Wait for 500 ms before sending the reply
        morse.println(replyValue);

        // ✅ Reset value so we only reply once per message
        hc12TestValue = 0;
      }
    }
  }
}

bool setupHc12() {
  morse.begin(9600); // Start HC-12 serial communication
  pinMode(HC12_SET_PIN, OUTPUT);
  digitalWrite(HC12_SET_PIN, LOW); // Enter AT command mode

  delay(1000); // Let the module initialize

  morse.println("AT");
  delay(100);

  if (morse.available()) {
    String response = morse.readStringUntil('\n');
    Serial.print("HC-12 Response: ");
    Serial.println(response);

    digitalWrite(HC12_SET_PIN, HIGH); // ✅ Switch to normal mode before returning

    if (response.startsWith("OK")) {
      Serial.println("HC-12 is ready for configuration.");
      return true;
    } else {
      Serial.println("Failed to configure HC-12.");
      return false;
    }
  } else {
    Serial.println("No response from HC-12.");
    digitalWrite(HC12_SET_PIN, HIGH); // ✅ Still ensure we switch back
    return false;
  }
}

void blink(bool _isDot) {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);

  if (_isDot) {
    delay(dotDuration);
  } else {
    delay(dashDuration);
  }

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  delay(morseInterval);
}


void morseBlink(int _value) {
  if (_value == 1){
    blink(true); // Dot
  } else if (_value == 2) {
    blink(false); // Dash
  } else {
    Serial.println("Invalid morse value. Please send 1 for dot or 2 for dash.");
  }
}

int talkMorse() {
  int _morseToSend = 0;
  bool dashBeeped = false;

  if (digitalRead(BUTTON_PIN) == LOW) {
    lastButtonPressTime = millis();

    while (digitalRead(BUTTON_PIN) == LOW) {
      unsigned long holdDuration = millis() - lastButtonPressTime;

      // 🔊 Beep once when dash threshold is reached
      if (!dashBeeped && holdDuration > 1000) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100); // Short beep
        digitalWrite(BUZZER_PIN, LOW);
        dashBeeped = true;
      }
    }

    delay(50); // Small debounce after release

    unsigned long pressDuration = millis() - lastButtonPressTime;

    if (pressDuration > 100 && pressDuration <= 1000) {
      _morseToSend = 1; // Dot
    } else if (pressDuration > 1000) {
      _morseToSend = 2; // Dash
    }
  }

  return _morseToSend;
}





void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  pinMode(HC12_SET_PIN, OUTPUT);
  digitalWrite(HC12_SET_PIN, HIGH); // Set HC-12 to normal mode
  //digitalWrite(HC12_SET_PIN, LOW); // Uncomment to set HC-12 to configuration mode

  Serial.begin(9600); // Start Serial communication for debugging


  setupHcTestMode();

  if(setupHc12()) {
    Serial.println("HC-12 setup successful.");
    //Beep 5x fast
    beep(5, 150);
  } else {
    //Beep 5x slow
    beep(5, 500);
    Serial.println("HC-12 setup failed.");
  }

}



void loop() {
  loopBuzzerLedAndButtonTest();
  loopHcTestMode();

  if (morse.available()) {
    String message = morse.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(message);

    morseReceived = message.toInt();

    if(morseReceived > 0) {
      morseBlink(morseReceived);
    }
  } else {
    //Listen for button press
    morseToSend = talkMorse ();

    if (morseToSend > 0) {
      Serial.print("Sending: ");
      Serial.println(morseToSend);
      morse.println(morseToSend); // Send the morse value via HC-12
    }
  }
}
