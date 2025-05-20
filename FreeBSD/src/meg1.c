
// ----------------------
//  AMuPLC (meg1.c)
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
extern char score[STLN];
extern int  gbc;
extern int  movn;
extern int  barc;
extern char lpc[22];
extern char mpc[22];
extern char hpc[22];
extern char rpc[22];
extern char dpc[22];
extern char tpc[22];
extern char gpc[22];
extern char spc[22];
extern char apc[22];
extern char bpc[22];
extern char ubc[22];
extern int  midi;
extern FILE  *fpi,*fpo;
extern int allf;
extern int trif;
char prtsym[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int  cline;
int  amovn;
int  mbtyp;
char xl[4];

int scorechk_meg(int argc,char *argv[],char mfile[STLN],int list)
//int  argc;
//char *argv[];
//char mfile[STLN];
//int  list;
{
    int  i,j;
    char card[82];
    int  bmc;      /* befor mc */
    FILE *fpd;

    amovn = 0;
    for(j = 0 ; j < argc; j++) if(argmv[j]) amovn++;
    /* printf("amovn=%d\n",amovn); */
    fpd = fopen_h(score,"rb");
    cline = 0;
    while(fgets(card, 82, fpd)) {
        /* printf("%s",card); */
        if(card[0] == '$' && card[1] == '$') break;
        if(card[0] == '/' && card[1] == '*') break;
        if(card[0] == 'C' && (card[4] != ' ')) {
            i = 8;
            xl[0] = card[3];
            xl[1] = card[4];
            xl[2] = '\0';
            /* printf("xl = %s\n",xl); */
            bmc = card[i];
            for(j = 0 ; j < amovn; j++) {
                mbtyp = card[i];
                if(allf) {
                    switch (mbtyp) {
                        case  'c' : condmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'l' : lowbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'm' : midbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'h' : higbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'r' : rtmbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'd' : dodbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  't' : tcfbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'g' : gcfbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  's' : synbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'a' : altbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'b' : basbmake_meg(argc,argv,mfile,list,j);
                                    break;
                        case  'u' : ubsbmake_meg(argc,argv,mfile,list,j);
                                    break;
                    }
                }
                switch (mbtyp) {
                    case  'C' : condmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'L' : lowbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'M' : midbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'H' : higbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'R' : rtmbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'D' : dodbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'T' : tcfbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'G' : gcfbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'S' : synbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'A' : altbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'B' : basbmake_meg(argc,argv,mfile,list,j);
                                break;
                    case  'U' : ubsbmake_meg(argc,argv,mfile,list,j);
                                break;
                    default   : barmake_meg(argc,argv,mfile,list,j,bmc);
                                break;
                }
                i++;
            }
            cline++;
        }
    }
    fclose(fpd);
    return(0);
}

int barmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track,int bmc)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
//int  bmc;
{
    switch (bmc) {
        case  'C' : condmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'L' : lowbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'M' : midbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'H' : higbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'R' : rtmbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'D' : dodbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'T' : tcfbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'G' : gcfbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'S' : synbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'A' : altbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'B' : basbmake_meg(argc,argv,xfile,list,track);
                    break;
        case  'U' : ubsbmake_meg(argc,argv,xfile,list,track);
                    break;
        default   : break;
    }
    return(0);
}

int condmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char wkc[4];
    char gfile[STLN];


    strcpy(gfile,xfile);
    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    sprintf(wkc,"%02x",cline);
    xfile[i+1] = wkc[0];
    xfile[i+2] = wkc[1];
    xfile[i] = *(prtsym+track);
    if(xfile[i] == '0' && cline == 0) {
        j = 0;
        while(gfile[j++] != '_');
        gfile[j-1] = '\0';
        strcat(gfile,".meg");
        fpo = fopen_h(gfile,"wb");
        condtitl_meg(argc,argv,list);
        fclose(fpo);
        printf("Generate %s\n",gfile);
    }
    /* printf("xfile = %s\n",xfile); */
    fpo = fopen_h(xfile,"wb");
    condhed_meg(list);
    /*
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    */
    movn = 0;
    for(j = 0; j < argc; j++) {
        if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                mbarout_meg(argv[j],list);
                mvlout_meg(list);
            }
            movn++;
        }
    }
    //finout_meg("Conductor staff/////",list);
    finout_meg("Conductor staff//////",list);  // 2025.4.12
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int condhed_meg(int list)
{
    FILE *fpr;
    char card[82];
    
    // printf("//// condhed_meg trif=%d\n",trif);
    if(trif) fpr = fopen_h("megtrf2.dat","rb");
    else fpr = fopen_h("megrdf2.dat","rb");
    while(fgets(card, 82, fpr)) {
        fprintf(fpo,"%s",card);
        if(list) printf("%s",card);
    }   
    fclose(fpr);
    return(0); 
}

int lowbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char wkc[4];
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    /*
    sprintf(wkc,"%02x",cline);
    xfile[i+1] = wkc[0];
    xfile[i+2] = wkc[1];
    */
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    /* printf("xfile = %s\n",xfile); */
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'l';
                xbarout_meg(argv[j],bkind,list);
                xmvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(lpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int midbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'm';
                xbarout_meg(argv[j],bkind,list);
                xmvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(mpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int higbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'h';
                xbarout_meg(argv[j],bkind,list);
                xmvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(hpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int rtmbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'r';
                xbarout_meg(argv[j],bkind,list);
                xmvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(rpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

/* chromatic scale , dodecaphonie */
int dodbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'd';
                xbarout_meg(argv[j],bkind,list);
                xmvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(dpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int synbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 's';
                xbarout_meg(argv[j],bkind,list);
                smvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(spc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int tcfbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 't';
                xbarout_meg(argv[j],bkind,list);
                smvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(tpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int gcfbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//gcfbmake_meg(argc,argv,xfile,list,track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'g';
                xbarout_meg(argv[j],bkind,list);
                smvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(gpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int altbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//altbmake_meg(argc,argv,xfile,list,track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'a';
                xbarout_meg(argv[j],bkind,list);
                smvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(apc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int basbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//basbmake_meg(argc,argv,xfile,list,track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'b';
                xbarout_meg(argv[j],bkind,list);
                smvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(bpc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int ubsbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track)
//ubsbmake_meg(argc,argv,xfile,list,track)
//int  argc;
//char *argv[];
//char xfile[STLN];
//int  list;
//int  track;
{
    int  i,j;
    char bkind;

    barc = 0;
    i = 0;
    while(xfile[i++] != '_');
    xfile[i+1] = xl[0];
    xfile[i+2] = xl[1];
    xfile[i] = *(prtsym+track);
    fpo = fopen_h(xfile,"wb");
    parthed_meg(list);
    if(xfile[i] == '0' && cline == 0)
        condtitl_meg(argc,argv,list);
    movn = 0;
    for(j = 0; j < argc; j++) {
       if(argmv[j]) {
            if(movn == track) {
                gbc = 0;
                bkind = 'u';
                xbarout_meg(argv[j],bkind,list);
                smvlout_meg(list);
            }
            movn++;
        }
    }
    finout_meg(ubc,list);
    printf("Generate %s Sequence number = %d",xfile,movn);
    printf(" bar count = %d\n",barc);
    fclose(fpo);
    return(0);
}

int mbarout_meg(char *barop,int list)
//char *barop;
//int  list;
{
    int  i;
    int  pas;
    int  barn;

    pas = 1;
    barn = 0;
    while(*barop) {
        if(isdigit(*barop)) {
            i = *barop & 0x0f;
            barn = barn*10 + i;
        }
        else {
            pas = 0;
            switch (*barop) {
                case 'm' : xcondle_meg(barn,list);break;
                case 'M' : xcondle_meg(barn,list);break;
                case 'l' : xcondle_meg(barn,list);break;
                case 'L' : xcondle_meg(barn,list);break;
                case 'h' : xcondle_meg(barn,list);break;
                case 'H' : xcondle_meg(barn,list);break;
                case 'r' : xcondle_meg(barn,list);break;
                case 'R' : xcondle_meg(barn,list);break;
                case 'd' : xcondle_meg(barn,list);break;
                case 'D' : xcondle_meg(barn,list);break;
                case 't' : xcondle_meg(barn,list);break;
                case 'T' : xcondle_meg(barn,list);break;
                case 'g' : xcondle_meg(barn,list);break;
                case 'G' : xcondle_meg(barn,list);break;
                case 's' : xcondle_meg(barn,list);break;
                case 'S' : xcondle_meg(barn,list);break;
                case 'a' : xcondle_meg(barn,list);break;
                case 'A' : xcondle_meg(barn,list);break;
                case 'b' : xcondle_meg(barn,list);break;
                case 'B' : xcondle_meg(barn,list);break;
                case 'u' : xcondle_meg(barn,list);break;
                case 'U' : xcondle_meg(barn,list);break;
                default  : printf("Bar operand error (%s)\n",
                           barop);break;
            }
            barn = 0;
        }
        barop++;
    }
    if(pas) {
        xcondle_meg(barn,list);
    }
    return(0);
}

int xbarout_meg(char *barop,char bkind,int list)
//char *barop;
//char bkind;
//int  list;
{
    int  i;
    int  pas;
    int  barn;

    pas = 1;
    barn = 0;
    while(*barop) {
        if(isdigit(*barop)) {
            i = *barop & 0x0f;
            barn = barn*10 + i;
        }
        else {
            pas = 0;
            switch (*barop) {
                case 'm' : midbar_meg(barn,list);break;
                case 'M' : midbar_meg(barn,list);break;
                case 'l' : lowbar_meg(barn,list);break;
                case 'L' : lowbar_meg(barn,list);break;
                case 'h' : highbar_meg(barn,list);break;
                case 'H' : highbar_meg(barn,list);break;
                case 'r' : rtmbar_meg(barn,list);break;
                case 'R' : rtmbar_meg(barn,list);break;
                case 'd' : dodbar_meg(barn,list);break;
                case 'D' : dodbar_meg(barn,list);break;
                case 't' : tclfbar_meg(barn,list);break;
                case 'T' : tclfbar_meg(barn,list);break;
                case 'g' : gclfbar_meg(barn,list);break;
                case 'G' : gclfbar_meg(barn,list);break;
                case 's' : synsbar_meg(barn,list);break;
                case 'S' : synsbar_meg(barn,list);break;
                case 'a' : altbar_meg(barn,list);break;
                case 'A' : altbar_meg(barn,list);break;
                case 'b' : basbar_meg(barn,list);break;
                case 'B' : basbar_meg(barn,list);break;
                case 'u' : ubsbar_meg(barn,list);break;
                case 'U' : ubsbar_meg(barn,list);break;
                default  : printf("Error! bar operand (%s)\n",barop);
                           break;
            }
            barn = 0;
        }
        barop++;
    }
    if(pas) {
        switch (bkind) {
            case 'm' : midbar_meg(barn,list);break;
            case 'l' : lowbar_meg(barn,list);break;
            case 'h' : highbar_meg(barn,list);break;
            case 'r' : rtmbar_meg(barn,list);break;
            case 'd' : dodbar_meg(barn,list);break;
            case 't' : tclfbar_meg(barn,list);break;
            case 'g' : gclfbar_meg(barn,list);break;
            case 's' : synsbar_meg(barn,list);break;
            case 'a' : altbar_meg(barn,list);break;
            case 'b' : basbar_meg(barn,list);break;
            case 'u' : ubsbar_meg(barn,list);break;
            default  : printf("Error! bar kind not defined (xbarout_meg)\n");
        }
    }
    return(barn);
}

int midbar_meg(int barn,int list)
{
    int  j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) {
            printf("-m%02d%05d %s%s\n",movn,gbc,
            "_-1______0______1______2______*______",
            "4______5______6______7velo Pedal");
        }
	if(Cr == '\r') {
            fprintf(fpo,"-m%02d%05d %s%s%c\n",movn,gbc,
                "_-1______0______1______2______*______",
                "4______5______6______7velo Pedal",Cr);
	}
	else {
            fprintf(fpo,"-m%02d%05d %s%s\n",movn,gbc,
                "_-1______0______1______2______*______",
                "4______5______6______7velo Pedal");
	}
    }
    return(0);
}

int lowbar_meg(int barn,int list)
{
    int  j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) printf("-l%02d%05d-%s%s\n",movn,gbc,
        "2_____-1______0______1______2______",
        "*______4______5______6__velo Pedal");
	if(Cr == '\r') {
            fprintf(fpo,"-l%02d%05d-%s%s%c\n",movn,gbc,
            "2_____-1______0______1______2______",
            "*______4______5______6__velo Pedal",Cr);
	}
	else {
            fprintf(fpo,"-l%02d%05d-%s%s\n",movn,gbc,
            "2_____-1______0______1______2______",
            "*______4______5______6__velo Pedal");
	}
    }
    return(0);
}

int highbar_meg(int barn,int list)
{
    int  j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) printf("-h%02d%05d %s%s\n",movn,gbc,
        "_____1______2______*______4______5______",
        "6______7______8____velo Pedal");
	if(Cr == '\r') {
            fprintf(fpo,"-h%02d%05d %s%s%c\n",movn,gbc,
            "_____1______2______*______4______5______",
            "6______7______8____velo Pedal",Cr);
	}
	else {
            fprintf(fpo,"-h%02d%05d %s%s\n",movn,gbc,
            "_____1______2______*______4______5______",
            "6______7______8____velo Pedal");
	}
    }
    return(0);
}

/* rithm bar */
int rtmbar_meg(int barn,int list)
{
    int  j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) printf("-r%02d%05d %s%s\n",movn,gbc,
        " tp 3____.____4____.____5____.____6____.",
        "____7____.____8____velo Pedal");
	if(Cr == '\r') {
            fprintf(fpo,"-r%02d%05d %s%s%c\n",movn,gbc,
            " tp 3____.____4____.____5____.____6____.",
            "____7____.____8____velo Pedal",Cr);
	}
	else {
            fprintf(fpo,"-r%02d%05d %s%s\n",movn,gbc,
            " tp 3____.____4____.____5____.____6____.",
            "____7____.____8____velo Pedal");
	}
    }
    return(0);
}

/* dodecaphonic , chromatic scale bar */
int dodbar_meg(int barn,int list)
{
    int  j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) printf("-d%02d%05d %s%s\n",movn,gbc,
        "p.v. mod cn:cdt pcg tp 4____.____5____.",
        "____6____.____7____.velo P_Exp");
	if(CR == '\r') {
            fprintf(fpo,"-d%02d%05d %s%s%c\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp 4____.____5____.",
            "____6____.____7____.velo P_Exp",Cr);
	}
	else {
            fprintf(fpo,"-d%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp 4____.____5____.",
            "____6____.____7____.velo P_Exp");
	}
    }
    return(0);
}

