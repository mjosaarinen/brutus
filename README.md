brutus
======

22-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

BRUTUS is an alternative testing framework for [CAESAR ciphers](http://competitions.cr.yp.to/caesar-submissions.html).
Individual ciphers are compiled into shared libraries. A testing `main()` 
program then loads only those ciphers that you want to run on a particular 
test on. BRUTUS runs on most modern Linux systems, including better-equipped
embedded systems. The codebase is fairly small and easily modifiable.

The main advantage of BRUTUS over SUPERCOP is that it allows a rapid
testing cycle. You can compile and run basic tests on over 200 candidate 
variants in just few minutes (about 95% currently runnable). You can also 
easily add your own test code into the framework.

After fixing a bunch of memory leaks and other horrendous bugs in the
implementations (you guys really should NOT do ANY security work),
the reference ciphers are now included with the BRUTUS package.
Many obvious bugs still remain but I haven't touched implementation errors
that don't threaten the stability of the testing framework. It's your
shame if you can't code.  

No warranty whatsoever. I don't own the cipher implementations.
If you use my code in your project, attribute me.

Cheers,
- Markku http://www.mjos.fi.

#Short Tutorial

Brutus is written for a recent-release Linux system. In addition to compilers
and gnu make you will need OpenSSL's `cryptolib` and headers installed as 
some candidates utilize AES. On debian/ubuntu:
```
$ sudo apt-get install libssl-dev
```
Choose a working directory and cd there. Extract the Brutus package:
```
$ git clone https://github.com/mjosaarinen/brutus.git
```
or

```
$ wget https://mjos.fi/dist/brutus-current.txz
$ tar xfvJ brutus-current.txz
```
Now check the compiler flags in `brutus_cc.cfg`. If you have an old compiler 
or an otherwise exotic system, you may want to discard native optimizations and
go with something geneneric like:
```
echo "gcc -Wall -O3" > brutus_cc.cfg 
```

We are ready to compile the libraries and test framework. Just write:
```
$ make
```
This takes one or two minutes to complete, depending on your system.
When `make` is called for the first time, the main test code and modules
are first compiled and then the makefile calls the dynlib compiling
script `mkaeadlibs.sh`. If your system has cryptolib in some exotic location,
you need to tweak this script. If you play with some part the test code, the
makefile will just recompile that part for your next test, not the libraries.

In the end, the file `aeadlibs.txt` contains the list of ciphers that produced
at least some kind of library. All of the dynamic libraries are in `aeadlibs`
subdirectory. The compiler output of each library is in `*.err` and `*.so` is
the shared library (if it exists). For example:

```
$ ls aeadlibs/stribob192*
aeadlibs/stribob192r1-8bit.err    aeadlibs/stribob192r2d2-bitslice.err
aeadlibs/stribob192r1-8bit.so     aeadlibs/stribob192r2d2-bitslice.so
aeadlibs/stribob192r1-ref.err     aeadlibs/stribob192r2d2-neon.err
aeadlibs/stribob192r1-ref.so      aeadlibs/stribob192r2d2-ref.err
aeadlibs/stribob192r1-xmm.err     aeadlibs/stribob192r2d2-ref.so
aeadlibs/stribob192r1-xmm.so      aeadlibs/stribob192r2d2-ssse3.err
aeadlibs/stribob192r2d2-8bit.err  aeadlibs/stribob192r2d2-ssse3.so
aeadlibs/stribob192r2d2-8bit.so
```
We also have the `brutus` executable binary:

```
$ ./brutus
Usage: brutus [flags] aead1.so aead2.so ..
  -h   Quick help
  -q   Switch off verbose
  -kN  Generate KAT for max length N
  -tN  Force exit after N seconds
  -rN  Use random seed N
  -cN  Coherence test (N sec timeout)
  -sN  Encryption/Authentication Speed (N secs each)
  -fN  Fast throughput test (N secs for enc/dec)
```
Brutus is invoked with flags and library filenames. The following wildcard 
will quickly test all variants of keyak for speed and coherence. Note that
all numbers "N" are optional; there are defaults. 
```
$ ./brutus -c2 -f aeadlibs/*keyakv1-*.so

BRUTUS (Sep 24 2014) by Markku-Juhani O. Saarinen <mjos@iki.fi>
[lakekeyakv1-ref] Coherence Check (limit=2 sec)  key=16  nsec=0  npub=16  a=16
[lakekeyakv1-ref] Throughput (limit=1 sec)  key=16  nsec=0  npub=16 a=16
[lakekeyakv1-ref] 43788.04 kB/s  encrypt(mlen=65536 adlen=0)
[lakekeyakv1-ref] 43811.63 kB/s  decrypt(mlen=65536 adlen=0)
[oceankeyakv1-ref] Coherence Check (limit=2 sec)  key=16  nsec=0  npub=16  a=16
[oceankeyakv1-ref] Throughput (limit=1 sec)  key=16  nsec=0  npub=16 a=16
[oceankeyakv1-ref] 43156.03 kB/s  encrypt(mlen=65536 adlen=0)
[oceankeyakv1-ref] 43047.13 kB/s  decrypt(mlen=65536 adlen=0)
[riverkeyakv1-ref] Coherence Check (limit=2 sec)  key=16  nsec=0  npub=16  a=16
[riverkeyakv1-ref] Throughput (limit=1 sec)  key=16  nsec=0  npub=16 a=16
[riverkeyakv1-ref] 18984.20 kB/s  encrypt(mlen=65536 adlen=0)
[riverkeyakv1-ref] 18877.08 kB/s  decrypt(mlen=65536 adlen=0)
[seakeyakv1-ref] Coherence Check (limit=2 sec)  key=16  nsec=0  npub=16  a=16
[seakeyakv1-ref] Throughput (limit=1 sec)  key=16  nsec=0  npub=16 a=16
[seakeyakv1-ref] 42998.03 kB/s  encrypt(mlen=65536 adlen=0)
[seakeyakv1-ref] 43321.04 kB/s  decrypt(mlen=65536 adlen=0
```
The output should be mostly self-explanatory.

