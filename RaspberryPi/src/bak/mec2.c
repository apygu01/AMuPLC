
// ----------------------
//  AMuPLC (mec2.c)
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
extern MECFRS *fr;
extern FILE  *fpi;
extern char scsv[MAXSC][82];      /* card save area */
extern int  scsn;                 /* card save number count */
extern int Nachf;
long getswing_mec();
extern long nvt[22];
char *vr = "ABCDEFGHIJ"; /* velocity on > */
char *vl = "abcdefghij"; /* velocity on < */

int pasread_mec()
{
    int  i;
    int  stpfg;
    int  getf;

    getf = 0;
    while(1) {
        for(i = 0; i < 82; i++) {
            if(mg->next[i] == '\r') {
                mg->next[i] = '\0';
                break;
            }
        }
        mg->next[81] = '\0';
        strcpy(scsv[scsn],mg->next);
        scsn++;
        if(scsn >= MAXSC) break;
        stpfg = 0;
        switch(mg->next2[0]) {
            case  '%' : stpfg = 1;
                        break;
            case  's' : stpfg = 1;
                        break;
            case  'c' : stpfg = 1;
                        break;
            case  'R' : stpfg = 1;
                        break;
            case  'r' : stpfg = 1;
                        break;
            case  ':' : stpfg = 1;
                        break;
            case  '|' : stpfg = 1;
                        break;
            /*
            case  '/' : if(mg->next2[1] == '%') stpfg = 1;
                        break;
            case  '+' : stpfg = 1;
                        break;
            */
            case  '=' : stpfg = 1;
                        break;
            case  'f' : stpfg = 1;
                        break;
            case 0x1a : stpfg = 1;
                        break;
            default   : break;
        }
        if(stpfg) break;
        strcpy(mg->next,mg->next2);
        for(i = 0; i < 80; i++) mg->next2[i] = ' ';
        fgets(mg->next2,82, fpi);
        getf = 1;
    }
    if(getf) {
        strcpy(mg->next,mg->next2);
        for(i = 0; i < 82; i++) {
            if(mg->next[i] == '\r') {
                mg->next[i] = '\0';
                break;
            }
        }
        mg->next[81] = '\0';
        for(i = 0; i < 80; i++) mg->next2[i] = ' ';
        fgets(mg->next2,82, fpi);
    }
    return(0);
}

