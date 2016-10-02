//Fan Controller designed for ps3

#include "Temperatures.h"
//298.15f
//unique thermistor defines
#define CANTHERM_MF52_100k 4150.0f,298.25f,100000.0f //B,T0,R0 //just estimating t0 value, couldn't find in datasheet so I changed it until the resulting temperature matched an IR thermometer
#define R_Balance 10000 //10k ohm resistor in series

//Thermistors
#define CPU_THERM A1
#define GPU_THERM A2
#define NB_THERM A3
#define EXHAUST_THERM A4

Temperatures tempObj;

//potentiometer
#define POTENTIOMETER A0
int pos; //for potentiometer value

//Fans
#define MAIN_FAN A7
#define NB_FAN A9 //northbridge
float percent = 0.0f;
int percent1 = 100; //default speed for northbridge fan
#define MAX_PMW 253 //we use 253 instead of 255 to limit the fans to about 99.2 percent to be safe

const int led = 11;
bool ledHIGH = true;
const int rgbLed_R = 5;
const int rgbLed_G = 10;
const int rgbLed_B = 9;
int redVal;
int blueVal;
int greenVal;

const float MAX_TEMP = 58.0f;//<- aggressive || normal -> 68.0f; //degrees celcius when system is considered hot
int FAN_LOW = 40; //40% default minimum fan speed

float testTemp; //code controlable value for testing parts of the program
bool tTempIncr;

bool isHot = false; //true if temps exceed MAX_TEMP

enum MODE{
  AUTO = 0,
  MANUAL
};

enum THERMISTORS{
  CPU = 0,
  GPU,
  NB,
  EXHAUST
};

bool ctrlMode;

void setup(){
  Serial.begin(9600);
  pinMode(MAIN_FAN, OUTPUT);
  pinMode(NB_FAN, OUTPUT);

  pinMode(led, OUTPUT);

  pinMode(rgbLed_R, OUTPUT);
  pinMode(rgbLed_G, OUTPUT);
  pinMode(rgbLed_B, OUTPUT);
  digitalWrite(rgbLed_R, HIGH);
  digitalWrite(rgbLed_G, HIGH);
  digitalWrite(rgbLed_B, HIGH);

  ctrlMode = AUTO;
  //ctrlMode = MANUAL;

  testTemp = 20.0f;
  tTempIncr = true;

  fanTest();
  tempObj.calcTemps(CANTHERM_MF52_100k,R_Balance); //do once before main loop
}

unsigned long serialTimer = millis(); //used for timing debug output
unsigned long tempTimer = millis(); //used for time between polling for temps
unsigned long ledTimer = millis(); //used to time led pulses
unsigned long toggleTime = millis(); //used for time between switching modes
unsigned long testTimer = millis(); //used for updating test temperature

void loop()                     
{
  unsigned long currentTime = millis();

  //calculate temperatures
  if(currentTime >= tempTimer)
  {
    tempTimer = millis()+500; //every .5 seconds
    tempObj.calcTemps(CANTHERM_MF52_100k,R_Balance);
  }

  //switch modes if the potentiometer is dialed way down
  if(analogRead(POTENTIOMETER) <= 200 && currentTime >= toggleTime)
   {
   toggleTime = millis()+3000; //wait 3 seconds before we can switch modes again
   digitalWrite(led, HIGH);
   delay(500);
   digitalWrite(led, LOW);
   ctrlMode = !ctrlMode;
   }

  setFans();
  setRgbLed();

  //To test isHot() set to true after 1 minute
  //if(currentTime >= 60000)
  //isHot = true;

  //output serial info every 2 seconds
  if(currentTime >= serialTimer)
  {
    serialTimer = millis()+2000;
    if(isHot)
      Serial.println("\nWARNING: SYSTEM HOT!");
    Serial.print("\nControl Mode: ");
    if(ctrlMode == AUTO)
      Serial.println("Automatic");
    else
      Serial.println("Manual");
    Serial.print("\nCPU Temp: "); 
    Serial.print(tempObj.getTemp(CPU)); 
    Serial.println("*C");
    Serial.print("GPU Temp: "); 
    Serial.print(tempObj.getTemp(GPU)); 
    Serial.println("*C");
    Serial.print("NB Temp: "); 
    Serial.print(tempObj.getTemp(NB)); 
    Serial.println("*C");
    Serial.print("Exhaust Air: "); 
    Serial.print(tempObj.getTemp(EXHAUST)); 
    Serial.println("*C");
    Serial.print("Main-Fan Speed: "); 
    Serial.print(percent); 
    Serial.println("%");
    Serial.print("NorthBridge-Fan Speed: "); 
    Serial.print(percent1); 
    Serial.println("%");
    //Serial.print(currentTime);
    //Serial.print("\nTest Temp: "); Serial.print(testTemp); Serial.println("*C");
    Serial.print("Signal LED: (R:"); 
    Serial.print(redVal); 
    Serial.print(" G:"); 
    Serial.print(greenVal); 
    Serial.print(" B:"); 
    Serial.print(blueVal); 
    Serial.println(")");
  }
  //flash the led if system is hot
  if(isHot)
  {
    if(currentTime >= ledTimer)
    {
      ledTimer = millis()+50;
      digitalWrite(led, ledHIGH);
      ledHIGH = !ledHIGH; //toggle led on or off
    }
  }
}

