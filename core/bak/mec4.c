
// ----------------------
//  AMuPLC (mec4.c)
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
extern MECMPV *mp;
extern FILE  *fpi;
extern char scsv[MAXSC][82];      /* card save area */
extern int  scsn;                 /* card save number count */
extern long getswing_mec();

int midic_mec()
{
    union meccode f6;
    int  i,j,k;
    char w[4];
    unsigned int x[2];
    int  m;
    unsigned int n;

    i = 1;
    j = 1;
    k = 0;
    m = 0;
    f6.zi[0] = 0xf602;
    f6.zi[1] = 0;
    while(mg->card[i]) {
        if(mg->card[i] == '>') break;
        if(mg->card[i] != ' ' && mg->card[i] != ',') {
            w[k] = mg->card[i];
            if((k = j % 2) == 0 && j > 0) {
                w[2] = '\0';
                sscanf(w,"%x",&x[m]);
                m++;
                if((m % 2) == 0 && m > 0) {
                    n = x[0];
                    n = n << 8;
                    n &= 0xff00;
                    f6.zi[1] = 0;
                    f6.zi[1] |= n;
                    n = 0x00ff & x[1];
                    f6.zi[1] |= n;
                    if(mg->trace) {
                        printf("Trace! F6(%04x%04x) 02 %02x %02x\n",
                            f6.zi[0],f6.zi[1],x[0],x[1]);
                    }
                    putcode_mec(&f6);
                    x[0] = 0;
                    x[1] = 0;
                    m = 0;
                }
            }
            j++;
        }
        i++;
    }
    f6.zi[1] = 0;
    if(m == 1) {
        f6.zi[0] = 0xf601;
        n = x[0] << 8;
        n &= 0xff00;
        f6.zi[1] = 0;
        f6.zi[1] |= n;
        if(mg->trace) {
            printf("Trace! F6(%04x%04x) 01 %02x %02x\n",
                f6.zi[0],f6.zi[1],x[0],x[1]);
        }
        putcode_mec(&f6);
    }
    /*
    else {
        f6.zi[0] = 0xf600;
        f6.zi[1] = 0;
        if(mg->trace) {
            printf("Trace! F6(%04x%04x) 00 %02x %02x\n",
                f6.zi[0],f6.zi[1],x[0],x[1]);
            }
        putcode_mec(&f6);
    }
    */
    return(0);
}

int spacecard_mec()
{
    int  i,k;
    int  wfg;
    char wk[80];

    wfg = 0;
    k = 0;
    for(i = 0; i < 79; i++) {
        if(mg->card[i] != ' ') {
            wk[k++] = mg->card[i];
            wfg = 1;
        }
        else {
            if(wfg) wk[k++] = ' ';
            wfg = 0;
        }
    }
    wk[k] = '\0';
    strcpy(mg->card,wk);
    return(0);
}

int spacecmp_mec(char *card,int len)
//char *card;
//int  len;
{   
    int  i,k;
    int  wfg;
    char wk[80];
    
    wfg = 0; 
    k = 0;
    /* printf("card = %s\n",card); */
    for(i = 0; i < len; i++) {
        if(card[i] != ' ') {
            wk[k++] = card[i];
            wfg = 1;
            if(card[i] == '+') wfg = 0;
            if(card[i] == '-') wfg = 0;
            if(card[i] == '_') wfg = 0;
        }       
        else {  
            if(wfg) wk[k++] = ' ';
            wfg = 0;
        }
    }
    if(isdigit(wk[k-1])) wk[k++] = ' ';
    wk[k] = '\0';
    strcpy(card,wk);
    return(0);
}

int mkyxfnum_h(char fname[STLN],int *yfile,int *xfile,char *ps)
//char fname[STLN];
//int  *yfile;
//int  *xfile;
//char *ps;
{
    char *pf;
    char w[4];
    int  i;

    pf = fname;
    while(*(pf++)) if(*pf == '_') break;
    pf++;
    for(i = 0; i < 36; i++,ps++) if(*pf == *ps) break;
    *yfile = i;
    /* getdigit_h(*pf,yfile); */
    pf++;
    w[0] = *pf;
    pf++;
    w[1] = *pf;
    w[2] = '\0';
    sscanf(w,"%x",xfile);   /* %d v_1.14 */
    return(0);
}