int note_mec(struct mecksn *mk,int exf,char exp[4])
//struct mecksn *mk;
//int  exf;
//char exp[4];
{
    union meccode fc;
    int  i,j,k;
    int  l,s;
    int  lastp;        /* avalable column */
    long ll;
    int  ecod;
    int  eeod;
    int  pasf;
    long nval;
    static int  accia;
    char wk[10];
    int  hosei;
    int  nclr;
    long wval;
    long lagsync;
    long offsync;
    float pport;
    long  swing;
    long nacv;

    pasf = 0;
    scsn = 0;
    switch(mg->next[0]) {
        case  '%' : pasf = 1;
                    break;
        case  's' : pasf = 1;
                    break;
        case  'R' : pasf = 1;
                    break;
        case  'r' : pasf = 1;
                    break;
        case  ':' : pasf = 1;
                    break;
        case  '|' : pasf = 1;
                    break;
        /*
        case  '/' : if(mg->next[1] == '%') pasf = 1;
                    break;
        case  '+' : pasf = 1;
                    break;
        */
        case  '=' : pasf = 1;
                    break;
        case  'f' : pasf = 1;
                    break;
        case 0x1a : pasf = 1;
                    break;
        default   : break;
    }
    if(!pasf && mg->expf == 0) pasread_mec();
    if(mg->next[0] == '-') strcpy(mg->temp,mg->next2);
    else strcpy(mg->temp,mg->next);
    pasf = 0;
    l = 0;
    hosei = 0;
    for(i = 0; i < 128; i++) mg->enoftb[i] = 0;
    wk[0] = mg->card[1];
    wk[1] = mg->card[2];
    wk[2] = '\0';
    sscanf(wk,"%d",&mr->pport);
    pport = mr->pport;
    if(mr->pport == 0) pport = 99.999;   /* %00 = 100% */
    if(mr->pport < 0 || mr->pport > 99) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d gate time rate\n",mg->line);
    }
    if(mg->card[4] == '_') {
        accia = 0;
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
        accia = 0;
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
        accia = 0;
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
        accia = 0;
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
    ll = mr->nval;
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
    if(mg->next[0] == '+' && mg->next[4] == '.' && mg->next2[6] == ')') {
        nacv = 0;
        nacv = getnacv_mec(mg->next2);
        if(mg->trace) {
            printf("Trace! nacv=%ld\n",nacv);
        }
        mr->nval = mr->nval - nacv;
    }
    swing = getswing_mec();
    if(mr->nval > swing) mr->nval = mr->nval + swing;
    else {
	printf("Error! %06d Swing over than note value %ld\n",mg->line,swing);
    }
    wk[0] = mg->card[69];
    wk[1] = mg->card[70];
    wk[2] = mg->card[71];
    wk[3] = mg->card[72];
    wk[4] = '\0';
    sscanf(wk,"%f",&mg->velo);
    ecod = 0;
    nclr = 0;
    lastp = 69;
    /* j = 11; ? */
    j = 10;
    if(mg->btyp == 't') j = 32;
    if(mg->btyp == 'g') j = 32;
    if(mg->btyp == 's') j = 32;
    if(mg->btyp == 'a') j = 32;
    if(mg->btyp == 'b') j = 32;
    if(mg->btyp == 'u') j = 32;
    for(i = j; i < lastp; i++) {
        if((mg->card[i] != ' ') && (mg->card[i] != '.')) ecod = 1;
        if(isdigit(mg->card[i]) || mg->card[i] == ':') nclr = 1;
    }
    if(!nclr) noteclr_mec();
    eeod = 0;
    if(mg->temp[0] == ':') {
        j = 10;
        if(mg->btyp == 't') j = 32;
        if(mg->btyp == 'g') j = 32;
        if(mg->btyp == 's') j = 32;
        if(mg->btyp == 'a') j = 32;
        if(mg->btyp == 'b') j = 32;
        if(mg->btyp == 'u') j = 32;
        for(i = j; i < 69; i++)
            if((mg->temp[i] != ' ') && (mg->temp[i] != '.')) eeod = 1;
    }
    if(mg->trace && ecod) {
        printf("Trace! nval=%ld splt=%d renp=%d ",
        mr->nval,mr->splt,mr->renp);
        printf("rept=%d hosei=%d --------\n",mr->rept,hosei);
    }
    accid_mec(mk);
    for(s = 0; s < mr->splt; s++) {
        /* fc code */
        nval = mr->nval;
        syncout_mec(mr->sync,1);  /* lag */
        /* note */
        if(!accia) {
            nval -= mr->acci;
            mr->acci = 0;
        }
        /* mr->cval = nval*mr->pport/100; */
        mr->cval = nval*pport/100;
        wval = mr->cval;
        mr->psyn = mr->sync;
        queset_mec(wval);
        strcpy(mg->temp,mg->card);
        noteoff_mec();
        if(ecod) {
            polfkey_mec();
            if(exf) dexpchk_mec(exp,nval);
            else noteon_mec();
            fraseck_mec(mr->sync);
            efect_mec();
        }
        fraseck_mec(mr->sync);
        /* sync */
        mr->psyn = mr->sync - mr->cval;
        mr->sync = mr->sync - nval;
        /* printf("sync=%ld nval=%ld\n",mr->sync,nval); */
        /* if(mr->sync > 0xffffff || mr->sync < 0) {  org  */ 
        if(mr->sync > 0xffffff || mr->sync < -6) { /* swing hosei */
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! Note synchronize bar = %d\n",mr->bnum);
            return(-1);
        }
        if(ecod || eeod) {
            /* fc code */
            if(!exf) syncout_mec(mr->psyn,2); /* offset */
            /* */
            wval = mr->nval - mr->cval;
            queset_mec(wval);
            strcpy(mg->befr,mg->card);
            if(mg->next[0] == '-') strcpy(mg->temp,mg->next2);
            else strcpy(mg->temp,mg->next);
            noteoff_mec();
            /* */
            if(mg->temp[0] == ':') {
                mg->wflag = 1;
                accid_mec(mk);
                mg->wflag = 0;
                polfkey_mec();
                anoteon_mec();
                fraseck_mec(mr->psyn);
                efect_mec();
            }
            fraseck_mec(mr->psyn);
        }
    }
    mr->sync -= hosei;
    if(mg->card[4] != '[') mr->nval = mr->nval - swing; /* %xx _/[3+xf hosei */
    if(scsn > 1) pasproc_mec(mk);
    return(0);
}

int pasproc_mec(struct mecksn *mk)
//struct mecksn *mk;         /* key signeture table */
{
    int  i,j,k;
    char svcard[82];
    union meccode ff;

    strcpy(svcard,mg->card);
    for(k = 0; k < scsn; k++) {
        strcpy(mg->card,scsv[k]);
        mg->line++;
	pr_card(mg->card);
	/*
        for(i = 0; i < 82; i++) {
            if(mg->card[i] == '\r') {
                mg->card[i] = '\0';
                break;
            }
        }
        if(mg->source) {
            if(mg->source == 1) printf("%s\n",mg->card);
            if(mg->source == 2) prints_mec(mg->line,mg->card);
        }
	*/
        switch (mg->card[0]) {
            case '-'  : bar_mec();
                        for(j = 0; j < 78; j++)
                            mg->ktbbak[j] = mg->ktb[j]; 
                        keyini_mec();
                        keyset_mec();
                        if(mg->debug == 3) keydmp_mec(mk);
                        break;
            case '['  : frasing_mec();
                        break;
            case '{'  : /* Gloval */
                        if(mg->card[1] == '?') break; /* Skip Chord line */
                        if(mg->card[1] == '$' && mg->card[2] == ':') break;
                        if(mg->card[1] == '$' && mg->card[3] == ':') break;
                        if(mg->card[1] == '$') {
                            //keyryhm_mec(&mk);
                            keyryhm_mec(mk);  // 2025.4.6
                            break;
                        }
                        else expression_mec(&ff);
                        break;
            case '('  : sbrtolbr_mec();
                        if(mg->card[1] == '$') {
                            //keyryhm_mec(&mk);
                            keyryhm_mec(mk);  // 2025.4.6
                            break;
                        }
                        else expression_mec(&ff);
                        lbrtosbr_mec();
                        break;
            case '<'  : midic_mec();
                        break;
            case '#'  : strcheck_mec();
                        break;
            case '$'  : /* ffwrite_mec(&ff); */
                        //keyryhm_mec(&mk);
                        keyryhm_mec(mk);  // 2025.4.6
                        break;
            //case '+'  : befnote_mec(&mk);
            case '+'  : befnote_mec(mk);  // 2025.4.6
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
                        /*
                        polfkey_mec();
                        anoteon_mec();
                        */
                        break;
            default   : break;
        }
    }
    strcpy(mg->card,svcard);
    return(0);
}

