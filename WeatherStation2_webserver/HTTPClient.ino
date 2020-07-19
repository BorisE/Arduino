void HTTP_SendData()
{
    WiFiClient client;
  
    if (!client.connect(host, port)) {
      Serial.println("connection failed");
      return;
    }

    String url = "/watchdog?command=watchdog&uptime=";
    url += String(millis());
    url += "&ip=";
    url += WiFi.localIP().toString();

    Serial.print("Connecting to server: ");
    Serial.println(url);
    
    // Envoi la requete au serveur - This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
  
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r\n');
      Serial.println("[HTTP response] " + line);
   }
   
   Serial.println();
}
