
// ----------------------
//  AMuPLC (mec3.c)
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
extern MECSYM *ms;   /* symbol table */
extern FILE  *fpi;
extern char scsv[MAXSC][82];      /* card save area */
extern int  scsn;                 /* card save number count */
/* extern long getswing_mec(); */
float setrubart_mec();
float rubart;
float lsm;  /* last smptef */
float bsm;  /* before smptef */
float csm;  /* current smpte */
extern int  spsf;  // SMPTE pass flag
extern union meccode sb1,sbf;  // 2019.3.31

/* master note */
int mnote_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    union meccode fc;
    int  i,j,k;
    int  l,s;
    long ll;
    int  ecod;
    int  eeod;
    int  pasf;
    long nval;
    int  accia;
    int  sousy;
    char wk[10];
    int  hosei;
    int  nclr;
    long lagsync;
    long offsync;
    /* long swing; */
    long nacv;

    scsn = 0;
    pasf = 0;
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
    i = 5;
    hosei = 0;
    accia = 0;
    sousy = 0;
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
        if(mg->card[i] == ')') sousy = 1;
        if(mg->card[i] == '>') accia = 1;
        while(mg->card[i] == ')' || mg->card[i] == '>') {
            /* if(mg->card[i] == '>') accia = 1; */
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
        if(sousy) mr->sous += mr->nval;
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
    /*
     swing = getswing_mec();
    if(mr->nval > swing) ll = mr->nval + swing;
    else { 
        ll = mr->nval;
        printf("Error! %06d Swing over than note value %ld\n",mg->line,swing);
    }   
    */
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
    if(mg->smpmid == 'm') {
        wk[0] = mg->card[39];
        wk[1] = mg->card[40];
        wk[2] = '\0';
        sscanf(wk,"%d",&mr->trnsv);
        wk[0] = mg->card[43];
        wk[1] = mg->card[44];
        wk[2] = mg->card[45];
        wk[3] = mg->card[46];
        wk[4] = '\0';
    sscanf(wk,"%f",&mg->velo);
    }
    j = 12;
    ecod = 0;
    nclr = 0;
    for(i = j; i < 33; i++) {
        if((mg->card[i] != ' ') && (mg->card[i] != '.')) ecod = 1;
        if(isdigit(mg->card[i]) || mg->card[i] == ':') nclr = 1;
    }
    if(!nclr) noteclr_mec();
    eeod = 0;
    if(mg->temp[0] == ':') {
        j = 12;
        for(i = j; i < 33; i++) {
            if((mg->temp[i] != ' ') && (mg->temp[i] != '.')) eeod = 1;
        }
    }
    if(mg->trace && ecod) {
        printf("Trace! nval=%ld splt=%d renp=%d ",
        ll,mr->splt,mr->renp);
        printf("rept = %d hosei = %d --------\n",mr->rept,hosei);
    }
    for(s = 0; s < mr->splt; s++) {
        /* fc code */
        nval = mr->nval;
        syncout_mec(mr->sync,1); /* lag */
        /* f1 f2 fa code */
        f1code_mec();
        mdynamic_mec();
        mstmpo_mec();
        /* note */
        if(!accia) {
            nval -= mr->acci;
            mr->acci = 0;
        }
        mr->cval = nval*mr->pport/100;
        /* w.cval = mr->cval; */
        mr->psyn = mr->sync;
        strcpy(mg->temp,mg->card);
        if(ecod) {
            if(mg->smpmid == 'm') stickon_mec();
        }
        fraseck_mec(mr->sync);
        /* sync */
        mr->psyn = mr->sync - mr->cval;
        mr->sync = mr->sync - nval;
        /* if(mr->sync > 0xffffff || mr->sync < 0) { org */
        if(mr->sync > 0xffffff || mr->sync < -6) { /* swing hosei */
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! mnote synchronize bar = %d\n",mr->bnum);
            return(-1);
        }
        if(ecod || eeod) {
            /* fc code */
            syncout_mec(mr->psyn,2); /* offset */
            /* w.cval = mr->nval - mr->cval; */
            strcpy(mg->befr,mg->card);
            if(mg->next[0] == '-') strcpy(mg->temp,mg->next2);
            else strcpy(mg->temp,mg->next);
            if(mg->smpmid == 'm') stickoff_mec();
            fraseck_mec(mr->psyn);
        }
    }
    mr->sync -= hosei;
    if(scsn > 1) mpasproc_mec(mk);
    setrubart_mec();
    if(rubart) mg->rbaf = 1;  /* rubart active on */
    return(0);
}

