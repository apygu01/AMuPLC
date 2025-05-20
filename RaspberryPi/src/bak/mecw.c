
// ----------------------
//  AMuPLC (mecw.c)
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
extern MECCPS *cp;
extern MECART *ar;
extern MECON  *on;
extern MECOFF *off;
extern FILE  *fpi;
extern int  scsn;
char npg[] = "0123456789abcdefghijklmnopqrstz";
extern char jrule[MAXRL][82];  /* Major Rule */
extern char irule[MAXRL][82];  /* minor Rule */


int dynacp_mec()
{
    int  i,j;
    char wk[82];

    /* printf("%s\n",mg->card); */
    i = 4;
    j = 0;
    while(1) {
        if(mg->card[i] == '\0') break;
        if(mg->card[i] == '}') break;
        if(mg->card[i] == ')') break;
        if(mg->card[i] == ' ') break;
        wk[j] = mg->card[i];
        i++;
        j++;
    }
    wk[j] = '\0';
    strcat(wk,"_dmy");
    /* printf("wk = %s\n",wk); */
    opencp_mec(wk);
    getexp_mec();
    return(0);
}

int dexpand_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    char exp[4];

    /* exp[0] = mg->card[75]; */
    exp[0] = mg->card[76];
    exp[1] = mg->card[77];
    exp[2] = mg->card[78];
    exp[3] = '\0';
    /* printf("exp = %s\n",exp); */
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
    renget_mec();
    if(mr->renp >= MSTC) {
        mg->errcnt++;
        mg->msglvl = 3;     
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d renpu max is %d\n",mg->line,MSTC);
        return(0);          
    }           
    if(mr->renp > 1) {          
        renproc_mec(mk,fpi); /* ignore expand */
        return(0);
    }
    /* for(i = 0; i < mr->rept; i++) { ignore rept */
    if(mg->btyp == 'r' || mg->btyp == 'd') {
        dnote_mec(mk,1,exp);
        return(0);
    }
    /* note_mec(mk); */
    note_mec(mk,1,exp);
    return(0);
}

int dexpchk_mec(char exp[4],long nval)
//char exp[4];
//long nval;
{
    int  rtn;
    char wk[4];
    char exps[6];

    exps[0] = exp[0];
    exps[1] = exp[1];
    exps[2] = exp[2];
    exps[3] = exp[3];
    exps[4] = '\0';
    ar->pmf = 0;
    if(mg->card[75] == '+') ar->pmf = 0;
    if(mg->card[75] == '-') ar->pmf = -1;
    ar->nval = nval;
    wk[0] = mg->card[1];
    wk[1] = mg->card[2]; 
    wk[2] = '\0';
    sscanf(wk,"%d",&ar->crate); 
    /* printf("nval = %ld\n",nval); */
    /* printf("ar->ctyp = %d ar->af = %d\n",ar->ctyp,ar->af); */
    rtn = 0;
    rtn = setartv_mec(exp);
    if(rtn) {
        /* Arpeggio ? */
        /* if(!strcmp(exps,"Arp")) { */
        if(exp[0] == '~') {
            if(isdigit(exp[1]) && isdigit(exp[2])) {
                dexpap_mec(exp); /* Arpeggio */
                return(0);
            }
        }
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
            printf("Error! Dynamic expand symbol %s not defined\n",exp);
        /* printf("ar->nmb = %d\n",ar->nmb); */
        return(1); /* err expand */
    }
    rtn = rtn + rtn;
    rtn = setontb_mec(exps);  /* set on table */
    rtn = rtn + rtn;
    rtn = setofftb_mec(); /* set off table */
    if(rtn) return(rtn); 
    expsync_mec(); /* note out */
    return(0);
}

