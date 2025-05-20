
// ----------------------
//  AMuPLC (mec6.c)
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
extern char nxcard[MAXNX][82];

int nextcard_mec(FILE *fpi)
//FILE *fpi;
{
    int  rf;
    int  nx;
    char card[82];
    int  rtn;

    rf = rfchk_mec(mg->next2);
    strcpy(nxcard[0],mg->next);
    if(rf < 0) {
        return(rf);  /* EOF */
    }
    if(rf == 0) {
        strcpy(mg->next,mg->next2);
        fgets(mg->next2,82,fpi);
        /* printf("%s",mg->next2); */
        return(1);
    }
    nx = 1;
    strcpy(nxcard[nx],mg->next2);
    nx++;
    while(rf) {
        fgets(card,82,fpi);
        /* printf("%s",card); */
        strcpy(nxcard[nx],card);
        rf = rfchk_mec(card);
        if(rf < 0) break;
        /* printf("rf = %d\n",rf); */
        nx++;
        if(nx > MAXNX) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! Nextcard greater than %d\n",MAXNX);
            exit(1);
        }
    }
    strcpy(mg->next,card);
    fgets(mg->next2,82,fpi);
    /* printf("%s",mg->next2); */
    return(nx - 1);
}

int rfchk_mec(char card[82])
//char card[82];
{
    int rf;

    rf = 0;
    switch(card[0]) {
        case ' '  : rf = 1;
                    break;
        case '-'  : rf = 1;
                    break;
        case '{'  : rf = 1;
                    break;
        case '['  : rf = 1;
                    break;
        case '('  : rf = 1;
                    break;
        case '#'  : rf = 1;
                    break;
        case '<'  : rf = 1;
                    break;
        case '@'  : rf = 1;
                    break;
        case '*'  : rf = 1;
                    break;
        case '$'  : rf = 1;
                    break;
        case '!'  : rf = 1;
                    break;
        case 'f'  : rf = -1;
                    break;
        default   : break;
    }
    if(card[0] == '$' && card[1] == '$' && card[2] == '$') rf = -2;
    return(rf);
}

