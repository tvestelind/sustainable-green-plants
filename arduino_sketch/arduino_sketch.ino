#include "platform.hpp"

namespace d = dht;
namespace p = platform;

// Define LED pins (PWM pins: 3,5,6,9,10,11 on arduino UNO)
const int RED   = 11;
const int GREEN = 10;
const int BLUE  = 9;
const int PUMP  = 2;
const int NOZZLE = 12;
const int FAN = 7;
const int BOARDLED = 13; // Pin 13 and Arduino UNO board LED
const int runInterval = 1;
const int measureInterval = 3600;


// Define DHT11 sensor
dht = new d::DHTSensor(p::DHT);
serial = new Serial(9600);

/**
 * User schedule of all settings
 * (7 settings, 1-9 predefined settinggroups, 1-7 days with 1h resolution)
 */
int num_usr_settings = 3;        // Setting sets available (Default: 3)
int num_usr_settings_day = 1;    // Day setting sets available (Default: 1)
const int NUM_SETTINGS = 7;      // Day setting sets
const int NUM_SETTINGS_DAY = 24; // Hour setting sets
// Setting sets
int usrSch[9][NUM_SETTINGS] = {
  {0,0,0,0,15,2,0}, // Spray the roots at a set interval and duration (Default)
  {0,0,0,1,15,2,0}, // Run the pump (Default)
  {0,0,0,0,20,2,0}, // Night setting (Default)
  {25,25,25,0,20,2,0},
  {50,50,50,0,20,2,0},
  {100,100,100,0,20,2,0},
  {100,100,100,0,20,2,0},
  {100,100,100,0,20,2,0},
  {100,100,100,0,20,2,0}
};
// Day setting sets (0:00-23:00)
int usrSchInd[NUM_SETTINGS][NUM_SETTINGS_DAY] = {
  {2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,1,2,2,2}, // Pump at 10:00 and 20:00
  {2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,1,2,2,2},
  {2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,1,2,2,2},
  {2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,1,2,2,2},
  {2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,1,2,2,2},
  {2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,1,2,2,2},
  {2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,1,2,2,2}
};

// Variables for RGB levels
unsigned int rval = 0;
unsigned int gval = 0;
unsigned int bval = 0;

// Variable for pump
int pumpOn = 0; // 0 off, 1 on
int nozzleInterval = 20; // min interval between nozzle activation >60 == off
int nozzleDuration = 2; // 2 sec active nozzle <0 == off
int fanStatus = 0; // 0 off, 1 on

// Run preprogrammed setup, oneReport after nozzle on
int runProgram = 1;
int oneReport = 0;

// Time
p::TimePoint t = 0;
time_t tDelay = runInterval;
time_t tDelayMin = measureInterval;

// Sensor data
int sensorTempData[] = {0,0}; // temp, humid, light

void setup() {
  // Set the time
  p::setTime(20,0,0,1,1,2014); // hour,min,sec,day,month,year

  // Set pins as outputs
  p::pinMode(p::RED, p::OUTPUT);
  p::pinMode(p::GREEN, p::OUTPUT);
  p::pinMode(p::BLUE, p::OUTPUT);
  p::pinMode(p::PUMP, p::OUTPUT);
  p::pinMode(p::NOZZLE, p::OUTPUT);
  p::pinMode(p::FAN, p::OUTPUT);
  p::pinMode(p::BOARDLED, p::OUTPUT);
  p::digitalWrite(p::BOARDLED, p::LOW);
}

void loop() {
  t = p::now();

  // Keep working as long as data is in the buffer
  while (serial.available() > 0) {
    serialControl();
  }

  if (t < tDelay) {
    return;
  }

  if (runProgram == 1) {
    scheduleFromUser();
    setLight();
    setFan(fanStatus);

    /**
     * The scheduling is per hour, but running the pump for that long is
     * unnessesary since it only takes about 30s to fill the pressure
     * accumulator.
     * Only run the pump during the first minute of the scheduled hour.
     */
    if (pumpOn == 1 && t.getMinute() > 0) {
      pumpOn = 0;
    }
    setPump();
  }
  if (nozzleInterval-1 == t.getMinute() % nozzleInterval) {
    if (nozzleDuration > t.getSecond()) {
      digitalWrite(NOZZLE, HIGH);
      oneReport = 1;
    }
    if (nozzleDuration <= t.getSecond()) {
      digitalWrite(NOZZLE, LOW);
      if (oneReport == 1) {
        oneReport = 0;
        serialReport();
      }
    }
  }
  if (t >= tDelayMin) {
    readSensors();

    if (sensorTempData[0] >= 30 || sensorTempData[1] >= 50) {
      setFan(1);
    }
    if (sensorTempData[0] < 30 && sensorTempData[1] < 50) {
      setFan(0);
    }

    tDelayMin = t + measureInterval;
  }

  tDelay = t + runInterval;
}

/*
 * Messages consist of the letter S followed by
 * int(0-255),int(0-255),int(0-255),int(0-1),int(0-30),int(0-10),int(0-1)
 * S,255,255,255,0,20,5,0 (R,G,B,pump,nozzlieInterval,nozzleDuration,fan)
 */
