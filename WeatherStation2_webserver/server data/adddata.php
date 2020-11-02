<?php
require_once "lib.php";

$host="localhost";
$login="local";
$pass="qq11";
$dbname="adlerweather";


///////////////////////////////////////////////////////////////////////////////////
// Дата
$getdate=$_GET['date'];
if ($getdate=="") {
   $mesdate=date("Y-m-d H:i:s");
} else {
   $mesdate_int=strtotime($getdate);
   $mesdate=date("Y-m-d H:i:s",$mesdate_int);
}
//print $mesdate;

///////////////////////////////////////////////////////////////////////////////////
// Содержимое
$postData = file_get_contents('php://input');
$data = json_decode($postData, true);

//print_r($data);
//var_dump($data["Temp"]);
//var_dump($data["Hum"]);

///////////////////////////////////////////////////////////////////////////////////
// Записать в базу данных
///////////////////////////////////////////////////////////////////////////////////
$link = mysql_connect($host, $login, $pass);
if (!$link) {
    die('Ошибка соединения: ' . mysql_error());
}
$SelectResult = mysql_select_db($dbname, $link);
if (!$SelectResult) {
    die('Ошибка выбора базы данных: ' . mysql_error());
}

$q="INSERT weatherdata SET date='".$mesdate."',
   `bmp`       =".checkValidValue($data['BMP'], VALIDATION_TYPE_PRESSURE).",
   `bmt`       =".checkValidValue($data['BMT'], VALIDATION_TYPE_TEMP).",
   `bmh`       =".checkValidValue($data['BMH'], VALIDATION_TYPE_HUMIDITY).",
   `Temp`      =".checkValidValue($data['Temp'],VALIDATION_TYPE_TEMP).",
   `Hum`       =".checkValidValue($data['Hum'], VALIDATION_TYPE_HUMIDITY).",
   `OW1`       =".checkValidValue($data['OW1'], VALIDATION_TYPE_TEMP).",
   `Amb`       =".checkValidValue($data['AMB'], VALIDATION_TYPE_TEMP).",
   `Obj`       =".checkValidValue($data['OBJ'], VALIDATION_TYPE_TEMP).",
   `bhv`       =".checkValidValue($data['BHV'], VALIDATION_TYPE_LUM).",
   `rain`       =".checkValidValue($data['Rain'], VALIDATION_TYPE_RAIN).",
   `RT`        =".$data['RT' ]."
   ";

$res=mysql_query($q,$link);
if (!$res) {
    die('Ошибка sql: ' . mysql_error());
}

print "\nQuery: $q\n";
print "Data saved";

mysql_close($link);


?>