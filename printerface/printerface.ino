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

int LED_ERR_BLACK = B00000001;
int LED_ERR_COL = B00000010;
int LED_POWER = B00001000;
int LED_PLAIN_A4 = B00010000;
int LED_PHOTO_A4 = B00100000;
int LED_ERR_PAPER = B01000000;
int LED_PHOTO_1015 = B10000000;

int ERRORS = B01000011;

int COPY_SETTINGS = B10110000;

void setup() {
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

void loop() {
  if (digitalRead(PWR_BTN)==LOW){
    needDebounce = true;
    otherLEDs = 
    (~(otherLEDs & LED_POWER))&(otherLEDs|LED_POWER);
    Serial.println("PWR_BTN");
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
   
  boolean btnScan = false;
  boolean btnColour = false;
  boolean btnBlack = false;
  boolean btnCopy = false;
  boolean btnCancel = false;  
  
  for (int i = 0; i<= 8-1; i++){
    int btnData = digitalRead(BTN_DATA_IN);
    if (i == 1){
      btnScan = !(boolean)(btnData);
    }
    
    if (i == 2){
      btnColour = !(boolean)(btnData);
    }
    
    if (i == 3){
      btnBlack = !(boolean)(btnData);
    }
    
    if (i == 5){
      btnCopy = !(boolean)(btnData);
    }
    
    if (i == 7){
      btnCancel = !(boolean)(btnData);
    }
    
    digitalWrite(LED_DATA_SHIFT, HIGH);
    digitalWrite(LED_DATA_SHIFT, LOW);
  }
  if (btnScan){
    Serial.println("btnScan");
    if (num < 9) num++;
  }
  if (btnColour){
    Serial.println("btnColour");
    if (num > 0) num--;
  }
  if (btnBlack){
    Serial.println("btnBlack");
    if (num < 9) num++;
  }
  if (btnCopy){
    Serial.println("btnCopy");
    otherLEDs = otherLEDs | LED_PLAIN_A4;
  }
  if (btnCancel){
    Serial.println("btnCancel");
  }
  if (needDebounce){
    needDebounce = false;
    debounce(PWR_BTN);
  }
}

