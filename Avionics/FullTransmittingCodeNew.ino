
// include the library for Radio Module
#include <RadioLib.h>

// SX1276 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1276 radio = new Module(10, 2, 9, 3);

// Include libraries for the sensors
#include <SD.h>
#include <Wire.h>
#include <SPI.h>

// Include revelant libraries for BMP388 Sensor
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

// Include revelant libraries for ICM20649 Sensor
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>

// Define pins
#define SENSOR_SCK 13
#define SENSOR_MISO 12
#define SENSOR_MOSI 11

Adafruit_ICM20649 icm;
uint16_t measurement_delay_us = 65535; // Delay between measurements for testing
// For SPI mode, we need a CS pin
#define ICM_CS 36

#define BMP_CS 37

#define SEALEVELPRESSURE_HPA (1002.24) // get readings and define zero whereever you are

Adafruit_BMP3XX bmp;

// GPS module
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.
const int chipSelect = BUILTIN_SDCARD;

String str = "";
int package = 0;
double temp;
double pressure;
double accel_x;
double accel_y;
double accel_z;
double gyro_x;
double gyro_y;
double gyro_z;
byte SIV = 0;

// To transmit to ground
long latitude = 0;
long longitude = 0;
double altitude_m;


double oldTime = millis();


// All encoding works by turning the data into array of bits

int encode(double x, uint8_t* e, int s) {
  uint8_t* d = (uint8_t*) &x;
  for (int i = 0; i < sizeof(double); i++) {
    e[s + i] = d[i];
  }
  return s + 8;
}

// Encode shorts
int encode(unsigned short x, uint8_t* e, int s) {
  e[s] = (uint8_t)(x >>  0);
  e[s + 1] = (uint8_t)(x >>  8);
  return s + 2;
}


void setup() {
  Serial.begin(115200);
  
  // initialize SX1278 with default settings
  Serial.print(F("[SX1276] Initializing ... "));
  int state = radio.begin(915.0,500.0,12,5,RADIOLIB_SX127X_SYNC_WORD,20,6,0); //
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  // RX enable:   4
  // TX enable:   5
  
  radio.setRfSwitchPins(4, 5);

  Serial.println("Adafruit BMP388 / BMP390 test");
    
  if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  // Initialize ICM20649 Sensor
  
  Serial.println("Adafruit ICM20649 test!");
  // Try to initialize!
  if (!icm.begin_SPI(ICM_CS)) {
    Serial.println("Failed to find ICM20649 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("ICM20649 Found!");
      // icm.setAccelRange(ICM20649_ACCEL_RANGE_4_G);
      Serial.print("Accelerometer range set to: ");
      switch (icm.getAccelRange()) {
      case ICM20649_ACCEL_RANGE_4_G:
        Serial.println("+-4G");
        break;
      case ICM20649_ACCEL_RANGE_8_G:
        Serial.println("+-8G");
        break;
      case ICM20649_ACCEL_RANGE_16_G:
        Serial.println("+-16G");
        break;
      case ICM20649_ACCEL_RANGE_30_G:
        Serial.println("+-30G");
        break;
      }
    
      // icm.setGyroRange(ICM20649_GYRO_RANGE_500_DPS);
      Serial.print("Gyro range set to: ");
      switch (icm.getGyroRange()) {
      case ICM20649_GYRO_RANGE_500_DPS:
        Serial.println("500 degrees/s");
        break;
      case ICM20649_GYRO_RANGE_1000_DPS:
        Serial.println("1000 degrees/s");
        break;
      case ICM20649_GYRO_RANGE_2000_DPS:
        Serial.println("2000 degrees/s");
        break;
      case ICM20649_GYRO_RANGE_4000_DPS:
        Serial.println("4000 degrees/s");
        break;
      }
      //  icm.setAccelRateDivisor(4095);
      uint16_t accel_divisor = icm.getAccelRateDivisor();
      float accel_rate = 1125 / (1.0 + accel_divisor);
    
      Serial.print("Accelerometer data rate divisor set to: ");
      Serial.println(accel_divisor);
      Serial.print("Accelerometer data rate (Hz) is approximately: ");
      Serial.println(accel_rate);
    
      //  icm.setGyroRateDivisor(255);
      uint8_t gyro_divisor = icm.getGyroRateDivisor();
      float gyro_rate = 1100 / (1.0 + gyro_divisor);
    
      Serial.print("Gyro data rate divisor set to: ");
      Serial.println(gyro_divisor);
      Serial.print("Gyro data rate (Hz) is approximately: ");
      Serial.println(gyro_rate);
      Serial.println();

      Serial.println("SparkFun u-blox Example");
      Wire.begin();
      //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial
      if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
      {
        Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
        while (1);
      }
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1) {
      // No SD card, so don't do anything more - stay stuck here
    }
  }
  Serial.println("card initialized.");
}