int filemake_h(int n,char inf[STLN],char fext[STLN],char dfile[STLN],int *yflp,int *xflp)
//int  n;
//char inf[STLN];
//char fext[STLN];
//char dfile[STLN];
//int  *yflp;
//int  *xflp;
{
    char *pc;
    char wfnum[8];
    char wk[8];
    // unsigned char wfnum[8];
    // unsigned char wk[8];
    int  i;
    int  trk;
    int  memb;

    trk = n/1000;  /* n/100 v_1.14 */
    memb = n - trk * 1000;  /* n - trk * 100 v_1.14 */
    *yflp = trk;
    *xflp = memb;
    pc = "0123456789abcdefghijklmnopqrstuvwxyz";
    for(i = 0; i < trk ; i++) pc++;
    wfnum[0] = *pc;
    wfnum[1] = '\0';
    sprintf(wk,"%02x",memb);  /* %02d v_1.14 */
    strcat(wfnum,wk);
    dfile[0] = '\0';
    strcat(dfile,inf);
    strcat(dfile,"_");
    strcat(dfile,wfnum);
    strcat(dfile,fext);
    return(0);
}

int tril_mec()
{
    int  i,j,k;
    int  lastp;

    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/3;
    else mr->wval = mr->cval/3;
    /* tril */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 't') {
            if(mg->quep == 7) {
                mr->keyp = mg->ktb[k];
                fd9code_mec();
            }
            else {
                mr->keyp = mg->ktb[k+1];
                fd9code_mec();
            }
        }
        if(mg->temp[i] == 'T') {
            if(mg->quep == 7) {
                mr->keyp = mg->ktb[k+1];
                fd9code_mec();
            }
            else {
                mr->keyp = mg->ktb[k];
                fd9code_mec();
            }
        }
        k++;
    }
    quecode2_mec();
    trilon_mec();
    return(0);
}

int trilon_mec()
{
    int  i,j,k;
    int  lastp;

    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/3;
    else mr->wval = mr->cval/3;
    /* tril */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 't') {
            mr->dyna = fr->tfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            if(mg->quep == 7) {
                mr->keyp = mg->ktb[k+1];
                fdcode_mec();
            }
            else {
                mr->keyp = mg->ktb[k];
                fdcode_mec();
                if(mg->quep == 15) mg->enoftb[mr->keyp]++;
            }
        }
        if(mg->temp[i] == 'T') {
            mr->dyna = fr->Tfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            if(mg->quep == 7) {
                mr->keyp = mg->ktb[k];
                fdcode_mec();
            }
            else {
                mr->keyp = mg->ktb[k+1];
                fdcode_mec();
                if(mg->quep == 15) mg->enoftb[mr->keyp]++;
            }
        }
        k++;
    }
    return(0);
}

int turn_mec()
{
    int  i,j,k;
    int  lastp;

    quecode_mec();
    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/4;
    else mr->wval = mr->cval/4;
    /* turn */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 's' || mg->temp[i] == 'S') {
            switch(mg->quep) {
                case  5 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                case 11 : if(mg->temp[i] == 's')
                              mr->keyp = mg->ktb[k+1];
                          if(mg->temp[i] == 'S')
                               mr->keyp = mg->ktb[k-1];
                          fd9code_mec();
                          break;
                case 17 : if(mg->temp[i] == 's')
                              mr->keyp = mg->ktb[k-1];
                          if(mg->temp[i] == 'S')
                               mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          break;
                default : break;
            }
        }
        k++;
    }
    turnon_mec();
    return(0);
}

int turnon_mec()
{
    int  i,j,k;
    int  lastp;

    quecode2_mec();
    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/4;
    else mr->wval = mr->cval/4;
    /* turn */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 's' || mg->temp[i] == 'S') {
            if(mg->temp[i] == 's') mr->dyna = fr->sfdyn;
            if(mg->temp[i] == 'S') mr->dyna = fr->Sfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->quep) {
                case  5 : if(mg->temp[i] == 's')
                              mr->keyp = mg->ktb[k+1];
                          if(mg->temp[i] == 'S')
                               mr->keyp = mg->ktb[k-1];
                          fdcode_mec();
                          break;
                case 11 : if(mg->temp[i] == 's')
                              mr->keyp = mg->ktb[k-1];
                          if(mg->temp[i] == 'S')
                               mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case 17 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          mg->enoftb[mr->keyp]++;
                          break;
                default : break;
            }
        }
        k++;
    }
    return(0);
}

