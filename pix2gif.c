/* pix2gif.c V7/3
 *
 * MG1 to GIF format converter.
 *
 * Converts Infocom MG1/EG1 picture files to separate GIF files.
 *
 * usage: pix2gif picture-file
 *
 * Mark Howell 13 September 1992 howell_ma@movies.enet.dec.com
 *
 * History:
 *    Make stack allocated arrays static
 *    Fix lint warnings
 *    Fix 64KB MS-DOS problems
 *    Handle transparent colours properly
 *    Put transparency information into GIF (now version 89a)
 */

#include "pix2gif.h"

#ifdef __MSDOS__
#include <alloc.h>
#define malloc(n) farmalloc(n)
#define calloc(n,s) farcalloc(n,s)
#define free(p) farfree(p)
#endif

static short mask[16] = {
    0x0000, 0x0001, 0x0003, 0x0007,
    0x000f, 0x001f, 0x003f, 0x007f,
    0x00ff, 0x01ff, 0x03ff, 0x07ff,
    0x0fff, 0x1fff, 0x3fff, 0x7fff
};

static unsigned char ega_colourmap[16][3] = {
	{   0,  0,  0 },
	{   0,  0,170 },
	{   0,170,  0 },
	{   0,170,170 },
	{ 170,  0,  0 },
	{ 170,  0,170 },
	{ 170,170,  0 },
	{ 170,170,170 },
	{  85, 85, 85 },
	{  85, 85,255 },
	{  85,255, 85 },
	{  85,255,255 },
	{ 255, 85, 85 },
	{ 255, 85,255 },
	{ 255,255, 85 },
	{ 255,255,255 }
};

#if defined(AMIGA) && defined(_DCC)
__far
#endif
static nlist_t *hash_table[HASH_SIZE];
static unsigned char colourmap[16][3];
static unsigned char code_buffer[CODE_TABLE_SIZE];
static char file_name[FILENAME_MAX + 1];
static short code_table[CODE_TABLE_SIZE][2];
static unsigned char buffer[CODE_TABLE_SIZE];

#if defined(__STDC__)
static void process_image (FILE *, pdirectory_t *);
static void decompress_image (FILE *, image_t *);
static short read_code (FILE *, compress_t *);
static void write_file (int, image_t *);
static void write_screen (FILE *, image_t *);
static void write_graphic_control (FILE *, image_t *);
static void write_image (FILE *, image_t *);
static void compress_image (FILE *, image_t *);
static void write_code (FILE *, short, compress_t *);
static void insert_code (short, short, short);
static short lookup (short, short);
static void clear_table (void);
static void delete_table (void);
static unsigned char read_byte (FILE *);
static void write_byte (FILE *, int);
static unsigned short read_word (FILE *);
static void write_word (FILE *, unsigned short);
static void read_bytes (FILE *, int, void *);
static void write_bytes (FILE *, int, const void *);
#else
static void process_image ();
static void decompress_image ();
static short read_code ();
static void write_file ();
static void write_screen ();
static void write_graphic_control ();
static void write_image ();
static void compress_image ();
static void write_code ();
static void insert_code ();
static short lookup ();
static void clear_table ();
static void delete_table ();
static unsigned char read_byte ();
static void write_byte ();
static unsigned short read_word ();
static void write_word ();
static void read_bytes ();
static void write_bytes ();
#endif

