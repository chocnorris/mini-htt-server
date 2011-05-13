 <?php
 print "PHP EJECUTANDOSE\n";
 print "LO EJECUTO CON LOS ARGUMENTOS: \n";

while (count($argv)>0){
	$arg=array_shift($argv);
	print $arg;
	print "\n";

}
?>

