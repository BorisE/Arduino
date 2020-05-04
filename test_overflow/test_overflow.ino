#define MAX_UL 4294967295
#define MAX_PUMP_RUNTIME UL4294966000

unsigned long currenttime;
unsigned long MAX_PUMP_RUNTIME = 10000;

unsigned long starttime = 0, now =0;;

void setup() {
  Serial.begin(9600);
  Serial.println("Millis overflow testing");
  // put your setup code here, to run once:
  starttime = millis();
}


void loop() {
  // put your main code here, to run repeatedly:
  now = millis();
  if ((now-starttime)>MAX_PUMP_RUNTIME)
  {
     Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!");
     Serial.print("NOW: ");
     Serial.println(now);
     starttime = millis();
  }


  Serial.println( ( unsigned long ) ( 10 - 4294967290 ) );

}
