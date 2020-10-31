<?php
header("Access-Control-Allow-Origin: *");

require_once "lib.php";

$host="localhost";
$login="local";
$pass="qq11";
$dbname="adlerweather";

// Подключиться к базе  данных
$link = mysql_connect($host, $login, $pass);
if (!$link) {
    die('Ошибка соединения: ' . mysql_error());
}
$SelectResult = mysql_select_db($dbname, $link);
if (!$SelectResult) {
    die('Ошибка выбора базы данных: ' . mysql_error());
}

define(DEFAULT_LAST_N,24*30);
define(DEFAULT_EVERY_SECONDS_INTERVAL,10*60);


$periodlength=$_REQUEST["lastnhours"];
if (empty($periodlength))
{
    $periodlength = DEFAULT_LAST_N;
}

$everyseconds=$_REQUEST["everysecondsinterval"];
if (empty($everyseconds))
	$everyseconds = DEFAULT_EVERY_SECONDS_INTERVAL;
print "<!-- OPTIONAL PARAMETERS: 
    lastnhours - output specified last hours; by default [".$periodlength."] hours. Received praram: [".$_REQUEST["lastnhours"]."]
    everysecondsinterval - agregate data to specivied interval in seconds; by default [".$everyseconds."] seconds (so 600 means that all data between 0:00 and 0:10 would be averaged, for pump status is used MAX)
-->";

// Отправить запрос
$q = "SELECT * FROM data WHERE MINUTE(`date`) % ".$everyseconds."=0 AND `date` > DATE_SUB(NOW(), INTERVAL ".$periodlength." HOUR)";
$q = "SELECT date, AVG(bmp) as bmp, AVG(bmh) as bmh, AVG(bmt) as bmt, AVG(temp) as temp, AVG(hum) as hum, AVG(ow1) as ow1, AVG(amb) as amb, AVG(obj) as obj  FROM weatherdata WHERE `date` > DATE_SUB(NOW(), INTERVAL ".$periodlength." HOUR) GROUP by UNIX_TIMESTAMP(date) DIV ".$everyseconds."";
  
print "<!--".$q."-->";

$res=mysql_query($q,$link);
if (!$res) {
    die('Ошибка sql: ' . mysql_error());
}
mysql_close($link);


