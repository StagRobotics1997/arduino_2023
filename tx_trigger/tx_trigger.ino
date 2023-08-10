#include <VirtualWire.h>
 
int RF_TX_PIN = 7;

int pushButton = 3;
int ledLight = 5;
int lastState = LOW;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledLight, OUTPUT);
  // pinMode(pushButton, INPUT);
  pinMode(pushButton, INPUT_PULLUP);
  vw_set_tx_pin(RF_TX_PIN); // Setup transmit pin
  vw_setup(2000); // Transmission speed in bits per second.
}

// the loop function runs over and over again forever
void loop() {
  int buttonState = digitalRead(pushButton); // digitalRead(pushButton);
  if (buttonState == LOW && lastState == HIGH) {  // trigger just pressed - send signal
    digitalWrite(ledLight, HIGH);
    const char *msg ="\001\009\009\007";

    vw_send((uint8_t *)msg, strlen(msg));
    delay(1000);
  }
  if (lastState != buttonState) { // state changed, toggle LED
    digitalWrite(ledLight, !buttonState);
    lastState = buttonState;
  }

  // delay(1000);                      // wait for a second
  // digitalWrite(ledLight, LOW);   // turn the LED off by making the voltage LOW
}
