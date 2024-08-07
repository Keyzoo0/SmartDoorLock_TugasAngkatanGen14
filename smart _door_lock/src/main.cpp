#include <Arduino.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <Wire.h> // include library
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define relay 8
#define buzzer 7

//=======deklarasi LCD========//

LiquidCrystal_I2C lcd(0x27, 16, 2);

//=======deklarasi keypad========//

const byte ROWS = 4; // Jumlah baris keypad
const byte COLS = 3; // Jumlah kolom keypad

char Keys[ROWS][COLS] = { // Membuat array keypad
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {0, 1, 2, 3}; // Deklarasi pin keypad
byte colPins[COLS] = {4, 5, 6};

Keypad customKeypad = Keypad(makeKeymap(Keys), rowPins, colPins, ROWS, COLS); // Masukkan info keypad pada library

char opsi;
char customKey;      // Variabel penampung input keypad
int number = 0;      // Variabel penampung nilai angka
int password = 9731; // Password

//=======deklarasi RFID========//
RFID rfid(10, 9); // D10:pin of tag reader SDA. D9:pin of tag reader RST
unsigned char status;
unsigned char str[MAX_LEN]; // MAX_LEN is 16: size of the array

String accessGranted[MaxUser][2] = {
    "812679922", "102735496"}; // RFID serial numbers to grant access to

int accessGrantedSize = 2; // The number of serial numbers

void setup()
{
  Serial.begin(9600);     // Serial monitor is only required to get tag ID numbers and for troubleshooting
  SPI.begin();            // Start SPI communication with reader
  rfid.init();            // initialization rfid
  lcd.init();             // initialization lcd
  pinMode(relay, OUTPUT); // initialization relay
  lcd.backlight();
}

void loop()
{
  //=============RDID==============================//
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK) // Wait for a tag to be placed near the reader
  {
    Serial.println("Card found");

    String temp = "";                // Temporary variable to store the read RFID number
    if (rfid.anticoll(str) == MI_OK) // Anti-collision detection, read tag serial number
    {

      for (int i = 0; i < 4; i++) // Record and display the tag serial number
      {
        temp = temp + (0x0F & (str[i] >> 4));
        temp = temp + (0x0F & str[i]);
      }

      checkAccess(temp); // Check if the identified tag is an allowed to open tag
    }
    rfid.selectTag(str); // Lock card to prevent a redundant read, removing the line will make the sketch read cards continually
  }
  rfid.halt();

  //==========input pin ============================//
  digitalWrite(relay, LOW);
  lcd.setCursor(0, 0);
  lcd.print("SMART DOOR LOCK "); // Tampilan pada layar LCD

  customKey = customKeypad.getKey(); // Baca input keypad

  //------------Prosedur jika input berupa angka------------//
  switch (customKey)
  {
  case '0' ... '9':
    lcd.setCursor(0, 1);
    lcd.print("PIN : "); // Tampilan pada layar LCD
    lcd.setCursor(6, 1);
    number = number * 10 + (customKey - '0');
    lcd.print(number);
    break;

  //------------Jika input '#' maka cek password------------//
  case '#':
    if (number == password)
    { // Jika password benar, maka
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("PIN BENAR!! "); // Tampilan LCD
      lcd.setCursor(1, 1);
      lcd.print("Silahkan Masuk");
      number = 0;
      pinMode(relay, LOW);
      tone(buzzer, 395);
      delay(150);
      tone(buzzer, 440);
      delay(150);

      tone(buzzer, 395);
      delay(150);
      tone(buzzer, 440);
      delay(150);
      noTone(buzzer);

      delay(3000);
      lcd.clear();
      pinMode(relay, HIGH);
    }
    else
    { // Jika salah, maka
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password Salah!!"); // Tampilan LCD
      tone(buzzer, 650);
      delay(150);
      noTone(buzzer);
      delay(50);
      tone(buzzer, 758);
      delay(150);
      noTone(buzzer);
      delay(50);
      tone(buzzer, 998);
      delay(150);
      noTone(buzzer);
      delay(50);
      tone(buzzer, 650);
      delay(150);
      noTone(buzzer);
      delay(50);
      tone(buzzer, 758);
      delay(150);
      noTone(buzzer);
      delay(50);
      tone(buzzer, 998);
      delay(150);
      noTone(buzzer);
      delay(50);
      delay(2000);
      number = 0;
      lcd.clear();
    }
    break;

  //------------Jika input '*' maka hapus tampilan------------//
  case '*':
    number = 0;
    lcd.clear();
    break;
  }
}
void checkAccess(String temp) // Function to check if an identified tag is registered to allow access
{
  boolean granted = false;
  for (int i = 0; i <= (accessGrantedSize - 1); i++) // Runs through all tag ID numbers registered in the array
  {
    if (accessGranted[i] == temp) // If a tag is found then open/close the lock
    {
      lcd.clear();
      Serial.println("Access Granted");
      lcd.setCursor(1, 0);
      lcd.print("Akses Diterima");
      lcd.setCursor(1, 1);
      lcd.print("Silahkan Masuk");
      granted = true;
      pinMode(relay, LOW);
      tone(buzzer, 395);
      delay(150);
      tone(buzzer, 440);
      delay(150);

      tone(buzzer, 395);
      delay(150);
      tone(buzzer, 440);
      delay(150);
      noTone(buzzer);
      delay(2000);
      pinMode(relay, HIGH);
      noTone(buzzer);
      lcd.clear();
    }
  }
  if (granted == false) // If the tag is not found
  {
    lcd.clear();
    Serial.println("Access Denied");
    lcd.setCursor(0, 0);
    lcd.print("Akses Ditolak");
    tone(buzzer, 650);
    delay(150);
    noTone(buzzer);
    delay(50);
    tone(buzzer, 758);
    delay(150);
    noTone(buzzer);
    delay(50);
    tone(buzzer, 998);
    delay(150);
    noTone(buzzer);
    delay(50);
    tone(buzzer, 650);
    delay(150);
    noTone(buzzer);
    delay(50);
    tone(buzzer, 758);
    delay(150);
    noTone(buzzer);
    delay(50);
    tone(buzzer, 998);
    delay(150);
    noTone(buzzer);
    delay(50);
  }
}