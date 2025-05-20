
// ----------------------
//  AMuPLC (mec7.c)
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

#include "mec.h"
extern MECARG *mg;
extern MECVAR *mr;
extern MECMST *mm;
extern MECFRS *fr;
extern long fnval;
extern long c_sync;
extern long max_sync;
extern int exfc;
extern int exfovf;  /* Expand symbol Table over flag */
int exlc; /* Expand line count */
char scard[MSTC][82];
/* struct MECEXP mp[MAXEXF]; change mp -> mx 2002.4 */
struct MECEXP mx[MAXEXF];

int smfmeta_mec()
{
    char c;
    union meccode f6;

    /* printf("%s\n",mg->card); */
    f6.zi[0] = 0xf602;
    c = mg->card[1];
    switch(c) {
        case 'N' : /* Sequence Number */
                   f6.zi[1] = 0xff00;
                   f6meta_smf(&f6);
                   break;
        case 'X' : /* Text Event */
                   f6.zi[1] = 0xff01;
                   f6meta_smf(&f6);
                   break;
        case 'C' : /* Copyright Notice */
                   f6.zi[1] = 0xff02;
                   f6meta_smf(&f6);
                   break;
        case 'S' : /* Sequence Name */
                   f6.zi[1] = 0xff03;
                   f6meta_smf(&f6);
                   break;
        case 'I' : /* Instrument Name */
                   f6.zi[1] = 0xff04;
                   f6meta_smf(&f6);
                   break;
        case 'L' : /* Liric */
                   f6.zi[1] = 0xff05;
                   f6meta_smf(&f6);
                   break;
        case 'M' : /* Marker */
                   f6.zi[1] = 0xff06;
                   f6meta_smf(&f6);
                   break;
        case 'P' : /* Cue Point */
                   f6.zi[1] = 0xff07;
                   f6meta_smf(&f6);
                   break;
        default  : break;
    }
    return(0);
}

/*
 mec -smf
 *X   < Put Text > 
*/
int f6meta_smf(union meccode *f6)
//union meccode *f6;
{
    int  i;
    char w[4];
    unsigned int c0,c1;
    unsigned int n;
    int  af;
    int  sp,ep;
    int  len;

    i = 0;
    af = 0;
    sp = 0;
    ep = 0;
    while(mg->card[i]) {
        if(mg->card[i] == '<') {
            sp = i;
            af++;
        }
        if(mg->card[i] == '>') {
            ep = i;
            af++;
        }
        i++;
        if(i > 79) break;
    }
    len = ep -sp;
    /* printf("len = %d\n",len); */
    if(af != 2) {
        mg->wrncnt++;
        mg->msglvl = 2;
        if(mg->msglvl >= mg->prtlvl)
            printf("Worning SMF Meta < Text > at f6meta_smf()\n");
        return(-1); 
    }
    if(mg->trace) {
        printf("Trace! F6(%04x%04x) SMF Meta Event FF %02d\n",
            f6->zi[0],f6->zi[1],f6->zi[1] & 0x00ff);
    }
    putcode_mec(f6);
    f6->zi[0] = 0xf602;
    f6->zi[1] = 0;
    n = len;
    n = n << 8;
    n &= 0xff00;
    f6->zi[1] = 0;
    f6->zi[1] |= n;
    n = 0x00ff & mg->card[sp + 1];
    f6->zi[1] |= n;
    if(mg->trace) {
        printf("Trace! F6(%04x%04x) 02 len = %02d '%c'\n",
            f6->zi[0],f6->zi[1],len,mg->card[sp + 1]);
    }
    putcode_mec(f6);
    for(i = sp + 2; i < ep + 1; i = i + 2) {
        if(mg->card[i] == '>') c0 = 0xf7;
        else c0 = mg->card[i];
        if(mg->card[i + 1] == '>')  c1 = 0xf7;
        else c1 = mg->card[i + 1];
        if(c0 != 0xf7 && c1 != 0xf7) {
            f6->zi[0] = 0xf602;
            f6->zi[1] = 0;
            n = c0;
            n = n << 8;
            n &= 0xff00;
            f6->zi[1] = 0;
            f6->zi[1] |= n;
            n = 0x00ff & c1;
            f6->zi[1] |= n;
            if(mg->trace) {
                printf("Trace! F6(%04x%04x) 02 '%c' '%c'\n",
                    f6->zi[0],f6->zi[1],c0,c1);
            }
            putcode_mec(f6);
        }
        /* printf("c0 = %c c1 = %c\n",c0,c1); */
    }
    if(c0 == 0xf7) {
        f6->zi[0] = 0xf601;
        n = c0 << 8;
        n &= 0xff00;
        f6->zi[1] = 0;
        f6->zi[1] |= n;
        if(mg->trace) {
            printf("Trace! F6(%04x%04x) 01 '%c' 00\n",
                f6->zi[0],f6->zi[1],c0);
        }
        putcode_mec(f6);
    }
    if(c1 == 0xf7) {
        f6->zi[0] = 0xf602;
        f6->zi[1] = 0;
        n = c0;
        n = n << 8;
        n &= 0xff00;
        f6->zi[1] = 0;
        f6->zi[1] |= n;
        n = 0x00ff & c1;
        f6->zi[1] |= n;
        if(mg->trace) {
            printf("Trace! F6(%04x%04x) 02 '%c' '%c'\n",
                f6->zi[0],f6->zi[1],c0,c1);
        }
        putcode_mec(f6);

        f6->zi[0] = 0xf600;
        f6->zi[1] = 0;
        if(mg->trace) {
            printf("Trace! F6(%04x%04x) 00 End of F6 Code\n",
                f6->zi[0],f6->zi[1]);
        }
        putcode_mec(f6);
    }
    return(0);
}

