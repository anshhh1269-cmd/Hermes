#include <Wire.h>
#include <Adafruit_LSM6DS3.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h> // Native RP2040 USB Keyboard library

// Configure 0.96" OLED Screen (128x64 I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialize Sensor & Native USB Keyboard
Adafruit_LSM6DS3 lsm6ds3;
USBKeyboard Keyboard;

// Hardware Interrupt Mapping (INT1 connected to D0 on your board)
const int interruptPin = D0;
volatile bool dataReady = false;

// Steering Sensitivity Thresholds
const float TILT_THRESHOLD = 0.8; // Radians per second trigger threshold

// Interrupt Service Routine (ISR)
void IRAM_ATTR onDataReady() {
  dataReady = true;
}

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);

  // Initialize Screen
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); // Lock up if screen is missing
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("BOOTING CONTROLLER...");
  display.display();

  // Initialize Shopee LSM6DS3 Sensor
  if (!lsm6ds3.begin_I2C()) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("ERROR: SENSOR MISSING");
    display.display();
    while (1);
  }

  // Configure Sensor Hardware Interrupt on INT1 pin
  lsm6ds3.configInt1(true, false); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), onDataReady, RISING);

  // Ready Screen Splash
  display.clearDisplay();
  display.setCursor(20, 15);
  display.setTextSize(2);
  display.println("USB WHEEL");
  display.setCursor(35, 40);
  display.setTextSize(1);
  display.println("READY TO PLAY!");
  display.display();
  delay(1500);
}

void loop() {
  // Check if sensor has fired the physical D0 interrupt pin
  if (dataReady) {
    dataReady = false; // Reset interrupt flag

    sensors_event_t accel, gyro, temp;
    lsm6ds3.getEvent(&accel, &gyro, &temp);

    // Read the Gyro's Y-Axis (rotation velocity around the turning plane)
    float steeringInput = gyro.gyro.y; 

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("=== USB TILT WHEEL ===");

    // Draw Visual Center Line Gauge Anchor on OLED
    display.drawFastVLine(64, 25, 15, SSD1306_WHITE);

    // Act as Steering Wheel Keyboard Inputs & Display Visual Feedback
    if (steeringInput > TILT_THRESHOLD) {
      // Tilting Right
      Keyboard.key_release(KEY_LEFT_ARROW); // Ensure opposite key releases
      Keyboard.key_press(KEY_RIGHT_ARROW);  // Emulate pressing right arrow
      
      display.setCursor(25, 50);
      display.println("[ STEER RIGHT -> ]");
      // Draw a block indicator on the right side of the screen
      display.fillRect(64, 28, map(steeringInput*10, 0, 50, 0, 60), 10, SSD1306_WHITE);
      
    } else if (steeringInput < -TILT_THRESHOLD) {
      // Tilting Left
      Keyboard.key_release(KEY_RIGHT_ARROW);
      Keyboard.key_press(KEY_LEFT_ARROW);   // Emulate pressing left arrow
      
      display.setCursor(25, 50);
      display.println("[ <- STEER LEFT ]");
      // Draw a block indicator on the left side of the screen
      int width = map(abs(steeringInput)*10, 0, 50, 0, 60);
      display.fillRect(64 - width, 28, width, 10, SSD1306_WHITE);
      
    } else {
      // Deadzone / Centered
      Keyboard.key_release(KEY_LEFT_ARROW);
      Keyboard.key_release(KEY_RIGHT_ARROW);
      
      display.setCursor(35, 50);
      display.println("[ CENTERED ]");
      display.fillRect(62, 28, 5, 10, SSD1306_WHITE); // Tiny center block
    }

    display.display(); // Push graphics updates to the OLED
  }
}