?>
 <html>
  <head>
    <title>Weather Station</title>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
        google.charts.load('current', {'packages':['line']});

        google.charts.setOnLoadCallback(drawChart);
        google.charts.setOnLoadCallback(drawChart2);
        google.charts.setOnLoadCallback(drawChart3);


        function drawChart() {
          var data = new google.visualization.DataTable();

          data.addColumn('datetime', 'Day');
          data.addColumn('number', 'DHT22 Temp');
          data.addColumn('number', 'BME Temp');
          data.addColumn('number', 'OW1 Temp');
          data.addColumn('number', 'MLX Temp');
         
<?php
    while( $line= mysql_fetch_array($res, MYSQL_BOTH) )
    {
		$Temp=$line['ow1'];
        //print "data.addRow([new Date(".date("Y",DBStrToStamp($line['date'])).", ".date("m",DBStrToStamp($line['date'])).", ".date("d",DBStrToStamp($line['date'])).", ".date("H",DBStrToStamp($line['date'])).", ".date("i",DBStrToStamp($line['date'])).", ".date("s",DBStrToStamp($line['date']))."), ".$line['temp'].", ".$line['hum']."]);\n";
        print "data.addRow([new Date(".(DBStrToStamp($line['date'])*1000)."), ".checkValidValue($line['temp'], VALIDATION_TYPE_TEMP).", ".checkValidValue($line['bmt'], VALIDATION_TYPE_TEMP).", ".checkValidValue($line['ow1'], VALIDATION_TYPE_TEMP).", ".checkValidValue($line['amb'], VALIDATION_TYPE_TEMP)."]);\n";
    }
	mysql_data_seek($res, 0);
	//[1,  37.8, 80.8, 41.8],
    //[2,  30.9, 69.5, 32.4],

?>
         var options = {
            interpolateNulls: true,
            chart: {
              title: 'Water Station',
              subtitle: 'Температура и влажность воздуха'
            },
			 legend: { 
				position: 'bottom', 
				alignment: 'center' ,
				orientation: 'vertical',
			},
            width: 900,
            height: 500,
            series: {
                0: {
                    axis: 'Temp'
                },
                1: {
                    axis: 'Temp'
                },
                2: {
                    axis: 'Temp'
                },
                3: {
                    axis: 'Temp'
                },

            },
            axes: {
                y: {
                    Temp: {
                        range: {
                            max: 20,
                            min: 35
                        },  
                        label: 'Температура (C)'
                    },
                    Hum: {
                        range: {
                            max: 30,
                            min: 90
                        },  
                        label: 'Влажность (%)'
                    }
                }
            },
            vAxes: {
                Hum: {
                    gridlines: {count: 1,  color: 'none'},
                    minorGridlines: {count: 1},
                }                
            },

          };


          //var chart = new google.charts.Line(document.getElementById('curve_chart'));
          var chart = new google.charts.Line(document.getElementById('curve_chart'));
          chart.draw(data, google.charts.Line.convertOptions(options));

        }

		function drawChart3() {
          var data = new google.visualization.DataTable();
          data.addColumn('datetime', 'Day');
          data.addColumn('number', 'DHT22 hum');
          data.addColumn('number', 'BME hum');
<?php
	//[1,  37.8, 80.8, 41.8],
    //[2,  30.9, 69.5, 32.4],
    while( $line= mysql_fetch_array($res, MYSQL_BOTH) )
    {
        //print "data.addRow([new Date(".date("Y",DBStrToStamp($line['date'])).", ".date("m",DBStrToStamp($line['date'])).", ".date("d",DBStrToStamp($line['date'])).", ".date("H",DBStrToStamp($line['date'])).", ".date("i",DBStrToStamp($line['date'])).", ".date("s",DBStrToStamp($line['date']))."), ".$line['temp'].", ".$line['hum']."]);\n";
        print "data.addRow([new Date(".(DBStrToStamp($line['date'])*1000)."), ".checkValidValue($line['hum'], VALIDATION_TYPE_HUMIDITY).", ".checkValidValue($line['bmh'], VALIDATION_TYPE_HUMIDITY)."]);\n";
    }
	mysql_data_seek($res, 0);
?>
		  var options = {
            interpolateNulls: true,
            chart: {
              title: 'Weather Station',
              subtitle: 'Humidity'
            },
			legend: 'bottom',
            width: 900,
            height: 500,
            series: {
                0: {
                    targetAxisIndex: 0
                },
                1: {
                    targetAxisIndex: 0
                },
            },
            vAxes: {
                0: {
                    minValue: 0,
                    maxValue: 5,
                    label: 'C'
                },
                1: {
                    gridlines: {count: 0},
                    minValue: 200,
                    maxValue: 1100,
                    label: '%'
                }
            }


          };

          var chart = new google.charts.Line(document.getElementById('curve_chart3'));
          chart.draw(data, google.charts.Line.convertOptions(options));
        }
		
        function drawChart2() {
          var data = new google.visualization.DataTable();
          data.addColumn('datetime', 'Day');
          data.addColumn('number', 'Pressure');
          data.addColumn('number', 'NormalPressure');
<?php
	
	$Alt = 165;
	$normalpressure = 760 * exp(-0.029 * 9.81* $Alt /(8.31*(273.15+ $Temp)));
	
	//[1,  37.8, 80.8, 41.8],
    //[2,  30.9, 69.5, 32.4],
    while( $line= mysql_fetch_array($res, MYSQL_BOTH) )
    {
        //print "data.addRow([new Date(".date("Y",DBStrToStamp($line['date'])).", ".date("m",DBStrToStamp($line['date'])).", ".date("d",DBStrToStamp($line['date'])).", ".date("H",DBStrToStamp($line['date'])).", ".date("i",DBStrToStamp($line['date'])).", ".date("s",DBStrToStamp($line['date']))."), ".$line['temp'].", ".$line['hum']."]);\n";
        //$normalpressure = 760 * exp(-0.029 * 9.81* $Alt /(8.31*(273.15+ $line['ow1'])));
		print "data.addRow([new Date(".(DBStrToStamp($line['date'])*1000)."), ".checkValidValue($line['bmp'],VALIDATION_TYPE_PRESSURE).", $normalpressure]);\n";
    }
	mysql_data_seek($res, 0);
?>
		  var options2 = {
            interpolateNulls: true,
            chart: {
              title: 'Weather Station',
              subtitle: 'Pressure'
            },
			legend: 'bottom',
            width: 900,
            height: 500,
            series: {
                0: {
                    targetAxisIndex: 0
                },
            },
            vAxes: {
                0: {
                    minValue: 0,
                    maxValue: 5,
                    label: 'C'
                },
                1: {
                    gridlines: {count: 0},
                    minValue: 200,
                    maxValue: 1100,
                    label: '%'
                }
            }


          };

          var chart2 = new google.charts.Line(document.getElementById('curve_chart2'));
          chart2.draw(data, google.charts.Line.convertOptions(options2));
        }
        
      
    </script>
  </head>
  <body>
    <div id="control" style="width: 900px; height: 30px;font-family: Roboto; font-size: 16px;" fill="#757575">
	<a href="?">Default</a>
	<a href="?everysecondsinterval=60&lastnhours=24">Details last 24 hours</a>
	</div>
    <div id="curve_chart" style="width: 900px; height: 500px"></div>
    <div id="curve_chart3" style="width: 900px; height: 500px"></div>
    <div id="curve_chart2" style="width: 900px; height: 500px"></div>
  </body>
</html>


<?php
/*
$opts = array('http'=>array('header' => "User-Agent:MyAgent/1.0\r\n")); 
//Basically adding headers to the request
$context = stream_context_create($opts);

$json = file_get_contents("http://192.168.0.235/1",false,$context);
$ws = json_decode($json);


print "Текущая температура: ".$ws->T."<br>\n";
print "Текущая влажность: ".$ws->H."<br>\n";

print "<!--";
print_r($ws);
print "-->";
//phpinfo();
*/


?>