int syncout_mec(long sync,int typ)
//long sync;
//int  typ;  /* 1:lag 2:offset */
{
    union meccode fb,fc;
    long lagsync;
    long offsync;
    long w_sync;

    if(typ == 1) {  /* lag */
        if(mr->lagb) lagsync = mr->lagb;
        else lagsync = mr->lag;
        w_sync = sync - lagsync;
    }
    if(typ == 2) {  /* offset */
        if(mr->offsetb) offsync = mr->offsetb;
        else offsync = mr->offset;
        w_sync = sync - offsync;
    }
    /* printf("max_sync=%ld\n",max_sync); */
    /* if(w_sync > 0xffffff || w_sync < 0) return(-1); */
    if(w_sync > max_sync) w_sync = max_sync;
    if(w_sync < 0) w_sync = 1;
    lsconv_h(&fc,w_sync);
    fc.zi[0] &= 0x00ff;
    fc.zi[0] |= 0xfc00;
    if(mg->debug == 1) {
        if(typ == 1) {
            printf("Debug! FC(%04x%04x) w_sync=sync(%ld)-lag(%ld)=%ld\n",
            fc.zi[0],fc.zi[1], sync,lagsync,w_sync);
        }
        if(typ == 2) {
            printf("Debug! FC(%04x%04x) w_sync=sync(%ld)-offset(%ld)=%ld\n",
            fc.zi[0],fc.zi[1], sync,offsync,w_sync);
        }
    }
    if(w_sync <= c_sync) {
        if(mg->trace) {
            if(typ == 1) {
                printf("Trace! FC(%04x%04x) w_sync=sync(%ld)-lag(%ld)=%ld\n",
                fc.zi[0],fc.zi[1], sync,lagsync,w_sync);
            }
            if(typ == 2) {
                printf("Trace! FC(%04x%04x) w_sync=sync(%ld)-offset(%ld)=%ld\n",
                fc.zi[0],fc.zi[1], sync,offsync,w_sync);
            }
        }
        putcode_mec(&fc);
    }
    c_sync = w_sync;
    return(0);
}

/*
   [],{},()
   {lag = xxx}
   (lag = _/))) (lag = ./>>> <note>)
   [lag = xxx <ms>] [lag = xxx <micro Sec>] [lag = xx <Flame>]
   {lag xxx <bit> etc.
   <unit = note,flame,bit,ms,micro sec>
*/