int accid_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    int  i,k;
    int  setf;
    char c;
    int  startp;
    int  lastp;
    int  nclr;

    nclr = 0;
    setf = 0;
    k = startkp_mec(&startp,&lastp);
    for(i = startp; i < lastp ; i++, k++) {
        if(mg->wflag == 1) c = mg->next[i];
        else c = mg->card[i];
        switch(c) {
            case '#' : mr->keyp = mg->ktb[k];
                       if(mg->ntb[mr->keyp]) fd9off_mec();
                       mg->ktb[k] = mg->nktb[k] + 1;
                       setf++;
                       break;
            case '&' : mr->keyp = mg->ktb[k];
                       if(mg->ntb[mr->keyp]) fd9off_mec();
                       mg->ktb[k] = mg->nktb[k] - 1;
                       setf++;
                       break;
            case '*' : mr->keyp = mg->ktb[k];
                       if(mg->ntb[mr->keyp]) fd9off_mec();
                       mg->ktb[k] = mg->nktb[k] + 2;
                       setf++;
                       break;
            case '@' : mr->keyp = mg->ktb[k];
                       if(mg->ntb[mr->keyp]) fd9off_mec();
                       mg->ktb[k] = mg->nktb[k] - 2;
                       setf++;
                       break;
            case '$' : mr->keyp = mg->ktb[k];
                       if(mg->ntb[mr->keyp]) fd9off_mec();
                       mg->ktb[k] = mg->nktb[k];
                       setf++;
                       break;
            default  : break;
        }
        if(isdigit(mg->card[i]) || mg->card[i] == ':') nclr = 1;
    }
    if(!nclr) noteclr_mec();
    if(mg->debug == 2 && setf) keydmp_mec(mk);
    return(0);
}

int noteon_mec()
{
    union meccode fd;
    int  i,j,k;
    int  lastp;
    int  v,vv;

    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            mr->dyna = mg->card[i] - '0';
            mr->keyp = mg->ktb[k];
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
            j = mr->keyp << 8;
            fd.zi[1] |= j;
            mr->veon = mr->dyna * mg->velo;
            if(mr->veon > 127) mr->veon = 127;
            mr->veon &= 0x00ff;
            fd.zi[1] |= mr->veon;
            if(mg->debug == 1) {
                printf("Debug! noteon_(1) note on chan=%d ",mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note on chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mr->veon) mg->ntb[mr->keyp] = k;
            else mg->ntb[mr->keyp] = 0;
            putcode_mec(&fd);
        }
        if(isalpha(mg->card[i])) {
            vv = 0;
            for(v = 0; v <= 9; v++) {
                if(mg->card[i] == vr[v]) {
                    vv = 1;
                    break;
                }
                if(mg->card[i] == vl[v]) {
                    vv = -1;
                    break;
                }
            }
            if(vv) {
                mr->dyna = v;
                mr->keyp = mg->ktb[k] + vv;
                fd.zi[0] = 0xfd90;
                fd.zi[1] = 0;
                mr->chan &= 0x000f;
                fd.zi[0] |= mr->chan;
                mr->keyp &= 0x00ff;
                j = mr->keyp << 8;
                fd.zi[1] |= j;
                mr->veon = mr->dyna * mg->velo;
                if(mr->veon > 127) mr->veon = 127;
                mr->veon &= 0x00ff;
                fd.zi[1] |= mr->veon;
                if(mg->debug == 1) {
                    printf("Debug! noteon_(2) note on chan=%d ",mr->chan);
                    printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f ",
                    mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
                    printf("%c v=%d vv=%d\n",mg->card[i],v,vv);
                }
                if(mg->trace) {
                    printf("Trace! FD(%04x%04x) note on chan=%d ",
                    fd.zi[0],fd.zi[1],mr->chan);
                    printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                    mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
                }
                if(mr->veon) mg->ntb[mr->keyp] = k;
                else mg->ntb[mr->keyp] = 0;
                putcode_mec(&fd);
            }
        }
        k++;
    }
    return(0);
}