int renproc_mec(struct mecksn *mk,FILE *fpi)
//struct mecksn *mk;
//FILE *fpi;
{
    int  i,j,k;
    int  rencnt;
    int  renp;
    int  splt;
    long nval;
    char scard[MSTC][82];
    char nc;  /* next card */

    /*
    printf(".%s",mg->card);
    printf(".%s",mg->next);
    printf(".%s",mg->next2);
    */
    strcpy(scard[0],mg->card);
    strcpy(scard[1],mg->next);
    strcpy(scard[2],mg->next2);
    nc = scard[2][0];
    for(i = 0; i < 3; i++) { /* 3:max comment line in [ */
        /* if(nc == ' ' || nc == '{' || nc == '+' || nc == '(') { */
        if(nc == ' ' || nc == '{' || nc == '(') {
            fgets(scard[2],82,fpi);
            nc = scard[2][0];
        }
        else break;
    }
    for(i = 0; i < 82; i++) {
        if(scard[1][i] == '\r') scard[1][i] = '\0';
        if(scard[2][i] == '\r') scard[2][i] = '\0';
    }
    /*
    printf("+%s",scard[0]);
    printf("+%s",scard[1]);
    printf("+%s",scard[2]);
    */
    rencnt = 1;
    j = 2;
    if(scard[1][4] == '[') rencnt++;
    if(scard[2][4] == '[') rencnt++;
    if(rencnt == 2) {
        j++;
        nc = ' ';
        for(i = 0; i < 3; i++) { /* 3:max comment line in [ */
            if(nc == ' ' || nc == '{' || nc == '(') {
                fgets(scard[j],82,fpi);
                nc = scard[j][0];
                if(nc == '{' && scard[j][1] == '$') {
                    nc = '$'; /* $ key can not ignore */
                    if(scard[j][2] == ':') nc = '{';
                    if(scard[j][3] == ':') nc = '{';
                }
                if(nc == '(' && scard[j][1] == '$') {
                    nc = '$'; /* $ key can not ignore */
                    if(scard[j][2] == ':') nc = '{';
                    if(scard[j][3] == ':') nc = '{';
                }
            }
            else break;
        }
        for(i = 0; i < 82; i++) {
            if(scard[j][i] == '\r') {
                scard[j][i] = '\0';
                break;
            }
        }
    }
    if(rencnt >= 3) {
        while(1) {
            j++;
            if(j > MSTC) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! renp count greater than %d\n",MSTC);
                return(-1);
            }
            nc = ' ';
            for(i = 0; i < 3; i++) { /* 3:max comment line in [ */
                if(nc == ' ' || nc == '{' || nc == '(') {
                    fgets(scard[j],82,fpi);
                    nc = scard[j][0];
                    if(nc == '{' && scard[j][1] == '$') {
                        nc = '$'; /* $ key can not ignore */
                        if(scard[j][2] == ':') nc = '{';
                        if(scard[j][3] == ':') nc = '{';
                    }
                    if(nc == '(' && scard[j][1] == '$') {
                        nc = '$'; /* $ key can not ignore */
                        if(scard[j][2] == ':') nc = '{';
                        if(scard[j][3] == ':') nc = '{';
                    }
                }
                else break;
            }
            for(i = 0; i < 82; i++) {
                if(scard[j][i] == '\r') {
                    scard[j][i] = '\0';
                    break;
                }
            }
            /* if(scard[j][4] != '[') break; */
            if(scard[j][4] == '-') break;
            if(scard[j][4] == '[') rencnt++;
            /* rencnt++; */
            /* printf("rencnt = %d mr->renp = %d\n",rencnt,mr->renp); */
            if(rencnt >= mr->renp) break;
        }
        for(k = 0; k < 1; k++) {
            j++;
            if(j > MSTC) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! renp count greater than %d\n",MSTC);
                return(-1);
            }
            nc = ' ';
            for(i = 0; i < 3; i++) { /* 3:max comment line in [ */
                if(nc == ' ' || nc == '{' || nc == '(') {
                    fgets(scard[j],82,fpi);
                    nc = scard[j][0];
                    if(nc == '{' && scard[j][1] == '$') {
                        nc = '$'; /* $ key can not ignore */
                        if(scard[j][2] == ':') nc = '{';
                        if(scard[j][3] == ':') nc = '{';
                    }
                    if(nc == '(' && scard[j][1] == '$') {
                        nc = '$'; /* $ key can not ignore */
                        if(scard[j][2] == ':') nc = '{';
                        if(scard[j][3] == ':') nc = '{';
                    }
                }
                else break;
            }
            for(i = 0; i < 82; i++) {
                if(scard[j][i] == '\r') {
                    scard[j][i] = '\0';
                    break;
                }
            }
        }
    }
    if(mg->debug == 5) {
        printf("Debug5! mr->rept=%d  mr->renp=%d rencnt=%d  j=%d\n",
        mr->rept,mr->renp,rencnt,j);
    }
    if(mr->rept <= 0) mr->rept = 1;
    for(k = 0; k < mr->rept; k++) {
        for(i = 0; i < j - 1 ; i++) {
            strcpy(mg->card,scard[i]);
            strcpy(mg->next,scard[i+1]);
            strcpy(mg->next2,scard[i+2]);
            /*
            printf(".%s",mg->card);
            printf(".%s",mg->next);
            printf(".%s",mg->next2);
            */
            mg->line++;
            if(mg->card[0] == '%' ||
                mg->card[0] == 's' || mg->card[0] == 'c') {
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
            if(mg->card[0] == 'r') rest_mec(mk);
            if(mg->card[0] == 'R') rest_mec(mk);
            if(mg->card[0] == '+') {
                if(mg->mfflg == 1) mnote_mec(mk);
                else {
                    //befnote_mec(&mk);
                    befnote_mec(mk);  // 2025.4.7
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
                }
            }
            if(mg->card[0] == '<') midic_mec();
            if(mg->card[0] == '*') smfmeta_mec(); 
            if(mg->source) {
                if(mg->source == 1) printf("%s",scard[i]);
                if(mg->source == 2) prints_mec(mg->line,scard[i]);
                mg->prflg = 0;
            }
        }
    }
    return(0);
}

