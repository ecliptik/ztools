# RISC OS makefile for the Infocom tools
#
# Note that you will have to place the source files into c and h
# directories, and create an o directory in the usual fashion
# Then drag the makefile to AMU.

CC = cc
LD = link
SQ = squeeze
CFLAGS = -c -ff -throwback
LDFLAGS =
LIBS = C:o.Stubs

CINC =
COBJS = check.o

IINC = tx.h
IOBJS = infodump.o showhead.o showdict.o showobj.o showverb.o txio.o getopt.o infinfo.o symbols.o

PINC = pix2gif.h
POBJS = pix2gif.o

TINC = tx.h
TOBJS = txd.o txio.o showverb.o getopt.o infinfo.o symbols.o showobj.o

.SUFFIXES: .o .c
.c.o:;		$(CC) $(CFLAGS) -o $@ $<

all : check infodump pix2gif txd

check : $(COBJS)
	$(LD) -o $@ $(LDFLAGS) $(COBJS) $(LIBS)
	$(SQ) $@

$(COBJS) : $(CINC)

infodump : $(IOBJS)
	$(LD) -o $@ $(LDFLAGS) $(IOBJS) $(LIBS)
	$(SQ) $@

$(IOBJS) : $(IINC)

pix2gif : $(POBJS)
	$(LD) -o $@ $(LDFLAGS) $(POBJS) $(LIBS)
	$(SQ) $@

$(POBJS) : $(PINC)

txd : $(TOBJS)
	$(LD) -o $@ $(LDFLAGS) $(TOBJS) $(LIBS)
	$(SQ) $@

$(TOBJS) : $(TINC)