int mpasproc_mec(struct mecksn *mk)
//struct mecksn *mk;         /* key signeture table */
{
    int  i,k;
    char svcard[82];
    union meccode ff;

    /*
    if(mg->debug) {
        printf("Debug! mpasproc_mec() <----------\n");
    }
    */
    spsf = 0;
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
                        if(mg->card[12] == 'p') mg->smpmid = 's';
                        if(mg->card[12] == '0') mg->smpmid = 'm';
                        if(spsf) {
                            if(mg->trace) printf("Trace! B1 Smpte out\n"); 
                            putcode_mec(&sb1);
                            putcode_mec(&sbf);
                            spsf = 0;
                        }
                        break;
            case '['  : frasing_mec();
                        break;
            case '{'  : if(mg->card[1] == '$') {
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
            //case '$'  : keyryhm_mec(&mk);
            case '$'  : keyryhm_mec(mk);      // 2025.4.6
                        break;
            case 'S'  : if(mg->card[1] == 'm') {
                            smptcode_mec();
                            // spsf = 1;
                        }
                        break;
            //case '+'  : befnote_mec(&mk);
            case '+'  : befnote_mec(mk);  // 2025.4.6
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

int stickon_mec()
{
    union meccode fd;
    int  i,j;

    j = 12;
    for(i = j; i < 33; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            mr->dyna = mg->card[i] - '0';
            mr->keyp = i - j;
            if(mg->card[i] == '>') {
                mr->dyna = 5; 
                mr->keyp = mr->keyp + 1;
            }
            if(mg->card[i] == '<') {
                mr->dyna = 5;  
                mr->keyp = mr->keyp - 1;
            }
            if(mr->keyp > 127) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! note on key position over 127\n");
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
                printf("Debug! tickon_() note on chan=%d ",mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note on chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
            putcode_mec(&fd);
        }
    }
    return(0);
}

int stickoff_mec()
{
    union meccode fd;
    int  i,j;

    j = 12;
    for(i = j; i < 33; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            mr->keyp = i - j;
            if(mg->card[i] == '>') mr->keyp = mr->keyp + 1; 
            if(mg->card[i] == '<') mr->keyp = mr->keyp - 1; 
            if(mr->keyp > 127) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! note on key position over 127\n");
            }
            fd.zi[0] = 0xfd90;
            fd.zi[1] = 0;
            mr->chan &= 0x000f;
            fd.zi[0] |= mr->chan;
            mr->keyp &= 0x00ff;
            j = mr->keyp << 8;
            fd.zi[1] |= j;
            if(mg->trace) {
                printf("Trace! FD(%04x%04x) note off chan=%d ",
                fd.zi[0],fd.zi[1],mr->chan);
                printf("keyp=%d trnsv=%d veon=0\n",mr->keyp,mr->trnsv);
            }
            putcode_mec(&fd);
        }
    }
    return(0);
}

