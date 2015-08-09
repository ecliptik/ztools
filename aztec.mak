# Amiga Aztec-C 5.2 makefile for the Infocom tools

CC = ln
CFLAGS = -ms -so -wlc
LDFLAGS =
LIBS = -lc

CINC =
COBJS = check.o

IINC = tx.h
IOBJS = infodump.o showhead.o showdict.o showobj.o showverb.o txio.o getopt.o infinfo.o symbols.o

PINC = pix2gif.h
POBJS = pix2gif.o

TINC = tx.h
TOBJS = txd.o txio.o showverb.o getopt.o infinfo.o symbols.o showobj.o

all : check infodump pix2gif txd

check : $(COBJS)
	$(CC) -o $@ $(LDFLAGS) $(COBJS) $(LIBS)

$(COBJS) : $(CINC)

infodump : $(IOBJS)
	$(CC) -o $@ $(LDFLAGS) $(IOBJS) $(LIBS)

$(IOBJS) : $(IINC)

pix2gif : $(POBJS)
	$(CC) -o $@ $(LDFLAGS) $(POBJS) $(LIBS)

$(POBJS) : $(PINC)

txd : $(TOBJS)
	$(CC) -o $@ $(LDFLAGS) $(TOBJS) $(LIBS)

$(TOBJS) : $(TINC)