int anoteon_mec()
{
    union meccode fd;
    int  i,j,k;
    int  lastp;
    int  v,vv;

    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->temp[i]) || mg->temp[i] == '>' ||
            mg->temp[i] == '<') {
            mr->dyna = mg->temp[i] - '0';
            mr->keyp = mg->ktb[k];
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
            j = mr->keyp << 8;
            fd.zi[1] |= j;
            mr->veon = mr->dyna * mg->velo;
            if(mr->veon > 127) mr->veon = 127;
            mr->veon &= 0x00ff;
            fd.zi[1] |= mr->veon;
            if(mg->debug == 1) {
                printf("Debug! anoteon_() note on chan=%d ",mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note on chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mr->veon) mg->ntb[mr->keyp] = k;
            else mg->ntb[mr->keyp] = 0;
            putcode_mec(&fd);
        }
        if(isalpha(mg->card[i])) {
            vv = 0;
            for(v = 0; v <= 9; v++) {
                if(mg->card[i] == vr[v]) {
                    vv = 1;
                    break;
                }
                if(mg->card[i] == vl[v]) {
                    vv = -1;
                    break;
                }
            }
            if(vv) {
                mr->dyna = v;
                mr->keyp = mg->ktb[k] + vv;
                fd.zi[0] = 0xfd90;
                fd.zi[1] = 0;
                mr->chan &= 0x000f;
                fd.zi[0] |= mr->chan;
                mr->keyp &= 0x00ff;
                j = mr->keyp << 8;
                fd.zi[1] |= j;
                mr->veon = mr->dyna * mg->velo;
                if(mr->veon > 127) mr->veon = 127;
                mr->veon &= 0x00ff;
                fd.zi[1] |= mr->veon;
                if(mg->debug == 1) {
                    printf("Debug! noteon_(2) note on chan=%d ",mr->chan);
                    printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f ",
                    mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
                    printf("%c v=%d vv=%d\n",mg->card[i],v,vv);
                }
                if(mg->trace) {
                    printf("Trace! FD(%04x%04x) note on chan=%d ",
                    fd.zi[0],fd.zi[1],mr->chan);
                    printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                    mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
                }
                if(mr->veon) mg->ntb[mr->keyp] = k;
                else mg->ntb[mr->keyp] = 0;
                putcode_mec(&fd);
            }
        }
        k++;
    }
    return(0);
}

/* polifonic key pressure */
int polfkey_mec()
{
    union meccode fd;
    int  i,j,k,d;
    int  pnflg;
    char *pa,*pc;
    int  lastp;

    k = startkp_mec(&j,&lastp);
    /* pa = "abcdefghij"; */
    pa = "jklnpquvyz";
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
        mr->keyp = mg->ktb[k];
        if(pnflg && mg->ntb[mr->keyp]) {
            mr->dyna = d + 1;
            fd.zi[0] = 0xfda0;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            mr->keyp &= 0x00ff;
            j = mr->keyp << 8;
            fd.zi[1] |= j;
            mr->kprs = mr->dyna * mg->velo;
            mr->kprs &= 0x00ff;
            if(mr->kprs > 127) mr->kprs = 127;
            fd.zi[1] |= mr->kprs;
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) pol.k.p chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d kprs=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->kprs,mr->dyna,mg->velo);
            }
            putcode_mec(&fd);
        }
        k++;
    }
    return(0);
}

int noteoff_mec()
{
    union meccode fd;
    int  i,j,k,d;
    int  anflg;
    char *pa,*pc,*pp;
    int  lastp;

    k = startkp_mec(&j,&lastp);
    /*
    pa = "ABCDEFGHIJ";
    pp = "abcdefghij";
    */
    pa = "JKLNPQUVYZ"; /* pol.key.p */
    pp = "jklnpquvyz"; /* velo. off */
    for(i = j; i < lastp; i++) {
        anflg = 0;
        pc = pa;
        for(d = 0; d < 10; d++) {
            if(mg->temp[i] == *pc) {
                anflg = 1;
                break;
            }
            pc++;
        }
        mr->keyp = mg->ktb[k];
        if(anflg && mg->ntb[mr->keyp]) {
            mr->dyna = d + 1;
            fd.zi[0] = 0xfd80;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            mr->keyp &= 0x00ff;
            j = mr->keyp << 8;
            fd.zi[1] |= j;
            mr->voff = mr->dyna * mg->velo;
            if(mr->voff > 127) mr->voff = 127;
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
        k++;
    }
    k = startkp_mec(&j,&lastp);
    if(mg->temp[0] == ':') {
        for(i = j; i < lastp; i++) {
            mr->keyp = mg->ktb[k];
            if(mg->ntb[mr->keyp]) {
                if(mg->temp[i] == ' ' && mg->ntb[mr->keyp] == k) {
                    fd9off_mec();
                }
            }
            k++;
        }
    }
    else {     /* mg->temp[0] != ':' */
        for(i = j; i < 69; i++) {
            anflg = 0;
            pc = pp;
            for(d = 0; d < 10; d++) {
                if(mg->temp[i] == *pc) {
                    anflg = 1;     /* qwertyuio */
                    break;
                }
                pc++;
            }
            if(!anflg) {
                mr->keyp = mg->ktb[k];
                if(mg->ntb[mr->keyp]) {
                    if(mg->temp[i] != ':' && mg->ntb[mr->keyp] == k)
                        fd9off_mec();
                }
                if(mg->temp[i] != ':' && mg->ntb[mr->keyp - 1]) {
                    if(mg->card[i] == '<' && mg->ntb[mr->keyp - 1] == k) {
                        mr->keyp = mr->keyp - 1;
                        fd9off_mec();
                    }
                }
                if(mg->temp[i] != ':' && mg->ntb[mr->keyp + 1]) {
                    if(mg->card[i] == '>' && mg->ntb[mr->keyp + 1] == k) {
                        mr->keyp = mr->keyp + 1;
                        fd9off_mec();
                    }
                }
                mr->keyp = mg->ktbbak[k];
                if(mg->ntb[mr->keyp]) {
                    if(mg->temp[i] != ':' && mg->ntb[mr->keyp] == k)
                        fd9off_mec();
                }
            }
            k++;
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

int fd9off_mec()
{
    union meccode fd;
    int  j;

    fd.zi[0] = 0xfd90;
    fd.zi[1] = 0;
    mr->chan &= 0x000f;
    fd.zi[0] |= mr->chan;
    mr->keyp &= 0x00ff;
    j = mr->keyp << 8;
    fd.zi[1] |= j;
    if(mg->debug == 1) {
        printf("Debug! fd9off_() note on chan=%d ",mr->chan);
        printf("keyp=%d trnsv=%d veon=0 (fd9off)\n",mr->keyp,mr->trnsv);
    }
    mg->ntb[mr->keyp] = 0;
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) note on chan=%d ",
        fd.zi[0],fd.zi[1],mr->chan);
        printf("keyp=%d trnsv=%d veon=0 (fd9off)\n",mr->keyp,mr->trnsv);
    }
    mg->ntb[mr->keyp] = 0;
    putcode_mec(&fd);
    return(0);
}

/* called by final */
int noteclr_mec()
{
    union meccode fd;
    int  i,j;

    for(i = 0; i < 128; i++) {
        if(mg->ntb[i]) {
            fd.zi[0] = 0xfd90;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            mr->keyp = i;
            mr->keyp &= 0x00ff;
            j = mr->keyp << 8;
            fd.zi[1] |= j;
            if(mg->debug == 1) {
                printf("Debug! noteclr_() note on chan=%d ",mr->chan);
                printf("keyp=%d trnsv=%d veon=0 (noteclr)\n",
                    mr->keyp,mr->trnsv);
            }
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note on chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d veon=0 (noteclr)\n",
                    mr->keyp,mr->trnsv);
            }
            mg->ntb[mr->keyp] = 0;
            putcode_mec(&fd);
        }
    }
    return(0);
}

