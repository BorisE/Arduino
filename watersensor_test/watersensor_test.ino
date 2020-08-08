// пин бесконтакного датчика уровня жидкости
#define LIQIID_SENSOR_APIN A0
#define LIQIID_SENSOR_PIN D5
 
void setup()
{
  // открываем монитор Serial-порта
  Serial.begin(115200);
  // настраиваем пин в режим входа
  pinMode(LIQIID_SENSOR_PIN, INPUT);
}
 
void loop()
{
  // считываем состояние пина
  int motionState = analogRead(LIQIID_SENSOR_APIN);
  int motionState2 = digitalRead(LIQIID_SENSOR_PIN);
  // выводим данные в Serial-порт
  Serial.print("Analog: ");
  Serial.println(motionState);
  Serial.print("Digital: ");
  Serial.println(motionState2);
  delay(500);
}
