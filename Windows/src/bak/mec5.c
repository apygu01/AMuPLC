
// ----------------------
//  AMuPLC (mec5.c)
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
int  renp;
int  splt;
long fnval;

/* frase sync check
 * called at after putcode_mec(fc)
 */
int fraseck_mec(long syn)
//long syn;  /* mr->syn or mr->psyn */
{
    union meccode e1,e2,e3,e4;
    int  p;       /* prase table */

    for(p = 0; p < 5; p++) {
        fr->fwsyn[p] = fr->fwsyn[p] - syn;
        if(fr->faf[p][0]) vfrase_mec(p,syn,&e1,&e2,&e3,&e4);  /* top code */
        else {
            if(fr->fwsyn[p] > 0 && fr->fract[p]) {
                fr->csyn[p] += fr->fwsyn[p];
                if(fr->csyn[p] > 0x7fffffff) {
                    mg->errcnt++;
                    mg->msglvl = 3;
                    if(mg->msglvl >= mg->prtlvl)
                    printf("Error! velocity frase sync\n");
                    fr->csyn[p] = 0;
                    fr->fract[p] = 0;
                }
                vfrase_mec(p,syn,&e1,&e2,&e3,&e4);
            }
        }
        fr->fwsyn[p] = syn;
    }
    return(0);
}

/* Velocity frase code out */
int vfrase_mec(int p,long syn,union meccode *e1,union meccode *e2,union meccode *e3,union meccode *e4)
//int  p;
//long syn;
//union meccode *e1,*e2,*e3,*e4;
{
    int  i;
    long syncw;
    union meccode fc;

    if(fr->faf[p][0]) {
        putfrcd_mec(p,0,e1,e2,e3,e4);
        fr->faf[p][0] = 0;
        fr->fd[p][0] = 0;
        fr->ed[p][0] = 0;
        fr->frnv[p][0] = 0;
        fr->fsyn[p][0] = 0;
	fr->fxtyp[p] = 0;
	fr->fxval[p] = 0;
    }
    for(i = 1; i < fr->frp[p]; i++) {
        if(mg->debug == 6) {
            printf("Debug! fr->csyn[%d]=%ld fr->frnv[%d][%d]=%ld",
                p,fr->csyn[p],p,i,fr->frnv[p][i]);
            printf(" fr->fsyn[%d][%d]=%ld\n",p,i,fr->fsyn[p][i]);
        }
        if(fr->faf[p][i] && (fr->csyn[p] >= fr->fsyn[p][i])) {
            /* printf("**%s\n",mg->next); */
            if(mg->next[0] == '%' || mg->next[0] == 'R' ||
                mg->next[0] == 'r') {
                getnval_mec(mg->next); /* fnval */
                syncw = syn - fr->frnv[p][i];
                if(i > 1) {
                    if(mg->debug == 6) {
                        printf("Debug! syncw=%ld mr->sync=%ld",
                            syncw,mr->sync);
                        printf(" syn=%ld fnval=%ld\n",syn,fnval);
                    }
                    if(syncw > mr->sync && syncw < syn) {
                        syncout_mec(syncw,1); /* lag */
                    }
                }
            }
            putfrcd_mec(p,i,e1,e2,e3,e4);
            fr->faf[p][i] = 0;
            fr->fd[p][i] = 0;
            fr->ed[p][i] = 0;
            fr->frnv[p][i] = 0;
            fr->fsyn[p][i] = 0;
        }
    }
    fr->fract[p] = 0;
    for(i = 0; i < fr->frp[p]; i++) if(fr->faf[p][i]) fr->fract[p] = 1;
    if(fr->fract[p] == 0) {
        fr->frp[p] = 0;
        fr->csyn[p] = 0;  /* clear cullent sync value */
        fr->fsmax[p] = 0;
        fr->fsyn[p][0] = 0;
	fr->fxtyp[p] = 0;
	fr->fxval[p] = 0;
    }
    return(0);
}

