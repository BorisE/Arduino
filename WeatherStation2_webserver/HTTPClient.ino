void HTTP_sendJSON()
{
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

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("[HTTP POST] received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP POST] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