int mordent_mec()
{
    int  i,j,k;
    int  lastp;

    quecode_mec();
    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/5;
    else mr->wval = mr->cval/5;
    /* mordent */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 'm') {
            switch(mg->quep) {
                case  3 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                case  8 : mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          break;
                case 14 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                case 19 : mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          break;
                default : break;
            }
        }
        if(mg->temp[i] == 'M') {
            switch(mg->quep) {
                case  3 : mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          break;
                case  8 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                case 14 : mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          break;
                case 19 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                default : break;
            }
        }
        k++;
    }
    mordon_mec();
    return(0);
}

int mordon_mec()
{
    int  i,j,k;
    int  lastp;

    quecode2_mec();
    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/5;
    else mr->wval = mr->cval/5;
    /* mordent */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 'm') {
            mr->dyna = fr->mfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->quep) {
                case  3 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case  8 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case 14 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case 19 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          mg->enoftb[mr->keyp]++;
                          break;
                default : break;
            }
        }
        if(mg->temp[i] == 'M') {
            mr->dyna = fr->Mfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->quep) {
                case  3 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case  8 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case 14 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case 19 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          mg->enoftb[mr->keyp]++;
                          break;
                default : break;
            }
        }
        k++;
    }
    return(0);
}

int dmordent_mec()
{
    int  i,j,k;
    int  lastp;

    quecode_mec();
    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/7;
    else mr->wval = mr->cval/7;
    /* double mordent */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 'w') {
            switch(mg->quep) {
                case  2 : mr->keyp = mg->ktb[k];          /* <> */
                          fd9code_mec();
                          break;
                case  6 : mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          break;
                case  9 : mr->keyp = mg->ktb[k];          /* <> */
                          fd9code_mec();
                          break;
                case 13 : mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          break;
                case 16 : mr->keyp = mg->ktb[k];         /* <> */
                          fd9code_mec();
                          break;
                case 20 : mr->keyp = mg->ktb[k+1];
                          fd9code_mec();
                          mr->keyp = mg->ktb[k];
                          break;
                default : break;
            }
        }
        if(mg->temp[i] == 'W') {
            switch(mg->quep) {
                case  2 : mr->keyp = mg->ktb[k+1];          /* <> */
                          fd9code_mec();
                          break;
                case  6 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                case  9 : mr->keyp = mg->ktb[k+1];          /* <> */
                          fd9code_mec();
                          break;
                case 13 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                case 16 : mr->keyp = mg->ktb[k+1];         /* <> */
                          fd9code_mec();
                          break;
                case 20 : mr->keyp = mg->ktb[k];
                          fd9code_mec();
                          break;
                default : break;
            }
        }
        k++;
    }
    dmordon_mec();
    return(0);
}

int dmordon_mec()
{
    int  i,j,k;
    int  lastp;

    quecode2_mec();
    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/7;
    else mr->wval = mr->cval/7;
    /* double mordent */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 'w') {
            mr->dyna = fr->wfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->quep) {
                case  2 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case  6 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case  9 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case 13 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case 16 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case 20 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          mg->enoftb[mr->keyp]++;
                          break;
                default : break;
            }
        }
        if(mg->temp[i] == 'W') {
            mr->dyna = fr->Wfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->quep) {
                case  2 : if(mg->temp[i] == 'W')
                               mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case  6 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case  9 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case 13 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          break;
                case 16 : mr->keyp = mg->ktb[k];
                          fdcode_mec();
                          break;
                case 20 : mr->keyp = mg->ktb[k+1];
                          fdcode_mec();
                          mg->enoftb[mr->keyp]++;
                          break;
                default : break;
            }
        }
        k++;
    }
    return(0);
}

int dtril_mec()
{
    int  i,j,k;
    int  root;
    int  lastp;

    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/9;
    else mr->wval = mr->cval/9;
    /* doble tril */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 'r') {
            root = 0;
            switch(mg->quep) {
                case 1 : root = 1;
                         break;
                case 7 : root = 1;
                         break;
                case 12 : root = 1;
                          break;
                case 18 : root = 1;
                          break;
                default : root = 0;
            }
            if(root) {
                mr->keyp = mg->ktb[k];
                fd9code_mec();
            }
            else {
                mr->keyp = mg->ktb[k+1];
                fd9code_mec();
            }
        }
        if(mg->temp[i] == 'R') {
            root = 0;
            switch(mg->quep) {
                case 1 : root = 1;
                         break;
                case 7 : root = 1;
                         break;
                case 12 : root = 1;
                          break;
                case 18 : root = 1;
                          break;
                default : root = 0;
            }
            if(root) {
                mr->keyp = mg->ktb[k+1];
                fd9code_mec();
            }
            else {
                mr->keyp = mg->ktb[k];
                fd9code_mec();
            }
        }
        k++;
    }
    quecode2_mec();
    dtrilon_mec();
    return(0);
}

