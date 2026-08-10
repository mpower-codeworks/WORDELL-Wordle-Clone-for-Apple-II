/*

       / / / / / / / /
     / / stats.h / /
   / / / / / / / /

         2026
        mpower

*/

#ifndef WORDLE_STATS_H
#define WORDLE_STATS_H

/* ===================
** function prototypes
** ===================
*/
int statsInit (
    void
);

int statsRecordWin (
    unsigned char guesses);

int statsRecordLoss (
    void
);

void statsPage (
    void
);

#endif
