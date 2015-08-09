# BeOS makefile for the Infocom tools
# Provided by Stephen van Egmond <svanegmond@home.com>

CC = $(BE_C_COMPILER)
#Some systems declare getopt, others do not.  Pick whichever works

#project independant information
MACHINE=$(shell uname -m)
ifeq ($(MACHINE), BePC)
        CPU = x86
else
        CPU = ppc
endif
           
ifeq ($(CC), gcc)
	CFLAGS = -DHAS_GETOPT
else
	CFLAGS =
endif

LDFLAGS =
LIBS =

CINC =
COBJS = check.o

IINC = tx.h
IOBJS = infodump.o showhead.o showdict.o showobj.o showverb.o txio.o infinfo.o symbols.o

PINC = pix2gif.h
POBJS = pix2gif.o

TINC = tx.h
TOBJS = txd.o txio.o showverb.o infinfo.o symbols.o showobj.o

MANPAGES = infodump.1 inforead.1 txd.1 check.1 pix2gif.1
FORMATTEDMAN = $(MANPAGES:.1=.man)

all : directory $(CPU)/check $(CPU)/infodump $(CPU)/pix2gif $(CPU)/txd

directory :
	mkdir -p $(CPU)

$(CPU)/check : $(COBJS)
	$(CC) -o $@ $(LDFLAGS) $(COBJS) $(LIBS)

$(COBJS) : $(CINC)

$(CPU)/infodump : $(IOBJS)
	$(CC) -o $@ $(LDFLAGS) $(IOBJS) $(LIBS)

$(IOBJS) : $(IINC)

$(CPU)/pix2gif : $(POBJS)
	$(CC) -o $@ $(LDFLAGS) $(POBJS) $(LIBS)

$(POBJS) : $(PINC)

$(CPU)/txd : $(TOBJS)
	$(CC) -o $@ $(LDFLAGS) $(TOBJS) $(LIBS)

$(TOBJS) : $(TINC)

clean :
	-rm *.o $(CPU)/check $(CPU)/infodump $(CPU)/pix2gif $(CPU)/txd
