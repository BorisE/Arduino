//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Objective Dome
// Device by Oleg Milantiev
// http://astro.milantiev.com
// Firmware by Boris Emchenko
// http://astromania.info
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Changes:
//   ver 0.1a [26 644/29 834] - first version
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <DHT.h> //for DHT22
#include <OneWire.h>
#include <Stepper.h> //stepper motor

// UNCOMENT TO USE DEBUG MODE (Verbose, but memory consuming)
#define DEBUG_FLAG 1

// UNCOMENT TO USE RELAY HEATING BLOCK
//#define RELAY_BLOCK 1



// UNCOMENT TO USE DEBUG MODE FOR HEATING RELAY (Verbose, but memory consuming)
#define DEBUG_FLAG_HEATING 1

// Delay between updates, in milliseconds
#define waittime_loop 100       




//STEPPER------------------------------------------------
int stepsPerRevolution = 200;   // change this to fit the number of steps per revolution
int motorSpeed=100;             // rotation speed in RPM

int currentStepPos = 0;  // number of steps the motor has taken

const int directionOpen = 1;
const int directionClose = -1;
int currentDirection = directionOpen; //-1 or 1 for direction

bool isMoving = false;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
//--------------------------------------------STEPPER block


//Buttons -------------------------------------------------
const int OpenedButtonPin = 2;
const int ClosedButtonPin = 3;
int openedButtonStatus = -1;
int closedButtonStatus = -1;
//--------------------------------------------Buttons block

//DHT22------------------------------------------------
#define DHTPIN 5     // what pin we're connected to
DHT dht(DHTPIN, DHT22);
//--------------------------------------------DHT22 block

//--- One-wire -----------------------------------------------------------------------------
#define ONE_WIRE_BUS 3 // Data wire is plugged into port 3 on the Arduino
//#define ONEWIRE_TEMPERATURE_PRECISION 11 //9 (0.5), 10 (0.25), 11 (0.125), 12 (0.0625)

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// arrays to hold device addresses
uint8_t OW_Temp1Addr[8] = { 0x28, 0x0B, 0x0D, 0x35, 0x3, 0x0, 0x0, 0x27 };
uint8_t OW_Temp2Addr[8]   = { 0x28, 0xA7, 0x30, 0x35, 0x03, 0x0, 0x0, 0x6E };
float OW_Temp1 = -100;
float OW_Temp2 = -100;
//-----------------------------------------------------------------------------One-wire

float ambientTemp =-100;

//--- Relay block -----------------------------------------------------------------------------
#define RELAY_1_PIN 7 //DIGITAL PIN FOR CONTROLLING RELAY
#define RELAY_OFF HIGH
#define RELAY_ON LOW
#define TEMP_DELTA_LIMIT_DEFAULT 25
uint8_t TEMP_DELTA_LIMIT = TEMP_DELTA_LIMIT_DEFAULT;
#define WET_THRESHOLD_DEFAULT 1000
uint16_t WET_THRESHOLD = WET_THRESHOLD_DEFAULT;
#define RELAY_RUN_TIME_LIMIT_DEFAULT 180000 //3 min
unsigned long RELAY_RUN_TIME_LIMIT = RELAY_RUN_TIME_LIMIT_DEFAULT;
uint8_t RelState = 0;
uint8_t RelStateRaw = RELAY_OFF;
unsigned long relaystart, relayend;

//dummy vars
uint16_t WET_SensorValue;

//----------------------------------------------------------------------------- Relay block ----


//--- Serial read block -----------------------------------------------------------------------------
String cmdbuf, cmdnamebuf, cmdvalbuf;
char bufch;
bool startflag = false, cmdnameflag = false, cmdvalflag = false;

bool initMoving = false;
bool forceDirection = false;
//----------------------------------------------------------------------------- Serial read block ----

#ifdef DEBUG_FLAG
unsigned long timestart = 0, timeend = 0;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.begin(9600);
  Serial.println("Objective Dome v0.1");
  Serial.println("[!ver:0.1]");
  Serial.println("[!ved:03092018]");

  //Stepper setup
  myStepper.setSpeed(60);

  //DHT22 part
  dht.begin();

  // OneWire setup

  //Relay setup
  pinMode(RELAY_1_PIN, OUTPUT);
  RelayHeatingOFF();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG_FLAG
  timestart = millis();
#endif

  /*****************************/
  // Start measurement
  /*****************************/
  Serial.println("=== New cycle ===");
  Serial.println("[!!be:1]");

  /*****************************/
  // Read and process data from serial:
  /*****************************/
  ProcessSerial();


  /*****************************/
  // BUTTONS BLOCK
  /*****************************/
  openedButtonStatus = digitalRead(OpenedButtonPin);
  closedButtonStatus = digitalRead(ClosedButtonPin);
  
  Serial.print("[!OPN:");
  Serial.print(openedButtonStatus, 1);
  Serial.println("]");
  Serial.print("[!CLS:");
  Serial.print(closedButtonStatus, 1);
  Serial.println("]");
#ifdef DEBUG_FLAG
  if (openedButtonStatus==1 && closedButtonStatus==0)
  {
    Serial.println("Cover is opened");
  }
  else if (openedButtonStatus==0 && closedButtonStatus==1)
  {
    Serial.println("Cover is closed");
  }
  else if (openedButtonStatus==0 && closedButtonStatus==0)
  {
    Serial.println("Cover is moving");
  }
  else
  {
    Serial.println("Cover position error!");
  }
