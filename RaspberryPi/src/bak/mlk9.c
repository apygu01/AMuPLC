
// ----------------------
//  AMuPLC (mlk9.c)
// ----------------------

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

#include "mlk.h"
/* mlkw.c */
extern MLKSRG *ls;
extern MLKVAR *lv;
extern MLKFRS *lr;
extern MLKCDE *lc;
extern MECMST *mm;
extern MLKPHS *ph;
extern int  iv[16][5];
long cdt; /* Current dl */
long pdt;
int  pdf; /* pdt on/off flag */
int  ppf;

int mksmf1_mlk(char ifile[STLN])
//char ifile[STLN];
{
    ls->msglvl = 1;
    if(ls->msglvl >= ls->prtlvl) {
        printf("Mlk end SMF Format 1 Resolution %d\n",
        lv->smfdiv);
    }
    return(0);
}
