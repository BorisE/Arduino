/* for Wemos D1 R1
 *  
#define PIN_WIRE_SDA (D1)    4   (by default, but I redefined, see below)
#define PIN_WIRE_SCL (D2)    5   (by default, but I redefined, see below)

//new ESP-12E GPIO2
#define LED_BUILTIN 2       D4

static const uint8_t D0   = 16; /no interrupts                    <-relay
static const uint8_t D1   = 5;  /SCL     
static const uint8_t D2   = 4;  /SDA
static const uint8_t D3   = 0;  /10k pullup                       <-always low on start
static const uint8_t D4   = 2;  /Tx1, 10k pullup, Buit-in LED     <-яркий диод
static const uint8_t D5   = 14; /built in led2 ("SCK")            <-relay
static const uint8_t D6   = 12;                                   <-relay
static const uint8_t D7   = 13; /RX0* 10k pulldown                <-relay
static const uint8_t D8   = 15; /Tx0*                             <-при загузке скетча использует этот пин. потом можно использовать; для реле не подходит

GPIO6-GPIO11 - flash pins

*/


#ifndef STASSID
#define STASSID "BATMAJ"
#define STAPSK  "8oknehcmE"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;


#define H1_1_PIN_DEFAULT D7
#define H1_2_PIN_DEFAULT D6
#define H2_1_PIN_DEFAULT D5 
#define H2_2_PIN_DEFAULT D0

struct Config {
  uint8_t H1_1_PIN;
  uint8_t H1_2_PIN;
  uint8_t H2_1_PIN;
  uint8_t H2_2_PIN;
};
Config config;      

void setup() {
  // put your setup code here, to run once:
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("");

  config.H1_1_PIN=H1_1_PIN_DEFAULT;
  config.H1_2_PIN=H1_2_PIN_DEFAULT;
  config.H2_1_PIN=H2_1_PIN_DEFAULT;
  config.H2_2_PIN=H2_2_PIN_DEFAULT;

  //Set relay ping
  pinMode(config.H1_1_PIN, OUTPUT);    // Установим вывод 5 как выход
  pinMode(config.H1_2_PIN, OUTPUT);    // Установим вывод 6 как выход
  pinMode(config.H2_1_PIN, OUTPUT);    // Установим вывод 6 как выход
  pinMode(config.H2_2_PIN, OUTPUT);    // Установим вывод 6 как выход

  digitalWrite(config.H1_1_PIN, HIGH); // Выключаем реле
  digitalWrite(config.H1_2_PIN, HIGH); // Выключаем реле
  digitalWrite(config.H2_1_PIN, HIGH); // Выключаем реле
  digitalWrite(config.H2_2_PIN, HIGH); // Выключаем реле
  
}

void loop() {
   delay(3000);             // Ждем 2с
   // on
   Serial.print("H1_1_PIN ");
   Serial.println("on");
   digitalWrite(config.H1_1_PIN, LOW); // Включаем реле
   delay(1000);             // Ждем 2с

   Serial.print("H1_2_PIN ");
   Serial.println("on");
   digitalWrite(config.H1_2_PIN, LOW);  // Включаем реле
   delay(1000);             // Ждем 2с

   Serial.print("H2_1_PIN ");
   Serial.println("on");
   digitalWrite(config.H2_1_PIN, LOW); // Включаем реле
   delay(1000);             // Ждем 2с 

   Serial.print("H2_2_PIN ");
   Serial.println("on");
   digitalWrite(config.H2_2_PIN, LOW);  // Включаем реле
   delay(2000);             // Ждем 2с 

   // off
   Serial.print("H1_1_PIN ");
   Serial.println("off");
   digitalWrite(config.H1_1_PIN, HIGH); // Выключаем реле
   delay(1000);             // Ждем 2с

   Serial.print("H1_2_PIN ");
   Serial.println("off");
   digitalWrite(config.H1_2_PIN, HIGH);  // Выключаем реле
   delay(1000);             // Ждем 2с
   
   Serial.print("H2_1_PIN ");
   Serial.println("off");
   digitalWrite(config.H2_1_PIN, HIGH); // Выключаем реле
   delay(1000);             // Ждем 2с 

   Serial.print("H2_2_PIN ");
   Serial.println("off");
   digitalWrite(config.H2_2_PIN, HIGH);  // Выключаем реле
   delay(2000);             // Ждем 2с 
}