/*
SMPTE time code : F1xxxxxx
 xxxxxx      : time code (hour * minites * seconds * frame)
 max = 24h * 60m * 60s * 30f = 2592000 + 108000 + 1800 + 30 = 2701830
 max = 108000 * hour + 1800 * minites + 30 * seconds + frame
*/
int f1code_mec()
{
    union meccode f1;
    char wk[8];
    long lhv;
    long lmv;
    long lsv;
    int  lsw;
    unsigned int tcbit;

    if(mg->card[78] == 'x') return(0);
    wk[0] = mg->card[74];
    wk[1] = mg->card[75];
    wk[2] = mg->card[76];
    wk[3] = mg->card[77];
    wk[4] = mg->card[78];
    wk[5] = '\0';
    sscanf(wk,"%f",&mg->smptef);
    wk[0] = mg->card[71];
    wk[1] = mg->card[72];
    wk[2] = '\0';
    sscanf(wk,"%d",&mg->smptes);
    wk[0] = mg->card[68];
    wk[1] = mg->card[69];
    wk[2] = '\0';
    sscanf(wk,"%d",&mg->smptem);
    wk[0] = mg->card[65];
    wk[1] = mg->card[66];
    wk[2] = '\0';
    sscanf(wk,"%d",&mg->smpteh);
    lhv = (long)mg->smpteh * 108000;
    lmv = (long)mg->smptem * 1800;
    lsv = (long)mg->smptes * 30;
    bsm = csm;
    csm = lhv + lmv + lsv + mg->smptef;
    if(mg->trace) {
        printf("Trace! mg->rbaf=%d lsm=%f csm=%f bsm=%f rubart=%f\n",
        mg->rbaf,lsm,csm,bsm,rubart);
    }
    if(mg->rbaf) mg->smpte = lsm + csm - bsm + rubart;
    else mg->smpte = lhv + lmv + lsv + mg->smptef;
    lsm = mg->smpte;
    /*
    printf("lhv=%ld lmv=%ld lsv=%ld (long)mg->smptef=%ld\n",
        lhv,lmv,lsv,(long)mg->smptef);
    */
    lsconv_h(&f1,(long)mg->smpte);
    f1.zi[0] &= 0x00ff;
    f1.zi[0] |= 0xf100;
    if(mg->trace) {
        printf("Trace! F1(%04x%04x) SMPTE(%02d:%02d:%02d:%05.2f)",
        f1.zi[0],f1.zi[1],mg->smpteh,mg->smptem,mg->smptes,mg->smptef);
        // printf(" mg->smpte=%ld\n",(long)mg->smpte);
        printf(" mg->smpte=%f\n",mg->smpte);  // 2013.11.28
    }
    putcode_mec(&f1);
    if(mg->smpte == 0) lsw = 0;
    /* if(mg->smpte <= 0) lsw = 0;   mod 2001.8 */
    else lsw = (mg->smpte - (long)mg->smpte) * 100;
    lsconv_h(&f1,lsw);
    f1.zi[0] &= 0x00ff;
    f1.zi[0] |= 0xf180;
    tcbit = mg->tctyp;
    tcbit = tcbit << 4;
    tcbit &= 0x0070;
    f1.zi[0] |= tcbit;
    if(mg->trace) {
        printf("Trace! F1(%04x%04x) f1code_() flactional flame=%d\n",
        f1.zi[0],f1.zi[1],lsw);
    }
    putcode_mec(&f1);
    return(0);
}
/* smpte start f1 code
Smpte flame type = 2
Smpte start time = 00:00:00/00.00
Smpte lag time = 00:00:05/00.00
*/
int smptcode_mec()
{
    union meccode b1,f4;
    char wk[8];
    long lhv;
    long lmv;
    long lsv;
    int  lsf;
    int  lsx;
    long lsw;
    int  lagh;
    int  lagm;
    int  lags;
    float lagf;
    long lag;
    unsigned int tcbit;
    int  i;
    int  loop;

    // if(mg->card[6] == 'l') return(0); // 2019.3.30 
    // No Out F4 code 2019.3.30
    if(mg->card[6] == 'l') {
        if(mg->card[30] == 'x') return(0);
        wk[0] = mg->card[26];
        wk[1] = mg->card[27];
        wk[2] = mg->card[28];
        wk[3] = mg->card[29];
        wk[4] = mg->card[30];
        wk[5] = '\0';
        sscanf(wk,"%f",&lagf);
        wk[0] = mg->card[23];
        wk[1] = mg->card[24];
        wk[2] = '\0';
        sscanf(wk,"%d",&lags);
        wk[0] = mg->card[20];
        wk[1] = mg->card[21];
        wk[2] = '\0';
        sscanf(wk,"%d",&lagm);
        wk[0] = mg->card[17];
        wk[1] = mg->card[18];
        wk[2] = '\0';
        sscanf(wk,"%d",&lagh);
        if(mg->trace) {
            printf("Trace! Smpte lag time (%02d:%02d:%02d:%05.2f)\n",
                lagh,lagm,lags,lagf);
        }
        loop = lagh * 60;
        for(i = 0 ; i < loop; i++) {
            lag = 3600 * 300;
            lsconv_h(&f4,lag);
            f4.zi[0] &= 0x00ff;
            f4.zi[0] |= 0xf400;
            if(mg->trace) printf("Trace! F4(%04x%04x) lag=%ld\n",
                f4.zi[0],f4.zi[1],lag);
            putcode_mec(&f4);
        }
        for(i = 0 ; i < lagm; i++) {
            lag = 3600 * 300;
            lsconv_h(&f4,lag);
            f4.zi[0] &= 0x00ff;
            f4.zi[0] |= 0xf400;
            if(mg->trace) printf("Trace! F4(%04x%04x) lag=%ld\n",
                f4.zi[0],f4.zi[1],lag);
            putcode_mec(&f4);
        }
        lsv = lags * 3000;
        lsx = (int)lagf * 100;
        lag = lsv + lsx;
        lsconv_h(&f4,lag);
        f4.zi[0] &= 0x00ff;
        f4.zi[0] |= 0xf400;
        if(mg->trace) printf("Trace! F4(%04x%04x) lag=%ld\n",
                f4.zi[0],f4.zi[1],lag);
        putcode_mec(&f4);
        return(0);
    }
    // ---------------------------------------------------------------
    // 

    /* flame return */
    if(mg->card[6] == 'f') {
        mg->tctyp = numdin_mec(mg->card);
        return(0);
    }
    if(mg->card[32] == 'x') return(0);
    wk[0] = mg->card[28];
    wk[1] = mg->card[29];
    wk[2] = mg->card[30];
    wk[3] = mg->card[31];
    wk[4] = mg->card[32];
    wk[5] = '\0';
    sscanf(wk,"%f",&mg->smptef);
    wk[0] = mg->card[25];
    wk[1] = mg->card[26];
    wk[2] = '\0';
    sscanf(wk,"%d",&mg->smptes);
    wk[0] = mg->card[22];
    wk[1] = mg->card[23];
    wk[2] = '\0';
    sscanf(wk,"%d",&mg->smptem);
    wk[0] = mg->card[19];
    wk[1] = mg->card[20];
    wk[2] = '\0';
    sscanf(wk,"%d",&mg->smpteh);
    lhv = (long)mg->smpteh * 108000;
    lmv = (long)mg->smptem * 1800;
    lsv = (long)mg->smptes * 30;
    mg->smpte = lhv + lmv + lsv + mg->smptef;
    /*
    printf("lhv=%ld lmv=%ld lsv=%ld (long)mg->smptef=%ld\n",
        lhv,lmv,lsv,(long)mg->smptef);
    */
    lsconv_h(&b1,(long)mg->smpte);
    b1.zi[0] &= 0x00ff;
    b1.zi[0] |= 0xb100;
    if(mg->trace) {
        printf("Trace! B1(%04x%04x) SMPTE(%02d:%02d:%02d:%05.2f)",
        b1.zi[0],b1.zi[1],mg->smpteh,mg->smptem,mg->smptes,mg->smptef);
        printf(" mg->smpte=%ld\n",(long)mg->smpte);
    }
    // putcode_mec(&b1);
    sb1 = b1;  // 2019.3.31
    lsx = (int)mg->smptef * 100;
    lsf = mg->smptef * 100;
    lsw = lsf - lsx;
    /* printf("lsx =%d lsf=%d lsw=%ld\n",lsx,lsf,lsw); */
    lsconv_h(&b1,lsw);
    b1.zi[0] &= 0x00ff;
    b1.zi[0] |= 0xb180;
    tcbit = mg->tctyp;
    tcbit = tcbit << 4;
    tcbit &= 0x0070;
    b1.zi[0] |= tcbit;
    // putcode_mec(&b1);
    sbf = b1;  // 2019.3.31
    if(mg->trace) {
        printf("Trace! B1(%04x%04x) smptcode_() flactional flame=%ld\n",
        b1.zi[0],b1.zi[1],lsw);
    }
    spsf = 1;
    return(0);
}

