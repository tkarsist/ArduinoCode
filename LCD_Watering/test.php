<?php
include "php_serial.class.osx.php"; 
include('cronPhp.php');

$job1=new cronPhp("19","13","*","*","*");
$job1->setName("<Chili   >");
$job2=new cronPhp("25","13","*","*","*");
$job2->setName("<HAHA    >");
$job3=new cronPhp("30","13","*","*","*");
$job3->setName("<IHANA   >");
$job4=new cronPhp("35","13","*","*","*");
$job4->setName("<HAHAA   >");
$job5=new cronPhp("13","20","*","*","*");
$job5->setName("<NAAA  >");
$job6=new cronPhp("*","*","*","*","*");
$job6->setName("p");
$job7=new cronPhp("20","14","1","*","*");
$job7->setName("<NUKU   >");
$job8=new cronPhp("20","8","2","*","*");
$job8->setName("<AAMU   >");
$job9=new cronPhp("10","14","1","*","*");
$job9->setName("<blaa   >");
 
// Let's start the class
$serial = new phpSerial; 
 
// First we must specify the device. This works on both linux and windows (if
// your linux serial device is /dev/ttyS0 for COM1, etc)
$serial->deviceSet("/dev/ttyACM1"); 

// We can change the baud rate
$serial->confBaudRate(115200);
 
// Then we need to open it
$serial->deviceOpen(); 
sleep(5);
$read = $serial->readPort();
$serial->sendMessage("<Hello !>"); 
while(true){
	sleep(5);
	if($job1->isTimeForCronJob()){
		$serial->sendMessage($job1->getName()); 
		echo $job1->getName();
		echo $job1->getJobTimeData();
	}
		if($job2->isTimeForCronJob()){
		$serial->sendMessage($job2->getName()); 
		echo $job2->getName();
		echo $job2->getJobTimeData();
	}
		if($job3->isTimeForCronJob()){
		$serial->sendMessage($job3->getName()); 
		echo $job3->getName();
		echo $job3->getJobTimeData();

	}
			if($job4->isTimeForCronJob()){
		$serial->sendMessage($job4->getName()); 
		echo $job4->getName();
		echo $job4->getJobTimeData();

	}
			if($job5->isTimeForCronJob()){
		$serial->sendMessage($job5->getName()); 
		echo $job5->getName();
		echo $job5->getJobTimeData();

	}
			if($job6->isTimeForCronJob()){
		$serial->sendMessage($job6->getName()); 
		echo $job6->getName();
		echo $job6->getJobTimeData();

	}
			if($job7->isTimeForCronJob()){
		$serial->sendMessage($job7->getName()); 
		echo $job7->getName();
		echo $job7->getJobTimeData();

	}
	if($job8->isTimeForCronJob()){
		$serial->sendMessage($job8->getName()); 
		echo $job8->getName();
		echo $job8->getJobTimeData();

	}
        if($job9->isTimeForCronJob()){
                $job9->setName("<".getRss().">");
		$serial->sendMessage($job9->getName());
                echo $job9->getName();
                echo $job9->getJobTimeData();

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
?>


?>
