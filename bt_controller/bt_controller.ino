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

Servo rightServo;
Servo leftServo;
ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

// Arduino setup function. Runs in CPU 1
void setup() {
  // Initialize serial
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial port to connect.
    // You don't have to do this in your game. This is only for debugging
    // purposes, so that you can see the output in the serial console.
    ;
  }

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
  rightServo.write(90);
  leftServo.write(90);
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
    //
  }
 
  // set motor speeds - algorithm from - https://xiaoxiae.github.io/Robotics-Simplified-Website/drivetrain-control/arcade-drive/
  int32_t drive = -gamepad->axisY();  // gamepad returns -512 for this axis when pushed fully up
  int32_t rotate = gamepad->axisX();
  int32_t maximum = max(abs(drive), abs(rotate));
  int32_t total = drive + rotate;
  int32_t difference = drive - rotate;
  int32_t right, left;
  // set speed according to the quadrant that the values are in
  if (drive >= 0) { // forward
    if (rotate >= 0) { //  # I quadrant
        left = maximum;
        right = difference; 
    } else {          //   # II quadrant
        left = total;
        right = maximum;
    }
  } else { // backward
    if (rotate >= 0) { // # IV quadrant
        left = total;
        right = -maximum;
    } else { // # III quadrant
        left = -maximum;
        right = difference;
    }
  }

  right = map(right, -512, 512, 0, 180);
  left = map(left, -512, 512, 0, 180);

  // old algorithm
  // right = map(drive + rotate, -720, 720, 0, 180);
  // left = map(drive - rotate, -720, 720, 0, 180);
  
  rightServo.write(right);
  leftServo.write(left);
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

  // It is safe to always do this before using the controller API.
  // This guarantees that the controller is valid and connected.
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    ControllerPtr myController = myControllers[i];
    if (myController && myController->isConnected() && myController->isGamepad()) {
        processGamepad(myController);
    }
  }
  delay(150);
}
