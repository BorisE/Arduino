// пин бесконтакного датчика уровня жидкости
#define LIQIID_SENSOR_APIN A0
#define LIQIID_SENSOR1_PIN D1
#define LIQIID_SENSOR2_PIN D2
#define LIQIID_SENSOR3_PIN D3
#define LIQIID_SENSOR4_PIN D8

void setup()
{
  // открываем монитор Serial-порта
  Serial.begin(115200);
  // настраиваем пин в режим входа
  pinMode(LIQIID_SENSOR1_PIN, INPUT);
  pinMode(LIQIID_SENSOR2_PIN, INPUT);
  pinMode(LIQIID_SENSOR3_PIN, INPUT);
  pinMode(LIQIID_SENSOR4_PIN, INPUT);
}
 
void loop()
{
  // считываем состояние пина
  int motionState = analogRead(LIQIID_SENSOR_APIN);
  int motionState1 = digitalRead(LIQIID_SENSOR1_PIN);
  int motionState2 = digitalRead(LIQIID_SENSOR2_PIN);
  int motionState3 = digitalRead(LIQIID_SENSOR3_PIN);
  int motionState4 = digitalRead(LIQIID_SENSOR4_PIN);
  // выводим данные в Serial-порт
  Serial.print("Analog: ");
  Serial.println(motionState);
  
  Serial.print("Digital 1 (pin ");
  Serial.print(LIQIID_SENSOR1_PIN);
  Serial.print("): ");
  Serial.println(motionState1);
  Serial.print("Digital 2 (pin ");
  Serial.print(LIQIID_SENSOR2_PIN);
  Serial.print("): ");
  Serial.println(motionState2);
  Serial.print("Digital 3 (pin ");
  Serial.print(LIQIID_SENSOR3_PIN);
  Serial.print("): ");
  Serial.println(motionState3);
  Serial.print("Digital 4 (pin ");
  Serial.print(LIQIID_SENSOR4_PIN);
  Serial.print("): ");
  Serial.println(motionState4);

  delay(500);
}
