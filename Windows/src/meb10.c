
// ----------------------
//  AMuPLC (meb10.c)
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
extern MEBVAR *mb;
extern MEBSRG *ls;
extern MECVAR *mr;
extern MEBCPS *cp;
extern MEBCSL *cs;
extern MEBANS *ma;
extern MEBVEX *vx;
extern char jrule[MAXRL][82];  /* Major Rule */
extern char irule[MAXRL][82];  /* minor Rule */
extern char *bfc;
extern char *bsc;
extern char Scale[MAXLVR];
extern char Scalex[MAXLVR];
int  tmc;
int  gvc;
int  dxc;
int  vlc;
int  pvc;  
int  mdc;

int setavns_meb(char skey[MAXLVL])
//char skey[MAXLVL];
{
    int  i,j;
    int  cf;
    int  pf;
    int  rt;
    int  rtn;
    char tk[MAXLVR];
    char sk[MAXLVR];
    char Cn[8];  /* Chord name */

    /* printf("%s\n",mv->card); */
    if(cp->pol) {
        /* rtn = Polyobs_meb();  Polyavns_meb() */
        return(rtn);
    }
    ma->aaf = 0;
    cf = getchord_meb(2,Cn); /* cf : C D E F G A B */
    if(mv->debug == 6) printf("setavns_meb() skey = %s Cn = %s cf = %d\n",
        skey,Cn,cf);
    if(cf == -2) return(1); /* error chord */
    if(cp->bf) {
        i = 0; 
        while(1) { 
            if(Cn[i] == '\0') break;
            if(Cn[i] == '/') {
                Cn[i] = '\0';
                break;
            }
            i++;
            if(i > 8) break;
        }
    }
    strcpy(tk,"$A:");
    strcat(tk,skey);

    // add 2019.12.17 on Chord 
    j = 0;
    while(1) {
        if(tk[j] == '/') {
            tk[j] = '\0';
            break;
        }
        if(tk[j] == '\0') break;
        j++;
        if(j > MAXLVR) break;
    }
    /////////////////

    pf = maget_cps(tk,sk);
    if(mv->debug == 6) printf("pf = %d tk = %s sk = %s\n", pf,tk,sk);
    if(pf) return(2); /* no chord */
    rtn = getRoot_meb(2,&rt); /* rt : Root Column position */
    if(rtn == 3 || rtn == 4 || rtn == 7) modRoot_meb(&rt);
    if(mv->debug == 6) printf("rtn = %d rt = %d\n",rtn,rt);
    chkaval_meb(rtn,rt,sk);
    ma->aaf = 1;
    return(0);
}

/* check (available note scale) interval from Root */
int chkaval_meb(int sf,int rt,char Note[MAXLVR])
//int  sf; /* # & flag */
//int  rt; /* Root position */
//char Note[MAXLVR];  
{
    int  i; 
    int  len;   
    int  iv;    
    int  rv;    
    int  ip;
    char Nc;
        
    len = strlen(Note);
    iv = 0; 
    for(i = 3; i < 69; i++) ma->anl[i] = ' ';
    ma->anl[0] = '!';
    ma->anl[1] = 'y';
    ma->anl[2] = 'y';
    ma->anl[69] = '\0'; 
    strcat(ma->anl,"xx.x ._...");
    if(mv->debug == 6) printf("chkaval() sf = %d rt = %d Note = %s len = %d\n",
        sf,rt,Note,len);   
    rv = getaval_meb(Note);
    if(rv > 0) {
        mv->wrncnt++;
        mv->msglvl = 0;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Worning! Available Note Scale %s\n",Scale);
        }
    }
    ip = 0;
    for( i = 0; i < len; i++) {
        if(Note[i] != ' ') {
            if(ip == 0) {
               Nc = Note[i];
               ip++;
            }
            iv = getival_meb(i,Note);
            iv = iv - rv;
            setaval2_meb(sf,rt,iv,Note[i]);
            setaval3_meb(rt,iv,Note[i]);
        }   
    }       
    ma->anl[rt] = Nc; /* 2003.9 */
    /* printf("%s\n",ma->anl); */
    return(0);
}       

/* get avil interval from Root in C */
int getaval_meb(char Note[MAXLVR])
//char Note[MAXLVR];
{   
    int  cf;
    char wk[MAXLVR];
    
    strcpy(wk,Scale);
    cf = -2;
    if(wk[0] == 'C') cf = 0;
    if(wk[0] == 'D') cf = 2;
    if(wk[0] == 'E') cf = 4;
    if(wk[0] == 'F') cf = 5;
    if(wk[0] == 'G') cf = 7; 
    if(wk[0] == 'A') cf = 9;
    if(wk[0] == 'B') cf = 11;
    if(cf > -2) {
        if(wk[1] == '#') cf = cf + 1; 
        if(wk[1] == '&') cf = cf - 1; 
        if(wk[1] == '*') cf = cf + 2; 
        if(wk[1] == '@') cf = cf - 2; 
    }
    if(mv->debug == 6) printf("Scale = %s cf = %d Note = %s\n",Scale,cf,Note);
    return(cf);
}   

int maget_cps(char tk[MAXLVR],char rtey[MAXLVR])
//char tk[MAXLVR];
//char rtey[MAXLVR];
{
    int  i,j,k;
    char sk[MAXLVL];
    
    if(mv->debug == 6) printf("Debug6! maget_cps() tk=%s\n",tk);
    i = 3;
    if(tk[2] != ':') i = 1; 
    j = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == '~') break;
        sk[j] = tk[i];
        i++;
        if(i > MAXLVL) break;
        j++;
    }
    sk[j] = '\0';
    if(mv->debug == 6) printf("maget() sk = %s ma->n = %d\n",sk,ma->n);
    Scalex[0] = '_';
    Scalex[1] = '.';
    Scalex[2] = '.';
    Scalex[3] = '.';
    Scalex[4] = '\0';
    rtey[0] = '\0';
    for(k = 0; k < ma->n; k++) {
        if(!strcmp(ma->l[k],sk)) {
            strcat(rtey,ma->m[k]);
            strcpy(Scale,ma->r[k]);
            strcpy(Scalex,ma->o[k]);
            if(mv->debug == 6)
                printf("maget() rtey = %s Scale = %s Scalex = %s\n",
                rtey,Scale,Scalex);
            return(0);
        }
    }
    return(1);
}   

/* Note set by interval */
int setaval2_meb(int sf,int rt,int iv,char Nc)
//int  sf; /* 1:& 2:# 3:&No 4:#No flag 5:&Root 6:#Root */
//int  rt; /* Root column */
//int  iv; /* interval from Root */
//char Nc; /* Note char */
{
    int  i,j;
    int  k1,k2;
    int  dv;
    int  p1,p2,pd;
    /* int  r1,r2,rd; */
    int  tf;
    int  pf;
    int  ft;
    char wk[82];

    if(mv->debug == 6) {
        printf("setaval2() ma->avc = %s\n",ma->avc);
        printf("Nc = %c sf = %d rt = %d iv = %d\n",Nc,sf,rt,iv);
    }
    if(rt < 11 || rt > 68) return(1); /* rt error */
    k1 = getkeyp_meb(rt,3); /* Root key position */
    p1 = getkeyp_meb(rt,1);
    p2 = getkeyp_meb(rt,3);
    pd = p2 - p1;
    /* Root key Position pd  1:#  -1:&  */
    if(pd == 0) {
        ma->anl[rt] = '.';
        if(sf == 1 || sf == 5) { /* Root& */
            ma->anl[rt] = '<';
            k1 = k1 - 1;
        }
        if(sf == 2 || sf == 6) { /* Root# */
            ma->anl[rt] = '>';
            k1 = k1 + 1;
        }
    }
    if(pd == 1) {  /* # key */
        ma->anl[rt] = '<';
        k1 = k1 - 1;
        if(sf == 0 || sf == 1 || sf == 5) {
            ma->anl[rt] = '.';
            k1 = getkeyp_meb(rt,3); /* Root key position */
        }
    }
    if(pd == -1) { /* & key */
        ma->anl[rt] = '>';
        k1 = k1 + 1;
        if(sf == 0 || sf == 2 || sf == 6) {
            ma->anl[rt] = '.';
            k1 = getkeyp_meb(rt,3); /* Root key position */
        }
    }
    if(mv->debug == 6 && iv == 0) {
        printf("Nc= %c Root pd= %d sf= %d ma->anl[%d]= %c\n",
            Nc,pd,sf,rt,ma->anl[rt]);
    }
    if(iv != 0) { /* Not Root */
        tf = 0;
        for(i = rt; i < 69; i++) {
            k2 = getkeyp_meb(i,3);
            dv = k2 - k1;
            /* printf("dv = %d k1 = %d k2 = %d\n",dv,k1,k2); */
            if(dv == iv) {
                if(tf == 0) {
                    tf = 1;
                    /* ma->anl[i] = '.'; */
                    ma->anl[i] = Nc; 
                    if(mv->debug == 6) {
                       printf("(1) Nc= %c dv,iv= %d ma->anl[%d]= %c\n",
                       Nc,dv,i,ma->anl[i]);
                    }
                }
                /* printf("ma->anl[%d] = %c\n",i,ma->anl[i]); */
            }
            else {
                if(Nc == '#' || Nc == '.') {
                    if(tf == 0) {
                        if((dv - 1) == iv) { /* iv over */
                            tf = 1; 
                            ma->anl[i] = '<';  /* . */
                            if(mv->debug == 6) {
                             printf("(2) Nc= %c dv-1,iv= %d ma->anl[%d]= %c\n",
                             Nc,dv - 1,i,ma->anl[i]);
                            }
                        }
                        if((dv + 1) == iv) { /* iv less */
                            if(ma->anl[i] == ' ') {
                                tf = 1;
                                ma->anl[i] = '>';
                            }
                            else tf = 0;
                            if(mv->debug == 6) {
                             printf("(3) Nc= %c dv+1,iv= %d ma->anl[%d]= %c\n",
                             Nc,dv + 1,i,ma->anl[i]);
                            }
                        }
                    }
                }
                if(Nc == '&') {
                    if((dv + 1) == iv) { /* iv less */
                        if(tf == 0) {
                            if(ma->anl[i] == ' ') {
                                tf = 1;
                                ma->anl[i] = '>';
                            }
                            else tf = 0;
                            if(mv->debug == 6) {
                             printf("(4) Nc= %c dv+1,iv= %d ma->anl[%d]= %c\n",
                             Nc,dv + 1,i,ma->anl[i]);
                            }
                        }
                    }
                    if((dv - 1) == iv) { /* iv over */       
                        if(tf == 0) {
                            tf = 1;
                            ma->anl[i] = '<';
                            if(mv->debug == 6) {
                             printf("(5) Nc= %c dv-1,iv= %d ma->anl[%d]= %c\n",
                             Nc,dv - 1,i,ma->anl[i]);
                            }
                        }
                    }
                }
            }
        }
    }
    if(mv->debug == 6) printf("%s\n",ma->anl);
    return(0);
}

