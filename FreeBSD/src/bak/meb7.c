
// ----------------------
//  AMuPLC (meb7.c)
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

#include "meb.h"
extern MIVARG *mv;
extern MIVMST *mm;
extern MIVMXM *mx;
extern MEBVAR *mb;
extern MEBSRG *ls;
extern MEBPH  *ph;
extern MEBCPS *cp;
extern char smpbar[];
extern char midbar[];
extern char mdlbar[];
extern char lowbar[];
extern char higbar[];
extern char topbar[];
extern char gcfbar[];
extern char synbar[];
extern char altbar[];
extern char basbar[];
extern char ubsbar[];
extern char rtmbar[];
extern char dodbar[];
char cpc[22] = "Conductor staff/////";
char lpc[22] = "Low staff///////////";
char hpc[22] = "High staff//////////";
char mpc[22] = "Middle staff////////";
char rpc[22] = "Rhythm staff////////";
char dpc[22] = "Dodecaphonie staff//";
char spc[22] = "Soprano staff///////";
char tpc[22] = "Top staff///////////";
char gpc[22] = "Gclef staff/////////";
char apc[22] = "Alt staff///////////";
char bpc[22] = "Bass staff//////////";
char upc[22] = "Under Bass staff////";
int  renp;
int  splt;
long fnval;
extern char *prtsym;

