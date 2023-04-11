#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MAX31856.h>
#include "HX711.h" 

#define RO_PIN 9
#define DI_PIN 10
#define DE_RE_PIN 11

Adafruit_ADS1X15 ads; 

// Thermocouple Declaration
Adafruit_MAX31856 tc1 = Adafruit_MAX31856(49, 52, 51, 53);
Adafruit_MAX31856 tc2 = Adafruit_MAX31856(48, 52, 51, 53); 

// Load Cell Declaration
HX711 lc1;
HX711 lc2;
const byte hx1_data_pin = 50; // tension load cell
const byte hx2_data_pin = 52; // torque load cell
const byte hx_sck_pin = 53;   // sck pin
float tension_calibration = -19960;
float torque_calibration = 40950;           

// For Ethernet connection with 485S Module
SoftwareSerial rs485Serial(RO_PIN, DI_PIN); // RO_PIN for Receive, DI_PIN for Transmit

void setup() {
    // For ethernet connection and setting it to transmitter
    pinMode(DE_RE_PIN, OUTPUT);
    digitalWrite(DE_RE_PIN, HIGH); // DAQ MUST BE HIGH TO SEND
    Serial.begin(9600);
    rs485Serial.begin(9600);
    
    // Thermocouple declaration
    tc1.begin();
    tc2.begin();
    tc1.setThermocoupleType(MAX31856_TCTYPE_K);
    tc2.setThermocoupleType(MAX31856_TCTYPE_K); 
    
    // Load Cell Declaration
    lc1.begin(hx1_data_pin, hx_sck_pin);
    lc1.set_scale();
    lc1.tare(); // Reset tension load cell to 0  

    lc2.begin(hx2_data_pin, hx_sck_pin);
    lc2.set_scale();
    lc2.tare(); // Reset torque load cell to 0
    ads.begin();
}
void loop() {
    // gathering raw PT data
    int16_t adc0 = ads.readADC_SingleEnded(0);
    int16_t adc1 = ads.readADC_SingleEnded(1);
    int16_t adc2 = ads.readADC_SingleEnded(2);
    int16_t adc3 = ads.readADC_SingleEnded(3);
    // int16_t adc4 = ads.readADC_SingleEnded(4); (uncomment when adding 5th PT)
    
    adc0 = 0.0801*adc0 - 263;
    adc1 = 0.0796 * adc1 - 262;
    adc2 = 0.08 * adc2 - 248;
    adc3 = 0.08 * adc3 - 248; 
    
    // gathering raw TC data
    // int16_t tc1Val;
    // int16_t tc2Val;  lc1.set_scale(tension_calibration);

    lc2.set_scale(torque_calibration);
    double LoadCell_1 = lc1.get_units();
    double LoadCell_2 = lc2.get_units();

    Serial.print("PT1 1: ");
    Serial.print(adc0);
    Serial.print(" PT2: ");
    Serial.print(adc1);
    Serial.print(" PT3 3: ");
    Serial.print(adc2);
    Serial.print(" PT4: ");
    Serial.print(adc3);
    Serial.print(" LC1: ");
    Serial.print(LoadCell_1);
    Serial.print(" LC2: ");
    Serial.println(LoadCell_2); 
    
    // JUAN'S LOAD CELL CODE HERE
    String pt1 = String(adc0);
    String pt2 = String(adc1);
    String pt3 = String(adc2);
    String pt4 = String(adc3);
    String lc1 = String(LoadCell_1);
    String lc2 = String(LoadCell_2);

    // Everything after this is for ethernet
    digitalWrite(DE_RE_PIN, HIGH);
    delay(1);
    // Printing PT data
    rs485Serial.print("P1:");
    rs485Serial.print(pt1);
    rs485Serial.print(" P2:");
    rs485Serial.print(pt2);
    rs485Serial.print(" P3:");
    rs485Serial.print(pt3);
    rs485Serial.print(" P4:");
    rs485Serial.print(pt4);
    rs485Serial.print(" LC1:");
    rs485Serial.print(lc1);
    rs485Serial.print(" LC2:");
    rs485Serial.print(lc2); 
    
    // Printing load cell data
    // rs4855Serial.write(" Load Cell 1: ");
    // rs4855Serial.write(lc1);
    // rs485Serial.write(" Load Cell 2: ");
    // rs485Serial.write(lc2);  

    rs485Serial.print('\n');
    delay(1); // in the future, if we want to set up a system where we only send the data we want to send by pressing a button/flipping a switch, set that up here. NOT in Mini DAQ. It's never easy.
}