int lag_offset_mec()
{
    int  i,j,k;
    int  flj,fln;
    char wk[81];
    char card[81];
    int  spf;
    int  uval; /* unit value */
    int  nval;
    int  slen;
    int  micros;

    if(mg->card[0] == '{') spf = 1;   /* {} */
    if(mg->card[0] == '(') spf = 2;   /* () */
    if(mg->card[0] == '[') spf = 3;   /* [] */
    strcpy(card,mg->card);
    slen = strlen(card);
    flj = 0;
    fln = 0;
    for(i = 0; i < slen; i++) {
        if(card[i] == '<') flj++;
        if(card[i] == '>') flj++;
        if(card[i] == 'u') fln++;  /* Unit */
        if(card[i] == 'i') fln++;  /* unIt bIt mIlIsec mIcrosec */
        if(card[i] == 'm') fln++;  /* Min Microsec Milisec Mtc flaMe */
        if(card[i] == 'c') fln++;  /* seC mtC */
    }
    if(flj != 2 && fln > 0) {
        inspr_mec(card); /* fill <  > */
        slen = strlen(card);
        strcpy(mg->card,card);
    }
    /* printf("%c\n",card[slen-1]); */
    card[0] = ' ';
    card[slen-1] = ' ';
    spacecut_mec(card,slen);
    /* printf("%s\n",card); */
    for(i = 0; i < 3;i++) wk[i] = card[i];
    wk[3] = '\0';
    if(!strcmp(wk,"lag")) {
        spf = spf + 10; /* lag */
        for(i = 0; i < 3; i++) card[i] = ' ';
    }
    if(!strcmp(wk,"off")) {
        spf = spf + 20; /* offset */
        for(i = 0; i < 6; i++) card[i] = ' ';
    }
    /* printf("spf=%d\n",spf); */
    spacecut_mec(card,slen);
    /* printf("%s\n",card); */
    slen = strlen(card);
    if(flj == 2 || fln > 0) { /* unit serch */
        strcpy(wk,card);
        slen = strlen(wk);
        for(i = 0; i < slen; i++) {
            if(wk[i] == '<') break;
            wk[i] = ' ';
        }
        spacecut_mec(wk,slen);
        slen = strlen(wk);
        /* printf("%s\n",wk); */
        /*
           note:1  flame:2  sec:3  msec:7  micro sec:5  mtc=unit4:4
           unit8:6  unit10:9  bit=unit80:8  unit100:10 */
        if(wk[1] == 'n' && wk[2] == 'o') mr->sutyp = 1;  /* note */
        if(wk[1] == 'f' && wk[2] == 'l') mr->sutyp = 2;  /* flame */
        if(wk[1] == 's' && wk[2] == 'e') mr->sutyp = 3;  /* sec  */
        if(wk[1] == 'b' && wk[2] == 'i') mr->sutyp = 8;  /* bit  */
        micros = 0;
        if(wk[1] == 'm') {
            for(i = 2; i < slen; i++) {
                if(wk[i] == 'r') micros++; 
                if(wk[i] == 'o') micros++; 
            }
	    if(micros > 0) mr->sutyp = 5; /* micro sec */
            else mr->sutyp = 7; /* milli sec */
            if(wk[2] == 't') mr->sutyp = 4;  /* mtc   */
        } 
        if(wk[1] == 'u') {
            for(i = 0; i < slen; i++) if(!isdigit(wk[i])) wk[i] = ' ';
            spacecut_mec(wk,slen);
            sscanf(wk,"%d",&j);
            /* printf("%s j=%d\n",wk,j); */
            if(j == 4) mr->sutyp = 4; /* unit4 = mtc */
            if(j == 8) mr->sutyp = 6; /* unit8 */
            if(j == 10) mr->sutyp = 9; /* unit10 */
            if(j == 80) mr->sutyp = 8; /* unit80 = bit */
            if(j == 100) mr->sutyp = 10; /* unit100 */
        }
        slen = strlen(card);
        k = 0;
        for(i = 0; i < slen; i++) {
            wk[i] = card[i];
            if(wk[i] == '=') wk[i] = ' ';
            if(wk[i] == '>') k++;
            if(wk[i] == '_') k++;
            if(wk[i] == '/') k++;
            if(wk[i] == ')') k++;
            if(wk[i] == '.') k++;
            if(wk[i] == '<') break;
        }
        wk[i] = '\0';
        slen = strlen(wk);
        spacecut_mec(wk,slen);
        uval = 0;
        if(k > 1) { /* _/).note date */
            /* printf("%s\n",wk); */
            card[0] = '\0';
            strcat(card,"%50 ");
            strcat(card,wk);
            getnval_mec(card); /* get fnval */
            uval =  fnval;
        }
        else {  /* numeric */
            /* printf("%s\n",wk); */
            sscanf(wk,"%d",&uval);
        }
    }
    else {
        /* printf("%s\n",card); */
        slen = strlen(card);
        mr->sutyp = 10;   /* default unit 100 */
        k = 0;
        for(i =0 ; i < slen; i++) {
            if(card[i] == '>') k++;
            if(card[i] == '_') k++;
            if(card[i] == '/') k++;
            if(card[i] == ')') k++;
            if(card[i] == '.') k++;
        }
        if(k > 1) {   /* symbol note */
            wk[0] = '\0';
            strcat(wk,"%50 ");
            strcat(wk,card);
            /* printf("%s\n",wk);  %xx */
            getnval_mec(wk); /* get fnval */
            uval =  fnval;
        }
        else { /* note numeric */
            sscanf(card,"%d",&uval);
        }
    }
    nval = uvtonv_mec(uval); /* uval to nval */
    switch(spf) {
        case   11 : mr->lag = nval;   /* {lag} */
                    if(mg->trace) {
                        printf("%s uval=%d mr->lag=%d mr->sutyp=%d\n",
                        mg->card,uval,mr->lag,mr->sutyp);
                    }
                    break;
        case   12 : mr->lag = nval;   /* (lag) */
                    if(mg->trace) {
                        printf("%s uval=%d mr->lag=%d mr->sutyp=%d\n",
                        mg->card,uval,mr->lag,mr->sutyp);
                    }
                    break;
        case   13 : mr->lagb = nval;  /* [lag] */
                    if(mg->trace) {
                        printf("%s uval=%d mr->lagb=%d mr->sutyp=%d\n",
                        mg->card,uval,mr->lagb,mr->sutyp);
                    }
                    break;
        case   21 : mr->offset = nval;  /* [lag] */
                    if(mg->trace) {
                        printf("%s uval=%d mr->offset=%d mr->sutyp=%d\n",
                        mg->card,uval,mr->offset,mr->sutyp);
                    }
                    break;
        case   22 : mr->offset = nval;  /* [lag] */
                    if(mg->trace) {
                        printf("%s uval=%d mr->offset=%d mr->sutyp=%d\n",
                        mg->card,uval,mr->offset,mr->sutyp);
                    }
                    break;
        case   23 : mr->offsetb = nval; /* [lag] */
                    if(mg->trace) {
                        printf("%s uval=%d mr->offsetb=%d mr->sutyp=%d\n",
                        mg->card,uval,mr->offsetb,mr->sutyp);
                    }
                    break;
        default   : break;
    }
    return(0);
}