/*   Master tempo set 
     Tempo        : FAxxyyyy    xx > (100 - 200)
     xx(tsym)     : tempo symbol no.
     yyyy(temp)   : tempo 
                    ex. [allegro]  xx(tsym) = 5x
                                   yyyy(temp) = xxxx
                  : FAwwyyyy    ww < 50
     ww(tsym)     : note type 0-21
     yyyy(temp)   : tempo val *100
                    ex. [m.m. _/ = 90] ww(tsym) = 9
                                       yyyy(temp) = 9000
                    use mr->nval,mr->tsym,mr->temp
*/
int mstmpo_mec()
{
    union meccode fa;
    char wk[8];
    float tempo;
    int  tsym;
    int  s;

    wk[0] = mg->card[49];
    wk[1] = mg->card[50];
    wk[2] = mg->card[51];
    wk[3] = mg->card[52];
    wk[4] = mg->card[53];
    wk[5] = mg->card[54];
    wk[6] = '\0';
    sscanf(wk,"%f",&tempo);
    fa.zi[0] = 0xfa00;
    /* s = mr->tsym; */
    tsym = 49; /* 1997.12 */
    tsym &= 0x00ff;
    fa.zi[0] |= tsym;
    s = tempo * 100;
    fa.zi[1] = s;
    if(mg->trace) {
        printf("Trace! FA(%04x%04x) tsym=%d tempo=%d\n",
        fa.zi[0],fa.zi[1],tsym,s);
    }
    putcode_mec(&fa);
    return(0);
}

