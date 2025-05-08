#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// I2C LCD Address
#define LCD_ADDRESS 0x27  // Changed to 0x20.  Make sure this is correct for your LCD.
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

// Servo Pins
Servo servoEntry;
Servo servoExit;
const int servoEntryPin = 9;
const int servoExitPin = 10;

// IR Sensor Pins
const int irEntry1 = 2;
const int irEntry2 = 3;
const int irExit1 = 4;
const int irExit2 = 5;

// Slot IR Sensor Pins
const int slotSensors[] = {A0, A1, A2, A3};
const int numSlots = 4;
bool slotOccupied[numSlots] = {false};
int availableSlots = numSlots;
int displayStartSlot = 0; // Start slot for display

void setup() {
  // Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.print("Auto ParkMaster");
  //lcd.print("System");

  // Attach Servos
  servoEntry.attach(servoEntryPin);
  servoExit.attach(servoExitPin);

  // Set IR Sensor Pins as Input
  for (int i = 0; i < numSlots; i++) {
    pinMode(slotSensors[i], INPUT);
  }
  pinMode(irEntry1, INPUT);
  pinMode(irEntry2, INPUT);
  pinMode(irExit1, INPUT);
  pinMode(irExit2, INPUT);

  // Initial Servo Positions
  servoEntry.write(0);
  servoExit.write(0);
  updateSlots();
  delay(1000);
  lcd.clear();
}

void loop() {
  entryGateControl();
  exitGateControl();
  updateSlots();
  displaySlots();
  delay(100);
}

void entryGateControl() {
  if (digitalRead(irEntry1) == LOW) {
    if (availableSlots > 0) { // Check if there are available slots
      lcd.clear();
      lcd.print("Entry: Open");
      servoEntry.write(90);
      delay(2000);
      if (digitalRead(irEntry2) == LOW) {
        delay(2000);
      }
      servoEntry.write(0);
      lcd.clear();
      lcd.print("Entry: Closed");
    } else {
      lcd.clear();
      lcd.print("Parking Full!");
      lcd.setCursor(0, 1);
      lcd.print("No Empty Slot");
      delay(3000); //display message
    }
  }
}

void exitGateControl() {
  if (digitalRead(irExit1) == LOW) {
    lcd.clear();
    lcd.print("Exit: Open");
    servoExit.write(90);
    delay(2000);
    if (digitalRead(irExit2) == LOW) {
      delay(2000);
    }
    servoExit.write(0);
    lcd.clear();
    lcd.print("Exit: Closed");
  }
}

void updateSlots() {
  availableSlots = numSlots;
  for (int i = 0; i < numSlots; i++) {
    slotOccupied[i] = (analogRead(slotSensors[i]) < 500); // Adjust threshold as needed
    if (slotOccupied[i]) {
      availableSlots--;
    }
  }
}

void displaySlots() {
  lcd.clear();
  if (availableSlots == 0) {
    lcd.print("Parking Full!");
    lcd.setCursor(0, 1);
    lcd.print("No Empty Slot");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Available : ");
    lcd.print(availableSlots);

    // Display slots with scrolling
    for (int i = 0; i < 4; i++) { // Display two slots at a time
      int slotIndex = (displayStartSlot + i) % numSlots; //wrap around
      lcd.setCursor(0, 1);
      lcd.print("Slot ");
      lcd.print(slotIndex + 1);
      lcd.print(":");
      if (slotOccupied[slotIndex]) {
        lcd.print("Full");
      } else {
        lcd.print("Empty");
      }
      delay(1000); // Delay for visibility
    }
    displayStartSlot = (displayStartSlot + 4) % numSlots; //increment by 2
  }
}