/* set on table */
int setontb_mec(char exps[4])
//char exps[4];
{
    int  i,j,k;
    int  nc;
    int  n;
    int  rc;
    int  dc;
    char wk[MAXNC];
    int  pf;
    int  s;
    int  dv;
    /* char npv[64];   % line notes position value */
    int  nps[64];  /* % line notes position */
    int  nvn[64];  /* % line notes position velo on value */
    int  nt[MAXAR];
    int  p,pp;
    int  kk;
    int  pv;
    int  sync;
    int  pc;
    char wcnt[MAXPTN];
    int  len;
    int  iw;

    /* ON Table */
    if(mg->debug == 6) printf("\nDebug6! ON table\n");
    /* set on->dc[] */
    if(mg->debug == 6) printf("Debug6! ar->crt = %s\n",ar->crt);
    dc = 0;
    k = 0;
    pf = 0;
    i = 0;
    while(1) {
        if(ar->crt[i] == '\0') break;
        if(ar->crt[i] == ' ' || ar->crt[i] == '_' ||
            ar->crt[i] == '+' || ar->crt[i] == '-') {
            if(i > 0) {
                on->dc[k] = dc;
                if(mg->debug == 6)
                    printf("Debug6! on->dc[%d] = %d\n",k,on->dc[k]);
                dc = 0;
                k++;
                if(k > MAXAR) {
                    overerr_mec("Pos:1 on->dc[] MAXAR",k);
                    return(1);
                }
                i++;
                if(i > MAXPTN) {
                    overerr_mec("Pos:2 on->dc[] MAXPTN",i);
                    return(1);
                }
                pf = 1;
            }
        }
        if(ar->crt[i] == '*') {
            while(1) {
                if(ar->crt[i] == '\0') break;
                if(ar->crt[i] == ' ') break;
                if(ar->crt[i] == '+') break;
                if(ar->crt[i] == '_') break;
                if(ar->crt[i] == '-') break;
                i++;
                if(i > MAXPTN) {
                    overerr_mec("Pos:3 on->dc[] MAXPTN",i);
                    return(1);
                }
            }
            pf = 1;
        }
        if(isdigit(ar->crt[i])) {
            j = 0;
            while(isdigit(ar->crt[i])) {
                wk[j] = ar->crt[i];
                i++;
                if(i > MAXPTN) {
                    overerr_mec("Pos:4 on->dc[] MAXPTN",i);
                    return(1);
                }
                j++;
                if(j > 8) {
                    mg->errcnt++;
                    mg->msglvl = 3;
                    if(mg->msglvl >= mg->prtlvl)
                        printf("Error! Too many number %s\n",ar->crt);
                    return(1);
                }
                wk[j] = '\0';
                sscanf(wk,"%d",&dc);
                /* printf("wk = %s dc = %d\n",wk,dc); */
            }
            pf = 1;
        }
        if(!pf) i++;
    }
    on->sc = k;
    if(mg->debug == 6) printf("Debug6! on->sc = %d\n",on->sc);
    if(ar->pmf == -1 || ar->pmf == 2) {
        k = on->sc - 1;
        for(i = 0; i < on->sc; i++) {
            nt[k] = on->dc[i];
            k--;
            if(k < 0) break; 
        }
        for(i = 0; i < on->sc; i++) {
            on->dc[i] = nt[i];
            if(mg->debug == 6)
                printf("Debug6! Reverse on->dc[%d] = %d\n",i,on->dc[i]);
        }
    }
    /* set on->rc[] */
    if(mg->debug == 6) printf("Debug6! ar->crt = %s\n",ar->crt);
    for(i = 0 ; i < MAXPTN; i++) on->rc[i] = 0; 
    rc = 1;
    k = 0;
    for(i = 0 ; i < MAXPTN; i++) {
        if(ar->crt[i] == '\0') break;
        if(ar->crt[i] == ' ' || ar->crt[i] == '_' ||
            ar->crt[i] == '+' || ar->crt[i] == '-') {
            if(i > 0) {
                on->rc[k] = rc;
                if(mg->debug == 6)
                    printf("Debug6! on->rc[%d] = %d\n",k,on->rc[k]);
                rc = 1;
                k++;
                if(k > MAXAR) {
                    overerr_mec("Pos:5 on->rc[] MAXAR",k);
                    return(1);
                }
            }
        }
        if(ar->crt[i] == '*') {
            j = 0;
            while(isdigit(ar->crt[i+1+j])) {
                wk[j] = ar->crt[i+1+j];
                j++;
                if( i+1+j > MAXPTN) {
                    overerr_mec("Pos:6 on->rc[] MAXPTN",i+1+j);
                    return(1);
                }
                if(j > 4) {
                    mg->errcnt++;
                    mg->msglvl = 3;
                    if(mg->msglvl >= mg->prtlvl)
                        printf("Error! Too many *number %s\n",ar->crt);
                    return(1);
                }
                wk[j] = '\0';
                sscanf(wk,"%d",&rc);
                /* printf("wk = %s rc = %d\n",wk,rc); */
            }
        }
    }
    if(ar->pmf == -1 || ar->pmf == 2) {
        k = on->sc - 1;
        for(i = 0; i < on->sc; i++) {
            nt[k] = on->rc[i];
            k--;
            if(k < 0) break;
        }
        for(i = 0; i < on->sc; i++) {
            on->rc[i] = nt[i];
            if(mg->debug == 6)
                printf("Debug6! Reverse on->rc[%d] = %d\n",i,on->rc[i]);
        }
    }
    /* set on->nc[] */
    if(mg->debug == 6) printf("Debug6! ar->cnt = %s\n",ar->cnt);
    for( i = 0; i < MAXPTN; i++) on->nc[i] = 0;
    k = 0;
    nc = 0;
    for( i = 0; i < MAXPTN; i++) {
        if(ar->cnt[i] == '\0') {
            if(nc) {
                on->nc[k] = nc;
                if(mg->debug == 6)
                    printf("Debug6! on->nc[%d] = %d\n",k,on->nc[k]);
                nc = 0;
                k++;
            }
            break;
        }
        if(ar->cnt[i] == ' ') {
            on->nc[k] = nc;
            if(mg->debug == 6) printf("Debug6! on->nc[%d] = %d\n",k,on->nc[k]);
            nc = 0;
            k++;
            if(k > MAXAR) {
                overerr_mec("Pos:7 on->nc[] MAXAR",k);
                return(1);
            }
        }
        else {
            nc++;
            if(ar->cnt[i] == 'r' || ar->cnt[i] == 'R') {
                nc = 0;
                on->nc[k] = nc;
                /*
                if(mg->debug == 6) printf("Debug6! on->nc[%d] = %d\n",
                    k,on->nc[k]);
                k++;
                if(k > MAXAR) {
                    overerr_mec("Pos:8 on->nc[] MAXAR",k);
                    return(1);
                }
                */
            }
        }
    }
    if(ar->pmf == -1 || ar->pmf == 1) {
        k = on->sc - 1;
        for(i = 0; i < on->sc; i++) {
            nt[k] = on->nc[i];
            k--;
            if(k < 0) break;
        }
        for(i = 0; i < on->sc; i++) {
            on->nc[i] = nt[i];
            if(mg->debug == 6)
                printf("Debug6! Reverse on->nc[%d] = %d\n",i,on->nc[i]);
        }
    }
    /* set on->dv[] */
    s = 0;
    for(i = 0; i < on->sc; i++) {
        s = s + on->dc[i] * on->rc[i];
    }
    if(s == 0) {
        printf("Worning! Dynamic Expand symbol %s r = \" \" not defined.\n",
            exps);
        return(0);
    }
    dv = ar->nval / s;
    if(mg->debug == 6)
        printf("Debug6! s = %d ar->nval = %ld dv = %d\n",s,ar->nval,dv);
    k = 0;
    for(i = 0; i < on->sc; i++) {
        for(j = 0; j < on->rc[i]; j++) {
            on->dv[k] = dv * on->dc[i];
            if(mg->debug == 6) printf("Debug6! on->dv[%d] = %d\n",k,on->dv[k]);
            k++;
            if(k > MAXDV) {
                overerr_mec("Pos:9 on->dv[] MAXDV",k);
                return(1);
            }
        }
    }
    on->dvc = k;
    if(mg->debug == 6) printf("Debug6! on->dvc = %d\n",on->dvc);
    /* set on->nvv[] */
    /* printf("ar->cvt = %s\n",ar->cvt); */
    // for(i = 0; i < MAXDV; i++) {
    for(i = 0; i < MAXAR; i++) {
        for(n = 0; n < MAXNC; n++) on->nvv[i][n] = 0;
    }
    i = 0;
    j = 0;
    n = 0;
    while(1) {
        if(ar->cvt[i] == '\0') break;
        if(isdigit(ar->cvt[i])) {
            on->nvv[j][n] = 0x0f & ar->cvt[i];
            n++;
            if(n > MAXNC) {
                overerr_mec("Pos:10 on->np[] MAXNC",p);
                return(1);
            }
        }
        if(ar->cvt[i] == ' ') {
            n = 0;
            j++;
            if(i > MAXAR) {
                overerr_mec("Pos:11 on->nvc[] MAXAR",k);
                return(1);
            }
        }
        i++;
        if(i > MAXPTN) {
            overerr_mec("Pos:12 on->nvc[] MAXPTN",k);
            return(1);
        }
    }
    if(mg->debug == 6) {
        printf("Debug6! ar->cvt = %s\n",ar->cvt);
        printf("on->nvv[] = ");
        for(i = 0; i < j; i++) {
            for(n = 0 ; n < MAXNC; n++) {
               if(on->nvv[i][n]) printf("%d",on->nvv[i][n]);
            }
            printf(" ");
        }
        printf("\n");
    }
    /* set on->fvb[] */
    /* printf("ar->fbt = %s\n",ar->fbt); */
    //for(i = 0; i < MAXDV; i++) {
    for(i = 0; i < MAXAR; i++) {
        for(n = 0; n < MAXNC; n++) on->fvb[i][n] = 0;
    }
    i = 0;
    j = 0;
    n = 0;
    iw = 0;
    while(1) {
        if(ar->fbt[i] == '\0') break;
        wk[iw] = ar->fbt[i];
        iw++;
        if(ar->fbt[i] == ' ') {
            n = 0;
            wk[iw] = '\0';
            sscanf(wk,"%f",&on->fvb[j][n]);
            /*
            printf("wk = %s on->fvb[%d][%d] = %3.1f\n",wk,j,n,on->fvb[j][n]);
            */
            j++;
            if(i > MAXAR) {
                overerr_mec("Pos:13 on->nvc[] MAXAR",k);
                return(1);
            }
            n++; 
            iw = 0;
            if(n > MAXNC) {
                overerr_mec("Pos:14 on->np[] MAXNC",p);
                return(1);
            }
        }
        i++;
        if(i > MAXPTN) {
            overerr_mec("Pos:15 on->nvc[] MAXPTN",k);
            return(1);
        }
    }
    if(mg->debug == 6) {
        printf("Debug6! ar->fbt = %s\n",ar->fbt);
        printf("on->fvb[] = ");
        for(i = 0; i < j; i++) {
            for(n = 0 ; n < MAXNC; n++) { 
               if(on->fvb[i][n]) printf("%3.1f",on->fvb[i][n]);
            }
            printf(" ");
        }
        printf("\n");
    }
    /* set on->np[], on->nv[] */
    /*
    j = 10;
    if(mg->btyp == 'r') j = 14;
    if(mg->btyp == 't') j = 32;
    if(mg->btyp == 'g') j = 32;
    if(mg->btyp == 's') j = 32;
    if(mg->btyp == 'a') j = 32;
    if(mg->btyp == 'b') j = 32;
    if(mg->btyp == 'u') j = 32;
    k = 0;
    for(i = j; i < 69; i++) {
        if(isdigit(mg->card[i])) {
            npv[k] = mg->card[i];
            k++;
            if(k > 64) {
                overerr_mec("Pos:16 on->np[] loop69",k);
                return(1);
            }
        }
    }
    */
    if(ar->pmf == -1) {
        len = strlen(ar->cnt);
        /* printf("len = %d\n",len); */
        j = 0;
        for(i = len - 1; i >= 0 ; i--) {
            wcnt[j] = ar->cnt[i];
            j++;
        }
        wcnt[j] = '\0';
    }
    else strcpy(wcnt,ar->cnt);
    if(mg->debug == 6) {
        printf("ar->cnt = %s\n",ar->cnt);
        printf("   wcnt = %s\n",wcnt);
    }
    p = 0;
    /* printf("%s",mg->card); */
    if(mg->btyp == 'r' || mg->btyp == 'd') pc = dnotepos_mec(nps,nvn);
    else pc = notepos_mec(nps,nvn);
    if(mg->debug == 6) printf("Debug6! pc = %d ar->cnt = %s\n",pc,ar->cnt);
    kk = 0;
    for(i = 0; i < on->sc; i++) {
        if(wcnt[p] == ' ') p++;
        pp = 0;
        /* while(isdigit(ar->cnt[p])) { */
        while(isdigit(wcnt[p]) || isalpha(wcnt[p])) {
            wk[pp] = wcnt[p];
            pp++;
            p++;
            if(p > MAXNC) {
                overerr_mec("Pos:17 on->np[] MAXNC",p);
                return(1);
            }
        }
        for(j = 0; j < on->rc[i]; j++) {
            if(on->nc[i] != 0) {
                for(k = 0; k < on->nc[i]; k++) {
                    pv = 0x0f & wk[k];
                    for(n = 0; n < MAXNC; n++) {
                        if(wk[k] == npg[n]) {
                            pv = n;
                            break;
                        }
                    }
                    on->np[kk][k] = nps[pv];
                    /*
                    if(ar->vaf) {
                    */
                    if(on->nvv[kk][k] || on->fvb[kk][k]) {
                        if(on->fvb[kk][k]) {
                            if(on->nvv[kk][k]) {
                                on->nv[kk][k] = on->fvb[kk][k] * on->nvv[kk][k];
                                if(mg->debug == 6) {
                                    printf("Debug6! setontb_(1) ");
                                    printf("on->fvb = %f on->nvv = %d\n",
                                       on->fvb[kk][k],on->nvv[kk][k]);
                                }
                            }
                            else {
                                on->nv[kk][k] = mr->dyna * on->fvb[kk][k];
                                if(mg->debug == 6) {
                                    printf("Debug6! setontb_(1) ");
                                    printf("mr->dyna = %d on->fvb = %f\n",
                                       mr->dyna,on->fvb[kk][k]);
                                }
                            }
                        }
                        else {
                            on->nv[kk][k] = mg->velo * on->nvv[kk][k];
                            if(mg->debug == 6) {
                                printf("Debug6! mg->velo = %2.1f ",mg->velo);
                                printf("on->nvv[%d][%d] = %d\n",
                                    kk,k,on->nvv[kk][k]);
                            }
                        }
                    }
                    else on->nv[kk][k] = nvn[pv];
                    if(mg->debug == 6) {
                      printf("Debug6! setontb_(1) ");
                      printf("on->np,nv[%d][%d] = %d,%d pv = %d wk[%d] = %c\n",
                      kk,k,on->np[kk][k],on->nv[kk][k],pv,k,wk[k]);
                    }
                }
            }
            else {
                k = 0;
                on->np[kk][k] = -1;
                on->nv[kk][k] = 0;
                if(mg->debug == 6) {
                    printf("Debug6! setontb_(2) ");
                    printf("on->np,nv[%d][%d] = %d,%d rest\n",
                    kk,k,on->np[kk][k],on->nv[kk][k]);
                }
            }
            kk++;
            if(kk > MAXDV) {
                overerr_mec("Pos:18 on->np[] MAXDV",kk);
                return(1);
            }
        }
        p++;
        if(p > MAXPTN) {
            overerr_mec("Pos:19 on->np[] MAXPTN",p);
            return(1);
        }
    }
    /* set on->sync[MAXDV] */
    if(mg->debug == 6) printf("Debug6! ON sync\n");
    sync = 0;
    for(i = 0 ; i < on->dvc; i++) { 
        on->sync[i] = sync;
        sync = sync + on->dv[i];
        if(mg->debug == 6) printf("Debug6! on->sync[%d] = %d\n",i,on->sync[i]);
    }
    /* set on->dnc[MAXDV] */
    k = 0;
    for(i = 0; i < on->sc; i++) {
        for(j = 0; j < on->rc[i]; j++) {
            on->dnc[k] = on->nc[i];
            if(mg->debug == 6)
                printf("Debug6! on->dnc[%d] = %d\n",k,on->dnc[k]);
            k++;
            if(k > on->dvc) break;
        }
    }
    return(0);
}