/* fill <  > */
int inspr_mec(char *wk)
//char *wk;
{
    int  i,j;
    int  slen;
    char cw1[81];
    char cw2[81];

    /* printf("wk=%s\n",wk); */
    /* strcpy(wk,mg->card); */
    slen = strlen(wk);
    spacecut_mec(wk,slen);
    slen = strlen(wk);
    strcpy(cw1,wk);
    strcpy(cw2,wk);
    /* printf("slen=%d\n",slen); */
    for(i = slen ; i >= 0 ; i--) {
        if(cw1[i] == 'u') break;
        if(cw1[i] == 'i') break;
        if(cw1[i] == 'm') break;
        if(cw1[i] == 'c') break;
    }
    for(; i >= 0 ; i--) {
        if(isdigit(cw1[i])) {
            cw1[i+1] = '\0';
            break;
        }
        if(cw1[i] == ')') { /* note _/) */
            cw1[i+1] = '\0';
            break;
        }
        if(cw1[i] == '>') {  /* note ./> */
            cw1[i+1] = '\0';
            break;
        }
        if(cw1[i] == '/') { /* note _/ */
            cw1[i+1] = '\0';
            break;
        }
        if(cw1[i] == 'O') {
            cw1[i+1] = '\0';
            break;
        }
        if(cw1[i] == '.') {
            cw1[i+1] = '\0';
            break;
        }
    }
    j = i + 1;
    for(i = 0 ; i < j ; i++) cw2[i] = ' ';
    slen = strlen(cw2);
    cw2[slen - 1] = '>';
    spacecut_mec(cw2,slen);
    wk[0] = '\0';
    strcat(wk,cw1);
    strcat(wk,"<");
    strcat(wk,cw2);
    strcat(wk,"}");
    /* printf("wk=%s\n",wk);  */
    return(0);
}

