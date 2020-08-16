/*
 * TEMPLATE HEADER
 */
const char HTTP_HTML_HEADER[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<title>Water Supply Control System</title>\n\
<style>\
table {  width: 100%;  max-width: 500px;  border: 1px solid #1C6EA4;  text-align: left;  border-collapse: collapse;  margin-left:auto;   margin-right:auto; }\n\
table td, table th {  border: 1px solid #AAAAAA;  padding: 3px 2px; }\
table thead {  background: #2672B5;  background: -moz-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: -webkit-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: linear-gradient(to bottom, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);  border-bottom: 1px solid #000000;  font-size: 15px;  font-weight: bold;  color: #FFFFFF; }\
table tbody td {  font-size: 13px; }\
.footer { font-size:10px }\
</style>\n\
<script></script>\n\
</head>\n\
<body style=\"text-align:center\">\
<h1>Water Supply Control System</h1>";

/*
 * TEMPLATE FOOTER
 */
const char HTTP_HTML_FOOTER[] PROGMEM = "<p class='footer'>WaterSupplyControl v{Ver} [{VDate}]. Passed since start: <span class='footer'><span id='RT'>{RT}</span> ms</p><span id='debug'></span>";
const char HTTP_HTML_END[] PROGMEM = "</body></html>";

/*
 RELAY TABLE TEMPLATE
 */
const char HTTP_HTML_RELAYTABLE[] PROGMEM = "<table><thead>\
        <tr><th>Relay 1</th><th>Relay 2</th><th>Relay 3</th><th>Relay 4</th></tr>\
    </thead><tbody>\n\
      <tr>\
        <td id='relay1'>{relay1}</td>\
        <td id='relay2'>{relay2}</td>\
        <td id='relay3'>{relay3}</td>\
        <td id='relay4'>{relay4}</td>\
      </tr>\n\
      <tr>\
        <td><button onclick=\"relaysend(1,1);\">ON</button><button onclick=\"relaysend(1,0);\">OFF</button></td>\
        <td><button onclick=\"relaysend(2,1);\">ON</button><button onclick=\"relaysend(2,0);\">OFF</button></td>\
        <td><button onclick=\"relaysend(3,1);\">ON</button><button onclick=\"relaysend(3,0);\">OFF</button></td>\
        <td><button onclick=\"relaysend(4,1);\">ON</button><button onclick=\"relaysend(4,0);\">OFF</button></td>\
      </tr>\n\
    </tbody></table>\n";

/*
 WATER SENSOR TABLE TEMPLATE
 */
const char HTTP_HTML_WSTABLE[] PROGMEM = "<br><table><tbody>\n\
      <tr>\
        <td id='WS3'>{WS3}</td>\
      </tr>\n\
      <tr>\
        <td id='WS2'>{WS2}</td>\
      </tr>\n\
      <tr>\
        <td id='WS1'>{WS1}</td>\
      </tr>\n\
    </tbody></table>\n";

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
        displayrelaystat(1,getData.relay1);\
        displayrelaystat(2,getData.relay2);\
        displayrelaystat(3,getData.relay3);\
        displayrelaystat(4,getData.relay4);\
        displaywsens(1,getData.WS1);\
        displaywsens(2,getData.WS2);\
        displaywsens(3,getData.WS3);\
        document.getElementById('RT').innerHTML=getData.RT;\
      };\
      xhr.send();\
    }\n\
    function displayrelaystat(rnum, rstat) {\
      document.getElementById('relay'+rnum).innerHTML=rstat;\
      document.getElementById('relay'+rnum).style.backgroundColor = (rstat == 'ON'? '#26b569' :'#b5262b');\
    }\n\
    function relaysend(rnum, rstat) {\
      var xhr=new XMLHttpRequest();\
      xhr.open(\"GET\", '/relay?relay' + rnum + '=' + rstat, true);\
      xhr.onreadystatechange = function () {\
        if (xhr.readyState != XMLHttpRequest.DONE || xhr.status != 200) return;\
        var getData = xhr.responseText;\
        document.getElementById('debug').innerHTML=getData;\
        update();\
      };\
      xhr.send();\
    }\n\
    function displaywsens(rnum, wstat) {\
      document.getElementById('WS'+rnum).innerHTML=wstat;\
      document.getElementById('WS'+rnum).style.backgroundColor = (wstat == '+'? '#26b5b1' :'');\
    }\n\
    </script>";


const char HTTP_HTML_REDIRECT[] PROGMEM = "<script>\
    window.setTimeout(\"update()\", {update});\
    function update(){\
        window.location.href = 'http://{self_url}/';\
    }\
  </script>";



/*
 * ROOT PAGE
 */
void handleRoot() {
  digitalWrite(STATUS_LED, HIGH);

  String page, page1;
  page = FPSTR(HTTP_HTML_HEADER);
  page1 = FPSTR(HTTP_HTML_RELAYTABLE);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_WSTABLE);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_FOOTER);
  page +=page1;
  page1 = FPSTR(HTTP_HTML_END);
  page +=page1;
  

  page.replace("{relay1}", String(getRelayStatusString(config.H1_1_PIN)));
  page.replace("{relay2}", String(getRelayStatusString(config.H1_2_PIN)));
  page.replace("{relay3}", String(getRelayStatusString(config.H2_1_PIN)));
  page.replace("{relay4}", String(getRelayStatusString(config.H2_2_PIN)));

  page.replace("{WS1}", String(getSensorStatusString(config.WS1_PIN)));
  page.replace("{WS2}", String(getSensorStatusString(config.WS2_PIN)));
  page.replace("{WS3}", String(getSensorStatusString(config.WS3_PIN)));

  
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
 * JSON Sensor Status Page
 */
void handleJSON(){
  digitalWrite(STATUS_LED, HIGH);

  String page = SensorsJSON();
  
  server.send(200, "application/json", page);

  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

String SensorsJSON()
{
  String page = "{";
  
  page += "\"relay1\": \"" + String(getRelayStatusString(config.H1_1_PIN)) + "\",";
  page += "\"relay2\": \"" + String(getRelayStatusString(config.H1_2_PIN)) + "\",";
  page += "\"relay3\": \"" + String(getRelayStatusString(config.H2_1_PIN)) + "\",";
  page += "\"relay4\": \"" + String(getRelayStatusString(config.H2_2_PIN)) + "\",";

  page += "\"WS1\": \"" + String(getSensorStatusString(config.WS1_PIN)) + "\",";
  page += "\"WS2\": \"" + String(getSensorStatusString(config.WS2_PIN)) + "\",";
  page += "\"WS3\": \"" + String(getSensorStatusString(config.WS3_PIN)) + "\",";
  
  page += "\"RT\": " + String(currenttime) + "";

  page +="}";

  return page;
}


/*
 * Relay Switch Handler
 */
void handleRelaySwitch() {

  digitalWrite(STATUS_LED, HIGH);

  Serial.print("Arg name: ");
  Serial.println(server.argName(0));
  Serial.print("Arg val: ");
  Serial.println(server.arg(0));

  int relayPin = getRelayPinByName(server.argName(0));
  Serial.print("Relay pin: ");
  Serial.println(relayPin);
  
  int relayStatus = convertReyalyStatusToInt(server.arg(0));
  Serial.print("need to set to: ");
  Serial.println(relayStatus);

  int stat=relaySwitchByParams(server.argName(0), server.arg(0));

  String message = "Relay [";
  message += server.argName(0);
  message += "] set to ";
  message += server.arg(0);
  message += ". Command status: ";
  message += stat;
  server.send(200, "text/plain", message);
  
  printRequestData();
  digitalWrite(STATUS_LED, LOW);
  
}


/*
 * Respond with OK
 */
void handlePingRequest(){
  digitalWrite(STATUS_LED, HIGH);
  String page = "OK"; //returned data
  server.send(200, "text/plain", page);
  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

/*
 * NOT FOUND PAGE
 */
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

/*
 *  Print HTTP request data
 */
void printRequestData()
{
  Serial.print("[HTTP REQUEST] client: ");
  Serial.print(server.client().remoteIP().toString());
  //Serial.print("[HTTP REQUEST] method: ");
  //Serial.println(server.method());
  Serial.println(" URI: " + server.uri());
}