/* set off table */
int setofftb_mec()
{
    int  i,j,k;
    char mc;
    int  sum;
    int  kk;
    int  sync;

    if(mg->debug == 6) printf("\nDebug6! OFF table\n");
    /* set off->mc[] */
    if(mg->debug == 6) printf("Debug6! ar->crt = %s\n",ar->crt);
    k = 0;
    if(isdigit(ar->crt[0])) {
        off->mc[0] = ' ';
        if(mg->debug == 6) printf("Debug6! off->mc[%d] = %c\n",0,off->mc[0]);
        k = 1;
    }
    for(i = 0 ; i < MAXPTN; i++) {
        if(ar->crt[i] == '\0') break;
        if(ar->crt[i] == ' ' || ar->crt[i] == '_' ||
            ar->crt[i] == '+' ||  ar->crt[i] == '-') {
            off->mc[k] = ar->crt[i];
            if(mg->debug == 6)
                printf("Debug6! off->mc[%d] = %c\n",k,off->mc[k]);
            k++;
            if(k > MAXAR) {
                overerr_mec("Pos:20 off->mc[] MAXAR",k);
                return(1);
            }
        }
    }
    /* set off->rs[MAXDV] */
    k = 0;
    for(i = 0; i < on->sc; i++) {
        sum = 0;
        for(j = 0; j < on->rc[i]; j++) {
            sum = sum + on->dv[k];
            k++;
            if(k > MAXDV) {
                overerr_mec("Pos:21 on->dv[] MAXDV",k);
                return(1);
            }
        }
        off->rs[i] = sum;
    }
    /* set off->dv[MAXDV] */
    /*  ex.
              ar->crt = "4_1*4+4 2*2-"
                         a  b  c  d
              nval = 100000
              % : crate 

              on->dv[]      off->dv[]
         a     12500        a + b1 * %
         b1     3125        b1 * %
         b2     3125        b2 * %
         b3     3125        b3 * %
         b4     3125        (b4 + c) * %
         c     12500        c * %
         d1     6250        d1 * %
         d2     6250        d2 * 100%

    */
    k = 0;
    for(i = 0; i < on->sc; i++) {
        sum = 0;
        if(i+1 < on->sc) sum = off->rs[i+1];
        if(i+1 >= on->sc) mc = ' ';
        else mc = off->mc[i+1];
        for(j = 0; j < on->rc[i]; j++) {
            /* off->dv[k] = on->dv[k]; */
            switch(mc) {
                case  ' ' : /* Space a * % */
                            off->dv[k] = (on->dv[k] * ar->crate) / 100;
                            break;
                case  '-' : /* sul a * 100% */
                            off->dv[k] = on->dv[k];
                            break;
                case  '_' : /* sul a + b1 * %  */
                            if(k+1 < on->dvc) { 
                                off->dv[k] = on->dv[k] +
                                   (on->dv[k+1] * ar->crate / 100);
                            }
                            else {
                                off->dv[k] = on->dv[k];
                            }
                            break;
                case  '+' : /* sul (a + (b1+b2+b3+b4)) * % */
                            off->dv[k] = ((on->dv[k] + sum) * ar->crate)/100;
                            break;
                default   : break;
            }
            if(mg->debug == 6)
              printf("Debug6! mc=%c off->dv[%d]=%d on->dv[%d]=%d sum=%d %d%c\n",
                mc,k,off->dv[k],k,on->dv[k],sum,ar->crate,'%');
            k++;
            if( k > on->dvc ) break;
            if(k > MAXDV) {
                overerr_mec("Pos:22 on->dv[] MAXDV",k);
                return(1);
            }
        }
    }
    /* set off->np[MAXDV][MAXNC] */
    /* set off->nv[MAXDV][MAXNC] */
    kk = 0;
    for(i = 0; i < on->sc; i++) {
        for(j = 0; j < on->rc[i]; j++) {
            if(on->nc[i] != 0) {
                for(k = 0; k < on->nc[i]; k++) {
                    off->np[kk][k] = on->np[kk][k];
                    off->nv[kk][k] = 0;
                    if(mg->debug == 6) {
                      printf("Debug6! ");
                      printf("off->np:nv[%d][%d] = %d:%d\n",
                      kk,k,off->np[kk][k],off->nv[kk][k]);
                    }
                }
            }
            else {
                k = 0;
                off->np[kk][k] = -1;
                off->nv[kk][k] = 0;
                if(mg->debug == 6) {
                    printf("Debug6! ");
                    printf("off->np:nv[%d][%d] = %d:%d rest\n",
                      kk,k,off->np[kk][k],off->nv[kk][k]);
                }
            }
            kk++;
            if(kk > MAXDV) {
                overerr_mec("Pos:23 off->np[] MAXDV",kk);
                return(1);
            }
        }
    }
    /* set off->sync[MAXDV] */
    sync = 0;
    i = 0;  
    while(1) {
        off->sync[i] = sync + off->dv[i];
        if(mg->debug == 6)  
            printf("Debug6! off->sync[%d] = %d\n",i,off->sync[i]);
        i++;    
        if(i > on->dvc - 1) break;
        sync = on->sync[i]; 
    }           
    /* set off->dnc[MAXDV] */
    k = 0;
    for(i = 0; i < on->sc; i++) {
        for(j = 0; j < on->rc[i]; j++) {
            off->dnc[k] = on->nc[i];
            if(mg->debug == 6)
                printf("Debug6! off->dnc[%d] = %d\n",k,off->dnc[k]);
            k++;
            if(k > on->dvc) break;
        }
    }
    return(0);
}

