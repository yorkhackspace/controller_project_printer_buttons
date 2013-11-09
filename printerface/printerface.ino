#include <Wire.h> 

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,10,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display

int PWR_BTN = 12;

int LED_DATA_OUT = 10;
int LED_DATA_SHIFT = 9;
int LED_DATA_STORE = 11;
int BTN_DATA_IN = 8;
//int BTN_DATA_SHIFT = 12;??

//   1
//   _
// 2|_|7
// 3|_|5
//   4
// 6 is in the middle

int SevSeg1 = B10100000;
int SevSeg2 = B11011010;
int SevSeg3 = B11110010;
int SevSeg4 = B11100100;
int SevSeg5 = B01110110;
int SevSeg6 = B01111110;
int SevSeg7 = B10100010;
int SevSeg8 = B11111110;
int SevSeg9 = B11110110;
int SevSeg0 = B10111110;

int SevSeg[] = {
B10111110,
B10100000,
B11011010,
B11110010,
B11100100,
B01110110,
B01111110,
B10100010,
B11111110,
B11110110
};

// 0 - <Nothing>
// 8 - Error, Black/photo
// 9 - Error, Colour
// 10- <Nothing>
// 11- Power
// 12- Plain A4
// 13- Photo A4
// 14- Error, Paper
// 15- Photo 10x15

int LED_POWER = B00001000;
int LED_ERR_COL = B00000010;
int LED_ERR_BLACK = B00000001;
int LED_ERR_PAPER = B01000000;

int LED_PLAIN_A4 = B00010000;
int LED_PHOTO_A4 = B00100000;
int LED_PHOTO_1015 = B10000000;

int ERRORS = B01000011;

int COPY_SETTINGS = B10110000;

int allAnimLEDs = B11110011;

int LEDANIM[] = {
B00000010,
B00000001,
B01000000,
B00010000,
B00100000,
B10000000,
B00100000,
B00010000,
B01000000,
B00000001
};
int animMax = 10;

boolean buttonsIn[8];
boolean buttonsHold[8];
boolean buttonsPress[8];

int btnScan = 1;
int btnColour = 2;
int btnBlack = 3;
int btnCopy = 5;
int btnCancel = 7;

void setup() {
  
  lcd.init();                      // initialize the lcd 
  lcd.setCursor(0,0);  
  lcd.print("Omniflux");
  lcd.setCursor(0,1);
  lcd.print("Hex Flange");
  
  // put your setup code here, to run once:
  pinMode(PWR_BTN, INPUT_PULLUP);
  pinMode(LED_DATA_OUT, OUTPUT);
  pinMode(LED_DATA_SHIFT, OUTPUT);
  pinMode(LED_DATA_STORE, OUTPUT);  
  pinMode(BTN_DATA_IN, INPUT);
  Serial.begin(9600);
}

void debounce(int pin){
  while (digitalRead(pin)==LOW){}
  delay(100);
}

int output = 0;
int otherLEDs = 0;

int num = 0;

boolean needDebounce = false;

boolean anim = false;
long animTime = 200;
int animStep = 0;
long lastTime;

void loop() {
  if (digitalRead(PWR_BTN)==LOW){
    needDebounce = true;
    otherLEDs = 
    (~(otherLEDs & LED_POWER))&(otherLEDs|LED_POWER);
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Power is");
    lcd.setCursor(0,1);
    lcd.print("now ");    
    if (otherLEDs & LED_POWER){
      lcd.print("on");
    }
    else{
      lcd.print("off");    
    }
  }

  output = SevSeg[num] | (otherLEDs<<8);
    
  for (int i = 0; i<=16-1; i++){
    int out = !((output >> i)&1);
    digitalWrite(LED_DATA_OUT, out);
    //delay(1);
    digitalWrite(LED_DATA_SHIFT, LOW);
    //delay(1);
    digitalWrite(LED_DATA_SHIFT, HIGH);
    //delay(1);   
  }
  delay(1);
  digitalWrite(LED_DATA_STORE, HIGH);
  delay(1);
  digitalWrite(LED_DATA_STORE, LOW);
  delay(1);
  digitalWrite(LED_DATA_STORE, HIGH);
  
  for (int i = 0; i<= 8-1; i++){
    int btnData = digitalRead(BTN_DATA_IN);
    buttonsIn[i] = !(boolean)(btnData);
    if (buttonsIn[i]){
        buttonsPress[i] = !buttonsHold[i];
        buttonsHold[i] = true;
    }
    else{
      buttonsHold[i] = false;
      buttonsPress[i] = false;
    }
    digitalWrite(LED_DATA_SHIFT, HIGH);
    digitalWrite(LED_DATA_SHIFT, LOW);
  }
  if (buttonsPress[btnScan]){
    Serial.println("btnScan");
    anim = !anim;
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Animation");
    lcd.setCursor(0,1);
    lcd.print("is now ");    
    if (anim){
      lcd.print("on");
    }
    else{
      lcd.print("off");    
    }
  }
  if (buttonsPress[btnColour]){
    Serial.println("btnColour");
    if (num > 0) num--;
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Digit is");
    lcd.setCursor(0,1);
    lcd.print("now ");    
    lcd.print(num);
  }
  if (buttonsPress[btnBlack]){
    Serial.println("btnBlack");
    if (num < 9) num++;
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Digit is");
    lcd.setCursor(0,1);
    lcd.print("now ");    
    lcd.print(num);
  }
  if (buttonsPress[btnCopy]){
    Serial.println("btnCopy");
    animTime = 200;
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Animation");
    lcd.setCursor(0,1);
    lcd.print("speed slow");    
  }
  if (buttonsPress[btnCancel]){
    Serial.println("btnCancel");
    animTime = 50;
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Animation");
    lcd.setCursor(0,1);
    lcd.print("speed fast");
  }
  if (needDebounce){
    needDebounce = false;
    debounce(PWR_BTN);
  }
  
  if (anim){
    long time = millis();
    if (time > lastTime + animTime){
      lastTime = time;
      animStep++;
      if (animStep > animMax-1){
        animStep = 0;
      }
      int anim = LEDANIM[animStep];
      otherLEDs =  (otherLEDs & (~allAnimLEDs)) | anim;
    }
  }
}