int setaval3_meb(int rt,int iv,char Nc)
//int  rt; /* Root column */
//int  iv; /* interval from Root */
//char Nc; /* Note char */
{
    int  i,j;
    int  k1,k2;
    int  ft;
    int  dv;
    int  len;

    for(i = rt; i < 69; i++) {
        if(ma->anl[i] == '<') {
            k1 = getkeyp_meb(i-1,3);
            k2 = getkeyp_meb(i,3);
            /* printf("k1 = %d k2 = %d\n",k1,k2); */
            dv = k2 - k1;
            if(dv == 1) {
                ma->anl[i] = ' '; 
                ma->anl[i - 1] = '.';
            }
        }
        if(ma->anl[i] == '>') {
            k1 = getkeyp_meb(i,3);      
            k2 = getkeyp_meb(i+1,3);
            /* printf("k1 = %d k2 = %d\n",k1,k2); */
            dv = k2 - k1;
            if(dv == 1) {
                ma->anl[i] = ' ';
                ma->anl[i + 1] = '.';
            }
        }
    }
    ft = 32;
    if(mb->bartyp == 'l') ft = 10;
    if(mb->bartyp == 'm') ft = 10;
    if(mb->bartyp == 'h') ft = 10;
    if(mb->bartyp == 't') ft = 32;
    if(mb->bartyp == 's') ft = 32;
    if(mb->bartyp == 'g') ft = 32;
    if(mb->bartyp == 'a') ft = 32;
    if(mb->bartyp == 'b') ft = 32;
    j = 0;
    len = strlen(ma->avc);
    /* printf("setaval3_() len = %d ma->avc = %s\n",len,ma->avc); */
    /* for(i = rt - 1; i < 69; i++) { */
    for(i = ft; i < 69; i++) {
        if(ma->anl[i] == '.') ma->anl[i] = ma->avc[j];
        if(ma->anl[i] == '>') if(ma->avc[j] == '.') ma->anl[i] = '+';
        if(ma->anl[i] == '<') if(ma->avc[j] == '.') ma->anl[i] = '-';
        if(ma->anl[i] != ' ') j++;
        if(j >= len) break;
        if(j > 50) break;
    }
    /* printf("%s\n",ma->anl); */
    return(0);
}

int trnstep_meb(char *inf,struct mivksn *mk,struct mivsym ms[])
//char *inf;
//struct mivksn *mk; 
//struct mivsym ms[];
{   
    int  i,j,k; 
    FILE *fpi;
    char obsf[STLN];
    char mixf[STLN];
    char c1;
    int  dxf;
        
    mv->line = 0;
    mb->ptrnsv = 0;
    mb->bnum = 0;
    ls->sequ = 0;               
    ls->bnum = 0;
    dxc = 0;
    mv->tempo = 60.00;
    vlc = 0;
    pvc = 0;
    mdc = 0;
    if(mv->debug == 5) printf("trnstep_() mb->observf = %d\n",mb->observf);
    if(mb->observf) {
        i = 0;
        while(inf[i] != '_') {
            mixf[i] = inf[i];
            i++;
        }
        mixf[i] = '\0';
        strcat(mixf,".mix");    
        if(mv->debug == 5) printf("mixf = %s\n",mixf);
        ls->fmxopend = 0;
        if((ls->fmx = fopen(mixf,"rb")) == NULL) {
            mv->wrncnt++;
            mv->msglvl = 2;
            if(mv->msglvl >= mv->prtlvl)
            printf("Worning! observe file \"%s\" not found\n",mixf);
            mb->observf = 0;
        }
        else ls->fmxopend = 1;
    }
    for(i = 0; i < 128; i++) ls->obstb[i] = 0;
    i = 0;
    while(inf[i] != '.') {
        obsf[i] = inf[i];
        i++;
    }
    obsf[i] = '\0';
    strcat(obsf,".mec");
    if(mv->debug == 10) printf("Debug10! trnstep() inf = %s obsf = %s\n",
        inf,obsf);
    if((ls->fob = fopen(obsf,"rb")) == NULL) {
        mv->wrncnt++;
        mv->msglvl = 0;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Worning! Observe file \"%s\" not found\n",obsf);
        }
        ls->fobopend = 0;
        mb->observf = 0;
    }   
    else ls->fobopend = 1;
    mopen_meb(); /* trnstep */
    // startval_meb(); 
    // mb->chdobf = mb->achdobf;  add 2013.10.13
    getNotew_meb(); /* get ka,kb,kc,kd from rule file */
    /* while(fgets(mv->card, 82, fpi)) { */
    while(mgets_meb(mv->card)) {
        for(i = 0; i < 82; i++) {
            if(mv->card[i] == '\n' || mv->card[i] == '\r') {
                mv->card[i] = '\0';
                break;  
            }
        }   
        mv->line++;
        switch (mv->card[0]) {
            case '%'  : /* Note line */
                        insutrns_meb();
                        if(mb->GTPf) GTrans_meb();
                        /* printf("ma->nsf = %d\n",ma->nsf); */
                        for(i = 0; i < 128; i++) {
                            cs->bv[i] = cs->kgb * cs->cv[i];
                            cs->cv[i] = 0;
                        }
                        if(ma->nsf == 1) chdNote_meb();
                        if(ma->nsf == 2) avlNote_meb();
                        if(ma->nsf == 3) {
                            chdNote_meb();
                            avlNote_meb();
                        }
                        if(mb->observf) {
                            if(mb->bartyp != 'r') {
                                beauobsv_meb();
                                beauobst_meb();
                            }
                        }
                        vermod_meb();
                        mb->renp = 1;
                        renget_meb();
                        if(mb->renp >= MSTC) {
                            mv->errcnt++;
                            mv->msglvl = 3;
                            if(mv->msglvl >= mv->prtlvl)
                                printf("Error! %06d renpu max is %d\n",
                                mv->line,MSTC);
                            break;
                        }  
                        if(mb->renp > 1) {
                            rensync_meb(fpi);
                            /* strcpy(mv->card,scard[0]); */
                        }
                        else for(i = 0; i < mb->rept; i++) {
                            notesync_meb();
                        }
                        break;
            case '-'  : /* Bar line */
                        barsync_meb();
                        mb->bartyp = mv->card[1];
                        setbnum_meb();
                        break;
            case '*'  : /* Vertical modify */
                        c1 = mv->card[1];
                        if(c1 == '+' || c1 == '-') addexp_meb();
                        if(c1 == 'E' || c1 == 'V' || c1 == 'P' || c1 == 'M' ||
                           c1 == 'e' || c1 == 'v' || c1 == 'p' || c1 == 'm') {
                            verexp_meb();
                        }
                        break;
            case 'R'  : /* Rest */
                        insutrns_meb();
                        if(mb->GTPf) GTrans_meb();
                        break;
            case 'r'  : /* rest */
                        insutrns_meb();
                        if(mb->GTPf) GTrans_meb();
                        break;
            case '{'  : /* Gloval */ 
                        if(mv->card[1] == 'm' && mv->card[3] != '.') {
                            exptmp_meb();
                            break;
                        }
                        if(mv->card[1] == '?') {
                            chordsel_cps();
                            break;
                        }
                        if(mv->card[1] == '!') {
                            chord_cps();
                            break;
                        }
                        if(mv->card[1] == '$' && mv->card[2] == ':') {
                            setlvl_cps();
                            break;
                        }   
                        if(mv->card[1] == '$' && mv->card[3] == ':') {
                            setlvl_cps();
                            break;
                        }   
                        if(mv->card[1] == '$') {
                            trnskey_meb(mk);
                            key_meb();
                            setkey_cps();
                            break;
                        }
                        beauexp_meb();
                        break;
            case '>'  : if(mv->card[1] == '{') {
                            beauexp_meb();
                            break;
                        }
            case '('  : /* Local */ 
                        if(mv->card[2] == ':' || mv->card[3] == ':' ||
                            mv->card[4] == ':') {
                            break;
                        }
                        if(mv->card[1] == '$') {
                            trnskey_meb(mk);
                            key_meb();
                            setkey_cps();
                        }
                        if(mv->card[1] == '?') {
                            chordsel_cps();
                            lbrtosbr_meb();
                            break;
                        }
                        if(mv->card[1] == '!') {
                            chord_cps();
                            break;
                        }
                        break;
            case '$'  : if(mv->card[2] == '$') break;
                        if(mv->card[1] == ':' || mv->card[2] == ':' ||
                            mv->card[3] == ':') {
                            setlvl_cps();  
                            break;
                        }   
                        trnskey_meb(mk);
                        key_meb();
                        setkey_cps();
                        break;
            case '+'  : /* Plus line */
                        insutrns_meb();
                        if(mb->GTPf) GTrans_meb();
                        vermod_meb();
                        break;
            case ':'  : if(mv->byps != 1) break;
                        insutrns_meb();
                        if(mb->GTPf) GTrans_meb();
                        vermod_meb();
                        break;
            case '!'  : /* chord */
                        chord_cps();
                        break;  
            case '?'  : /* chord select step 1 */
                        break;  
            case '@'  : /* out @inst line to fn.mit */
                        getinstv_meb(ms);
                        break;
            case '#'  : /* #define #set .. */
                        strck_meb();
                        break;
            default   : break;
        }
        if(mv->card[0] == '(' && mb->xstolf) {
            sbrtolbr_meb();
        } 
        if(mv->card[0] == '{' && mb->xltosf &&
            (mv->card[1] != '/') && (mv->card[75] != '/')) {
            lbrtosbr_meb();
        }
        dxf = 0;
        if(mv->card[75] == '+') dxf++;
        if(mv->card[75] == '-') dxf++;
        if(mb->decxp && mv->card[0] == '%' && dxf) decdxp_meb();
        else {
            if(mv->source == 3) printf("%s\n",mv->card);
            mprintf_meb(mv->card);
        }
    }   
    //if(ls->fobopend) fclose(ls->fob);  // 2025.4.2
    /*
    // comment out 2025.4.2 core dump why ?
    if(ls->fobopend) {  
        fclose(ls->fob);    
        ls->fobopend = 0;
    }
    */
    return(0);
}