int uvtonv_mec(int uval)
//int  uval;
{
    long  lnval;
    int  nval;
    float fv;

    lnval = 0;
    /* printf("mr->temp=%d\n",mr->temp); */
    switch(mr->sutyp) {
        case   1 : /* note */
                   lnval = uval;
                   break;
        case   2 : /* flame */
                   fv = 500 * mr->temp /900;
                   fv = fv * uval;
                   if(fv != 0) {
                       // 2025.4.7 abs(fv) -> abs((int)fv)
                       if(fv/abs((int)fv) > 1) lnval = fv + 1;  // kuriage
                       if(fv/abs((int)fv) < 1) lnval = fv - 1;  // kuriage
                   }
                   break;
        case   3 : /* sec */
                   fv = 6000*100000/mr->temp;
                   lnval = fv * uval;
                   break;
        case   7 : /* msec */
                   fv = 600*100000/mr->temp;
                   lnval = fv * uval;
                   break;
        case   5 : /* micro sec */
                   fv = 6*100000/mr->temp;
                   lnval = fv * uval;
                   break;
        case   4 : /* MTC */
                   fv = 5*mr->temp/9;
                   fv = fv/4; /* Resolution 4 */
                   lnval = fv * uval;
                   break;
        case   6 : /* Unit Resolution 8 */
                   fv = 5*mr->temp/9;
                   fv = fv/8; /* Resolution 8 */
                   lnval = fv * uval;
                   break;
        case   9 : /* Unit Resolution 10 */
                   fv = 5*mr->temp/9;
                   fv = fv/10; /* Resolution 10 */
                   lnval = fv * uval;
                   break;
        case   8 : /* Unit Resolution 80 */
                   fv = 5*mr->temp/9;
                   fv = fv/80; /* Resolution 80 */
                   lnval = fv * uval;
                   break;
        case  10 : /* Unit Resolution 100 */
                   fv = 5*mr->temp/9;
                   fv = fv/100; /* Resolution 100 */
                   lnval = fv * uval;
                   break;
        default  : break; 
    }
    if(lnval > 100000) {
        mg->wrncnt++;
        mg->msglvl = 2;
        if(mg->msglvl >= mg->prtlvl)
            printf("Worning! lag/offset value over than 100000, %ld ignored\n",
            lnval);
        nval = 0;
    }
    else nval = lnval;
    return(nval);
}

int spacecut_mec(char *card,int slen)
//char *card;
//int  slen;
{
    int  i,j;
    char wk[81];

    i = 0;
    j = 0;
    while(1) {
        if(card[i] != ' ') { /* space char cut */
            wk[j] = tolower(card[i]);
            j++;
        }
        i++;
        if(i > slen) break;
        if(i > 80) break;
    }
    wk[j] = '\0';
    strcpy(card,wk);
    return(0);
}

int expand_mec(char *inf,struct mecksn *mk)
//char *inf;
//struct mecksn *mk;         /* key signeture table */
{
    char w[5];
    FILE *fpe;
    char expf[MAXFN]; /* File mame max. */
    int  i;
    int  rtn;

    /* printf("%s\n",mg->card);  */
    i = 4;
    while(mg->card[i] != ' ') {
        i++;
        if(mg->card[i] == '[') return(1); /*  no expand */
        if(i > 13) break;
    }
    if(mg->card[75] == ' ') return(2);
    w[0] = mg->card[75];
    w[1] = mg->card[76];
    w[2] = mg->card[77];
    w[3] = mg->card[78];
    w[4] = '\0';
    mkexpfn_mmm(inf,expf,w);
    /* printf("%s\n",expf); */
    rtn = chkexpf_mec(expf);
    if(mg->trace) printf("Trace! rtn(pos) = %d exlc = %d\n",rtn,exlc);
    if(rtn == -1) return(-1); /* open error */
    strcpy(mg->befr,mg->card);
    overln_mec(); /* add [xx to mg->card */
    mr->renp = 1;
    if( mg->btyp == 's' ||
        mg->btyp == 't' ||
        mg->btyp == 'g' ||
        mg->btyp == 'a' ||
        mg->btyp == 'b' ||
        mg->btyp == 'u' ||
        mg->btyp == 'd') {
        syncepv_mec();      /* pitch vend */
        syncecnt_mec();    /* modul. control cg. */
        syncepcg_mec();    /* program change */
        syncetrs_mec();     /* transpose */
    }
    pedalck_mec();
    renget_mec();
    if(mr->renp >= MSTC) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d renpu max is %d\n",mg->line,MSTC);
        return(0);
    }
    if(mr->renp > 1) {
        if(rtn < 0) renexp_mec(expf,mk);
        else trenexp_mec(expf,mk,rtn);
    }
    strcpy(mg->card,mg->befr);
    return(0);
}