/*     Master velocity(dynamic) : F2xxyyyy
       xx          : reserve
       yyyy        : dynamic
       ex. 68.2% = 6820
*/
int mdynamic_mec()
{
    union meccode f2;
    char wk[8];
    float dynamic;
    int  s;

    wk[0] = mg->card[57];
    wk[1] = mg->card[58];
    wk[2] = mg->card[59];
    wk[3] = mg->card[60];
    wk[4] = mg->card[61];
    wk[5] = '\0';
    sscanf(wk,"%f",&dynamic);
    f2.zi[0] = 0xf200;
    s = dynamic * 10;
    f2.zi[1] = s;
    if(mg->trace) {
        printf("Trace! F2(%04x%04x) dynamic=%d\n",f2.zi[0],f2.zi[1],s);
    }
    putcode_mec(&f2);
    return(0);
}

/* master file control F0xxyyyy xx : file type , yyyy : file number */
int mfilec_mec()
{
    union meccode f0;
    int  i,j,memb;
    int  track; /* add 97.10 */
    int  c;
    int  pass;
    char wk[8];
    unsigned int fnum;
    char ch;

    /* Comment out 97.10
    j = 0;
    for(i = 71; i < 74; i++,j++) wk[j] = mg->card[i];
    wk[j] = '\0';
    sscanf(wk,"%d",&memb);
    */
    wk[0] = mg->card[3];
    wk[1] = mg->card[4];
    wk[2] = '\0';
    sscanf(wk,"%02x",&track);
    f0.zi[0] = 0xf000;
    j = 0;
    for(i = 8 ; i < 44; i++,j++) {
        ch = mg->card[i];
        if( ch != '.') {
            pass = 0;
            if(mg->allf) {
                c = toupper(ch);
                pass = 1;
            }
            else {
                if(isupper(ch)) {
                    c = ch;
                    pass = 1;
                }
            }
            if(pass) {
                c &= 0x00ff;
                f0.zi[0] |= c;
                /* fnum = j*1000 + (memb - 1);  A18 = 10000 + 18 = 10018 */
                fnum = j*1000 + track;  /* A18 = 10000 + 18 = 10018 */
                f0.zi[1] = fnum;
                if(mg->trace) {
                    printf("Trace! F0(%04x%04x) ftype=%c, fnum=%d\n",
                        f0.zi[0],f0.zi[1],c,fnum);
                }
                putcode_mec(&f0);
                mfilec3_mec();
            }
        }
    }
    return(0);
}

/* master file control F3xxyyyy xx : file type , yyyy : file number */
int mfilec3_mec()
{
    union meccode f3;
    int  i,j;
    int  channel;
    int  port;
    char wk[8];

    j = 0;
    for(i = 64; i < 67; i++,j++) wk[j] = mg->card[i];
    wk[j] = '\0';
    sscanf(wk,"%d",&port);
    f3.zi[0] = 0xf300;
    channel = 0;
    for(i = 46 ; i < 62; i++,channel++) {
        if(mg->card[i] == 'S') break;
    }
    channel &= 0x000f;
    f3.zi[0] |= channel;
    f3.zi[1] = port;
    if(mg->trace) {
        printf("Trace! F3(%04x%04x) channel=%d port=%d\n",
            f3.zi[0],f3.zi[1],channel,port);
    }
    putcode_mec(&f3);
    return(0);
}

int ffcode_mec(union meccode *ff)
//union meccode *ff;
{
    int  k;

    mr->ffflg = 1;
    ff->zi[0] = 0xff00;
    k = mr->port & 0x0f;
    ff->zi[0] |= (k << 4);
    k = mr->chan & 0x0f;
    ff->zi[0] |= k;
    k = mr->trak << 8;
    ff->zi[1] = 0;
    ff->zi[1] |= k;
    k = mr->memb & 0x00ff;
    ff->zi[1] |= k;
    if(mg->trace) {
        printf("Trace! FF(%04x%04x) /port=%d chan=%d trak=%d memb=%d\n",
        ff->zi[0],ff->zi[1],mr->port,mr->chan,mr->trak,mr->memb);
    }
    putcode_mec(ff);
    return(0);
}

