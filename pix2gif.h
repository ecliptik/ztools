/* pix2gif.h
 *
 * pix2gif.c header file.
 *
 * Mark Howell 26 August 1992 howell_ma@movies.enet.dec.com
 *
 */

#include <stdio.h>
#include <stdlib.h>

#if !defined(FILENAME_MAX)
#define FILENAME_MAX 255
#endif

#if !defined(EXIT_SUCCESS)
#define EXIT_SUCCESS 0
#endif

#if !defined(EXIT_FAILURE)
#define EXIT_FAILURE 1
#endif

#if !defined(SEEK_SET)
#define SEEK_SET 0
#endif

#ifdef __DJGPP__
#undef __MSDOS__
#endif

#if defined(__MSDOS__)
#define __USE_FARPTRS__
#endif

/*
  7/3 -- I'm guessing "unix" was used for non-ANSI C dialects.
         So I'm replacing those tests with #ifndef __STDC__
*/

#ifndef __STDC__
#define const
#endif

#define MAX_BIT 512 /* Must be less than or equal to CODE_TABLE_SIZE */
#define CODE_SIZE 8
#define CODE_TABLE_SIZE 4096
#define PREFIX 0
#define PIXEL 1
#define RED 0
#define GREEN 1
#define BLUE 2
#define CURRENT_VERSION "GIF89a"
#define HASH_SIZE 8192
#define hashfunc(a, b) ((long) ((long) (a) + (long) (b)) % HASH_SIZE)

typedef struct header_s {
    unsigned char part;
    unsigned char flags;
    unsigned short unknown1;
    unsigned short images;
    unsigned short unknown2;
    unsigned char dir_size;
    unsigned char unknown3;
    unsigned short checksum;
    unsigned short unknown4;
    unsigned short version;
} header_t;

typedef struct pdirectory_s {
    short image_number;
    short image_width;
    short image_height;
    short image_flags;
    long image_data_addr;
    long image_cm_addr;
} pdirectory_t;

typedef struct image_s {
    short width;
    short height;
    short colours;
    long pixels;
#ifdef __USE_FARPTRS__
    unsigned char huge *image;
#else
    unsigned char *image;
#endif
    unsigned char (*colourmap)[3];
    unsigned short transflag;
    unsigned short transpixel;
} image_t;

typedef struct compress_s {
    short next_code;
    short slen;
    short sptr;
    short tlen;
    short tptr;
} compress_t;

typedef struct nlist_s {
    struct nlist_s *next;
    short prefix;
    short pixel;
    short code;
} nlist_t;

#define sig_k_bln 6

struct sigdef {
    char sig_t_signature[6];            /* gif signature                    */
};

#define sd_m_pixel 7
#define sd_m_zero 8
#define sd_m_cr 112
#define sd_m_gcm 128
#define sd_k_bln 7

struct sddef {
    unsigned short sd_w_width;          /* width of screen                  */
    unsigned short sd_w_height;         /* height of screen                 */
    struct {                            /* flags                            */
	unsigned sd_v_pixel : 3;        /* # bits/pixel in image            */
	unsigned sd_v_zero : 1;         /* reserved                         */
	unsigned sd_v_cr : 3;           /* # bits of colour resolution      */
	unsigned sd_v_gcm : 1;          /* global colourmap present         */
    } sd_r_flags;
    unsigned char sd_b_background;      /* background colour                */
    unsigned char sd_b_mbz;             /* null terminating byte            */
};

#define id_m_pixel 7
#define id_m_zero 56
#define id_m_interlace 64
#define id_m_lcm 128
#define id_k_bln 9

struct iddef {
    unsigned short id_w_left;           /* image left                       */
    unsigned short id_w_top;            /* image right                      */
    unsigned short id_w_width;          /* image width                      */
    unsigned short id_w_height;         /* image height                     */
    struct {                            /* flags                            */
	unsigned id_v_pixel : 3;        /* # bits/pixel in image            */
	unsigned id_v_zero : 3;         /* reserved                         */
	unsigned id_v_interlace : 1;    /* interlaced image                 */
	unsigned id_v_lcm : 1;          /* local colourmap present          */
    } id_r_flags;
};

#define ext_k_bln 1

struct extdef {
    unsigned char ext_b_fcode;          /* function code                    */
};
