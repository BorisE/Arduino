//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Weather station for Amateur Observatories
// by Boris Emchenko
// http://astromania.info
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Changes:
//   ver 0.8i2 [26 644/29 834] - also get current params
//   ver 0.8i [26 474/28 954] - can set param values from serial; also get
//   ver 0.8h2 [25 716/28 954] - overheating protection algorithm chage (temp delta)
//   ver 0.8h [25 460/28 922] - send relay state to server
//   ver 0.8g [25 388/28 630] - auto heating on during wet; auto off after heating time exceeded; added debug mode with program runtime calc; code optimization
//   ver 0.8f [28 506] - added overheating protection
//   ver 0.8e [28 410] - added reading from serial for external commands (relay controlling up to now)
//   ver 0.8d [27 846] - adding relay support
//   ver 0.7d [27 638] - dacha settings
//   ver 0.7h [27 654] - added Rain Gauge (RG-11) support
//   ver 0.6 [27 008] - export to serial to read from computer
//   ver 0.5d2 [26 050] - dacha settings
//   ver 0.5d [25 978] - dacha settings
//   ver 0.5b2 [25 978] - added 2nd dht22 sensor
//   ver 0.5b [25 440] - added wet sensor
//   ver 0.4b2 [24 986] - Some errors in BH sensors calculations
//   ver 0.4b [24 978] - Some optimization (removed DallasTemperature library)
//   ver 0.3a [27 318][26 748] - added onewire termometers. Removed LCD, because of out of mem
//   ver 0.2a - added ethernet module
//   ver 0.1a - first version. Included: MLX, DHT22, BH1750, BMP085. Also added (temp?) LCD support.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <i2cmaster.h> //for mlx
#include <Wire.h> //for bmp085 & BH1750
#include <DHT.h> //for DHT22
#include <LCD.h>
//LCDOFF #include <LiquidCrystal_I2C.h>
#include <SPI.h> //for Ethernet
#include <Ethernet.h>
#include <OneWire.h>

// UNCOMENT TO USE DEBUG MODE (Verbose, but memory consuming)
//#define DEBUG_FLAG 1

// UNCOMENT TO USE DEBUG MODE FOR HEATING RELAY (Verbose, but memory consuming)
#define DEBUG_FLAG_HEATING 1

// UNCOMENT TO USE DEBUG MODE FOR WEB ACTIVITY (Verbose, but memory consuming)
//#define DEBUG_FLAG_WEBACTIVITY 1

//MLX block----------------------------------------
#define MLX_ADRDRESS 0x5A //MLX device address prefix
#define MLX_objectTemp 0x07 //MLX address for object temp measurement
#define MLX_ambientTemp 0x06 //MLX address for ambient temp measurement
//----------------------------------------MLX block

//BMP085 data----------------------------------------
#define BMP085_ADDRESS 0x77  // I2C address of BMP085

const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
int ac1; int ac2; int ac3;
unsigned int ac4; unsigned int ac5; unsigned int ac6;
int b1; int b2; int mb; int mc; int md;
// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5; 
//---------------------------------------BMP085 block

//DHT22------------------------------------------------
#define DHTPIN 5     // what pin we're connected to
#define DHTPIN2 9     // what pin we're connected to
DHT dht(DHTPIN, DHT22);
DHT dht2(DHTPIN2, DHT11);
//--------------------------------------------DHT22 block

//BH1750------------------------------------------------
#define BH1750_ADRDRESS 0x23 //setting i2c address

//Setting resolution
#define BH1750_RES_1LX 0x10      //0x10 16 0001_0000 Cont, res:   1Lx,   time: 120ms
#define BH1750_RES_05LX 0x11    //0x11 17 0001_0001 Cont, res: 0.5Lx, time: 120ms
#define BH1750_RES_4LX 0x13      //0x13 0001_0011 Cont, res:   4Lx,   time: 16ms

//Sensitivity (exposure)
#define BH1750_SENS_DEFAULT 0x45 //69, 0100_0101  sens x1
#define BH1750_SENS_MIN 0x1F     //31, 0001_1111  sens x0.45
#define BH1750_SENS_MAX 0xFE     //254, 1111_1110 sens x3.68

//Base waiting time for data measurement
#define BH1750_RES_1LX_WAITTIME 150 //in datasheet = 120
#define BH1750_RES_4LXL_WAITTIME 50 //in datasheet = 16

