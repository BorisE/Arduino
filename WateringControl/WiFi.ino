void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println();
}

void sendHttpResponse_MainPage(WiFiEspClient client)
{
    Serial.println("Sending main page...");
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    // send a standard http response header
    // use \r\n instead of many println statements to speedup data send
    client.print(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Connection: close\r\n"  // the connection will be closed after completion of the response
      "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
      "\r\n");
    //client.print("<!DOCTYPE HTML>\r\n");
    client.print("<html>\r\n");
    client.print("<h1>Watering Control</h1>\r\n");
    client.print("v");
    client.print(VERSION);
    client.print(" [");
    client.print(VERSION_DATE);
    client.print("]<br>\r\n");

    client.print("Requests received: ");
    client.print(++reqCount);
    client.print("<br><br>\r\n");
    
    client.print("Soil sensor: ");
    client.print(Soil_1_Val);
    client.print("<br>\r\n");

    client.print("DHT temp: ");
    client.print(dhtTemp);
    client.print("<br>\r\n");
    client.print("DHT humidity: ");
    client.print(dhtHum);
    client.print("<br>\r\n");

    client.print("Pump switch: ");
    int sid=random(10000);
    if (PumpStatus == LOW)
    {
      client.print("ON");
      client.print(" <a href=/pumpoff/");
      client.print(sid);
      client.print(">OFF</a>");
    }
    else
    {
      client.print("OFF");
      client.print(" <a href=/pumpon/");
      client.print(sid);
      client.print(">ON</a>");
    }
    client.print("<br>\r\n");

    client.print("Pump current: ");
    client.print(AcsValueF);
    client.print("<br>\r\n");
    
    client.print("</html>\r\n");

    // The HTTP response ends with another blank line:
    client.println();
}

void sendHttpResponse_goRoot(WiFiEspClient client)
{
    Serial.println("Sending redirect response...");
    int sid=random(10000);
    
    //HTTP/1.1 301 Moved Permanently 
    //Location: http://www.example.org/index.asp
    client.print(
      "HTTP/1.1 301 Moved Permanently\r\n"
      "Location: /");
    
    //client.print(sid);
    
    client.print(
      "\r\n"
      "Connection: close\r\n"  // the connection will be closed after completion of the response
      "\r\n");
}
