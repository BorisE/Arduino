#define HTTP_SEND_RES_OK 1
#define HTTP_SEND_RES_NOCONNECTION 0
#define HTTP_SEND_RES_HTTPCODE_BAD -1
/*
 * ret value:
 *  1 if success
 *  0 no connection
 *  -1 bad httpcode
 *  [100..511] HTTP codes see RFC7231 or \AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.7.2\libraries\ESP8266HTTPClient\src\ESP8266HTTPClient.h 
  */
int HTTP_sendJSON()
{
  int ret=HTTP_SEND_RES_NOCONNECTION;
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient

    Serial.print(F("[HTTP POST] begin...\n"));
    // configure traged server and url
    http.begin(client, configData.POST_URL); //HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print(F("[HTTP POST] POST...\n"));

    String json = SensorsJSON();
    
    // start connection and send HTTP header and body
    int httpCode = http.POST(json);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP POST] POST code: %d\n", httpCode);
      ret = httpCode;
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println(F("[HTTP POST] received payload:\n<<"));
        Serial.println(payload);
        Serial.println(">>");
        ret = HTTP_SEND_RES_OK;
      }
    } 
    else 
    {
      ret=HTTP_SEND_RES_HTTPCODE_BAD;
      Serial.printf("[HTTP POST] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  return ret;
}

/*
 *  Send data to NarodMon POST
 *  POST
 */
int NarodMon_sent_POST() {
// http://narodmon.ru/get?ID=MAC&mac1=value1&...&macN=valueN[&time=UnixTime][&name=название][&lat=широта][&lon=долгота][&alt=высота_над_морем]
  int ret=HTTP_SEND_RES_NOCONNECTION;
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient

    Serial.print(F("[HTTP NARODMON] start sending data to "));
    Serial.println(NARODMON_SERVER_POST);
    
    // configure traged server and url
    if (http.begin(NARODMON_SERVER_POST)) {
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
      String POST_Prarms= SensorsParamString();
  
      Serial.print(F("[HTTP NARODMON] Sending data: "));
      Serial.println(POST_Prarms);
  
      
      // start connection and send HTTP header and body
      int httpCode = http.POST(POST_Prarms);
  
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP NARODMON] POST code: %d\n", httpCode);
        ret = httpCode;
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          Serial.println(F("[HTTP NARODMON] received payload:\n<<"));
          Serial.println(payload);
          Serial.println(">>");
          ret = HTTP_SEND_RES_OK;
        }
      } 
      else 
      {
        ret=HTTP_SEND_RES_HTTPCODE_BAD;
        Serial.printf("[HTTP NARODMON] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
  
      http.end();
    }else{
      Serial.print(F("[HTTP NARODMON] Unable to begin connection"));
    }
  }
  return ret;
}


int NarodMon_sent_GET() {
// http://narodmon.ru/get?ID=MAC&mac1=value1&...&macN=valueN[&time=UnixTime][&name=название][&lat=широта][&lon=долгота][&alt=высота_над_морем]
  int ret=HTTP_SEND_RES_NOCONNECTION;
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient

    // Prepere URL
    char Naromdmon_Get[200] = NARODMON_SERVER_GET;
    strcat(Naromdmon_Get,"?");
    String GET_Prarms= SensorsParamString();
    strcat(Naromdmon_Get,GET_Prarms.c_str());

    Serial.print(F("[HTTP NARODMON] start sending data to: "));
    Serial.println(Naromdmon_Get);

    if (http.begin(Naromdmon_Get)) {
      Serial.print(F("[HTTP NARODMON] Sending data: "));
      Serial.println(GET_Prarms);

      // start connection and send HTTP header and body
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP NARODMON] GET code: %d\n", httpCode);
        ret = httpCode;
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          Serial.println(F("[HTTP NARODMON] received payload:\n<<"));
          Serial.println(payload);
          Serial.println(">>");
          ret = HTTP_SEND_RES_OK;
        }
      } 
      else 
      {
        ret=HTTP_SEND_RES_HTTPCODE_BAD;
        Serial.printf("[HTTP NARODMON] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }else{
      Serial.print(F("[HTTP NARODMON] Unable to begin connection"));
    }
  }
  return ret;
}
