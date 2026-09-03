// the loop function runs over and over again forever
//
// Project for Stag Robotics
// Purpose - use bluetooth gamepad controller to drive launcher robot
//
// Using libraries - Bluepad32, and Servo
//
// Code significantly influenced by exampes for Bluepad32 - https://gitlab.com/ricardoquesada/bluepad32
//
// https://gitlab.com/ricardoquesada/bluepad32/-/blob/main/docs/plat_nina.md
// Above link shows how to load the proper firmware onto the arduino/nina boards
//

#include <Bluepad32.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

int RELAY_PIN = 4;
Servo rightServo;
Servo leftServo;
ControllerPtr myControllers[BP32_MAX_CONTROLLERS];
uint8_t old_right = 90;
uint8_t old_left = 90;

// LED Definitions
#define STRIPLENGTH_SHORT 25  // Popular NeoPixel ring size
#define STRIPLENGTH_LONG 64
#define ANTLER_LED_STRIP 20
#define SHORT_STRIP_COUNT 5
#define CHARGE_COLOR 0, 0, 255
#define WHITE 150, 150, 150
#define RED 255, 0, 0
#define DIM_RED 10, 0, 0
#define BLUE 0, 0, 255
#define DELAYVAL 100  // Time (in milliseconds) to pause between pixels
#define LED_PIN 3
Adafruit_NeoPixel pixels(STRIPLENGTH_SHORT* SHORT_STRIP_COUNT + STRIPLENGTH_LONG + ANTLER_LED_STRIP, LED_PIN, NEO_GRB + NEO_KHZ800);

int ledMode = 0;

// Arduino setup function. Runs in CPU 1
void setup() {
  // Initialize serial
  Serial.begin(9600);
  int retries = 0;
  while (!Serial) {
    delay(1000);
    retries++;
    if (retries > 5) break;
    // wait for serial port to connect.
    // You don't have to do this in your game. This is only for debugging
    // purposes, so that you can see the output in the serial console.
    ;
  }

  pinMode(RELAY_PIN, OUTPUT);

  String fv = BP32.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);

  // To get the BD Address (MAC address) call:
  const uint8_t* addr = BP32.localBdAddress();
  Serial.print("BD Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(addr[i], HEX);
    if (i < 5)
      Serial.print(":");
    else
      Serial.println();
  }

  // This call is mandatory. It setups Bluepad32 and creates the callbacks.
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  // open PWM connection to motor controllers
  rightServo.attach(1);
  leftServo.attach(2);
  // set the controllers to idle (90)
  rightServo.write(91);
  leftServo.write(91);

  pixels.begin();
}

void charge() {
  pixels.clear();  // Set all pixel colors to 'off'

  int diff = STRIPLENGTH_LONG - STRIPLENGTH_SHORT;
  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    pixels.setPixelColor(i, pixels.Color(CHARGE_COLOR));
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(CHARGE_COLOR));
      }
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
}


void shoot() {
  pixels.clear();  // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one
  int diff = STRIPLENGTH_LONG - STRIPLENGTH_SHORT;
  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(WHITE));
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(WHITE));
      }
      // pixels.setPixelColor(i + STRIPLENGTH_LONG - diff, pixels.Color(150, 150, 150));
      // pixels.setPixelColor(i + STRIPLENGTH_LONG + STRIPLENGTH_SHORT - diff, pixels.Color(150, 150, 150));
    }
  }
  pixels.show();  // Send the updated pixel colors to the hardware.

  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(RED));
    if (i > 0) {
      pixels.setPixelColor(i - 1, pixels.Color(DIM_RED));
    }
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(RED));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff - 1, pixels.Color(DIM_RED));
      }
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
  for (int i = 0; i < ANTLER_LED_STRIP; i++) {  // For each pixel...
    pixels.setPixelColor(i + STRIPLENGTH_LONG + STRIPLENGTH_SHORT * SHORT_STRIP_COUNT, pixels.Color(BLUE));
  }
  pixels.show();
  delay(3000);  // Pause before next pass through loop
  // Send the updated pixel colors to the hardware.
}