#if defined(__STDC__)
int main (int argc, char *argv[])
#else
int main (argc, argv)
int argc;
char *argv[];
#endif
{
    int i;
    FILE *fp;
    header_t header;
    pdirectory_t *directory;

    if (argc != 2) {
	(void) fprintf (stderr, "usage: %s picture-file\n\n", argv[0]);
	(void) fprintf (stderr, "PIX2GIF version 7/2 - convert Infocom MG1/EG1 files to GIF. By Mark Howell\n");
	(void) fprintf (stderr, "Works with V6 Infocom games.\n");
	exit (EXIT_FAILURE);
    }

    if ((fp = fopen (argv[1], "rb")) == NULL) {
	perror ("fopen");
	exit (EXIT_FAILURE);
    }

    header.part = read_byte (fp);
    header.flags = read_byte (fp);
    header.unknown1 = read_word (fp);
    header.images = read_word (fp);
    header.unknown2 = read_word (fp);
    header.dir_size = read_byte (fp);
    header.unknown3 = read_byte (fp);
    header.checksum = read_word (fp);
    header.unknown4 = read_word (fp);
    header.version = read_word (fp);

    (void) printf ("Total number of images is %d.\n", (int) header.images);

    if ((directory = (pdirectory_t *) calloc ((size_t) header.images, sizeof (pdirectory_t))) == NULL) {
	(void) fprintf (stderr, "Insufficient memory\n");
	exit (EXIT_FAILURE);
    }

    for (i = 0; (unsigned int) i < (unsigned int) header.images; i++) {
	directory[i].image_number = read_word (fp);
	directory[i].image_width = read_word (fp);
	directory[i].image_height = read_word (fp);
	directory[i].image_flags = read_word (fp);
	directory[i].image_data_addr = (unsigned long) read_byte (fp) << 16;
	directory[i].image_data_addr += (unsigned long) read_byte (fp) << 8;
	directory[i].image_data_addr += (unsigned long) read_byte (fp);
	if ((unsigned int) header.dir_size == 14) {
	    directory[i].image_cm_addr = (unsigned long) read_byte (fp) << 16;
	    directory[i].image_cm_addr += (unsigned long) read_byte (fp) << 8;
	    directory[i].image_cm_addr += (unsigned long) read_byte (fp);
	} else {
	    directory[i].image_cm_addr = 0;
	    (void) read_byte (fp);
	}
    }
    for (i = 0; (unsigned int) i < (unsigned int) header.images; i++)
	process_image (fp, &directory[i]);
    free (directory);
    (void) fclose (fp);

    exit (EXIT_SUCCESS);

    return (0);

}/* main */

#if defined(__STDC__)
static void process_image (FILE *fp, pdirectory_t *directory)
#else
static void process_image (fp, directory)
FILE *fp;
pdirectory_t *directory;
#endif
{
    int colours = 16, i;
    image_t image;

    for (i = 0; i < 16; i++) {
	colourmap[i][RED] = ega_colourmap[i][RED];
	colourmap[i][GREEN] = ega_colourmap[i][GREEN];
	colourmap[i][BLUE] = ega_colourmap[i][BLUE];
    }

    if (directory->image_cm_addr) {
	if (fseek (fp, directory->image_cm_addr, SEEK_SET) != 0) {
	    perror ("fseek");
	    exit (EXIT_FAILURE);
	}
	colours = read_byte (fp);

	/* Fix for some buggy _Arthur_ pictures. */
	if (colours > 14)
	    colours = 14;
	read_bytes (fp, colours * 3, (void *) &colourmap[2][RED]);
	colours += 2;
    }

    if (directory->image_flags & 1) {
	colourmap[directory->image_flags >> 12][0] = 0;
	colourmap[directory->image_flags >> 12][1] = 0;
	colourmap[directory->image_flags >> 12][2] = 0;
    }

    (void) printf ("pic %03d   size %3d x %3d   %2d colours   colour map ",
	    (int) directory->image_number,
	    (int) directory->image_width, (int) directory->image_height,
	    (int) colours);

    if (directory->image_cm_addr != 0)
	(void) printf ("$%05lx", (long) directory->image_cm_addr);
    else
	(void) printf ("------");

    if (directory->image_flags & 1) {
	image.transflag = 1;
	image.transpixel = (unsigned short) directory->image_flags >> 12;
	(void) printf ("   transparent is %u\n", image.transpixel);
    }
    else {
	image.transpixel = 0;
	image.transflag = 0;
	(void) printf ("\n");
    }

    if (directory->image_data_addr == 0)
	return;

    image.width = directory->image_width;
    image.height = directory->image_height;
    image.colours = colours;
    image.pixels = 0;
    if ((image.image = (unsigned char *) calloc ((size_t) directory->image_width, (size_t) directory->image_height)) == NULL) {
	(void) fprintf (stderr, "Insufficient memory\n");
	exit (EXIT_FAILURE);
    }
    image.colourmap = colourmap;

    if (fseek (fp, directory->image_data_addr, SEEK_SET) != 0) {
	perror ("fseek");
	exit (EXIT_FAILURE);
    }
    decompress_image (fp, &image);

    write_file ((int) directory->image_number, &image);

    free (image.image);

}/* process image */

