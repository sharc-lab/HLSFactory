/* +++Date last modified: 05-Jul-1997 */
/*
 * MiBench office/stringsearch: the Boyer-Moore-Horspool searchers of the
 * SNIPPETS collection (bmhsrch.c, bmhisrch.c, bmhasrch.c).
 *
 *   BMHSRCH.C   A Boyer-Moore-Horspool string search, Jerry Coffin 1991,
 *               revised by Bob Stout.  10/21/93 rdg  Fixed bug found by
 *               Jeff Dunlop.  Public domain.
 *   BMHISRCH.C  case-insensitive version.
 *   BMHASRCH.C  case-insensitive version that also folds accented chars.
 *
 * HLS adaptation: the file-scope state of bmh*_init() is local to each
 * search, strings are fixed-size buffers addressed by offset (no malloc or
 * realloc, no pointers returned), and toupper/tolower are ASCII helpers.
 */
#include "stringsearch_bmh.h"

#define LARGE 32767 /* flag for last character match */
#define ALPHABET 256

typedef unsigned char uchar;

static const unsigned char lowervec[256] = {                     /* rdg 10/93 */
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
 32,'!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
'0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
'@','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','[','\\',']','^','_',
'`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','{','|','}','~',127,
#ifdef LOWER_ACCENTED_CHARS
'c','u','e','a','a','a','a','c','e','e','e','i','i','i','a','a',
'e',145,146,'o','o','o','u','u','y','o','u',155,156,157,158,159,
'a','i','o','u','n','n',166,167,168,169,170,171,172,173,174,175,
#else
128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
#endif
176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

#define lowerc(c) lowervec[(uchar)(c)]

static int str_length(const char *s, int max)
{
    int n = 0;
    for (int i = 0; i < max; i++) {
        if (s[i] == '\0')
            break;
        n++;
    }
    return n;
}

static int ascii_toupper(int c)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

static int ascii_tolower(int c)
{
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

/*
 * Common scan loop of the three searchers.  skip[] and skip2 come from the
 * respective *_init(); mode selects the character comparison used when
 * verifying a candidate: 0 exact, 1 upper-cased text vs upper-cased
 * pattern, 2 table-folded.  pat is already folded for modes 1 and 2.
 */
static int bmh_scan(const char *string, int stringlen, const uchar *pat, int patlen,
                    const int skip[ALPHABET], int skip2, int mode)
{
    int i, j, s;

    i = patlen - 1 - stringlen;
    if (i >= 0)
        return -1;

    for (;;) {
        while ((i += skip[(uchar)string[stringlen + i]]) < 0)
            ; /* mighty fast inner loop */

        if (i < (LARGE - stringlen))
            return -1;
        i -= LARGE;

        j = patlen - 1;
        s = stringlen + (i - j);
        while (--j >= 0) {
            int c = (uchar)string[s + j];
            int p = pat[j];
            if (mode == 1)
                c = ascii_toupper(c);
            else if (mode == 2)
                c = lowerc(c);
            if (c != p)
                break;
        }
        if (j < 0)
            return s;

        if ((i += skip2) >= 0)
            return -1;
    }
}

/* bmh_init() + bmh_search() */
static int bmh_search_one(const char pattern[SS_PAT_LEN], const char string[SS_TEXT_LEN])
{
    int skip[ALPHABET];
    uchar pat[SS_PAT_LEN];
    int patlen = str_length(pattern, SS_PAT_LEN);
    int stringlen = str_length(string, SS_TEXT_LEN);
    int skip2, lastpatchar;

    for (int i = 0; i < SS_PAT_LEN; i++)
        pat[i] = (uchar)pattern[i];

    for (int i = 0; i < ALPHABET; ++i)
        skip[i] = patlen;
    for (int i = 0; i < SS_PAT_LEN; ++i)
        if (i < patlen)
            skip[pat[i]] = patlen - i - 1;
    lastpatchar = pat[patlen - 1];
    skip[lastpatchar] = LARGE;
    skip2 = patlen; /* Horspool's fixed second shift */
    for (int i = 0; i < SS_PAT_LEN; ++i)
        if (i < patlen - 1 && pat[i] == lastpatchar)
            skip2 = patlen - i - 1;

    return bmh_scan(string, stringlen, pat, patlen, skip, skip2, 0);
}

/* bmhi_init() + bmhi_search() */
static int bmhi_search_one(const char pattern[SS_PAT_LEN], const char string[SS_TEXT_LEN])
{
    int skip[ALPHABET];
    uchar pat[SS_PAT_LEN];
    int patlen = str_length(pattern, SS_PAT_LEN);
    int stringlen = str_length(string, SS_TEXT_LEN);
    int skip2, lastpatchar;

    for (int i = 0; i < SS_PAT_LEN; i++)
        pat[i] = (uchar)ascii_toupper((uchar)pattern[i]); /* uppercase copy */

    for (int i = 0; i < ALPHABET; ++i)
        skip[i] = patlen;
    for (int i = 0; i < SS_PAT_LEN; ++i) {
        if (i < patlen - 1) {
            skip[pat[i]] = patlen - i - 1;
            skip[ascii_tolower(pat[i])] = patlen - i - 1;
        }
    }
    lastpatchar = pat[patlen - 1];
    skip[lastpatchar] = LARGE;
    skip[ascii_tolower(lastpatchar)] = LARGE;
    skip2 = patlen;
    for (int i = 0; i < SS_PAT_LEN; ++i)
        if (i < patlen - 1 && pat[i] == lastpatchar)
            skip2 = patlen - i - 1;

    return bmh_scan(string, stringlen, pat, patlen, skip, skip2, 1);
}

/* bmha_init() + bmha_search() */
static int bmha_search_one(const char pattern[SS_PAT_LEN], const char string[SS_TEXT_LEN])
{
    int skip[ALPHABET];
    uchar pat[SS_PAT_LEN];
    int patlen = str_length(pattern, SS_PAT_LEN);
    int stringlen = str_length(string, SS_TEXT_LEN);
    int skip2;

    /* the original compares lowerc(pat[j]); fold the pattern once up front */
    for (int i = 0; i < SS_PAT_LEN; i++)
        pat[i] = lowerc(pattern[i]);

    for (int i = 0; i < ALPHABET; ++i) {
        int j;
        skip[i] = patlen;
        for (j = SS_PAT_LEN - 1; j >= 0; --j) {
            if (j < patlen && lowerc(i) == pat[j])
                break;
        }
        if (j >= 0)
            skip[i] = patlen - j - 1;
        if (j == patlen - 1)
            skip[i] = LARGE;
    }

    skip2 = patlen;
    for (int i = 0; i < SS_PAT_LEN; ++i)
        if (i < patlen - 1 && pat[i] == pat[patlen - 1])
            skip2 = patlen - i - 1;

    return bmh_scan(string, stringlen, pat, patlen, skip, skip2, 2);
}

void stringsearch_bmh(const char patterns[SS_N][SS_PAT_LEN],
                      const char texts[SS_N][SS_TEXT_LEN], int pos_bmh[SS_N],
                      int pos_bmhi[SS_N], int pos_bmha[SS_N])
{
    for (int i = 0; i < SS_N; i++) {
        pos_bmh[i] = bmh_search_one(patterns[i], texts[i]);
        pos_bmhi[i] = bmhi_search_one(patterns[i], texts[i]);
        pos_bmha[i] = bmha_search_one(patterns[i], texts[i]);
    }
}