int overln_mec()
{
    int  i,j,k;
    char wk[8];
    char wk2[14];
    int  pf;

    i = 4;
    pf = 0;
    while(mg->card[i] != ' ') {
        if(mg->card[i] == '+' || mg->card[i] == '-') {
            pf = 1;
            break;
        }
        i++;
        if(i > 13) break;
    }
    if(pf) {
        j = i;
        k = 0;
        while(mg->card[j] != ' ') {
            wk2[k] = mg->card[j];
            j++;
            k++;
            if(j > 13) break;
        }
    }
    mg->card[i] = '[';
    sprintf(wk,"%4d",exlc + 1);
    /* printf("%s\n",wk); */
    if(wk[0] != ' ') mg->card[++i] = wk[0];
    if(wk[1] != ' ') mg->card[++i] = wk[1];
    if(wk[2] != ' ') mg->card[++i] = wk[2];
    if(wk[3] != ' ') mg->card[++i] = wk[3];
    if(pf) {
        for( j = 0; j < k; j++) mg->card[++i] = wk2[j];
    }
    /*
    if(mg->list || mg->source)
        printf("%s\n",mg->card);
    */
    return(0);
}

int chkexpf_mec(char expf[MAXFN])
//char expf[MAXFN];
{
    FILE *fpe;
    char line[82];
    int  ll;
    int  i,k;
    char wk[4];
    int  ovf;

    if(exfovf) {
        exlc = 0;
        if((fpe = fopen(expf,"rb")) != 0) {
            while(fgets(line, 82, fpe)) {
                if(line[0] == '%') exlc++;
                if(line[2] == '$') break;
            }
            fclose(fpe);
        }
        else {
            mg->wrncnt++;
            mg->msglvl = 2;
            if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d %s not found\n",mg->line,expf);
            return(-1);
        }
        return(-4); /* Table over */
    }
    i = 0;
    while(expf[i] != '_') {
        i++;
        if(i > MAXFN) break;
    }
    wk[0] = expf[++i];
    wk[1] = expf[++i];
    wk[2] = expf[++i];
    wk[3] = '\0';
    /* printf("wk = %s\n",wk); */
    for(i = 0; i < exfc; i++) {
        if(!strcmp(mx[i].exf,wk)) {
            exlc = mx[i].lc;
            return(i); /* Ari */
        }
    }
    exlc = 0;
    /* printf("expf = %s\n",expf); */
    if((fpe = fopen(expf,"r")) != 0) {
        ovf = 0;
        while(fgets(line, 82, fpe)) {
            if(line[0] == '%') {
                strcpy(mx[exfc].exd[exlc],line);
                exlc++;
                if(exlc > MSTC && ovf == 0) {
                    mg->wrncnt++;
                    mg->msglvl = 2;
                    if(mg->msglvl >= mg->prtlvl)
                        printf("Worning! %06d %s line over\n",mg->line,expf);
                    ovf = 1;
                }
            }
            if(line[2] == '$') break;
        }
        fclose(fpe);
        if(ovf) return(-2); /* line over */
        mx[exfc].lc = exlc;
        strcpy(mx[exfc].exf,wk);
        /* printf("mx[%d].exf = %s\n",exfc,mx[exfc].exf); */
        exfc++;
        if(exfc > MAXEXF) {
            mg->wrncnt++;
            mg->msglvl = 2;
            if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d %s Expand Symbol over\n",mg->line,expf);
            exfovf = 1;
            return(-3); /* Table over */
        }
    }
    else {
        mg->wrncnt++;
        mg->msglvl = 2;
        if(mg->msglvl >= mg->prtlvl)
            printf("Worning! %06d %s not found\n",mg->line,expf);
        return(-1);
    }
    return(exfc - 1); /* Nomal */
}

/* fnx_xxx.meg -> fnx_str.mph */
int mkexpfn_mmm(char *inf,char outf[MAXFN],char *str)
//char *inf;
//char outf[MAXFN];
//char *str;
{
    int  i;
    char *pif;
    char w[MAXFN];

    /* printf("%s\n",inf); */
    pif = inf;
    i = 0;
    while(*pif) {
        if(*pif == '_') break;
        outf[i] = *pif;
        if(i >= MAXFN -1 ) break;
        pif++;
        i++;
    }
    outf[i] = '\0';
    /* strcat_h(outf,"_"); */
    str[0] = '_';
    strcat_h(outf,str);
    strcat_h(outf,".mph");
    /* printf("%s\n",outf); */
    return(0);
}

