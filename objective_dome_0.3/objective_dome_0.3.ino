//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Objective Dome
// Device by Oleg Milantiev
// http://astro.milantiev.com
// Firmware by Boris Emchenko
// http://astromania.info
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Changes:
//   ver 0.3r [13354/12772] - trying to restore prevously lost full operational sketch
//   ver 0.2 [26 644/29 834] - move to AccelStepper library
//   ver 0.1a [26 644/29 834] - first version
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <AccelStepper.h>

// UNCOMENT TO USE DEBUG MODE (Verbose, but memory consuming)
//#define DEBUG_FLAG 1


//#define RELAY_PRESENT 1     // UNCOMENT TO USE RELAY HEATING BLOCK
//#define DHT_PRESENT 1       // UNCOMENT TO USE DHT
//#define ONEWIRE_PRESENT 1   // UNCOMENT TO USE ONEWIRE


// UNCOMENT TO USE DEBUG MODE FOR HEATING RELAY (Verbose, but memory consuming)
#define DEBUG_FLAG_HEATING 1

// Delay between updates, in milliseconds
#define waittime_loop 5000       




//STEPPER------------------------------------------------
int stepsPerCycle = 1200;           // open/close length cycle
int currentStepPosition = 0;        // current motor position
int stepperInitPosition = 0;        // position, when moving was pressed

float motorMaxSpeed=200.0;          // set maximum speed
float motorAcceleration=50.0;      // set acceleration

int minDistanceBeforeChecking = 10; // don't check buttions until move some distance


int stepperTargetPosition = 0;   // start position

const int directionOpen = 1;
const int directionClose = -1;
int currentDirection = directionOpen; //-1 or 1 for direction

bool isMoving = false;

// initialize the stepper library on pins 8 through 11:
AccelStepper stepper(AccelStepper::HALF4WIRE, 26, 28, 30, 32);

//--------------------------------------------STEPPER block


//Buttons -------------------------------------------------
const int OpenedButtonPin = 12;
const int ClosedButtonPin = 11;
const int OPENEDSTATUS = HIGH; //PULLUP, so normally HIGH (opened). After connecting to ground (closed) goes LOW 
int openedButtonStatus = -1;
int closedButtonStatus = -1;
//--------------------------------------------Buttons block

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
  Serial.println("Objective Dome v0.3r");
  Serial.println("[!ver:0.3r]");
  Serial.println("[!ved:18112018]");

  //Stepper setup
  stepper.setMaxSpeed(100.0);
  stepper.setAcceleration(50.0);
    
  //Button setup
  pinMode(OpenedButtonPin, INPUT_PULLUP);
  pinMode(ClosedButtonPin, INPUT_PULLUP);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG_FLAG
  timestart = millis();
#endif
  /*****************************/
  // Read and process data from serial:
  /*****************************/
  ProcessSerial();

  //Check buttons
  readButtonStatus();

  if (!stepper.isRunning())
  {
      /*****************************/
      // Start measurement
      /*****************************/
      Serial.println("=== New cycle ===");
      Serial.println("[!!be:1]");
  }

  //print buttons stats if no moving and every 50 steps
  if (abs(stepper.currentPosition () -  stepperInitPosition) % 50 == 0)
  {

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
          Serial.println("Cover is in mid position");
        }
        else
        {
          Serial.println("Cover position error!");
        }
      #endif
  }

      
  // Двигаемся?
  if (stepper.distanceToGo() != 0)
  {
     
      // Переместились на минимальную дистанцию уже?
      if ( abs(stepper.currentPosition () -  stepperInitPosition) > minDistanceBeforeChecking)
      {
          //Проверяем, не пора ли остановиться?
          if (openedButtonStatus==1 || closedButtonStatus==1)
          {
            stopMoving();
          }
      }

      //still running, move to 
      stepper.run();
  }

  //print stats if no moving and every 50 steps
  if (abs(stepper.currentPosition () -  stepperInitPosition) % 50 == 0)
  {
      Serial.print ("[!MVE:");
      Serial.print ((stepper.isRunning()? 1 : 0));
      Serial.println("]");
      
      Serial.print ("[!SPC:");
      Serial.print (stepper.speed());
      Serial.println("]");
      
      Serial.print ("[!POS:");
      Serial.print (stepper.currentPosition());
      Serial.println("]");
      
      Serial.print ("[!DST:");
      Serial.print (stepper.distanceToGo());
      Serial.println("]");
  }


  if (!stepper.isRunning())
  {
   
      /*****************************/
      //Print settings
      /*****************************/
      Serial.print ("[!SPD:");
      Serial.print (stepper.maxSpeed());
      Serial.println("]");
      
      Serial.print ("[!ACL:");
      Serial.print (motorAcceleration);
      Serial.println("]");
      
      Serial.print ("[!CLN:");
      Serial.print (stepsPerCycle);
      Serial.println("]");

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

}
////////////////////////////////////////////////////////////////////////////////////////////////