#if defined(__STDC__)
static void decompress_image (FILE *fp, image_t *image)
#else
static void decompress_image (fp, image)
FILE *fp;
image_t *image;
#endif
{
    int i;
    short code, old = 0, first, clear_code;
    compress_t comp;

    clear_code = 1 << CODE_SIZE;
    comp.next_code = clear_code + 2;
    comp.slen = 0;
    comp.sptr = 0;
    comp.tlen = CODE_SIZE + 1;
    comp.tptr = 0;

    for (i = 0; i < CODE_TABLE_SIZE; i++) {
	code_table[i][PREFIX] = CODE_TABLE_SIZE;
	code_table[i][PIXEL] = i;
    }

    for (;;) {
	if ((code = read_code (fp, &comp)) == (clear_code + 1))
	    return;
	if (code == clear_code) {
	    comp.tlen = CODE_SIZE + 1;
	    comp.next_code = clear_code + 2;
	    code = read_code (fp, &comp);
	} else {
	    first = (code == comp.next_code) ? old : code;
	    while (code_table[first][PREFIX] != CODE_TABLE_SIZE)
                first = code_table[first][PREFIX];
            code_table[comp.next_code][PREFIX] = old;
            code_table[comp.next_code++][PIXEL] = code_table[first][PIXEL];
        }
        old = code;
        i = 0;
        do
            buffer[i++] = (unsigned char) code_table[code][PIXEL];
        while ((code = code_table[code][PREFIX]) != CODE_TABLE_SIZE);
        do 
            image->image[image->pixels++] = buffer[--i]; 
        while (i > 0);
    }

}/* decompress_image */

#if defined(__STDC__)
static short read_code (FILE *fp, compress_t *comp)
#else
static short read_code (fp, comp)
FILE *fp;
compress_t *comp;
#endif
{
    short code, bsize, tlen, tptr;

    code = 0;
    tlen = comp->tlen;
    tptr = 0;

    while (tlen) {
        if (comp->slen == 0) {
            if ((comp->slen = fread (code_buffer, 1, MAX_BIT, fp)) == 0) {
                perror ("fread");
                exit (EXIT_FAILURE);
            }
            comp->slen *= 8;
            comp->sptr = 0;
        }
	bsize = ((comp->sptr + 8) & ~7) - comp->sptr;
        bsize = (tlen > bsize) ? bsize : tlen;
        code |= (((unsigned int) code_buffer[comp->sptr >> 3] >> (comp->sptr & 7)) & mask[bsize]) << tptr;

        tlen -= bsize;
        tptr += bsize;
        comp->slen -= bsize;
        comp->sptr += bsize;
    }
    if ((comp->next_code == mask[comp->tlen]) && (comp->tlen < 12))
        comp->tlen++;

    return (code);

}/* read_code */

#if defined(__STDC__)
static void write_file (int image_number, image_t *image)
#else
static void write_file (image_number, image)
int image_number;
image_t *image;
#endif
{
    FILE *fp;

    (void) sprintf (file_name, "pix%03d.gif", (int) image_number);

    if ((fp = fopen (file_name, "wb")) == NULL) {
        perror ("fopen");
        exit (EXIT_FAILURE);
    }

    write_bytes (fp, sig_k_bln, (const void *) CURRENT_VERSION);
    write_screen (fp, image);
    if (image->transflag) /* save 8 bytes if possible */
	write_graphic_control(fp, image);
    write_image (fp, image);
    compress_image (fp, image);
    write_byte (fp, ';');

    (void) fclose (fp);

}/* write_file */

#if defined(__STDC__)
static void write_screen (FILE *fp, image_t *image)
#else
static void write_screen (fp, image)
FILE *fp;
image_t *image;
#endif
{
    int i;

    for (i = 1; (image->colours - 1) >> i; i++)
        ;

    write_word (fp, (unsigned short) image->width);
    write_word (fp, (unsigned short) image->height);
    write_byte (fp, ((i - 1) & 7) | (((i - 1) & 7) << 4) | (1 << 7));
    write_byte (fp, 0);
    write_byte (fp, 0);

    write_bytes (fp, (1 << i) * 3, (const void *) image->colourmap);

}/* write_screen */

