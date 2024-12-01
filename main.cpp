#include "Adafruit_Keypad.h"
#include <LiquidCrystal.h>
#include <Servo.h>
#define LED PA10
Servo myServo; 
String currentPassword = "1234"; 
String inputPassword = "";       
bool isChangingPassword = false; 
bool isEnteringNewPassword = false;
int failedAttempts = 0; 
bool doorOpen = false; 
const int rs = PB9, en = PB8;
const int d0 = PA0, d1 = PA1, d2 = PA2, d3 = PA3;
const int d4 = PA4, d5 = PA5, d6 = PA6, d7 = PA7;
LiquidCrystal lcd(rs, en, d0, d1, d2, d3, d4, d5, d6, d7);
const byte ROWS = 4; 
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {PB7, PB6, PB5, PB4}; 
byte colPins[COLS] = {PB3, PA15, PA12, PA11};


Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
void blink_led();
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  analogWrite(LED, 0); 
  customKeypad.begin();
  lcd.begin(16, 2);
  lcd.print("DIGITAL DOOR"); 
  delay(2000);
  lcd.clear(); 
  lcd.print("NHAP MAT KHAU"); 
  myServo.attach(PB1); 
  myServo.write(0);  
}

void loop() {
  customKeypad.tick();
  while (customKeypad.available()) {
    keypadEvent e = customKeypad.read();
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      char key = (char)e.bit.KEY;


      if (doorOpen && key == '*') {
        myServo.write(0); 
        analogWrite(LED, 0); 
        lcd.clear();
        lcd.print("DOOR CLOSED");
        delay(2000);
        lcd.clear();
        lcd.print("NHAP MAT KHAU");
        doorOpen = false; 
        continue;
      }


      if (failedAttempts >= 3) {
        if (key == 'C') {
          failedAttempts = 0; 
          lcd.clear();
          lcd.print("NHAP MAT KHAU");
        } else {
          lcd.clear();
          lcd.print("SAI QUA NHIEU!");
          delay(2000);
          lcd.clear();
          lcd.print("NHAN C DE MO");
        }
        continue;
      }


      if (key >= '0' && key <= '9') {
        if (inputPassword.length() < 16) {
          blink_led();
          inputPassword += key;
          lcd.setCursor(inputPassword.length() - 1, 1);
          lcd.print(key);
        }
      } else if (key == '#') { 
        if (isChangingPassword) {

          if (!isEnteringNewPassword) {

            if (inputPassword == currentPassword) {
              lcd.clear();
              lcd.print("NHAP MK MOI:");
              lcd.setCursor(0, 1);
              isEnteringNewPassword = true; 
            } else {
              lcd.clear();
              lcd.print("MK CU SAI!");
              delay(2000);
              lcd.clear();
              lcd.print("NHAP MAT KHAU");
              isChangingPassword = false;
            }
          } else {

            currentPassword = inputPassword;
            lcd.clear();
            lcd.print("MK MOI DA LUU!");
            delay(2000);
            lcd.clear();
            lcd.print("NHAP MAT KHAU");
            isChangingPassword = false;
            isEnteringNewPassword = false;
          }
        } else {

          if (inputPassword == currentPassword) {
            lcd.clear();
            lcd.print("MAT KHAU DUNG!");
            blink_led();
            myServo.write(180);
            delay(1000);
            lcd.clear();
            lcd.print("OPEN DOOR");
            analogWrite(LED, 255); 

            doorOpen = true;

          } else {
            failedAttempts++;
            lcd.clear();
            lcd.print("SAI MAT KHAU!");
            delay(2000);
            if (failedAttempts >= 3) {
              lcd.clear();
              lcd.print("SAI QUA NHIEU!");
              delay(2000);
              lcd.clear();
              lcd.print("NHAN C DE MO");
            } else {
              lcd.clear();
              lcd.print("NHAP MAT KHAU");
            }
          }
        }
        inputPassword = ""; 
        lcd.setCursor(0, 1);
        lcd.print("                ");
      } else if (key == 'B') {
        inputPassword = "";
        lcd.setCursor(0, 1);
        lcd.print("                ");
      } else if (key == 'D') {

        if (!isChangingPassword) {
          lcd.clear();
          lcd.print("NHAP MK CU:");
          lcd.setCursor(0, 1);
          isChangingPassword = true;
          isEnteringNewPassword = false; 
        }
        inputPassword = "";
        lcd.setCursor(0, 1);
        lcd.print("                ");
      }

      Serial.print("Key Pressed: ");
      Serial.println(key);
    }
  }
}


void blink_led(){
  analogWrite(LED, 255); 
  delay(300);
  analogWrite(LED, 0); 
}