byte BH1750_res=BH1750_RES_1LX; //set initial resolution
byte BH1750_sens=BH1750_SENS_DEFAULT; //set initial sensetiveness (exposure time)
byte BH1750_buff[2];
int BH1750_wait_time;
float BH1750_val=0;
//--------------------------------------------BH1750 block

//LCD2004 i2c ------------------------------------------------
//LCDOFF 
/*
#define I2C_ADDR    0x3F  // Define I2C Address where the PCF8574A is
 #define BACKLIGHT_PIN     3
 #define En_pin  2
 #define Rw_pin  1
 #define Rs_pin  0
 #define D4_pin  4
 #define D5_pin  5
 #define D6_pin  6
 #define D7_pin  7
 
 int n = 1;
 
 LiquidCrystal_I2C       lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
 */
//--------------------------------------------LCD2004 i2c block

//--- One-wire -----------------------------------------------------------------------------
#define ONE_WIRE_BUS 3 // Data wire is plugged into port 3 on the Arduino
//#define ONEWIRE_TEMPERATURE_PRECISION 11 //9 (0.5), 10 (0.25), 11 (0.125), 12 (0.0625)

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// arrays to hold device addresses
uint8_t OW_Temp1Addr[8] = { 0x28, 0x0B, 0x0D, 0x35, 0x3, 0x0, 0x0, 0x27 };
uint8_t OW_Temp2Addr[8]   = { 0x28, 0xA7, 0x30, 0x35, 0x03, 0x0, 0x0, 0x6E };
float OW_Temp1=-100;
float OW_Temp2=-100;
//-----------------------------------------------------------------------------One-wire

//--- wet sensor -----------------------------------------------------------------------------
#define WET_DigitalPin 8
#define WET_AnalogPin 2
uint16_t WET_SensorValue;
uint8_t WET_digitalValue;
//-----------------------------------------------------------------------------wet sensor----

//--- Rain Gauge ------------------------------------------------------------------------------
#define RAIN_GAUGE_INTERRRUPT_PIN 0 //Interrupt 0 is on DIGITAL PIN 2
volatile uint16_t RainGaugeCounter=0;
uint16_t RG_to_send=0;
//----------------------------------------------------------------------------- Rain Gauge ----

//--- Relay block -----------------------------------------------------------------------------
#define RELAY_1_PIN 7 //DIGITAL PIN FOR CONTROLLING RELAY
#define RELAY_OFF HIGH
#define RELAY_ON LOW
#define TEMP_DELTA_LIMIT_DEFAULT 25
uint8_t TEMP_DELTA_LIMIT=TEMP_DELTA_LIMIT_DEFAULT;
#define WET_THRESHOLD_DEFAULT 1000
uint16_t WET_THRESHOLD=WET_THRESHOLD_DEFAULT;
#define RELAY_RUN_TIME_LIMIT_DEFAULT 180000 //3 min
unsigned long RELAY_RUN_TIME_LIMIT=RELAY_RUN_TIME_LIMIT_DEFAULT;
uint8_t RelState=0;
uint8_t RelStateRaw=RELAY_OFF;
unsigned long relaystart, relayend;
//----------------------------------------------------------------------------- Relay block ----

//--- Serial read block -----------------------------------------------------------------------------
String cmdbuf,cmdnamebuf,cmdvalbuf;
char bufch;
bool startflag=false,cmdnameflag=false, cmdvalflag=false;
//----------------------------------------------------------------------------- Serial read block ----

//---Ethernet block------------------------------------------------------------------------------------
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF}; // assign a MAC address for the ethernet controller.

IPAddress ip(192,168,1,178); // fill in an available IP address on your network here
IPAddress myDns(192,168,1,1); // fill in your Domain Name Server address here

const char WEBSERVER[] = "astromania.info";
IPAddress WEBSERVER_IP(77,221,130,25); 
const char WEBPAGE[]= "/checkconn/putweatherdata.php?";
const int WEBSERVER_PORT=80;

#define postingInterval 20000  // delay between updates, in milliseconds

#ifdef DEBUG_FLAG
  unsigned long timestart=0, timeend=0;
#endif
   