/* + Dynamic expand sync out */
int expsync_mec()
{
    int  i,j,k;
    int  ok;
    int  fk;
    long csyn;

    ok = 0;
    fk = 0;
    if(mg->debug == 6)
        printf("Debug6! Plus mr->sync = %ld on->dvc = %d\n",mr->sync,on->dvc);
    while(1) {
        if(on->sync[ok] < off->sync[fk]) {
            csyn = mr->sync - on->sync[ok];
            if(mg->debug == 6)
                printf("Debug6! on->sync[%d] = %d csyn = %ld\n",
                    ok,on->sync[ok],csyn);
            if(csyn > 0) {
                if(mr->sync != csyn) syncout_mec(csyn,1); /* type : lag */
            }
            for(k = 0; k < on->dnc[ok]; k++) { 
                mr->keyp = on->np[ok][k];
                mr->veon = on->nv[ok][k];
                if(mr->keyp != -1) dexputfd_mec();
            }
            ok++;
        }
        if(off->sync[fk] < on->sync[ok]) {
            csyn = mr->sync - off->sync[fk];
            if(mg->debug == 6)
                printf("Debug6! off->sync[%d] = %d csyn = %ld\n",
                    fk,off->sync[fk],csyn);
            if(csyn > 0) {
              if(mr->sync != csyn) syncout_mec(csyn,2); /* type : offset */
            }
            for(k = 0; k < off->dnc[fk]; k++) { 
                mr->keyp = off->np[fk][k];
                mr->veon = off->nv[fk][k];
                if(mr->keyp != -1) dexputfd_mec();
            }
            fk++;
        }
        if(off->sync[fk] == on->sync[ok]) {
            csyn = mr->sync - on->sync[ok];
            if(mg->debug == 6)
                printf("Debug6! on/off->sync[%d] = %d csyn = %ld\n",
                    ok,on->sync[ok],csyn);
            if(csyn > 0) {
                if(mr->sync != csyn) syncout_mec(csyn,1); /* type : lag */
            }
            for(k = 0; k < on->dnc[ok]; k++) { 
                mr->keyp = on->np[ok][k];
                mr->veon = on->nv[ok][k];
                if(mr->keyp != -1) dexputfd_mec();
            }
            for(k = 0; k < off->dnc[fk]; k++) { 
                mr->keyp = off->np[fk][k];
                mr->veon = off->nv[fk][k];
                if(mr->keyp != -1) dexputfd_mec();
            }
            ok++;
            fk++;
        }
        if(ok > on->dvc - 1) break; 
        if(fk > on->dvc - 1) break;
    }
    if(fk <= on->dvc - 1) {
        while(1) {
            csyn = mr->sync - off->sync[fk];
            if(mg->debug == 6)
                printf("Debug6! last/off->sync[%d] = %d csyn = %ld\n",
                    fk,off->sync[fk],csyn);
            if(csyn > 0) {
                if(mr->sync != csyn) syncout_mec(csyn,2); /* type : offset */
            }
            for(k = 0; k < off->dnc[fk]; k++) { 
                mr->keyp = off->np[fk][k];
                mr->veon = off->nv[fk][k];
                if(mr->keyp != -1) dexputfd_mec();
            }
            fk++;
            if(fk > on->dvc - 1) break;
        }
    }
    return(0);
}

int notepos_mec(int nps[64],int nvn[64])
//int  nps[64];
//int  nvn[64];
{
    int  i,j,k;
    int  p;
    int  lastp;

    for(i = 0; i < 64; i++) {
        nps[i] = -1;
        nvn[i] = 0;
    }
    k = startkp_mec(&j,&lastp);
    p = 0;
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
            mr->veon = mr->dyna * mg->velo;
            if(mr->veon > 127) mr->veon = 127;
            nps[p] = mr->keyp + mr->trnsv;
            nvn[p] = mr->veon;
            if(mg->debug == 6)
                printf("Debug6! nps[%d] = %d nvn[%d] = %d\n",
                p,nps[p],p,nvn[p]);
            p++;
            if(p > 64) break;
            if(mg->debug == 6) {
                printf("Debug6! note on chan=%d ",mr->chan);  
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }   
        }   
        k++;
    }   
    return(p);
}   

