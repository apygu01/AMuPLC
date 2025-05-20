
// ----------------------
//  AMuPLC (meg2.c)
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
extern char score[STLN];
extern int trif;

int scoremk_meg(int argc,char *argv[],int list)
//int  argc;
//char *argv[];
//int  list;
{
    int  i;
    int  prtn;
    char *pa;
    char mfile[STLN];

    for(i = 0; i < MAXA; i++) {
        argfn[i] = 0;
        argmv[i] = 0;
    }
    for(i = 1; i < argc; i++) {
        pa = argv[i];
        if(isalpha(*pa)) argfn[i] = 1;
        else if(*pa != '-') argmv[i] = 1;
    }
    for (i = 0; i < argc ; i++) {
        if(argfn[i]) {
            multif_h(argv[i],mfile,".meg",-1,&prtn);
            /* printf("mfile = %s\n",mfile); */
            scorechk_meg(argc,argv,mfile,list);
        }
        /* fclose(fpo); */
    }
    return(0);
}

int parthed_meg(int list)
{
    FILE *fpr;
    char card[82];

    // printf("//// parthed_meg trif=%d\n",trif);
    if(trif) fpr = fopen_h("megtrf3.dat","rb");
    else fpr = fopen_h("megrdf3.dat","rb");           
    while(fgets(card, 82, fpr)) {
		fprintf(fpo,"%s",card);
        if(list) printf("%s",card);
    }
    fclose(fpr);
    return(0);
}