int chdNote_meb()
{
    int  i,j,k;
    int  keyp;
    char wk[82];

    if(mv->debug == 6) {
      printf("chdNote() ma->snf= %d ma->arp= %d mb->bartyp= %c mb->ksin= %d ",
        ma->nsf,ma->arp,mb->bartyp,mb->ksin);
        printf("mb->chdobf= %d\n",mb->chdobf);
        printf("        cp->caf = %d ma->aaf = %d\n",cp->caf,ma->aaf);
    }
    k = 5;  /* mb->bartyp == 'm' */
    j = 10;
    if(mb->bartyp == 'l') k = 0;
    if(mb->bartyp == 'h') k = 16;
    if(mb->bartyp == 't') {
        k = 35;
        j = 32;
    }
    if(mb->bartyp == 'g') {
        k = 28;
        j = 32;
    }
    if(mb->bartyp == 's') {
        k = 21;
        j = 32;
    }
    if(mb->bartyp == 'a') {
        k = 14;
        j = 32;
    }
    if(mb->bartyp == 'b') {
        k = 7;
        j = 32;
    }
    if(mb->bartyp == 'u') {
        k = 0;
        j = 32;
    }
    if(mb->chdobf == 3) {
        // printf("Test //// j = %d ma->anl = %s\n",j,ma->anl);
        for(i = j; i < 69; i++) mv->card[i] = ' ';  // test
    }
    if(cp->caf) {
        for(i = j; i < 69; i++) {
            if(cp->cnl[i] == '\0') break;
            if(mv->card[i] == ' ') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '.') mv->card[i] = cp->cnl[i];
            /* Coment out 2007.2.18
            if(mv->card[i] == '>') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '<') mv->card[i] = cp->cnl[i];
            */
            if(mv->card[i] == 'x') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == 'o') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '-') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '+') mv->card[i] = cp->cnl[i];
        }
    }
    return(0);
}

/* Available Note Scale set */
int avlNote_meb()
{
    int  i,j,k;
    int  keyp;
    char wk[82];

    if(mv->debug == 6) {
      printf("avlNote() ma->snf= %d ma->arp= %d mb->bartyp= %c mb->ksin= %d ",
        ma->nsf,ma->arp,mb->bartyp,mb->ksin);
        printf("mb->chdobf= %d\n",mb->chdobf);
        printf("        cp->caf = %d ma->aaf = %d\n",cp->caf,ma->aaf);
    }
    // printf("%s\n",mv->card);
    k = 5;  /* mb->bartyp == 'm' */
    j = 10;
    if(mb->bartyp == 'l') k = 0;
    if(mb->bartyp == 'h') k = 16;
    if(mb->bartyp == 't') {
        k = 35;
        j = 32;
    }
    if(mb->bartyp == 'g') {
        k = 28;
        j = 32;
    }
    if(mb->bartyp == 's') {
        k = 21;
        j = 32;
    }
    if(mb->bartyp == 'a') {
        k = 14;
        j = 32;
    }
    if(mb->bartyp == 'b') {
        k = 7;
        j = 32;
    }
    if(mb->bartyp == 'u') {
        k = 0;
        j = 32;
    }
    /* debug 2019.12.19
    printf("iiiiii cp->cnl=\n%s\n",cp->cnl);  // test 2019.12.19
    printf("jjjjjj ma->anl=\n%s\n",ma->anl);  // test 2019.12.19
    printf("%s\n",mv->card);
    */
    if(cp->caf && ma->aaf) {
        //strcpy(wk,cp->cnl);
        strcpy(wk,ma->anl);
        wk[1] = 'z';
        wk[2] = 'z';
        for(i = 0; i < 82; i++) {
            if(cp->cnl[i] == '\0') cp->cnl[i] = ' ';
            if(ma->anl[i] == '\0') break;
            //if(wk[i] == ' ') wk[i] = ma->anl[i];
            if(wk[i] == ' ') wk[i] = cp->cnl[i];
        }
        // printf("kkkkk wk=\n%s\n",wk);
        for(i = j; i < 69; i++) {
            if(mv->card[i] == ' ') mv->card[i] = wk[i];
            if(mv->card[i] == '.') mv->card[i] = wk[i];
            /* Comment out 2007.2.18
            if(mv->card[i] == '>') mv->card[i] = wk[i];
            if(mv->card[i] == '<') mv->card[i] = wk[i];
            */
            if(mv->card[i] == 'x') mv->card[i] = wk[i];
            if(mv->card[i] == 'o') mv->card[i] = wk[i];
            if(mv->card[i] == '-') mv->card[i] = wk[i];
            if(mv->card[i] == '+') mv->card[i] = wk[i];
        }
    }
    // printf("llllll Scalex = \n%s\n",Scalex);
    // printf("%s\n",mv->card);
    if(cp->caf && ma->aaf == 0) {
        /* printf("j = %d cp->cnl = %s\n",j,cp->cnl); */
        for(i = j; i < 69; i++) {
            if(cp->cnl[i] == '\0') break;
            if(mv->card[i] == ' ') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '.') mv->card[i] = cp->cnl[i];
            /* Comment out 2007.2.18
            if(mv->card[i] == '>') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '<') mv->card[i] = cp->cnl[i];
            */
            if(mv->card[i] == 'x') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == 'o') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '-') mv->card[i] = cp->cnl[i];
            if(mv->card[i] == '+') mv->card[i] = cp->cnl[i];
        }
    }
    if(cp->caf == 0 && ma->aaf) {
        for(i = j; i < 69; i++) {
            if(ma->anl[i] == '\0') break;
            if(mv->card[i] == ' ') mv->card[i] = ma->anl[i];
            if(mv->card[i] == '.') mv->card[i] = ma->anl[i];
            /* Comment out 2007.2.18
            if(mv->card[i] == '>') mv->card[i] = ma->anl[i];
            if(mv->card[i] == '<') mv->card[i] = ma->anl[i];
            */
            if(mv->card[i] == 'x') mv->card[i] = ma->anl[i];
            if(mv->card[i] == 'o') mv->card[i] = ma->anl[i];
            if(mv->card[i] == '-') mv->card[i] = ma->anl[i];
            if(mv->card[i] == '+') mv->card[i] = ma->anl[i];
        } 
    }
    if(mb->chdobf == 2) {
        if(cp->caf == 0 && ma->aaf == 0) {
            for(i = j; i < 69; i++) mv->card[i] = ' '; 
        }
        if(cp->caf) {
            for(i = j; i < 69; i++) {
                if(cp->cnl[i] == '\0') break;
                mv->card[i] = cp->cnl[i];
            }
        }
        if(ma->aaf) {
            /* printf("j = %d ma->anl = %s\n",j,ma->anl); */
            for(i = j; i < 69; i++) {
                if(ma->anl[i] == '\0') break;
                if(mv->card[i] == ' ') mv->card[i] = ma->anl[i];
            }
        }
    }
    if(Scalex[0] == '+' || Scalex[0] == '-') {
        if(mv->card[75] == '_' && mv->card[78] == '.') { /* add Scalex */
            mv->card[75] = Scalex[0];
            mv->card[76] = Scalex[1];
            mv->card[77] = Scalex[2];
            mv->card[78] = Scalex[3];
        }
        /* printf("%s\n",mv->card); */
    }
    return(0);
}

/*
 chord select
   get data from fn_xyz.obj
*/
int chordsel_cps()
{
    int  i;
    int  bnc;
    int  cnc;
    int  anc;

    if(ls->fobopend) objchdsl_meb();
    bnc = 0;
    cnc = 0;
    anc = 0;
    for(i =0; i < 128; i++) {
        if(cs->bv[i]) bnc++;
        if(cs->cv[i]) cnc++;
        if(cs->av[i]) anc++;
    }
    if(mv->debug == 7) {
        printf("chordsel_() mb->sequ=%d mb->bnum=%d bnc(Befor Note Count)=%d ",
        mb->sequ,mb->bnum,bnc);
        printf("cnc(Current)=%d anc(After)=%d\n",cnc,anc);
    }
    beaugsln_meb(); /* modify mv->card */
    return(0); 
}

int objchdsl_meb()
{       
    union MCODE u;
    unsigned char iword[4];
    int c;
    int i,j,k;
    
    if(mv->debug == 7) printf("objchdsl_() ls->sequ = %d ls->bnum = %d\n",
        ls->sequ,ls->bnum);
    k = 0;
    for(i = 0; i <128; i++) cs->av[0] = 0;
    while(1) {
        /* if(ls->sequ == mb->sequ && ls->bnum <= mb->bnum) break; */
        if(ls->sequ == mb->sequ && ls->bnum > mb->bnum) break;
        if((c = fgetc(ls->fob)) == EOF) {
            mb->observf = 0;
            ls->fobopend = 0;
            fclose(ls->fob);
            return(9);
        }
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            compchk_mlk(&u,iword);
            k = 0;
            switch (u.wc[1]) {
                case 0xfb : fbdecode_mlk(&u);
                            for(i = 0; i < 128; i++) {
                                cs->bv[i] = cs->kgb * cs->cv[i];
                                cs->cv[i] = cs->kgc * cs->av[i];
                                cs->av[i] = 0;
                            }
                            break;
                case 0xfd : gfdset_mlk(&u);
                            break;
                default   : break;
            }
        }
    }
    return(0);
}

int insutrns_meb()
{
    char wk[4];
    int  pf;
    int  tpv;

    pf = 0;
    if( mb->bartyp == 's' ||
        mb->bartyp == 't' ||
        mb->bartyp == 'g' ||
        mb->bartyp == 'a' ||
        mb->bartyp == 'u' ||
        mb->bartyp == 'b') {
        pf = 1;
    }
    if(!pf) return(0);
    wk[0] = mv->card[29];
    wk[1] = mv->card[30];
    wk[2] = mv->card[31];
    wk[3] = '\0';
    sscanf(wk,"%d",&tpv);
    tpv = tpv + mb->ptrnsv;
    if(tpv < -90 || tpv > 90) {
        printf("Error! Transpose value over %d\n",tpv);
        return(1);
    }
    sprintf(wk,"%3d",tpv);
    mv->card[29] = wk[0]; 
    mv->card[30] = wk[1]; 
    mv->card[31] = wk[2]; 
    return(0);
}

int GTrans_meb()
{
    char wk[4];
    int  pf;  
    int  tpv;
            
    pf = 0; 
    if( mb->bartyp == 's' ||
        mb->bartyp == 't' ||
        mb->bartyp == 'g' ||
        mb->bartyp == 'a' ||
        mb->bartyp == 'u' ||
        mb->bartyp == 'b') {
        pf = 1;
    }
    if(!pf) return(0);
    wk[0] = mv->card[29];
    wk[1] = mv->card[30];
    wk[2] = mv->card[31];
    wk[3] = '\0';
    sscanf(wk,"%d",&tpv);
    tpv = tpv + mb->GTPv;
    if(tpv < -90 || tpv > 90) {
        printf("Error! Transpose value over %d\n",tpv);
        return(1);
    }
    sprintf(wk,"%3d",tpv);
    mv->card[29] = wk[0];
    mv->card[30] = wk[1];
    mv->card[31] = wk[2];
    return(0);
}

int trnskey_meb(struct mivksn *mk)
//struct mivksn *mk; 
{
    int  i,j;
    int  tabfl;
    char wk[82];

    if(mv->debug == 16) {
        printf("//trnskey_()  PASS-23-meb10.c // \n");
        printf("%s\n",mv->card);
    }
    if(mv->card[0] == '(' || mv->card[0] == '{') {
        i = 1;
        j = 0;
        while(1) {
            if(mv->card[i] == ')' || mv->card[i] == '}') break;
            wk[j] = mv->card[i];
            i++;
            j++;
            if(i > 79) break;
        }
        wk[j] = '\0';
        /* printf("wk = %s\n",wk); */
    }
    i = 3;
    tabfl = 0;
    while(mv->card[i]) {
        if(mv->card[i] == 'T' && mv->card[i+1] == 'r') tabfl = 1;
        i++;
        if(i > 79) break;
    }
    if(tabfl) mb->ptrnsv = numdinmb_meb(mv->card);
    else for(i = 7; i < 78; i++) mv->bktb[i] = mv->ktb[i]; /* set base key */
    chdkey2_mec(mk);
    return(0);
}

