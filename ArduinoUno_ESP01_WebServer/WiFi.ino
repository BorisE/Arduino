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

void sendHttpResponse(WiFiEspClient client)
{
    Serial.println("Sending response");
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
    client.print("<!DOCTYPE HTML>\r\n");
    client.print("<html>\r\n");
    client.print("<h1>Hello World!</h1>\r\n");
    client.print("Requests received: ");
    client.print(++reqCount);
    client.print("<br>\r\n");
    client.print("Analog input A0: ");
    client.print(analogRead(0));
    client.print("<br>\r\n");

    client.print("DHT temp: ");
    client.print(dhtTemp);
    client.print("<br>\r\n");
    client.print("DHT humidity: ");
    client.print(dhtHum);
    client.print("<br>\r\n");

    client.print("Pump switch: ");
    client.print(digitalRead(RELAY_PUMP_PIN));
    client.print("<br>\r\n");
  
    client.print("</html>\r\n");

    // The HTTP response ends with another blank line:
    client.println();
}
