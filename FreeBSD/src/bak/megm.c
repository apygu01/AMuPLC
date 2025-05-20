
// ----------------------
//  AMuPLC (megm.c)
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
FILE *fpi,*fpo;
//FILE  *fopen_h(),*fpi,*fpo;
int  gbc;
int  movn;
int  barc;
int  midi;     // Conducter lines midi or quing
int  trif;     // Trial mode flag 
int  allf;
char staff;    /* set staff mark */
int  memcnt;
int  seson;
int  deflp;    /* default staff make loop */
char lpc[22] = "Low staff///////////";
char hpc[22] = "High staff//////////";
char mpc[22] = "Middle staff////////";
char rpc[22] = "Rithm staff/////////";
char dpc[22] = "Dodecaphonie staff//";
char tpc[22] = "Top staff///////////";
char gpc[22] = "Gclef staff/////////";
char spc[22] = "Soprano staff///////";
char apc[22] = "Alt staff///////////";
char bpc[22] = "Bass staff//////////";
char ubc[22] = "Under Bass staff////";
int  argfn[MAXA];
int  argmv[MAXA];
char score[STLN];

int main(int argc,char *argv[])
//int  argc;
//char *argv[];
{
    int  i,j;
    int  list;
    char dtopt[10];
    char *pa;
    char *pb;
    int  help;
    int  stfcnt;

    list = 0;
    barc = 0;
    movn = 0;
    help = 0;
    midi = 1;
    trif = 0;
    seson = 0;
    stfcnt = 0;
    if(argc <= 1) {
        help_meg();
        return(0);
    }
    for(i = 0; i < argc; i++) {
        pa = argv[i];
        j = 0;
        while(*pa) {
            dtopt[j] = toupper(*pa);
            j++;
            if(j > 8) break;
            pa++;
        }
        dtopt[j] = '\0';
        if(!strcmp(dtopt,"-H")) help = 1;
        if(!strcmp(dtopt,"-TRI")) trif = 1;
        if(!strcmp(dtopt,"-TRIAL")) trif = 1;
        if(!strcmp(SUPPORT,"Debug")) {
            if(!strcmp(dtopt,"-S")) list = 1;
            if(!strcmp(dtopt,"-T")) list = 1;
            if(!strcmp(dtopt,"-L")) list = 2;
            if(!strcmp(dtopt,"-M")) midi = 1;
            if(!strcmp(dtopt,"-Q")) midi = 0;
            if(!strcmp(dtopt,"-A")) allf = 1;
            if(!strcmp(dtopt,"-ALL")) allf = 1;
            if(!strcmp(dtopt,"-Tri")) trif = 1;
            if(!strcmp(dtopt,"-Trial")) trif = 1;
        }
        if(i > 1 && dtopt[0] == '/') {
            seson = 1;
            strcpy(score,"meg");
            pa = argv[i];
            j = 3;
            pa++;
            while(*pa) {
                score[j] = *pa;
                j++;
                if(j > STLN) break;
                pa++;
            }
            score[j] = '\0';
            strcat(score,".scr");
        }
    }
    if(help) {
        fhelp_meg();
        return(0);
    }
    for(i = 0; i < MAXA; i++) {
        argfn[i] = 0;
        argmv[i] = 0;
    }
    staff = 'M';
    for(i = 1; i < argc; i++) {
        pa = argv[i];
        if(*pa == '/') *pa = '-';
        if(isalpha(*pa)) argfn[i] = 1;
        else if(*pa != '-') {    /* Check staff define */
            argmv[i] = 1;
            pb = argv[i];
            while(*pb) {
                if(isalpha(*pb)) {
                    if(staff != toupper(*pb)) stfcnt++;
                    staff = toupper(*pb);
                }
                pb++;
            }
            if(staff != 'M') break;
        }
    }
    if(stfcnt > 1) staff = 'X';   /* mixed staff */
    if(!seson) {  // 2025.5.3
        if(trif) strcpy(score,"megtutor.scr");
        else strcpy(score,"megdef.scr");
    }
    scoremk_meg(argc,argv,list);
    return(0);
}

