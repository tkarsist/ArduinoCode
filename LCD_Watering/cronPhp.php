<?php

class cronPHP{

	private $min;
	private $hour;
	private $day;
	private $month;
	private $weekday;
	private $lastrun;
	private $time;
	private $name;

	public function __construct($min,$hour,$day,$month,$weekday){
		date_default_timezone_set("Europe/Helsinki");
		$this->min=$min;
		$this->hour=$hour;
		$this->day=$day;
		$this->month=$month;
		$this->weekday=$weekday;
		$this->lastrun='0';
		$this->time="0";


	}
	public function setName($name){
		$this->name=$name;
	}
	public function getName(){
		return $this->name;
	}
	public function getJobTimeData(){
		$time=time();
		$timeString="Current time: ".date("d-M-Y H:i:s",$time)." Job last run: ".date("d-M-Y H:i:s",$this->lastrun)."\n";
		return $timeString;
	}
	public function isTimeForCronJob(){
		$this->time=time();
		$cronTimestamped=$this->crontToTimestamp($this->min, $this->hour, $this->day,$this->month,$this->weekday);
		if($this->isSameMinute($this->time,$cronTimestamped)){
			if($this->isSameMinute($this->time,$this->lastrun))
				return false;
			else{
					$this->updateCronJob();
					return true;
				}
			}
			return false;	
		}

		private function updateCronJob(){
			$this->time=time();
			$this->lastrun=$this->time;

		}

		private function isSameMinute($curStamp,$destStamp){
			if ($curStamp>=$destStamp && date ("d-M-Y H:i",$curStamp)==date ("d-M-Y H:i",$destStamp))
				return true;
			return false;
		}

		private function crontToTimestamp($i,$h,$d,$m,$wd){
			$sec=0;
			$min=0;
			$hour=0;
			$day=0;
			$month=0;
			$weekday=-0;


			$tid=time();
			$curMin=date("i",$tid);
			$curHour=date("H",$tid);
			$curDay=date("d",$tid);
			$curMonth=date("m",$tid);
			$curWeekDay=date("N",$tid);
			$year=date("Y",$tid);

			if($i=="*")
				$min=$curMin;
			else
				$min=$i;

			if($h=="*")
				$hour=$curHour;
			else
				$hour=$h;

			if($d=="*")
				$day=$curDay;
			else
				$day=$d;

			if($m=="*")
				$month=$curMonth;
			else
				$month=$m;

			if($wd=="*")
				$weekday=$curWeekDay;
			else
				$weekday=$wd;

			$tstamp=mktime($hour,$min,$sec,$month,$day,$year);
	//echo $tstamp;
			return $tstamp;
		}


	}
?>