int renexp_mec(char expf[MAXFN],struct mecksn *mk)
//char expf[MAXFN]; /* File mame max. */
//struct mecksn *mk;
{
    int  i,j,k;
    int  rencnt;
    FILE *fpe;
    char np[37];
    int  renp;
    int  splt;
    long nval;
    char next[82];
    char next2[82];
    int  grate;
    int  glc;
    char wk[4];
    int  lc;

    if((fpe = fopen(expf,"rb")) == 0) return(-1); /* open error */
    for(k = 0; k < 37; k++) np[k] = ' ';
    for(i = 32,k = 0; i < 69; i++,k++) {
        if(mg->card[i] == ':') np[k] = ':';
        if(isdigit(mg->card[i])) np[k] = ':';
    }
    wk[0] = mg->card[1];
    wk[1] = mg->card[2];
    wk[2] = '\0';
    sscanf(wk,"%d",&grate);
    strcpy(scard[0],mg->card);
    lc = exlc + 1;
    glc = ((lc * grate) + 1) / 100;  
    /* printf("wk = %s grate = %c%d lc = %d glc = %d\n",
          wk,'%',grate,lc,glc); */
    rencnt = 1;
    for(j = 1; j < lc;) {
        if(j > MSTC) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! renp count greater than %d\n",MSTC);
            return(-1);
        }
        /*
        printf("%s\n",mg->next); 
        printf("%s\n",mg->next2); 
        */
        fgets(scard[j],82,fpe);
        if(scard[j][0] == '%') {
            scard[j][4] = '[';
            for(i = 32,k = 0; i < 69; i++,k++) {
                scard[j][i] = np[k];
                if(mg->next[i] == ':') scard[j][i] = ':';
                else if(j > glc) scard[j][i] = ' ';
                if(mg->next[0] != '%') {
                    if(mg->next2[i] == ':') scard[j][i] = ':';
                    else if(j > glc) scard[j][i] = ' ';
                }
            }
            /* scard[j][75] = mg->card[75]; */
            scard[j][76] = mg->card[76];
            scard[j][77] = mg->card[77];
            scard[j][78] = mg->card[78];
        }
        if(scard[j][4] == '[') {
            j++;
            rencnt++;
        }
    }
    fclose(fpe);
    if(mr->rept <= 0) mr->rept = 1;
    strcpy(next,mg->next); /* save */
    strcpy(next2,mg->next2);
    for(k = 0; k < mr->rept; k++) {
        for(i = 0; i < lc ; i++) {
            strcpy(mg->card,scard[i]);
            strcpy(mg->next,scard[i+1]);
            strcpy(mg->next2,scard[i+2]); 
            if(mg->card[0] == '%' || mg->card[0] == 's' ||
                mg->card[0] == 'c') {
                if(mg->mfflg == 1) mnote_mec(mk);
                else {
                    if(i > 0) {
                        if( mg->btyp == 's' ||
                            mg->btyp == 't' ||
                            mg->btyp == 'g' ||
                            mg->btyp == 'a' ||
                            mg->btyp == 'b' ||
                            mg->btyp == 'u' ||
                            mg->btyp == 'd') {
                            syncepv_mec();   /* pitch vend */
                            syncecnt_mec();  /* modul. control cg. */
                            syncepcg_mec();  /* program change */
                            syncetrs_mec();     /* transpose */
                        }
                        pedalck_mec();
                    }
                    if(mg->btyp == 'r' || mg->btyp == 'd')
                        dnote_mec(mk,0,"dumy");
                    else note_mec(mk,0,"dumy");
                    if(i > 0) apedalck_mec(); // 2013.8.19
                }
            }
            if(i == 0) {
                nval = mr->nval;
                splt = mr->splt;
                renp = mr->renp;
            }
            mr->nval = nval;
            mr->splt = splt;
            mr->renp = renp;
            if(mg->source) {
                mg->prflg = 0;
                if(mg->source == 1) printf("%s",mg->card);
                if(mg->source == 2) prints_mec(mg->line,mg->card);
            }
        }
    }
    strcpy(mg->next,next); /* Back */
    strcpy(mg->next2,next2);
    return(0);
}

