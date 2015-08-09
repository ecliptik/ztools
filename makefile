# Unix C makefile for the Infocom tools

CC = cc
NROFF	= nroff

#Some systems declare getopt, others do not.  Pick whichever works
#CFLAGS = -O -DHAS_GETOPT
CFLAGS = -O
LDFLAGS =
LIBS =


.SUFFIXES: .c .h .1 .man

.1.man:
	$(NROFF) -man $*.1 | col -b > $*.man

MANPAGES = infodump.1 inforead.1 txd.1 check.1 pix2gif.1
FORMATTEDMAN = $(MANPAGES:.1=.man)

CINC =
COBJS = check.o

IINC = tx.h
IOBJS = infodump.o showhead.o showdict.o showobj.o showverb.o txio.o infinfo.o symbols.o

PINC = pix2gif.h
POBJS = pix2gif.o

TINC = tx.h
TOBJS = txd.o txio.o showverb.o infinfo.o symbols.o showobj.o

all : check infodump pix2gif txd doc

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

clean :
	-rm *.o check infodump pix2gif txd $(FORMATTEDMAN)

doc: $(FORMATTEDMAN)
