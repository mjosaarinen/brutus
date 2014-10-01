#! /bin/bash

# genhtml.sh
# 01-Oct-14  Markku-Juhani O. Saarinen <mjos@iki.fi>
# called by genindex.sh. requires imagemagick to convert pgm -> png

echo "<!DOCTYPE HTML>"
echo "<html>"
echo "<head>"
echo "<title>CAESAR AEAD Feedback Gallery</title>"
echo "</head>"
echo "<body>"

echo "<center>"
echo "<br><b>Gallery of CAESAR First Round Candidate Feedback Properties</b><br>"
echo "<i>Derived from reference implementations with"
echo "<a href=\"https://github.com/mjosaarinen/brutus\">BRUTUS</a>.</i><br>"
echo "<br>"
echo "One pixel per byte. Grid lines every 16 bytes (128 bits).<br>" 
echo "<i>X: Ciphertext difference. Approximately 256 bytes per line.<br>"
echo "Y: Plaintext modification offset. Only one byte changed per line.</i><br>"
echo "<br><a href=\"https://mjos.fi\">Markku-Juhani O. Saarinen</a>" 
echo "&lt;mjos@iki.fi&gt;<br>Generated"
date
echo "</center>"

echo "<table style=\"width:100%\">"
echo -n "<tr>"

n=0
for pgm in *.pgm 
do
	nam=`echo $pgm | sed 's/.pgm//'`
	convert $pgm $nam.png
	
	if [ $((n % 4)) -eq 0 ]
	then
		echo -n "</tr>"
		echo "<tr>"
	fi
	n=$((n + 1))
	echo "<td><img src=\""$nam.png"\"><br>"
	echo -n $n". "
	cat $nam.def 
	echo "</td>"
done

echo "</tr></table>"

echo "</body>"
echo "</html>"