/* */
int synsbar_meg(int barn,int list)
{
    int j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) {
            printf("-s%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "1______2______*______4______5______6 velo P_Exp");
        }
	if(Cr == '\r') {
            fprintf(fpo,"-s%02d%05d %s%s%c\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "1______2______*______4______5______6 velo P_Exp",Cr);
	}
	else {
            fprintf(fpo,"-s%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "1______2______*______4______5______6 velo P_Exp");
	}
    }
    return(0);
}

int tclfbar_meg(int barn,int list)
//int  barn;
//int  list;
{
    int j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) {
            printf("-t%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "*______4______5______6______7______8 velo P_Exp");
        }
	if(Cr == '\r') {
            fprintf(fpo,"-t%02d%05d %s%s%c\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "*______4______5______6______7______8 velo P_Exp",Cr);
	}
	else {
            fprintf(fpo,"-t%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "*______4______5______6______7______8 velo P_Exp");
	}
    }
    return(0);
}

int gclfbar_meg(int barn,int list)
//int  barn;
//int  list;
{
    int j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) {
            printf("-g%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "2______*______4______5______6______7 velo P_Exp");
        }
	if(Cr == '\r') {
            fprintf(fpo,"-g%02d%05d %s%s%c\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "2______*______4______5______6______7 velo P_Exp",Cr);
	}
	else {
            fprintf(fpo,"-g%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "2______*______4______5______6______7 velo P_Exp");
	}
    }
    return(0);
}

int altbar_meg(int barn,int list)
//altbar_meg(barn,list)
//int  barn;
//int  list;
{
    int j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) {
            printf("-a%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "0______1______2______*______4______5 velo P_Exp");
        }
	if(Cr == '\r') {
            fprintf(fpo,"-a%02d%05d %s%s%c\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "0______1______2______*______4______5 velo P_Exp",Cr);
	}
	else {
            fprintf(fpo,"-a%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "0______1______2______*______4______5 velo P_Exp");
	}
    }
    return(0);
}

int basbar_meg(int barn,int list)
//basbar_meg(barn,list)
//int  barn;
//int  list;
{
    int j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) {
            printf("-b%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "1______0______1______2______*______4 velo P_Exp");
        }
	if(Cr == '\r') {
            fprintf(fpo,"-b%02d%05d %s%s%c\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "1______0______1______2______*______4 velo P_Exp",Cr);
	}
	else {
            fprintf(fpo,"-b%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "1______0______1______2______*______4 velo P_Exp");
	}
    }
    return(0);
}

int ubsbar_meg(int barn,int list)
//ubsbar_meg(barn,list)
//int  barn;
//int  list;
{
    int j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(list) {
            printf("-u%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "2_____-1______0______1______2______* velo P_Exp");
        }
	if(Cr == '\r') {
            fprintf(fpo,"-u%02d%05d %s%s%c\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "2_____-1______0______1______2______* velo P_Exp",Cr);
	}
	else {
            fprintf(fpo,"-u%02d%05d %s%s\n",movn,gbc,
            "p.v. mod cn:cdt pcg tp",
            "2_____-1______0______1______2______* velo P_Exp");
	}
    }
    return(0);
}

/* conducters line */
int xcondle_meg(int barn,int list)
//int  barn;
//int  list;
{
    int j;

    for(j = 1; j <= barn ; j++) {
        barc++;
        gbc++;
        if(midi) {
	    if(Cr == '\r') {
                fprintf(fpo,"-c%02d%05d %s%s%c\n",movn,gbc,
                "  0____.____1____.____2 Exp  tp  velo",
                "   tempo    dymc  hh:mm:ss/ff.ff",Cr);
	    }
	    else {
                fprintf(fpo,"-c%02d%05d %s%s\n",movn,gbc,
                "  0____.____1____.____2 Exp  tp  velo",
                "   tempo    dymc  hh:mm:ss/ff.ff");
	    }
            if(list) printf("-c%02d%05d %s%s\n",movn,gbc,
                "  0____.____1____.____2 Exp  tp  velo",
                "   tempo    dymc  hh:mm:ss/ff.ff");
        }
        else {
	    if(Cr == '\r') {
                fprintf(fpo,"-c%02d%05d %s%s%c\n",movn,gbc,
                "  pno  msg  hh:mm:ss/ff.ff  evno  ainf",
                "  tempo    dymc  hh:mm:ss/ff.ff",Cr);
	    }
	    else {
                fprintf(fpo,"-c%02d%05d %s%s\n",movn,gbc,
                "  pno  msg  hh:mm:ss/ff.ff  evno  ainf",
                "  tempo    dymc  hh:mm:ss/ff.ff");
	    }
            if(list) printf("-c%02d%05d %s%s\n",movn,gbc,
                "  pno  msg  hh:mm:ss/ff.ff  evno  ainf",
                "  tempo    dymc  hh:mm:ss/ff.ff");
        }
    }
    return(0);
}

int mvlout_meg(int list)
{
    if(list) printf("==%02d%s%s\n",movn,
    "$$$$$   =============================",
    "======================  xx:xx:xx/xx.xx");
    if(Cr == '\r') {
        fprintf(fpo,"==%02d%s%s%c\n",movn,
        "$$$$$   =============================",
        "======================  xx:xx:xx/xx.xx",Cr);
    }
    else {
        fprintf(fpo,"==%02d%s%s\n",movn,
        "$$$$$   =============================",
        "======================  xx:xx:xx/xx.xx");
    }
    return(0);
}

int smvlout_meg(int list)
{
    if(list) printf("==%02d%s%s\n",movn,
    "$$$$$ ===============================",
    "=========================== $$$$$$$$$$");
    if(Cr == '\r') {
        fprintf(fpo,"==%02d%s%s%c\n",movn,
        "$$$$$ ===============================",
        "=========================== $$$$$$$$$$",Cr);
    }
    else {
        fprintf(fpo,"==%02d%s%s\n",movn,
        "$$$$$ ===============================",
        "=========================== $$$$$$$$$$");
    }
    return(0);
}

int xmvlout_meg(int list)
{
    if(list) printf("==%02d%s%s\n",movn,
    "$$$$$================================",
    "============================$$$$$$$$$$");
    if(Cr == '\r') {
        fprintf(fpo,"==%02d%s%s%c\n",movn,
        "$$$$$================================",
        "============================$$$$$$$$$$",Cr);
    }
    else {
        fprintf(fpo,"==%02d%s%s\n",movn,
        "$$$$$================================",
        "============================$$$$$$$$$$");
    }
    return(0);
}

int finout_meg(char title[22],int list)
//char title[22];
//int  list;
{
    int  i;
    char wktit[22];

    strcpy(wktit,title);
    for(i = 0; i < 22; i++) if(wktit[i] == '/') wktit[i] = '$';
    if(list) {
        printf("%s%s\n",
        "final..................................",
        "...................................final");
        printf("$$$$%s%s%s\n",wktit,"$$$$$$$$$$$$$$$",
        "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    }
    if(Cr == '\r') {
        fprintf(fpo,"%s%s%c\n",
        "final..................................",
        "...................................final",Cr);
        fprintf(fpo,"$$$$%s%s%s%c\n",wktit,"$$$$$$$$$$$$$$$",
        "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",Cr);
    }
    else {
        fprintf(fpo,"%s%s\n",
        "final..................................",
        "...................................final");
        fprintf(fpo,"$$$$%s%s%s\n",wktit,"$$$$$$$$$$$$$$$",
        "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    }
    return(0);
}

int condtitl_meg(int argc,char *argv[],int list)
//int  argc;
//char *argv[];
//int  list;
{
    int  i,j,k;
    char card[82];
    char mc;
    int  pbnum[36];
    int  ib;
    char *pb;

    fpi = fopen_h(score,"rb");
    while(fgets(card, 82, fpi)) {
        if(card[0] == '$' && card[1] == '$') break;
        if(card[0] == '/' && card[1] == '*') break;
        if(card[0] == 'C' && card[6] == '.') {
            k = 8;
            mc = card[k];
            for(j = 0 ; j < amovn; j++,k++) card[k] = mc;
        }
        fputs(card,fpo);
        for(i = 0; i < 80; i++) {
            if(card[i] == '\n') card[i] = '\0';
            if(card[i] == '\r') card[i] = '\0';
        }
        if(list) printf("%s\n",card);
    }
    fclose(fpi);
    k = 0;
    for(j = 0; j < 36;j++) pbnum[j] = 0;
    for(j = 0; j < argc; j++) {
        if(argmv[j]) {
            pb = argv[j];
            while(*pb) {
                if(isdigit(*pb)) {
                    ib = *pb & 0x0f;
                    pbnum[k] = pbnum[k]*10 + ib;
                }
                pb++;
            }
            k++;
        }
    }
    prbtitl_meg(list);
    return(0);
}

int prbtitl_meg(int list)
//int list;
{
    FILE *fpr;
    char card[82];

    if(trif) fpr = fopen_h("megtrf1.dat","rb");
    else fpr = fopen_h("megrdf1.dat","rb");
    while(fgets(card, 82, fpr)) {
        fprintf(fpo,"%s",card);
        if(list) printf("%s",card);
    }
    fclose(fpr);
    return(0);
}

