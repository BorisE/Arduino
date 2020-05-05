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
      return -1;
    }
}
