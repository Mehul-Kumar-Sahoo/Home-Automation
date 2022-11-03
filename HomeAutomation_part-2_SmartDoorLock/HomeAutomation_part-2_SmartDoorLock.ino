#include <MFRC522.h>
#include <Keypad.h>
#include <SPI.h>
#include <Servo.h>
#include <TM1637Display.h>

int CLK = 5;
int DIO = 6;

TM1637Display display = TM1637Display(CLK,DIO);
MFRC522 mfrc522(10, 9); // MFRC522 mfrc522(SS_PIN, RST_PIN)
Servo servo;

String tagUID = "16 F8 07 FD";  // initial UID
boolean RFIDMode = true;

int key_val = 0;
int code = 9625;
const byte rows = 4;
const byte columns = 4;
byte row_pins[rows] = {A0, A1, A2, A3};
byte column_pins[columns] = {4, 3, 2};
char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

// Create array that turns all segments on:
const uint8_t data[] = {0xff, 0xff, 0xff, 0xff};

// Create array that turns all segments off:
const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};

const uint8_t unlock[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,           // O
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,                   // P
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,                   // E
  SEG_C | SEG_E | SEG_G                                    // N
};

const uint8_t error[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,                   // E
  SEG_E | SEG_G,                                           // R
  SEG_E | SEG_G,                                           // R
  
};

void setup() {
  // put your setup code here, to run once:
  display.setBrightness(7);
  display.clear();
  delay(1000);
  servo.attach(8);
//  servo.write(0);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  int key = keypad.getKey();
  if(key != 0){
    if(key == 42){
      key_val = 0;
      display.clear();
    }
    else if(key == 35){
      if(key_val == code){
        display.setSegments(unlock);
        servo.write(360);
        delay(412);
        servo.write(90);
        delay(8000);
        display.setSegments(blank);
        key_val = 0;
        servo.write(-360);
        delay(412);
        servo.write(90);
      }
      else{
        display.setSegments(error);
      }
    }
    else{
      key_val = key_val*10 + (int(key)-48);
      display.showNumberDec(key_val);
    }
//    display.showNumberDec(key);
  }
  if (RFIDMode == true) {
    Serial.println("HI");
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      Serial.println("1");
      return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      Serial.println("2");
      return;
    }

    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++)
    {
      Serial.print(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[j], HEX);
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();
    Serial.println();
    Serial.println(tag.substring(1));
    if (tag.substring(1) == tagUID)
    {
      display.setSegments(unlock);
      servo.write(360);
      delay(412);
      servo.write(90);
      delay(8000);
      display.setSegments(blank);
      key_val = 0;
      servo.write(-360);
      delay(412);
      servo.write(90);
    }
    else{
      display.setSegments(error);
    }
  }  
}