int chdkey2_mec(struct mivksn *mk)
//struct mivksn *mk;
{
    char wk[8];
    char c;
    int  i,j;
    int  err;
    int  pas;

    if(mv->debug == 10) printf("key_mec() \n");;
    keyini_mec();
    err = 0;
    pas = 0;
    i = 0;
    while(mv->card[i]) {
        if(mv->card[i] == '}') break;
        if(mv->card[i] == 'T' && mv->card[i-1] == ' ') break;
        i++;
    }
    while(mv->card[i] != 'r' && mv->card[i] != 'l') {
        i--;
        if(i < 0) break;
        pas = 1;
    }
    wk[0] = '\0';
    if(pas) {
        i = i - 2;
        for(j = 0; j < 3; j++) wk[j] = mv->card[i++];
        wk[3] = '\0';
    }
    if(!strcmp(wk,"dur")) mv->mkscl = 1;
    if(!strcmp(wk,"oll")) mv->mkscl = 2;
    if(!strcmp(wk,"jor")) mv->mkscl = 3;
    if(!strcmp(wk,"nor")) mv->mkscl = 4;
    i = 1;
    if(mv->card[1] == '$') i = 2;
    mb->ksin = 0;
    if(mv->card[i] == '#') {
        while(mv->card[i] == '#') {
            i++;
            mb->ksin++;
        }
    }
    if(mv->card[i] == '&') {
        mb->ksin = 7;
        while(mv->card[i] == '&') {
            i++;
            mb->ksin++;
        }
    }
    if(mb->ksin < 0 || mb->ksin > 14) {
        printf("Error! %s key signature\n",mv->card);
        mb->ksin = 0;
        return(-1);
    }
    mb->ksin &= 0x000f;
    if(mv->debug == 10) printf("Debug10! mb->ksin=%d\n",mb->ksin);
    c = mv->card[i];
    if(isalnum(c)) {
        if(c == 'c' || c == 'C') {
            mb->ryhm = 4;
            mb->time = 4;
        }
        else {
            if(!isdigit(c)) err = 1;
            j = 0;
            mb->ryhm = 0;
            while(mv->card[i]) {
                if(mv->card[i] == '/') break;
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) err =2;
            }
            if(j == 0 || j > 3) err = 3;
            if(err) {
                printf("Error! %s rhythm data error\n", mv->card);
                return(err);
            }
            wk[j] = '\0';
            sscanf(wk,"%d",&mb->ryhm);
            if(mb->ryhm > 255) {
                printf("Error! %d rhythm greater than 255\n",mb->ryhm);
                return(3);
            }
            j = 0;
            i++;
            while(isdigit(mv->card[i])) {
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) {
                    printf("Error! %s rhythm(time) data error\n",
                    mv->card);
                    return(4);
                }
            }
            if(j == 0 || j > 3) {
                printf("Error! %s rhythm(time) data error\n",
                mv->card);
                return(5);
            }
            wk[j] = '\0';
            sscanf(wk,"%d",&mb->time);
            if(mb->time > 255) {
                printf("Error! %s ryhm(time) greater than 255\n",
                mv->card);
                return(6);
            }
        }
        if(mv->debug == 10) printf("Debug10! mb->ryhm=%d mb->time=%d\n",
            mb->ryhm,mb->time);
    }
    keyset_meb();
    if(mv->debug == 10) keydmp_mec(mk);
    return(0);
}

int Special_key(char line[82],char skey[MAXLVL],char wkey[MAXLVL])
//char line[82];
//char skey[MAXLVL];
//char wkey[MAXLVL];
{
    int  i,j,k;
    int  cf;
    int  sf;
    char wk[32];
    char ck[16];
    char cd[16];
    char card[82];
    char wline[82];
    char sl[MAXLVL];
    char sr[MAXLVR];

    /* printf("mv->card = %s\n",mv->card); */
    cf = cdcheck_meb(skey,ck,cd);
    if(!cf) {
        sf = 0;
        if(cd[0] == '\0') sf = 1;
        if(sf) strcpy(cd,"M"); /* I -> IM */
    }
    if(mv->debug == 6) {
        printf("Special choed skey = %s wkey = %s\n",skey,wkey);
        printf("        cp->key = %d cf = %d ck = %s cd = %s\n",
            cp->key,cf,ck,cd);
    }
    if(cf) return(cf);
    k = 0;
    while(1) {
        if(cp->key == 1) strcpy(wline,jrule[k]);
        if(cp->key == 2) strcpy(wline,irule[k]);
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        sf = 0;
        if(wline[0] == '$' && wline[1] == '8' &&
             wline[2] == ':' && wline[3] == '~') sf = 1;
        if(sf) {
            /* Find */
            i = 0;
            j = 4;
            while(1) {
                if(wline[j] == ' ') break;
                wk[i] = wline[j];
                i++;
                if(i > 32) break;
                j++;
                if(j > MAXLVL) break;
            }
            wk[i] = '\0';
            /* printf("wk = %s cd = %s\n",wk,cd); */
            if(!strcmp(wk,cd)) {
                /* printf("Find wline = %s\n",wline); */
                getslsr_meb(wline,sl,sr);
                srcard_meb(ck,cd,sr,card);
                strcpy(mv->card,"!");
                strcat(mv->card,card);
            }
        }
    }
    return(0);
}

/* check !key */
int cdcheck_meb(char skey[MAXLVL],char ck[16],char cd[16])
//char skey[MAXLVL];
//char ck[16];
//char cd[16];
{
    int  i,j,k;
    int  af;
    char tk[MAXLVL];

    strcpy(tk,skey);
    i = 0;
    af = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == ':') return(1); 
        i++;
        if(i > MAXLVL) break;
    }
    i = 0;
    if(tk[0] == '&' || tk[0] == '#') {
        ck[0] = tk[0];
        i = 1;
    }
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] != 'I' && tk[i] != 'V') break; 
        ck[i] = tk[i];
        i++;
        if(i > 16) break;
    }
    ck[i] = '\0';
    if(mv->debug == 6) printf("cdcheck_() tk = %s i = %d ck = %s\n",tk,i,ck);
    j = i;
    k = 0;
    while(1) {
        if(tk[i] == '\0') break;
        cd[k] = tk[i];
        i++;
        if(i > MAXLVL) break;
        k++;
        if(k > 16) return(-1);
    }
    cd[k] = '\0';
    tk[j] = '\0';
    return(0);
}

int getslsr_meb(char line[82],char sl[MAXLVL],char sr[MAXLVR])
//char line[82];
//char sl[MAXLVL];
//char sr[MAXLVR];
{
    int  i,j;
    int  af;

    i = 3;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        sl[j] = line[i];
        i++;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    sl[j] = '\0';
    i = 3;
    af = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '=') {
            af = 1;
            break;
        }
        i++;
        if(i > 80) break;
    }
    if(!af) return(1);
    i++;
    while(line[i] == ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        i++;
        if(i > 80) break;
    }
    j = 0;
    af = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '\"' || line[i] == '\'')  af++;
        if(af != 1) if(line[i] == ' ') break;
        if(line[i] != '\'' && line[i] != '"') sr[j++] = line[i];
        i++;
        if(i > 80) break;
        if(j >= MAXLVR) break;
    }
    sr[j] = '\0';
    /* printf("sl = %s sr = %s\n",sl,sr); */
    return(0);
}

int srcard_meb(char ck[16],char cd[16],char sr[MAXLVR],char card[82])
//char ck[16];
//char cd[16];
//char sr[MAXLVR];
//char card[82];
{
    int  i,j,k,n;
    int  pf;
    int  af;
    int  len;
    char wk[82];
    char line[82];
    char rest[82];
    char sk[MAXLVR];
    char tk[MAXLVR];
    int  chdf;
    int  oaf; /* on chord rtn */
    char onc[MAXLVR];

    if(mv->debug == 6) {
        printf("srcard_() ck = %s cd = %s\n",ck,cd);
        printf("          sr = %s\n",sr);
    }
    strcpy(line,sr);
    beauspace_meb(line);
    i = 0;
    pf = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '$') {
            pf++;
        }
        i++;
        if(i > 80) break;
    }
    if(pf <= 0) return(1);
    af = 1;
    if(cp->bf) {
        strcpy(tk,"$1:");
        strcat(tk,cp->bg);
        af = cpget_cps(tk,onc);
        if(mv->debug == 6) printf("         af = %d cp->bg = %s onc = %s\n",
            af,cp->bg,onc);
        if(af) return(af);
        strcpy(cp->bc,onc);
    }
    oaf = 0;
    if(!af) oaf = 1;
    /* Arg */
    i = 0;
    while(1) {
        wk[i] = line[i];
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        i++;
        if(i > 80) break;
    }
    wk[i] = '\0';
    tkmod_meb(ck,wk);
    if(strcmp(cd,"M")) strcat(wk,cd); /* ignore M */
    /* printf("wk = %s\n",wk); */
    if(oaf) {
        strcat(wk,"/");
        strcat(wk,cp->bg);
    }
    i = 0;
    for(k = 0; k < pf; k++) {
        af = 0;
        while(1) {
            if(line[i] == '\0') break;
            if(line[i] == '\r') break;
            if(line[i] == '\n') break;
            if(line[i] == '$') {
                af = 1;
                break;
            }
            i++;
            if(i > 80) break;
        }
        if(!af) break;
        j = 0;
        while(1) {
            if(line[i] == '\0') break;
            if(line[i] == '\r') break;
            if(line[i] == '\n') break;
            if(line[i] == ' ') break;
            if(line[i] == '-') break;
            if(line[i] == '_') break;
            if(line[i] == '.') break;
            if(line[i] == ',') break;
            if(line[i] == '[') break;
            if(line[i] == ']') break;
            tk[j] = line[i];
            i++;
            if(i > 80) break;
            j++;
            if(j >= MAXLVR) break;
        }
        tk[j] = '\0';
        /* printf("tk = %s\n",tk); */
        if(tk[0] == '$' && tk[1] == ':') {
            strcpy(tk,"$1:");
            strcat(tk,ck);
            af = cpget_cps(tk,sk);
            strcat(wk," ");
            strcat(wk,sk);
            if(strcmp(cd,"M")) strcat(wk,cd); /* ignore M */
            /* printf("wk = %s\n",wk); */
        }
        if(tk[0] == '$' && tk[1] == '4') {
            tkmod_meb(ck,tk);
            af = cpget_cps(tk,sk);
            strcat(wk," ,");
            strcat(wk,sk);
            /* printf("wk = %s\n",wk); */
        }
        if(tk[0] == '$' && tk[1] == '3') {
            tkmod_meb(ck,tk);
            af = cpget_cps(tk,sk);
            strcat(wk," ");
            strcat(wk,sk);
            /* printf("wk = %s\n",wk); */
        }
        if(tk[0] == '$' && tk[1] == '9') {
            tkmod_meb(ck,tk);
            af = cpfind_cps(tk,sk);
            if(!af) {
                strcat(wk," ");
                strcat(wk,sk);
                /* printf("wk = %s\n",wk); */
            }
        }
    }
    strcpy(card,wk);
    return(0);
}