int dnotepos_mec(int nps[64],int nvn[64])
//int  nps[64];
//int  nvn[64];
{
    union meccode fd;
    int  i,j;
    int  p;
    int  lastp;
    int  sval;
    char wk[4];

    for(i = 0; i < 64; i++) {
        nps[i] = -1;
        nvn[i] = 0;
    }
    lastp = 69;
    sval = 30;  /* start key position */
    j = 14;     /* mg->btyp == 'r' */
    if(mg->btyp == 'd') {
        j = 33;
        sval = 40;
    }
    if(mg->btyp == 'r') {
        wk[0] = mg->card[11];
        wk[1] = mg->card[12];
        wk[2] = '\0';
        sscanf(wk,"%d",&mr->trnsv);
    }
    p = 0;
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
            mr->keyp &= 0x00ff;
            mr->chan &= 0x000f;
            mr->veon = mr->dyna * mg->velo;
            if(mr->veon > 127) mr->veon = 127;
            nps[p] = mr->keyp + mr->trnsv;
            nvn[p] = mr->veon;
            if(mg->debug == 6)
                printf("Debug6! nps[%d] = %d nvn[%d] = %d\n",
                p,nps[p],p,nvn[p]);
            p++;
            if(p > 64) break;
            if(mg->debug == 6) {
                printf("Debug6! note on chan=%d ",mr->chan);
                printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
                mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
            }
        }
    }
    return(p);
}

/* set art val */
int setartv_mec(char exp[4])
//char exp[4];
{
    int  i;
    int  sf;
    int  len;

    ar->af = 0;
    sf = 0;
    for(i = 0 ; i < ar->nmb; i++) {
        if(!strcmp(exp,ar->dxs[i])) {
            sf = 1;
            break;
        }
    }
    if(!sf) return(1);
    ar->af = 1;
    strcpy(ar->cdxs,ar->dxs[i]);
    ar->ctyp = ar->typ[i];
    len = strlen(ar->rt[i]);
    spacecmp_mec(ar->rt[i],len);
    strcpy(ar->crt,ar->rt[i]);
    len = strlen(ar->nt[i]);
    spacecmp_mec(ar->nt[i],len);
    strcpy(ar->cnt,ar->nt[i]);
    len = strlen(ar->vt[i]);
    spacecmp_mec(ar->vt[i],len);
    strcpy(ar->cvt,ar->vt[i]);

    len = strlen(ar->ft[i]);
    spacecmp_mec(ar->ft[i],len);
    strcpy(ar->fbt,ar->ft[i]);

    if(mg->debug == 6) {
        printf("Debug6! ar-> cdxs=%s crate=%d nval=%ld pmf=%d\n",
            ar->cdxs,ar->crate,ar->nval,ar->pmf);
        printf("        ar->crt = \"%s\"\n",ar->crt);
        printf("        ar->cnt = \"%s\"\n",ar->cnt);
        printf("        ar->cvt = \"%s\"\n",ar->cvt);
        printf("        ar->fbt = \"%s\"\n",ar->fbt);
    }
    return(0);
}

/* Arpeggio */
int dexpap_mec(char exp[4])
//char exp[4];
{
    union meccode fd;
    int  i;
    int  dt;
    int  ptn;

    /* printf("ar->pmf = %d ar->nval = %ld\n",ar->pmf,ar->nval); */
    /* printf("mr->temp=%d\n",mr->temp); */
    dt = 1;
    if(isdigit(exp[1])) ptn = 0x0f & exp[1];
    if(isdigit(exp[2])) dt = 0x0f & exp[2];
    if(mg->debug == 6) printf("\nDebug6! Arpeggio table\n");
    if(mg->debug == 6) {
        printf("Debug6! ptn = %d dt = %d ",ptn,dt);
        printf("ar->nval = %ld\n",ar->nval);
    }
    switch(ptn) {
        case  0 : arptn0_mec(dt);
                  break;
        case  1 : arptn1_mec(dt);
                  break;
        case  2 : arptn2_mec(dt);
                  break;
        case  3 : arptn3_mec(dt);
                  break;
        default : /* Cont. */
                  mg->wrncnt++;
                  mg->msglvl = 2;
                  if(mg->msglvl >= mg->prtlvl)
                  printf("Worning! Ignore dynamic expand %s symbol\n",exp);
                  break;
    }
    return(0);
}

/* up */
int arptn0_mec(int dt)
//int  dt;
{
    union meccode fd;
    int  i,j,k;
    int  p;
    int  lastp;
    int  dsyn; /* delta sync */
    int  csyn; /* current sync */
    int  dyna;
    int  keyp;
    int  nval;
    int  ptn;

    /* printf("ar->pmf = %d ar->nval = %ld\n",ar->pmf,ar->nval); */
    /* printf("mr->temp=%d\n",mr->temp); */
    /* dsyn = 1500 * dt;  test */
    dsyn = 1000 * dt;
    dsyn = dsyn * mr->temp / 6000;
    csyn = mr->sync;
    k = startkp_mec(&j,&lastp);
    p = 0;
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            on->sync[p] = csyn;
            csyn = csyn - dsyn;
            if(csyn < 0) csyn = 3;
            dyna = mg->card[i] - '0';
            keyp = mg->ktb[k];
            if(mg->card[i] == '>') {
                dyna = 5;
                keyp = keyp + 1;
            }
            if(mg->card[i] == '<') {
                dyna = 5;
                keyp = keyp - 1;
            }
            on->np[p][0] = keyp;
            on->nv[p][0] = dyna * mg->velo;
            if(mg->debug == 6)
                printf("Debug6! on->np:nv[%d] = %d:%d on->sync[%d] = %d\n",
                    p,keyp,on->nv[p][0],p,on->sync[p]);
            p++;
        }
        k++;
    }
    on->dvc = p;
    if(mg->debug == 6) printf("Debug6! on->dvc = %d ar->crate = %d%c\n",
        on->dvc,ar->crate,'%');
    for(i = 0; i < on->dvc; i++) {
        /* Comment out 2012.7.7
        off->sync[i] = (on->sync[on->dvc - 1] * (100 - ar->crate)) / 100; 
        off->sync[i] = on->sync[on->dvc - 1] - 
            ((ar->nval * (100 - ar->crate))/100);
        */
        off->sync[i] = on->sync[on->dvc - 1] - 
            (ar->nval *  ar->crate/100);
        if(off->sync[i] <= 0) off->sync[i] = 1;
        off->np[i][0] = on->np[i][0];
        off->nv[i][0] = 0;
        if(mg->debug == 6)
            printf("Debug! off->np:nv[%d] = %d:%d off->sync[%d] = %d\n",
                i,off->np[i][0],off->nv[i][0],i,off->sync[i]);
    }
    if(ar->pmf == -1) bksync_mec(); 
    arpsync_mec();
    return(0);
}

