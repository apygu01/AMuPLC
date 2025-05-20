
// ----------------------
//  AMuPLC (mlkw.c)
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
MLKPHS *ph;
MLKMPH *mp;
int  iv[16][5];
extern long cdt; /* Current dl */
extern long pdt;
extern int  pdf; /* pdt on/off flag */
extern int  ppf;

int mkexecf_mlk(char ifile[STLN])
//char ifile[STLN];
{
    MLKPHS lph;
    MLKMPH mph;

    ph = &lph;
    mp = &mph;
    if(ls->debug) printf("mkexecf_() ifile=%s ls->outfm=%d lv->msfdt=%d\n",
        ifile,ls->outfm,lv->smfdiv);
    if(ls->outfm == 0) {
	mkmidif_mlk(ifile); /* mtc .mex */
    }
    if(ls->outfm == 1) {      /* -old option */
        if(ls->otrace) ls->otrace = 8;
        mkdtx(ifile);     /* delta time .mex file */
    }
    /* SMF 0 out file .mid */
    if(ls->outfm == 2) {
        if(ls->otrace) ls->otrace = 8;
        mksmf0_mid(ifile);
    }
    if(ls->outfm == 3) mksmf1_mlk(ifile); /* SMF format 1 .mid */
    if(ls->outfm == 4) {
        if(ls->otrace) ls->otrace = 8;
	newmex_mlk(ifile); /* new.mex multi port phrase */
    }
    return(0);
}

