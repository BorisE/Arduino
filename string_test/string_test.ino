String readBuffer;

void setup() {
  Serial.begin(9600);
  readBuffer = "GET /set/pumprun2/10000000 HTTP/1.1\r\n";
}

void loop() {
  // put your main code here, to run repeatedly:
    
    int start;

  
  if (readBuffer.indexOf("GET /set/")>=0)
  {
    Serial.println(readBuffer);
    Serial.println(readBuffer.indexOf("pumprun/"));

    

    if (start = readBuffer.indexOf("/",readBuffer.indexOf("pumprun/"))+1)
    {
//      Serial.println(start);
//      int finish = readBuffer.indexOf(" ",start);
//      Serial.println(finish);
//      String param = readBuffer.substring(start, finish);
//      Serial.print("[");
//      Serial.print(param);
//      Serial.println("]");
    }
    String param="2";
    getParamStr("pumprun2",param);
    Serial.print("[");
    Serial.print(param);
    Serial.println("]");

    Serial.print("[UL:");
    Serial.print(getParamLong("pumprun2"));
    Serial.println("]");

    Serial.print("[F:");
    Serial.print(getParamFloat("pumprun2"));
    Serial.println("]");

  }

  if (start=0)
  {
    Serial.println("True");
  }
  
}
//6768
//6754 - 296
//6786 - 302
void getParamStr(String ParamName, String &param)
{
    if (int start = readBuffer.indexOf("/",readBuffer.indexOf(ParamName+"/"))+1)
    {
      //Serial.println(start);
      //int finish = readBuffer.indexOf(" ",start);
      //Serial.println(finish);
      param = readBuffer.substring(start, readBuffer.indexOf(" ",start));
//      Serial.print("[");
//      Serial.print(param);
//      Serial.println("]");
    }
    else
    {
      param = "";
    }
}

unsigned long getParamLong(String ParamName)
{
    if (int start = readBuffer.indexOf("/",readBuffer.indexOf(ParamName+"/"))+1)
    {
      //Serial.println(start);
      //int finish = readBuffer.indexOf(" ",start);
      //Serial.println(finish);
      String param = readBuffer.substring(start, readBuffer.indexOf(" ",start));
//      Serial.print("[");
//      Serial.print(param);
//      Serial.println("]");
      return param.toInt();
    }
    else
    {
      return 0;
    }
}

float getParamFloat(String ParamName)
{
    if (int start = readBuffer.indexOf("/",readBuffer.indexOf(ParamName+"/"))+1)
    {
      //Serial.println(start);
      //int finish = readBuffer.indexOf(" ",start);
      //Serial.println(finish);
      String param = readBuffer.substring(start, readBuffer.indexOf(" ",start));
//      Serial.print("[");
//      Serial.print(param);
//      Serial.println("]");
      return param.toFloat();
    }
    else
    {
      return 0;
    }
}
