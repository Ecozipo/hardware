//declaration de toute les libraires
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//declaration OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// On an arduino UNO:       A4(SDA), A5(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//declaration sur le waterflow sensor 
byte sensorPin       = 2;
byte sensorInterrupt = 0;
float calibrationFactor = 4.5;
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

//realy electrovanne
const int relayPin = 10;
bool stateHR =true;

//logo pour oled
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11111111,
  0b00000001, 0b11111110,
  0b00000111, 0b11111100,
  0b00011111, 0b11110000,
  0b01111111, 0b11000000,
  0b00000011, 0b11111000,
  0b00000111, 0b11100000,
  0b00001111, 0b11000000,
  0b00011111, 0b10000000,
  0b00111111, 0b00000000,
  0b00000111, 0b11000000,
  0b00001111, 0b10000000,
  0b00011111, 0b00000000,
  0b00001110, 0b00000000,
  0b00000100, 0b00000000,
  0b00001000, 0b00000000 };

int count = 0;

void setup() {
  //initialisation serial
  Serial.begin(9600);

  /*####################################Initialisation screen#####################################*/
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  /*####################################Initialisation screen#####################################*/
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(10); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();
  //loading ...
  display.drawPixel(50, 25, SSD1306_WHITE);
  display.display();
  delay(1000);
  display.drawPixel(60, 25, SSD1306_WHITE);
  display.display();
  delay(1000);
  display.drawPixel(70, 25, SSD1306_WHITE);
  display.display();
  delay(2000);
  //display SPARE message
  display.clearDisplay();
  display.setTextSize(3.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  display.setFont();
  display.println("SPARE");
  display.display();
  delay(1000);
  display.startscrolldiagleft(0x07, 0xff);
  delay(4000);
  display.stopscroll();
  delay(1000);
  //draw logo
  testdrawbitmap();    
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);

   /*####################################water flow sensor#####################################*/
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop() {
   /*####################################water flow sensor#####################################*/
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    unsigned int frac;
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t"); 		  // Print tab space
    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");        
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 
    Serial.print("\t"); 		  // Print tab space
  	Serial.print(totalMilliLitres/1000);
	  Serial.print("L");
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }

   /*####################################affichage sur oled#####################################*/
  if(flowRate>0){
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20,0);
    display.print(F("DEBIT "));
    if(flowRate<10){
    display.setCursor(55,25);
    display.print(int(flowRate));  
    } else{
      display.setCursor(45,25);
    display.print(int(flowRate));
    }
    display.setCursor(50,50);  
    display.setTextSize(1);
    display.println(F("L/min")); 

    display.display();
  }else{
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20,0); 
    display.println(F("CONSO"));
    display.setCursor(0,30);
    float tank = totalMilliLitres;
    display.print(float(tank/1000));
    display.println(F(" L"));
    display.display();
  }
 /*####################################test hydrovanne#####################################*/
 if(stateHR==true){
  digitalWrite(relayPin, HIGH);
  stateHR=false;
 }else{
  digitalWrite(relayPin, LOW);
  stateHR=true;
 }
   


  delay(500);

}

void pulseCounter()
{
  pulseCount++;
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2