/*
    Frase data 1   : E1xyzzzz
     x             : reserve(frase type)
     y             : channel
     zzzz(fsval)   : frase start value
    Frase data 2   : E2xxyyyy
     xx(fcno)      : frase control number
     yyyy(feval)   : frase end value
    Frase data 3   : E3xxxxxx
     xxxxxx(fnval) : frase note value
*/
int putfrcd_mec(int p,int frp,union meccode *e1,union meccode *e2,union meccode *e3,union meccode *e4)
//int  p;
//int  frp;
//union meccode *e1,*e2,*e3,*e4;
{
    int  i,m;
    int  fsval;
    int  feval;
    union {
        long cval;
        short cvai[2];
    } w;

    fsval = fr->fd[p][frp];
    feval = fr->ed[p][frp];
    e1->zi[0] = 0xe100;
    e1->zi[1] = 0;
    e1->zi[0] |= (0x00f0 & (p << 4));
    mr->chan &= 0x000f;
    e1->zi[0] |= mr->chan;
    /* if(fsval > 127) fsval = 127; */
    e1->zi[1] = fsval & 0x3fff;
    if(mg->trace) {
        printf("Trace! E1(%04x%04x) frase code 1 ftype=%d chan=%d ",
            e1->zi[0],e1->zi[1],p,mr->chan);
        printf("fsval=%d\n",fsval);
    }
    putcode_mec(e1);
    /* if(feval > 127) feval = 127; */
    e2->zi[0] = 0xe200;
    e2->zi[0] |= (0x00ff & fr->faf[p][frp]);
    e2->zi[1] = feval & 0x3fff;
    if(mg->trace) {
        printf("Trace! E2(%04x%04x) frase code 2 ",e2->zi[0],e2->zi[1]);
        printf("cntno=%d feval=%d\n",fr->faf[p][frp],feval);
    }
    putcode_mec(e2);
    w.cval = fr->frnv[p][frp];
    if(w.cval > 8388606) {
        m = w.cval / 8000000;
        for( i = 0; i < m ; i++) {
            w.cval = 8000000;
            //lsconv_h(&e3,w.cval);
            lsconv_h(e3,w.cval);  // 2025.4.7
            e3->zi[0] &= 0x00ff;
            e3->zi[0] |= 0xe300;
            if(mg->trace) {
                printf(
                "Trace! E3(%04x%04x) frase code 3 nval=%ld\n",
                    e3->zi[0],e3->zi[1],w.cval);
             }
             putcode_mec(e3);
        }
        w.cval = fr->frnv[p][frp] % 8000000;
    }
    lsconv_h(e3,w.cval);
    e3->zi[0] &= 0x00ff;
    e3->zi[0] |= 0xe300;
    if(mg->trace) {
        printf("Trace! E3(%04x%04x) frase code 3 nval=%ld\n",
            e3->zi[0],e3->zi[1],w.cval);
    }
    putcode_mec(e3);
     
    e4->zi[0] = (0x00ff & fr->fxtyp[p]);
    e4->zi[0] |= 0xe400;
    e4->zi[1] = fr->fxval[p] & 0x3fff;
    if(mg->trace) {
        printf("Trace! E4(%04x%04x) frase code 4 fxtyp=%d fxval=%d\n",
            e4->zi[0],e4->zi[1],fr->fxtyp[p],fr->fxval[p]);
    }
    putcode_mec(e4);
    return(0);
}

/* make frasing code */
int frasing_mec()
{
    int  i;
    int  bupss;
    char swk[6];

    for(i = 0 ; i < 5; i++) swk[i] = tolower(mg->card[i]);
    swk[i] = '\0';
    if(!strcmp(swk,"[lag ")) {
        lag_offset_mec();
        return(0);
    }
    if(!strcmp(swk,"[offs")) {
        lag_offset_mec();
        return(0);
    }
    bupss = 0;
    i = 0;
    while(mg->card[i]) {
        if(mg->card[i] == '_') bupss = 1;
        if(mg->card[i] == 'O') bupss = 2;
        if(mg->card[i] == 'o') bupss = 3;
        i++;
        if(i > 80) break;
    }
    if(bupss) {
        fraschk_mec();
    }
    else nbufrck_mec();
    return(0);
}