int multif_h(char *infn,char *megf,char *ftyp,int fnmb,int *prtn)
//char *infn;
//char *megf;
//char *ftyp;
//int  fnmb;
//int  *prtn;
{
    int  i,n;
    int  nlen;
    char w[25];
    int  mflg;
    int  xflg;
    char *pmf;
    char *xmf;
    char xw[10];
    char fnum[25];

    xw[0] = '\0';
    xmf = infn;
    *prtn = 0;
    xflg = 0;
    while(*(xmf++)) if(*xmf == ';') xflg = 1;
    if(xflg) {
        i = 0;
        xmf = infn;
        while(*(xmf++)) if(*xmf == ';') break;
        while(*(xmf++)) xw[i++] = *xmf;
    }
    sscanf(xw,"%d",&n);
    mflg = 0;
    i = 0;
    pmf = infn;
    nlen = 3;
    strcpy(fnum,"000");
    while(*pmf) {
        if(*pmf == '/') {
            mflg = 1;
            break;
        }
        w[i++] = *pmf;
        pmf++;
    }
    w[i] = '\0';
    if(mflg) if(!xflg) {
        printf("Error! file name \"%s\"\n",infn);
        exit(1);
    }
    if(mflg) while(*(infn++)) if(*infn == ';') break;
    if(mflg) {
        while(*infn != '/') {
            fnum[nlen] = *infn;
            nlen--;
            infn--;
        }
    }
    sscanf(fnum,"%d",prtn);
    sprintf(fnum,"%03d",fnmb+1);
    strcpy(megf,w);
    strcat_h(megf,"_");
    strcat_h(megf,fnum);
    strcat_h(megf,ftyp);
    return(n-fnmb);
}

FILE *fopen_h(char *file, char *mode)
//char *file;
//char *mode;
{
    FILE *fp;
    //FILE *fp,*fopen();
    char scrf[100];

    if((fp = fopen(file, mode)) != NULL) return(fp);
    sprintf(scrf,"%s",TACTDIR);
    //sprintf(scrf,"%s\0",TACTDIR);
    strcat(scrf,file);
    if((fp = fopen(scrf, mode)) != NULL) return(fp);
    printf("Can't open file %s mode %s\n",
        scrf, mode);
    exit(1);
}

int strcat_h(char *tos, char *from)
//char *tos,*from;
{
    while(*tos) tos++;
    while(*from) {
        *tos = *from;
        from++;
        tos++;
    }
    *tos = '\0';
    return(0);
}

int fhelp_meg()
{
    int  c;
    FILE *fpd;
    char scrf[100];

    printf("\nMeg V %s\n",VERSION);
    if((fpd = fopen("meghlp.dat","r")) == NULL) {
        sprintf(scrf,"%s",TACTDIR);
        //sprintf(scrf,"%s\0",TACTDIR);
        strcat(scrf,"meghlp.dat");
        if((fpd = fopen(scrf,"r")) == NULL) {
            printf("Can't open file %s mode r\n",scrf);
            return(1);
        }
    }
    while((c = fgetc(fpd)) != EOF) putchar(c);
    fclose(fpd);
    return(0);
}

help_meg()
{
    printf("\n Music Edit file Genarater V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    printf(" meg fn Barnum [next sequence Burnum]");
    if(!strcmp(SUPPORT,"Release")) {
        printf(" [/Score] [Options]\n");
        printf("     fn          file name\n");
        printf("     Barnum      xx : bar number\n");
        printf("     Score       [/def] /solo /pf /pop /orc ..\n");
        printf("     Options :   -tri -trial Trial mode\n");
        printf("                 -h(help)\n");
        printf(" usage: e.g.\n");
        printf("      meg -h\n");
        printf("      meg www 4 -tri\n");
        printf("\n\n");
        return(0);
    }
    printf("  [ /Score] [ -Options]\n");
    printf("     fn         file name\n");
    printf("     Barnum     xx : bar number\n");
    printf("     Score      [/def] /solo /pf /pop /orc ..\n");
    printf("     Options :  -s(source) -h(help) -a(all)\n");
    printf("                -q(cueing format : default midi format)\n");
    printf("                -h(help)\n");
    printf("  usage e.g.\n");
    printf(" meg  abc /solo 8\n");
    printf("      abc <- output file name\n");
    printf("          /solo <- solo score\n");
    printf("               8 <- bars\n");
    printf(" meg  aaa /pop 8 8 8 -s\n");
    printf("      aaa <- output file name\n");
    printf("          /pop  <- pop score\n");
    printf("               8 8 8 <- Sequence 0,1,2 bars\n");
    printf("\n");
    return(0);
}

