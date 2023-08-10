#include <VirtualWire.h>
 
int RF_RX_PIN = 4;
int LED_PIN = 10;
int RELAY_PIN = 2;
bool state = false;
// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.print("setup");
  vw_set_rx_pin(RF_RX_PIN);  // Setup receive pin.
  vw_setup(2000); // Transmission speed in bits per second.
  vw_rx_start(); // Start the PLL receiver.
}

// the loop function runs over and over again forever
void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if(vw_get_message(buf, &buflen)) // non-blocking I/O
  {
    int i;
    // Message with a good checksum received, dump HEX
    Serial.print("Got: ");
    // Serial.print(buflen);
    // for(i = 0; i < buflen; ++i)
    // {
    //   Serial.print(buf[i], HEX);
    //   // Serial.print(" ");
    // //Serial.print(buf[i]);
    // }
    // Serial.println(" ");
    if (buflen == 1 && buf[0] == 1) {
      Serial.write("got signal");
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(RELAY_PIN, HIGH);
      delay(2000);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(RELAY_PIN, LOW);
    }
    Serial.print("#");
  }
  Serial.print(".");
  delay(500);
   digitalWrite(LED_PIN, state);
  // digitalWrite(RELAY_PIN, state);
  state = !state;
  // int buttonState = digitalRead(pushButton);
  // if (buttonState == HIGH && lastState == LOW) {  // trigger just pressed - send signal
  //   digitalWrite(LED_PIN, HIGH);
  //   const char *msg = "hello world";
  //   vw_send((uint8_t *)msg, strlen(msg));
  //   delay(1000);
  // }
  // if (lastState != buttonState) { // state changed, toggle LED
  //   digitalWrite(LED_PIN, buttonState);
  //   lastState = buttonState;
  // }

  // delay(1000);                      // wait for a second
  // digitalWrite(ledLight, LOW);   // turn the LED off by making the voltage LOW
}