int tkmod_meb(char ck[16],char tk[MAXLVR])
//char ck[16];
//char tk[MAXLVR];
{
    char wk[MAXLVR];
    char wk2[MAXLVR];
    int  i,j;
    int  pf;

    if(mv->debug == 6) printf("tkmod_() ck = %s tk = %s\n",ck,tk);
    i = 0;
    pf = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == '~') {
            pf = 1;
            break;
        }
        wk[i] = tk[i];
        i++;
        if(i > MAXLVR) break;
    }
    if(!pf) return(1);
    wk[i] = '\0';
    strcat(wk,ck);
    j = 0;
    i++;
    while(1) {
        if(tk[i] == '\0') break;
        wk2[j] = tk[i];
        i++;
        if(i > MAXLVR) break;
        j++;
    }
    wk2[j] = '\0';
    strcat(wk,wk2);
    /* printf("tkmod_() wk = %s wk2 = %s\n",wk,wk2); */
    strcpy(tk,wk);
    return(0);
}

int cpfind_cps(char tk[MAXLVR],char rtey[MAXLVR])
//char tk[MAXLVR];
//char rtey[MAXLVR];
{
    int  i,j,k;
    int  sf;
    char wk[32];
    char wline[82];
    char sl[MAXLVL];
    char sr[MAXLVR];

    k = 0;
    if(mv->debug == 6) printf("cpfind_() tk = %s\n",tk);
    while(1) {
        if(cp->key == 1) strcpy(wline,jrule[k]);
        if(cp->key == 2) strcpy(wline,irule[k]);
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        sf = 0;
        if(wline[0] == '$' && wline[1] == '9' &&
             wline[2] == ':') sf = 1;
        if(sf) { 
            /* Find */
            i = 0;
            while(1) {
                if(wline[i] == ' ') break;
                wk[i] = wline[i]; 
                i++;
                if(i > 32) break;
            }
            wk[i] = '\0';
            /* printf("wk = %s tk = %s\n",wk,tk); */
            if(!strcmp(wk,tk)) {
                /* printf("Find wline = %s\n",wline); */
                getslsr_meb(wline,sl,rtey);
                /* printf("wline = %s rtey = %s\n",wline,rtey); */
                return(0);  /* find */
            }
        }
    }
    return(1);
}

int Poly_key(char skey[MAXLVL],char wkey[MAXLVL],char bc[8],char bk[8])
//char skey[MAXLVL];
//char wkey[MAXLVL];
//char bc[8];
//char bk[8];
{
    char uc[8];
    char uk[8];
    char sk[MAXLVR];
    char tk[MAXLVR];
    int  rtn;

    if(mv->debug == 6)
        printf("Poly_() skey = %s wkey = %s bc = %s bk = %s\n",skey,wkey,bc,bk);
    upchk_meb(wkey,uc,uk); /* Chord check */
    strcpy(tk,"$1:");
    strcat(tk,uc);
    rtn = cpget_cps(tk,sk);
    if(rtn) return(rtn);
    if(mv->debug == 6) printf("Poly_() tk = %s sk = %s\n",tk,sk);
    strcpy(mv->card,"!");
    strcat(mv->card,skey);
    strcat(mv->card," ");
    strcat(mv->card,sk);
    strcat(mv->card,uk);
    strcat(mv->card,"/");
    strcpy(tk,"$1:");
    strcat(tk,bc);
    rtn = cpget_cps(tk,sk);
    if(rtn) return(rtn);
    strcat(mv->card,sk);
    strcat(mv->card,bk);
    strcat(mv->card," ,Poly chord");
    strcpy(tk,"$9:");
    strcat(tk,skey);
    rtn = cpfind_cps(tk,sk);
    if(rtn) return(rtn);
    if(mv->debug == 6) printf("        tk = %s sk = %s\n",tk,sk);
    /* strcat(mv->card," "); */
    strcat(mv->card,sk);
    return(0);
}

/* Upper Chord check */
int upchk_meb(char wk[MAXLVL],char bc[16],char bk[16])
//char wk[MAXLVL];
//char bc[16];    
//char bk[16]; 
{       
    int  i,j;
    
    i = 0;  
    i = 0;
    if(wk[0] == '&' || wk[0] == '#') { 
        bk[0] = wk[0];
        i = 1;
    }   
    while(1) {
        if(wk[i] == '\0') break;
        if(wk[i] != 'I' && wk[i] != 'V') break;
        bc[i] = wk[i];
        i++;
        if(i > 16) break;
    }       
    bc[i] = '\0';
    j = 0;
    while(1) {
        if(wk[i] == '\0') break;
        bk[j] = wk[i];
        i++;
        if(i > 16) break;
        j++;
    }
    bk[j] = '\0';
    if(mv->debug == 6) printf("upchk_() wk = %s bc = %s bk = %s\n",wk,bc,bk);
    return(0);
}

/* Set Poly Chord observe note line */
int Polyobs_meb()
{
    int  i,j;
    int  cf;
    int  af;
    int  pf;
    int  rt;
    int  rtn;
    char tk[MAXLVR];
    char sk[MAXLVR];
    char temp[82];
    char work[82];
    char work2[82];
    char work3[82];
    char Cn[8];  /* Chord name */

    if(mv->debug == 6) printf("Polyobs_() %s\n",mv->card);
    strcpy(temp,mv->card); /* save */
    i = 0;
    while(1) {
        if(mv->card[i] == '\0') break;
        if(mv->card[i] == '/') break;
        i++;
        if(i > 82) break;
    }
    i++;
    j = 0;
    while(1) {
        if(mv->card[i] == '\0') break;
        work[j] = mv->card[i];
        i++;
        if(i > 82) break;
        j++;
    }
    work[j] = '\0';
    /* printf("work = %s\n",work); */
    i = 0;
    af = 0;
    while(1) {
        if(work[i] == '\0') break;
        if(work[i] == '/') {
            af = 1;
            break;
        }
        i++;
        if(i > 82) break;
    }
    if(!af) return(-1);
    i = 0;
    while(1) {
        if(work[i] == '\0') break;
        if(work[i] == ' ') break;
        work2[i] = work[i];
        i++;
        if(i > 82) break;
    }
    work2[i] = '\0';
    /* printf("work2 = %s\n",work2); */
    while(1) {
        if(work[i] == '\0') break;
        if(work[i] == '/') break;
        i++;
        if(i > 82) break;
    }
    j = 0;
    i++;
    while(1) {
        if(work[i] == '\0') break;
        work3[j] = work[i];
        i++;
        if(i > 82) break;
        j++;
    }
    work3[j] = '\0';
    /* printf("work3 = %s\n",work3); */
    strcpy(mv->card,"{!");
    strcat(mv->card,work2);
    strcat(mv->card," ");
    strcat(mv->card,work3);
    /* printf("mod %s\n",mv->card); */
    /* Base chord */
    cp->bf = 0;
    cp->caf = 0;
    cf = getchord_meb(1,Cn); /* cf : C D E F G A B */
    if(mv->debug == 6) printf("chdobsev_meb() Cn = %s cf = %d\n",Cn,cf);
    if(cf == -2) return(1); /* error chord */
    strcpy(tk,"$5:");
    strcat(tk,Cn);
    pf = cpget_cps(tk,sk);
    if(mv->debug == 6) printf("pf = %d tk = %s sk = %s\n",pf,tk,sk);
    if(pf) return(2); /* no chord */
    rtn = getRoot_meb(1,&rt); /* rt : Root Column position */
    if(rtn == 3 || rtn == 4 || rtn == 7) modRoot_meb(&rt);
    if(mv->debug == 6) printf("rtn = %d rt = %d\n",rtn,rt);
    chkival_meb(rtn,rt,sk);
    if(mv->debug == 6) {
        printf("Base chord\n");
        printf("%s\n",cp->cnl);
    }
    strcpy(work,cp->cnl);  /* base */
    strcpy(mv->card,temp);  /* back */
    /* Upper chord */
    cp->bf = 0;
    cp->caf = 0;
    cf = getchord_meb(1,Cn); /* cf : C D E F G A B */ 
    if(mv->debug == 6) printf("chdobsev_meb() Cn = %s cf = %d\n",Cn,cf);
    if(cf == -2) return(1); /* error chord */
    strcpy(tk,"$5:");
    strcat(tk,Cn);
    pf = cpget_cps(tk,sk);
    if(mv->debug == 6) printf("pf = %d tk = %s sk = %s\n",pf,tk,sk);
    if(pf) return(2); /* no chord */
    rtn = getRoot_meb(1,&rt); /* rt : Root Column position */
    if(rtn == 3 || rtn == 4 || rtn == 7) modRoot_meb(&rt);
    if(mv->debug == 6) printf("rtn = %d rt = %d\n",rtn,rt);
    chkival_meb(rtn,rt,sk);
    if(mv->debug == 6) {
        printf("Upper chord\n");
        printf("%s\n",cp->cnl);
    }
    strcpy(work2,cp->cnl);  /* upper */
    work2[0] = ' ';
    work2[1] = ' ';
    work2[2] = ' ';
    for(i = 3; i < 68; i++) cp->cnl[i] = ' ';
    for(i = 3; i < 61; i++) { /* base -> cp->cnl */
        cp->cnl[i] = work[i+7];
    }
    for(i = 0; i < 74; i++) {
        if(cp->cnl[i+7] == ' ') cp->cnl[i+7] = work2[i];
    }
    /* printf("%s\n",cp->cnl); */
    cp->caf = 1;
    cp->bf = 1;
    return(0);
}