int dtrilon_mec()
{
    int  i,j,k;
    int  root;
    int  lastp;

    if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/9;
    else mr->wval = mr->cval/9;
    /* doble tril */
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 'r') {
            mr->dyna = fr->rfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            root = 0;
            switch(mg->quep) {
                case 1 : root = 1;
                         break;
                case 7 : root = 1;
                         break;
                case 12 : root = 1;
                          break;
                case 18 : root = 1;
                          break;
                default : root = 0;
            }
            if(root) {
                mr->keyp = mg->ktb[k+1];
                fdcode_mec();
            }
            else {
                mr->keyp = mg->ktb[k];
                fdcode_mec();
                if(mg->quep == 21) mg->enoftb[mr->keyp]++;
            }
        }
        if(mg->temp[i] == 'R') {
            mr->dyna = fr->Rfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            root = 0;
            switch(mg->quep) {
                case 1 : root = 1;
                         break;
                case 7 : root = 1;
                         break;
                case 12 : root = 1;
                          break;
                case 18 : root = 1;
                          break;
                default : root = 0;
            }
            if(root) {
                mr->keyp = mg->ktb[k];
                fdcode_mec();
            }
            else {
                mr->keyp = mg->ktb[k+1];
                fdcode_mec();
                if(mg->quep == 21) mg->enoftb[mr->keyp]++;
            }
        }
        k++;
    }
    return(0);
}

int quecode_mec()
{
    union meccode fc;
    long wl;

    /* pport hosei */
    wl = mr->wval * mr->pport/100;
    wl = mr->wval - wl;
    wl = mg->ques[mg->quep] + wl;
    syncout_mec(wl,1); /* lag */
    fraseck_mec(wl);
    return(0);
}

int quecode2_mec()
{
    union meccode fc;
    long wl;

    wl = mg->ques[mg->quep];
    syncout_mec(wl,1); /* lag */
    fraseck_mec(wl);
    return(0);
}

/* syncesizer part pitch vend message
-s0100001 p.v. mod cn:cdt pcg tp
%68 _/)   xxxx   0  0:  0   0  0
%68 _/)))[2  x   0  0:  0   0  0
*/
int syncepv_mec()
{
    union meccode fd;
    char wk[8];
    int  j;
    int  msb;
    int  lsb;
    int  pass;

    /*
    wk[0] = mg->card[9];
    wk[1] = mg->card[10];
    wk[2] = mg->card[11];
    wk[3] = mg->card[12];
    wk[4] = mg->card[13];
    wk[5] = '\0';
    */
    j = 4;
    while(j >= 0) {
        if(mg->card[j+9] == ' ') {
            while(j >= 0) {
                wk[j] = ' ';
                j--;
            }
            break;
        }
        else wk[j] = mg->card[j+9];
        j--;
    }
    wk[5] = '\0';
    sscanf(wk,"%d",&j);
    /* ntf("wk=%s j=%d\n",wk,j); */
    pass = 0;
    if(wk[3] == '0' && wk[4] == '0') pass = 1;
    if(j != mr->pitv)  pass = 1;
    if(j > 63 || j < -64) {
        pass = 0;
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %05d pitch_vend %d\n",mr->bnum,j);
        if(j > 63 && j < 128) {
            pass = 1;
            j = 63; // pass
        }
        if(j < 0 && j > -128) {
            pass = 1;
            j = -64; // pass
        }
    }
    if(pass) {
        mr->pitv = j;
        fd.zi[0] = 0xfde0;
        fd.zi[1] = 0;
        mr->chan &= 0x000f;
        fd.zi[0] |= mr->chan;
        /*  Comment out 2013.10.21  LSB = 0 , MSB -64 - 0 63 
        lsb = mr->pitv + 8192;
        lsb &= 0x007f;
        j = lsb << 7;
        j &= 0x7f00;
        fd.zi[1] |= j;
        msb = mr->pitv + 8192;
        msb = msb >> 7;
        msb &= 0x007f;
        fd.zi[1] |= msb;
        */
        lsb = 0;
        msb = mr->pitv + 64;
        msb &= 0x007f;
        fd.zi[1] |= msb;
        if(!mp->mpvf) {
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) pitch vend chan=%d lsb=%d msb=%d",
                    fd.zi[0],fd.zi[1],mr->chan,lsb,msb);
                printf(" mr->pitv=%d\n",mr->pitv);
            }
            putcode_mec(&fd);
        }
    }
    return(0);
}