int rest_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    union meccode fc;
    int  i,j;
    int  dgf;
    int  eol;
    int  rept;
    int  clrf;
    long l;
    char wk[4];
    long lagsync;
    long swing;
    char card[82];
    int  swf;

    dgf = 0;
    rept = 1;
    /* rest */
    swf = 0;
    strcpy(card,mg->card);
    for(i = 0; i < 12; i++) {
        if(card[i] == '+' || card[i] == '-') {
            card[i] = ' ';
            swf = 1;
            break;
        }
    }
    if(swf) {
        i++;
        while(isdigit(card[i])) {
            card[i] = ' ';
            i++;
        }
        card[i] = ' ';
    }
    /* printf("%s\n",card); */
    if(card[0] == 'R') {
        mr->cval = 100000;
        if(card[1] == '.') mr->cval = 150000;
        if(card[2] == '.') mr->cval = 175000;
        if(card[1] == 'w' || card[1] == 'W') {
            mr->cval = 400000;
            if(card[2] == '.') mr->cval = 600000;
            if(card[3] == '.') mr->cval = 700000;
            if(card[4] == '.') mr->cval = 750000;
        }
        if(card[1] == 'h' || card[1] == 'H') {
            mr->cval = 200000;
            if(card[2] == '.') mr->cval = 300000;
            if(card[3] == '.') mr->cval = 350000;
            if(card[4] == '.') mr->cval = 375000;
        }
    }
    if(card[0] == 'r') {
        mr->cval = 50000;
        i = 1;
        while(card[i] == 'r') {
            mr->cval = mr->cval/2;
            i++;
        }
        if(card[i] == '.') mr->cval += mr->cval/2;
        if(card[i+1] == '.') mr->cval += mr->cval/2;
    }
    /*
    eol = 0;
    while(card[eol] != ' ') eol++;
    i = 0;
    while(card[i] != ' ') i++;
    if(eol > i) eol = i;
    */
    eol = 0;
    while(card[eol] != ' ') eol++;
    i = 0;
    while(card[i] != '\0') i++;
    if(eol < i) i = eol;
    for(j = 0; j < i; j++) if(isdigit(card[j])) dgf = 1;
    if(dgf) {
        i = 0;
        while(!isdigit(card[i])) i++;
        j = 0;
        while(isdigit(card[i])) {
        wk[j] = card[i];
            i++;
            j++;
        }
        wk[j] = '\0';
        sscanf(wk,"%d",&rept);
        l = mr->cval * rept;
        if(l > 1677215) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! %06d rest repeart over %d\n",mg->line,rept);
        }
        else mr->cval = l;
    }
    swing = getswing_mec();
    if(mr->cval > swing) {
        mr->cval = mr->cval + swing;
    }
    else {
	printf("Error! %06d Swing over than rest value %ld\n",mg->line,swing);
    }
    /* printf("mr->cval=%ld swing=%ld\n",mr->cval,swing); */
    if(mg->btyp == 'r') {
        wk[0] = mg->card[11];
        wk[1] = mg->card[12];
        wk[2] = '\0'; 
        sscanf(wk,"%d",&mr->trnsv);
    }
    j = 4;
    while(card[j] != ' ') j++;
    if(j < 11) j = 11;
    dgf = 0;
    for(i = j; i < 69; i++) {
        if(card[i] != ' ' && card[i] != ':') dgf = 1;
    }
    clrf = 0;
    for(i = 0; i < 128; i++) if(mg->ntb[i]) clrf = 1;
    if(dgf || clrf) {
        accid_mec(mk);
        queset_mec(mr->sync);
        syncout_mec(mr->sync,1); /* lag */
        strcpy(mg->temp,mg->card);
        polfkey_mec();
        noteoff_mec();
        fraseck_mec(mr->sync);
    }
    else syncout_mec(mr->sync,1); /* lag */
    mr->sync -= mr->cval;
    /* if(mr->sync > 0xffffff || mr->sync < 0) { org */
    if(mr->sync > 0xffffff || mr->sync < -6) { /* swing hosei */
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! Rest synchronize bar = %d\n",mr->bnum);
        return(-1);
    }
    return(0);
}