/* Vertical Exchange */
int verexp_meb()
{
    int  i,j,k;
    int  bf;
    int  af;
    int  mf;
    int  rtn;
    int  len;
    char aw[MAXVCH];
    char bw[MAXVCH];
    char c1;
    char card[160];
    char wk[8];

    /* printf("%s\n",mv->card); */
    c1 = mv->card[1];
    i = 0;
    j = 0;
    while(1) {
        card[j] = mv->card[i];
        if(mv->card[i] == '\0') break;
        if(mv->card[i] == ',') card[j] = ' ';
        if(c1 == 'E') {
            if(mv->card[i] == '!') {
                if(mv->card[i+1] == '!' || mv->card[i+1] == '~')
                card[++j] = ' '; 
            }
            if(mv->card[i] == '~') {
                if(mv->card[i+1] == '!' || mv->card[i+1] == '~')
                card[++j] = ' '; 
            }
        }
        else {
            if(mv->card[i] == '!') card[++j] = ' '; 
            if(mv->card[i] == '~') card[++j] = ' '; 
        }
        if(isdigit(mv->card[i])) {
            if(mv->card[i-1] == ')') {
                card[j] = ' '; 
                card[++j] = '*'; 
                card[++j] = mv->card[i]; 
            }
            /*
            if(mv->card[i-1] == ' ') {
                card[j] = '*'; 
                card[++j] = mv->card[i]; 
            }
            */
            if(mv->card[i+1] == '!') {
                card[++j] = ' '; 
            }
        }
        if(mv->card[i] == '(') {
            card[j] = ' '; 
            card[++j] = '('; 
            card[++j] = ' '; 
        }
        if(mv->card[i] == ')') {
            card[j] = ' '; 
            card[++j] = ')'; 
            card[++j] = ' '; 
        }
        if(mv->card[i] == '*') {
            card[j] = ' '; 
            card[++j] = '*'; 
        }
        i++;
        if(i > 80) break;
        j++;
        if(j > 160) break;
    }
    vexpspace_meb(card);
    if(mv->debug == 11) printf("%s\n",card);
    i = 0;
    bf = 0;
    af = 0;
    while(1) {
        if(mv->card[i] == '\0') break;
        if(mv->card[i] == '(') bf++;
        if(mv->card[i] == ')') af++;
        i++;
        if(i > 80) break;
    }
    /* printf("bf = %d af = %d\n",bf,af); */
    if(bf != af) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! %06d %s\n",mv->line,mv->card);
        return(0);
    }
    strcpy(aw,card);
    for( i = 0; i < af; i++) {
        bw[0] = '\0';
        rtn = lexpand_meb(aw,bw);
        if(rtn) af++;
        for( j = 0; j < MAXVCH; j++) {
            aw[j] = bw[j];
            if(bw[j] == '\0') break;
        }
    }
    while(1) {
        mf = 0;
        i = 3;
        while(1) {  /* Final check */
            if(aw[i] == '\0') break;
            if(aw[i] == ')') aw[i] = ' ';
            if(aw[i] == '*' && isdigit(aw[i+1])) mf++;
            i++;
            if(i > MAXVCH) break;
        }
        vspace_meb(aw);
        if(mf) {
            bw[0] = '\0';
            /* printf("aw = %s\n",aw); */
            rtn = lexpand_meb(aw,bw);
            if(mv->debug == 11) vprint_meb("berexp_() bw",bw);
            for( j = 0; j < MAXVCH; j++) {
                aw[j] = bw[j];
                if(bw[j] == '\0') break;
            }
        }
        else break;
    }
    if(mv->debug == 11) vprint_meb("verexp_() aw",aw);
    switch(mv->card[1]) {
        case 'T' : tmpovex_meb(aw);
                   tmc = 0;
                   vx->tulf = 1;
                   break;
        case 'E' : dynavex_meb(aw);
                   dxc = 0;
                   vx->dulf = 1;
                   break;
        case 'G' : glvdvex_meb(aw);
                   gvc = 0;
                   vx->gulf = 1;
                   break;
        case 'V' : velovex_meb(aw);
                   vlc = 0;
                   vx->vulf = 1;
                   break;
        case 'P' : ptchvex_meb(aw);
                   pvc = 0;
                   vx->pulf = 1;
                   break;
        case 'M' : moduvex_meb(aw);
                   mdc = 0;
                   vx->mulf = 1;
                   break;
        case 't' : tmpovex_meb(aw);
                   tmc = 0;
                   vx->tulf = 0;
                   break;
        case 'e' : dynavex_meb(aw);
                   dxc = 0;
                   vx->dulf = 0;
                   break;
        case 'g' : glvdvex_meb(aw);
                   gvc = 0;
                   vx->gulf = 0;
                   break;
        case 'v' : velovex_meb(aw);
                   vlc = 0;
                   vx->vulf = 0;
                   break;
        case 'p' : ptchvex_meb(aw);
                   pvc = 0;
                   vx->pulf = 0;
                   break;
        case 'm' : moduvex_meb(aw);
                   mdc = 0;
                   vx->mulf = 0;
                   break;
        default  : break;
    }
    return(0);
}

int tmpovex_meb(char aw[MAXVCH])
//char aw[MAXVCH];   
{       
    int  i,j,k; 
    char wk[8]; 
    int  wf;
        
    if(mv->debug == 11) vprint_meb("tmpovex() aw",aw);
    j = 0;
    k = 0;
    i = 3;  
    wf = 0;
    while(1) {
        if(aw[i] == '\0') break;
        if(aw[i] == ' ') {
            vx->vtm[k][j] = '\0';
            /* printf("%s ",vx->vtm[k]); */
            if(j > 0) k++;
            j = -1;
        }
        else vx->vtm[k][j] = aw[i];
        i++;
        if(i > MAXVCH) break;
        j++;
        if(j > 5) {
            if(!wf) {
                vx->vtm[k][j] = '\0';
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! *Velocity data %s\n",vx->vtm[k]);
                }
            }
            wf++;
            vx->vtm[k][0] = '~';
            vx->vtm[k][1] = '\0';
            k++;
            j = 0;
        }
    }  
    vx->tmc = k;
    if(mv->debug == 11) {
        printf("vx->tmc = %d\n",vx->tmc);
        printf("vx->vtm[] table\n");
        for(i = 0; i < vx->tmc; i++) printf("%s ",vx->vtm[i]);
        printf("\n");
    }
    return(0);
}

int dynavex_meb(char aw[MAXVCH])
//char aw[MAXVCH];
{
    int  i,j,k;
    char c0;
    char wk[8];
    int  wf;
    int  wf2;

    if(mv->debug == 11) vprint_meb("dynavex() aw",aw);
    j = 0;
    k = 0;
    i = 3;  
    wf = 0;
    wf2 = 0;
    while(1) {
        if(aw[i] == '\0') {
            if(j > 3) k++;
            break;
        }
        if(aw[i] == ' ') {
            vx->vdx[k][j] = '\0'; 
            c0 = vx->vdx[k][0];
            if(c0 != '+' && c0 != '-' && c0 != '!' && c0 != '_' &&
                c0 != '~') {
                if(!wf2) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl) {
                        printf("Error! *Dynamic_exchange data %s\n",vx->vdx[k]);
                    }
                }
                wf2++;
                vx->vdx[k][0] = '~';
                vx->vdx[k][1] = '\0';
            }
            /* printf("%s ",vx->vdx[k]); */
            if(j > 0) k++;
            j = -1;
        }
        else vx->vdx[k][j] = aw[i];
        i++;
        if(i > MAXVCH) break;
        j++;
        if(j > 4) {
            if(!wf) {
                vx->vdx[k][j] = '\0';
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! *Dynamic_exchange data %s\n",vx->vdx[k]);
                }
            }
            wf++;
            vx->vdx[k][0] = '~';
            vx->vdx[k][1] = '\0';
            k++;
            j = 0;
        }
    }   
    vx->dxc = k;
    if(mv->debug == 11) {
        printf("vx->dxc = %d\n",vx->dxc);
        printf("vx->vdx[] table\n");
        for(i = 0; i < vx->dxc; i++) printf("%s ",vx->vdx[i]);
        printf("\n");
    }
    return(0);
}

int glvdvex_meb(char aw[MAXVCH])
//char aw[MAXVCH];
{
    int  i,j,k;
    char wk[8];
    int  wf;

    if(mv->debug == 11) vprint_meb("glvdvex() aw",aw);
    j = 0;
    k = 0;
    i = 3;
    wf = 0;
    while(1) {
        if(aw[i] == '\0') break;
        if(aw[i] == ' ') {
            vx->vgv[k][j] = '\0';
            /* printf("%s ",vx->vgv[k]); */
            if(j > 0) k++;
            j = -1;
        }
        else vx->vgv[k][j] = aw[i];
        i++;
        if(i > MAXVCH) break;
        j++;
        if(j > 5) {
            if(!wf) {
                vx->vgv[k][j] = '\0';
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! *Gloval velocity data %s\n",vx->vgv[k]);
                }
            }
            wf++;
            vx->vgv[k][0] = '~';
            vx->vgv[k][1] = '\0';
            k++;
            j = 0;
        }
    }   
    vx->gvc = k;
    if(mv->debug == 11) {
        printf("vx->gvc = %d\n",vx->gvc);
        printf("vx->vgv[] table\n");
        for(i = 0; i < vx->gvc; i++) printf("%s ",vx->vgv[i]);
        printf("\n");
    }
    return(0);
}

int velovex_meb(char aw[MAXVCH])
//char aw[MAXVCH];
{
    int  i,j,k;
    char wk[8];
    int  wf;

    if(mv->debug == 11) vprint_meb("velovex() aw",aw);
    j = 0;
    k = 0;
    i = 3;  
    wf = 0;
    while(1) {
        if(aw[i] == '\0') break;
        if(aw[i] == ' ') {
            vx->vvl[k][j] = '\0';
            // printf("// %s ",vx->vvl[k]);
            if(j > 0) k++;
            j = -1;
        }
        else vx->vvl[k][j] = aw[i];
        i++;
        if(i > MAXVCH) break;
        j++;
        if(j > 4) {
            if(!wf) {
                vx->vvl[k][j] = '\0';
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! *Velocity data %s\n",vx->vvl[k]);
                }
            }
            wf++;
            vx->vvl[k][0] = '~';
            vx->vvl[k][1] = '\0';
            k++;
            j = 0;
        }
    }   
    vx->vlc = k;
    if(mv->debug == 11) {
        printf("vx->vlc = %d\n",vx->vlc);
        printf("vx->vvl[] table\n");
        for(i = 0; i < vx->vlc; i++) printf("%s ",vx->vvl[i]);
        printf("\n");
    }
    return(0);
}

int ptchvex_meb(char aw[MAXVCH])
//char aw[MAXVCH];
{
    int  i,j,k;
    char wk[8];
    int  wf;

    if(mv->debug == 11) vprint_meb("ptchvex() aw",aw);
    j = 0;
    k = 0;
    i = 3;  
    wf = 0;
    while(1) {
        if(aw[i] == '\0') break;
        if(aw[i] == ' ') {
            vx->vpv[k][j] = '\0';
            /* printf("%s ",vx->vpv[k]); */
            if(j > 0) k++;
            j = -1;
        }
        else vx->vpv[k][j] = aw[i];
        i++;
        if(i > MAXVCH) break;
        j++;
        if(j > 5) {
            if(!wf) {
                vx->vpv[k][j] = '\0';
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! *Pitchvend data %s\n",vx->vpv[k]);
                }
            }
            wf++;
            vx->vpv[k][0] = '~';
            vx->vpv[k][1] = '\0';
            k++;
            j = 0;
        }
    }   
    vx->pvc = k;
    if(mv->debug == 11) {
        printf("vx->pvc = %d\n",vx->pvc);
        printf("vx->vpv[] table\n");
        for(i = 0; i < vx->pvc; i++) printf("%s ",vx->vpv[i]);
        printf("\n");
    }
    return(0);
}

