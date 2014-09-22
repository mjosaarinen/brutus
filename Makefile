# Makefile
# 21-Sep-14 Markku-Juhani O. Saarinen <mjos@iki.fi>

DIST		= brutus
BIN		= brutus
OBJS		= src/main.o src/util.o \
		src/test_coherent.o src/test_speed.o

CC		= gcc
CFLAGS		= $(shell cat brutus_cflags.cfg)
LIBS		= -ldl -lm
LDFLAGS		=
INCS		= -Iinc

all:		$(BIN) aeadlibs.txt

aeadlibs.txt:	crypto_aead
		./mkaeadlibs.sh

$(BIN):		$(OBJS)
		$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LIBS)

.c.o:
		$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
		rm -rf $(DIST)-*.txz $(OBJS) $(BIN) \
			aeadlibs aeadlibs.txt mkaeadlibs.log

dist:		clean		
		cd ..; tar cfvJ $(DIST)/$(DIST)-`date -u "+%Y%m%d%H%M00"`.txz \
			$(DIST)/*