/* beautifyd frase & check
    [p<f _/3;f>pp _/))]
    [23 < 45 _/)4]
    [mp < fff o/;fff>ff o/]
    [12 < ff o/.2; ff>p O*12]
*/
int fraschk_mec()
{
    int  i,j,n;
    int  smcnt;
    int  fsval,feval;
    char wk1[82];
    char wk2[82];
    int  cntno;   /* control no */
    int  p;       /* prase table */
    int  frp;
    int  fxtyp;   /* fxtyp */

    smcnt = 0;
    for(i = 0; i < 80; i++) {
        wk2[i] = ' ';
        if(mg->card[i] == ';') smcnt++;
    }
    i = 0;
    while(mg->card[i]) {
        if(mg->card[i] == ']') break;
        i++;
        if(i > 80) return(0);
    }
    i++;
    while(mg->card[i] == ' ') i++;
    cntno = 0;    /* status off */
    p = 0;
    if(mg->card[i] == 'v' || mg->card[i] == 'V') {
        cntno = 7; /* volume */
        p = 0;
    }
    if(mg->card[i] == 'm' || mg->card[i] == 'M') {
        cntno = 1; /* modulation */
        p = 1;
    }
    if(mg->card[i] == 'p' || mg->card[i] == 'P') {
        cntno = 150; /* pitch vend */
        p = 2;
    }
    if(mg->card[i] == 'e' || mg->card[i] == 'E') {
        cntno = 11; /* Expression */
        p = 3;
    }
    if(mg->card[i] == 'c' || mg->card[i] == 'C') { /* control change */
        while(!isdigit(mg->card[i])) {
            i++;
            if(i > 80) return(0);
        }
        j = 0;
        while(isdigit(mg->card[i])) {
            wk1[j] = mg->card[i];
            i++;
            j++;
        }
        wk1[j] = '\0';
        sscanf(wk1,"%d",&cntno);
        /* printf("wk1=%s cntno=%d\n",wk1); */
        p = 4;
    }
    fxtyp = 0;
    n = 0;
    for(i = 6; i < 72; i++) {
        if(mg->card[i] == 'I' && mg->card[i+1] == 'n') {  /* Interval */
            fxtyp = 1;
            n = numdin_mec(mg->card);
            break;
        }
        if(mg->card[i] == 'N' && mg->card[i+1] == 'u') {  /* Number */
            fxtyp = 2;
            n = numdin_mec(mg->card);
            break;
        }
    }
    fr->fxtyp[p] = fxtyp;
    fr->fxval[p] = n; 
    /* printf("fxtyp = %d n = %d\n",fxtyp,n); */

    strcpy(wk1,mg->card);
    /* printf("org=%s\n",wk1); */
    n = 0;
    frp = 0;
    mg->card[0] = '\0';
    for(j = 0; j < smcnt+1 ; j++) {
        i = 0;
        while(wk1[n]) {
            if(wk1[n] == ' ' && wk1[n+1] == '_') break;
            if(wk1[n] == ' ' && wk1[n+1] == 'o') break;
            if(wk1[n] == ' ' && wk1[n+1] == 'O') break;
            if(wk1[n] == ' ' && wk1[n+1] == '|') break;
            wk2[i] = wk1[n];
            i++;
            n++;
            if(n > 80) break;
        }
        wk2[0] = ' ';
        wk2[i] = '\0';
        fsval = 0;
        feval = 0;
        bfunc_mec(wk2,&fsval,&feval);
        wk2[0] = '%';
        wk2[1] = 'x';
        wk2[2] = 'x';
        i = 3;
        while(wk1[n]) {
            if(wk1[n] == ';') break;
            if(wk1[n] == ']') break;
            wk2[i] = wk1[n];
            i++;
            n++;
            if(i > 80) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d frasing data\n",mg->line);
                return(-1);
            }
        }
        wk2[i] = ' '; /* dumy */
        wk2[++i] = '\0';
        frenget_mec(wk2);
        getnval_mec(wk2);
        if(mg->trace) {
            printf("Trace! cntno=%d fsval=%d feval=%d",cntno,fsval,feval);
            printf(" fnval=%ld splt=%d renp=%d rept=%d\n",
                fnval,splt,renp,fr->rept);
            printf(" fxtyp=%d fxval=%d\n",fr->fxtyp[p],fr->fxval[p]);
        }
        fr->fd[p][frp] = fsval;    /* frase start data set */
        fr->ed[p][frp] = feval;    /* frase end data set */
        fr->frnv[p][frp] = fnval * fr->rept;    /* sync value */
        fr->fsmax[p] += fr->frnv[p][frp];  /* max. sync valu set */
        if(fr->fsmax[p] > 0x7fff0000) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! %d frase sync over %ld\n",
                mg->line,fr->fsyn[p][frp]);
            break;
        }
        else {
            fr->fsyn[p][frp + 1] = fr->fsmax[p];
            fr->faf[p][frp] = cntno;        /* status on */
            frp++;
        }
    }
    fr->frp[p] = frp;
    fr->fract[p] = 1;  /* frase active flag on */
    return(0);
}