/* up down */
int arptn1_mec(int dt)
//int  dt;
{
    union meccode fd;
    int  i,j,k;
    int  p;
    int  lastp;
    int  dsyn; /* delta sync */
    int  csyn; /* current sync */
    int  dyna;
    int  keyp;
    int  nval;
    int  ptn;
    int  offsync[MAXDV];
    int  np[MAXDV];
    int  nv[MAXDV];

    /* printf("ar->pmf = %d ar->nval = %ld\n",ar->pmf,ar->nval); */
    /* printf("mr->temp=%d\n",mr->temp); */
    /* dsyn = 1500 * dt;  test */
    dsyn = 1000 * dt;
    dsyn = dsyn * mr->temp / 6000;
    csyn = mr->sync;
    k = startkp_mec(&j,&lastp);
    p = 0;
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            on->sync[p] = csyn;
            csyn = csyn - dsyn;
            if(csyn < 0) csyn = 3;
            dyna = mg->card[i] - '0';
            keyp = mg->ktb[k];
            if(mg->card[i] == '>') {
                dyna = 5;
                keyp = keyp + 1;
            }
            if(mg->card[i] == '<') {
                dyna = 5;
                keyp = keyp - 1;
            }
            on->np[p][0] = keyp;
            on->nv[p][0] = dyna * mg->velo;
            if(mg->debug == 6)
                printf("Debug6! on->np:nv[%d] = %d:%d on->sync[%d] = %d\n",
                    p,on->np[p][0],on->nv[p][0],p,on->sync[p]);
            p++;
        }
        k++;
    }
    on->dvc = p;
    k = on->dvc - 1;
    for(i = 0 ; i < on->dvc; i++) {
        offsync[i] = csyn;
        np[i] = on->np[i][0];
        on->sync[p] = csyn;
        csyn = csyn - dsyn;
        if(csyn < 0) csyn = 3;
        on->np[p][0] = on->np[k][0];
        on->nv[p][0] = on->nv[k][0];
        if(mg->debug == 6)
            printf("Debug6! on->np:nv[%d] = %d:%d on->sync[%d] = %d\n",
                p,on->np[p][0],on->nv[p][0],p,on->sync[p]);
        p++;
        k--;
        if(k < 0) break;
    }
    for(i = 0 ; i < on->dvc; i++) {
        off->sync[i] = offsync[i] + 10;
        off->np[i][0] = np[i];
        off->nv[i][0] = 0;
        if(mg->debug == 6)
            printf("Debug6! off->np:nv[%d] = %d:%d off->sync[%d] = %d\n",
                i,off->np[i][0],off->nv[i][0],i,off->sync[i]);
    }
    k = on->dvc;
    on->dvc = p;
    if(mg->debug == 6) printf("Debug6! on->dvc = %d ar->crate = %d%c\n",
        on->dvc,ar->crate,'%');
    for(i = k; i < on->dvc; i++) {
        /* Comment out 2012.7.7
        off->sync[i] = (on->sync[on->dvc - 1] * (100 - ar->crate)) / 100; 
        off->sync[i] = on->sync[on->dvc - 1] - 
            ((ar->nval * (100 - ar->crate))/100);
        */
        off->sync[i] = on->sync[on->dvc - 1] - 
            (ar->nval *  ar->crate/100);
        if(off->sync[i] <= 0) off->sync[i] = 1;
        off->np[i][0] = on->np[i][0];
        off->nv[i][0] = 0;
        if(mg->debug == 6)
            printf("Debug! off->np:nv[%d] = %d:%d off->sync[%d] = %d\n",
                i,off->np[i][0],off->nv[i][0],i,off->sync[i]);
    }
    if(ar->pmf == -1) bksync_mec(); 
    arpsync_mec();
    return(0);
}

/* up up */
int arptn2_mec(int dt)
//int  dt;
{
    union meccode fd;
    int  i,j,k;
    int  p;
    int  lastp;
    int  dsyn; /* delta sync */
    int  csyn; /* current sync */
    int  dyna;
    int  keyp;
    int  nval;
    int  ptn;
    int  offsync[MAXDV];
    int  np[MAXDV];
    int  nv[MAXDV];

    /* printf("ar->pmf = %d ar->nval = %ld\n",ar->pmf,ar->nval); */
    /* printf("mr->temp=%d\n",mr->temp); */
    /* dsyn = 1500 * dt;  test */
    dsyn = 1000 * dt;
    dsyn = dsyn * mr->temp / 6000;
    csyn = mr->sync;
    k = startkp_mec(&j,&lastp);
    p = 0;
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            on->sync[p] = csyn;
            csyn = csyn - dsyn;
            if(csyn < 0) csyn = 3;
            dyna = mg->card[i] - '0';
            keyp = mg->ktb[k];
            if(mg->card[i] == '>') {
                dyna = 5;
                keyp = keyp + 1;
            }
            if(mg->card[i] == '<') {
                dyna = 5;
                keyp = keyp - 1;
            }
            on->np[p][0] = keyp;
            on->nv[p][0] = dyna * mg->velo;
            if(mg->debug == 6)
                printf("Debug6! on->np:nv[%d] = %d:%d on->sync[%d] = %d\n",
                    p,on->np[p][0],on->nv[p][0],p,on->sync[p]);
            p++;
        }
        k++;
    }
    on->dvc = p;
    for(i = 0 ; i < on->dvc; i++) {
        offsync[i] = csyn;
        np[i] = on->np[i][0];
        on->sync[p] = csyn;
        csyn = csyn - dsyn;
        if(csyn < 0) csyn = 3;
        on->np[p][0] = on->np[i][0];
        on->nv[p][0] = on->nv[i][0];
        if(mg->debug == 6)
            printf("Debug6! on->np:nv[%d] = %d:%d on->sync[%d] = %d\n",
                p,on->np[p][0],on->nv[p][0],p,on->sync[p]);
        p++;
    }
    for(i = 0 ; i < on->dvc; i++) {
        off->sync[i] = offsync[i] + 10;
        off->np[i][0] = np[i];
        off->nv[i][0] = 0;
        if(mg->debug == 6)
            printf("Debug6! off->np:nv[%d] = %d:%d off->sync[%d] = %d\n",
                i,off->np[i][0],off->nv[i][0],i,off->sync[i]);
    }
    k = on->dvc;
    on->dvc = p;
    if(mg->debug == 6) printf("Debug6! on->dvc = %d ar->crate = %d%c\n",
        on->dvc,ar->crate,'%');
    for(i = k; i < on->dvc; i++) {
        /* Comment out 2012.7.7
        off->sync[i] = (on->sync[on->dvc - 1] * (100 - ar->crate)) / 100; 
        off->sync[i] = on->sync[on->dvc - 1] - 
            ((ar->nval * (100 - ar->crate))/100);
        */
        off->sync[i] = on->sync[on->dvc - 1] - 
            (ar->nval *  ar->crate/100);
        if(off->sync[i] <= 0) off->sync[i] = 1;
        off->np[i][0] = on->np[i][0];
        off->nv[i][0] = 0;
        if(mg->debug == 6)
            printf("Debug! off->np:nv[%d] = %d:%d off->sync[%d] = %d\n",
                i,off->np[i][0],off->nv[i][0],i,off->sync[i]);
    }
    if(ar->pmf == -1) bksync_mec(); 
    arpsync_mec();
    return(0);
}