/* syncesizer part modulation & control change message
-s0100001 p.v. mod cn:cdt pcg tp
%68 _/)      0 xxx xx:xxx   0  0
*/
int syncecnt_mec()
{
    union meccode fd;
    char wk[8];
    int  contnmb;
    int  j;
    int  pass;

    /* modulation */
    if(isdigit(mg->card[17])) {
        wk[0] = mg->card[15];
        wk[1] = mg->card[16];
        wk[2] = mg->card[17];
        wk[3] = '\0';
        sscanf(wk,"%d",&j);
        pass = 0;
        if(wk[1] == '0' && wk[2] == '0') pass = 1;
        if(j != mr->mitv || isdigit(mg->card[16])) pass = 1;
        if(j > 127 || j < 0) {
            pass = 0;
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! %05d Moduration %d\n",mr->bnum,j);
        }
        if(pass) { 
            mr->mitv = j;
            /* printf("wk=%s mr->mitv=%d\n",wk,mr->mitv); */
            fd.zi[0] = 0xfdb0;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            contnmb = 1;            /* modulation */
            contnmb &= 0x007f;
            j = contnmb << 8;
            fd.zi[1] |= j;
            mr->mitv &= 0x007f;
            fd.zi[1] |= mr->mitv;
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) modulation chan=%d mr->mitv=%d\n",
                fd.zi[0],fd.zi[1],mr->chan,mr->mitv);
            }
            putcode_mec(&fd);
        }
    }
    /* control change */
    /* if(mg->card[20] == '0' && mg->card[24] == '0') return(0); */
    if(isdigit(mg->card[20])) {
        wk[0] = mg->card[19];
        wk[1] = mg->card[20];
        wk[2] = '\0';
        sscanf(wk,"%d",&contnmb);
        if(contnmb > 127 || contnmb < 0) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! %06d control number %d\n",mg->line,contnmb);
            return(0);
        }
        /* printf("wk=%s contnmb=%d\n",wk,contnmb); */
        wk[0] = mg->card[22];
        wk[1] = mg->card[23];
        wk[2] = mg->card[24];
        wk[3] = '\0';
        sscanf(wk,"%d",&j);
        if(j != mr->citv || isdigit(mg->card[19]) || isdigit(mg->card[23])) {
            mr->citv = j;
            /* printf("wk=%s mr->citv=%d\n",wk,mr->citv); */
            fd.zi[0] = 0xfdb0;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            contnmb &= 0x007f;
            j = contnmb << 8;
            fd.zi[1] |= j;
            mr->citv &= 0x007f;
            fd.zi[1] |= mr->citv;
            if(mg->trace) {
                printf(
                  "Trace! FD(%04x%04x) control change chan=%d cn=%d val=%d\n",
                  fd.zi[0],fd.zi[1],mr->chan,contnmb,mr->citv);
            }
            putcode_mec(&fd);
        }
    }
    return(0);
}
/* syncesizer part program change message
-s0100001 p.v. mod cn:cdt pcg tp
%68 _/)      0   0  0:  0 ...  0
*/
int syncepcg_mec()
{
    union meccode fd;
    char wk[8];
    int  j;

    /* program change */
    if(!isdigit(mg->card[28])) return(0); /* chg 2000.6 */
    if(mg->card[28] != '.') {
        wk[0] = mg->card[26];
        wk[1] = mg->card[27];
        wk[2] = mg->card[28];
        wk[3] = '\0';
        sscanf(wk,"%d",&j);
        /* mr->pgiv = j; Comment out 2005.9.9 */
        mr->pgiv = j - 1;
        if(mr->pgiv < 0 || mr->pgiv > 128) {
            printf("Error! %06d Program change data %d\n",mg->line,mr->pgiv);
            return(-1);
        }
        /* printf("wk=%s mr->pgiv=%d\n",wk,mr->pgiv); */
        fd.zi[0] = 0xfdc0;
        fd.zi[1] = 0;
        mr->chan &= 0x000f;
        fd.zi[0] |= mr->chan;
        mr->pgiv &= 0x007f;
        j = mr->pgiv << 8;
        fd.zi[1] |= j;
        fd.zi[1] |= 0x00ff; /* 0xff is system rest must ignore at mlk prog. */
        if(mg->trace) {
            printf("Trace! FD(%04x%04x) program change chan=%d mr->pgiv=%d",
                fd.zi[0],fd.zi[1],mr->chan,mr->pgiv);
            printf(" dumy_code=0xff\n");
        }
        putcode_mec(&fd);
    }
    return(0);
}

