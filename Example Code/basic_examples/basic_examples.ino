#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
//sometimes this library needs the I2C address changed from 68 to 98
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    D1
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 18

int test = D1;
bool usempu = true;

void accelColorPicker();

Adafruit_MPU6050 mpu;  //delcare the accelerometer object

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, test, NEO_GRB + NEO_KHZ800);

void initaccell()  {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: +-8G");

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: 500deg");

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: 21Hz");
  Serial.println("");

}

void setup() {
  // put your setup code here, to run once:
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(70); // Set BRIGHTNESS to about 1/5 (max = 255)

  //start the serial Monitor
  Serial.begin(115200);
  delay(100);
  Serial.println("Infinity Mirror Starting Up!!");

  //initialize the MPU
  if (usempu){
    initaccell();
  }

  delay(10);
  pinMode(D0, INPUT);
  pinMode(D8, INPUT);
  pinMode(D9, INPUT);

}


int pixel = 0; //this is the pixel i want to change
int red = 255;
int green = 20;
int blue = 5;
unsigned int hue = 0;  //max value is 65535
unsigned int opphue = 65535;  //max value is 65535
unsigned int maxint = 65535;
int val = 40;
sensors_event_t a, g, temp;
int mode = 0;
bool button = false;

void loop() {
  // put your main code here, to run repeatedly:
  // Do a theater marquee effect in various colors...
  //theaterChaseRainbow(50);
  //doubleLoop(); 
  
  if(!digitalRead(D9)){   // If the button is pressed, it is pulled low
    strip.fill(strip.Color(0,0,0));  //blank the LEDs to show successful read
    strip.show();
    
    if(mode == 0){
      mode = 1;
    }else{
      mode = 0;
    }
    delay(2);
    while(!digitalRead(D9)){
      //wait until button is no longer pressed
      delay(1);
    }
  }

  
  if(mode == 1) {
    accelColorPicker();
  } else {
    colorWipe(strip.ColorHSV(hue, 255, 150), 50);
    hue += 1200;
  }
  //int out = (digitalRead(D8) * 100) + (digitalRead(D0) * 10) + digitalRead(D9);
  Serial.println(digitalRead(D9));

}


void accelColorPicker(){
    mpu.getEvent(&a, &g, &temp); // this line polls sensor for new values, stores them in a, g, temp

  float x = a.acceleration.x * 100;
  float y = a.acceleration.y * 100;
  float z = a.acceleration.z * 100;

  int xcolor = map(x, -1000, 1000, 0, 255);
  int ycolor = map(y, -1000, 1000, 0, 255);
  int zcolor = map(z, -1000, 1000, 0, 255);

  xcolor = constrain(xcolor, 0, 255);
  ycolor = constrain(ycolor, 0, 255);
  zcolor = constrain(zcolor, 0, 255);
  //Serial.println(xcolor);

  strip.fill(strip.Color(xcolor, ycolor, zcolor));
  strip.show();
}

void doubleLoop() {
  //red green blue
  for(int i = 0; i < LED_COUNT; i+=2){
    strip.setPixelColor(i, strip.ColorHSV(hue, 255, val));
    strip.setPixelColor((13-i), strip.ColorHSV(opphue, 255, val));
    strip.show();
    delay(100);
  } 
  val = ((val +85) % 200) + 55;
  hue += 600;
  opphue = maxint - hue;
  //delay(10);
//   red = (red - 10) %256;
//   green = (green +14) % 256;
//   blue = (blue * 3) % 256;
  //pixel = (pixel + 1) % 14; 
}


// Some functions from Adafruit for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}


// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}


/* 
if(!digitalRead(D9) && !button){
    strip.fill(strip.Color(0,0,0));
    strip.show();
    button = true;
    if(mode == 0){
      mode = 1;
    }else{
      mode = 0;
    }
    delay(2);
    while(!digitalRead(D9)){
      delay(1);
    }
    button = false;
  }*/

