#! /bin/bash

#  mkaeadlibs.sh 
#  21-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

CRYPTO_AEAD=crypto_aead
AEADLIBS=aeadlibs
BRUTUS_CC=`cat brutus_cc.cfg`

# cleanup
rm -rf $AEADLIBS aeadlibs.txt 
mkdir $AEADLIBS

ls -1d $CRYPTO_AEAD/*/* | {
	while read srcdir
	do
		aead=`echo $srcdir | sed 's@'$CRYPTO_AEAD'/@@g' | tr '/' '-'`
		echo == $aead == 
		srcfiles=`ls -1 $srcdir/*.c $srcdir/*.cpp $srcdir/*.cc \
			$srcdir/*.s $srcdir/*.S  2> /dev/null`
		echo COMPILING $srcfiles
		$BRUTUS_CC -shared -fPIC -o $AEADLIBS/$aead.so \
			-Iinc -I$srcdir -DBRUTUS_NAME='"'$aead'"' \
			$srcfiles src/aead_params.c -lcrypto 2> $AEADLIBS/$aead.err
		if [ -e $AEADLIBS/$aead.so ]
		then
			echo -n 'OK.  ' 
			du -b $AEADLIBS/$aead.so 
		else
			echo -n 'FAIL:' 
			wc $AEADLIBS/$aead.err
		fi
		echo
	done } | tee mkaeadlibs.log

# create the list
ls -1 $AEADLIBS/*.so > aeadlibs.txt

