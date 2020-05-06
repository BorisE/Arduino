void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(ip);

  
  Serial.println();
}


/* JSON repsonse
 * {"ver":"0.5","ved":"20200505","req":1598,"soi":1020,"tem":28.30,"hum":39.80,"pum":0,"pcr":0.43}
 */

void sendHttpResponse_JSON(WiFiEspClient client)
{
  //{"req":1598,"soi":1020,"tem":28.30,"hum":39.80,"pum":0,"pcr":0.43}
  client.print(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: application/json\r\n"
      "Connection: close\r\n"  // the connection will be closed after completion of the response
      "\r\n");
  client.print('{');
  client.print("\"R\":");
  client.print(++reqCount);
  client.print(",\"S\":");
  client.print(Soil_1_Val);
  client.print(",\"T\":");
  client.print(dhtTemp);
  client.print(",\"H\":");
  client.print(dhtHum);
  client.print(",\"P\":");
  client.print(PumpStatus);
  client.print(",\"C\":");
  client.print(AcsValueF);
  
  client.println('}');
  // The HTTP response ends with another blank line:
  client.println();
}

void sendHttpResponse_Settings_JSON(WiFiEspClient client)
{
  //{"ver":"0.5","ved":"20200505","VWT":100,"MPR":60000}
  client.print(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: application/json\r\n"
      "Connection: close\r\n"  // the connection will be closed after completion of the response
      "\r\n");
  client.print('{');
  client.print("\"ver\":\"");
  client.print(VERSION);
  client.print("\",\"ved\":\"");
  client.print(VERSION_DATE);
  client.print("\",\"VWT\":");
  client.print(SOIL_VERYWET_THRESHOLD);
  client.print(",\"MPR\":");
  client.print(MAX_PUMP_RUNTIME);
  
  client.println('}');
  // The HTTP response ends with another blank line:
  client.println();
}