int moduvex_meb(char aw[MAXVCH])
//char aw[MAXVCH];
{
    int  i,j,k;
    char wk[8];
    int  wf;

    if(mv->debug == 11) vprint_meb("moduvex() aw",aw);
    j = 0;
    k = 0;
    i = 3;  
    wf = 0;
    while(1) {
        if(aw[i] == '\0') break;
        if(aw[i] == ' ') {
            vx->vvl[k][j] = '\0';
            /* printf("%s ",vx->vmd[k]); */
            if(j > 0) k++;
            j = -1;
        }
        else vx->vmd[k][j] = aw[i];
        i++;
        if(i > MAXVCH) break;
        j++;
        if(j > 3) {
            if(!wf) {
                vx->vmd[k][j] = '\0';
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! *Modulation data %s\n",vx->vmd[k]);
                }
            }
            wf++;
            vx->vmd[k][0] = '~';
            vx->vmd[k][1] = '\0';
            k++;
            j = 0;
        }
    }   
    vx->mdc = k;
    if(mv->debug == 11) {
        printf("vx->mdc = %d\n",vx->mdc);
        printf("vx->vmd[] table\n");
        for(i = 0; i < vx->mdc; i++) printf("%s ",vx->vmd[i]);
        printf("\n");
    }
    return(0);
}

int lexpand_meb(char aw[MAXVCH],char bw[MAXVCH])
//char aw[MAXVCH];
//char bw[MAXVCH];
{
    int  i,j,k;
    int  ii;
    int  jj;
    int  af;
    int  mf; /* * flag */
    int  sp;
    int  ep;
    int  mpd;
    int  pass;
    char wk[MAXVCH];
    char tw[8];
    char tm[8];

    i = 0;
    af = 0;
    sp = 0;
    ep = 0;
    while(1) {
        bw[i] = aw[i];
        if(aw[i] == '(') sp = i; 
        if(aw[i] == ')') {
            af = 1;
            break;
        }
        if(aw[i] == '\0') break;
        i++;
        if(i > MAXVCH) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
                printf("Error! %06d lexpand_() MAXVCH over %d\n",
                mv->line,MAXVCH);
            return(0);
        }
        ep++;
    }
    if(mv->debug == 11)
         printf("Debug11! lexpand_() sp = %d bw[sp] = %c ep = %d bw[ep] = %c\n",
         sp,bw[sp],ep,bw[ep]);
    mf = 0;
    for( i = sp; i < ep; i++) if(aw[i] == '*') mf++; 
    pass = 0;
    if(mf) {
        /* Step add */
        if(sp == 0) {  /* No ( ) */
            i = 0;
            while(1) {
                if(aw[i] == '*' && isdigit(aw[i+1])) break;
                bw[i] = aw[i];
                if(aw[i] == '\0') break;
                i++;
                if(i > MAXVCH) break;
            }
            bw[i] = '\0';
            if(mv->debug == 11) vprint_meb("Step add aw",aw);
            if(mv->debug == 11) vprint_meb("Step add bw",bw);
            i = 0;
            j = 0;
            k = 0;
            while(1) {
                if(aw[i] == '*' && isdigit(aw[i+1])) {
                    ii = 0;
                    i++;
                    while(aw[i] != ' ') {
                        if(aw[i] == ')') break;
                        if(aw[i] == '\0') break;
                        tm[ii] = aw[i];
                        i++;
                        ii++;
                        if(ii > 8) {
                            mv->errcnt++;
                            mv->msglvl = 3;
                            if(mv->msglvl >= mv->prtlvl)
                                printf("Error! %06d %s\n",mv->line,mv->card);
                            return(0);
                        }
                    }
                    tm[ii] = '\0';
                    sscanf(tm,"%d",&mpd);
                    if(mv->debug == 11)
                       printf("Debug11! lexpand_(a) tw = %s tm = %s mpd = %d\n",
                       tw,tm,mpd);
                    wk[0] = '\0';
                    for( jj = 0; jj < mpd - 1; jj++) {
                        strcat(wk," ");
                        if(tw[0] == '!' || tw[0] == '~') {
                            if(tw[0] == '!') strcat(wk,"!");
                            if(tw[0] == '~') strcat(wk,"~");
                        }
                        else strcat(wk,tw);
                    }
                    /* printf("wk = %s\n",wk); */
                    strcat(wk," ");
                    j = strlen(wk);
                    if(j > MAXVCH) break;
                    if(mv->debug == 11) printf("wk = %s j = %d\n",wk,j);
                    break;
                }
                wk[j] = aw[i];
                tw[k] = aw[i];
                if(tw[k] == ' ') {
                    tw[k] = '\0';
                    k = -1;
                }
                k++;
                if(k > 8) k = 0;
                if(aw[i] == '\0') break;
                i++;
                if(i > MAXVCH) break;
                j++;
                if(j > MAXVCH) break;
            }
            if(mv->debug == 11) printf("wk = %s\n",wk);
            strcat(bw,wk);
            j = 0;
            while(1) {
                wk[j] = aw[i];
                if(aw[i] == '\0') break;
                i++;
                if(i > MAXVCH) break;
                j++;
            }
            strcat(bw,wk);
            if(mv->debug == 11) printf("wk = %s\n",wk);
            vspace_meb(bw);
            /*
            printf("bw = %s\n",bw);
            */
            return(0);
        }
        k = 0;
        /* Step 1 */
        for( i = sp,j = 0; i < ep; i++,j++) {
            if(aw[i] == '*' && isdigit(aw[i+1])) {
                ii = 0;
                i++;
                while(aw[i] != ' ') {
                    if(aw[i] == ')') break;
                    tm[ii] = aw[i];
                    i++;
                    ii++;
                    if(ii > 8) {
                        mv->errcnt++;
                        mv->msglvl = 3;
                        if(mv->msglvl >= mv->prtlvl)
                            printf("Error! %06d %s\n",mv->line,mv->card);
                        return(0);
                    }
                } 
                tm[ii] = '\0';
                sscanf(tm,"%d",&mpd);
                if(mv->debug == 11)
                     printf("Debug11! lexpand_(0) tw = %s tm = %s mpd = %d\n",
                     tw,tm,mpd);
                for( jj = 0; jj < mpd - 1; jj++) {
                    strcat(wk," ");
                    if(tw[0] == '!' || tw[0] == '~') {
                        if(tw[0] == '!') strcat(wk,"!");
                        if(tw[0] == '~') strcat(wk,"~");
                    }
                    else strcat(wk,tw);
                }
                if(mv->debug == 11) printf("wk = %s\n",wk);
                strcat(wk," ");
                j = strlen(wk);
                if(j > MAXVCH) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                        printf("Error! %06d %s\n",mv->line,mv->card);
                    return(0);
                }
                if(mv->debug == 11) printf("wk = %s j = %d\n",wk,j);
                pass = 1;
            }
            wk[j] = aw[i]; 
            wk[j+1] = '\0'; 
            tw[k] = aw[i];
            if(aw[i] == ' ') {
                tw[k] = '\0';
                k = -1;
            }
            k++;
            if(k > 8) k = 0;
        } 
        if(mv->debug == 11) vprint_meb("Step 1 wk",wk);
    }
    /* Step 2 */
    if(pass) {
        for(i = 0; i < sp; i++) bw[i] = aw[i];
        j = 0;
        while(1) {
            if(wk[j] == '\0') break;
            bw[i] = wk[j];
            j++;
            if(j > MAXVCH) break;
            i++;
            if(i > MAXVCH) break;
        }
        k = ep;
        while(1) {
            bw[i] = aw[k];
            if(aw[k] == '\0') break;
            i++;
            if(i > MAXVCH) break; 
            k++;
            if(i > MAXVCH) break;
        }
        vspace_meb(bw);
        if(mv->debug == 11) vprint_meb("Step 2 bw",bw);
        return(1); /* *xx ok */
    }
    for( i = sp,j = 0; i < ep; i++,j++) {
        wk[j] = aw[i];
        if(wk[j] == '(') wk[j] = ' ';
        if(wk[j] == ')') wk[j] = ' ';
    }
    wk[j] = '\0';
    mpd = 1;
    if(aw[ep] == ')' && aw[ep+1] == ' ' && aw[ep+2] == '*') {
        k = ep + 3;
        if(isdigit(aw[k])) {
            j = 0;
            while(isdigit(aw[k])) {
                if(aw[k] == '\0') break;
                tm[j] = aw[k];
                j++;
                if(j > 8) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                        printf("Error! %06d %s\n",mv->line,mv->card);
                    return(0);
                }
                k++;
            }
        }
        tm[j] = '\0';
        sscanf(tm,"%d",&mpd);
        ep = k;
        if(mv->debug == 11)
             printf("Debug11! lexpand_(5) tm = %s mpd = %d ep = %d\n",
             tm,mpd,ep);
    }
    /* Step 3 */
    vspace_meb(wk);
    if(mv->debug == 11) {
        printf("Step 3 mpd = %d wk\n%s\n",mpd,wk);
    }
    for(i = 0; i < sp; i++) bw[i] = aw[i];
    for(k = 0; k < mpd; k++) {
        j = 0;
        while(1) {
            if(wk[j] == '\0') break;
            bw[i] = wk[j];
            j++;
            if(j > MAXVCH) break;
            i++;
            if(i > MAXVCH) break; 
        }
        bw[i] = '\0';
        if(mv->debug == 11) vprint_meb("Step 3 bw",bw);
    }
    /* Step 4 */
    k = ep;
    if(aw[k] == ')') aw[k] = ' ';
    while(1) {
        bw[i] = aw[k];
        if(aw[k] == '\0') break;
        i++;
        if(i > MAXVCH) break; 
        k++;
        if(i > MAXVCH) break; 
    }
    vspace_meb(bw);
    if(mv->debug == 11) vprint_meb("Step 4 bw",bw);
    return(0);
}

int vspace_meb(char line[MAXVCH])
//char line[MAXVCH];
{
    int  i,k;
    int  wfg;
    char wk[MAXVCH];
  
    wfg = 0;
    k = 0;
    for(i = 0; i < MAXVCH; i++) {
        if(line[i] == '\0') break;
        if(line[i] != ' ') {
            wk[k++] = line[i];
            wfg = 1;
        }
        else {
            if(wfg) wk[k++] = ' ';
            wfg = 0;
        }
    }
    wk[k] = '\0';
    strcpy(line,wk);
    return(0);
}

int vprint_meb(char *aw,char wk[MAXVCH])
//char *aw;
//char wk[MAXVCH];
{
    int  i,j;

    printf("%s\n",aw);
    i = 0;
    j = 0;
    while(1) {
        if(wk[i] == '\0') break;
        putchar(wk[i]);
        i++;
        if(i > MAXVCH) break;
        j++;
        if(j == 79) {
            printf("\n");
            j = 0;
        }
        if(j > 72 && wk[i] == ' ') {
            printf("\n");
            j = 0;
        }
    }
    printf("\n");
    return(0);
}