void fanTest()
{
  Serial.println("Fan Test...");
  //Fan Test
  digitalWrite(led, HIGH);
  Serial.println("...Speeding up");
  for(int i = 102; i <= MAX_PMW; i++)  //102 is 40% % is the lowest desired fan speed
  {
    analogWrite(MAIN_FAN, i);
    analogWrite(NB_FAN, i);
    //Fade signal LED White during fan test
    analogWrite(rgbLed_R, 255-i*2);
    analogWrite(rgbLed_G, 255-i*2);
    analogWrite(rgbLed_B, 255-i*2);
    delay(25);
  }
  //pulse led quickly while at fastest setting
  Serial.println("...Fastest Setting!  99.2 %");
  for(int t = 0; t <= 10; t++)
  {
    digitalWrite(led, LOW);
    delay(75);
    digitalWrite(led, HIGH);
    delay(75);
  }
  digitalWrite(led, LOW);
  Serial.println("...Slowing down");
  for(int i = MAX_PMW; i >= 102; i--) //40% is the slowest desired speed (102 is 40% of 255)
  {
    analogWrite(MAIN_FAN, i);
    analogWrite(NB_FAN, i);
    //Fade signal LED White during fan test
    analogWrite(rgbLed_R, 255-i*2);
    analogWrite(rgbLed_G, 255-i*2);
    analogWrite(rgbLed_B, 255-i*2);
    delay(25);
  }
  //pulse led slowly while at slowest setting
  Serial.println("...Slowest Setting!  40 %");
  analogWrite(rgbLed_R, 204);
  analogWrite(rgbLed_G, 204);
  analogWrite(rgbLed_B, 204);
  for(int t = 0; t <= 5; t++)
  {
    digitalWrite(led, LOW);
    delay(300);
    digitalWrite(led, HIGH);
    delay(300);
  }
  digitalWrite(led, LOW);
  Serial.println("...done\n");
}