#if defined(__STDC__)
static void write_graphic_control (FILE *fp, image_t *image)
#else
static void write_graphic_control (fp, image)
FILE *fp;
image_t *image;
#endif
{
    write_byte(fp, '!');               /* Extension Introducer        */
    write_byte(fp, 0xF9);              /* Graphic Control Label       */
    write_byte(fp, 4);                 /* # bytes in block            */
    write_byte(fp, image->transflag);   /* bits 7-5: reserved          */
                                       /* bits 4-2: Disposal Method   */
                                       /* bit 1   : User Input Flag   */
                                       /* bit 0   : Transparency Flag */
    write_byte(fp, 0);                 /* Delay Time LSB              */
    write_byte(fp, 0);                 /* Delay Time MSB              */
    write_byte(fp, image->transpixel);  /* Transparent color index     */
    write_byte(fp, 0);                 /* Block terminator            */
}

#if defined(__STDC__)
static void write_image (FILE *fp, image_t *image)
#else
static void write_image (fp, image)
FILE *fp;
image_t *image;
#endif
{

    write_byte (fp, ',');
    write_word (fp, (unsigned short) 0);
    write_word (fp, (unsigned short) 0);
    write_word (fp, (unsigned short) image->width);
    write_word (fp, (unsigned short) image->height);
    write_byte (fp, 0);

}/* write_image */

#if defined(__STDC__)
static void compress_image (FILE *fp, image_t *image)
#else
static void compress_image (fp, image)
FILE *fp;
image_t *image;
#endif
{
    int init_comp_size;
    long index;
    short code, clear_code, prefix, pixel;
    compress_t comp;

    clear_table ();

    for (init_comp_size = 1; (image->colours - 1) >> init_comp_size; init_comp_size++)
        ;

    clear_code = 1 << init_comp_size++;
    code_buffer[0] = 255;
    index = 0;

    comp.next_code = clear_code + 2;
    comp.slen = init_comp_size;
    comp.sptr = 0;
    comp.tlen = 255 * 8;
    comp.tptr = 8;

    write_byte (fp, init_comp_size - 1);
    write_code (fp, clear_code, &comp);
    prefix = image->image[index++];
    while (index < image->pixels) {
        pixel = image->image[index++];
        code = lookup (prefix, pixel);
        if (code) {
            prefix = code;
        } else {
            write_code (fp, prefix, &comp);
            if (comp.next_code == 4096) {
                delete_table ();
                comp.next_code = clear_code + 2;
                write_code (fp, clear_code, &comp);
                comp.slen = init_comp_size;
            } else
                insert_code (prefix, pixel, comp.next_code++);
            prefix = pixel;
        }
    }
    write_code (fp, prefix, &comp);
    write_code (fp, (short) (clear_code + 1), &comp);
    if (comp.tptr) {
        code_buffer[0] = (unsigned char) ((comp.tptr + 7) >> 3);
        write_bytes (fp, (int) code_buffer[0] + 1, (const void *) code_buffer);
    }
    write_byte (fp, 0);
    delete_table ();

}/* compress_image */

#if defined(__STDC__)
static void write_code (FILE *fp, short code, compress_t *comp)
#else
static void write_code (fp, code, comp)
FILE *fp;
short code;
compress_t *comp;
#endif
{
    short bsize, slen, sptr;

    slen = comp->slen;
    sptr = 0;

    while (slen) {
        if (comp->tlen == 0) {
            write_bytes (fp, 256, (const void *) code_buffer);
            comp->tlen = 255 * 8;
            comp->tptr = 8;
        }
        bsize = ((comp->tptr + 8) & ~7) - comp->tptr;
        bsize = (slen > bsize) ? bsize : slen;

        code_buffer[comp->tptr >> 3] = (unsigned char) ((unsigned int) code_buffer[comp->tptr >> 3] & mask[comp->tptr & 7]);
        code_buffer[comp->tptr >> 3] = (unsigned char) ((unsigned int) code_buffer[comp->tptr >> 3] | ((code >> sptr) & mask[bsize]) << (comp->tptr & 7));

        slen -= bsize;
        sptr += bsize;
        comp->tlen -= bsize;
        comp->tptr += bsize;
    }
    if ((comp->next_code == (mask[comp->slen] + 1)) && (comp->slen < 12))
        comp->slen++;

}/* write_code */