int befnote_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    accid_mec(mk);
    return(0);
}

int queset_mec(long cval)
//long cval;
{
    int  i;

    mg->ques[0] = mr->psyn;                /* all  */
    mg->ques[1] = mr->psyn - cval/9;       /* r1 */
    mg->ques[2] = mr->psyn - cval/7;       /* w1 */
    mg->ques[3] = mr->psyn - cval/5;       /* m1 */
    mg->ques[4] = mr->psyn - (cval/9)*2;   /* r2 */
    mg->ques[5] = mr->psyn - cval/4;       /* s1 */
    mg->ques[6] = mr->psyn - (cval/7)*2;   /* w2 */
    mg->ques[7] = mr->psyn - cval/3;       /* t1,r3 */
    mg->ques[8] = mr->psyn - (cval/5)*2;   /* m2 */
    mg->ques[9] = mr->psyn - (cval/7)*3;   /* w3 */
    mg->ques[10] = mr->psyn - (cval/9)*4;  /* r4 */
    mg->ques[11] = mr->psyn - (cval/4)*2;  /* s2 */
    mg->ques[12] = mr->psyn - (cval/9)*5;  /* r5 */
    mg->ques[13] = mr->psyn - (cval/7)*4;  /* w4 */
    mg->ques[14] = mr->psyn - (cval/5)*3;  /* m3 */
    mg->ques[15] = mr->psyn - (cval/3)*2;  /* t2,r6 */
    mg->ques[16] = mr->psyn - (cval/7)*5;  /* w5 */
    mg->ques[17] = mr->psyn - (cval/4)*3;  /* s3 */
    mg->ques[18] = mr->psyn - (cval/9)*7;  /* r7 */
    mg->ques[19] = mr->psyn - (cval/5)*4;  /* m4 */
    mg->ques[20] = mr->psyn - (cval/7)*6;  /* w6 */
    mg->ques[21] = mr->psyn - (cval/9)*8;  /* r8 */
    if(mg->debug == 3) {
        for(i = 0; i < 22; i++) 
            printf("Debug3!  mg->ques[%d]=%ld\n",i,mg->ques[i]);
    }
    return(0);
}

int efect_mec()
{
    int  i,j,k;
    int  tril;
    int  turn;
    int  mord;
    int  dmor;
    int  dtril;
    int  alef;
    int  lastp;

    tril = 0;
    turn = 0;
    mord = 0;
    dmor = 0;
    dtril = 0;
    alef = 0;
    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        switch(mg->temp[i]) {
            case 't' : tril = 1;
                       alef = 1;
                       break;
            case 'T' : tril = 2;
                       alef = 1;
                       break;
            case 's' : turn = 1;
                       alef = 1;
                       break;
            case 'S' : turn = 2;
                       alef = 1;
                       break;
            case 'm' : mord = 1;
                       alef = 1;
                       break;
            case 'M' : mord = 2;
                       alef = 1;
                       break;
            case 'w' : dmor = 1;
                       alef = 1;
                       break;
            case 'W' : dmor = 2;
                       alef = 1;
                       break;
            case 'r' : dtril = 1;
                       alef = 1;
                       break;
            case 'R' : dtril = 2;
                       alef = 1;
                       break;
            default  : break;
        }
    }
    mg->quep = 0;
    if(alef) alefec_mec();                    /*  0 */
    mg->quep = 1;
    if(dtril) {                               /*  1 */
        quecode_mec();
        dtril_mec();
    }
    mg->quep = 2;
    if(dmor) dmordent_mec();                  /*  2 */
    mg->quep = 3;
    if(mord) mordent_mec();                   /*  3 */
    mg->quep = 4;
    if(dtril) {                               /*  4 */
        quecode_mec();
        dtril_mec();
    }
    mg->quep = 5;
    if(turn) turn_mec();                      /*  5 */
    mg->quep = 6;
    if(dmor) dmordent_mec();                  /*  6 */
    mg->quep = 7;
    if(tril || dtril) quecode_mec();
    if(tril) tril_mec();                      /*  7 */
    if(dtril) dtril_mec();                    /*  7 */
    mg->quep = 8;
    if(mord) mordent_mec();                   /*  8 */
    mg->quep = 9;
    if(dmor) dmordent_mec();                  /*  9 */
    mg->quep = 10;
    if(dtril) {                               /* 10 */
        quecode_mec();
        dtril_mec();
    }
    mg->quep = 11;
    if(turn) turn_mec();                      /* 11 */
    mg->quep = 12;
    if(dtril) {                               /* 12 */
        quecode_mec();
        dtril_mec();
    }
    mg->quep = 13;
    if(dmor) dmordent_mec();                  /* 13 */
    mg->quep = 14;
    if(mord) mordent_mec();                   /* 14 */
    mg->quep = 15;
    if(tril || dtril) quecode_mec();
    if(tril) tril_mec();                      /* 15 */
    if(dtril) dtril_mec();                    /* 15 */
    mg->quep = 16;
    if(dmor) dmordent_mec();                  /* 16 */
    mg->quep = 17;
    if(turn) turn_mec();                      /* 17 */
    mg->quep = 18;
    if(dtril) {                               /* 18 */
        quecode_mec();
        dtril_mec();
    }
    mg->quep = 19;
    if(mord) mordent_mec();                   /* 19 */
    mg->quep = 20;
    if(dmor) dmordent_mec();                  /* 20 */
    mg->quep = 21;
    if(dtril) {                               /* 21 */
        quecode_mec();
        dtril_mec();
    }
    return(0);
}

