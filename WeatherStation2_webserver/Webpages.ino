//
// Web page template
//
const char HTTP_HTML[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  <title>Weather Station</title>\
  <script>\
    window.setInterval(\"update()\", 2000);\
    function update(){\
      var xhr=new XMLHttpRequest();\
      xhr.open(\"GET\", \"/temp\", true);\
      xhr.onreadystatechange = function () {\
        if (xhr.readyState != XMLHttpRequest.DONE || xhr.status != 200) return;\
        document.getElementById('temp').innerHTML=xhr.responseText;\
      };\
      xhr.send();\
    }\
  </script>\
</head>\
<body style=\"text-align:center\">\
    <h1>Weather Station</h1>\
    <font size=\"7\"><span id=\"temp\">{0}</span>&deg;</font><br>\
    <font size=\"7\"><span id=\"hum\">{1}</span>%</font>\
</body>\
</html>";


const char HTTP_HTML_HEADER[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<title>Weather Station</title>\
<style>\
table {  width: 100%;  max-width: 500px;  border: 1px solid #1C6EA4;  text-align: left;  border-collapse: collapse;  margin-left:auto;   margin-right:auto; }\
table td, table th {  border: 1px solid #AAAAAA;  padding: 3px 2px; }\
table thead {  background: #2672B5;  background: -moz-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: -webkit-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: linear-gradient(to bottom, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);  border-bottom: 1px solid #000000;  font-size: 15px;  font-weight: bold;  color: #FFFFFF; }\
table tbody td {  font-size: 13px; }\
</style>\
</head>\
<body style=\"text-align:center\">\
<h1>Weather Station</h1>";

const char HTTP_HTML_FOOTER[] PROGMEM = "{runtime}ms since starting</body></html>";

const char HTTP_HTML_SENSORSTABLE[] PROGMEM = "<table><thead>\
    <tr><th>Sensor</th><th>Value</th></tr>\
    </thead><tbody>\
      <tr><td>Pressure</td><td>{BMP} mmHg</td></tr>\
      <tr><td>Temperature BME280</td><td>{BMT} &deg;</td></tr>\
      <tr><td>Humidity BME280</td><td>{BMH} %</td></tr>\
      <tr><td>Temperature DHT22</td><td>{Temp} &deg;</td></tr>\
      <tr><td>Humidity DHT22</td><td>{Hum} %</td></tr>\
      <tr><td>Temperature OneWire</td><td>{OW1} &deg;</td></tr>\
    </tbody></table>";
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



void printRequestData()
{
  Serial.print("[HTTP REQUEST] client: ");
  Serial.println(server.client().remoteIP().toString());
  Serial.print("[HTTP REQUEST] method: ");
  Serial.println(server.method());
  Serial.println("[HTTP REQUEST] uri: " + server.uri());
}

void handleRoot() {
  digitalWrite(STATUS_LED, HIGH);

  String page, page1;
  page = FPSTR(HTTP_HTML_HEADER);
  page1 = FPSTR(HTTP_HTML_SENSORSTABLE);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_FOOTER);
  page +=page1;

  page.replace("{BMP}", String(bmePres));
  page.replace("{BMH}", String(bmeHum));
  page.replace("{BMT}", String(bmeTemp));
  page.replace("{Temp}", String(dhtTemp));
  page.replace("{Hum}", String(dhtHum)); 
  page.replace("{OW1}", String(OW_Temp1));
  page.replace("{runtime}", String(currenttime));
  
  server.send(200, "text/html", page);
  
  //server.send(200, "text/plain", "hello from esp8266!");
  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

void handleGetTemp() {
  digitalWrite(STATUS_LED, 1); 
  server.send(200, "text/plain", String(dhtTemp));
  digitalWrite(STATUS_LED, 0);
}

void handleNotFound() {
  digitalWrite(STATUS_LED, HIGH);
  String message = "File Not Found\n\n";
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
  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

void handleGIF(){
  digitalWrite(STATUS_LED, HIGH);
  static const uint8_t gif[] PROGMEM = {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
      0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
      0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
      0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
    };
  char gif_colored[sizeof(gif)];
  memcpy_P(gif_colored, gif, sizeof(gif));
  // Set the background to a random set of colors
  gif_colored[16] = millis() % 256;
  gif_colored[17] = millis() % 256;
  gif_colored[18] = millis() % 256;
  
  server.send(200, "image/gif", gif_colored, sizeof(gif_colored));

  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}
