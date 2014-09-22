brutus
======

22-Sep-14  Markku-Juhani O. Saarinen <mjos@iki.fi>

BRUTUS is an alternative testing framework for [CAESAR ciphers](http://competitions.cr.yp.to/caesar-submissions.html).
Individual ciphers are compiled into shared libraries. A testing `main()` 
program then loads only those ciphers that you want to run on a particular 
test on. BRUTUS runs on most modern Linux systems, including better-equipped
embedded systems. The codebase is fairly small, about 1000 lines total.

The main advantage of BRUTUS over SUPERCOP is that it allows a rather rapid
testing cycle. You can compile and run basic tests on all (nearly 200)
candidate variants in just few minutes. You can also easily add your own
statistical tests or test vector generation/verification code into the
framework.

No warranty whatsoever. If you use this code in your project, attribute me.

Cheers,
- Markku http://www.mjos.fi.

#Short Tutorial

Brutus is written for a recent Linux system. In addition to compilers
and gnumake you will need OpenSSL's `cryptolib` and headers installed as 
some candidates utilize AES. On debian/ubuntu:
```
$ sudo apt-get install libssl-dev
```
Choose a working directory and cd there. Extract the Brutus package.
```
$ tar xfvJ brutus-<version>.txz
```
You will need the SUPERCOP package. The latest version at the time of
writing can be download and extracted as:
```
$ wget http://hyperelliptic.org/ebats/supercop-20140910.tar.bz2
$ tar xfvj supercop-20140910.tar.bz2
```
Don't to compile or run SUPERCOP! Brutus just needs a symlink to the sources:
```
$ cd brutus
$ ln -s ../supercop-20140910/crypto_aead crypto_aead
```
Replace that with correct path to the crypto_aead subdirectory. You may
alternatively create a local crypto_aead subdirectory and copy only those
ciphers that you are interested in there manually.

Now check the compiler flags in `brutus_cflags.cfg`. If have an old compiler 
or an otherwise exotic system, you may want to discard native optimizations and
go with something geneneric like:
```
echo -n "-Wall -O3" > brutus_cflags.cfg 
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
$ ls aeadlibs/stribob192r1-*
aeadlibs/stribob192r1-8bit.err  aeadlibs/stribob192r1-ref.so
aeadlibs/stribob192r1-8bit.so   aeadlibs/stribob192r1-xmm.err
aeadlibs/stribob192r1-ref.err   aeadlibs/stribob192r1-xmm.so
```
We also have the `brutus` executable binary:

```
$ ./brutus
Usage: brutus [flags] aead1.so aead2.so ..
   -q  Switch off verbatim.
   -c  Coherence test.
   -s  Encryption/Authentication Speed test.
   -f  Fast encryption throughput test.
```
Brutus is invoked with library filenames. The following wildcard will test
all variants of keyak for speed and coherence:
```
$ ./brutus -c -s aeadlibs/*keyakv1-*.so
```
The output should be mostly self-explanatory.

