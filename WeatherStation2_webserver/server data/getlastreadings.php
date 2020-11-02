<?php
header("Access-Control-Allow-Origin: *");
header('Content-type: application/json');

require_once "lib.php";


/********     Подключиться к базе  данных  ***************/
$host="localhost";
$login="local";
$pass="qq11";
$dbname="adlerweather";

$link = mysql_connect($host, $login, $pass);
if (!$link) {
    die('Ошибка соединения: ' . mysql_error());
}
$SelectResult = mysql_select_db($dbname, $link);
if (!$SelectResult) {
    die('Ошибка выбора базы данных: ' . mysql_error());
}

/********     LAST 24 h  ***************/
$q="set @ill_t_1=0, @prevdate=0; SELECT date, bhv, (bhv-@ill_t_1) as dbhv, (bhv-@ill_t_1) as dbhv, TIMESTAMPDIFF(SECOND, @prevdate, date ) as timespan, @ill_t_1:=bhv, @prevdate:=date FROM `weatherdata` WHERE DATE(date) = DATE(NOW()) ORDER BY `weatherdata`.`date`  ASC";

// Отправить запрос Last 24
$q = "SELECT MIN(bmp)as bmpminl24, MAX(bmp)as bmpmaxl24, AVG(bmp)as bmpavgl24, 
		MIN(ow1)as ow1minl24, MAX(ow1)as ow1maxl24, AVG(ow1)as ow1avgl24, 
		MIN(hum)as humminl24, MAX(hum)as hummaxl24, AVG(hum)as humavgl24, 
		MIN(bhv)as bhvminl24, MAX(bhv)as bhvmaxl24, AVG(bhv)as bhvavgl24, 
		MIN(rain)as rainminl24, MAX(rain)as rainmaxl24, AVG(rain)as rainavgl24, 
		MIN(ow1-obj)as cidxminl24, MAX(ow1-obj)as cidxmaxl24, AVG(ow1-obj)as cidxavgl24   
		FROM `weatherdata` WHERE date >= NOW() - INTERVAL 1 DAY"; 

//print "<!--".$q."-->";

$res=mysql_query($q,$link);
if (!$res) {
    die('Ошибка sql: ' . mysql_error());
}
$linel24= mysql_fetch_array($res, MYSQL_ASSOC );

/********     TODAY  ***************/
// Отправить запрос Today
$q = "SELECT MIN(bmp)as bmpmint0, MAX(bmp)as bmpmaxt0, AVG(bmp)as bmpavgt0, 
		MIN(ow1)as ow1mint0, MAX(ow1)as ow1maxt0, AVG(ow1)as ow1avgt0, 
		MIN(hum)as hummint0, MAX(hum)as hummaxt0, AVG(hum)as humavgt0, 
		MIN(bhv)as bhvmint0, MAX(bhv)as bhvmaxt0, AVG(bhv)as bhvavgt0, 
		MIN(rain)as rainmint0, MAX(rain)as rainmaxt0, AVG(rain)as rainavgt0, 
		MIN(ow1-obj)as cidxmint0, MAX(ow1-obj)as cidxmaxt0, AVG(ow1-obj)as cidxavgt0   
		FROM `weatherdata` WHERE DATE(date) = DATE(NOW())"; 

//print "<!--".$q."-->";

$res=mysql_query($q,$link);
if (!$res) {
    die('Ошибка sql: ' . mysql_error());
}
$linet0= mysql_fetch_array($res, MYSQL_ASSOC );



/********     YESTERDAY  ***************/

// Отправить запрос Yesterday
	$q = "SELECT MIN(bmp)as bmpmint_1, MAX(bmp)as bmpmaxt_1, AVG(bmp)as bmpavgt_1, 
			MIN(ow1)as ow1mint_1, MAX(ow1)as ow1maxt_1, AVG(ow1)as ow1avgt_1,	
			MIN(hum)as hummint_1, MAX(hum)as hummaxt_1, AVG(hum)as humavgt_1, 
			MIN(bhv)as bhvmint_1, MAX(bhv)as bhvmaxt_1, AVG(bhv)as bhvavgt_1,  
			MIN(rain)as rainmint_1, MAX(rain)as rainmaxt_1, AVG(rain)as rainavgt_1,  
			MIN(ow1-obj)as cidxmint_1, MAX(ow1-obj)as cidxmaxt_1, AVG(ow1-obj)as cidxavgt_1   	
			FROM `weatherdata` WHERE DATE(date) = SUBDATE(CURDATE(),1)"; 
//	$q = "SELECT MIN(bmp)as bmpmint_1 FROM `weatherdata` WHERE DATE(date)=SUBDATE(CURDATE(),1)"; 
//print "<!--".$q."-->";

$res=mysql_query($q,$link);
if (!$res) {
    die('Ошибка sql: ' . mysql_error());
}

$linet_1= mysql_fetch_array($res, MYSQL_ASSOC );


/********     LAST WEEK  (last 7 days) ***************/

// Отправить запрос Last 7 days
	$q = "SELECT MIN(bmp)as bmpminw, MAX(bmp)as bmpmaxw, AVG(bmp)as bmpavgw, 
			MIN(ow1)as ow1minw, MAX(ow1)as ow1maxw, AVG(ow1)as ow1avgw,	
			MIN(hum)as humminw, MAX(hum)as hummaxw, AVG(hum)as humavgw, 
			MIN(bhv)as bhvminw, MAX(bhv)as bhvmaxw, AVG(bhv)as bhvavgw,  
			MIN(rain)as rainminw, MAX(rain)as rainmaxw, AVG(rain)as rainavgw,  
			MIN(ow1-obj)as cidxminw, MAX(ow1-obj)as cidxmaxw, AVG(ow1-obj)as cidxavgw  	
			FROM `weatherdata` WHERE DATE(date) >= DATE(NOW()) - INTERVAL 7 DAY"; 
	//$q = "SELECT MIN(bmp)as bmpmint_1 FROM `weatherdata` WHERE DATE(date) >= DATE(NOW()) - INTERVAL 7 DAY"; 
//print "<!--".$q."-->";

$res=mysql_query($q,$link);
if (!$res) {
    die('Ошибка sql: ' . mysql_error());
}

$linew= mysql_fetch_array($res, MYSQL_ASSOC );


/********     FINALIZE ***************/

mysql_close($link);

echo json_encode(array_merge($linel24,$linet0,$linet_1,$linew));

?>