/* Table Use */
int trenexp_mec(char expf[MAXFN],struct mecksn *mk,int p)
//char expf[MAXFN]; /* File mame max. */
//struct mecksn *mk;
//int  p; /* Symbol table pointer */
{
    int  i,j,k;
    int  rencnt;
    FILE *fpe;
    char np[37];
    int  renp;
    int  splt;
    long nval;
    char next[82];
    char next2[82];
    int  grate;
    int  glc;
    char wk[4];
    int  lc;

    /* printf("expf = %s p = %d\n",expf,p); */
    for(k = 0; k < 37; k++) np[k] = ' ';
    for(i = 32,k = 0; i < 69; i++,k++) {
        if(mg->card[i] == ':') np[k] = ':';
        if(isdigit(mg->card[i])) np[k] = ':';
    }
    wk[0] = mg->card[1];
    wk[1] = mg->card[2];
    wk[2] = '\0';
    sscanf(wk,"%d",&grate);
    strcpy(scard[0],mg->card);
    lc = exlc + 1;
    glc = ((lc * grate) + 1) / 100;  
    /* printf("wk = %s grate = %c%d lc = %d glc = %d\n",
          wk,'%',grate,lc,glc); */
    rencnt = 1;
    for(j = 1; j < lc;) {
        if(j > MSTC) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! renp count greater than %d\n",MSTC);
            return(-1);
        }
        /* printf("%s\n",mg->next); */
        strcpy(scard[j],mx[p].exd[j - 1]);
        if(scard[j][0] == '%') {
            scard[j][4] = '[';
            for(i = 32,k = 0; i < 69; i++,k++) {
                scard[j][i] = np[k];
                if(mg->next[i] == ':') scard[j][i] = ':';
                else if(j > glc) scard[j][i] = ' ';
                if(mg->next[0] != '%') {
                    if(mg->next2[i] == ':') scard[j][i] = ':';
                    else if(j > glc) scard[j][i] = ' ';
                }
            }
            /* scard[j][75] = mg->card[75]; */
            scard[j][76] = mg->card[76];
            scard[j][77] = mg->card[77];
            scard[j][78] = mg->card[78];
        }
        if(scard[j][4] == '[') {
            j++;
            rencnt++;
        }
    }
    strcpy(scard[j],mg->next);
    strcpy(scard[++j],mg->next2);
    if(mr->rept <= 0) mr->rept = 1;
    strcpy(next,mg->next); /* save */
    strcpy(next2,mg->next2);
    for(k = 0; k < mr->rept; k++) {
        for(i = 0; i < lc ; i++) {
            strcpy(mg->card,scard[i]);
            strcpy(mg->next,scard[i+1]);
            strcpy(mg->next2,scard[i+2]); 
            if(mg->card[0] == '%' || mg->card[0] == 's' ||
                mg->card[0] == 'c') {
                if(mg->mfflg == 1) mnote_mec(mk);
                else {
                    if(i > 0) {
                        if( mg->btyp == 's' ||
                            mg->btyp == 't' ||
                            mg->btyp == 'g' ||
                            mg->btyp == 'a' ||
                            mg->btyp == 'b' ||
                            mg->btyp == 'u' ||
                            mg->btyp == 'd') {
                            syncepv_mec();   /* pitch vend */
                            syncecnt_mec();  /* modul. control cg. */
                            syncepcg_mec();  /* program change */
                            syncetrs_mec();     /* transpose */
                        }
                        pedalck_mec();
                    }
                    if(mg->btyp == 'r' || mg->btyp == 'd')
                        dnote_mec(mk,0,"dumy");
                    else note_mec(mk,0,"dumy");
                    if(i > 0) apedalck_mec(); // 2013.8.19
                }
            }
            if(i == 0) {
                nval = mr->nval;
                splt = mr->splt;
                renp = mr->renp;
            }
            mr->nval = nval;
            mr->splt = splt;
            mr->renp = renp;
            if(mg->source) {
                mg->prflg = 0;
                if(mg->source == 1) printf("%s",mg->card);
                if(mg->source == 2) prints_mec(mg->line,mg->card);
            }
        }
    }
    strcpy(mg->next,next); /* Back */
    strcpy(mg->next2,next2);
    return(0);
}