/* not beautifyd frase & check
   [p<f/3;f>pp///]         -> [p<f _/3;f>pp _/))]
   [23 > 45//4]            -> [23 < 45 _/)4]
   [mp < fff/h;fff>ff/h]   -> [mp < fff o/;fff>ff o/]
   [12 < ff/h.2; ff>p/w12] -> [12 < ff o/.2; ff>p O*12]
*/
int nbufrck_mec()
{
    int  i,j,k,n;
    int  smcnt;
    int  endmk;
    int  fsval,feval;
    char wk1[82];
    char wk2[82];
    char c;
    int  cntno;
    int  p;       /* prase table */
    int  frp;
    int  fxtyp;

    n = strlen(mg->card);
    if(n > 70) return(0);
    n = 0;
    k = 0;
    endmk = 0;
    smcnt = 0;
    for(i = 0; i < 80; i++) {
        wk2[i] = ' ';
        if(mg->card[i] == ';') smcnt++;
    }
    cntno = 0;    /* status off */
    p = 0;
    if(mg->card[i] == 'v' || mg->card[i] == 'V') {
        cntno = 7; /* volume */
        p = 0;
    }
    if(mg->card[i] == 'm' || mg->card[i] == 'M') {
        cntno = 1; /* modulation */
        p = 1;
    }
    if(mg->card[i] == 'p' || mg->card[i] == 'P') {
        cntno = 150; /* pitch vend */
        p = 2;
    }
    if(mg->card[i] == 'e' || mg->card[i] == 'E') {
        cntno = 11; /* Expression */
        p = 3;
    }
    if(mg->card[i] == 'c' || mg->card[i] == 'C') { /* control change */
        while(!isdigit(mg->card[i])) {
            i++;
            if(i > 80) return(0);
        }
        j = 0;
        while(isdigit(mg->card[i])) {
            wk1[j] = mg->card[i];
            i++;
            j++;
        }
        wk1[j] = '\0';
        sscanf(wk1,"%d",&cntno);
        p = 4;
    }
    n = 0;
    fxtyp = 0;
    for(i = 6; i < 72; i++) {
        if(mg->card[i] == 'i' || mg->card[i] == 'I') {  /* Interval */
            fxtyp = 1;
            n = numdin_mec(mg->card);
            break;
        }
        if(mg->card[i] == 'n' || mg->card[i] == 'N') {  /* Number */
            fxtyp = 2;
            n = numdin_mec(mg->card);
            break;
        }
    }
    fr->fxtyp[p] = fxtyp;
    fr->fxval[p] = n; 
    /* printf("fxtyp = %d n = %d\n",fxtyp,n); */

    strcpy(wk1,mg->card);
    /* printf("org=%s\n",wk1); */
    frp = 0;
    mg->card[0] = '\0';
    for(j = 0; j < smcnt+1 ; j++) {
        i = 0;
        while(wk1[n]) {
            if(wk1[n] == '/') break;
            wk2[i] = wk1[n];
            i++;
            n++;
            if(n > 80) break;
        }
        c = wk2[0];   /* save */
        wk2[0] = ' ';
        wk2[i] = '\0';
        fsval = 0;
        feval = 0;
        bfunc_mec(wk2,&fsval,&feval);
        if(fsval > 127 || feval > 127) {
            mg->wrncnt++;
            mg->msglvl = 2;
            if(mg->msglvl >= mg->prtlvl)
            printf("Worning! %06d max value is 127\n",mg->line);
        }
        wk2[0] = c;   /* back */
        strcat(mg->card,wk2);
        k += strlen(wk2);
        i = 0;
        while(wk1[n]) {
            if(wk1[n] == ';') break;
            if(wk1[n] == ']') {
                endmk = 1;
                break;
            }
            wk2[i] = wk1[n];
            i++;
            n++;
            if(i > 80) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d frasing data\n",mg->line);
                return(-1);
            }
        }
        wk2[i] = ' '; /* dumy for nbchg_mec() */
        wk2[++i] = '\0';
        nbchg_mec(wk2);
        strcat(mg->card,wk2);
        k += strlen(wk2);
        if(k > 80) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! %06d beautify length\n",mg->line);
            return(-1);
        }
        if(mg->trace) {
            printf("Trace! cntno=%d fsval=%d feval=%d",cntno,fsval,feval);
            printf(" fnval=%ld splt=%d renp=%d rept=%d\n",
                fnval,splt,renp,fr->rept);
            printf(" fxtyp=%d fxval=%d\n",fr->fxtyp[p],fr->fxval[p]);
        }
        fr->fd[p][frp] = fsval;    /* frase start value set */
        fr->ed[p][frp] = feval;    /* frase end value set */
        fr->frnv[p][frp] = fnval * fr->rept;    /* sync value */
        fr->fsmax[p] += fr->frnv[p][frp];  /* max. sync valu set */
        if(fr->fsmax[p] > 0x7fff0000) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! %d frase sync over %ld\n",
                mg->line,fr->fsyn[p][frp]);
            break;
        }
        else {
            fr->fsyn[p][frp + 1] = fr->fsmax[p];
            fr->faf[p][frp] = cntno;             /* status on */
            frp++;
        }
        if(endmk) {
            mg->card[k] = ']';
            mg->card[++k] = '\0';
            break;
        }
    }
    fr->frp[p] = frp;
    fr->fract[p] = 1;  /* frase active flag on */
    return(0);
}