void setFans()
{
  //we want to control the main fan based on the highest temperature
  float hT = tempObj.getHighestTemp();

  //uncomment to use testTemp
  //hT = testTemp;

  //if temp isn't too hot
  if(hT <= MAX_TEMP)
    isHot = false;

  //set FAN_LOW (for main fan) based on highest temp
  /*
  if(hT <= 35)
    FAN_LOW = 40;
  else if(hT >= 35 && hT <= 40)
    FAN_LOW = 45;
  else if(hT >= 41 && hT <= 44)
    FAN_LOW = 50;
  else if(hT >= 45 && hT <= 48)
    FAN_LOW = 55;
  else if(hT >= 49 && hT <= 51)
    FAN_LOW = 63;
  else if(hT >= 52 && hT <= 55)
    FAN_LOW = 70;
  else if(hT >= 56 && hT <= 59)
    FAN_LOW = 77;
  else if(hT >= 60 && hT <= 63)
    FAN_LOW = 85;
  else if(hT >= 64 && hT <= 67)
    FAN_LOW = 93;
  else if(hT >= MAX_TEMP)
  {
    FAN_LOW = 99.2;
    isHot = true;
  }*/
  //^^ Regular code above, aggressive setup for Mike's below
  if(hT <= 26.00)
    FAN_LOW = 40;
  else if(hT >= 26.01 && hT <= 30.00)
    FAN_LOW = 45;
  else if(hT >= 30.01 && hT <= 34.00)
    FAN_LOW = 50;
  else if(hT >= 34.01 && hT <= 37.00)
    FAN_LOW = 55;
  else if(hT >= 37.01 && hT <= 40.00)
    FAN_LOW = 63;
  else if(hT >= 40.01 && hT <= 43.00)
    FAN_LOW = 66;
  else if(hT >= 43.01 && hT <= 45.00)
    FAN_LOW = 70;
  else if(hT >= 45.01 && hT <= 49.00)
    FAN_LOW = 77;
  else if(hT >= 49.01 && hT <= 53.00)
    FAN_LOW = 85;
  else if(hT >= 53.01 && hT <= 55.00)
    FAN_LOW = 93;
  else if(hT >= 55.01 && hT <= 57.99)
    FAN_LOW = 93;
  else if(hT >= MAX_TEMP)
  {
    FAN_LOW = 99.2;
    isHot = true;
  }

  float nbTemp = tempObj.getTemp(NB);

  /*
  //control the nbFan based on nb temp
   if(nbTemp <= 40)
   percent1 = 50;
   else if(nbTemp >= 41 && nbTemp <= 45)
   percent1 = 65;
   else if(nbTemp >= 46 && nbTemp <= 50)
   percent1 = 80;
   else if(nbTemp >= 51)
   percent1 = 100;
   */  //^^ just leave at 100

  if(ctrlMode == AUTO)
  {
    //set main fan according to temperature (slightly higher than FAN_LOW)
    percent = FAN_LOW + 10;
    if(percent > 99.2)
      percent = 99.2;

    int pmw = (percent*255)/100;
    int pmw1 = (percent1*255)/100;
    analogWrite(MAIN_FAN, pmw);
    analogWrite(NB_FAN, pmw1);
  }
  else if(ctrlMode == MANUAL)
  {
    pos = analogRead(POTENTIOMETER); //read value from potentiometer
    //Serial.println(pos);
    percent = (pos*100)/1000;
    if(percent > 99.2)
      percent = 99.2;
    if(percent < FAN_LOW) //40%
      percent = FAN_LOW;
    //Serial.println(percent);
    int pmw = (percent*255)/100;
    int pmw1 = (percent1*255)/100;
    //Serial.println(pmw);
    //Serial.println("\n");
    analogWrite(MAIN_FAN, pmw);
    analogWrite(NB_FAN, pmw1);
  }
}

void setRgbLed()
{
  //we want to fade the RBG LED based on the highest temperature
  // Blue=>Green::Cool=>Warm  Green=>Red::Warm=>Hot
  float hT = tempObj.getHighestTemp();

  //uncomment to use testTemp
  //hT = testTemp;

  //define operating temp range and caculate PMW values
  /*
  if(hT <= 35.0f)
  {
    redVal = 0;
    greenVal = 0;
    blueVal = 255;
  }
  else if(hT > MAX_TEMP)
  {
    redVal = 255;
    greenVal = 0;
    blueVal = 0;
  }
  else if(hT >= 35.01f && hT <= 50.0f) //fade from blue to green
  {
    float p = ((hT-35.01)*100)/(50-35.01);
    greenVal = (p*255)/100;
    blueVal = 255 - greenVal;
    redVal = 0;
  }
  else if(hT >= 50.01f && hT <= MAX_TEMP) //fade from green to red
  {
    float p = ((hT-55.01)*100)/(MAX_TEMP-50.01);
    redVal = (p*255)/100;
    greenVal = 255 - greenVal;
    blueVal = 0;
  }*/
  //^^ Regular code above, aggressive setup for Mike's below
  if(hT <= 25.0f)
  {
    redVal = 0;
    greenVal = 0;
    blueVal = 255;
  }
  else if(hT > MAX_TEMP)
  {
    redVal = 255;
    greenVal = 0;
    blueVal = 0;
  }
  else if(hT >= 25.01f && hT <= 39.0f) //fade from blue to green
  {
    float p = ((hT-25.01)*100)/(39-25.01);
    greenVal = (p*255)/100;
    blueVal = 255 - greenVal;
    redVal = 0;
  }
  else if(hT >= 39.01f && hT <= MAX_TEMP) //fade from green to red
  {
    float p = ((hT-39.01)*100)/(MAX_TEMP-39.01);
    redVal = (p*255)/100;
    greenVal = 255 - redVal;
    blueVal = 0;
  }

  //write pins
  analogWrite(rgbLed_R, 255 - redVal);
  analogWrite(rgbLed_G, 255 - greenVal);
  analogWrite(rgbLed_B, 255 - blueVal);
}


