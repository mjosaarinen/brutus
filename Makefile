# Makefile
# 21-Sep-14 Markku-Juhani O. Saarinen <mjos@iki.fi>

DIST		= brutus
BIN		= brutus
OBJS		= src/main.o src/util.o \
		src/speed.o \
		src/coherence.o \
		src/kat.o \
		src/xprmnt.o

CC		= $(shell cat brutus_cc.cfg)
LIBS		= -ldl -lm
LDFLAGS		=
INCS		= -Iinc

all:		$(BIN) aeadlibs.txt

aeadlibs.txt:	crypto_aead
		./mkaeadlibs.sh

$(BIN):		$(OBJS)
		$(CC) -o $(BIN) $(OBJS) $(LIBS)

.c.o:
		$(CC) $(INCS) -c $< -o $@

clean:
		rm -rf $(DIST)-*.txz $(DIST)-*.txz.asc $(OBJS) $(BIN) \
			aeadlibs aeadlibs.txt mkaeadlibs.log

dist:		clean		
		cd ..; tar cfvJ $(DIST)/$(DIST)-current.txz $(DIST)/*

sig:		dist
		gpg -a -b $(DIST)-current.txz
