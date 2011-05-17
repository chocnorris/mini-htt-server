 <?php
 echo  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"> ";
 echo	"<html lang=\"es\">";
 echo	"<head>";
 echo	"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
 echo	"<title>Ejemplo</title>";
 echo	"</head>";
 echo	"<body>";
 echo	"<h1>Ejemplo php</h1>";
 echo 	"<p>Las variables que se pasaron: <br>";
 echo	"a=";
 echo 	$_GET['a'];
 echo 	"<br>";
 echo	"b=";
 echo 	$_GET['b'];
 echo 	"<br>";
 echo	"c=";
 echo 	$_GET['c'];
 echo	"\n</p>";
 echo	"<p> \n";
 echo 	$_SERVER['QUERY_STRING'];
 echo	"</p>";
 echo 	"\n";
 echo	"</body>";
 echo  	"</html>";
?>