/* down up */
int arptn3_mec(int dt)
//int  dt;
{
    union meccode fd;
    int  i,j,k;
    int  p;
    int  lastp;
    int  dsyn; /* delta sync */
    int  csyn; /* current sync */
    int  dyna;
    int  keyp;
    int  nval;
    int  ptn;
    int  np[MAXDV];
    int  nv[MAXDV];

    /* printf("ar->pmf = %d ar->nval = %ld\n",ar->pmf,ar->nval); */
    /* printf("mr->temp=%d\n",mr->temp); */
    /* dsyn = 1500 * dt;  test */
    dsyn = 1000 * dt;
    dsyn = dsyn * mr->temp / 6000;
    csyn = mr->sync;
    k = startkp_mec(&j,&lastp);
    p = 0;
    for(i = j; i < lastp; i++) {
        if(isdigit(mg->card[i]) || mg->card[i] == '>' ||
            mg->card[i] == '<') {
            on->sync[p] = csyn;
            csyn = csyn - dsyn;
            if(csyn < 0) csyn = 3;
            dyna = mg->card[i] - '0';
            keyp = mg->ktb[k];
            if(mg->card[i] == '>') {
                dyna = 5;
                keyp = keyp + 1;
            }
            if(mg->card[i] == '<') {
                dyna = 5;
                keyp = keyp - 1;
            }
            on->np[p][0] = keyp;
            on->nv[p][0] = dyna * mg->velo;
            if(mg->debug == 6)
                printf("Debug6! on->np:nv[%d] = %d:%d on->sync[%d] = %d\n",
                    p,on->np[p][0],on->nv[p][0],p,on->sync[p]);
            p++;
        }
        k++;
    }
    on->dvc = p;
    for(i = 0 ; i < on->dvc; i++) {
        np[i] = on->np[i][0];
        on->sync[p] = csyn;
        csyn = csyn - dsyn;
        if(csyn < 0) csyn = 3;
        on->np[p][0] = on->np[i][0];
        on->nv[p][0] = on->nv[i][0];
        if(mg->debug == 6)
            printf("Debug6! on->np:nv[%d] = %d:%d on->sync[%d] = %d\n",
                p,on->np[p][0],on->nv[p][0],p,on->sync[p]);
        p++;
    }
    for(i = 0 ; i < on->dvc; i++) {
        off->sync[i] = on->sync[p-1] + 10;
        off->np[i][0] = np[i];
        off->nv[i][0] = 0;
        if(mg->debug == 6)
            printf("Debug6! off->np:nv[%d] = %d:%d off->sync[%d] = %d\n",
                i,off->np[i][0],off->nv[i][0],i,off->sync[i]);
    }
    bksync_mec();
    k = on->dvc;
    on->dvc = p;
    if(mg->debug == 6) printf("Debug6! on->dvc = %d ar->crate = %d%c\n",
        on->dvc,ar->crate,'%');
    for(i = k; i < on->dvc; i++) {
        /* Comment out 2012.7.7
        off->sync[i] = (on->sync[on->dvc - 1] * (100 - ar->crate)) / 100; 
        off->sync[i] = on->sync[on->dvc - 1] - 
            ((ar->nval * (100 - ar->crate))/100);
        */
        off->sync[i] = on->sync[on->dvc - 1] - 
            (ar->nval *  ar->crate/100);
        if(off->sync[i] <= 0) off->sync[i] = 1;
        off->np[i][0] = on->np[i][0];
        off->nv[i][0] = 0;
        if(mg->debug == 6)
            printf("Debug! off->np:nv[%d] = %d:%d off->sync[%d] = %d\n",
                i,off->np[i][0],off->nv[i][0],i,off->sync[i]);
    }
    if(ar->pmf == -1) bksync_mec(); 
    arpsync_mec();
    return(0);
}

int bksync_mec()
{
    int  i,j,k;
    int  np[MAXDV];
    int  nv[MAXDV];

    k = on->dvc - 1;
    for(i = 0; i < on->dvc; i++) {
        np[k] = on->np[i][0];
        nv[k] = on->nv[i][0];
        k--;
        if(k < 0) break;
    }
    for(i = 0; i < on->dvc; i++) {
        on->np[i][0] = np[i];
        on->nv[i][0] = nv[i];
        if(mg->debug == 6)
            printf("Debug6! on->np:nv[%d] = %d:%d Reverse\n",
                i,on->np[i][0],on->nv[i][0]);
    }
    k = on->dvc - 1;
    for(i = 0; i < on->dvc; i++) {  
        np[k] = off->np[i][0];
        nv[k] = off->nv[i][0]; 
        k--; 
        if(k < 0) break; 
    }       
    for(i = 0; i < on->dvc; i++) {
        off->np[i][0] = np[i];
        off->nv[i][0] = nv[i];
        if(mg->debug == 6)
            printf("Debug6! off->np:nv[%d] = %d:%d Reverse\n",
                i,off->np[i][0],off->nv[i][0]);
    }
    return(0);
}

/* Arpeggio sync out */
int arpsync_mec()
{
    int  i,j,k;
    int  ok;
    int  fk;
    long csyn;

    ok = 0;
    fk = 0;
    if(mg->debug == 6)
        printf("Debug6! Plus mr->sync = %ld on->dvc = %d\n",mr->sync,on->dvc);
    while(1) {
        if(on->sync[ok] > off->sync[fk]) {
            csyn = on->sync[ok];
            if(mg->debug == 6)
                printf("Debug6! on->sync[%d] = %d\n",
                    ok,on->sync[ok]);
            if(csyn > 0) {
                if(mr->sync != csyn) syncout_mec(csyn,1); /* type : lag */
                mr->keyp = on->np[ok][0];
                mr->veon = on->nv[ok][0];
                if(mr->keyp != -1) dexputfd_mec();
            }
            ok++;
        }
        if(off->sync[fk] > on->sync[ok]) {
            csyn = off->sync[fk];
            if(mg->debug == 6)
                printf("Debug6! off->sync[%d] = %d\n",
                    fk,off->sync[fk]);
            if(csyn > 0) {
              if(mr->sync != csyn) syncout_mec(csyn,2); /* type : offset */
            }
            mr->keyp = off->np[fk][0];
            mr->veon = off->nv[fk][0];
            if(mr->keyp != -1) dexputfd_mec();
            fk++;
        }
        if(off->sync[fk] == on->sync[ok]) {
            csyn = on->sync[ok];
            if(mg->debug == 6)
                printf("Debug6! on/off->sync[%d] = %d\n",
                    ok,on->sync[ok]);
            if(csyn > 0) {
                if(mr->sync != csyn) syncout_mec(csyn,1); /* type : lag */
                mr->keyp = on->np[ok][0];
                mr->veon = on->nv[ok][0];
                if(mr->keyp != -1) dexputfd_mec();
            }
            mr->keyp = off->np[fk][0];
            mr->veon = off->nv[fk][0];
            if(mr->keyp != -1) dexputfd_mec();
            ok++;
            fk++;
        }
        if(ok > on->dvc - 1) break; 
        if(fk > on->dvc - 1) break;
    }
    if(fk <= on->dvc - 1) {
        while(1) {
            csyn = off->sync[fk];
            if(mg->debug == 6)
                printf("Debug6! last/off->sync[%d] = %d\n",
                    fk,off->sync[fk]);
            if(csyn > 0) {
                if(mr->sync != csyn) syncout_mec(csyn,2); /* type : offset */
            }
            mr->keyp = off->np[fk][0];
            mr->veon = off->nv[fk][0];
            if(mr->keyp != -1) dexputfd_mec();
            fk++;
            if(fk > on->dvc - 1) break;
        }
    }
    return(0);
}

int getexp_mec()
{
    int  k;
    int  sf;
    char line[82];
    char card[82];

    k = 0;
    strcpy(card,mg->card); /* save */
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]);
        if(cp->key == 2) strcpy(line,irule[k]);
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
         /* *+: get *+abc dynamic expand */
        sf = 0;
        if(line[0] == '*' && line[1] == '+') sf = 1;
        if(line[0] == '*' && line[1] == '-') sf = 1;
        if(sf) {
            /* printf("%s",line); */
            strcpy(mg->card,line);
            addexp_mec();
        }
    }
    strcpy(mg->card,card); /* back */
    return(0);
}