/* large bra. to small bracket */
int lbrtosbr_mec()
{
    int  k;

    k = 0;
    while(mg->card[k]) {
        k++;
        if(k > 80) break;
    }
    for(; k > 0; k--) {
        if(mg->card[k] == '}') {
            mg->card[k] = ')';
            break;
        }
    }
    mg->card[0] = '(';
    return(0);
}

/* small bra. to large bracket */
int sbrtolbr_mec()
{
    int  k;

    k = 0;
    while(mg->card[k]) {
        k++;
        if(k > 80) break;
    }
    for(; k > 0; k--) {
        if(mg->card[k] == ')') {
            mg->card[k] = '}';
            break;
        }
    }
    mg->card[0] = '{';
    return(0);
}

int dodnon_mec()
{
    union meccode fd;
    int  i,j,w;
    int  lastp;
    int  sval;

    lastp = 69;
    sval = 30;  /* start key position */
    j = 14;     /* mg->btyp == 'r' */
    if(mg->btyp == 'd') {
        j = 33;
        sval = 40;
    }
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            mr->dyna = mg->card[i] - '0';
            mr->keyp = i + sval - j;
            if(mg->card[i] == '>') {
                mr->dyna = 5; 
                mr->keyp = mr->keyp + 1;
            }
            if(mg->card[i] == '<') {
                mr->dyna = 5;  
                mr->keyp = mr->keyp - 1;
            }
            fd.zi[0] = 0xfd90;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            mr->keyp &= 0x00ff;
            w = mr->keyp << 8;
            fd.zi[1] |= w;
            mr->veon = mr->dyna * mg->velo;
            if(mr->veon > 127) mr->veon = 127;
            mr->veon &= 0x00ff;
            fd.zi[1] |= mr->veon;
            if(mg->debug == 1) {
                printf("Debug! dodnon_() note on chan=%d ",mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note on chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mr->veon) mg->ntb[mr->keyp] = 1;
            else mg->ntb[mr->keyp] = 0;
            putcode_mec(&fd);
        }
    }
    return(0);
}

int dodanon_mec()
{
    union meccode fd;
    int  i,j,w;
    int  lastp;
    int  sval;

    lastp = 69;
    sval = 30;  /* start key position */
    j = 14;     /* mg->btyp == 'r' */
    if(mg->btyp == 'd') {
        j = 33;
        sval = 40;
    }
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->temp[i]) || mg->temp[i] == '>' ||
            mg->temp[i] == '<') {
            mr->dyna = mg->temp[i] - '0';
            mr->keyp = i + sval - j;
            if(mg->temp[i] == '>') {
                mr->dyna = 5; 
                mr->keyp = mr->keyp + 1;
            }
            if(mg->temp[i] == '<') {
                mr->dyna = 5;  
                mr->keyp = mr->keyp - 1;
            }
            mr->keyp &= 0x00ff;
            fd.zi[0] = 0xfd90;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            w = mr->keyp << 8;
            fd.zi[1] |= w;
            mr->veon = mr->dyna * mg->velo;
            if(mr->veon > 127) mr->veon = 127;
            mr->veon &= 0x00ff;
            fd.zi[1] |= mr->veon;
            if(mg->debug == 1) {
                printf("Debug! dodanon_() note on chan=%d ",mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note on chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mr->veon) mg->ntb[mr->keyp] = 1;
            else mg->ntb[mr->keyp] = 0;
            putcode_mec(&fd);
        }
    }
    return(0);
}
/* polifonic key pressure */
int dodpfkey_mec()
{
    union meccode fd;
    int  i,j,d,w;
    int  pnflg;
    char *pa,*pc;
    int  lastp;
    int  sval;

    lastp = 69;
    sval = 30;  /* start key position */
    j = 14;     /* mg->btyp == 'r' */
    if(mg->btyp == 'd') {
        j = 33;
        sval = 40;
    }
    pa = "QWERTYUIOP";
    for(i = j; i < lastp; i++) {
        pnflg = 0;
        pc = pa;
        for(d = 0; d < 10; d++) {
            if(mg->temp[i] == *pc) {
                pnflg = 1;
                break;
            }
            pc++;
        }
        mr->keyp = i + sval - j;
        if(pnflg && mg->ntb[mr->keyp]) {
            mr->dyna = d + 1;
            fd.zi[0] = 0xfda0;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            mr->keyp &= 0x00ff;
            w = mr->keyp << 8;
            fd.zi[1] |= w;
            mr->kprs = mr->dyna * mg->velo;
            mr->kprs &= 0x00ff;
            fd.zi[1] |= mr->voff;
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) pol.k.p chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d kprs=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->kprs,mr->dyna,mg->velo);
            }
            putcode_mec(&fd);
        }
    }
    return(0);
}