int faf7code_mec()
{
    union meccode fa,f7;

    if(mg->mtempo != 0) return(0);
    mg->wrncnt++;
    mg->msglvl = 2;
    if(mg->msglvl >= mg->prtlvl)
    printf("Worning! tempo not defined, set _/ = 60\n");
    mg->mtempo = 6000;
    mr->tsym = 9;   /* _/ nvt[9] */
    fa.zi[0] = 0xfa00;
    fa.zi[0] |= mg->mtsymb;
    fa.zi[1] = mg->mtempo;
    if(mg->trace)
    printf("Trace! FA(%04x%04x) mtsymb=%d, mtempo=%d\n",
        fa.zi[0],fa.zi[1],mg->mtsymb,mg->mtempo);
    putcode_mec(&fa);
    f7.zi[0] = 0xf700;
    mr->trns &= 0x00ff;
    f7.zi[0] |= mr->trns;
    f7.zi[1] = mr->inst;
    if(mg->trace) printf("Trace! F7(%04x%04x) trns=%d, inst=%d\n",
        f7.zi[0],f7.zi[1],mr->trns,mr->inst);
    putcode_mec(&f7);
    return(0);
}

int prints_mec(int line,char source[82])
//int  line;
//char source[82];
{
    char card[82];

    strcpy(card,source);
    card[73] = '\0';
    printf("%06d %s\n", line, card);
    return(0);
}

int strcheck_mec()
{
    int  i,j,n;
    char work[80];

    n = 0;
    while(mg->card[n]) n++;
    j = 0;
    while(mg->card[j] != ' ') {
        work[j] = tolower(mg->card[j]);
        j++;
    }
    work[j] = '\0';
    if(!strcmp(work,"#modify_start")) return(0);
    if(!strcmp(work,"#add")) return(0);
    if(!strcmp(work,"#sub")) return(0);
    if(!strcmp(work,"#set")) return(0);
    if(!strcmp(work,"#unset")) return(0);
    if(!strcmp(work,"#stop")) return(0);
    if(!strcmp(work,"#pose")) return(0);
    if(!strcmp(work,"#start")) return(0);
    if(!strcmp(work,"#resume")) return(0);
    if(!strcmp(work,"#reset")) return(0);
    if(!strcmp(work,"#asn")) return(0);
    if(!strcmp(work,"#assign")) return(0);
    if(!strcmp(work,"#define")) {
        while(mg->card[j] == ' ') {
            j++;
            if(j > n) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d #define statement\n",mg->line);
                return(1);
            }
        }
        i = 0;
        while(mg->card[j] != ' ') {
            work[i++] = mg->card[j];
            j++;
            if(j > n) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d #define statement\n",mg->line);
                return(1);
            }
        }
        work[i] = '\0';
        if(!strcmp(work,"member")) {
            mr->memb = numdin_mec(mg->card);
            return(0);
        }
        if(!strcmp(work,"port")) {
            mr->port = numdin_mec(mg->card);
            return(0);
        }
        if(!strcmp(work,"channel")) {
            mr->chan = numdin_mec(mg->card);
            return(0);
        }
        if(!strcmp(work,"track")) {
            mr->trak = numdin_mec(mg->card);
            return(0);
        }
        if(!strcmp(work,"trace")) {
            mg->trace = numdin_mec(mg->card);
            return(0);
        }
        if(!strcmp(work,"debug")) {
            mg->debug = numdin_mec(mg->card);
            return(0);
        }
        if(!strcmp(work,"expression")) {
            exprnum_mec();
            return(0);
        }
        if(!strcmp(work,"transpose")) {
            mr->trnsp = numdin_mec(mg->card);
            if(mg->trace) printf("Trace! Define transpose %d\n",mr->trnsp);
            return(0);
        }
        if(!strcmp(work,"time_code_type")) {
            mg->tctyp = numdin_mec(mg->card);
            if(mg->tctyp == 24) mg->tctyp = 0; /* 24 flame/sec */
            if(mg->tctyp == 25) mg->tctyp = 1; /* 25 flame/sec */
            /* if(mg->tctyp == 30) mg->tctyp = 2;  30 flame/sec drop */
            if(mg->tctyp == 30) mg->tctyp = 3; /* 30 non drop */
            return(0);
        }
        if(!strcmp(work,"direct_time_lock_port")) {
            putsync_mec(3);
            return(0);
        }
        if(!strcmp(work,"midi_time_code_port")) {
            putsync_mec(4);
            return(0);
        }
        if(!strcmp(work,"direct_time_lock_port_and_sync_start")) {
            putsync_mec(5);
            return(0);
        }
        if(!strcmp(work,"midi_time_code_port_and_sync_start")) {
            putsync_mec(6);
            return(0);
        }
        if(!strcmp(work,"direct_time_lock_sync_stop")) {
            putsync_mec(7);
            return(0);
        }
        if(!strcmp(work,"midi_time_code_sync_stop")) {
            putsync_mec(8);
            return(0);
        }
        return(0);
    }
    if(!strcmp(work,"#include")) {
        return(0);
    }
    mg->wrncnt++;
    mg->msglvl = 2;
    if(mg->msglvl >= mg->prtlvl)
    printf("Worning! %06d %s undefined\n",mg->line,work);
    return(1);
}

