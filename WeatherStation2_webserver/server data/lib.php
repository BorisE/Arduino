<?php
define (VALIDATION_TYPE_NOVALIDATION, 0);
define (VALIDATION_TYPE_TEMP, 10);
define (VALIDATION_TYPE_HUMIDITY, 20);
define (VALIDATION_TYPE_PRESSURE, 30);
define (VALIDATION_TYPE_LUM, 40);
define (VALIDATION_TYPE_RAIN, 50);


define (VALIDATION_TEMP_MAXVALID,   99.0);
define (VALIDATION_TEMP_MINVALID,   -30.0);
define (VALIDATION_HUM_MAXVALID,    100.0);
define (VALIDATION_HUM_MINVALID,    1.0);
define (VALIDATION_PRESS_MAXVALID,  1000.0);
define (VALIDATION_PRESS_MINVALID,  100.0);
define (VALIDATION_LUM_MAXVALID,  300000.0);
define (VALIDATION_LUM_MINVALID,  0.0);
define (VALIDATION_RAIN_MINVALID, 0);
define (VALIDATION_RAIN_MAXVALID, 1024);

function checkValidValue($val, $validtype = VALIDATION_TYPE_TEMP)
{
    switch ($validtype)
    {
        case VALIDATION_TYPE_TEMP:
            if ($val > VALIDATION_TEMP_MAXVALID || $val < VALIDATION_TEMP_MINVALID)
            {
                return "null";
            }
            else
                return $val;
        case VALIDATION_TYPE_HUMIDITY:
            if ($val > VALIDATION_HUM_MAXVALID || $val < VALIDATION_HUM_MINVALID)
                return "null";
            else
                return $val;
        case VALIDATION_TYPE_PRESSURE:
            if ($val > VALIDATION_PRESS_MAXVALID || $val < VALIDATION_PRESS_MINVALID)
                return "null";
            else
                return $val;
        case VALIDATION_TYPE_LUM:
            if ($val > VALIDATION_LUM_MAXVALID || $val < VALIDATION_LUM_MINVALID)
                return "null";
            else
                return $val;
        case VALIDATION_TYPE_RAIN:
            if ($val > VALIDATION_RAIN_MAXVALID || $val < VALIDATION_RAIN_MINVALID)
                return "null";
            else
                return $val;
  

  case VALIDATION_TYPE_NOVALIDATION:
            return $val;
        default:
            return $val;
    }
}


//////////////////////////////////////////////////////////////////
function float_parse($st)
{
// Точка/запятая

	$st=str_replace(",",".",$st);
	return $st;
}

#---------------------------------------------------------------------------------------------------------------------------
# Перевод из обычного формата "Y-m-d [H:i:s]" в timestamp
#---------------------------------------------------------------------------------------------------------------------------
   function DBStrToStamp($str)
   {
      $errorf=0;
      $res="";
      //Отсекаем дату и время если есть)
      if (substr_count($str," ")==1):
         $strtime=trim(strstr($str," "));
         $strdate=substr($str,0,strpos($str," "));
      else:
         $strtime="";
         $strdate=$str;
      endif;

      //Обработка даты
      if (substr_count($strdate,"-")==2)
      {
         $y=substr($strdate,0,strpos($strdate,"-"));
         $strdate=strstr($strdate,"-");
         $m=substr($strdate,1,strrpos($strdate,"-")-1);
         $d=substr($strdate,strrpos($strdate,"-")+1);
         if (!is_numeric($y) or !is_numeric($m) or !is_numeric($d)):
            $errorf=2;
         endif;
      }
      else
      {
         $errorf=3;
      }

      //Обработка времени
      if ($strtime<>""):
         if (substr_count($strtime,":")==2)
         {
            $h=substr($strtime,0,strpos($strtime,":"));
            $strtime=strstr($strtime,":");
            $mn=substr($strtime,1,strrpos($strtime,":")-1);
            $sec=substr($strtime,strrpos($strtime,":")+1);
            if (!is_numeric($h) or !is_numeric($mn) or !is_numeric($sec)):
               $errorf=4;
            endif;
         }
         else
         {
            $errorf=5;
         }
      endif;

      if ($errorf):
         $res=-$errorf;
      elseif ($strtime==""):
         $res=mktime(0,0,0,$m,$d,$y);
      else:
         $res=mktime($h,$mn,$sec,$m,$d,$y);
         if ($res==-1) $res=0; // для случая, когда входящая дата 0000
     endif;

      return $res;
   }

#---------------------------------------------------------------------------------------------------------------------------
# Перевод из обычного формата "d-m-Y [H:i:s]" в timestamp
#---------------------------------------------------------------------------------------------------------------------------
   function SStrToStamp($str)
   {
      $errorf=0;
      $res="";
      //Отсекаем дату и время если есть)
      if (substr_count($str," ")==1):
         $strtime=trim(strstr($str," "));
         $strdate=substr($str,0,strpos($str," "));
      else:
         $strtime="";
         $strdate=$str;
      endif;

      //Обработка даты
      if (substr_count($strdate,"-")==2)
      {
         $d=substr($strdate,0,strpos($strdate,"-"));
         $strdate=strstr($strdate,"-");
         $m=substr($strdate,1,strrpos($strdate,"-")-1);
         $y=substr($strdate,strrpos($strdate,"-")+1);
         if (!is_numeric($y) or !is_numeric($m) or !is_numeric($d)):
            $errorf=2;
         endif;
      }
      else
      {
         $errorf=3;
      }


      //Обработка времени
      if ($strtime<>""):
         if (substr_count($strtime,":")==2)
         {
            $h=substr($strtime,0,strpos($strtime,":"));
            $strtime=strstr($strtime,":");
            $mn=substr($strtime,1,strrpos($strtime,":")-1);
            $sec=substr($strtime,strrpos($strtime,":")+1);
            if (!is_numeric($h) or !is_numeric($mn) or !is_numeric($sec)):
               $errorf=4;
            endif;
         }
         else if (substr_count($strtime,":")==1)
         {
            $h=substr($strtime,0,strpos($strtime,":"));
            $strtime=strstr($strtime,":");
            $mn=substr($strtime,strrpos($strtime,":")+1);
            $sec=0;
            if (!is_numeric($h) or !is_numeric($mn)):
               $errorf=5;
            endif;
         }
         else
         {
            $errorf=6;
         }
      endif;

      if ($errorf):
         $res=-$errorf;
      elseif ($strtime==""):
         $res=mktime(0,0,0,$m,$d,$y);
      else:
         $res=mktime($h,$mn,$sec,$m,$d,$y);
      endif;

      return $res;
   }
#---------------------------------------------------------------------------------------------------------------------------
# Перевод из обычного формата "d-m-Y [H:i:s]" в mysql'овский "Y-m-d [H:i:s]"
#---------------------------------------------------------------------------------------------------------------------------
   function SStrToDBStr($str)
   {
      $stm=SStrToStamp($str);
      if ($stm>0) $stm=date("Y-m-d".(strpos($str,":")===false?"":" H:i:s"),$stm);
      return $stm;
   }





?>