void serialControl()
{
    char c = serial.read();
    switch (c) {
      case 'S':
      {
        rval   = serial.read();         // First valid integer
        gval   = serial.read();         // Second valid integer
        bval   = serial.read();         // Third valid integer
        pumpOn = serial.read();         // Fourth valid integer
        nozzleInterval = serial.read(); // Fifth valid integer
        nozzleDuration = serial.read(); // Sixth valid integer
        fanStatus = serial.read();      // Seventh valid integer

        setLight();
        setPump();
        setNozzle();
        setFan(fanStatus);
        break;
      }
      case 'R':
      {
        readSensors();
        serialReport();
        break;
      }
      case 'Q':
      {
        if (runProgram == 1) {
          runProgram = 0;
          break;
        }
        if (runProgram == 0) {
          runProgram = 1;
          break;
        }
        break;
      }
      case 'T':
      {
        int h  = serial.read; //First valid integer
        int m  = serial.read; //Second valid integer
        int d  = serial.read; //Third valid integer
        p::setTime(h,m,0,d,1,2014);    //hour,min,sec,day,month,year
        t = p::now();
        tDelay = runInterval;
        tDelayMin = measureInterval;
        break;
      }
      case 'U':
      {
        int k = 0;
        num_usr_settings = serial.read(); // The first int is the number
        while (k < num_usr_settings) {        // of user settings from the user
          for (int i = 0; i < NUM_SETTINGS; i++) {
            usrSch[k][i] = serial.read();
            serial.write(usrSch[k][i]);
            if (i == NUM_SETTINGS - 1) {
              k++;
              serial.write('\n');
            }
            else {
              serial.write(',');
            }
          }
        }
        break;
      }
      case 'I':
      {
        int k = 0;
        num_usr_settings_day = serial.read();
        while (k < num_usr_settings_day) {
          for (int i = 0; i < NUM_SETTINGS_DAY; i++) {
            usrSchInd[k][i] = serial.read();
            serial.write(usrSchInd[k][i]);
            if (i == NUM_SETTINGS_DAY - 1) {
              k++;
              serial.write('\n');
            }
            else{
              serial.write(',');
            }
          }
        }
        break;
      }
    }
}

void scheduleFromUser() {
  // Change every hour
  int x = t.getHour(); // 0-23
  int y = t.getDay() % num_usr_settings_day; // Rotate the days
  int z = usrSchInd[y][x];

  rval = usrSch[z][0];
  gval = usrSch[z][1];
  bval = usrSch[z][2];
  pumpOn = usrSch[z][3];
  nozzleInterval = usrSch[z][4];
  nozzleDuration = usrSch[z][5];
  fanStatus = usrSch[z][6];
}

void serialReport() {
  String reportValues;

  reportValues = "R";
  reportValues = reportValues + rval;
  reportValues = reportValues + ",";
  reportValues = reportValues + gval;
  reportValues = reportValues + ",";
  reportValues = reportValues + bval;
  reportValues = reportValues + ",";
  reportValues = reportValues + pumpOn;
  reportValues = reportValues + ",";
  reportValues = reportValues + nozzleInterval;
  reportValues = reportValues + ",";
  reportValues = reportValues + nozzleDuration;
  reportValues = reportValues + ",";
  reportValues = reportValues + fanStatus;

  reportValues = timeReport() + "S" + sensorTempData[0] + ","
                                    + sensorTempData[1] + reportValues;
  serial.write(reportValues);
}

void setLight()
{
  // Set LED (value between 0 and 255)
  p::analogWrite(p::RED, rval);
  p::analogWrite(p::GREEN, gval);
  p::analogWrite(p::BLUE, bval);
}

/**
 * Turn the pump on or off depending on the value of the global variable pumpOn.
 */
void setPump() {
  if (pumpOn == 1) {
    digitalWrite(PUMP, HIGH);
  }
  if (pumpOn == 0) {
    digitalWrite(PUMP, LOW);
  }
}

void setNozzle()
{
  if (nozzleDuration == 0) {
    digitalWrite(NOZZLE, LOW);
  }
}

void setFan(int setStatus)
{
  if (setStatus == 1) {
    digitalWrite(FAN, HIGH);
  }
  if (setStatus == 0) {
    digitalWrite(FAN, LOW);
  }
}

String timeReport() {
  String timeString;

  timeString = "T";
  timeString = timeString + t.getDay();
  timeString = timeString + ":";
  timeString = timeString + t.getHour();
  timeString = timeString + ":";
  timeString = timeString + t.getMinute();
  timeString = timeString + ":";
  timeString = timeString + t.getSecond();

  return timeString;
}

void readSensors()
{
  dhtSensor();
}


void dhtSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if NaN (not a number) then set to 0
  if (isnan(t) || isnan(h)) {
    sensorTempData[0] = 0;
    sensorTempData[1] = 0;
  }
  else {
    sensorTempData[0] = (int)t;
    sensorTempData[1] = (int)h;
  }
}