int PACKET_SIZE = 6;

void loop() {  
  // loop through BMP388
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  temp = bmp.temperature;
  pressure = bmp.pressure/1000;
  altitude_m = bmp.readAltitude(SEALEVELPRESSURE_HPA);

  uint8_t e[PACKET_SIZE];
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" *C");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" kPa");
  Serial.print("Altitude: ");
  Serial.print(altitude_m);
  Serial.println(" m");
  str = String(temp,2) + "," + String(pressure,3) + "," + String(altitude_m,2) + ",";
  
  // loop through ICM20649
  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  icm.getEvent(&accel, &gyro, &temp);
  // read values from ICM20649
  accel_x = accel.acceleration.x;
  accel_y = accel.acceleration.y;
  accel_z = accel.acceleration.z;
  gyro_x = gyro.gyro.x;
  gyro_y = gyro.gyro.y;
  gyro_z = gyro.gyro.z;

  //  The BMP388 already prints the temperature
  //  Serial.print("Temperature ");
  //  Serial.print(temp.temperature);
  //  Serial.println(" deg C");
  
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("Accel X: ");
  Serial.print(accel_x);
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.print(accel_y);
  Serial.print(" ");
  Serial.print("Z: ");
  Serial.print(accel_z);
  Serial.println(" m/s^2");
  
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("Gyro X: ");
  Serial.print(gyro_x);
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.print(gyro_y);
  Serial.print(" ");
  Serial.print("Z: ");
  Serial.print(gyro_z);
  Serial.println(" radians/s");

  // send the GPS data
  
  if (millis() - lastTime > 10000)
  {
    lastTime = millis(); //Update the timer
    
    latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    SIV = myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.println(SIV);


    
  }
  // prepare the package to be sent
  str += String(accel_x,2) + "," + String(accel_y,2) + "," + String(accel_z,2) + ",";
  str += String(gyro_x,2) + "," + String(gyro_y,2) + "," + String(gyro_z,2)+ ",";
  str += String(latitude,2) + "," + String(longitude,2) + "," + String(SIV,2);
//  
//  Serial.print(F("[SX1276] Transmitting packet ... "));
//  // you can transmit C-string or Arduino string up to
//  // 256 characters long
//  // NOTE: transmit() is a blocking method!
//  //       See example SX127x_Transmit_Interrupt for details
//  //       on non-blocking transmission method.
//  

// Unsigned shorts for transmission data
// VERIFY BEFOREHAND THAT UNSIGNED SHORTS CAN BE USED (Signs should not change for latitude and longitude?)

  unsigned short uAltitude;
  unsigned short uLatitude;
  unsigned short uLongitude;

  uAltitude = (unsigned short) altitude_m;
  uLatitude = (unsigned short)latitude;
  uLongitude = (unsigned short)longitude;

  encode(uAltitude, e, 0);
  encode(uLatitude, e, 2);
  encode(uLongitude, e, 4);

  Serial.print(F("altitude_m: "));
  Serial.println(altitude_m);
  Serial.print(F("uAltitude: "));
  Serial.println(uAltitude);

  package = radio.transmit(e, PACKET_SIZE);
//
//  if (package == RADIOLIB_ERR_NONE) {
//    // the packet was successfully transmitted
//    Serial.println(F(" success!"));
//
//    // print measured data rate
//    Serial.print(F("[SX1276] Datarate:\t"));
//    Serial.print(radio.getDataRate());
//    Serial.println(F(" bps"));
//    Serial.println(F("Package End"));
//    Serial.println();
//
//  } else if (package == RADIOLIB_ERR_PACKET_TOO_LONG) {
//    // the supplied packet was longer than 256 bytes
//    Serial.println(F("too long!"));
//
//  } else if (package == RADIOLIB_ERR_TX_TIMEOUT) {
//    // timeout occurred while transmitting packet
//    Serial.println(F("timeout!"));
//
//  } else {
//    // some other error occurred
//    Serial.print(F("failed, code "));
//    Serial.println(package);
//  }

  File dataFile = false; //SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(str);
    dataFile.close();
    Serial.println("logged to SD card");
  } else {
    // if the file isn't open, pop up an error:
    Serial.println("error opening datalog.txt");
  }
  double currentTime = millis();
  double timeDiff = currentTime - oldTime;
  oldTime = currentTime;
  Serial.println(timeDiff);

  // wait for a second before transmitting again
//  delay(1000);
}
