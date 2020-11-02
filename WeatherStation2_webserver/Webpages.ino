/*
 * TEMPLATE HEADER
 */
const char HTTP_HTML_HEADER[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<title>Weather Station</title>\
<style>\n\
table {  width: 100%;  max-width: 500px;  border: 1px solid #1C6EA4;  text-align: left;  border-collapse: collapse;  margin-left:auto;   margin-right:auto; }\n\
table td, table th {  border: 1px solid #AAAAAA;  padding: 3px 2px; }\
table thead {  background: #2672B5;  background: -moz-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: -webkit-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: linear-gradient(to bottom, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);  border-bottom: 1px solid #000000;  font-size: 15px;  font-weight: bold;  color: #FFFFFF; }\
table tbody td {  font-size: 13px; }\
.footer { font-size:10px }\
</style>\n\
<script></script>\
</head>\n\
<body style=\"text-align:center\">\n\
<h1>Weather Station</h1>\n";

/*
 * TEMPLATE FOOTER
 */
const char HTTP_HTML_FOOTER[] PROGMEM = "<p class='footer'>WEATHER STATION MkII v{Ver} [{VDate}]. Passed since start: <span class='footer'><span id='RT'>{RT}</span> ms</p>";
const char HTTP_HTML_END[] PROGMEM = "</body></html>";

/*
 * TEMPLATE MENU
 */
const char HTTP_HTML_MENU[] PROGMEM = "<a href='/configmode' onclick=\"return confirm('Station would stop gathering data and will enter config mode. Are you sure? If Yes connect by WiFi to AP [{ConfigAP}] to change configuration')\">Enter config mode</a>";

/*
 SENSOR TABLE TEMPLATE
 */
const char HTTP_HTML_SENSORSTABLE[] PROGMEM = "<table><thead>\
    <tr><th>Sensor</th><th>Value</th></tr>\
    </thead><tbody>\
      <tr><td>Pressure</td><td><span id='BMP'>{BMP}</span> mmHg</td></tr>\
      <tr><td>Temperature BME280</td><td><span id='BMT'>{BMT}</span> &deg;</td></tr>\
      <tr><td>Humidity BME280</td><td><span id='BMH'>{BMH}</span> %</td></tr>\
      <tr><td>Temperature DHT22</td><td><span id='Temp'>{Temp}</span> &deg;</td></tr>\
      <tr><td>Humidity DHT22</td><td><span id='Hum'>{Hum}</span> %</td></tr>\
      <tr><td>Temperature DHT22_2</td><td><span id='Temp2'>{Temp2}</span> &deg;</td></tr>\
      <tr><td>Humidity DHT22_2</td><td><span id='Hum2'>{Hum2}</span> %</td></tr>\
      <tr><td>Temperature OneWire</td><td><span id='OW1'>{OW1}</span> &deg;</td></tr>\
      <tr><td>Object MLX90614</td><td><span id='OBJ'>{OBJ}</span> &deg;</td></tr>\
      <tr><td>Ambient MLX90614</td><td><span id='AMB'>{AMB}</span> &deg;</td></tr>\
      <tr><td>Illuminance BH1750FVI</td><td><span id='BHV'>{BHV}</span> lx</td></tr>\
      <tr><td>Rain analog sensor</td><td><span id='Rain'>{Rain}</span></td></tr>\
    </tbody></table>";


/*
 * AUTOUPDATE DATA
 */
const char HTTP_HTML_UPDATE[] PROGMEM = "<script>\
    window.setInterval(\"update()\", {update});\
    function update(){\
      var xhr=new XMLHttpRequest();\
      xhr.open(\"GET\", \"/json\", true);\
      xhr.onreadystatechange = function () {\
        if (xhr.readyState != XMLHttpRequest.DONE || xhr.status != 200) return;\
        var getData = JSON.parse(xhr.responseText);\
        document.getElementById('BMP').innerHTML=getData.BMP;\
        document.getElementById('BMT').innerHTML=getData.BMT;\
        document.getElementById('BMH').innerHTML=getData.BMH;\
        document.getElementById('Temp').innerHTML=getData.Temp;\
        document.getElementById('Hum').innerHTML=getData.Hum;\
        document.getElementById('Temp2').innerHTML=getData.Temp2;\
        document.getElementById('Hum2').innerHTML=getData.Hum2;\
        document.getElementById('OW1').innerHTML=getData.OW1;\
        document.getElementById('OBJ').innerHTML=getData.OBJ;\
        document.getElementById('AMB').innerHTML=getData.AMB;\
        document.getElementById('BHV').innerHTML=getData.BHV;\
        document.getElementById('Rain').innerHTML=getData.Rain;\
        document.getElementById('RT').innerHTML=getData.RT;\
      };\
      xhr.send();\
    }\
  </script>";

/*
 * PING DATA
 */
const char HTTP_HTML_PINGANDRETURN[] PROGMEM = "<script>\
    window.setInterval(\"update()\", {update});\
    function update(){\
      var xhr=new XMLHttpRequest();\
      xhr.open(\"GET\", \"/ping\", true);\
      xhr.onreadystatechange = function () {\
        if (xhr.readyState != XMLHttpRequest.DONE || xhr.status != 200) return;\
        var getData = xhr.responseText;\
        if (getData == 'OK') {window.history.back();}\
      };\
      xhr.send();\
    }\
  </script>";


const char HTTP_HTML_REDIRECT[] PROGMEM = "<script>\
    window.setTimeout(\"update()\", {update});\
    function update(){\
        window.location.href = '/';\
    }\
  </script>";

/*
 * CONFIG MODE MESSAGE
 */
const char HTTP_HTML_CONFIGMODE[] PROGMEM = "WeatherStation entering to <b>Config Mode</b>. You can change there:<br><br> <li>WiFi settings<li>hardware pins <li> and so on";


//
// Web page settings template
//
const char HTTP_HTML_SETTINGS[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  <title>Weather Station</title>\
</head>\
<body style=\"text-align:center\">\
    <h1>Weather Station settings</h1>\
    <p>\
    <form method=\"post\">\
        Set to: <input type=\"text\" name=\"sp\" value=\"{2}\" style=\"width:50px\"><br/><br/>\
        <input type=\"submit\" style=\"width:100px\">\
    <form>\
    </p>\
</body>\
</html>";



/*
 * ROOT PAGE
 * old table design before moving to SPIFFS templates
 * left for compatibility
 */
void handleRoot() {
  digitalWrite(STATUS_LED, HIGH);

  String page, page1;
  page = FPSTR(HTTP_HTML_HEADER);
  page1 = FPSTR(HTTP_HTML_SENSORSTABLE);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_FOOTER);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_MENU);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_END);
  page +=page1;
  

  page.replace("{BMP}", String(bmePres));
  page.replace("{BMH}", String(bmeHum));
  page.replace("{BMT}", String(bmeTemp));
  page.replace("{Temp}", String(dhtTemp));
  page.replace("{Hum}", String(dhtHum)); 
  page.replace("{Temp2}", String(dhtTemp2));
  page.replace("{Hum2}", String(dhtHum2)); 
  page.replace("{OW1}", String(OW_Temp1));
  page.replace("{OBJ}", String(mlxObj));
  page.replace("{AMB}", String(mlxAmb));
  page.replace("{BHV}", String(bh1750Lux));
  page.replace("{Rain}", String(rainSensor));
  
  page.replace("{RT}", String(currenttime));
  page.replace("{Ver}", String(VERSION));
  page.replace("{VDate}", String(VERSION_DATE));
  page.replace("{ConfigAP}", String(ssid));

  page.replace("<script></script>", FPSTR(HTTP_HTML_UPDATE));
  page.replace("{update}", String(JS_UPDATEDATA_INTERVAL));

  server.send(200, "text/html", page);

  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

/*
 * NOT FOUND PAGE
 */
void handleNotFound() {
  String message = F("File Not Found\n\n");
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/*
 * JSON Sensor Status Page
 * 
 * used for dynamic data update
 */
void handleJSON(){
  digitalWrite(STATUS_LED, HIGH);

  String page = SensorsJSON();
  
  server.send(200, "application/json", page);

  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

/*
 * Enter Config Mode Wait Page
 */
void handleConfigMode(){
  digitalWrite(STATUS_LED, HIGH);

  String page, page1;
  page = FPSTR(HTTP_HTML_HEADER);
  page1 = FPSTR(HTTP_HTML_CONFIGMODE);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_FOOTER);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_END);
  page +=page1;

  page.replace("{RT}", String(currenttime));
  page.replace("{Ver}", String(VERSION));
  page.replace("{VDate}", String(VERSION_DATE));
  page.replace("{ConfigAP}", String(ssid));

  page.replace("<script></script>", FPSTR(HTTP_HTML_REDIRECT));
  page.replace("{update}", "6000"); //timeout before redirecting page
  //page.replace("{self_url}", WiFi.localIP().toString());

  server.send(200, "text/html", page);

  delay(1000);
  
  server.stop(); // stop web server because of conflict with WiFi manager
  
  runConfigPortal();

  server.begin();
}


/*
 * Enter Config Mode Wait Page
 */
void handlePingRequest(){
  digitalWrite(STATUS_LED, HIGH);
  String page = "OK"; //returned data
  server.send(200, "text/plain", page);
  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

/*
 * Function to make JSON string with all current sensor data
 */
String SensorsJSON()
{
  String page = "{";
  
  page += "\"BMP\": " + String(bmePres) + ", ";
  page += "\"BMH\": " + String(bmeHum) + ", ";
  page += "\"BMT\": " + String(bmeTemp) + ", ";
  page += "\"Temp\": " + String(dhtTemp) + ", ";
  page += "\"Hum\": " + String(dhtHum) + ", ";
  page += "\"Temp2\": " + String(dhtTemp2) + ", ";
  page += "\"Hum2\": " + String(dhtHum2) + ", ";
  page += "\"OW1\": " + String(OW_Temp1) + ", ";
  page += "\"OBJ\": " + String(mlxObj) + ", ";
  page += "\"AMB\": " + String(mlxAmb) + ", ";
  page += "\"BHV\": " + String(bh1750Lux) + ", ";
  page += "\"Rain\": " + String(rainSensor) + ", ";
  page += "\"RT\": " + String(currenttime) + ", ";
  page += "\"Ver\": \"" + String(VERSION) + "\", ";
  page += "\"VDate\": \"" + String(VERSION_DATE) + "\"";

  page +="}";

  return page;
}

/*
 * Make param string for NarodMon
 */
String SensorsParamString(){
  String buf="ID=" + WiFi.macAddress()+ "&";
  buf.replace(":", ""); 

  buf+= "TEMP1=" + String(OW_Temp1) + "&";
  buf+= "PRESS=" + String(bmePres) + "&";
  buf+= "HUM=";
  if ((dhtHum) > 0) 
    buf+= String(dhtHum);
  else 
    buf+= String(bmeHum);
  buf+= "&";
  buf+= "LUX=" + String(bh1750Lux) + "&";
  buf+= "CIDX=" + String((OW_Temp1 - mlxObj));

  return buf;
}

/*
 * Print Request Info
 * for verbose debug
 */
void printRequestData()
{
  Serial.print(F("[HTTP REQUEST] client: "));
  Serial.print(server.client().remoteIP().toString());
  //Serial.print("[HTTP REQUEST] method: ");
  //Serial.println(server.method());
  Serial.println(" URI: " + server.uri());
}
