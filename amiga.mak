
# Dice C makefile for ZTools

SRCS = check.c infodump.c showhead.c showdict.c showobj.c showverb.c \
       getopt.c txio.c pix2gif.c txd.c infinfo.c symbols.c
OBJS = $(SRCS:"*.c":"*.o")

COBJS = check.o
IOBJS = infodump.o showhead.o showdict.o showobj.o showverb.o txio.o \
	getopt.o infinfo.o symbols.o
POBJS = pix2gif.o
TOBJS = txd.o txio.o showverb.o getopt.o infinfo.o symbols.o showobj.o

all : check infodump pix2gif txd

$(OBJS) : $(SRCS)
    dcc -c %(right) -o %(left)

check : $(COBJS)
    dcc %(right) -o %(left)

infodump : $(IOBJS)
    dcc %(right) -o %(left)

pix2gif : $(POBJS)
    dcc %(right) -o %(left)

txd : $(TOBJS)
    dcc %(right) -o %(left)