long getnval_meb(char card[])
//char card[12];
{

    int  i,j,k;
    int  l,s;
    long ll;
    long nval;
    int  pasf;

    nval = 0;
    pasf = 0;
    if(card[4] == '_') {
        i = 6;
        nval = 100000;
        while(card[i] == ')') {
            i++;
            l++;
            if(l > 5) return(1);
            nval = nval / 2;
        }
        if(card[i] == '.') {
            if(l == 5) return(2);
            ll = nval / 2;
            nval = ll + nval;
            i++;
            if(card[i] == '.') {
                if(l == 4) return(3);
                nval = ll/2 + nval;
                i++;
            }
        }
        pasf = 1;
    }
    /* Modify 1996.3.24 */
    /* printf("i=%d\n",i);  Debug */
    if(card[4] == 'O') {
        i = 5;
        nval = 400000;
        if(card[i] == '.') {
            ll = nval / 2;
            nval = ll + nval;
            i++;
            if(card[i] == '.') {
                nval = ll/2 + nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(card[4] == 'o') {
        i = 6;
        nval = 200000;
        if(card[i] == '.') {
            ll = nval / 2;
            nval = ll + nval;
            i++;
            if(card[i] == '.') {
                nval = ll/2 + nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(card[4] == '|') {
        i = 7;
        nval = 800000;
        if(card[i] == '.') {
            ll = nval/2;
            nval = ll/2 + nval;
            i++;
            if(card[i] == '.') {
                nval = ll/2 + nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(card[4] == '.') {
        i = 6;
        nval = 50000;
        while(card[i] == ')') {
            i++;
            l++;
            if(l > 3) return(4);
            nval = nval / 2;
        }
        if(card[i] == '.') {
            if(l == 3) return(5);
            ll = nval / 2;
            nval = ll + nval;
            i++;
            if(card[i] == '.') {
                if(l == 2) return(6);
                nval = ll/2 + nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(pasf) return(nval);
    else return(-1);
}

/* set bar title */
int setbtitl_meb(char card[82],char *inf)
//char card[82];
//char *inf;
{
    int  i,j;
    char btyp;

    btyp = tolower(mb->mbartyp);
    switch (btyp) {
        case  'c' : for(i = 0; i < 20; i++) card[i+4] = cpc[i];
                    break;
        case  'l' : for(i = 0; i < 20; i++) card[i+4] = lpc[i];
                    break;
        case  'h' : for(i = 0; i < 20; i++) card[i+4] = hpc[i];
                    break;
        case  'm' : for(i = 0; i < 20; i++) card[i+4] = mpc[i];
                    break;
        case  'r' : for(i = 0; i < 20; i++) card[i+4] = rpc[i];
                    break;
        case  'd' : for(i = 0; i < 20; i++) card[i+4] = dpc[i];
                    break;
        case  't' : for(i = 0; i < 20; i++) card[i+4] = tpc[i];
                    break;
        case  'g' : for(i = 0; i < 20; i++) card[i+4] = gpc[i];
                    break;
        case  's' : for(i = 0; i < 20; i++) card[i+4] = spc[i];
                    break;
        case  'a' : for(i = 0; i < 20; i++) card[i+4] = apc[i];
                    break;
        case  'b' : for(i = 0; i < 20; i++) card[i+4] = bpc[i];
                    break;
        case  'u' : for(i = 0; i < 20; i++) card[i+4] = upc[i];
                    break;
        default   : break;
    }
    i++;
    for(j = i; j < 70; j++) {
        if(card[2] == '$') card[j] = '$';
        else card[j] = '/';
    }
    j = 0;
    while(1) {
        if(inf[j] == '\0') break;
        card[i] = inf[j];
        j++;
        if(j > STLN) break;
        i++;
        if(i > 70) break;
    }
    switch (mb->mbartyp) {
        case  'c' : card[4] = 'c';
                    break;
        case  'l' : card[4] = 'l';
                    break;
        case  'h' : card[4] = 'h';
                    break;
        case  'm' : card[4] = 'm';
                    break;
        case  'r' : card[4] = 'r';
                    break;
        case  'd' : card[4] = 'd';
                    break;
        case  't' : card[4] = 't';
                    break;
        case  'g' : card[4] = 'g';
                    break;
        case  's' : card[4] = 's';
                    break;
        case  'a' : card[4] = 'a';
                    break;
        case  'b' : card[4] = 'b';
                    break;
        case  'u' : card[4] = 'u';
                    break;
        default   : break;
    }
    if(card[2] == '$') {
        for(i = 4; i < 25; i++) if(card[i] == '/') card[i] = '$';
        serial_meb(card);
    }
    return(0);
}

/* save duble bar number */
int dblbsv_meb()
{
    long temp;

    temp = mb->sequ * 100000;
    mm->dblbar[mm->dblbc] = temp + mb->bnum;
    /*
    printf("mb->sequ=%d mb->bnum=%ld mm->dblbar[%d]=%ld\n",
        mb->sequ,mb->bnum,mm->dblbc,mm->dblbar[mm->dblbc]);
    */
    mm->dblbc++;
    if(mm->dblbc >= 100) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! duble bar limit over. max(100)\n");
        mm->dblbc = 0;
    }
    return(0);
}

/* duble bar check & change syngle bar to duble bar */
int dblbchk_meb()
{
    int  i;
    long barln;

    barln = mb->sequ * 100000;
    barln += mb->bnum;
    for(i = 0; i < mm->dblbc; i++) {
        if(barln == mm->dblbar[i]) {
            dblbar_meb();
            break;
        }
    }
    return(0);
}

/* syngle bar check & change duble bar to syngle bar */
int sglbchk_meb()
{
    int  i;
    long barln;

    barln = mb->sequ * 100000;
    barln += mb->bnum;
    for(i = 0; i < mm->dblbc; i++) {
        if(barln == mm->dblbar[i]) return(0);
    }
    sglbar_meb();
    return(0);
}

int sglbar_meb()
{
    int  i;
    char c;

    mv->card[0] = '-';
    c = mv->card[1];
    if(c == 'c') return(0);
    if(c == 'l' || c == 'm' || c == 'h') i = 11;
    else i = 32;
    for(; i < 68; i++) if(mv->card[i] == '=') mv->card[i] = '_';
    return(0);
}

int dblbar_meb()
{
    int  i;
    char c;

    mv->card[0] = '=';
    c = mv->card[1];
    if(c == 'c') return(0);
    if(c == 'l' || c == 'm' || c == 'h') i = 11;
    else i = 32;
    for(; i < 68; i++) if(mv->card[i] == '_') mv->card[i] = '=';
    return(0);
}

int addbar_meb()
{
    char card[82];
    char wk[8];
    int  i;

    genbar_meb(mb->bartyp,card);
    for(i = 0; i < mb->addbarv; i++) {
        mprintf_meb(card);
        if(mv->source == 1 || mv->trace) printf("%s\n",card);
        mv->line++;
        sprintf(wk,"%02d",mb->sequ);
        card[2] = wk[0];
        card[3] = wk[1];
        mb->bnum++;
        sprintf(wk,"%05d",mb->bnum);
        card[4] = wk[0];
        card[5] = wk[1];
        card[6] = wk[2];
        card[7] = wk[3];
        card[8] = wk[4];
    }
    mb->bnum--;
    mb->xbnum = mb->bnum;
    mb->addbarv = 0;
    return(0);
}

/* ganarate bar line
 default
-s0000001 p.v. mod cn:cdt pcg tp1______2______*______4______5______6 velo P_Exp
*/
int genbar_meb(char bartyp,char card[82])
//char bartyp;
//char card[82];
{
    char wk[40];
    int  i;
    char btyp;

    card[0] = '-';
    card[1] = bartyp;
    sprintf(wk,"%02d",mb->sequ);
    card[2] = wk[0];
    card[3] = wk[1];
    mb->bnum++;
    sprintf(wk,"%05d",mb->bnum);
    card[4] = wk[0];
    card[5] = wk[1];
    card[6] = wk[2];
    card[7] = wk[3];
    card[8] = wk[4];
    card[9] = ' ';
    card[10] = ' ';
    card[11] = ' ';
    btyp = tolower(bartyp);
    /* printf("bartyp=%c btyp=%c\n",bartyp,btyp); */
    if(bartyp == 'c') {
        for(i = 0; i < 36; i++) card[i+12] = midbar[i];
        if(mb->smpmid == 2)
            for(i = 0; i < 36; i++) card[i+12] = smpbar[i];
        strcpy(wk,"  tempo    dymc  hh:mm:ss/ff.ff");
        for(i = 0; i < 32; i++) card[i+48] = wk[i];
        card[79] = '\0';
        return(0);
    }
    switch (btyp) {
        case  'm' : for(i = 0; i < 60; i++) card[i+9] = mdlbar[i];
                    break;
        case  'l' : for(i = 0; i < 60; i++) card[i+9] = lowbar[i];
                    break;
        case  'h' : for(i = 0; i < 60; i++) card[i+9] = higbar[i];
                    break;
        case  't' : for(i = 0; i < 60; i++) card[i+9] = topbar[i];
                    break;
        case  'g' : for(i = 0; i < 60; i++) card[i+9] = gcfbar[i];
                    break;
        case  's' : for(i = 0; i < 60; i++) card[i+9] = synbar[i];
                    break;
        case  'a' : for(i = 0; i < 60; i++) card[i+9] = altbar[i];
                    break;
        case  'b' : for(i = 0; i < 60; i++) card[i+9] = basbar[i];
                    break;
        case  'u' : for(i = 0; i < 60; i++) card[i+9] = ubsbar[i];
                    break;
        case  'r' : for(i = 0; i < 60; i++) card[i+9] = rtmbar[i];
                    break;
        case  'd' : for(i = 0; i < 60; i++) card[i+9] = dodbar[i];
                    break;
        default   : break;
    }
    strcpy(wk,"velo P_Exp");
    for(i = 0; i < 10; i++) card[i+69] = wk[i];
    card[79] = '\0';
    return(0);
}

/* Seq. move & copy */
int beautmvc_meb(char mfile[STLN],int cline)
//char mfile[STLN];
//int  cline;
{
    char sfile[STLN];   /* source file */
    char dfile[STLN];   /* destination file */
    int  i,j,k;
    int  sourcon;
    int  deston;
    int  cdeston;
    int  mdeston;
    int  rmvon;
    char wkc[4];
    int  sp;
    int  fari;
    int  tolw;
    int  toup;

    sourcon = 0;
    deston = 0;
    rmvon = 0;
    cdeston = 0;
    mdeston = 0;
    fari = 0;
    tolw = 0;
    toup = 0;
    for(i = 0; i < 36; i++) {
        if(mx->tmvcp[i] == '*') sourcon = 1;
        if(mx->tmvcp[i] == '+') cdeston = 1;
        if(mx->tmvcp[i] == '@') mdeston = 1;
        if(mx->tmvcp[i] == '/') rmvon = 1;
        if(mx->tmvcp[i] == '?') fari = 1;
        if(mx->tmvcp[i] == '}') tolw = 1;
        if(mx->tmvcp[i] == '{') toup = 1;
    }
    if(cdeston || mdeston) deston = 1;
    if(deston && sourcon == 0) {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! Seq. copy or move source not defined\n");
        }
        return(1);
    }
    if(sourcon && deston == 0) {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! Seq. copy or move destination not defined\n");
        }
        return(2);
    }
    if(rmvon) {
        i = 0;
        while(mfile[i++] != '_');
        for(j = 0; j < cline; j++) {
            for(k = 0; k < 36; k++) {
                if(mx->tmvcp[k] == '/') {
                    /* if(mx->partf[j][k] != '.') { */
                        sprintf(wkc,"%02x",mx->track[j]);
                        mfile[i+1] = wkc[0];
                        mfile[i+2] = wkc[1];
                        mfile[i] = *(prtsym+k);
                        if(mfile[i] == '0' && mfile[i+1] == '0' &&
                            mfile[i+2] == '0') break;  /* cond. track */
                        mebrmvf_meb(mfile);
                        mx->partf[j][k] = '/';
                    /* } */
                }
            }
        }
    }
    if(cdeston) {  /* Copy */
        /* find source track */
        sp = 0;
        while(mx->tmvcp[sp] != '*') sp++;
        /* fine destination track */
        i = 0;
        while(mfile[i++] != '_');
        for(j = 0; j < cline; j++) {
            for(k = 0; k < 36; k++) {
                if(mx->tmvcp[k] == '+') {
                    if(mx->partf[j][sp] != '.') {
                        if(mx->partf[j][sp] == '?') break;
                        strcpy(sfile,mfile);
                        strcpy(dfile,mfile);
                        sfile[i] = *(prtsym+sp);
                        dfile[i] = *(prtsym+k);
                        sprintf(wkc,"%02x",mx->track[j]);
                        sfile[i+1] = wkc[0];
                        sfile[i+2] = wkc[1];
                        dfile[i+1] = wkc[0];
                        dfile[i+2] = wkc[1];
                        mebcopy_meb(dfile,sfile);
                        printf("Copy %s to %s\n",sfile,dfile);
                        mx->partf[j][k] = mx->partf[j][sp];
                    }
                }
            }
        }
    }
    if(mdeston) {  /* Move */
        /* find source track */
        sp = 0;
        while(mx->tmvcp[sp] != '*') sp++;
        /* find destination track */
        i = 0;
        while(mfile[i++] != '_');
        for(j = 0; j < cline; j++) {
            for(k = 0; k < 36; k++) {
                if(mx->tmvcp[k] == '@') {
                    if(mx->partf[j][sp] != '.') {
                        if(mx->partf[j][sp] == '?') break;
                        strcpy(sfile,mfile);
                        strcpy(dfile,mfile);
                        sfile[i] = *(prtsym+sp);
                        dfile[i] = *(prtsym+k);
                        sprintf(wkc,"%02x",mx->track[j]);
                        sfile[i+1] = wkc[0];
                        sfile[i+2] = wkc[1];
                        dfile[i+1] = wkc[0];
                        dfile[i+2] = wkc[1];
                        mebcopy_meb(dfile,sfile);
                        printf("Move %s to %s\n",sfile,dfile);
                        mx->partf[j][k] = mx->partf[j][sp];
                        if(sfile[i] == '0' && sfile[i+1] == '0' &&
                            sfile[i+2] == '0') {
                            printf("\n");
                            break;  /* cond. track */
                        }
                        mebrmvf_meb(mfile);
                        mx->partf[j][sp] = '/';
                    }
                }
            }
        }
    }
    if(fari) {
        for(j = 0; j < cline; j++) {
            for(k = 0; k < 36; k++)
               if(mx->tmvcp[k] == '?') mx->partf[j][k] = '?';
        }
        mx->partf[0][0] = 'C';
    }
    if(tolw) {
        for(j = 0; j < cline; j++) {
            for(k = 0; k < 36; k++) {
                if(mx->tmvcp[k] == '}') {
                    if(isalpha(mx->partf[j][k])) {
                        mx->partf[j][k] =  tolower(mx->partf[j][k]);
                        /* printf("Pass meb7 %c\n",mx->partf[j][k]); */
                    }
                }
            }
        }
        mx->partf[0][0] = 'C';
    }
    if(toup) {
        for(j = 0; j < cline; j++) {
            for(k = 0; k < 36; k++) {
                if(mx->tmvcp[k] == '{') {
                    if(isalpha(mx->partf[j][k])) {
                        mx->partf[j][k] =  toupper(mx->partf[j][k]);
                        /* printf("Pass meb7 %c\n",mx->partf[j][k]); */
                    }
                }
            }
        }
        mx->partf[0][0] = 'C';
    }
    return(0);
}

/* member move & copy */
int beaummvc_meb(char mfile[82],int cline)
//char mfile[STLN];
//int  cline;
{
    char sfile[STLN];
    char dfile[STLN];   /* Add top file */
    int  i,j,k;
    int  sourcon;
    int  deston;
    int  cdeston;
    int  mdeston;
    int  rmvon;
    char wkc[4];
    int  sp;
    int  fari;
    int  tolw;
    int  toup;

    sourcon = 0;
    deston = 0;
    rmvon = 0;
    cdeston = 0;
    mdeston = 0;
    fari = 0;
    tolw = 0;
    toup = 0;
    for(i = 0; i < cline; i++) {
        if(mx->mmvcp[i] == '*') sourcon = 1;
        if(mx->mmvcp[i] == '+') cdeston = 1;
        if(mx->mmvcp[i] == '@') mdeston = 1;
        if(mx->mmvcp[i] == '/') rmvon = 1;
        if(mx->mmvcp[i] == '?') fari = 1;
        if(mx->mmvcp[i] == '}') tolw = 1;
        if(mx->mmvcp[i] == '{') toup = 1;
    }
    if(cdeston || mdeston) deston = 1;
    if(deston && sourcon == 0) {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! Track copy or move source not defined\n");
        }
        return(1);
    }
    if(sourcon && deston == 0) {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! Track copy or move destination not defined\n");
        }
        return(2);
    }
    if(rmvon) {
        i = 0;
        while(mfile[i++] != '_');
        for(j = 0; j < cline; j++) {
            if(mx->mmvcp[j] == '/') {
                for(k = 0; k < 36; k++) {
                    sprintf(wkc,"%02x",mx->track[j]);
                    mfile[i+1] = wkc[0];
                    mfile[i+2] = wkc[1];
                    mfile[i] = *(prtsym+k);
                    if(mfile[i] == '0' && mfile[i+1] == '0' &&
                        mfile[i+2] == '0') break;
                    mebrmvf_meb(mfile);
                    mx->partf[j][k] = '/';
                }
            }
        }
    }
    if(cdeston) {  /* Copy */
        /* find source track */
        sp = 0;
        while(mx->mmvcp[sp] != '*') sp++;
        i = 0;
        while(mfile[i++] != '_');
        for(j = 0; j < cline; j++) {
            if(mx->mmvcp[j] == '+') {
                for(k = 0; k < 36; k++) {
                    if(mx->partf[sp][k] != '.') {
                        if(mx->partf[sp][k] == '?') break;
                        strcpy(sfile,mfile);
                        strcpy(dfile,mfile);
                        sfile[i] = *(prtsym+k);
                        dfile[i] = *(prtsym+k);
                        sprintf(wkc,"%02x",mx->track[sp]);
                        sfile[i+1] = wkc[0];
                        sfile[i+2] = wkc[1];
                        sprintf(wkc,"%02x",mx->track[j]);
                        dfile[i+1] = wkc[0];
                        dfile[i+2] = wkc[1];
                        mebcopy_meb(dfile,sfile);
                        printf("Copy %s to %s\n",sfile,dfile);
                        mx->partf[j][k] = mx->partf[sp][k];
                    }
                }
            }
        }
    }
    if(mdeston) {  /* Move */
        /* find source track */
        sp = 0;
        while(mx->mmvcp[sp] != '*') sp++;
        i = 0;
        while(mfile[i++] != '_');
        for(j = 0; j < cline; j++) {
            if(mx->mmvcp[j] == '@') {
                for(k = 0; k < 36; k++) {
                    if(mx->partf[sp][k] != '.') {
                        if(mx->partf[sp][k] == '?') break;
                        strcpy(sfile,mfile);
                        strcpy(dfile,mfile);
                        sfile[i] = *(prtsym+k);
                        dfile[i] = *(prtsym+k);
                        sprintf(wkc,"%02x",mx->track[sp]);
                        sfile[i+1] = wkc[0];
                        sfile[i+2] = wkc[1];
                        sprintf(wkc,"%02x",mx->track[j]);  /* 97.10 */
                        dfile[i+1] = wkc[0];
                        dfile[i+2] = wkc[1];
                        mebcopy_meb(dfile,sfile);
                        printf("Move %s to %s\n",sfile,dfile);
                        mx->partf[j][k] = mx->partf[sp][k];
                        if(sfile[i] == '0' && sfile[i+1] == '0' &&
                            sfile[i+2] == '0') {
                            printf("\n");
                            break;  /* cond. track */
                        }
                        mebrmvf_meb(mfile);
                        mx->partf[sp][k] = '/';
                    }
                }
            }
        }
    }
    if(fari) {
        for(j = 0; j < cline; j++) {
            if(mx->mmvcp[j] == '?')
                for(k = 0; k < 36; k++) mx->partf[j][k] = '?';
        }
        mx->partf[0][0] = 'C';
    }
    if(tolw) {
        for(j = 0; j < cline; j++) {
            if(mx->mmvcp[j] == '}') {
                for(k = 0; k < 36; k++) {
                    if(isalpha(mx->partf[j][k])) {
                        mx->partf[j][k] =  tolower(mx->partf[j][k]);
                        /* printf("Pass meb7 %c\n",mx->partf[j][k]); */
                    }
                }
            }
        }
        mx->partf[0][0] = 'C';
    }                   
    if(toup) {          
        for(j = 0; j < cline; j++) { 
            if(mx->mmvcp[j] == '{') {
                for(k = 0; k < 36; k++) {
                    if(isalpha(mx->partf[j][k])) {
                        mx->partf[j][k] =  toupper(mx->partf[j][k]);
                        /* printf("Pass meb7 %c\n",mx->partf[j][k]); */
                    }       
                }           
            }           
        }               
        mx->partf[0][0] = 'C';
    }               
    return(0);
}

/* Seq. shift */
int seqshift_meb(char mfile[STLN],int cline)
//char mfile[STLN];
//int  cline;
{
    char sfile[STLN];   /* source file */
    char dfile[STLN];   /* destination file */
    int  i,j,k;
    int  sp;
    char wkc[4];
    int  Rsqsf; /* Right sequence shift flag */
    int  Lsqsf; /* Left sequence shift flag */
    int  Rsqsp; /* Right sequence shift position */
    int  Lsqsp; /* Left sequence shift position */

    Rsqsf = 0;
    Lsqsf = 0;
    Rsqsp = 0;
    Lsqsp = 0;
    for(i = 0; i < 36; i++) {
        if(mx->tmvcp[i] == '>') {
            Rsqsf++;
            Rsqsp = i;
        }
        if(mx->tmvcp[i] == '<') {
            Lsqsf++;
            Lsqsp = i;
        }
    }
    printf("seqshift_() Rsqsf=%d Rsqsp=%d Lsqsf=%d Lsqsp=%d\n",
        Rsqsf,Rsqsp,Lsqsf,Lsqsp);
    if(Rsqsf > 1) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! Too many sequence shift '>'\n");
        return(-1);
    }
    if(Lsqsf > 1) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! Too many sequence shift '<'\n");
        return(-1);
    }
    if(Rsqsf && Lsqsf) {
        mv->wrncnt++;
        mv->msglvl = 2;
        if(mv->msglvl >= mv->prtlvl)
            printf("Worning! Too many sequence shift '>' and  '<'\n");
    }
    if(Rsqsf) {
        strcpy(sfile,mfile);
        strcpy(dfile,mfile);
        sp = Rsqsp;
        i = 0;
        while(mfile[i++] != '_');
        for(j = 0; j < cline; j++) {
            for(k = 35; k >= sp; k--) {
                if(mx->partf[j][k] != '.') {
                    sfile[i] = *(prtsym+k);
                    dfile[i] = *(prtsym+k+1);
                    sprintf(wkc,"%02x",mx->track[j]);
                    sfile[i+1] = wkc[0];
                    sfile[i+2] = wkc[1];
                    dfile[i+1] = wkc[0];
                    dfile[i+2] = wkc[1];
                    mx->partf[j][k+1] = mx->partf[j][k];
                    if(mx->partf[j][k+1] == '.') {
                        if(!rename(sfile,dfile))
                            printf("Rename %s to %s\n",sfile,dfile);
                    }
                    else {
                        mebcopy_meb(dfile,sfile);
                        printf("Move %s to %s\n",sfile,dfile);
                    }
                    mx->partf[j][k+1] = mx->partf[j][k];
                    if(sfile[i] == '0' && sfile[i+1] == '0' &&
                        sfile[i+2] == '0') {
                        printf("\n");
                        break;  /* cond. track */
                    }
                    /* mebrmvf_meb(mfile); */
                    mx->partf[j][k] = '.';
                }
            }
        }
    }
    if(Lsqsf) {
        strcpy(sfile,mfile);
        strcpy(dfile,mfile);
        sp = Lsqsp;
        i = 0;
        while(mfile[i++] != '_');   
        for(j = 0; j < cline; j++) {
            for(k = 1; k <= sp; k++) {
                if(mx->partf[j][k] != '.') {
                    sfile[i] = *(prtsym+k);
                    dfile[i] = *(prtsym+k-1);
                    sprintf(wkc,"%02x",mx->track[j]);
                    sfile[i+1] = wkc[0];
                    sfile[i+2] = wkc[1];
                    dfile[i+1] = wkc[0]; 
                    dfile[i+2] = wkc[1];
                    mx->partf[j][k-1] = mx->partf[j][k];
                    if(mx->partf[j][k-1] == '.') {
                        if(!rename(sfile,dfile))
                            printf("Rename %s to %s\n",sfile,dfile);
                    }
                    else {
                        mebcopy_meb(dfile,sfile);
                        printf("Move %s to %s\n",sfile,dfile);
                    }
                    mx->partf[j][k-1] = mx->partf[j][k];
                    if(sfile[i] == '0' && sfile[i+1] == '0' &&
                        sfile[i+2] == '0') {
                        printf("\n");
                        break;  /* cond. track */
                    }
                    /* mebrmvf_meb(mfile); */
                    mx->partf[j][k] = '.';
                }
            }
        }
    }
    return(0);
}

/* Cline ? gloval serch */
int gserch_meb(char mfile[STLN],int cline)
//char mfile[STLN];
//int  cline;
{
    int  i,j,k;

    /* printf("gserch_() Cline gloval serch\n"); */
    for(j = 0; j < cline; j++) {  
        for(k = 0; k < 36; k++)
           if(mx->partf[j][k] == '.') mx->partf[j][k] = '?';
    }
    mx->partf[0][0] = 'C';
    return(0);
}

/* Cline % gloval clear */
int gclear_meb(char mfile[STLN],int cline)
//char mfile[STLN];
//int  cline;
{
    int  i,j,k;
    char wkc[4];
    char c;

    /* printf("gclear_() Cline gloval clear\n"); */
    i = 0;
    while(mfile[i++] != '_');
    for(j = 0; j < cline; j++) {
        for(k = 0; k < 36; k++) {
            c = mx->partf[j][k];
            if(c == '.' || c == 'c' || c == 'l' || c == 'm' || c == 'h' ||
            c == 't' || c == 'g' || c == 's' || c == 'a' || c == 'b' ||
            c == 'u' || c == 'r' || c == 'd') {
                sprintf(wkc,"%02x",mx->track[j]);
                mfile[i+1] = wkc[0];
                mfile[i+2] = wkc[1];
                mfile[i] = *(prtsym+k);
                if(mfile[i] == '0' && mfile[i+1] == '0' &&
                    mfile[i+2] == '0') break;
                mebrmvf_meb(mfile);
                mx->partf[j][k] = '?';
            }
        }
    }
    mx->partf[0][0] = 'C';
    return(0);
}

// beau phraze
// [p<f/3;f>pp///]         -> [p<f _/3;f>pp _/))]
// [23 > 45//*4]           -> [23 < 45 _/)*4]
// [mp < fff/h;fff>ff/h]   -> [mp < fff o/;fff>ff o/]
// [12 < ff/h.2; ff>p/w12] -> [12 < ff o/.2; ff>p O/12]
int beaufras_meb()
{
    int  i,j,k,n;
    int  smcnt;
    int  endmk;
    int  pv1,pv2;
    char wk1[82];
    char wk2[82];
    char c;
    int  bupss;
    char wk3[30];
    char wk4[30];
    int  psf;

    /* beautifyd check */
    bupss = 0;
    i = 0;
    while(mv->card[i]) {
        if(mv->card[i] == '_') bupss = 1;
        if(mv->card[i] == 'O') bupss = 2;
        if(mv->card[i] == 'o') bupss = 3;
        if(mv->card[i] == ']') break;
        i++;
        if(i > 80) break;
    }
    n = strlen(mv->card); /* line over no need beautify */
    if(n > 70) return(0);
    i = 0;
    while(mv->card[i]) {
        if(mv->card[i] == ']') break;
        i++;
        if(i > 80) return(0);
    }
    i++;
    while(mv->card[i] == ' ') i++;
    wk3[0] = '\0';
    if(mv->card[i] == 'v' || mv->card[i] == 'V') strcpy(wk3," Volume");
    if(mv->card[i] == 'p' || mv->card[i] == 'P') strcpy(wk3," Pitch_vend");
    if(mv->card[i] == 'e' || mv->card[i] == 'E') strcpy(wk3," Expression");
    if(mv->card[i] == 'x' || mv->card[i] == 'X') strcpy(wk3," Expression");
    if(mv->card[i] == 'm' || mv->card[i] == 'M') strcpy(wk3," Modulation");
    if(mv->card[i] == 'c' || mv->card[i] == 'C') {
        strcpy(wk3," Controller ");
        while(!isdigit(mv->card[i])) {
            i++;
            if(i > 80) return(0);
        }
        j = 0;
        wk1[0] = '\0';
        while(isdigit(mv->card[i])) {
            wk1[j] = mv->card[i];
            i++;
            j++;
        }
        wk1[j] = '\0';
        strcat(wk3,wk1);
    }
    while(mv->card[i] != ' ') {
        i++;
        if(i > 78) break;
    }
    while(mv->card[i] == ' ') {
        i++;
        if(i > 78) break;
    }
    psf = 0;
    if(mv->card[i] == 'i' || mv->card[i] == 'I') {
        strcpy(wk4," Interval ");
        while(!isdigit(mv->card[i])) {
            i++;
            if(i > 80) return(0);
        }
        j = 0;
        wk1[0] = '\0';
        while(isdigit(mv->card[i])) {
            wk1[j] = mv->card[i];
            i++;
            j++;
        }
        wk1[j] = '\0';
        strcat(wk4,wk1);
        psf = 1;
    }
    if(mv->card[i] == 'n' || mv->card[i] == 'N') {
        strcpy(wk4," Number_of_Controle_data ");
        while(!isdigit(mv->card[i])) {
            i++;
            if(i > 80) return(0);
        }
        j = 0;
        wk1[0] = '\0';
        while(isdigit(mv->card[i])) {
            wk1[j] = mv->card[i];
            i++;
            j++;
        }
        wk1[j] = '\0';
        strcat(wk4,wk1);
        psf = 2;
    }
    /* printf("wk3=%s wk4=%s\n",wk3,wk4); */
    if(bupss) {
        i = 0;
        while(mv->card[i] != ']') i++;
        mv->card[++i] = '\0';
        strcat(mv->card,wk3);
        if(psf) strcat(mv->card,wk4);
    }
    n = 0;
    k = 0;
    endmk = 0;
    smcnt = 0;
    for(i = 0; i < 80; i++) {
        wk2[i] = ' ';
        if(mv->card[i] == ';') smcnt++;
    }
    strcpy(wk1,mv->card);
    /* printf("org=%s\n",wk1); */
    mv->card[0] = '\0';
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
        pv1 = 0;
        pv2 = 0;
        bfunc_meb(wk2,&pv1,&pv2);
        if(pv1 > 127 || pv2 > 127) {
            mv->wrncnt++;
            mv->msglvl = 2;
            if(mv->msglvl >= mv->prtlvl)
            printf("Worning! %05d phraze [pv1 < pv2] max value is 127\n",
            mv->line);
        }
        if(mv->debug == 1) printf("Debug1! pv1=%d pv2=%d\n",pv1,pv2);
        wk2[0] = c;   /* back */
        strcat(mv->card,wk2);
        k += strlen(wk2);
        i = 0;
        while(wk1[n]) {
            if(wk1[n] == ';') break;
            if(wk1[n] == ']') {
                endmk = 1;
                break;
            }
            if(wk1[n] != ' ') { /* Cut white space */
                wk2[i] = wk1[n];
                i++;
            }
            n++;
            if(i > 80) {
                if(!mv->pass) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                    printf("Error! %05d frasing data\n",mv->line);
                }
                return(-1);
            }
        }
        wk2[i] = ' '; /* dumy for nbchg_meb() */
        wk2[++i] = '\0';
        if(!bupss) nbchg_meb(wk2);
        strcat(mv->card,wk2);
        k += strlen(wk2);
        if(k > 80) {
            if(!mv->pass) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl)
                printf("Error! %05d beautify length\n",mv->line);
            }
            return(-1);
        }
        if(endmk) {
            mv->card[k] = ']';
            break;
        }
    }
    i = 0;
    while(mv->card[i] != ']') i++;
    mv->card[++i] = '\0';
    strcat(mv->card,wk3);
    if(psf) strcat(mv->card,wk4);
    return(0);
}

int nbchg_meb(char wk[82])
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
        if(c == 'd') {
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

int bfunc_meb(char s[82],int *pv1,int *pv2)
//char s[82];
//int  *pv1,*pv2;
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
    if(df) sscanf(wk,"%d",pv1);
    /* printf("wk=%s pv1=%d\n",wk,*pv1); */
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
        *     ppp  (1)  12.8    12
        *      pp  (2)  25.6    25
        *       p  (3)  38.4    38
        *      mp  (4)  51.2    51
        *          (5)  64      64
        *      mf  (6)  76.8    77
        *       f  (7)  89.6    90
        *      ff  (8) 102.4   103
        *     fff  (9) 115.2   115
        */
        *pv1 = 64;
        if(pv == 3) *pv1 = 12;
        if(pv == 2) *pv1 = 25;
        if(pv == 1) *pv1 = 38;
        if(mv && pv) *pv1 = 51;
        if(mv & fv) *pv1 = 77;
        if(fv == 1) *pv1 = 90;
        if(fv == 2) *pv1 = 103;
        if(fv == 3) *pv1 = 115;
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
    if(df) sscanf(wk,"%d",pv2);
    /* printf("wk=%s pv2=%d\n",wk,*pv2); */
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
        *pv2 = 64;
        if(pv == 3) *pv2 = 12;
        if(pv == 2) *pv2 = 25;
        if(pv == 1) *pv2 = 38;
        if(mv && pv) *pv2 = 51;
        if(mv & fv) *pv2 = 77;
        if(fv == 1) *pv2 = 90;
        if(fv == 2) *pv2 = 103;
        if(fv == 3) *pv2 = 115;
    }
    if((q == 1 && (*pv1 > *pv2)) ||
       (q == 2 && (*pv1 < *pv2))) {
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

/* set mv->byps flag */
int setbyps_meb()
{
    long cbn;
    long sbn;
    long ebn;

    if(mv->byps == 2) return(0);
    switch(ls->brself) {
        case  1 : if(mb->sequ > mm->esequ) mv->byps = 2;
                  else if(mb->sequ >= mm->ssequ) mv->byps = 1;
                  break;
        case  2 : cbn = mb->bnum;   /* for compiler bug ? */
                  sbn = mm->sbarn;
                  ebn = mm->ebarn;
                  if(cbn >= ebn) mv->byps = 2;
                  else if(cbn >= sbn) mv->byps = 1;
                  break;
        case  3 : cbn = mb->sequ * 100000 + mb->bnum;
                  sbn = mm->ssequ * 100000 + mm->sbarn;
                  ebn = mm->esequ * 100000 + mm->ebarn;
                  if(cbn >= ebn) mv->byps = 2;
                  else if(cbn >= sbn) mv->byps = 1;
                  /*
                  if(mb->sequ <= mm->esequ && cbn >= ebn) mv->byps = 2;
                  else if(mb->sequ > mm->ssequ || cbn >= sbn)
                      mv->byps = 1;
                  */
                  break;
        default : break;
    }
    return(0);
}

int incfile_meb()
{
    FILE *fpinc;
    char card[82];

    fpinc = fopen_h(mv->incf,"r");
    while(fgets(card,82,fpinc)) {
        mprintf_meb(card);
    }
    fclose(fpinc);
    mb->inclf = 0;
    return(0);
}

