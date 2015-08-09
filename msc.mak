# Microsoft Quick C makefile for the Infocom tools

CC = qcl
CFLAGS = /AL /Ox /DNDEBUG
LD = qlink
LDFLAGS = /NOI /ST:0x1000
LIBS =

CINC =
COBJS = check.obj

IINC = tx.h
IOBJS = infodump.obj showhead.obj showdict.obj showobj.obj showverb.obj \
		txio.obj getopt.obj infinfo.obj symbols.obj

PINC = pix2gif.h
POBJS = pix2gif.obj

RINC =
ROBJS = inforead.obj

TINC = tx.h
TOBJS = txd.obj txio.obj showverb.obj getopt.obj infinfo.obj symbols.obj showobj.o

all: check.exe infodump.exe inforead.exe pix2gif.exe txd.exe

check.exe : $(COBJS)
	$(LD) $(LDFLAGS) @<<
$(COBJS)
$@
NUL
$(LIBS);
<<

$(COBJS) : $(CINC)

infodump.exe : $(IOBJS)
	$(LD) $(LDFLAGS) @<<
$(IOBJS)
$@
NUL
$(LIBS);
<<

$(IOBJS) : $(IINC)

inforead.exe : $(ROBJS)
	$(LD) $(LDFLAGS) @<<
$(ROBJS)
$@
NUL
$(LIBS);
<<

$(ROBJS) : $(RINC)

pix2gif.exe : $(POBJS)
	$(LD) $(LDFLAGS) @<<
$(POBJS)
$@
NUL
$(LIBS);
<<

$(POBJS) : $(PINC)

txd.exe : $(TOBJS)
	$(LD) $(LDFLAGS) @<<
$(TOBJS)
$@
NUL
$(LIBS);
<<

$(TOBJS) : $(TINC)

