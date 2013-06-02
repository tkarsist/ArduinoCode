<?php
include "php_serial.class.osx.php"; 
include('cronPhp.php');

$job1=new cronPhp("19","13","*","*","*");
$job1->setName("<pili   >");

$cronjobs['haa']=$job1;

$jobSerial=new cronPhp("*","*","*","*","*");
$jobSerial->setName("keepalive");
 
// Let's start the class
$serial = new phpSerial; 
 
// First we must specify the device. This works on both linux and windows (if
// your linux serial device is /dev/ttyS0 for COM1, etc)
$serial->deviceSet("/dev/ttyACM0"); 

// We can change the baud rate
$serial->confBaudRate(115200);
 
// Then we need to open it
$serial->deviceOpen(); 
sleep(5);
$read = $serial->readPort();
$serial->sendMessage("<Hello !>"); 
while(true){
	sleep(5);

	$cronjobs=readCronjobsFromDB($cronjobs);

	foreach($cronjobs as $jobi){
		if($jobi->isTimeForCronJob()){
			if($jobi->getName()=="<RSS        >")
                $serial->sendMessage("<".getRss()."       >");
            else
				$serial->sendMessage($jobi->getName()); 
			echo $jobi->getName();
			echo $jobi->getJobTimeData();

		}		
	}
	if($jobSerial->isTimeForCronJob()){
		$serial->sendMessage($jobSerial->getName()); 
		echo $jobSerial->getName();
		echo $jobSerial->getJobTimeData();

	}

} 
// To write into

 
// Or to read from
$read = $serial->readPort(); 
 
// If you want to change the configuration, the device must be closed
//$serial->deviceClose(); 
 
// We can change the baud rate
//$serial->confBaudRate(2400); 
 
// etc...

function getRss(){
	$rss = simplexml_load_file('http://open.live.bbc.co.uk/weather/feeds/en/658225/3dayforecast.rss');

	//echo '<h1>'. $rss->channel->title . '</h1>';

	$counter="0";
	foreach ($rss->channel->item as $item) {
   		//echo $item->title."\n";
   		if($counter=="0"){
   			preg_match('/(?: )\S+/i', $item->title, $match);
   			$output=$match[0];
   			$output=str_replace(" ","",$output);
   			$output=str_replace(",","",$output);
   			//echo $match[0];
   			//echo $output;
   			$counter=8;
   		}
	}
	return $output;
}

function readCronjobsFromDB($cronJobs){	
	$username="root";
	$password="bemari";
	$database="cron";

	mysql_connect("localhost",$username,$password);

	@mysql_select_db($database) or die( "Unable to select database");

	$cronresults=$cronJobs;
	//var_dump($cronresults);
	$query="select * from cronjobs";
	$result=mysql_query($query);
	$num=mysql_numrows($result);
	$i=0;
	while ($i<$num){
		if(!isset($cronresults[mysql_result($result,$i,"id")])){
			//echo "taalla ollaan";
			$cronresults[mysql_result($result,$i,"id")]=new cronPhp(mysql_result($result,$i,"minute"),mysql_result($result,$i,"hour"),mysql_result($result,$i,"day"),mysql_result($result,$i,"month"),mysql_result($result,$i,"weekday"));
			$cronresults[mysql_result($result,$i,"id")]->setName("<".mysql_result($result,$i,"message")."        >");
		}
		$i+=1;
	}
	$keys=array_keys($cronresults);
	foreach($keys as $key){
		$isThere=0;
		$i=0;
		while ($i<$num){
			if(mysql_result($result,$i,"id")==$key)
				$isThere+=1;
			
			$i+=1;
		}
		if($isThere==0)
				unset($cronresults[$key]);
			
				
	}

	return $cronresults;


}



?>
