
// ----------------------
//  AMuPLC (mksm.c)
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

/* sksm.c */
#include "mks.h"
MESV *ev;
FILE *fpi,*fpo;

int main(int argc,char *argv[])
//int  argc;
//char *argv[];
{
    int  i,j;
    char fn[FNLEN];
    char *pa;
    char aopt[20];
    int  af[20]; /* argument file check Max 20 */
    int  help;
    int  dump;
    MESV mksv;

    ev = &mksv;
    help = 0;
    dump = 0;
    ev->debug = 0;
    if(argc == 1) {
        help_mks();
        return(0);
    }
    if(argc > 20) {
        printf("argument over than 20\n");
        return(0);
    }
    init_ev();
    if(DIVISION == 0) ev->divf = 0;
    if(DIVISION == 1) ev->divf = 1;
    if(!strcmp(COMPUTER_TYPE,"IBMPC"))  ev->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"MSDOS"))  ev->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"SUN"))    ev->comptyp = 1;
    if(!strcmp(COMPUTER_TYPE,"Linux"))  ev->comptyp = 2;
    if(!strcmp(COMPUTER_TYPE,"PCBSD"))  ev->comptyp = 2;
    for(i = 0; i < argc; i++) {
        pa = argv[i];
        af[i] = 1;
        j = 0;
        while(*pa) {
            aopt[j] = toupper(*pa++);
            j++;
            if(j >= 20) break;
        }
        aopt[j] = '\0';
        if(!strcmp(aopt,"-T")) ev->trace = 1;
        if(!strcmp(aopt,"-OT")) ev->trace = 1;
        if(!strcmp(SUPPORT,"Debug")) { // add 2013.12.4
            if(!strcmp(aopt,"-D")) ev->debug = 1;
            if(!strcmp(aopt,"-H")) help = 1;
            if(!strcmp(aopt,"-DP")) dump = 1;
            // if(!strcmp(aopt,"-T")) ev->trace = 1;
            // if(!strcmp(aopt,"-OT")) ev->trace = 1;
        } //--------------
        if(!strcmp(aopt,"-R24")) ev->res = 24;
        if(!strcmp(aopt,"-R48")) ev->res = 48;
        if(!strcmp(aopt,"-R96")) ev->res = 96;
        if(!strcmp(aopt,"-R120")) ev->res = 120;
        if(!strcmp(aopt,"-R192")) ev->res = 192;
        if(!strcmp(aopt,"-R240")) ev->res = 240;
        if(!strcmp(aopt,"-R384")) ev->res = 384;
        if(!strcmp(aopt,"-R480")) ev->res = 480;
        if(!strcmp(aopt,"-R960")) ev->res = 960;
        if(!strcmp(aopt,"-D4")) ev->mtcres = 4;
        if(!strcmp(aopt,"-D8")) ev->mtcres = 8;
        if(!strcmp(aopt,"-D10")) ev->mtcres = 10;
        if(!strcmp(aopt,"-D80")) ev->mtcres = 80;
        if(!strcmp(aopt,"-D100")) ev->mtcres = 100;
        if(!strcmp(aopt,"-S24")) ev->flame = 24;
        if(!strcmp(aopt,"-S25")) ev->flame = 25;
        if(!strcmp(aopt,"-S29")) ev->flame = 29;
        if(!strcmp(aopt,"-S30")) ev->flame = 30;
        if(!strcmp(aopt,"-F0")) ev->format = 0;
        if(!strcmp(aopt,"-F1")) ev->format = 1;
        if(aopt[0] == '-') af[i] = 0;
    }
    if(ev->res) ev->divf = 0;
    if(ev->mtcres > 0 || ev->flame > 0) ev->divf = 1;
    /* printf("ev->divf=%d\n",ev->divf); */
    if(ev->divf == 0) init_div();
    if(ev->divf == 1) init_mtc();
    if(help) {
        help_mks();
        return(0);
    }
    if(dump) {
        for(i = 1; i < argc ; i++) {
            if(af[i]) {
                if(strlen(argv[i]) > FNLEN) {
                    printf("Please check file name.");
                    return(0);
                }
                strcpy(fn,argv[i]);
                strcat(fn,".mex");
                dump_mks(fn);
            }
        }
        return(0);
    }
    if(ev->debug) 
        printf("main() ev->debub=%d ev->trace=%d\n",ev->debug,ev->trace);
    for(i = 1; i < argc; i++) {
        if(af[i]) {
            if(strlen(argv[i]) > FNLEN) {
                printf("Please check file name.");
                return(0);
            }
            strcpy(fn,argv[i]);
            Mesmain_mks(fn);
        }
    }
    return(0);
}