int alefec_mec()
{
    int  i,j,k;
    int  tril;
    int  turn;
    int  mord;
    char wk[8];
    int  kk;
    int  lastp;

    if(mg->befr[0] == '+') {
        wk[0] = mg->befr[69];
        wk[1] = mg->befr[70];
        wk[2] = mg->befr[71];
        wk[3] = mg->befr[72];
        wk[4] = '\0';
        sscanf(wk,"%f",&mg->velo);
    }
    k = startkp_mec(&j,&lastp);
    tril = 0;
    turn = 0;
    mord = 0;
    for(i = j; i < lastp; i++) {
        if(mg->temp[i] == 't' || mg->temp[i] == 'T') tril = 1;
        if(mg->temp[i] == 's' || mg->temp[i] == 'S') turn = 1;
        if(mg->temp[i] == 'm' || mg->temp[i] == 'M') mord = 1;
        if(mg->temp[i] == 'w' || mg->temp[i] == 'W') mord = 2;
        if(mg->temp[i] == 'r' || mg->temp[i] == 'R') tril = 2;
    }
    if(tril == 1) {
        kk = k;
        if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/3;
        else mr->wval = mr->cval/3;
        /* tril */
        for(i = j; i < 69; i++) {
            if(mg->temp[i] == 't') mr->dyna = fr->tfdyn;
            if(mg->temp[i] == 'T') mr->dyna = fr->Tfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->temp[i]) {
                case 't' : mr->keyp = mg->ktb[kk];
                           fdcode_mec();
                           break;
                case 'T' : mr->keyp = mg->ktb[kk+1];
                           fdcode_mec();
                           break;
                default  : break;
            }
            kk++;
        }
    }
    if(turn) {
        kk = k;
        if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/4;
        else mr->wval = mr->cval/4;
        /* turn */
        for(i = j; i < 69; i++) {
            if(mg->temp[i] == 's') mr->dyna = fr->sfdyn;
            if(mg->temp[i] == 'S') mr->dyna = fr->Sfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->temp[i]) {
                case 's' : mr->keyp = mg->ktb[kk];
                           fdcode_mec();
                           break;
                case 'S' : mr->keyp = mg->ktb[kk];
                           fdcode_mec();
                           break;
                default  : break;
            }
            kk++;
        }
    }
    if(mord == 1) {
        kk = k;
        if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/5;
        else mr->wval = mr->cval/5;
        /* mordent */
        for(i = j; i < 69; i++) {
            if(mg->temp[i] == 'm') mr->dyna = fr->mfdyn;
            if(mg->temp[i] == 'M') mr->dyna = fr->Mfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->temp[i]) {
                case 'm' : mr->keyp = mg->ktb[kk];
                           fdcode_mec();
                           break;
                case 'M' : mr->keyp = mg->ktb[kk+1];
                           fdcode_mec();
                           break;
                default  : break;
            }
            kk++;
        }
    }
    if(mord == 2) {
        kk = k;
        if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/7;
        else mr->wval = mr->cval/7;
        /* mordent */
        for(i = j; i < 69; i++) {
            if(mg->temp[i] == 'w') mr->dyna = fr->wfdyn;
            if(mg->temp[i] == 'W') mr->dyna = fr->Wfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->temp[i]) {
                case 'w' : mr->keyp = mg->ktb[kk];
                           fdcode_mec();
                           break;
                case 'W' : mr->keyp = mg->ktb[kk+1];
                           fdcode_mec();
                           break;
                default  : break;
            }
            kk++;
        }
    }
    if(tril == 2) {
        kk = k;
        if(mg->temp[0] == ':') mr->wval = (mr->nval - mr->cval)/9;
        else mr->wval = mr->cval/9;
        /* tril */
        for(i = j; i < 69; i++) {
            if(mg->temp[i] == 'r') mr->dyna = fr->rfdyn;
            if(mg->temp[i] == 'R') mr->dyna = fr->Rfdyn;
            if(isdigit(mg->befr[i])) mr->dyna = mg->befr[i] - '0';
            mr->veon = mr->dyna * mg->velo;
            switch(mg->temp[i]) {
                case 'r' : mr->keyp = mg->ktb[kk];
                           fdcode_mec();
                           break;
                case 'R' : mr->keyp = mg->ktb[kk+1];
                           fdcode_mec();
                           break;
                default  : break;
            }
            kk++;
        }
    }
    return(0);
}

int fdcode_mec()
{
    union meccode fd;
    int  j;

    fd.zi[0] = 0xfd90;
    fd.zi[1] = 0;
    mr->chan &= 0x000f;
    fd.zi[0] |= mr->chan;
    mr->keyp &= 0x00ff;
    j = mr->keyp << 8;
    fd.zi[1] |= j;
    if(mr->veon > 127) mr->veon = 127;
    mr->veon &= 0x00ff;
    fd.zi[1] |= mr->veon;
    if(mg->debug == 1) {
        printf("Debug! fdcode_() note on chan=%d ",mr->chan);
        printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
        mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
    }
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) note on chan=%d ",
        fd.zi[0],fd.zi[1],mr->chan);
        printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
        mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
    }
    mg->ntb[mr->keyp] = 1;
    putcode_mec(&fd);
    return(0);
}

