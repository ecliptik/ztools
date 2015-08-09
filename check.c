/*
 * check.c V7/3
 *
 * Perform the same action as $verify on Infocom story files. Also, optionally
 * trim the story file to its exact byte length.
 *
 * Usage: check story-file-name [new-story-file-name]
 *
 * Mark Howell 7 August 1993 howell_ma@movies.enet.dec.com
 *
 * History:
 *	  Fix lint warnings
 *	  Add support for V7 and V8 games
 */

#include <stdio.h>
#include <stdlib.h>

#define H_VERSION 0
#define H_FILE_SIZE 26
#define H_CHECKSUM 28
#define H_SIZE 64

#define PAGE_SIZE 512

#if !defined(EXIT_FAILURE)
#define EXIT_FAILURE 1
#endif

#define get_byte(offset) ((unsigned char) header[offset])
#define get_word(offset) ((unsigned short) (((unsigned short) header[offset] << 8) + (unsigned short) header[offset + 1]))

#ifdef __STDC__
int main (int argc, char *argv[])
#else
int main (argc, argv)
int argc;
char *argv[];
#endif
{
	FILE *ifp, *ofp = NULL;
	unsigned int i, size, h_version;
	unsigned long h_file_size;
	unsigned long n;
	unsigned char header[H_SIZE], buffer[PAGE_SIZE];
	unsigned short h_checksum, checksum;

	if (argc < 2) {
		(void) fprintf (stderr, "usage: %s story-file\n\n", argv[0]);
		(void) fprintf (stderr, "CHECK version 7/2 - verify Infocom story files. By Mark Howell\n");
		(void) fprintf (stderr, "Works with V1 to V8 Infocom games.\n");
		exit (EXIT_FAILURE);
	}

	if ((ifp = fopen (argv[1], "rb")) == NULL) {
		perror ("fopen: input file");
		exit (EXIT_FAILURE);
	}

	if (fread (header, H_SIZE, 1, ifp) != 1) {
		perror ("fread: story header");
		exit (EXIT_FAILURE);
	}

	h_version = (unsigned int) get_byte (H_VERSION);
	h_checksum = (unsigned short) get_word (H_CHECKSUM);
	h_file_size = (unsigned long) get_word (H_FILE_SIZE);

	if (h_file_size == 0) {
		(void) fprintf (stdout, "Story has no file size in the header\nEnter file size:\n");
		(void) scanf ("%ld", &h_file_size);
		if (h_version == 1 || h_version == 2 || h_version == 3)
			h_file_size /= 2;
		else if (h_version == 4 || h_version == 5)
			h_file_size /= 4;
		else
			h_file_size /= 8;
	}

	if (argc > 2 && (ofp = fopen (argv[2], "wb")) == NULL) {
		perror ("fopen: output file");
		exit (EXIT_FAILURE);
	}

	if (ofp != NULL && fwrite (header, H_SIZE, 1, ofp) != 1) {
		perror ("fwrite: story header");
		exit (EXIT_FAILURE);
	}

	if (h_version == 1 || h_version == 2 || h_version == 3)
		h_file_size *= 2;
	else if (h_version == 4 || h_version == 5)
		h_file_size *= 4;
	else
		h_file_size *= 8;

	h_file_size -= H_SIZE;
	size = PAGE_SIZE - H_SIZE;
	checksum = 0;
	while (h_file_size) {
		if ((n = fread (buffer, 1, (size_t) size, ifp)) != size) {
			perror ("fread: story data");
			fprintf(stderr, "Got %d bytes expected %d\n", n, size);
			exit (EXIT_FAILURE);
		}
		if (ofp != NULL && fwrite (buffer, (size_t) size, 1, ofp) != 1) {
			perror ("fwrite: story data");
			exit (EXIT_FAILURE);
		}
		for (i = 0; i < size; i++)
			checksum += (unsigned short) buffer[i];
		h_file_size -= size;
		size = (h_file_size >= PAGE_SIZE) ? PAGE_SIZE : (unsigned int) h_file_size;
	}

	if (checksum == h_checksum)
		(void) printf ("Checksum OK\n");
	else
		(void) printf ("Checksum bad! actual checksum = %04x, required checksum = %04x\n",
				(unsigned int) checksum, (unsigned int) h_checksum);

	if (fclose (ifp) != 0) {
		perror ("fclose: input file");
		exit (EXIT_FAILURE);
	}

	if (ofp != NULL && fclose (ofp) != 0) {
		perror ("fclose: output file");
		exit (EXIT_FAILURE);
	}

	return (0);

}/* check */
