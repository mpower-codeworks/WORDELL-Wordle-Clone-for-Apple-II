/*

      / / / / / / / / / /
    / / makepack.c  / /
  / / / / / / / / / /

   word list builder
     for wordell

         2026
        mpower

compile with tcc or msvc cl

you can use the windows
batch files to build

BUILD_FILE_MAKER_TCC.bat
-- or --
BUILD_FILE_MAKER_CL.bat

or copy-paste one of the following
  
[TCC - 6kb]
tcc makepack.c

-- or --

[MSVC - 12 kb]
cl /nologo ^
/O1 /Os /MD /GS- ^
MAKEPACK.C ^
/FeMAKEPACK.EXE ^
/link /OPT:REF ^
/OPT:ICF ^
/INCREMENTAL:NO

Purpose
-------
reads:
    ALL.TXT
    SOLUTION.TXT

writes:
    ALL5.BIN
    SOL5.BIN
    WORDDATA.H

Files needed for the Apple II disk
----------------------------------
ALL5.BIN
SOL5.BIN
WORDELL.SYSTEM

Packed files read much faster for Apple II
Every word is exactly five bytes. No CR/LF
bytes are stored in the packed files.

*/

#include <stdio.h>  /* FILE, fopen, fclose, fgets, fwrite,
                       fprintf, printf, puts, NULL */
#include <stdlib.h> /* exit */
#include <string.h> /* strcpy */

/* ===========
** definitions
** ===========
*/
#define WORD_LENGTH 5
#define LETTERS     26
#define MAX_WORDS   20000

/* full word lists are held here while the
** packed  files and header  are generated
*/
static char allWords[MAX_WORDS][WORD_LENGTH + 1];
static char solWords[MAX_WORDS][WORD_LENGTH + 1];

/* number of valid words loaded
** from  each source text  file
*/
static unsigned int allCount;
static unsigned int solCount;

/* =======
** isWord5
** =======
*/
static int isWord5(const char *s) {
    int i;

    /* verify that the first five bytes
    ** are simple lower-case letters
    */
    for (i = 0; i < WORD_LENGTH; ++i) {
        if (s[i] < 'a' || s[i] > 'z') {
            return 0;
        }
    }

    /* accept only exact five-letter words
    */
    return s[WORD_LENGTH] == 0;
}

/* =========
** stripLine
** =========
*/
static void stripLine(char *s) {
    int i;

    i = 0;
    while (s[i]) {
        
        /* remove either DOS or UNIX line endings
        */
        if (s[i] == '\r' || s[i] == '\n') {
            s[i] = 0;
            return;
        }

        /* convert source words to lower case
        */
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] |= 0x20;
        }
        ++i;
    }
}

/* ========
** loadList
** ========
*/
static unsigned int loadList (
    const char *name,
    char words[][WORD_LENGTH + 1]) {
    
    /* file handle for source word list
    */
    FILE *fp;

    char line[80];
    unsigned int count;

    /* open source word list
    */
    fp = fopen(name, "r");
    if (fp == NULL) {
        printf("Cannot open %s\n", name);
        exit(1);
    }

    /* read one text line  at a time and
    ** keep only valid five-letter words
    */
    count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        stripLine(line);
        if (isWord5(line)) {
            if (count >= MAX_WORDS) {
                puts("Too many words.");
                exit(1);
            }

            /* save the clean word in memory
            */
            strcpy(words[count], line);
            ++count;
        }
    }

    /* close file
    */
    fclose(fp);
    return count;
}

/* ===========
** writePacked
** ===========
*/
static void writePacked (
    const char *name,
    char words[][WORD_LENGTH + 1],
    unsigned int count) {
    
    /* file handle for packed output
    */
    FILE *fp;

    /* looper
    */
    unsigned int i;

    /* create packed output file
    */
    fp = fopen(name, "wb");
    if (fp == NULL) {
        printf("Cannot create %s\n", name);
        exit(1);
    }

    /* write exactly five bytes per
    ** word, no CR/LF no terminator
    */
    for (i = 0; i < count; ++i) {
        fwrite(words[i], 1, WORD_LENGTH, fp);
    }

    /* close file
    */
    fclose(fp);
}

/* ===========
** writeHeader
** ===========
*/
static void writeHeader(void) {
    
    /* file handle for generated header
    */
    FILE *fp;
    
    unsigned long offsets[LETTERS];
    unsigned int  counts [LETTERS];
    unsigned int  i;
    unsigned int  c;
    unsigned int  maxGroup;

    /* clear letter index tables
    */
    for (i = 0; i < LETTERS; ++i) {
        offsets[i] = 0;
        counts[i] = 0;
    }

    /* build one offset and count for each
    ** starting letter in ALL5.BIN
    */
    for (i = 0; i < allCount; ++i) {
        c = allWords[i][0] - 'a';
        if (c < LETTERS) {
            if (counts[c] == 0) {
                offsets[c] = ((unsigned long)i) * WORD_LENGTH;
            }
            ++counts[c];
        }
    }

    /* find the largest letter group - this
    ** is used by the Apple II  buffer size
    */
    maxGroup = 0;
    for (i = 0; i < LETTERS; ++i) {
        if (counts[i] > maxGroup) {
            maxGroup = counts[i];
        }
    }

    /* create generated C header
    */
    fp = fopen("WORDDATA.H", "w");
    if (fp == NULL) {
        puts("Cannot create WORDDATA.H");
        exit(1);
    }

    /* write file contents
    */
    fprintf(fp, "/*\n");
    fprintf(fp, "** generated word data\n");
    fprintf(fp, "** this file is generated by makepack.c\n");
    fprintf(fp, "*/\n\n");
    fprintf(fp, "#ifndef WORDDATA_H\n");
    fprintf(fp, "#define WORDDATA_H\n\n");
    fprintf(fp, "#define SOLUTION_COUNT %uU\n", solCount);
    fprintf(fp, "#define ALL_WORD_COUNT  %uU\n", allCount);
    fprintf(fp, "#define MAX_GROUP_WORDS %uU\n", maxGroup);
    fprintf(fp, "#define GROUP_BUF_SIZE  (MAX_GROUP_WORDS * WORD_LENGTH)\n\n");

    /* write byte offsets into ALL5.BIN
    */
    fprintf(fp, "static const unsigned long allOffset[26] = {\n");
    for (i = 0; i < LETTERS; ++i) {
        fprintf(fp, "    %luL%s\n", offsets[i],
                (i + 1 == LETTERS) ? "" : ",");
    }
    fprintf(fp, "};\n\n");

    /* write word counts for each letter group
    */
    fprintf(fp, "static const unsigned int allCount[26] = {\n");
    for (i = 0; i < LETTERS; ++i) {
        fprintf(fp, "    %uU%s\n", counts[i],
                (i + 1 == LETTERS) ? "" : ",");
    }
    fprintf(fp, "};\n\n");
    fprintf(fp, "#endif\n");

    /* close file
    */
    fclose(fp);
}

/* =============
** program entry
** =============
*/
int main(void) {

    /* load normal text files
    */
    allCount = loadList("ALL.TXT", allWords);
    solCount = loadList("SOLUTION.TXT", solWords);

    /* create packed binary files
    */
    writePacked("ALL5.BIN", allWords, allCount);
    writePacked("SOL5.BIN", solWords, solCount);

    /* create header used by WORDLE80.C
    */
    writeHeader();

    printf("ALL words:      %u\n", allCount);
    printf("Solution words: %u\n", solCount);
    puts("Wrote ALL5.BIN, SOL5.BIN, WORDDATA.H");

    return 0;
}