#if defined(__STDC__)
static void insert_code (short prefix, short pixel, short code)
#else
static void insert_code (prefix, pixel, code)
short prefix;
short pixel;
short code;
#endif
{
    short hashval;
    nlist_t *np;

    if ((np = (nlist_t *) malloc (sizeof (*np))) == NULL) {
        (void) fprintf (stderr, "Insufficient memory\n");
        exit (EXIT_FAILURE);
    }
    hashval = (short) hashfunc (prefix, pixel);
    np->next = (nlist_t *) hash_table[hashval];
    hash_table[hashval] = np;
    np->prefix = prefix;
    np->pixel = pixel;
    np->code = code;

}/* insert_code */

#if defined(__STDC__)
static short lookup (short prefix, short pixel)
#else
static short lookup (prefix, pixel)
short prefix;
short pixel;
#endif
{
    short hashval;
    nlist_t *np;

    hashval = (short) hashfunc (prefix, pixel);
    for (np = hash_table[hashval]; np != NULL; np = (nlist_t *) np->next)
        if ((np->prefix == prefix) && (np->pixel == pixel))
            return (np->code);

    return (0);

}/* lookup */

#if defined(__STDC__)
static void clear_table (void)
#else
static void clear_table ()
#endif
{
    int i;

    for (i = 0; i < HASH_SIZE; i++)
        hash_table[i] = NULL;

}/* clear_table */

#if defined(__STDC__)
static void delete_table (void)
#else
static void delete_table ()
#endif
{
    int i;
    nlist_t *np, *tp;

    for (i = 0; i < HASH_SIZE; i++) {
        for (np = hash_table[i]; np != NULL; np = tp) {
            tp = (nlist_t *) np->next;
            free (np);
        }
        hash_table[i] = NULL;
    }

}/* delete_table */

#if defined(__STDC__)
static unsigned char read_byte (FILE *fp)
#else
static unsigned char read_byte (fp)
FILE *fp;
#endif
{
    int c;

    if ((c = fgetc (fp)) == EOF) {
        perror ("fgetc");
        exit (EXIT_FAILURE);
    }

    return ((unsigned char) c);

}/* read_byte */

#if defined(__STDC__)
static void write_byte (FILE *fp, int c)
#else
static void write_byte (fp, c)
FILE *fp;
int c;
#endif
{

    if (fputc (c, fp) == EOF) {
        perror ("fputc");
        exit (EXIT_FAILURE);
    }

}/* write_byte */

#if defined(__STDC__)
static unsigned short read_word (FILE *fp)
#else
static unsigned short read_word (fp)
FILE *fp;
#endif
{
    unsigned int w;

    w = (unsigned int) read_byte (fp);
    w += (unsigned int) read_byte (fp) << 8;

    return (w);

}/* read_word */

#if defined(__STDC__)
static void write_word (FILE *fp, unsigned short w)
#else
static void write_word (fp, w)
FILE *fp;
unsigned short w;
#endif
{

    write_byte (fp, (int) w & 255);
    write_byte (fp, (int) w >> 8);

}/* write_word */

#if defined(__STDC__)
static void read_bytes (FILE *fp, int size, void *s)
#else
static void read_bytes (fp, size, s)
FILE *fp;
int size;
void *s;
#endif
{

    if (fread (s, (size_t) size, 1, fp) != 1) {
        perror ("fread");
        exit (EXIT_FAILURE);
    }

}/* read_bytes */

#if defined(__STDC__)
static void write_bytes (FILE *fp, int size, const void *s)
#else
static void write_bytes (fp, size, s)
FILE *fp;
int size;
const void *s;
#endif
{

    if (fwrite (s, (size_t) size, 1, fp) != 1) {
        perror ("fwrite");
        exit (EXIT_FAILURE);
    }

}/* write_bytes */
