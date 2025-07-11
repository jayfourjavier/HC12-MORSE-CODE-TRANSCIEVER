#include <SoftwareSerial.h>
#include <Arduino.h>

// === Uncomment the following line on the Initiator Arduino ===
// #define INITIATOR

#define HC12_RX 11
#define HC12_TX 12

SoftwareSerial hc12(HC12_TX, HC12_RX); // RX, TX

void setup() {
  Serial.begin(9600);
  hc12.begin(9600);
  
#ifdef INITIATOR
  Serial.println("Initiator: Waiting 5s before sending...");
  delay(5000);
  int value = 1;
  hc12.println(value);
  Serial.print("Initiator sent: ");
  Serial.println(value);
#endif
}

void loop() {
  if (hc12.available()) {
    String receivedStr = hc12.readStringUntil('\n');
    receivedStr.trim(); // Remove extra spaces/newlines

    int received = receivedStr.toInt();
    int nextValue = received + 1;

    Serial.print("Received: ");
    Serial.print(received);
    Serial.print(" -> Sending: ");
    Serial.println(nextValue);

    delay(500); // Small delay to ensure clean transmission
    hc12.println(nextValue);
  }
}