/* transpose 
-s0100001 p.v. mod cn:cdt mod tp
%68 _/)      0   0  0:  0   0xxx
*/
int syncetrs_mec()
{
    char wk[8];

    /* get transpose value */
    if(mg->card[31] == 'x' || mg->card[31] == 'X' || mg->card[31] == ' ')
        return(0);
    wk[0] = mg->card[29];
    wk[1] = mg->card[30];
    wk[2] = mg->card[31];
    wk[3] = '\0';
    sscanf(wk,"%d",&mr->trnsv);
    if(mg->trace) {
        printf("Trace! mr->trnsv=%d\n",mr->trnsv);
    }
    return(0);
}

/* pedal check
-s0100001 p.v. mod cn:cdt pcg tp1______2______*______4______5______6 velo pedal
%68 _/       0   0  0:  0 ...  0              5                      12.8 @_...
*/
int pedalck_mec()
{
    int  cnb;
    int  cdt;

    /* for(i = 74; i < 79; i++) { */
    switch (mg->card[74]) {
        case '@' : /* sustain f sw(dumper) on */
                   cnb = 64;
                   cdt = 127;
                   pdalout_mec(cnb,cdt);
                   break;
        case '*' : /* sustain f sw(dumper) off */
                   cnb = 64;
                   cdt = 0;
                   pdalout_mec(cnb,cdt);
                   break;
        case '#' : /* portment f sw(dumper) on */
                   cnb = 65;
                   cdt = 127;
                   pdalout_mec(cnb,cdt);
                   break;
        case '$' : /* portament f sw(dumper) off */
                   cnb = 65;
                   cdt = 0;
                   pdalout_mec(cnb,cdt);
                   break;
        case '+' : /* soft f sw on */
                   cnb = 67;
                   cdt = 127;
                   pdalout_mec(cnb,cdt);
                   break;
        case '-' : /* soft f sw off */
                   cnb = 67;
                   cdt = 0;
                   pdalout_mec(cnb,cdt);
                   break;
        default  : break;
    }
    return(0);
}

/* after pedal check
-s0100001 p.v. mod cn:cdt pcg tp1______2______*______4______5______6 velo pedal
%68 _/       0   0  0:  0 ...  0              5                      12.8 ._..@
*/
int apedalck_mec()
{
    int  cnb;
    int  cdt;

    /* for(i = 74; i < 79; i++) { */
    switch (mg->card[78]) {
        case '@' : /* sustain f sw(dumper) on */
                   cnb = 64;
                   cdt = 127;
                   pdalout_mec(cnb,cdt);
                   break;
        case '*' : /* sustain f sw(dumper) off */
                   cnb = 64;
                   cdt = 0;
                   pdalout_mec(cnb,cdt);
                   break;
        case '#' : /* portment f sw(dumper) on */
                   cnb = 65;
                   cdt = 127;
                   pdalout_mec(cnb,cdt);
                   break;
        case '$' : /* portament f sw(dumper) off */
                   cnb = 65;
                   cdt = 0;
                   pdalout_mec(cnb,cdt);
                   break;
        case '+' : /* soft f sw on */
                   cnb = 67;
                   cdt = 127;
                   pdalout_mec(cnb,cdt);
                   break;
        case '-' : /* soft f sw off */
                   cnb = 67;
                   cdt = 0;
                   pdalout_mec(cnb,cdt);
                   break;
        default  : break;
    }
    return(0);
}

int pdalout_mec(int cnb,int cdt)
//int  cnb;
//int  cdt;
{
    union meccode fd;
    int  j;

    fd.zi[0] = 0xfdb0;
    fd.zi[1] = 0;
    mr->chan &= 0x000f;
    fd.zi[0] |= mr->chan;
    cnb &= 0x007f;
    j = cnb << 8;
    fd.zi[1] |= j;
    cdt &= 0x007f;
    fd.zi[1] |= cdt;
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) control change chan=%d cn=%d val=%d\n",
            fd.zi[0],fd.zi[1],mr->chan,cnb,cdt);
    }
    putcode_mec(&fd);
    return(0);
}