/*
    make multi file
        ex0_x   ->  ex0_x00.xxx return(99)  prtn = 0
        ex0_xx  ->  ex0_0xx.xxx return(0)   prtn = 36
        ex0_xxx ->  ex0_xxx.xxx return(0)   prtn = 0
        ex0/Y;X ->  ex0_002.xxx retnrn(Y)   prtn = X (fnmb = 0,fnmb = -1)
        ex0/Y;X ->  ex0_002.xxx retnrn(Y-1) prtn = X (fnmb = 1)
    modify 1989.9.26 v_1.14  _axx. : hex suport
*/
int mkmultif_h(char *infn,char *outf,char *ftyp,int fnmb,int *prtn,int *yfile,int *xfile)
//char *infn;
//char *outf;
//char *ftyp;
//int  fnmb;
//int  *prtn;
//int  *yfile;
//int  *xfile;
{
    int  i,j,n;
    char w[25];
    int  mflg;
    int  xflg;
    int  vflg;
    char *pmf;
    char *xmf;
    char xw[10];
    char *fnum;
    int  fleng;
    int  pleng;
    int  xleng;
    char *ps;

    ps = "0123456789abcdefghijklmnopqrstuvwxyz";
    xw[0] = '\0';
    xmf = infn;
    *prtn = 0;
    vflg = 0;
    xflg = 0;
    while(*(xmf++)) {
        if(*xmf == '(') break;
        if(*xmf == '/') vflg = 1;   /* 1st dimensional vector _xx or _x */
        if(*xmf == '_') vflg = 2;   /* 1st dimensional vector _xx or _x */
        if(*xmf == ';') xflg = 1;   /* 2nd dimensional vector /Y;X */
        if(*xmf == ',') {
            xflg = 1;   /* 2nd dimensional vector /Y;X */
            *xmf = ';';
        }
    }
    if(vflg == 1 && xflg == 0) {
        xmf = infn;
        while(*(xmf++)) if(*xmf == '/') *xmf = '_';
    }
    if(vflg) {
        /* _xx */
        mflg = 0;
        fleng = 0;
        pmf = infn;
        while(*(pmf++)) fleng++;
        pleng = 0;
        i = 0;
        pmf = infn;
        while(*pmf) {
            pleng++;
            if(*pmf == '_') {
                mflg = 1;
                break;
            }
            w[i++] = *pmf;
            pmf++;
        }
        w[i] = '\0';
        xleng = fleng - pleng;
        /* printf("w = %s xleng = %d\n",w,xleng); */
        if(xleng == 3) {
            *prtn = 0;
            strcpy(outf,infn);
            strcat_h(outf,ftyp);
            mkyxfnum_h(outf,yfile,xfile,ps);
            return(0);
        }
        if(xleng == 2) {
            *prtn = 36;
            strcpy(outf,w);
            xmf = infn;
            while(*(xmf++)) if(*xmf == '_') break;
            xmf++;
            w[0] = *xmf;
            xmf++;
            w[1] = *xmf;
            w[2] = '\0';
            strcat_h(outf,"_0");
            strcat_h(outf,w);
            strcat_h(outf,ftyp);
            mkyxfnum_h(outf,yfile,xfile,ps);
            return(0);
        }
        if(xleng == 1) {
            *prtn = 0;
            strcpy(outf,infn);
            strcat_h(outf,"00");
            strcat_h(outf,ftyp);
            mkyxfnum_h(outf,yfile,xfile,ps);
            return(99);
        }
    }
    if(xflg) {
        i = 0;
        xmf = infn;
        while(*(xmf++)) if(*xmf == ';') break;
        while(*(xmf++)) xw[i++] = *xmf;
    }
    sscanf(xw,"%x",prtn);   /* %d v_1.14 */
    mflg = 0;
    i = 0;
    pmf = infn;
    fnum = "000";
    while(*pmf) {
        if(*pmf == '/') {
            mflg = 1;
            break;
        }
        w[i++] = *pmf;
        pmf++;
    }
    if(mflg) if(!xflg) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! file name \"%s\"\n",infn);
        exit(1);
    }
    w[i] = '\0';
    if(mflg) {
        i = 0;
        pmf = infn;
        while(*(pmf++)) if(*pmf == '/') break;
        while(*(pmf++)) {
            if(*pmf == ';') break;
            fnum[i] = *pmf;
            i++;
        }
        fnum[i] = '\0';
        j = 0;
        while(*(pmf++)) {
            if(*pmf == '\0') break;
            xw[j] = *pmf;
            j++;
        }
        /* printf("i = %d  fnum = %s xw = %s\n",i,fnum,xw); */
        fnum[1] = '\0';
        if(i == 1) {
            for(j = 0; j < 36; j++,ps++) if(fnum[0] == *ps) break;
            n = j;
        }
        else sscanf(fnum,"%d",&n);
        *yfile = n;
        sscanf(xw,"%x",xfile);  /* %d v_1.14 */
        strcpy(outf,w);
        strcat_h(outf,"_");
        fnum[1] = '\0';
        strcat_h(outf,fnum);
        sprintf(fnum,"%02x",fnmb+1);  /* %03d v_1.14 */
        strcat_h(outf,fnum);
        strcat_h(outf,ftyp);
        return(n-fnmb);
    }
    strcpy(outf,w);
    if(fnmb == -1) strcat_h(outf,"_000");
    if(fnmb == 0) strcat_h(outf,"_001");
    if(fnmb == 1) strcat_h(outf,"_002");
    strcat_h(outf,ftyp);
    mkyxfnum_h(outf,yfile,xfile,ps);
    return(0);
}