#endif
  

  /*****************************/
  // STEPPER BLOCK
  /*****************************/
  if (isMoving)
  {
    if (openedButtonStatus==1 || closedButtonStatus==1)
    {
      // Уперлись, останавливаемся
      isMoving = false;
#ifdef DEBUG_FLAG
      Serial.println("Move complete. Stopping");
#endif
    }
    else
    {
      // Сделаем квант движения
      stepperMove();
    }
  }
  else
  {
    if (initMoving)
    {
#ifdef DEBUG_FLAG
      Serial.println("Start moving...");
#endif
      initMoving = false;
      
      if (! forceDirection)
      {
        // Изменим направление
        reverseDirection();
      }
      
      // Сделаем квант движения
      stepperMove();
    }
  }

  /*****************************/
  //Print some settings
  /*****************************/
  Serial.print ("[!SPD:");
  Serial.print (motorSpeed);
  Serial.println("]");

  Serial.print ("[!DIR:");
  Serial.print (currentDirection);
  Serial.println("]");

  Serial.print ("[!POS:");
  Serial.print (currentStepPos);
  Serial.println("]");

  /*****************************/
  //DHT22 part
  /*****************************/
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  /*****************************/
  float DHT22_h = dht.readHumidity();
  float DHT22_t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(DHT22_t) || isnan(DHT22_h)) {
    Serial.println("Failed to read from DHT");
    DHT22_t = -100;
    DHT22_h = -1;
  }
  else
  {
    Serial.print("[!DT1:");
    Serial.print(DHT22_t, 1);
    Serial.println("]");
    Serial.print("[!DH1:");
    Serial.print(DHT22_h, 1);
    Serial.println("]");
#ifdef DEBUG_FLAG
    Serial.print("Humidity: ");
    Serial.print(DHT22_h);
    Serial.println(" %\t");
    Serial.print("Temperature: ");
    Serial.print(DHT22_t);
    Serial.println(" C  ");

    float dp = dewPoint(DHT22_t, DHT22_h);
    float dp2 = dewPointFast(DHT22_t, DHT22_h);
    Serial.print("Dew point: ");
    Serial.print(dp);
    Serial.print(" /fast: ");
    Serial.print(dp2);
    Serial.println(" *C");
#endif
  }
  Serial.println();//line break


  /*****************************/
  // OneWire part
  /*****************************/
  float OW_Temp1 = getOneWireTemp(OW_Temp1Addr);
  float OW_Temp2 = getOneWireTemp(OW_Temp2Addr);

  Serial.print("[!Te1:");
  Serial.print(OW_Temp1, 1);
  Serial.println("]");
  Serial.print("[!Te2:");
  Serial.print(OW_Temp2, 1);
  Serial.println("]");
  Serial.println();

#ifdef DEBUG_FLAG
  Serial.print("Inside t: ");
  Serial.print(OW_Temp1);
  Serial.print("C ");
  Serial.print("Outside t: ");
  Serial.print(OW_Temp2);
  Serial.print("C ");
  Serial.println();
#endif


  /*****************************/
  // Assign ambientTemp to one of temp sensors
  /*****************************/
  ambientTemp = OW_Temp1;

  

  /*****************************/
  // RELAY BLOCK
  /*****************************/
#ifdef RELAY_BLOCK
  RelStateRaw = digitalRead(RELAY_1_PIN);
  if (RelStateRaw == RELAY_OFF)
  {
    RelState = 0;
  }
  else
  {
    RelState = 1;
  }
#ifdef DEBUG_FLAG
  Serial.print ("Heating relay state: ");
  Serial.print (RelState);
  Serial.println();
#endif

  Serial.print ("[!RL1:");
  Serial.print (RelState);
  Serial.println("]");

  relayend = millis() - relaystart;

#ifdef DEBUG_FLAG_HEATING
  if (RelState == 1)
  {
    Serial.print("Heating temp: ");
    Serial.print(ambientTemp);
    Serial.print(" (limit: ");
    Serial.print(OW_Temp1 + TEMP_DELTA_LIMIT);
    Serial.println(")");

    Serial.print("Heating time: ");
    Serial.print(relayend);
    Serial.print(" (limit: ");
    Serial.print(RELAY_RUN_TIME_LIMIT);
    Serial.println(")");
  }
#endif

  if (RelState == 1 && ambientTemp > (OW_Temp1 + TEMP_DELTA_LIMIT))
  {
    Serial.println("Temp is too high. SWITCHING OFF");
    RelayHeatingOFF();
  }
  else if (RelState == 1 && relayend > RELAY_RUN_TIME_LIMIT)
  {
    Serial.println("Heating time is too long. SWITCHING OFF");
    RelayHeatingOFF();
  }
  else if (RelState == 0 && WET_SensorValue < WET_THRESHOLD)
  {
    Serial.println("Wet. SWITCHING ON heating relay");
    RelayHeatingON();
  }

  Serial.println();

#endif //RELAY BLOCK


  /*****************************/
  //LAST MEASUREMENT FLAG
  /*****************************/
  Serial.println("[!!en:1]");


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.print("waiting ");
  Serial.println(waittime_loop);
  Serial.println();//line break

  delay(waittime_loop); // wait some time before printing again

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_FLAG
  timeend = millis() - timestart;
  Serial.print("[!!r:");
  Serial.print(timeend);
  Serial.println("]");
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////
