WATERING CONTROL
(c) 2020 by Boris Emchenko

  usage through web:
  http://192.168.0.31/ - get JSON status

  JSON Response: {"R":1,"S":358,"T":-100.00,"H":0.00,"P":0,"C":0.47}
  R  1    - requests count
  S 358   - Soil sensor 1 value
  T -100  - Temperature from DHT sensor
  H 0     - Humidity from DHT sensor
  P 0     - Pump status (on|off)
  C 0.47  - Pump current


  http://192.168.0.31/pumpon - pump on
  http://192.168.0.31/pumpoff - pump off

  
  http://192.168.0.31/set/ - get parameters JSON
  {"ver":"0.8","ved":"20200509","VWT":100,"MPR":1000}
  ver, ved - версия и дата прошивки
  VWT - SOIL_VERYWET_THRESHOLD значение сенсора при котором насос отключается 
  MPR - MAX_PUMP_RUNTIME максмиальное время работы насоса

  http://192.168.0.31/set/pumprun/10000 -  set MAX_PUMP_RUNTIME parameter to 10000
  http://192.168.0.31/set/verywet/400 - set parameter Very Wet Threshold to 400

  add ramdom parameter to bypass cache:
  http://192.168.0.235/pumpon/1023014





  
Changes:
   ver 0.8 2020/05/09 [22542/1073] - send data by http get
   ver 0.7 2020/05/06 [20158/971] - moved some strings to flash memory
   ver 0.6 2020/05/06 [19958/1101] - moved to JSON responses
                                   - set param VWT
                                   - some optimization
   ver 0.5 2020/05/05 [20146/1333] - set params from web
   ver 0.4 2020/05/05 [19386] - optimizing command parsing (to use less readbuffer, because we got into global memory restriction)
                              - restrict pump running: by time and by very wet
                              - amp correction pedestal
                              - string messages optimized
   ver 0.3 2020/05/04 [19412] - current sensor added, some web pages logic reworked
   ver 0.2 2020/05/03 [18872] - web pages with redirect (rnd fix)
   ver 0.1 2020/05/03 [18102] - Starting release (WiFi, DHT wo lib, pump relay)
