#! /bin/bash

# genindex.sh
# 01-Oct-14  Markku-Juhani O. Saarinen <mjos@iki.fi>
# create index.html

../brutus -x2 ../aeadlibs/*ref.so
./genhtml.sh | tee index.html
rm -f *.def *.pgm
echo "DONE"