int nbchg_mec(char wk[82])
//char wk[82];
{
    int  i;
    int  j;
    char c;
    char wcard[82];

    i = 0;
    j = 0;
    for(i = 0; i < 80 ; i++) {
        if(wk[i] <= 0x1f) wk[i] = ' ';
    }
    wk[i] = '\0';
    j = 0;
    c = wk[1];
    if(c == '/' || c == ' ' || c == '.' || c == '*' || c == '[') {
        while(wk[j] != ' ') j++;
        for(i = j ; i >= 0 ; i--) wk[i+5] = wk[i];
        for(i = 6 ; i <= j+4 ; i++) {
            if(wk[i] == '/') wk[i] = ')';
        }
        wk[4] = '_';
    }
    else {
        j = 0;
        if(c == 'h') {
            while(wk[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) wk[i+4] = wk[i];
            wk[4] = 'o';
            wk[5] = '/';
        }
        if(c == 'w') {
            while(wk[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) wk[i+3] = wk[i];
            wk[3] = ' ';
            wk[4] = 'O';
        }
        if(c == 'b') {
            while(wk[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) wk[i+3] = wk[i];
            wk[3] = ' ';
            wk[4] = '|';
            wk[5] = 'O';
            wk[6] = '|';
        }
        if(c == '[' || isdigit(c)) {
            j = 0;
            while(wk[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) wk[i+5] = wk[i];
            wk[4] = '_';
        }
    }
    frenget_mec(wk);
    getnval_mec(wk);
    wcard[0] = ' ';
    j = 1;
    i = 4;
    while(wk[i]) {
       wcard[j] = wk[i];
       if(j >= 80) break;
       if(wk[i] == ' ' && i > 4) break;
       i++;
       j++;
    }
    wcard[j] = '\0';
    strcpy(wk,wcard);
    return(0);
}

int bfunc_mec(char s[82],int *fsval,int *feval)
//char s[82];
//int  *fsval,*feval;
{
    int  i,j;
    int  p,q;
    char wk[40];
    int  af,df;
    int  pv,fv,mv;

    q = 0;
    i = 0;
    while(s[i]) {
        if(s[i] == '\0') break;
        if(s[i] == '\n') break;
        if(s[i] == '<') q = 1;
        if(s[i] == '>') q = 2;
        if(q) break;
        i++;
    }
    i = 1;
    j = 0;

    if(q) {
        while(s[i]) {
            if(s[i] == '\n') break;
            if(s[i] == '<' || s[i] == '>') break;
            wk[j] = s[i];
            i++;
            j++;
        }
    }
    wk[j] = '\0';
    i = 0;
    df = 0;
    af = 0;
    while(wk[i]) {
       if(isdigit(wk[i])) df = 1;
       if(isalpha(wk[i])) af = 1;
       i++;
    }
    if(df && af) return(-1);
    if(df) sscanf(wk,"%d",fsval);
    /* printf("wk=%s fsval=%d\n",wk,*fsval); */
    if(af) {
        i = 0;
        pv = 0;
        fv = 0;
        mv = 0;
        while(wk[i]) {
            switch(wk[i]) {
	    case 'p' : pv++;
                       break;
            case 'P' : pv++;
                       break;
            case 'f' : fv++;
                       break;
            case 'F' : fv++;
                       break;
            case 'm' : mv++;
                       break;
            case 'M' : mv++;
                       break;
            default  : break;
            }
            i++;
        }
        if(mv && (pv > 1)) return(-1);
        if(mv && (fv > 1)) return(-1);
        /*
        *     ppp  (1)  20
        *      pp  (2)  45
        *       p  (3)  65
        *      mp  (4)  80
        *          (5)  88
        *      mf  (6)  95
        *       f  (7) 108
        *      ff  (8) 118
        *     fff  (9) 127
        */
        *fsval = 88;
        if(pv == 3) *fsval = 20;
        if(pv == 2) *fsval = 45;
        if(pv == 1) *fsval = 65;
        if(mv && pv) *fsval = 80;
        if(mv & fv) *fsval = 95;
        if(fv == 1) *fsval = 108;
        if(fv == 2) *fsval = 118;
        if(fv == 3) *fsval = 127;
    }
    p = 0;
    if(q) {
        j = 0;
        i = 2;
        while((s[i] != '<') && (s[i] != '>')) i++;
        i++;
        while(s[i]) {
            if(s[i] == '\0') break;
            if(s[i] == '\n') break;
            if(s[i] == '/') p = 1;
            if(s[i] == '_') p = 2;
            if(s[i] == ')') p = 3;
            if(p) break;
            wk[j] = s[i];
            j++;
            i++;
        }
    }
    wk[j] = '\0';
    df = 0;
    af = 0;
    i = 0;
    while(wk[i]) {
        if(isdigit(wk[i])) df = 1;
        if(isalpha(wk[i])) af = 1;
        i++;
    }
    if(df && af) return(-1);
    if(df) sscanf(wk,"%d",feval);
    /* printf("wk=%s feval=%d\n",wk,*feval); */
    if(af) {
        i = 0;
        pv = 0;
        fv = 0;
        mv = 0;
        while(wk[i]) {
            switch(wk[i]) {
	    case 'p' : pv++;
                       break;
            case 'P' : pv++;
                       break;
            case 'f' : fv++;
                       break;
            case 'F' : fv++;
                       break;
            case 'm' : mv++;
                       break;
            case 'M' : mv++;
                       break;
            default  : break;
            }
            i++;
        }
        if(mv && (pv > 1)) return(-1);
        if(mv && (fv > 1)) return(-1);
        *feval = 88;
        if(pv == 3) *feval = 20;
        if(pv == 2) *feval = 45;
        if(pv == 1) *feval = 65;
        if(mv && pv) *feval = 80;
        if(mv & fv) *feval = 95;
        if(fv == 1) *feval = 108;
        if(fv == 2) *feval = 118;
        if(fv == 3) *feval = 127;
    }
    // add 2013.10.23
    if(*fsval > 127) *fsval = 127;
    if(*feval > 127) *feval = 127;
    if(*fsval < 0 ) *fsval = 0;
    if(*feval < 0 ) *feval = 0;
    // -------------
    if((q == 1 && (*fsval > *feval)) ||
       (q == 2 && (*fsval < *feval))) {
        i = 0;
        while(s[i]) {
            if(s[i] == '\0') break;
            if(s[i] == '\n') break;
            if(s[i] == '<') {
                s[i] = '>';
                break;
            }
            if(s[i] == '>') {
                s[i] = '<';
                break;
            }
            i++;
        }
    }
    return(0);
}

int getnval_mec(char wkcd[82])
//char wkcd[82];
{
    int  i,j,k;
    int  l;
    long ll;
    int  pasf;
    char wk[10];

    pasf = 0;
    l = 0;
    if(wkcd[4] == '_') {
        i = 6;
        fnval = 100000;
        while(wkcd[i] == ')') {
            i++;
            l++;
            if(l > 5) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d _/))))) is max.\n",mg->line);
                return(1);
            }
            fnval = fnval / 2;
        }
        if(wkcd[i] == '.') {
            if(l == 5) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d _/))))). is limit over\n",mg->line);
                return(1);
            }
            ll = fnval / 2;
            fnval = ll + fnval;
            i++;
            if(wkcd[i] == '.') {
                if(l == 4) {
                    mg->wrncnt++;
                    mg->msglvl = 2;
                    if(mg->msglvl >= mg->prtlvl)
                    printf("Worning! %06d _/)))).. is limit over\n",mg->line);
                    return(1);
                }
                fnval = ll/2 + fnval;
                i++;
            }
        }
        pasf = 1;
    }
    if(wkcd[4] == 'O') {
        i = 5;
        fnval = 400000;
        if(wkcd[i] == '.') {
            ll = fnval / 2;
            fnval = ll + fnval;
            i++;
            if(wkcd[i] == '.') {
                fnval = ll/2 + fnval;
                i++;
            }
        }
        pasf = 1;
    }
    if(wkcd[4] == 'o') {
        i = 6;
        fnval = 200000;
        if(wkcd[i] == '.') {
            ll = fnval / 2;
            fnval = ll + fnval;
            i++;
            if(wkcd[i] == '.') {
                fnval = ll/2 + fnval;
                i++;
            }
        }
        pasf = 1;
    }
    if(wkcd[4] == '|') {
        i = 7;
        fnval = 800000;
        if(wkcd[i] == '.') {
            ll = fnval/2;
            fnval = ll/2 + fnval;
            i++;
            if(wkcd[i] == '.') {
                fnval = ll/2 + fnval;
                i++;
            }
        }
        pasf = 1;
    }
    if(wkcd[4] == '.') {
        i = 6;
        fnval = 50000;
        while(wkcd[i] == ')' || wkcd[i] == '>') {
            i++;
            l++;
            if(l > 3) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d ./>>> is max.\n",mg->line);
                return(1);
            }
            fnval = fnval / 2;
        }
        if(wkcd[i] == '.') {
            if(l == 3) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d ./>>>. limit over\n",mg->line);
                return(1);
            }
            ll = fnval / 2;
            fnval = ll + fnval;
            i++;
            if(wkcd[i] == '.') {
                if(l == 2) {
                    mg->wrncnt++;
                    mg->msglvl = 2;
                    if(mg->msglvl >= mg->prtlvl)
                    printf("Worning! %06d ./>>.. limit over\n",mg->line);
                    return(1);
                }
                fnval = ll/2 + fnval;
                i++;
            }
        }
        pasf = 1;
    }
    if(wkcd[4] == '[') pasf = 1;
    else {
        splt = 1;
        renp = 1;
    }
    if(!pasf) return(1);
    l = 4;
    while(wkcd[l] != ' ') l++;
    j = 0;
    while(isdigit(wkcd[i])) wk[j++] = wkcd[i++];
    wk[j] = '\0';
    if(j && i <= l) sscanf(wk,"%d",&splt);
    if(wkcd[i] == '[') {
        j = 0;
        i++;
        while(isdigit(wkcd[i])) wk[j++] = wkcd[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&renp);
    }
    ll = fnval;
    if(wkcd[4] != '[') {
        k = splt * renp;
        fnval = ll / k;
    }
    return(0);
}

