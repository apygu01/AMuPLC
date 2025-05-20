
// ----------------------
//  AMuPLC (megw.c)
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

#include "meg.h"
extern argfn[MAXA];
extern argmv[MAXA];
extern FILE  *fpi,*fpo;

int megtri_meg(int list)
//int list;
{
    FILE *fpr;
    char card[82];

    fpr = fopen_h("megtri1.dat","rb");
    while(fgets(card, 82, fpr)) { 
        fprintf(fpo,"%s",card);
        if(list) printf("%s",card);
    }
    fclose(fpr);
    return(0);
}

