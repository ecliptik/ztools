# Ztools

An essential set of Z-machine tools for interpreter authors, experienced Inform programmers, and Z-code hackers, originally written by Mark Howell, and now maintained by Matthew Russotto.

The toolkit consists of TXD, a Z-code disassembler, and Infodump, a story file table parser, together with utilities for verifying and stripping story files, converting Infocom's IBM MCGA graphics files to GIF, and extracting story files from bootable DOS discs.

Originally from
- http://inform-fiction.org/zmachine/ztools.html

# Original readme.1st

Ztools 7/3.1 - 11/7/1998
------------------------

Author:

    Mark Howell (howell_ma@movies.enet.dec.com)

    Currently maintained by Matthew T. Russotto
    (russotto@pond.com)

Contents:

    readme.1st - this file
    Makefile - Unix makefile
    amiga.mak - Amiga Dice C makefile
    aztec.mak - Amiga Aztec C makefile
    mpwcw.mak - Macintosh Code Warrior C makefile
    msc.mak - Microsoft Quick C makefile
    vms.mak - VMS VAX C makefile
    Makefile.djpp - DJGPP makefile
    beos.mak - BeOS PowerPC and x86 Makefile
    check.c - Sources
    getopt.c
    infinfo.c
    infodump.c
    inforead.c
    pix2gif.c
    showdict.c
    showhead.c
    showobj.c
    showverb.c
    txd.c
    txio.c
    pix2gif.h - Header files
    tx.h
    check.1  -   nroff-format man pages.  
    infodump.1   Thanks to Neil Bowers (neilb@cre.canon.co.uk) 
    inforead.1   for providing these.
    pix2gif.1 
    txd.1

Building:

    Unix: make

    BeOS: Makefile is beos.mak

    DOS: nmake /F msc.mak

    DOS (using DJGPP): Makefile is Makefile.djgpp

    VMS: mms/desc=vms.mak

    Amiga: makefile is amiga.mak for Dice C or aztec.mak for Aztec C

    Macintosh (programmer's workshop): makefile is mpwcw.mak

    Program dependencies:

	check : check.c

	infodump : infodump.c txio.c getopt.c showdict.c showhead.c
		   showobj.c showverb.c infinfo.c symbols.c tx.h

	inforead : inforead.c

	pix2gif : pix2gif.c pix2gif.h

	txd : txd.c txio.c showverb.c infinfo.c symbols.c tx.h

    inforead.c requires Microsoft C, Borland C, or DJGPP as it's only for a PC.

Bugs:

    Please report bugs and portability bugs to the current maintainer.

Tools:

    check - checks story file integrity, same as $verify command. Works on all
	    types except for V1, V2 and early V3 games which have no checksum
	    in the header. Optionally outputs a new story file of the correct
	    length.

	usage: check story-file-name [new-story-file-name]

    infodump - dumps header, object list, grammar and dictionary of a given
	       story file. Works on all types. Grammar doesn't work for V6
	       games yet.

	usage: infodump [options...] story-file-name

    inforead - converts IBM bootable disks to story files. Only for IBM PCs,
	       requires Microsoft C, Borland C, or DJGPP. Optional
               parameters are the start track (defaults to 6) and the
               drive number (0 for A: or 1 for B:).

	usage: inforead story-file-name [start-track [drive#]]

    pix2gif - converts IBM MG1/EG1/EG2 picture files from V6 games to
	      individual GIF files, viewable on most platforms.

	usage: pix2gif picture-file-name

    txd - disassembles story files to Z-code assembly language, plus text
	  strings. Works on all games from V1 to V8. Also supports Inform
	  assembler syntax.

	usage: txd [options...] story-file-name

Notes for 7/3:
	
	The major change for ZTools 7/3 is the addition of Inform symbols for
	newer Inform games.  Both infodump and txd support a "-s" option which
	will enable the use of these symbols.  I am not particularly happy with
	the output format of the symbol-enabled dumps, so this may
        very well change in the future.  There is also some provision
        for a "user" symbol table intended for helping decipher the
        workings of games without symbols, but this should be
        considered unfinished and undocumented in this release. 
	
	Having said that, here's a few examples of lines in a user symbol table
	(not all of which work in this release, and all of which may change in
	the future)
	
	Global	0	location
	Routine 0x5880	sparse
	Attribute	0x1B	vehicle
	Property	13	treasure_value	word
	local	3	curaction	sparse
	
	Please note that this release changed the makefiles, and I
        don't have all the systems there exist makefiles for, so don't
        be too surprised if some Makefile changes are necessary to
        make ZTools compile. 
	