int putsync_mec(int k)
//int k;
{
    union meccode e6;
    int  n;

    n = 0;
    if(k == 3 || k == 4 || k == 5 || k == 6) n = numdin_mec(mg->card);
    if(n < 0) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d \"%s\"\n", mg->line, mg->card);
        return(1);
    }
    if(n > 32) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d port no. limit is 32 (%d)\n",mg->line, n);
        return(2);
    }
    e6.zi[0] = 0xe600;
    if(k == 3) e6.zi[0] = 0xe603;
    if(k == 4) e6.zi[0] = 0xe604;
    if(k == 5) e6.zi[0] = 0xe605;
    if(k == 6) e6.zi[0] = 0xe606;
    if(k == 7) e6.zi[0] = 0xe607;
    if(k == 8) e6.zi[0] = 0xe608;
    e6.zi[1] = n;
    putcode_mec(&e6);
    if(mg->trace) {
        printf("Trace! E6(%04x%04x) kind=%d port no. %d\n",
            e6.zi[0],e6.zi[1],k,n);
    }
    return(0);
}

float setrubart_mec()
{
    int  i,j,k;
    int  val;
    int  sf;
    char wk[16];
    float unit;

    rubart = 0;
    sf = 0;
    if(mg->card[0] == '%') k = 4; 
    if(mg->card[0] == 'c') k = 4; 
    if(mg->card[0] == 's') k = 4; 
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
            printf("Error! %06d Rubart data\n",mg->line);
            return(0);
	}
    }
    wk[j] = '\0';
    sscanf(wk,"%d",&val);
    /* printf("unit = %c\n",mg->card[i]); */
    if(mg->card[i] == ' ') val  = val * sf; /* + - */
    else {
	unit = 0;
        switch(mg->card[i]) {
            case 'f' : unit = 1.0;       /* flame */
                       break;
            case 'q' : unit = 1.0/4.0;   /* MTC */
                       break;     
            case 't' : unit = 1.0/8.0;   /* SMPTE Time code */
                       break;
            case 'b' : unit = 1.0/80.0;  /* Bit falame */
                       break;
            case 'd' : unit = 1.0/100.0; /* 1/100 flame */
                       break;
            case 'm' : unit  = 30.0/1000.0; /* msec */
                       break;
            case 's' : unit = 30.0;    /* Sec */
                       break;
            default  : break;
        }
        rubart = val * unit * sf;
    }
    if(mg->trace) {
        printf("Trace! input val=%s rubart=%f\n",wk,rubart);
    }
    /* mg->rbaf = 1;  rubart active on */
    return(rubart);
}