/* Vertical modify */
int vermod_meb()
{
    switch(mb->bartyp) {
        case  'l' : vlcmod_meb();
                    dxcmod_meb();
                    break;
        case  'm' : vlcmod_meb();
                    dxcmod_meb();
                    break;
        case  'h' : vlcmod_meb();
                    dxcmod_meb();
                    break;
        case  't' : vlcmod_meb();
                    dxcmod_meb();
                    break;
        case  's' : vlcmod_meb();
                    dxcmod_meb();
                    pvcmod_meb();
                    mdcmod_meb();
                    break;
        case  'g' : vlcmod_meb();
                    dxcmod_meb();
                    pvcmod_meb();
                    mdcmod_meb();
                    break;
        case  'a' : vlcmod_meb();
                    dxcmod_meb();
                    pvcmod_meb();
                    mdcmod_meb();
                    break;
        case  'b' : vlcmod_meb();
                    dxcmod_meb();
                    pvcmod_meb();
                    mdcmod_meb();
                    break;
        case  'u' : vlcmod_meb();
                    dxcmod_meb();
                    pvcmod_meb();
                    mdcmod_meb();
                    break;
        case  'r' : vlcmod_meb(); 
                    dxcmod_meb();
                    break;
        case  'd' : vlcmod_meb(); 
                    dxcmod_meb();
                    break;
        default   : break;
    }
    return(0);
}

int vlcmod_meb()
{
    char c;
    float fv;
    char wk[8];

    /* Velocity */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->vulf == 0) return(0);
    }
    if(vx->vlc && vlc < vx->vlc) {
        c = vx->vvl[vlc][0];
        if(c == '!') {
            sprintf(wk,"%3.1f",mv->velo);
            /* printf("wk = %s velo = %f\n",wk,mv->velo); */
            if(wk[1] == '.') {
                mv->card[69] = ' '; 
                mv->card[70] = wk[0];
                mv->card[71] = wk[1];
                mv->card[72] = wk[2];
            }
            else {
                mv->card[69] = wk[0]; 
                mv->card[70] = wk[1];
                mv->card[71] = wk[2];
                mv->card[72] = wk[3];
            }
        }   
        else if(c != '~') {
            if(isdigit(vx->vvl[vlc][0])) {
                sscanf(vx->vvl[vlc],"%f",&fv);
                sprintf(wk,"%3.1f",fv);
                // printf("?? wk = %s fv = %f\n",wk,fv);
                if(wk[1] == '.') {
                    mv->card[69] = ' ';
                    mv->card[70] = wk[0];
                    mv->card[71] = wk[1];
                    mv->card[72] = wk[2];
                }
                else {
                    mv->card[69] = wk[0];
                    mv->card[70] = wk[1];
                    mv->card[71] = wk[2];
                    mv->card[72] = wk[3];
                }
            }
        }   
        /* c == ~ No change */
        vlc++;
    }   
    /* printf("%s\n",mv->card); */
    return(0);
}

int dxcmod_meb()
{
    char c;
    char wk[8];

    /* Dynamic Expand Symbol */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->dulf == 0) return(0);
    }
    if(vx->dxc && dxc < vx->dxc) {
        c = vx->vdx[dxc][0];
        if(c == '!') {
            mv->card[75] = '_';
            mv->card[76] = '.';
            mv->card[77] = '.';
            mv->card[78] = '.';
        }   
        if(c == '+' || c == '-' || c == '_') {
            mv->card[75] = vx->vdx[dxc][0];
            mv->card[76] = vx->vdx[dxc][1];
            mv->card[77] = vx->vdx[dxc][2];
            mv->card[78] = vx->vdx[dxc][3];
        }
        /* c == ~ No change */
        dxc++;
    }
    return(0);
}

int pvcmod_meb()
{
    char c;  
    int  k;
    char wk[8];

    /* Pitchvend */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->pulf == 0) return(0);
    }
    if(vx->pvc && pvc < vx->pvc) {
        c = vx->vpv[pvc][0];
        if(c == '!') {
            if(mv->card[9] == '-' || mv->card[9] == '+') mv->card[9] = ' ';
            mv->card[10] = ' ';
            mv->card[11] = ' ';
            mv->card[12] = ' ';
            mv->card[13] = '0';
        }
        else if(c != '~') {
            sscanf(vx->vpv[pvc],"%d",&k);
            sprintf(wk,"%5d",k);
            /* printf("wk = %s k = %d\n",wk,k); */
            mv->card[9] = wk[0];
            mv->card[10] = wk[1];
            mv->card[11] = wk[2];
            mv->card[12] = wk[3];
            mv->card[13] = wk[4];
        }
        /* c == ~ No change */
        pvc++;
    }
    return(0);
}

int mdcmod_meb()
{
    char c;  
    int  k;
    char wk[8];

    /* Modulation */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->mulf == 0) return(0);
    }
    if(vx->mdc && mdc < vx->mdc) {
        c = vx->vmd[mdc][0];
        if(c == '!') {
            mv->card[15] = ' ';
            mv->card[16] = ' ';
            mv->card[17] = '0';
        }
        else if(c != '~') {
            if(isdigit(vx->vmd[mdc][0])) {
                sscanf(vx->vmd[mdc],"%d",&k);
                sprintf(wk,"%3d",k);
                /* printf("wk = %s k = %d\n",wk,k); */
                mv->card[15] = wk[0];
                mv->card[16] = wk[1];
                mv->card[17] = wk[2];
            }
        }
        /* c == ~ No change */
        mdc++;
    }
    return(0);
}

/* Main expand modify */
int mexpmod_meb()
{
    char c;

    /* Main Expand symbol */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->vulf == 0) return(0);
    }       
    if(vx->dxc && dxc < vx->dxc) {
        c = vx->vdx[dxc][0];
        if(c == '!') {
            mv->card[33] = ' ';
            mv->card[34] = '.';
            mv->card[35] = '.';
            mv->card[36] = '.';
        }
        else if(c != '~') {
            mv->card[33] = vx->vdx[dxc][0];
            mv->card[34] = vx->vdx[dxc][1];
            mv->card[35] = vx->vdx[dxc][2];
            mv->card[36] = vx->vdx[dxc][3];
        }   
        /* c == ~ No change */
        dxc++;
    }      
    /* printf("%s\n",mv->card); */
    return(0);
}

/* Main Vertical modify */
int mvlcmod_meb()
{   
    char c;
    float fv;
    char wk[8];

    /* Velocity */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->vulf == 0) return(0);
    }
    if(vx->vlc && vlc < vx->vlc) {
        c = vx->vvl[vlc][0];
        if(c == '!') {
            sprintf(wk,"%3.1f",mv->velo);
            /* printf("wk = %s velo = %f\n",wk,mv->velo); */
            if(wk[1] == '.') {
                mv->card[43] = ' ';
                mv->card[44] = wk[0];
                mv->card[45] = wk[1];
                mv->card[46] = wk[2];
            }
            else {
                mv->card[43] = wk[0];
                mv->card[44] = wk[1];
                mv->card[45] = wk[2];
                mv->card[46] = wk[3];
            }
        }
        else if(c != '~') {
            sscanf(vx->vvl[vlc],"%f",&fv);
            sprintf(wk,"%3.1f",fv);
            /* printf("wk = %s fv = %f\n",wk,fv); */ 
            if(wk[1] == '.') {
                mv->card[43] = ' ';
                mv->card[44] = wk[0];
                mv->card[45] = wk[1];
                mv->card[46] = wk[2];
            }
            else {
                mv->card[43] = wk[0];
                mv->card[44] = wk[1];
                mv->card[45] = wk[2];
                mv->card[46] = wk[3];
            }
        }     
        /* c == ~ No change */
        vlc++; 
    }       
    /* printf("%s\n",mv->card); */
    return(0);
}

int tmcmod_meb()
{
    char c;
    float fv;
    char wk[8];

    /* Tempo */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->tulf == 0) return(0);
    }
    if(vx->tmc && tmc < vx->tmc) {
        c = vx->vtm[tmc][0];
        if(c == '!') {
            sprintf(wk,"%6.2f",mv->mtemp);
            /* printf("wk = %s velo = %f\n",wk,mv->mtemp); */
            mv->card[49] = wk[0];
            mv->card[50] = wk[1];
            mv->card[51] = wk[2];
            mv->card[52] = wk[3];
            mv->card[53] = wk[4];
            mv->card[54] = wk[5];
        }
        else if(c != '~') {
            if(isdigit(vx->vtm[tmc][0])) {
                sscanf(vx->vtm[tmc],"%f",&fv);
                sprintf(wk,"%6.2f",fv);
                /* printf("wk = %s fv = %f\n",wk,fv); */
                mv->card[49] = wk[0];
                mv->card[50] = wk[1];
                mv->card[51] = wk[2];
                mv->card[52] = wk[3];
                mv->card[53] = wk[4];
                mv->card[54] = wk[5];
            }
        }
        /* c == ~ No change */
        tmc++;
    }
    return(0);
}

int gvcmod_meb()
{
    char c;
    float fv;
    char wk[8];

    /* Global Velocity (dyna) */
    if(mv->card[0] == '+' || mv->card[0] == ':') { /* + : line ? */
        if(vx->gulf == 0) return(0);
    }
    if(vx->gvc && gvc < vx->gvc) {
        c = vx->vgv[gvc][0];
        if(c == '!') {
            sprintf(wk,"%5.1f",mv->mdymc);
            /* printf("wk = %s velo = %f\n",wk,mv->mdymc); */
            mv->card[57] = wk[0];
            mv->card[58] = wk[1];
            mv->card[59] = wk[2];
            mv->card[60] = wk[3];
            mv->card[61] = wk[4];
        }
        else if(c != '~') {
            if(isdigit(vx->vgv[gvc][0])) {
                sscanf(vx->vgv[gvc],"%f",&fv);
                sprintf(wk,"%5.1f",fv);
                /* printf("gvc = %d wk = %s fv = %f\n",gvc,wk,fv); */
                mv->card[57] = wk[0];
                mv->card[58] = wk[1];
                mv->card[59] = wk[2];
                mv->card[60] = wk[3];
                mv->card[61] = wk[4];
            }
        }
        /* c == ~ No change */
        gvc++;
    }
    return(0);
}

int vexpspace_meb(char card[160])
//char card[160];
{
    int  i,k;
    int  wfg;
    char wk[160];
  
    wfg = 0;
    k = 0;
    for(i = 0; i < 159; i++) {
        if(card[i] != ' ') {
            wk[k++] = card[i];
            wfg = 1;
        }
        else {
            if(wfg) wk[k++] = ' ';
            wfg = 0;
        }
    }
    wk[k] = '\0';
    strcpy(card,wk);
    return(0);
}