int frenget_mec(char card[82])
//char card[82];
{
    int  i,j;
    int  renp;
    int  rept;
    char wk[4];

    i = 4;
    renp = 0;
    rept = 0;
    fr->rept = 1;
    while(card[i]) {
        if(card[i] == '[') renp = 1;
        if(card[i] == '*') rept = 1;
        i++;
        if(i > 9) break;
    }
    if(renp) {
        i = 0;
        while(card[i] != '[') i++;
        j = 0;
        i++;
        while(isdigit(card[i])) wk[j++] = card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&fr->renp);
    }
    if(rept) {
        i = 0;
        while(card[i] != '*') i++;
        j = 0;
        i++;
        while(isdigit(card[i])) wk[j++] = card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&fr->rept);
    }
    return(0);
}

/* {;zxx} phrase */
int efdyna_mec()
{
    char wk[8];
    int  i;
    int  dyn;
    char c;

    c = mg->card[2];
    if(isdigit(c) || c == 'm' || c == 'p' || c == 'f') {
        efdyna2_mec();
        return(0);
    }
    dyn = 5;
    i = 3;
    while(1) {
        c = mg->card[i];
        if(c != 'm' && c != 'p' && c != 'f' && c != '}') dyn = 99;
        i++;
        if(i > 5) break;
    }
    c = mg->card[3];
    if(isdigit(c)) dyn = c - '0';
    if(dyn == 99) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
            printf("Error! %06d Dynamic phrase\n",mg->line);
        return(-1);
    }
    if(!isdigit(c)) {
        if(c == 'p') dyn = 3;
        if(mg->card[4] == 'p') dyn = 2;
        if(mg->card[5] == 'p') dyn = 1;
        if(mg->card[3] == 'm') {
            if(mg->card[4] == 'p') dyn = 4;
            if(mg->card[4] == 'f') dyn = 6;
        }
        if(mg->card[3] == 'f') dyn = 7;
        if(mg->card[4] == 'f') dyn = 8;
        if(mg->card[5] == 'f') dyn = 9;
    }
    switch(mg->card[2]) {
        case 't' : fr->tfdyn = dyn;
                   strcpy(wk,"tril");
                   break;
        case 'T' : fr->Tfdyn = dyn;
                   strcpy(wk,"Tril");
                   break;
        case 's' : fr->sfdyn = dyn;
                   strcpy(wk,"turn");
                   break;
        case 'S' : fr->Sfdyn = dyn;
                   strcpy(wk,"Turn");
                   break;
        case 'w' : fr->wfdyn = dyn;
                   strcpy(wk,"wtril");
                   break;
        case 'W' : fr->Wfdyn = dyn;
                   strcpy(wk,"Wtril");
                   break;
        case 'm' : fr->mfdyn = dyn;
                   strcpy(wk,"mordent");
                   break;
        case 'M' : fr->Mfdyn = dyn;
                   strcpy(wk,"Mordent");
                   break;
        case 'r' : fr->rfdyn = dyn;
                   strcpy(wk,"tr...");
                   break;
        case 'R' : fr->Rfdyn = dyn;
                   strcpy(wk,"Tr...");
                   break;
        default  : break;
    }
    if(mg->trace) printf("Trace! %s dyna=%d\n",wk,dyn);
    return(0);
}