int dump_mks(char *fn)
//char *fn;
{
    int i;
    int c;

    i = 0;
    printf("mks dump fn = %s\n",fn);
    fpi = fopen_h(fn,"rb");
    while((c = fgetc(fpi)) != EOF) {
        printf("%02x ",c & 0xff);
        i++;
        if((i % 20) == 0) {
            printf("\n");
        }
    }
    printf("\n");
    fclose(fpi);
    return(0);
}

int help_mks()
{
    printf("\n Make Standard Midi File  V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    if(!strcmp(SUPPORT,"Release")) {
        printf(" mks fn options[ -t -Rxx ]\n");
        printf("           fn : File name\n");
        printf("           -t : trace\n");
        printf("        -Rxxx : 240");
        printf(",480,960 quarter note Resolution\n");
        printf("              : Default Resolution 480\n");
        printf("              : Default 30 Smpte flame type\n");
        printf("              : Default smf format 0\n");
        printf("\n");
        printf(" usage: e.g.\n");
        printf("      mks www -t\n");
        printf("\n");
        return(0);
    }
    printf(" mks fn options[-t -Rxx -Dxx -f -d -dp]\n");
    printf("        -dp   : dump\n");
    printf("        -t    : trace\n");
    printf("        -Rxxx : 24,48,60,96,120,192,240");
    printf(",384,480,960 quarter note Resolution\n");
    printf("        -Sxxx : 24,25,29,30 Smpte flame type\n");
    printf("        -Dxxx : 4,8,10,80,100 smpte Division resolution\n");
    printf("        -fx   : f0,f1\n");
    printf("                f0      : smf format 0\n");
    printf("                   f1   : smf format 1 Play NG\n");
    printf("\n");
    printf(" usage: e.g.\n");
    printf("      mks www -f1 -t\n");
    printf("      mks www -R240 -t\n");
    printf("\n");
    return(0);
}

FILE *fopen_h(char *file, char *mode)
//char *file;
//char *mode;
{
    FILE *fp;

    if((fp = fopen(file, mode)) == NULL) {
        printf("Mks Can't open file %s mode %s\n",file, mode);
        exit(1);
    }
    return(fp);
}

int init_ev()
{
    ev->dckn = 0;
    ev->debug = 0;
    ev->trace = 0;
    ev->comptyp = 0;
    ev->format = 0;
    ev->outf = 0;
    ev->occnt = 0;
    ev->trks = 0;
    ev->lag = 0;
    ev->lags = 0;
    ev->res = 0;
    ev->flame = 0;
    ev->smfdiv = 0;
    ev->mtcres = 0;
    ev->tempo = 6000;
    ev->nval[0] = 800000;
    ev->nval[1] = 700000;
    ev->nval[2] = 600000;
    ev->nval[3] = 400000;
    ev->nval[4] = 350000;
    ev->nval[5] = 300000;
    ev->nval[6] = 200000;
    ev->nval[7] = 175000;
    ev->nval[8] = 150000;
    ev->nval[9] = 100000;
    ev->nval[10] = 87500;
    ev->nval[11] = 75000;
    ev->nval[12] = 50000;
    ev->nval[13] = 43750;
    ev->nval[14] = 37500;
    ev->nval[15] = 25000;
    ev->nval[16] = 21875;
    ev->nval[17] = 18750;
    ev->nval[18] = 12500;
    ev->nval[19] =  9375;
    ev->nval[20] =  6250;
    ev->nval[21] =  3125;
    return(0);
}