EthernetClient WebClient; // initialize the library instance:

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
//-----------------------------------------------------------------------------Ethernet block

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.begin(9600);
  Serial.println("Weather station v0.8i");
  Serial.println("[!ver:0.8i]");
  Serial.println("[!ved:03092014]");

  //mlx part
  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups

  //bmp085 part
  Wire.begin(); //also for BH1750
  bmp085Calibration();

  //DHT22 part
  dht.begin();
  dht2.begin();

  // OneWire setup

  //LCD part
  //LCDOFF lcd.begin (20,4,LCD_5x8DOTS);
  //LCDOFF lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE); // init the backlight

  //wet sensor setup
  pinMode(WET_DigitalPin, INPUT);  

  //add interrupt for rain gauge
  attachInterrupt(RAIN_GAUGE_INTERRRUPT_PIN, rainGaugeStateChange, RISING);

  //Relay setup
  pinMode(RELAY_1_PIN, OUTPUT);
  RelayHeatingOFF();
  
  //Ethernet part
  delay(1000); // give the ethernet module time to boot up
  Ethernet.begin(mac, ip, myDns); // start the Ethernet connection using a fixed IP address and DNS server
  // print the Ethernet board/shield's IP address
  Serial.print("IP: "); 
  Serial.println(Ethernet.localIP());
  Serial.println();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{

  #ifdef DEBUG_FLAG
    timestart = millis();
  #endif  

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Read data from serial:
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ProcessSerial();
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Start measurement  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.println("=== New cycle ===");
  delay(300);
  Serial.println("[!!be:1]");
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //mlx part
  double objectTemp = 0.0;
  double ambientTemp = 0.0;

  objectTemp = readMLX(MLX_objectTemp);
  ambientTemp=readMLX(MLX_ambientTemp);

  Serial.print("[!Obj:");
  Serial.print(objectTemp,1);
  Serial.println("]");
  Serial.print("[!Amb:");
  Serial.print(ambientTemp,1);
  Serial.println("]");
  Serial.println();//line break

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //bmp085 part
  float bmp085_temperature = bmp085GetTemperature(bmp085ReadUT()); //MUST be called first
  float bmp085_pressure = bmp085GetPressure(bmp085ReadUP());
  float bmp085_pressure_mmHg = bmp085_pressure/133.322;


  #ifdef DEBUG_FLAG
    float bmp085_atm = bmp085_pressure / 101325; // "standard atmosphere"
    float bmp085_altitude = calcAltitude(bmp085_pressure); //Uncompensated caculation - in Meters 
    
    Serial.print("Temperature: ");
    Serial.print(bmp085_temperature, 2); //display 2 decimal places
    Serial.println("C");
  
    Serial.print("Pressure: ");
    Serial.print(bmp085_pressure, 0); //whole number only.
    Serial.print(" Pa   ");
    Serial.print(bmp085_pressure_mmHg);
    Serial.println(" mmHg");
    Serial.print("Standard Atmosphere: ");
    Serial.println(bmp085_atm, 4); //display 4 decimal places
    Serial.print("Altitude: ");
    Serial.print(bmp085_altitude, 2); //display 2 decimal places
    Serial.println(" M");
  #endif
  
  Serial.print("[!BTe:");
  Serial.print(bmp085_temperature, 1); //display 2 decimal places
  Serial.println("]");
  Serial.print("[!Pre:");
  Serial.print(bmp085_pressure_mmHg); //display 2 decimal places
  Serial.println("]");


  Serial.println();//line break

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //DHT22 part
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float DHT22_h = dht.readHumidity();
  float DHT22_t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(DHT22_t) || isnan(DHT22_h)) {
    Serial.println("Failed to read from DHT");
    DHT22_t=-100;
    DHT22_h=-1;
  } 
  else 
  {
    #ifdef DEBUG_FLAG
      Serial.print("Humidity: "); 
      Serial.print(DHT22_h);
      Serial.println(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(DHT22_t);
      Serial.println(" C  ");
  
      float dp=dewPoint(DHT22_t,DHT22_h);
      float dp2=dewPointFast(DHT22_t,DHT22_h);
      Serial.print("Dew point: "); 
      Serial.print(dp);
      Serial.print(" /fast: ");
      Serial.print(dp2);
      Serial.println(" *C");
    #endif
    Serial.print("[!DT1:"); 
    Serial.print(DHT22_t,1);
    Serial.println("]");
    Serial.print("[!DH1:"); 
    Serial.print(DHT22_h,1);
    Serial.println("]");
  }
  Serial.println();//line break

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //DHT22 II part
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float DHT22_2_h = dht2.readHumidity();
  float DHT22_2_t = dht2.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(DHT22_2_t) || isnan(DHT22_2_h)) {
    DHT22_2_t=-100;
    DHT22_2_h=-1;
    Serial.println("Failed to read from DHT-2");
  } 
  else {
    #ifdef DEBUG_FLAG
      Serial.print("Humidity(2): "); 
      Serial.print(DHT22_2_h);
      Serial.println(" %\t");
      Serial.print("Temperature(2): "); 
      Serial.print(DHT22_2_t);
      Serial.println(" C  ");
  
      //float dp=dewPoint(DHT22_2_t,DHT22_2_h);
      float dp2=dewPointFast(DHT22_2_t,DHT22_2_h);
      Serial.print("Dew point (fast): "); 
      Serial.print(dp2);
      //Serial.print(" /fast: ");
      //Serial.print(dp2);
      Serial.println(" *C");
    #endif
    Serial.print("[!DT2:"); 
    Serial.print(DHT22_2_t,1);
    Serial.println("]");
    Serial.print("[!DH2:"); 
    Serial.print(DHT22_2_h,1);
    Serial.println("]");
  }
  Serial.println();//line break

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // BH1750  
  int i;
  double k=1;
  String BH1750_res_st="";

  if (BH1750_val < 10) {
    //Initializing sensitivity mode
    BH1750_Set_Sensitivity(BH1750_ADRDRESS,BH1750_SENS_MAX);
    //Initializing resolution mode
    BH1750_Set_Resolution(BH1750_ADRDRESS,BH1750_RES_05LX);
  } 
  else if (BH1750_val < 1000) {
    //Initializing sensitivity mode
    BH1750_Set_Sensitivity(BH1750_ADRDRESS,BH1750_SENS_DEFAULT);
    //Initializing resolution mode
    BH1750_Set_Resolution(BH1750_ADRDRESS,BH1750_RES_1LX);
  } 
  else {
    //Initializing sensitivity mode
    BH1750_Set_Sensitivity(BH1750_ADRDRESS,BH1750_SENS_MIN);
    //Initializing resolution mode
    BH1750_Set_Resolution(BH1750_ADRDRESS,BH1750_RES_4LX);
  }  

  //Wait for data measurement
  BH1750_Wait_time();

  //Read data
  if(2==BH1750_Read(BH1750_ADRDRESS))
  //if all 2 bytes was read, then print data
  {
    BH1750_val=(((uint16_t)BH1750_buff[0]<<8) | BH1750_buff[1]) / 1.2;
    k= ((double)BH1750_SENS_DEFAULT / BH1750_sens);
    BH1750_val=BH1750_val * k;
    BH1750_res_st="1";
    if (BH1750_RES_05LX==BH1750_res) {
      BH1750_val=BH1750_val/2;
      BH1750_res_st="0.5";
    }
    else if (BH1750_RES_4LX==BH1750_res) {
      BH1750_res_st="4";
    }
    #ifdef DEBUG_FLAG
      Serial.print("Illuminance: "); 
      Serial.print(BH1750_val,1);     
      Serial.print("["+BH1750_res_st+"x]"); 
      Serial.print(" r:"); 
      Serial.print(BH1750_res); 
      Serial.print(" s:"); 
      Serial.print(BH1750_sens); 
      Serial.print(" wt:"); 
      Serial.print(BH1750_wait_time); 
      Serial.println();
    #endif
    Serial.print("[!Lum:"); 
    Serial.print(BH1750_val,1);     
    Serial.println("]");
    Serial.print("[!Lur:"); 
    Serial.print(BH1750_res); 
    Serial.println("]");
    Serial.print("[!Lus:"); 
    Serial.print(BH1750_sens); 
    Serial.println("]");
    Serial.print("[!Luw:"); 
    Serial.print(BH1750_wait_time); 
    Serial.println("]");

    Serial.println();
  }
  else
  {
    BH1750_val=-1;
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // OneWire part
  float OW_Temp1 = getOneWireTemp(OW_Temp1Addr);
  float OW_Temp2 = getOneWireTemp(OW_Temp2Addr);

  #ifdef DEBUG_FLAG
    Serial.print("Inside t: ");
    Serial.print(OW_Temp1);
    Serial.print("C ");
    Serial.print("Outside t: ");
    Serial.print(OW_Temp2);
    Serial.print("C ");
    Serial.println();
  #endif
  
  Serial.print("[!Te1:"); 
  Serial.print(OW_Temp1,1);
  Serial.println("]");
  Serial.print("[!Te2:"); 
  Serial.print(OW_Temp2,1);
  Serial.println("]");

  Serial.println();

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // wetsensor
  WET_SensorValue = analogRead(WET_AnalogPin);
  WET_digitalValue = digitalRead(WET_DigitalPin);
  
  #ifdef DEBUG_FLAG
  
    Serial.print("Wetsensor analog value: ");
    Serial.print(WET_SensorValue);
    Serial.print("     digital value: ");
    Serial.println(WET_digitalValue);
  #endif
  
  Serial.print("[!Wet:"); 
  Serial.print(WET_SensorValue);
  Serial.println("]");

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Rain Gauge 
  RG_to_send=RainGaugeCounter;
  RainGaugeCounter=0;

  #ifdef DEBUG_FLAG
    Serial.print("Rain Gauge counter: ");
    Serial.println(RG_to_send);
  #endif
  
  Serial.print("[!RGC:"); 
  Serial.print(RG_to_send);
  Serial.println("]");

  Serial.println();

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // RELAY BLOCK
  RelStateRaw=digitalRead(RELAY_1_PIN);
  if (RelStateRaw==RELAY_OFF)
  {
    RelState=0;
  }
  else
  {
    RelState=1;
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
    if (RelState==1)
    { 
      Serial.print("Heating temp: ");
      Serial.print(ambientTemp);
      Serial.print(" (limit: ");
      Serial.print(OW_Temp1+TEMP_DELTA_LIMIT);
      Serial.println(")");
      
      Serial.print("Heating time: ");
      Serial.print(relayend);
      Serial.print(" (limit: ");
      Serial.print(RELAY_RUN_TIME_LIMIT);
      Serial.println(")");
    }
  #endif

  if (RelState==1 && ambientTemp>(OW_Temp1+TEMP_DELTA_LIMIT))
  {
    Serial.println("Temp is too high. SWITCHING OFF");
    RelayHeatingOFF();
  }
  else if (RelState==1 && relayend>RELAY_RUN_TIME_LIMIT)
  {
    Serial.println("Heating time is too long. SWITCHING OFF");
    RelayHeatingOFF();
  }
  else if (RelState==0 && WET_SensorValue < WET_THRESHOLD)
  {
    Serial.println("Wet. SWITCHING ON heating relay");
    RelayHeatingON();
  }

  Serial.println();
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //LAST MEASUREMENT FLAG
  Serial.println("[!!en:1]");

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // LCD
  //LCDOFF 
  /*
    lcd.clear(); // clear display, set cursor position to zero
   lcd.setBacklight(HIGH);     // Backlight on
   
   lcd.setCursor (0,0); 
   lcd.print("OBJ/AMB: ");
   lcd.print(objectTemp);
   lcd.print("/");
   lcd.print(ambientTemp);
   
   lcd.setCursor (0,1);        // go col 0 of line 1
   lcd.print("P: ");
   lcd.print(bmp085_pressure_mmHg,1);
   lcd.print("mm T:");
   lcd.print(bmp085_temperature);
   
   lcd.setCursor (0,2);        // go col 0 of line 2
   lcd.print("H: ");
   lcd.print(DHT22_h,1);
   lcd.print("% T:");
   lcd.print(DHT22_t,1);
   
   
   lcd.setCursor (0,3);        // go col 0 of line 3
   lcd.print("I: ");
   lcd.print(BH1750_val,1);
   lcd.print("["+BH1750_res_st+"x]"); 
   lcd.print(" s:"); 
   lcd.print(BH1750_sens); 
   */



  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Ethernet part   
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // if there's incoming data from the net connection send it out the serial port.  
  // This is for debugging purposes only:

  #ifdef DEBUG_FLAG_WEBACTIVITY
    while (WebClient.available()) {
      char c = WebClient.read();
      Serial.print(c);
    }
  #endif
  
  // if there's no net connection, but there was one last timethrough the loop, then stop the client:
  if (!WebClient.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    WebClient.stop();
  }

  //Serial.print(millis() );
  //Serial.print(" - ");
  //Serial.print(lastConnectionTime);
  //Serial.print(" - ");
  //Serial.print(postingInterval);
  //Serial.println("!");

  if(!WebClient.connected()) {
    // if you're not connected, and GIVEN seconds have passed since your last connection, then connect again and send data:
    if (millis() - lastConnectionTime > postingInterval) {
      httpRequest(objectTemp,ambientTemp,bmp085_pressure_mmHg,bmp085_temperature,DHT22_h,DHT22_t,DHT22_2_h,DHT22_2_t,BH1750_val,BH1750_sens,BH1750_res_st,BH1750_res,BH1750_wait_time, OW_Temp1, OW_Temp2, WET_SensorValue,WET_digitalValue,RG_to_send,RelState);
    }
  }
  // store the state of the connection for next time through the loop:
  lastConnected = WebClient.connected();

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  uint16_t waittime_loop=(postingInterval/2);
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
