/*
 * TEMPLATE HEADER
 */
const char HTTP_HTML_HEADER[] PROGMEM = "<!DOCTYPE html>\
<html>\
<head>\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<meta charset=\"utf-8\">\
<title>Water Supply Control System</title>\n\
<style>\
table {  width: 100%;  max-width: 500px;  border: 1px solid #1C6EA4;  text-align: center;  border-collapse: collapse;  margin-left:auto;   margin-right:auto; }\n\
table td, table th {  border: 1px solid #AAAAAA;  padding: 3px 2px; }\
table thead {  background: #2672B5;  background: -moz-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: -webkit-linear-gradient(top, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);   background: linear-gradient(to bottom, #5c95c7 0%, #3b80bc 66%, #2672B5 100%);  border-bottom: 1px solid #000000;  font-size: 15px;  font-weight: bold;  color: #FFFFFF; }\
table tbody td {  font-size: 13px; }\
.footer { font-size:10px }\
</style>\n\
<script></script>\n\
</head>\n\
<body style=\"text-align:center\" onload='load()'>\
<h1>Water Supply Control System</h1>";

/*
 * TEMPLATE FOOTER
 */
const char HTTP_HTML_FOOTER[] PROGMEM = "<p class='footer'>WaterSupplyControl v{Ver} [{VDate}]. Passed since start: <span class='footer'><span id='RT'>{RT}</span> ms</p><div id='jdebug'></div><div id='debug'></div><p id='debug2'></p>";
const char HTTP_HTML_END[] PROGMEM = "</body></html>";

/*
 RELAY TABLE TEMPLATE
 */
const char HTTP_HTML_RELAYTABLE[] PROGMEM = "<table><thead>\
        <tr><th>Relay OPEN</th><th>Relay CLOSE</th><th>Relay 3</th><th>Relay 4</th></tr>\
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
        <td id='WS1'>{WS1}</td>\
      </tr>\n\
      <tr>\
        <td id='WS2'>{WS2}</td>\
      </tr>\n\
      <tr>\
        <td id='WS3'>{WS3}</td>\
      </tr>\n\
    </tbody></table>\n";


/*
 WATERFLOW TABLE TEMPLATE
 */
const char HTTP_HTML_WFLOWTABLE[] PROGMEM = "<br><table><tbody>\n\
      <tr><td colspan=10><span id='WFval'>{WFval}</span> L/min</td></tr>\n\
      <tr>\
        <td id='WF1'></td>\
        <td id='WF2'></td>\
        <td id='WF3'></td>\
        <td id='WF4'></td>\
        <td id='WF5'></td>\
        <td id='WF6'></td>\
        <td id='WF7'></td>\
        <td id='WF8'></td>\
        <td id='WF9'></td>\
        <td id='WF10'></td>\
      </tr>\n\
    </tbody></table>\n";



/*
 * AUTOUPDATE DATA
 */
const char HTTP_HTML_UPDATE[] PROGMEM = "<script>\
    window.setInterval(\"update()\", {update});\
    \
    function update(){\
      var xhr=new XMLHttpRequest();\
      xhr.open(\"GET\", \"/json\", true);\
      xhr.onreadystatechange = function () {\
        if (xhr.readyState != XMLHttpRequest.DONE || xhr.status != 200) return;\
        /*document.getElementById('debug2').innerHTML=xhr.responseText;*/\
        var getData = JSON.parse(xhr.responseText);\
        displayrelaystat(1,getData.relay1);\
        displayrelaystat(2,getData.relay2);\
        displayrelaystat(3,getData.relay3);\
        displayrelaystat(4,getData.relay4);\
        displaywsens(1,getData.WS1);\
        displaywsens(2,getData.WS2);\
        displaywsens(3,getData.WS3);\
        waterflowdraw(getData.WF);\
        document.getElementById('RT').innerHTML=getData.RT / 1000;\
        document.getElementById('debug').innerHTML=document.getElementById('debug').innerHTML + (getData.debug !='' ? '<br>' + getData.debug : '');\
      };\
      xhr.send();\
    }\n\
    \
    function displayrelaystat(rnum, rstat) {\
      document.getElementById('relay'+rnum).innerHTML=rstat;\
      document.getElementById('relay'+rnum).style.backgroundColor = (rstat == 'ON'? '#26b569' :'#b5262b');\
    }\n\
    \
    function relaysend(rnum, rstat) {\
      var xhr=new XMLHttpRequest();\
      xhr.open(\"GET\", '/relay?relay' + rnum + '=' + rstat, true);\
      xhr.onreadystatechange = function () {\
        if (xhr.readyState != XMLHttpRequest.DONE || xhr.status != 200) return;\
        var getData = xhr.responseText;\
        document.getElementById('jdebug').innerHTML=getData;\
        update();\
        if (rstat==1){\
          setTimeout('relaysend('+rnum+',0)', {engine_move_timeout} );\
        }\
      };\
      xhr.send();\
    }\n\
    \
    function displaywsens(rnum, wstat) {\
      document.getElementById('WS'+rnum).innerHTML=wstat;\
      document.getElementById('WS'+rnum).style.backgroundColor = (wstat == '+'? '#26b5b1' :'');\
    }\n\
    \
    function waterflowdraw(waterflow) {\
        document.getElementById('WFval').innerHTML=waterflow;\n\
        var calcWF=waterflow/100*10;\n\
        for (var i=1; i <= calcWF; i++) {\
            document.getElementById('WF'+i).style.backgroundColor = '#26b569';\
        }\
        for (var i=calcWF+1; i <= 10; i++) {\
            document.getElementById('WF'+i).style.backgroundColor = '';\
        }\
    }\n\
    \
    function load(){\
      update();\
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
  page1 = FPSTR(HTTP_HTML_WFLOWTABLE);
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

  page.replace("{WFval}", String(flow_l_min));
  
  page.replace("{RT}", String(currenttime));
  page.replace("{Ver}", String(VERSION));
  page.replace("{VDate}", String(VERSION_DATE));
  page.replace("{ConfigAP}", String(ssid));

  page.replace("<script></script>", FPSTR(HTTP_HTML_UPDATE));
  page.replace("{update}", String(JS_UPDATEDATA_INTERVAL));
  page.replace("{engine_move_timeout}", String(VENT_CHANGESTATE_TIMEOUT));

  

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

String SensorsJSON(bool postdebug=true)
{
  String page = "{";
  
  page += "\"relay1\": \"" + String(getRelayStatusString(config.H1_1_PIN)) + "\",";
  page += "\"relay2\": \"" + String(getRelayStatusString(config.H1_2_PIN)) + "\",";
  page += "\"relay3\": \"" + String(getRelayStatusString(config.H2_1_PIN)) + "\",";
  page += "\"relay4\": \"" + String(getRelayStatusString(config.H2_2_PIN)) + "\",";

  page += "\"WS1\": \"" + String(getSensorStatusString(config.WS1_PIN)) + "\",";
  page += "\"WS2\": \"" + String(getSensorStatusString(config.WS2_PIN)) + "\",";
  page += "\"WS3\": \"" + String(getSensorStatusString(config.WS3_PIN)) + "\",";
  //page += "\"WS3\": \"" + String(digitalRead(config.WS3_PIN)) + "\",";
  

  page += "\"WF\": \"" + String(flow_l_min) + "\",";
  if (postdebug)
  { 
    page += "\"debug\": \"" + String(debugstack) + "\",";
    debugstack[0] = '\0'; //empty buffer
  }
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
