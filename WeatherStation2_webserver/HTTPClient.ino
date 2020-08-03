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

    Serial.print("[HTTP POST] begin...\n");
    // configure traged server and url
    http.begin(client, POST_URL); //HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP POST] POST...\n");

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
        Serial.println("[HTTP POST] received payload:\n<<");
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
