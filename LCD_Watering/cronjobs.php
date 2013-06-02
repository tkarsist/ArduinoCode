<?php

$username="root";
$password="bemari";
$database="cron";

mysql_connect("localhost",$username,$password);

@mysql_select_db($database) or die( "Unable to select database");

if ($_SERVER['REQUEST_METHOD'] == 'POST') { 
	if (isset($_POST["id"]) && !empty($_POST["id"])) {
		$query='delete from cronjobs where id="'.$_POST["id"].'"';
		mysql_query($query);
		echo "Tuhosit id:n : ".$_POST["id"];
	}
	if (isset($_POST["add"]) && !empty($_POST["add"])) {
		$query='insert into cronjobs values(null,"'.$_POST["minute"].'","'.$_POST["hour"].'","'.$_POST["day"].'","'.$_POST["month"].'","'.$_POST["weekday"].'","'.$_POST["message"].'");';
		//echo $query;
		mysql_query($query);
		echo "Uusi rivi pulahti";
	}
}


$query="select * from cronjobs";
$result=mysql_query($query);
$num=mysql_numrows($result);
?>
<html><head></head>
<body>
	<h1>Cronjobs<h1>
	<h2><?php echo date("d-m-Y H:i:s",time());?></h2>

			<table border="1">
				<tr>
					<th>Minute</th>
					<th>Hour</th>
					<th>Day</th>
					<th>Month</th>
					<th>Weekday</th>
					<th>Message</th>
					<th>Action</th>

				</tr>	
<?php
$i=0;
while($i<$num){
	?>
	<form name="input" action="<?php echo $_SERVER['PHP_SELF'];?>" method="post">
			<input type="hidden" name="id" value="<?php echo mysql_result($result,$i,"id");?>">

				<tr>
					<td><?php echo mysql_result($result,$i,"minute");?></td>
					<td><?php echo mysql_result($result,$i,"hour");?></td>
					<td><?php echo mysql_result($result,$i,"day");?></td>
					<td><?php echo mysql_result($result,$i,"month");?></td>
					<td><?php echo mysql_result($result,$i,"weekday");?></td>
					<td><?php echo mysql_result($result,$i,"message");?></td>
					<td><input type="Submit" value="Delete"></td>

				</tr>
			
		</form>		
<?php

	$i=$i+1;
}




mysql_close();


?>
<form name="input" action="<?php echo $_SERVER['PHP_SELF'];?>" method="post">
			<input type="hidden" name="add" value="add">

				<tr>
					<td><input type="text" name="minute"></td>
					<td><input type="text" name="hour"></td>
					<td><input type="text" name="day"></td>
					<td><input type="text" name="month"></td>
					<td><input type="text" name="weekday"></td>
					<td><input type="text" name="message"></td>
					<td><input type="Submit" value="Add"></td>

				</tr>
			
		</form>
</table>
</body>
</html>