int fd9code_mec()
{
    union meccode fd;
    int  j;

    fd.zi[0] = 0xfd90;
    fd.zi[1] = 0;
    mr->chan &= 0x000f;
    fd.zi[0] |= mr->chan;
    mr->keyp &= 0x00ff;
    j = mr->keyp << 8;
    fd.zi[1] |= j;
    if(mg->debug == 1) {
        printf("Debug! fd9code_() note on chan=%d ",mr->chan);
        printf("keyp=%d trnsv=%d veon=0 (fd9code)\n",mr->keyp,mr->trnsv);
    }
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) note on chan=%d ",
        fd.zi[0],fd.zi[1],mr->chan);
        printf("keyp=%d trnsv=%d veon=0 (fd9code)\n",mr->keyp,mr->trnsv);
    }
    mg->ntb[mr->keyp] = 0;
    putcode_mec(&fd);
    return(0);
}

int startkp_mec(int *startp,int *lastp)
//int *startp;
//int *lastp;
{
    int  k;

    *startp = 11; // 2015.5.15
    *lastp = 69;
    k = 6;    // mg->btyp == 'm'
    if(mg->btyp == 'm') k = 6;
    if(mg->btyp == 'l') k = 1;
    if(mg->btyp == 'h') k = 17;

    /*
    *startp = 10;  // 2015.3.6
    *lastp = 69;
    k = 5;    // mg->btyp == 'm'
    if(mg->btyp == 'm') k = 5;
    if(mg->btyp == 'l') k = 0;
    if(mg->btyp == 'h') k = 16;
    */

    if(mg->btyp == 's' || mg->btyp == 'd') {
        k = 21;
        *startp = 32;
    }
    if(mg->btyp == 't') {
        k = 35;
        *startp = 32;
    }
    if(mg->btyp == 'g') {
        k = 28;
        *startp = 32;
    }
    if(mg->btyp == 'a') {
        k = 14;
        *startp = 32;
    }
    if(mg->btyp == 'b') {
        k = 7;
        *startp = 32;
    }
    if(mg->btyp == 'u') {
        k = 0;
        *startp = 32;
    }
    return(k);
}

long getswing_mec()
{
    int  i,j,k;
    long  swing;
    int  sf;
    char wk[16];
    float unit;
    float cunit;
    float tempo;

    swing = 0;
    sf = 0;
    if(mg->card[0] == '%') k = 4; 
    if(mg->card[0] == 's') k = 4; 
    if(mg->card[0] == 'c') k = 4; 
    if(mg->card[0] == 'R') k = 0; 
    if(mg->card[0] == 'r') k = 0; 
    for(i = k; i < 12; i++) {
	if(mg->card[i] == '+') {
	    sf = 1;
	    break;
        }
	if(mg->card[i] == '-') {
            sf = -1;
            break;
        }
        if(mg->card[i] == ' ') break;
    }
    if(sf == 0) return(0);
    j = 0;
    i++;
    while(isdigit(mg->card[i])) {
        wk[j] = mg->card[i];
	i++;
        j++;
        if(i > 12) {
            printf("Error! %06d Swing data\n",mg->line);
            return(0);
	}
    }
    wk[j] = '\0';
    sscanf(wk,"%ld",&swing);
    /* printf("unit = %c\n",mg->card[i]); */
    if(mg->card[i] == ' ') swing = swing * sf * 100; /* + - 1/100 nval */
    /* -------- Comment out 2013.11.29
    else {
	unit = 0;
        switch(mg->card[i]) {
            case 'f' : unit = 1.0;       // flame 
                       break;
            case 'q' : unit = 1.0/4.0;   // MTC 
                       break;
            case 't' : unit = 1.0/8.0;   // SMPTE Time code 
                       break;
            case 'b' : unit = 1.0/80.0;  // Bit flame 
                       break;
            case 'd' : unit = 1.0/100.0; // 1/100 flame 
                       break;
            case 'm' : unit  = 30.0/1000.0; // msec 
                       break;
            case 's' : unit = 30.0;         // Sec 
                       break;
            default  : break;
        }
        if(mg->debug) printf("getswing_() unit=%f mr->temp=%d",unit,mr->temp);
        // tempo = mr->temp * nvt[mr->tsym]/1000.0;
        cunit = unit * 6000/mr->temp * 100000;
        cunit = cunit / 30; // flame
        swing = swing * cunit * sf;
        if(mg->debug) {
            printf(" cunit=%f swing=%d\n",cunit,swing);
        }
    }
    ----------------  */
    if(mg->trace) {
        printf("Trace! input val=%s swing=%ld\n",wk,swing);
    }
    return(swing);
}

long getnacv_mec(char *card)
//char *card;
{
    int  i;
    int  l;
    long nval;
    long ll;

    nval = 0;
    if(Nachf == 1) Nachf = 2;
    l = 0;
    if(card[4] == '.') {
        i = 6;
        /* mr->nval = 50000; */
        nval = 100000;
        while(card[i] == ')') {
            i++;
            l++;
            if(l > 3) {
                return(0); /* limit over */
            }
            nval = nval / 2;
        }
        if(card[i] == '.') {
            if(l == 3) {
                return(0); /* limit over */
            }
            ll = nval / 2;
            nval = ll + nval;
            i++;
            if(card[i] == '.') {
                if(l == 2) {
                    /* printf("Error1! %06d ./>>.. limit over\n",mg->line); */
                    return(1);
                }
                nval = ll/2 + nval;
                i++;
            }
        }
    }
    return(nval);
}