/* {;xx} phrase */
int efdyna2_mec()
{
    int  i;
    int  dyn;
    char c;

    dyn = 5;
    i = 2;
    while(1) {
        c = mg->card[i];
        if(c != 'm' && c != 'p' && c != 'f' && c != '}') dyn = 99;
        i++;
        if(i > 4) break;
    }
    c = mg->card[2];
    if(isdigit(c)) dyn = c - '0';
    if(dyn == 99) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
            printf("Error! %06d Dynamic phrase\n",mg->line);
        return(-1);
    }
    if(!isdigit(c)) {
        if(c == 'p') dyn = 3;
        if(mg->card[3] == 'p') dyn = 2;
        if(mg->card[4] == 'p') dyn = 1;
        if(mg->card[2] == 'm') {
            if(mg->card[3] == 'p') dyn = 4;
            if(mg->card[3] == 'f') dyn = 6;
        }
        if(mg->card[2] == 'f') dyn = 7;
        if(mg->card[3] == 'f') dyn = 8;
        if(mg->card[4] == 'f') dyn = 9;
    }
    fr->tfdyn = dyn;
    fr->Tfdyn = dyn;
    fr->sfdyn = dyn;
    fr->Sfdyn = dyn;
    fr->wfdyn = dyn;
    fr->Wfdyn = dyn;
    fr->mfdyn = dyn;
    fr->Mfdyn = dyn;
    fr->rfdyn = dyn;
    fr->Rfdyn = dyn;
    if(mg->trace) printf("Trace! efect dyna=%d\n",dyn);
    return(0);
}