int dnote_mec(struct mecksn *mk,int exf,char exp[4])
//struct mecksn *mk;
//int  exf;       
//char exp[4];    
{
    union meccode fc;
    int  i,j,k;
    int  l,s;
    int  lastp;        /* avalable column */
    long ll;
    long wl;
    int  ecod;
    int  eeod;
    int  pasf;
    long nval;
    int  accia;
    char wk[10];
    int  hosei;
    long swing;
    long nacv;

    pasf = 0;
    scsn = 0;
    switch(mg->next[0]) {
        case  '%' : pasf = 1;
                    break;
        case  's' : pasf = 1;
                    break;
        case  'c' : pasf = 1;
                    break;
        case  'R' : pasf = 1;
                    break;
        case  'r' : pasf = 1;
                    break;
        case  ':' : pasf = 1;
                    break;
        case  '/' : if(mg->next[1] == '%') pasf = 1;
                    break;
        case  '|' : pasf = 1;
                    break;
        case  '=' : pasf = 1;
                    break;
        case  'f' : pasf = 1;
                    break;
        case 0x1a : pasf = 1;
                    break;
        default   : break;
    }
    if(!pasf) pasread_mec();
    if(mg->next[0] == '-') strcpy(mg->temp,mg->next2);
    else strcpy(mg->temp,mg->next);
    pasf = 0;
    l = 0;
    hosei = 0;
    accia = 0;
    for(i = 0; i < 128; i++) mg->enoftb[i] = 0;
    wk[0] = mg->card[1];
    wk[1] = mg->card[2];
    wk[2] = '\0';
    sscanf(wk,"%d",&mr->pport);
    if(mr->pport == 0) mr->pport = 100;   /* %00 = 100% */
    if(mr->pport < 0 || mr->pport > 100) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d gate time rate\n",mg->line);
    }
    if(mg->card[4] == '_') {
        i = 6;
        mr->nval = 100000;
        while(mg->card[i] == ')') {
            i++;
            l++;
            if(l > 5) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d _/))))) is max.\n",mg->line);
                return(1);
            }
            mr->nval = mr->nval / 2;
        }
        if(mg->card[i] == '.') {
            if(l == 5) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d _/))))). is limit over\n",mg->line);
                return(1);
            }
            ll = mr->nval / 2;
            mr->nval = ll + mr->nval;
            i++;
            if(mg->card[i] == '.') {
                if(l == 4) {
                    mg->wrncnt++;
                    mg->msglvl = 2;
                    if(mg->msglvl >= mg->prtlvl)
                    printf("Worning! %06d _/)))).. is limit over\n",mg->line);
                    return(1);
                }
                mr->nval = ll/2 + mr->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mg->card[4] == 'O') {
        i = 5;
        mr->nval = 400000;
        if(mg->card[i] == '.') {
            ll = mr->nval / 2;
            mr->nval = ll + mr->nval;
            i++;
            if(mg->card[i] == '.') {
                mr->nval = ll/2 + mr->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mg->card[4] == 'o') {
        i = 6;
        mr->nval = 200000;
        if(mg->card[i] == '.') {
            ll = mr->nval / 2;
            mr->nval = ll + mr->nval;
            i++;
            if(mg->card[i] == '.') {
                mr->nval = ll/2 + mr->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mg->card[4] == '|') {
        i = 7;
        mr->nval = 800000;
        if(mg->card[i] == '.') {
            ll = mr->nval/2;
            mr->nval = ll/2 + mr->nval;
            i++;
            if(mg->card[i] == '.') {
                mr->nval = ll/2 + mr->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mg->card[4] == '.') {
        i = 6;
        /* mr->nval = 50000; */
        mr->nval = 100000;
        while(mg->card[i] == ')' || mg->card[i] == '>') {
            if(mg->card[i] == '>') accia = 1;
            i++;
            l++;
            if(l > 3) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d ./>>> is max.\n",mg->line);
                return(1);
            }
            mr->nval = mr->nval / 2;
        }
        if(mg->card[i] == '.') {
            if(l == 3) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl)
                printf("Worning! %06d ./>>>. limit over\n",mg->line);
                return(1);
            }
            ll = mr->nval / 2;
            mr->nval = ll + mr->nval;
            i++;
            if(mg->card[i] == '.') {
                if(l == 2) {
                    mg->wrncnt++;
                    mg->msglvl = 2;
                    if(mg->msglvl >= mg->prtlvl)
                    printf("Worning! %06d ./>>.. limit over\n",mg->line);
                    return(1);
                }
                mr->nval = ll/2 + mr->nval;
                i++;
            }
        }
        pasf = 1;
        if(accia) mr->acci += mr->nval;
    }
    if(mg->card[4] == '[') pasf = 1;
    else {
        mr->splt = 1;
        mr->renp = 1;
    }
    if(!pasf) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d note line\n",mg->line);
        return(1);
    }
    l = 4;
    while(mg->card[l] != ' ') l++;
    j = 0;
    while(isdigit(mg->card[i])) wk[j++] = mg->card[i++];
    wk[j] = '\0';
    if(j && i <= l) sscanf(wk,"%d",&mr->splt);
    if(mg->card[i] == '[') {
        j = 0;
        i++;
        while(isdigit(mg->card[i])) wk[j++] = mg->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mr->renp);
    }
    /* ll = mr->nval; */
    swing = getswing_mec();
    if(mr->nval > swing) ll = mr->nval + swing;
    else {
        ll = mr->nval;
	printf("Error! %06d Swing over than note value %ld\n",mg->line,swing);
    }       
    if(mg->card[4] != '[') {
        k = mr->splt * mr->renp;
        mr->nval = ll / k;
        hosei = ll - (mr->nval * k);
    }
    if(mg->next[4] == '.' && mg->next[5] == '/' &&
        mg->next[6] == ')') {
        nacv = 0;
        nacv = getnacv_mec(mg->next);
        if(mg->trace) {
            printf("Trace! nacv=%ld\n",nacv);
        }       
        mr->nval = mr->nval - nacv;
    } 
    if(mg->btyp == 'r') {
        wk[0] = mg->card[11];
        wk[1] = mg->card[12];
        wk[2] = '\0';
        sscanf(wk,"%d",&mr->trnsv);
    }
    wk[0] = mg->card[69];
    wk[1] = mg->card[70];
    wk[2] = mg->card[71];
    wk[3] = mg->card[72];
    wk[4] = '\0';
    sscanf(wk,"%f",&mg->velo);
    ecod = 0;
    lastp = 69;
    j = 10;
    if( mg->btyp == 's' ||
        mg->btyp == 't' ||
        mg->btyp == 'g' ||
        mg->btyp == 'a' ||
        mg->btyp == 'b' ||
        mg->btyp == 'u' ||
        mg->btyp == 'd') j = 32;
    for(i = j; i < lastp; i++) {
        if(mg->card[i] != ' ' && mg->card[i] != ':') ecod = 1;
    }
    eeod = 0;
    if(mg->temp[0] == ':') {
        j = 14;
        if(mg->btyp == 'd') j = 32;
        for(i = j; i < 69; i++) {
            if(mg->temp[i] != ' ' && mg->temp[i] != ':') eeod = 1;
        }
    }
    if(mg->trace && ecod) {
        printf("Trace! nval=%ld splt=%d renp=%d ",
        ll,mr->splt,mr->renp);
        printf("rept=%d hosei=%d --------\n",mr->rept,hosei);
    }
    /* accid_mec(mk); */
    for(s = 0; s < mr->splt; s++) {
        /* fc code */
        nval = mr->nval;
        syncout_mec(mr->sync,1); /* lag */
        /* note */
        if(!accia) {
            nval -= mr->acci;
            mr->acci = 0;
        }
        mr->cval = nval*mr->pport/100;
        wl = mr->cval;
        mr->psyn = mr->sync;
        queset_mec(wl);
        strcpy(mg->temp,mg->card);
        dodnoff_mec();
        if(ecod) {
            dodpfkey_mec();
            if(exf) dexpchk_mec(exp,nval); 
            else dodnon_mec();
            /* dodnon_mec(); */
            /* fraseck_mec(wl); */
            /* efect_mec(); */
        }
        fraseck_mec(wl);
        /* sync */
        mr->psyn = mr->sync - mr->cval;
        mr->sync = mr->sync - nval;
        /* if(mr->sync > 0xffffff || mr->sync < 0) { org */
        if(mr->sync > 0xffffff || mr->sync < -6) { /* swing hosei */
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! dnote synchronize bar = %d\n",mr->bnum);
            return(-1);
        }
        if(ecod || eeod) {
            /* fc code */
            syncout_mec(mr->psyn,2); /* offset */
            /* */
            wl = mr->nval - mr->cval;
            queset_mec(wl);
            strcpy(mg->befr,mg->card);
            if(mg->next[0] == '-') strcpy(mg->temp,mg->next2);
            else strcpy(mg->temp,mg->next);
            dodnoff_mec();
            /* */
            if(mg->temp[0] == ':') {
                mg->wflag = 1;
                accid_mec(mk);
                mg->wflag = 0;
                dodpfkey_mec();
                dodanon_mec();
                /* fraseck_mec(wl); */
                /* efect_mec(); */
            }
            fraseck_mec(wl);
        }
    }
    mr->sync -= hosei;
    if(scsn > 1) pasproc_mec(mk);
    return(0);
}