void idle() {
  pixels.clear();  // Set all pixel colors to 'off'

  int diff = STRIPLENGTH_LONG - STRIPLENGTH_SHORT;
  for (int i = 0; i < STRIPLENGTH_LONG; i++) {  // For each pixel...

    pixels.setPixelColor(i, pixels.Color(50, 50, 50, 50));
    pixels.setPixelColor(i + 1, pixels.Color(255, 255, 255, 255));
    pixels.setPixelColor(i + 2, pixels.Color(50, 50, 50, 50));
    if (i > 0) {
      pixels.setPixelColor(i - 1, pixels.Color(0, 0, 0, 0));
    }
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff - 1, pixels.Color(0, 0, 0, 0));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(50, 50, 50, 50));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff + 1, pixels.Color(255, 255, 255, 255));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff + 2, pixels.Color(50, 50, 50, 50));
      }
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(10);  // Pause before next pass through loop
  }
  for (int i = STRIPLENGTH_LONG; i > 0; i--) {  // For each pixel...

    pixels.setPixelColor(i, pixels.Color(50, 50, 50, 50));
    pixels.setPixelColor(i - 1, pixels.Color(255, 255, 255, 255));
    pixels.setPixelColor(i - 2, pixels.Color(50, 50, 50, 50));
    if (i > 0) {
      pixels.setPixelColor(i + 1, pixels.Color(0, 0, 0, 0));
    }
    if (i > STRIPLENGTH_LONG - STRIPLENGTH_SHORT - 1) {
      for (int x = 0; x < SHORT_STRIP_COUNT; x++) {
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff + 1, pixels.Color(0, 0, 0, 0));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff, pixels.Color(50, 50, 50, 50));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff - 1, pixels.Color(255, 255, 255, 255));
        pixels.setPixelColor(i + STRIPLENGTH_LONG + (x * STRIPLENGTH_SHORT) - diff - 2, pixels.Color(50, 50, 50, 50));
      }
    } else {
      pixels.fill(0, STRIPLENGTH_LONG - 2, STRIPLENGTH_LONG + (STRIPLENGTH_SHORT * SHORT_STRIP_COUNT));
    }

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(10);  // Pause before next pass through loop
  }
}
// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.print("CALLBACK: Controller is connected, index=");
      Serial.println(i);
      myControllers[i] = ctl;
      foundEmptySlot = true;

      // Optional, once the gamepad is connected, request further info about the
      // gamepad.
      ControllerProperties properties = ctl->getProperties();
      char buf[80];
      sprintf(buf,
              "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
              "flags: 0x%02x",
              properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
              properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
              properties.vendor_id, properties.product_id, properties.flags);
      Serial.println(buf);
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println(
      "CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.print("CALLBACK: Controller is disconnected from index=");
      Serial.println(i);
      myControllers[i] = nullptr;
      foundGamepad = true;
      // make sure servos are set to idle
      rightServo.write(90);
      leftServo.write(90);
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
      "CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void processGamepad(ControllerPtr gamepad) {
  // There are different ways to query whether a button is pressed.
  // By query each button individually:
  //  a(), b(), x(), y(), l1(), etc...

  if (gamepad->a()) {
    Serial.print("A pressed.");
    shoot();
    digitalWrite(RELAY_PIN, HIGH);
    delay(4000);
    digitalWrite(RELAY_PIN, LOW);
    ledMode = 0;
  }
  if (gamepad->b()) {
    Serial.print("B pressed.");
    ledMode = 1;
  }
  // set motor speeds - algorithm from - https://xiaoxiae.github.io/Robotics-Simplified-Website/drivetrain-control/arcade-drive/
  int32_t drive = -gamepad->axisY();  // gamepad returns -512 for this axis when pushed fully up
  int32_t rotate = gamepad->axisX();
  if (rotate > 0 && rotate < 50) rotate = 0;
  if (rotate < 0 && rotate > -50) rotate = 0;
  rotate = rotate / 2;
  int32_t maximum = max(abs(drive), abs(rotate));
  int32_t total = drive + rotate;
  int32_t difference = drive - rotate;
  int32_t right, left;

  // set speed according to the quadrant that the values are in
  if (drive >= 0) {     // forward
    if (rotate >= 0) {  // # I quadrant
      left = maximum;
      right = difference;
    } else {  //  # II quadrant
      left = total;
      right = maximum;
    }
  } else {              // backward
    if (rotate >= 0) {  // # IV quadrant
      left = total;
      right = -maximum;
    } else {  // # III quadrant
      left = -maximum;
      right = difference;
    }
  }

  right = map(right, -512, 512, 0, 180);
  left = map(left, -512, 512, 0, 180);


  // old algorithm
  // right = map(drive + rotate, -720, 720, 0, 180);
  // left = map(drive - rotate, -720, 720, 0, 180);

  if (gamepad->r2() || gamepad->l2()) {
    left = right;
  }
  if (right == 90) right = 92;
  if (left == 90) left = 92;
  rightServo.write((right + old_right) / 2);
  leftServo.write((left + old_left) / 2);
  old_left = (left + old_left) / 2;
  old_right = (right + old_right) / 2;

  char buf[256];
  snprintf(buf, sizeof(buf) - 1,
           "X: %4li, Y: %4li => L: %4li, R: %4li",
           rotate, drive, left, right);
  Serial.println(buf);

  // Another way to query the buttons, is by calling buttons(), or
  // miscButtons() which return a bitmask.
  // Some gamepads also have DPAD, axis and more.
  // char buf[256];
  // snprintf(buf, sizeof(buf) - 1,
  //          "idx=%d, dpad: 0x%02x, buttons: 0x%04x, "
  //          "axis L: %4li, %4li, axis R: %4li, %4li, "
  //          "brake: %4ld, throttle: %4li, misc: 0x%02x, "
  //          "gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d, "
  //          "battery: %d",
  //          gamepad->index(),        // Gamepad Index
  //          gamepad->dpad(),         // DPAD
  //          gamepad->buttons(),      // bitmask of pressed buttons
  //          gamepad->axisX(),        // (-511 - 512) left X Axis
  //          gamepad->axisY(),        // (-511 - 512) left Y axis
  //          gamepad->axisRX(),       // (-511 - 512) right X axis
  //          gamepad->axisRY(),       // (-511 - 512) right Y axis
  //          gamepad->brake(),        // (0 - 1023): brake button
  //          gamepad->throttle(),     // (0 - 1023): throttle (AKA gas) button
  //          gamepad->miscButtons(),  // bitmak of pressed "misc" buttons
  //          gamepad->gyroX(),      // Gyro X
  //          gamepad->gyroY(),      // Gyro Y
  //          gamepad->gyroZ(),      // Gyro Z
  //          gamepad->accelX(),     // Accelerometer X
  //          gamepad->accelY(),     // Accelerometer Y
  //          gamepad->accelZ(),     // Accelerometer Z
  //          gamepad->battery()       // 0=Unknown, 1=empty, 255=full

  // );
  // Serial.println(buf);
  // Serial.print("=");
  // You can query the axis and other properties as well. See
  // Controller.h For all the available functions.
}


// Arduino loop function. Runs in CPU 1
void loop() {
  // This call fetches all the controller info from the NINA (ESP32) module.
  // Just call this function in your main loop.
  // The controllers pointer (the ones received in the callbacks) gets updated
  // automatically.
  BP32.update();
  Serial.print("-");
  // It is safe to always do this before using the controller API.
  // This guarantees that the controller is valid and connected.
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    ControllerPtr myController = myControllers[i];
    if (myController && myController->isConnected()) {
      Serial.print(".");
      processGamepad(myController);
      Serial.println("+");
    }
  }
  delay(150);
  if (ledMode == 0) idle();
  if (ledMode == 1) charge();
  // if (ledMode == 2) shoot();
}
