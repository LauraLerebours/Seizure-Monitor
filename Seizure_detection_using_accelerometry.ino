#include "config.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

#define ADXL343_SCK 13
#define ADXL343_MISO 12
#define ADXL343_MOSI 11
#define ADXL343_CS 10
// set up the 'analog' feed
AdafruitIO_Feed *accelx = io.feed("accelx");
AdafruitIO_Feed *accely = io.feed("accely");
AdafruitIO_Feed *accelz = io.feed("accelz");




/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/* NeoTrellis M4, etc.                    */
/* Uncomment following line for Wire1 bus */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(12345, &Wire1);

/* Uncomment for software SPI */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(ADXL343_SCK, ADXL343_MISO, ADXL343_MOSI, ADXL343_CS, 12345);

/* Uncomment for hardware SPI */
//Adafruit_ADXL343 accel = Adafruit_ADXL343(ADXL343_CS, &SPI, 12345);


void displayDataRate(void)
{
  Serial.print  ("Data Rate:    ");

  switch(accel.getDataRate())
  {
    case ADXL343_DATARATE_3200_HZ:
      Serial.print  ("3200 ");
      break;
    case ADXL343_DATARATE_1600_HZ:
      Serial.print  ("1600 ");
      break;
    case ADXL343_DATARATE_800_HZ:
      Serial.print  ("800 ");
      break;
    case ADXL343_DATARATE_400_HZ:
      Serial.print  ("400 ");
      break;
    case ADXL343_DATARATE_200_HZ:
      Serial.print  ("200 ");
      break;
    case ADXL343_DATARATE_100_HZ:
      Serial.print  ("100 ");
      break;
    case ADXL343_DATARATE_50_HZ:
      Serial.print  ("50 ");
      break;
    case ADXL343_DATARATE_25_HZ:
      Serial.print  ("25 ");
      break;
    case ADXL343_DATARATE_12_5_HZ:
      Serial.print  ("12.5 ");
      break;
    case ADXL343_DATARATE_6_25HZ:
      Serial.print  ("6.25 ");
      break;
    case ADXL343_DATARATE_3_13_HZ:
      Serial.print  ("3.13 ");
      break;
    case ADXL343_DATARATE_1_56_HZ:
      Serial.print  ("1.56 ");
      break;
    case ADXL343_DATARATE_0_78_HZ:
      Serial.print  ("0.78 ");
      break;
    case ADXL343_DATARATE_0_39_HZ:
      Serial.print  ("0.39 ");
      break;
    case ADXL343_DATARATE_0_20_HZ:
      Serial.print  ("0.20 ");
      break;
    case ADXL343_DATARATE_0_10_HZ:
      Serial.print  ("0.10 ");
      break;
    default:
      Serial.print  ("???? ");
      break;
  }
  Serial.println(" Hz");
}

void displayRange(void)
{
  Serial.print  ("Range:         +/- ");

  switch(accel.getRange())
  {
    case ADXL343_RANGE_16_G:
      Serial.print  ("16 ");
      break;
    case ADXL343_RANGE_8_G:
      Serial.print  ("8 ");
      break;
    case ADXL343_RANGE_4_G:
      Serial.print  ("4 ");
      break;
    case ADXL343_RANGE_2_G:
      Serial.print  ("2 ");
      break;
    default:
      Serial.print  ("?? ");
      break;
  }
  Serial.println(" g");
}
void setup(void)
{
  Serial.begin(115200);
  while (! Serial);
  Serial.println("Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);
  // accel.setRange(ADXL343_RANGE_8_G);
  // accel.setRange(ADXL343_RANGE_4_G);
  // accel.setRange(ADXL343_RANGE_2_G);

  /* Display some basic information on this sensor */
  accel.printSensorDetails();
  displayDataRate();
  displayRange();
  Serial.println("");

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}
float xPast = 0;
float yPast = 0;
float zPast = 0;
float xVals[] = {0.0, 0.0, 0.0};
float yVals[] = {0.0, 0.0, 0.0};
float zVals[] = {0.0, 0.0, 0.0};
void loop(void)
{
  io.run();
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);
  bool isHigh = false;
  int count = 0;
 
  float z = event.acceleration.z;
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  xVals[count] = x;
  yVals[count] = y;
  zVals[count] = z;
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  bool isX = getAvg(xVals) > 10 && (abs(x - xPast) > 20 || abs(xPast - x) > 20)
        && xPast != 0
        && !checkZeros(xVals);
  bool isY = getAvg(yVals) > 10 && (abs(y - yPast) > 20 || abs(yPast - y) > 20)
        && yPast != 0
        && !checkZeros(yVals);
  bool isZ = getAvg(zVals) > 10 && (abs(z - zPast) > 20 || abs(zPast - z) > 20)
        && zPast != 0
        && !checkZeros(zVals);
  if(isX){
    Serial.print("Seizure");
    isHigh = true;
    delay(15000);
  }
  if(isY){
    Serial.print("Seizure");
    isHigh = true;
    delay(15000);
  }
  if(isZ){
    Serial.print("Seizure");
    isHigh = true;
    delay(15000);
  }
  if(isHigh){
    xPast = 0;
    yPast = 0;
    zPast = 0;
    resetZeros(xVals);
    resetZeros(yVals);
    resetZeros(zVals);
  } else{
    xPast = x;
    yPast = y;
    zPast = z;
  }

  count = count + 1;
  if(count > 4){
    count = 0;
  }
 
 
  accelx->save(event.acceleration.x);
  accely->save(event.acceleration.y);
  accelz->save(event.acceleration.z);
  delay(1000);
}

float getAvg(float arr[]){
  int i;
  int sum = 0;
  for(i = 0; i < sizeof(arr); i++){
    sum = sum + abs(arr[i]);
  }
  return sum;
}

bool checkZeros(float arr[]){
  int i;
  int sum = 0;
  for(i = 0; i < sizeof(arr); i++){
    if(arr[i] = 0){
      return true;
    }
  }
  return false;
}

void resetZeros(float arr[]){
  int i;
  for(i = 0; i < sizeof(arr); i++){
    arr[i] = 0.0;
  }
}

void handleMessage(AdafruitIO_Data *data) {
 
  Serial.print("received <- ");
 
}
