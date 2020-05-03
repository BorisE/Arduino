#include <SoftwareSerial.h> //Подключаем библиотеку работы с Последовательным портом
SoftwareSerial mySerial(6, 5);

void setup()
{
Serial.begin(9600);
//Serial.setTimeout(500);

// set the data rate for the SoftwareSerial port
mySerial.begin(9600); //Открываем последовательный порт Esp, устанавливаем скорость 9600. Внимание!! убедитесь что Esp работает именно на этой скорости!!
            // Изначально на скорости 112500
            // Дал команду AT+CIOBAUD=9600 после чего смог перекомпелироваться на 9600
            // После для верности дал еще AT+UART_DEF=9600,8,1,0,0
mySerial.println("AT"); // Отправляем тестовую команду АТ в созданный порт, если всё работает в ответе должно прийти ОК
            /* прикольная команда AT+GMR
             выдает:
                AT version:1.2.0.0(Jul  1 2016 20:04:45)
                SDK version:1.5.4.1(39cb9a32)
                Ai-Thinker Technology Co. Ltd.
                Dec  2 2016 14:21:16
                OK*/
               

Serial.println("Start working!!!");
}

void loop() // run over and over
{
if (mySerial.available()){
  Serial.println(mySerial.readString());
}

String txt_in="";

if (Serial.available()) {
  txt_in=Serial.readString();
  Serial.println(txt_in);
  mySerial.println(txt_in);

}
}