int dodnoff_mec()
{
    union meccode fd;
    int  i,j,d,w;
    int  anflg;
    char *pa,*pc,*pp;
    int  lastp;
    int  sval;

    lastp = 69;
    sval = 30;  /* start key position */
    j = 14;     /* mg->btyp == 'r' */
    if(mg->btyp == 'd') {
        j = 33;
        sval = 40;
    }
    pa = "bcdefghij";
    pp = "BCDEFGHIJ";
    for(i = j; i < lastp; i++) {
        anflg = 0;
        pc = pa;
        for(d = 0; d < 9; d++) {
            if(mg->temp[i] == *pc) {
                anflg = 1;
                break;
            }
            pc++;
        }
        mr->keyp = i + sval - j;
        if(anflg && mg->ntb[mr->keyp]) {
            mr->dyna = d + 1;
            fd.zi[0] = 0xfd80;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            mr->keyp &= 0x00ff;
            w = mr->keyp << 8;
            fd.zi[1] |= w;
            mr->voff = mr->dyna * mg->velo;
            mr->voff &= 0x00ff;
            fd.zi[1] |= mr->voff;
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note off chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d voff=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->voff,mr->dyna,mg->velo);
            }
            mg->ntb[mr->keyp] = 0;
            putcode_mec(&fd);
        }
    }
    lastp = 69;
    sval = 30;  /* start key position */
    j = 14;     /* mg->btyp == 'r' */
    if(mg->btyp == 'd') {
        j = 33;
        sval = 40;
    }
    if(mg->temp[0] == ':') {
        for(i = j; i < lastp; i++) {
            mr->keyp = i + sval -j;
            if(mg->ntb[mr->keyp]) {
                if(mg->temp[i] == ' ' && mg->ntb[mr->keyp]) {
                    fd9off_mec();
                }
            }
        }
    }
    else {     /* mg->temp[0] != ':' */
        for(i = j; i < 69; i++) {
            anflg = 0;
            pc = pp;
            for(d = 0; d < 9; d++) {
                if(mg->temp[i] == *pc) {
                    anflg = 1;     /* BCDEFGHIJ */
                    break;
                }
                pc++;
            }
            if(!anflg) {
                mr->keyp = i + sval -j;
                if(mg->ntb[mr->keyp]) {
                    if(mg->temp[i] != ':' && mg->ntb[mr->keyp])
                        fd9off_mec();
                }
            }
        }
    }
    for(i = 0; i < 128; i++) {   /* efect(tril etc.) note off */
        while(mg->enoftb[i] > 0) {
            mr->keyp = i;
            fd9off_mec();
            mg->enoftb[i]--;
        }
    }
    return(0);
}