/* add dynamic expand symbol call by *+abc line */
int addexp_mec()
{
    int  i,j;
    int  k;
    int  sf;
    int  pf;
    int  nf;
    int  vf;
    int  fbf;
    int  len;
    char wk[4];

    wk[0] = mg->card[2];
    wk[1] = mg->card[3];
    wk[2] = mg->card[4];
    wk[3] = '\0';
    /* +axy Arpeggio ? */
    if(wk[0] == 'a' || wk[0] == 'A') {
        if(isdigit(wk[1]) && isdigit(wk[2])) return(0);
    }
    spacecard_mec();
    if(mg->debug == 6) printf("\nDebug6! %s",mg->card);
    /* set ar->dxs */
    k = ar->nmb;
    sf = 0;
    for(i = 0; i < ar->nmb; i++) {
        if(!strcmp(wk,ar->dxs[i])) {
            sf = 1;
            break;
        }
    }
    if(sf) {
        k = i;
        strcpy(ar->dxs[k],wk); 
    }
    else {
        strcpy(ar->dxs[k],wk); 
        ar->nmb++;
        if(ar->nmb > MAXDXS) {
            mg->wrncnt++;
            mg->msglvl = 2;
            if(mg->msglvl >= mg->prtlvl)
            printf("Worning! dynamic expand symbol count over than %d\n",
            MAXDXS);
            ar->nmb--;
        }
    }
    if(mg->debug == 6) printf("Debug6! ar->dxs[%d] = %s\n",k,ar->dxs[k]);
    /* set ar->typ */
    i = 0;
    sf = 0;
    len = strlen(mg->card);
    while(1) {
        if(mg->card[i] == 'r' || mg->card[i] == 'R') {
            if(mg->card[i+1] == '=' || mg->card[i+2] == '=') {
                sf = 1;
                break;
            }
        }
        i++;
        if(i > len - 4) break;
    }
    ar->typ[k] = sf;
    /* printf("sf = %d len = %d\n",sf,len); */
    /* set ar->rt */
    pf = 0;
    if(sf) {
        j = 0;
        while(1) {
            if(mg->card[i] == '\"') {
                while(1) {
                    i++;
                    if(i > len) break;
                    ar->rt[k][j] = mg->card[i];
                    if(mg->card[i] == '\"') {
                        pf = 1;
                        break;
                    }
                    j++;
                    if(j > MAXPTN) {
                        overerr_mec("Pos:24 ar->rt MAXPTN",j);
                        break;
                    }
                }
            }
            i++;
            if(i > len) break;
            if(pf) break;
        }
    }
    if(pf) {
        ar->rt[k][j] = '\0';
        if(mg->debug == 6) printf("Debug6! ar->rt[%d] = %s\n",k,ar->rt[k]);
    }
    /* set ar->nt */
    i = 0;
    nf = 0;
    while(1) {
        if(mg->card[i] == 'n' || mg->card[i] == 'N') {
            if(mg->card[i+1] == '=' || mg->card[i+2] == '=') {
                nf = 1;
                break;
            }
        }
        i++;
        if(i > len - 4) break;
    }
    if(nf) { 
        j = 0;
        pf = 0;
        while(1) {
            if(mg->card[i] == '\"') {
                while(1) {
                    i++;
                    if(i > len) break;
                    ar->nt[k][j] = mg->card[i];
                    if(mg->card[i] == '\"') {
                        pf = 1;
                        break;
                    }
                    j++;
                    if(j > MAXPTN) {
                        overerr_mec("Pos:25 ar->nt MAXPTN",j);
                        break;
                    }
                }
            }
            i++;
            if(i > len) break;
            if(pf) break;
        }
    }
    if(nf) {
        ar->nt[k][j] = '\0';
        if(mg->debug == 6) printf("Debug6! ar->nt[%d] = %s\n",k,ar->nt[k]);
    }
    /* set ar->vt */
    i = 0;  
    vf = 0;
    while(1) {
        if(mg->card[i] == 'v' || mg->card[i] == 'V') {
            if(mg->card[i+1] == '=' || mg->card[i+2] == '=') {
                vf = 1;
                break;
            } 
        }
        i++;
        if(i > len - 4) break;
    }
    if(vf) { 
        j = 0; 
        pf = 0; 
        while(1) {
            if(mg->card[i] == '\"') {
                while(1) {
                    i++;
                    if(i > len) break;
                    ar->vt[k][j] = mg->card[i];
                    if(mg->card[i] == '\"') {
                        pf = 1;
                        break;
                    }
                    j++;
                    if(j > MAXPTN) {
                        overerr_mec("Pos:26 ar->vt MAXPTN",j);
                        break;
                    }
                }
            }
            i++;
            if(i > len) break;
            if(pf) break;
        }
    }
    if(vf) {
        ar->vt[k][j] = '\0';
        if(mg->debug == 6) printf("Debug6! ar->vt[%d] = %s\n",k,ar->vt[k]);
    }
    /* set ar->ft */
    i = 0;
    fbf = 0;
    while(1) {
        if(mg->card[i] == 'b' || mg->card[i] == 'B') {
            if(mg->card[i+1] == '=' || mg->card[i+2] == '=') {
                fbf = 1;
                break;
            }
        }
        i++;
        if(i > len - 4) break;
    }
    if(fbf) {
        j = 0;
        pf = 0;
        while(1) {
            if(mg->card[i] == '\"') {
                while(1) {
                    i++;
                    if(i > len) break;
                    ar->ft[k][j] = mg->card[i];
                    if(mg->card[i] == '\"') {
                        pf = 1;
                        break;
                    }
                    j++;
                    if(j > MAXPTN) {
                        overerr_mec("Pos:27 ar->ft MAXPTN",j);
                        break;
                    }
                }
            }
            i++;
            if(i > len) break;
            if(pf) break;
        }
    }
    if(fbf) {
        ar->ft[k][j] = '\0';
        if(mg->debug == 6) printf("Debug6! ar->ft[%d] = %s\n",k,ar->ft[k]);
    }
    return(0);
}

int dexputfd_mec()
{
    union meccode fd;
    int  j;

    fd.zi[0] = 0xfd90;
    fd.zi[1] = 0;
    mr->chan &= 0x000f;
    fd.zi[0] |= mr->chan;
    mr->keyp = mr->keyp - mr->trnsv; /* add 99.10 */
    mr->keyp &= 0x00ff;
    j = mr->keyp << 8;
    fd.zi[1] |= j;
    /* mr->veon = mr->dyna * mg->velo; */
    if(mr->veon > 127) mr->veon = 127;
    mr->veon &= 0x00ff;
    fd.zi[1] |= mr->veon;
    if(mg->debug) {
        printf("Debug! dexputfd_() note on chan=%d ",mr->chan);
        printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
        mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
    }
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) note on chan=%d ",
        fd.zi[0],fd.zi[1],mr->chan);
        printf("keyp=%d trnsv=%d veon=%d dyna=%d velo=%2.1f\n",
        mr->keyp,mr->trnsv,mr->veon,mr->dyna,mg->velo);
    }
    /*
    if(mr->veon) mg->ntb[mr->keyp] = k;
    else mg->ntb[mr->keyp] = 0;
    */
    putcode_mec(&fd);
    return(0);
}

int overerr_mec(char msg[80],int ov)
//char msg[80];
//int  ov;
{
    mg->errcnt++;
    mg->msglvl = 3;     
    if(mg->msglvl >= mg->prtlvl) {
        /* printf("Error! Arrange %s %06d MAXDV over %d\n",msg,mg->line,ov); */
        printf("Error! Arrange %s over %d bar = %05d\n",msg,ov,mr->bnum);
    }
    return(0);
}

