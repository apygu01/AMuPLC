
// ----------------------
//  AMuPLC (meb8.c)
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
extern MEBVAR *mb;
extern MEBEDT *ge;
extern MEBPH  *ph;
extern MEBGRT *gr,*gd,*ga,*gm,*gs,*gc;
extern MEBCPS *cp;
extern MEBANS *ma;
/* char phx[MAXPH][20];  phrase text */
/* char phl[MAXPL][82];  phrase line */
//#define MAXFN 30
FILE *fexsv;  /* Save expand file flag */
char *exl1 = "%82 [        0   0  0:  0 ...  0                        ";
char *exl2 = "             12.8 . ...";
char *ttl = " p.v. mod cn:cdt pcg tp.______.______.______.______.______. ";
char *vel = "velo P_Exp";
/* char *dexps = "_..."; */
char dexps[4];

/*
 Symbol or Text  Type      Efect
;aaa                I    aaa -> Next line
;xyz o/             I    xyz -> o/ lines in meas 
;xyz > _/.          I    xyz -> greater than _/. lines
;Text * o/         II    Text -> xyz -> all o/ lines
;Text xx           II    Text -> xyz -> Number of xx lines
;Text Byy          II    Text -> xyz -> yy meas note lines
;Tp.Solo-1 Byy o/  II    Tp.Solo-1 -> tp1 -> yy meas o/ lines
;bbb *              I    bbb -> all lines
;Text < o/.        II    Text -> xyz less than o/. lines
  priority   0:none  1:xx,byy  2:*(all) -> nval
*/
int setexp_meb(char *inf)
//char *inf;
{
    int  i,j;
    char wk[16];
    int  pf;
    int  pr;
    int  df;
    int  hr;
    char c,c2;
    int  lc;
    long nval;
    int  k;
    char card[82];
    char exp[4];

    beauspace_meb(mv->card);
    strcpy(card,mv->card);
    getmph_meb(inf,card,exp);
    /* printf("card=%s\n",card); */
    pr = 0;
    hr = 0;
    k = 0;
    while(card[k]) {
        if(card[k] == '*') pr = 2; 
        if(card[k] == '>') hr = 2; 
        if(card[k] == '<') hr = 3; 
        if(card[k] == '=') {
            hr = 1;
            if(card[k - 1] == '>') hr = 4; 
            if(card[k - 1] == '<') hr = 5; 
        }
        if(card[k] == '\0') break; 
        if(card[k] == '\n') break; 
        k++;
        if(k > 80) break;
    }
    k = 0;
    while(card[k]) {
        if(card[k] == '\0') break; 
        if(card[k] == '\n') break; 
        k++;
        if(k > 81) break;
    }
    /* printf("%s\n",card); */
    savexs_meb(card,exp);
    ph->bc[ph->esc] = 0;
    ph->nc[ph->esc] = 0;
    ph->nval[ph->esc] = 0;
    ph->af[ph->esc] = 0;
    ph->pr[ph->esc] = 0;
    ph->nval[ph->esc] = 0;
    pf = 0;
    i = 0;
    while(card[i] != ' ') {
        i++;
        if(i > k) {
            pf = 1;
            break;
        }
    }
    nval = 0;
    ph->af[ph->esc] = 1;
    ph->bc[ph->esc] = 1;
    ph->nc[ph->esc] = 1;
    if(!pf) {
        c = card[++i];
        c2 = card[i + 1];
        if(isdigit(c)) {
            j = 0;
            while(isdigit(card[i])) {
                wk[j] = card[i];
                j++;
                i++;
            } 
            wk[j] = '\0';
            sscanf(wk,"%d",&lc);
            /* printf("wk = %s lc = %d\n",wk,lc); */
            ph->bc[ph->esc] = 32767;
            ph->nc[ph->esc] = lc;
            pr = 1;
        }
        else {
            if(isalpha(c)) c = tolower(c);
            /* printf("c = %c\n",c); */
            switch(c) {
                case  '*' : pr = 2;  /* priority */
                            ph->bc[ph->esc] = 32767;
                            ph->nc[ph->esc] = 32767;
                            break;
                case  '=' : /* EQ */
                            ph->bc[ph->esc] = 32767;
                            ph->nc[ph->esc] = 32767;
                            break;
                case  '>' : /* GT */
                            ph->bc[ph->esc] = 32767;
                            ph->nc[ph->esc] = 32767;
                            break;
                case  '<' : /* LT */
                            ph->bc[ph->esc] = 32767;
                            ph->nc[ph->esc] = 32767;
                            break;
                case  'b' : /* meas */
                            i++;
                            j = 0;
                            while(isdigit(card[i])) {
                                wk[j] = card[i];
                                j++;
                                i++;
                            }
                            wk[j] = '\0';
                            sscanf(wk,"%d",&lc); 
                            /* printf("wk = %s lc = %d\n",wk,lc); */
                            pr = 1;
                            ph->bc[ph->esc] = lc;
                            ph->nc[ph->esc] = 32767;
                            break;
                default   : /* none */
                            pr = 0;
                            break;
            }
        }
        while(card[i] != ' ') {
            i++;
            if(i > k) break;
        }
        if(i <= k) {
            wk[0] = '\0';
            strcpy(wk,"%xx ");
            j = 4;
            i++;
            while(card[i] != ' ') {
                wk[j] = card[i];
                j++;
                i++;
                if(i > k) break;
            }
            wk[j] = '\0';
            if(j > 5) {
                /* printf("wk = %s j = %d k = %d\n",wk,j,k); */
                nval = getnval_meb(wk);
                if(nval < 0) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                        printf("Error! %06d Expand symbol\n",mv->line);
                    ph->af[ph->esc] = 0;
                }
            }
        }
    }
    if(pr >= 3) {
        ph->bc[ph->esc] = 32767; /* Max int */
        ph->nc[ph->esc] = 32767;
    }
    k = 4;
    df = 0;
    while(card[k]) {
        if(isdigit(card[k])) {
            df = 1;
            break;
        }
        if(card[k] == '\0') break; 
        if(card[k] == '\n') break; 
        k++;
        if(k > 81) break;
    }
    if(df) {
        j = 0;
        i = k;
        while(isdigit(card[i])) {
            wk[j] = card[i];
            j++;
            i++;
        } 
        wk[j] = '\0';
        /* printf("card = %s wk = %s\n",card,wk); */
        sscanf(wk,"%d",&lc);
        if(card[k - 1] == 'b' || card[k - 1] == 'B') {
            /* Bar Count */
            ph->bc[ph->esc] = lc;
        }
        else {
            /* Note Count */
            ph->nc[ph->esc] = lc;
        }
    }
    ph->pr[ph->esc] = pr;
    ph->hr[ph->esc] = hr;
    ph->nval[ph->esc] = nval;
    if(mv->debug == 1) {
        printf("Debug1! ph->es[%d] = %s\n",ph->esc,ph->es[ph->esc]);
        printf("Debug1! ph->bc[%d] = %d\n",ph->esc,ph->bc[ph->esc]);
        printf("Debug1! ph->nc[%d] = %d\n",ph->esc,ph->nc[ph->esc]);
        printf("Debug1! ph->af[%d] = %d\n",ph->esc,ph->af[ph->esc]);
        printf("Debug1! ph->nval[%d] = %ld\n",ph->esc,ph->nval[ph->esc]);
        printf("Debug1! ph->pr[%d] = %d\n",ph->esc,ph->pr[ph->esc]);
    }
    ph->esc++;
    if(ph->esc > MAXEX) {
        mv->wrncnt++;
        mv->msglvl = 2;
        if(mv->msglvl >= mv->prtlvl)
            printf("Worning! %06d Expand symbol over than %d\n",
                mv->line,MAXEX);
    }
    return(0);
}

int savexs_meb(char *card,char exp[4])
//char *card;
//char exp[4];
{
    int  k;

    k = 0;
    while(card[k]) {
        if(card[k] == ' ') break; 
        if(card[k] == '\0') break; 
        if(card[k] == '\n') break; 
        k++;
        if(k > 81) break;
    }
    /* printf("%s k=%d\n",card,k); */
    if(k == 4) {
        ph->es[ph->esc][0] = card[1];
        ph->es[ph->esc][1] = card[2];
        ph->es[ph->esc][2] = card[3];
        ph->es[ph->esc][3] = '\0';
        return(0);
    }
    ph->es[ph->esc][0] = exp[0];
    ph->es[ph->esc][1] = exp[1];
    ph->es[ph->esc][2] = exp[2];
    ph->es[ph->esc][3] = '\0';
    return(0);
}

/* set expand symbol */
int chkes_meb()
{
    long nval;
    int  i,k;
    int  pr;
    int  nv;
    int  nf;

    nval = getnval_meb(mv->card);
    /* printf("nval = %ld\n",nval); */
    k = 0; /* add 2011 */
    pr = 99; /* priority */
    for(i = 0; i < ph->esc; i++) {
        if(ph->af[i]) {
            if(ph->pr[i] < pr) k = i;
        }
    }
    pr = ph->pr[k];
    nv = 900000;
    for(i = 0; i < ph->esc; i++) {
        if(ph->pr[i] == pr && ph->af[i] != 0) {
            if(ph->nval[i] > nv) k = i;
            nv = ph->nval[i];
        }
    }
    nf = 0;
    if(ph->af[k]) {
        switch(ph->hr[k]) {
            case  0 : /*  none */
                      if(ph->nval[k] == 0) setes_meb(k);
                      break;
            case  1 : /* = EQ */
                      if(nval == ph->nval[k]) setes_meb(k); 
                      break;
            case  2 : /* > GT */
                      if(nval > ph->nval[k]) setes_meb(k); 
                      break;
            case  3 : /* < LT */
                      if(nval < ph->nval[k]) setes_meb(k); 
                      break;
            case  4 : /* >= GE */
                      if(nval >=  ph->nval[k]) setes_meb(k); 
                      break;
            case  5 : /* <= LE */
                      if(nval <=  ph->nval[k]) setes_meb(k); 
                      break;
            default : break;
        }
        nf = 0;
        for(i = 0; i < 12; i++) {
            if(mv->card[i] == '[') nf = 1; 
        }
        if(mv->card[75] == '_' || mv->card[75] == '-') {
            /* if(nf == 0) ph->nc[k]--; */
            ph->nc[k]--;
            if(ph->nc[k] <= 0) ph->af[k] = 0;
            if(mv->debug == 1) 
            printf("ph->nc[%d] = %d ph->af[%d] = %d\n",k,ph->nc[k],k,ph->af[k]);
        }
    }
    /* printf("++%s\n",mv->card); */
    return(nf);
}

int setes_meb(int k)
//int  k;
{
    int  i;
    char w[4];

    if(mv->debug == 1) printf("Debug1! getes_meb() k=%d\n",k);
    if(mb->expnl == 1) return(0);  /* Show exp */
    if(mb->expnl == 2) return(0);  /* Hide exp */
    if(mb->expnl == 4) return(0);  /* Save exp */
    if(mb->expnl == 5) return(0);  /* Inc exp */
    if(mb->expnl == 6) return(0);  /* Load exp files */
    for(i = 0; i < 12; i++) {
        if(mv->card[i] == '[') {
            return(1); /* ignore [x renpu line */
        }
    }
    w[0] = mv->card[76];
    w[1] = mv->card[77];
    w[2] = mv->card[78];
    if(w[0] == '.' && w[1] == '.' && w[2] == '.') {
        mv->card[76] = ph->es[k][0];
        mv->card[77] = ph->es[k][1];
        mv->card[78] = ph->es[k][2];
    }
    return(0);
}

int expandn_meb(char *inf)
//char *inf;
{
    char w[4];
    char c;
    static char expf[MAXFN]; /* File mame max. */
    int  rtn;
    int  i;

    w[0] = mv->card[76];
    w[1] = mv->card[77];
    w[2] = mv->card[78];
    w[3] = '\0';
    if(mv->debug == 1) printf("Debug1! expandn_() inf = %s exp = %s\n",inf,w);
    /* printf("%s\n",mv->card); */
    if(mb->expnl == 7) { /* clear expand symbols */
        mv->card[74] = '.';
        mv->card[75] = '_';
        mv->card[76] = '.';
        mv->card[77] = '.';
        mv->card[78] = '.';
        return(0);
    }
    c = mv->card[75];
    if(c == '+' || c == '-') return(0);
    if(c == '_') {
        if(mb->expsvf) {
            fclose(fexsv);  /* Close saved expand file */
            mb->expsvf = 0; /* Save expand file closed */
            mv->msglvl = 1;
            if(mv->msglvl >= mv->prtlvl)
                printf("Expand note lines saved to %s\n",expf);
            savesym_meb(expf);
        }
    }
    rtn = 0;
    if(w[0] != '.' && w[1] != '.' && w[2] != '.') {
        /* printf("%s\n",w); */
        if(mv->card[75] == '_') mkexpfn_mmm(inf,expf,w);
        /* printf("%s\n",expf); */
        if(mb->expnl == 6) loadexpf_meb(expf);
        if(mb->expnl == 1) showexp_meb(expf);
        if(mb->expnl == 2) rtn = hidexp_meb();
        if(mb->expnl == 3) rtn = rstexp_meb();
        if(mb->expnl == 4) rtn = savexp_meb(expf);
        if(mb->expnl == 5) rtn = inclexp_meb(expf);
    }
    if(mv->next[0] != '%') rtn = 0;
    if(mv->card[75] == ' ') {
        if(w[0] == '.' && w[1] == '.' && w[2] == '.') mv->card[75] = '_';
    }
    return(rtn);
}

/* fn.mph -> fn_exp.mph */
int loadexpf_meb(char expf[MAXFN])
//char expf[MAXFN];
{
    int  i,k;
    int  rtn;
    int  dupf;
    char fn[STLN];
    char exp[STLN];
    char text[4];
    char line[82];
    char exlin[82];
    FILE *mph,*mpx;
    char scrf[100];

    /* printf("loadexp expf = %s\n",expf); */
    for( i = 0; i < STLN; i++) {
        fn[i] = expf[i];
        if(expf[i] == '_') break;
        if(expf[i] == '\0') break;
    }
    text[0] = expf[i+1];
    text[1] = expf[i+2];
    text[2] = expf[i+3];
    text[3] = '\0';
    fn[i] = '\0';
    strcat(fn,".mph");
    dupf = 0;
    for( i = 0; i < ph->exlc; i++) {
        if(!strcmp(text,ph->exls[i])) dupf = 1;
    }
    /* printf("dupf = %d\n",dupf); */
    if(dupf) return(0); /* duplicate No Load */
    mv->msglvl = 1;
    if(mv->msglvl >= mv->prtlvl) {
        printf("Reload expand file expf=%s text=%s fn=%s\n",expf,text,fn);
    }
    rtn = 0;
    if((mph = fopen(fn,"r")) != 0) {
        rtn = expserch_meb(text,mph,exp,exlin); /* Serch text & get exp */ 
    }
    else {
        if((mph = fopen("meb.mph","r")) == 0) {
            sprintf(scrf,"%s",TACTDIR);
            //sprintf(scrf,"%s\0",TACTDIR);
            strcat(scrf,"meb.mph");
            /* printf("scrf = %s\n",scrf); */
            if((mph = fopen(scrf,"r")) == 0) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! Can't open meb.mph mode r\n");
                } 
                return(0);
            }
        }
    }
    if(!rtn) {
        mph = fopen("meb.mph","r");            
        rtn = expserch_meb(text,mph,exp,exlin); /* Serch text & get exp */
    }
    if(rtn) {
        mpx = fopen(expf,"w");
        fputs(exlin,mpx);
        k = 0;
        while(fgets(line, 82, mph)) { /* read fn.mph */
            if(line[0] != '%' && line[0] != '-') break;
            if(line[0] == '%') {
                /* printf("%s",line); */
                fputs(line,mpx);
                k++;
            }
        }
        if(!k) {
            fprintf(mpx,"%s",exl1);
            fprintf(mpx,"%s\n",exl2);
        }
        fclose(mpx);
    }
    else {
        if((mpx = fopen(expf,"r")) == 0) { /* expf no exsist */
            printf("Expand file %s no exsist put blank line\n",expf);
            mpx = fopen(expf,"w");
            fprintf(mpx,"!%s\n",text);
            fprintf(mpx,"%s",exl1);
            fprintf(mpx,"%s\n",exl2);
            fclose(mpx);
        }
    }
    fclose(mph);
    loadsym_meb(expf);
    return(0);
}

int loadsym_meb(char expf[MAXFN])
//char expf[MAXFN];
{       
    int  i;
    char wk[4];
    
    i = 0;
    /* printf("expf = %s\n",expf); */
    while(expf[i] != '_') { 
        i++;
        if(i > MAXFN) break;
    }
    wk[0] = expf[++i];
    wk[1] = expf[++i];
    wk[2] = expf[++i];
    wk[3] = '\0'; 
    strcpy(ph->exls[ph->exlc],wk); /* Save symbol */
    /* printf("ph->exfs[%d] = %s\n",ph->exlc,ph->exfs[ph->exlc]); */
    ph->exlc++;
    return(0);
}   

int savesym_meb(char expf[MAXFN])
//char expf[MAXFN];
{
    int  i;
    char wk[4];

    i = 0;
    /* printf("expf = %s\n",expf); */
    while(expf[i] != '_') {
        i++;
        if(i > MAXFN) break;
    }
    wk[0] = expf[++i];
    wk[1] = expf[++i];
    wk[2] = expf[++i];
    wk[3] = '\0';
    strcpy(ph->exfs[ph->exfc],wk); /* Save symbol */
    /* printf("ph->exfs[%d] = %s\n",ph->exfc,ph->exfs[ph->exfc]); */
    ph->exfc++;
    return(0);
}

int showexp_meb(char expf[MAXFN])
//char expf[MAXFN];
{
    FILE *fpe;
    char line[82];
    int  lc;  /* Line count */
    int  ll;
    char np[37];
    int  i,k;
    char wk[4];
    int  rf;
    int  grate; /* gate rate */
    int  glc;   /* gate time line count */

    rf = 0;
    i = 4;
    while(mv->card[i] != ' ') {
        i++;
        if(mv->card[i] == '[') rf = 1;
        if(i > 13) break;
    }
    /* if(mv->card[i - 1] == '[') rf = 1; */
    if(mv->card[4] == '[') {
        mb->prflg = 0;
        return(1);  /* Return for pcaseper_meb() */
    }
    lc = 0;
    if((fpe = fopen(expf,"r")) != 0) {
        while(fgets(line, 82, fpe)) {
            if(line[0] == '%') lc++;
            if(line[2] == '$') break;
        }
        fclose(fpe);
    }
    else {
        mv->wrncnt++;
        mv->msglvl = 1;
        if(mv->msglvl >= mv->prtlvl)
            printf("Worning! %06d %s not found\n",mv->line,expf);
        return(0);
    }
    mb->prflg = 0;
    for(k = 0; k < 37; k++) np[k] = ' '; 
    for(i = 32,k = 0; i < 69; i++,k++) {
        if(mv->card[i] == ':') np[k] = ':';
        if(isdigit(mv->card[i])) np[k] = ':';
    }
    wk[0] = mv->card[1];
    wk[1] = mv->card[2];
    wk[2] = '\0';
    sscanf(wk,"%d",&grate);
    glc = ((lc * grate) + 1) / 100;
    /* printf("wk = %s grate = %c%d lc = %d glc = %d\n",
          wk,'%',grate,lc,glc); */
    i = 0;
    while(expf[i] != '.') {
        i++;
        if(i >= MAXFN) break;
    }
    wk[0] = expf[i - 3];
    wk[1] = expf[i - 2];
    wk[2] = expf[i - 1];
    ll = 0;
    /* printf("%s\n",mv->next); */
    if((fpe = fopen(expf,"r")) != 0) {
        verln_meb(rf,line,lc);
        while(fgets(line, 82, fpe)) {
            ll++;
            if(line[0] == '%') {
                line[4] = '[';
                line[75] = ' ';
                line[76] = wk[0];
                line[77] = wk[1];
                line[78] = wk[2];
                for(i = 32,k = 0; i < 69; i++,k++) {
                    line[i] = np[k];
                    if(mv->next[i] == ':') line[i] = ':';
                    else if(ll > glc) line[i] = ' ';
                    if(mv->next[0] != '%') {
                        if(mv->next2[i] == ':') line[i] = ':';
                        else if(ll > glc) line[i] = ' ';
                    } 
                    /* else if(ll > glc) line[i] = ' '; */
                }
                if(mv->source == 1 || mv->trace) printf("%s", line);
                mprintf_meb(line);
            }
            if(line[2] == '$') break;
        }
        fclose(fpe);
    }
    return(1);  /* Do not 0 */
}

/* Include expanded note lines */
int inclexp_meb(char expf[MAXFN])
//char expf[MAXFN];
{
    FILE *fpe;
    char line[82];
    int  lc;  /* Line count */
    int  ll;
    char np[37]; 
    int  i,k;
    char wk[4];
    int  rf;

    rf = 0;
    i = 4;
    /*
    printf("%s\n",mv->card);
    printf("%s\n",mv->next);
    */
    while(mv->card[i] != ' ') {
        i++;
        if(mv->card[i] == '[') rf = 1;
        if(i > 13) break;
    }
    /* if(mv->card[i - 1] == '[') rf = 1; */
    if(mv->card[4] == '[') {
        mb->prflg = 0;
        return(1);  /* Return for pcaseper_meb() */
    }
    lc = 0;
    if((fpe = fopen(expf,"r")) != 0) {
        while(fgets(line, 82, fpe)) {
            if(line[0] == '%') lc++;
            if(line[2] == '$') break;
        }
        fclose(fpe);
    }
    else {
        mv->wrncnt++;
        mv->msglvl = 1;
        if(mv->msglvl >= mv->prtlvl)
            printf("Worning! %06d %s not found\n",mv->line,expf);
        return(0);
    }
    mb->prflg = 0;
    ll = 0;
    for(k = 0; k < 37; k++) np[k] = ' '; 
    /*
    for(i = 32,k = 0; i < 69; i++,k++) {
        if(mv->card[i] == ':') np[k] = ':';
        if(isdigit(mv->card[i])) np[k] = ':';
    }
    */
    i = 0;
    while(expf[i] != '.') {
        i++;
        if(i >= MAXFN) break;
    }
    wk[0] = expf[i - 3];
    wk[1] = expf[i - 2];
    wk[2] = expf[i - 1];
    if((fpe = fopen(expf,"r")) != 0) {
        verln_meb(rf,line,lc);
        while(fgets(line, 82, fpe)) {
            ll++;
            if(line[0] == '%') {
                line[4] = '[';
                line[76] = wk[0];
                line[77] = wk[1];
                line[78] = wk[2];
                for(i = 32,k = 0; i < 69; i++,k++) line[i] = np[k]; 
                if(mv->source == 1 || mv->trace) printf("%s", line);
                mprintf_meb(line);
            }
            if(line[2] == '$') break;
        }
        fclose(fpe);
    }
    return(0);
}

int verln_meb(int rf,char line[82],int lc)
//int  rf; /* Renp flag */
//char line[82];
//int  lc;
{
    int  i,j,k;
    char wk[8];
    char wk2[14];
    int  pf;

    /* printf("rf = %d lc = %d\n",rf,lc); */
    if(rf) {
        i = 4;
        pf = 0;
        while(mv->card[i] != ' ') {
            if(mv->card[i] == '+' || mv->card[i] == '-') {
                pf = 1;
                break;
            }
            i++;
            if(i > 13) break;
        }
        if(pf) {
            j = i;
            k = 0;
            while(mv->card[j] != ' ') {
                wk2[k] = mv->card[j];
                j++;
                k++;
                if(j > 13) break;
            }
        }
        i = 4;
        while(mv->card[i] != '[') {
            i++;
            if(i > 13) break;
        }
        for(j = i; j < 13; j++) {
            if(mv->card[j] == ' ') break;
            mv->card[j] = ' ';
        }
        mv->card[i] = '[';
        sprintf(wk,"%4d",lc + 1);
        if(wk[0] != ' ') mv->card[++i] = wk[0];
        if(wk[1] != ' ') mv->card[++i] = wk[1];
        if(wk[2] != ' ') mv->card[++i] = wk[2];
        if(wk[3] != ' ') mv->card[++i] = wk[3];
        if(pf) {
            for( j = 0; j < k; j++) mv->card[++i] = wk2[j];
        }
    }
    else {
        i = 4;
        pf = 0;
        while(mv->card[i] != ' ') {
            if(mv->card[i] == '+' || mv->card[i] == '-') {
                pf = 1;
                break;
            }
            i++;
            if(i > 13) break;
        }
        if(pf) {
            j = i;
            k = 0;
            while(mv->card[j] != ' ') {
                wk2[k] = mv->card[j];
                j++;
                k++;
                if(j > 13) break;
            }
        }
        mv->card[i] = '[';
        sprintf(wk,"%4d",lc + 1);
        /* printf("%s\n",wk); */
        if(wk[0] != ' ') mv->card[++i] = wk[0];
        if(wk[1] != ' ') mv->card[++i] = wk[1];
        if(wk[2] != ' ') mv->card[++i] = wk[2];
        if(wk[3] != ' ') mv->card[++i] = wk[3];
        if(pf) {
            for( j = 0; j < k; j++) mv->card[++i] = wk2[j];
        }
    } 
    if(mv->source == 1 || mv->trace) printf("%s\n",mv->card);
    mprintf_meb(mv->card);
    return(0);
}

int hidexp_meb()
{
    int  i,j,k;
    int  mkf;

    if(mv->card[4] == '[') {
        mb->prflg = 0;
        return(1);  /* Return for pcaseper_meb() */
    }
    mkf = 0;
    for(i = 4; i < 13; i++) {
        if(mv->card[i] == '[') {
            mkf = 1;
            break;
        }
    }
    for(k = 4; k < 13; k++) {
        if(mv->card[k] == '+' || mv->card[k] == '-') {
            mkf = 2;
            break;
        }
    }
    if(mkf == 1) for(--i;i < 13;i++) mv->card[i] = ' ';
    //if(mkf == 1) for(i - 1;i < 13;i++) mv->card[i] = ' ';
    if(mkf == 2) {
        for(j = 4; j < 13; j++) if(mv->card[j] == '[') break;
        while(mv->card[k] != ' ') {
            mv->card[j] = mv->card[k];
            j++;
            k++;
        }
        for( ;j < k; j++) mv->card[j] = ' ';
    }
    return(0);
}

int rstexp_meb()
{
    int  i,j;
    int  mkf;
    int  swf;

    if(mv->card[75] == '+') return(0);
    if(mv->card[75] == '-') return(0);
    if(mv->card[4] == '[') {
        mb->prflg = 0;
        return(1);  /* Return for pcaseper_meb() */
    }
    mkf = 0;
    for(i = 4; i < 13; i++) {
        if(mv->card[i] == '[') {
            mkf = 1;
            break;
        }
    }
    swf = 0;
    for(j = 4; j < 13; j++) {
        if(mv->card[j] == '+' || mv->card[j] == '-') {
            swf = 1;
            break;
        }
    }
    if(mkf) {
        if(swf) {
            for(--i; i < 13; i++,j++) {
            //for(i - i; i < 13; i++,j++) {
                if(mv->card[j] == ' ') break;
                mv->card[i] = mv->card[j];
            }
            for(; i < 13; i++) mv->card[i] = ' ';
        }
        else for(--i;i < 13;i++) mv->card[i] = ' ';
        //else for(i - 1;i < 13;i++) mv->card[i] = ' ';
    }
    mv->card[74] = '.';
    mv->card[75] = '_';
    mv->card[76] = '.';
    mv->card[77] = '.';
    mv->card[78] = '.';
    return(0);
}

int savexp_meb(char expf[MAXFN])
//char expf[MAXFN];
{           
    int  i;
    char card[82];
    int  dupf;  /* Expand symbol duplicate */
    char wk[4];
    char bakf[MAXFN];  /* fn_xyz.mpb backup */

    /* printf("expf = %s\n",expf); */
    strcpy(card,mv->card);
    i = 0;
    while(expf[i] != '_') {
        i++;
        if(i > MAXFN) break;
    }
    wk[0] = expf[++i];
    wk[1] = expf[++i];
    wk[2] = expf[++i];
    wk[3] = '\0';
    dupf = 0;
    for( i = 0; i < ph->exfc; i++) {
        if(!strcmp(wk,ph->exfs[i])) dupf = 1;
    }
    /* printf("wk=%s dupf = %d\n",wk,dupf); */
    if(dupf) return(0); /* duplicate No Save */
    strcpy(bakf,expf);
    i = 0;
    while(bakf[i] != '.') {
        i++;
        if(i > MAXFN) break;
    }
    bakf[++i] = 'm'; 
    bakf[++i] = 'p'; 
    bakf[++i] = 'b'; 
    bakf[++i] = '\0'; 
    /* printf("expf = %s bakf = %s\n",expf,bakf); */
    if(card[75] == '_' && mv->next[75] == ' ') {
        if(!mb->expsvf) rename(expf,bakf);
        if((fexsv = fopen(expf,"w")) != 0) {
            mb->expsvf = 1; 
        }
    }
    if(card[75] == ' ') {
        if(mb->expsvf) {
            i = 0;
            while(expf[i] != '_') {
                i++;
                if(i > MAXFN) break; 
            }
            i++;
            card[76] = expf[i++];
            card[77] = expf[i++];
            card[78] = expf[i];
            /* printf("%s\n",card); */
            fprintf(fexsv,"%s\n",card);
        }
    }
    return(4); /* Do'not 0 */
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
        outf[i++] = *pif;
        if(i >= MAXFN -1 ) break;
        pif++;
    }
    outf[i] = '\0';
    strcat_h(outf,"_");
    strcat_h(outf,str);
    strcat_h(outf,".mph");
    /* printf("%s\n",outf); */
    return(0);
}

/*
 read fn.mph file ,If not found fn.mph then open meb.mph 
 Check !text and get !xyz ,
 put %xx [ line to fn_xyz.mph file
*/
int getmph_meb(char *inf,char card[82],char exp[4])
//char *inf;
//char card[82];
//char exp[4];
{
    int  i,j,k;
    char fn[STLN];
    char expf[STLN];
    FILE *mph,*mpx;
    char line[82];
    char exlin[82];
    char text[30];
    int  rtn;

    for( i = 0; i < STLN; i++) {
        fn[i] = inf[i];
        expf[i] = inf[i];
        if(inf[i] == '_') break;
        if(inf[i] == '\0') break;
    }
    fn[i] = '\0';
    expf[i] = '\0';
    strcat(fn,".mph");
    /* printf("fn=%s %s\n",fn,card); */
    text[0] = card[1];
    text[1] = card[2];
    text[2] = card[3];
    text[3] = '\0';
    k = strlen(card);
    if(k > 4) {
        if(card[4] != ' ') {
            i = 1;
            j = 0;
            while(card[i] != ' ') {
                if(card[i] == '\n') break;
                if(card[i] == '\r') break;
                text[j] = card[i];
                i++;
                j++;
                if(j > 29) break;
           }
           text[j] = '\0';
       }
    }
    rtn = 0;
    if((mph = fopen(fn,"r")) != 0) {
        rtn = expserch_meb(text,mph,exp,exlin); /* Serch text & get exp */
    }
    else {
        if((mph = fopen("meb.mph","r")) == 0) {
            mv->errcnt++;            
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl) {
                printf("Error! meb.mph file not found\n");
            }
            return(0);
        }
    }
    if(!rtn) {
        mph = fopen("meb.mph","r");
        rtn = expserch_meb(text,mph,exp,exlin); /* Serch text & get exp */
    }
    if(rtn) {
        strcat(expf,"_");
        strcat(expf,exp);
        strcat(expf,".mph"); 
        /* printf("expf=%s\n",expf); */
        if((mpx = fopen(expf,"r")) != 0) {
            fclose(mph);
            fclose(mpx);
            return(0);
        }
        /* Check & Create fn_exp.mph */
        expcard_meb(inf,card,exp,expf);
        if((mpx = fopen(expf,"r")) == 0) { /* if fn_abc.mph file not exsist */
            mpx = fopen(expf,"w");
            fputs(exlin,mpx); /* put !xxx line */
            k = 0;
            while(fgets(line, 82, mph)) { /* read fn.mph */
                if(line[0] != '%' && line[0] != '-') break;
                if(line[0] == '%') {
                    /* printf("%s",line); */
                    fputs(line,mpx);
                    k++;
                }
            }
            if(!k) {
                fprintf(mpx,"%s",exl1);
                fprintf(mpx,"%s\n",exl2);
            }
        }
        fclose(mpx);
    }
    fclose(mph);
    return(0);
}   

/* Check & Create fn_exp.mph */
int expcard_meb(char *inf,char card[82],char exp[4],char expf[STLN])
//char *inf;
//char card[82];
//char exp[4];
//char expf[STLN];
{
    int  i,j;
    char wk[82];

    for( i = 0; i < STLN; i++) {
        expf[i] = inf[i];
        if(inf[i] == '_') break;
        if(inf[i] == '\0') break;
    }
    expf[++i] = '\0';
    strcat(expf,exp);
    strcat(expf,".mph");
    /* printf("expf=%s\n",expf); */

    strcpy(wk,card);
    card[0] = ';';
    card[1] = exp[0];
    card[2] = exp[1];
    card[3] = exp[2];
    i = 3;
    while(wk[i] != ' ') {
        i++;
        if(wk[i] == '\0') break;
        if(wk[i] == '\r') break;
        if(wk[i] == '\n') break;
        if(i > 30) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
                printf("Error! %06d %s Expand symbol\n",mv->line,wk);
            break; 
        }
    }
    /* printf("%s\n",card); */
    j = 4;
    for( ; i < 82; i++) {
        card[j] = wk[i];
        j++;
        if(card[j] == '\0') break;
        if(card[j] == '\r') break;
        if(card[j] == '\n') break;
    }
    /* printf("%s\n",card); */
    return(0);
}

/* Serch text, rtn : 1 found */
int expserch_meb(char *text,FILE *mph,char exp[4],char exlin[82])
//char *text;
//FILE *mph;
//char exp[4];
//char exlin[82];
{
    char line[82];
    int  rtn;

    /* printf("Serch %s\n",text); */
    exlin[0] = '\0';
    while(fgets(line, 82, mph)) {
        rtn = txserch_meb(line,text); 
        if(rtn) {
           exp[0] = line[1];
           exp[1] = line[2];
           exp[2] = line[3];
           exp[3] = '\0';
           if(mv->debug == 1) printf("!%s exp = %s\n",text,exp);
           strcpy(exlin,line);
           return(1);
        }
    }
    return(0);
}

/* Serch text 2, rtn : 0 Not found  1 : found  Max. 10 text */
int txserch_meb(char line[82],char *text)
//char line[82];
//char *text;
{
    int  i,j,k;
    char wk[30];
    char tx[30];

    beauspace_meb(line);
    j = strlen(text);
    if(j == 3) {
        wk[0] = line[1];
        wk[1] = line[2];
        wk[2] = line[3];
        wk[3] = '\0';
        if(!strcmp(wk,text)) return(1); /* found */
        return(0);
    }
    for(i = 0; i < j; i++) {
        if(text[i] == '\0') break;
        if(text[i] == '\r') break;
        if(text[i] == '\n') break;
        tx[i] = tolower(text[i]);
    }
    tx[i] = '\0';
    k = strlen(line);
    if(k < 6) return(0);
    i = 5;
    /* 1 */
    j = 0;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("tx=%s %s ",tx,wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 2 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }   
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 3 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 4 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 5 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 6 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 7 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 8 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 9 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */

    /* 10 */
    j = 0;
    i++;
    while(line[i] != ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        wk[j] = line[i];
        wk[j] = tolower(line[i]);
        i++;
        if(i >= k) return(0);
        j++;
        if(j > 29) break;
    }
    wk[j] = '\0';
    /* printf("%s ",wk); */
    if(!strcmp(wk,tx)) return(1); /* found */
    return(0);
}

int brepear_meb()
{
    int  i,j;

    j = 0;
    for(i = 69; i < 79; i++) {
        mv->card[i] = vel[j];
        j++;
    }
    return(0);
}

/* Save Expand Symbol to ph->exrs[MXEX][4] */
int svexrs_meb()
{
    if(mv->card[76] == '.') return(0);
    if(mv->card[77] == '.') return(0);
    if(mv->card[78] == '.') return(0);
    /* if(mv->next[75] != ' ') return(0); Show or Include expand ? */
    ph->exrc[ph->exrs][0] = mv->card[76];
    ph->exrc[ph->exrs][1] = mv->card[77];
    ph->exrc[ph->exrs][2] = mv->card[78];
    ph->exrc[ph->exrs][3] = '\0';
    ph->exrc++;
    return(0);
}

/* Store fn_exp.mph to fn.mph */
int storexp_meb(char inf[STLN],char expf[STLN])
//char inf[STLN];
//char expf[STLN];
{
    int  i;
    int  perf;
    char phf[STLN];
    char exf[STLN];
    char tmpf[STLN];
    FILE *mph;
    char line[82];

    strcpy(phf,inf);
    i = 0;
    while(phf[i] != '_') {
        if(phf[i] == '\0') break;
        if(phf[i] == '\r') break;
        if(phf[i] == '\n') break;
        i++;
        if(i > STLN) break;
    }
    phf[i] = '\0';
    strcpy(tmpf,phf);
    strcat(tmpf,".tmp");
    strcpy(exf,phf);
    strcat(exf,".mph");
    strcat(phf,"_");
    strcat(phf,expf);
    strcat(phf,".mph");
    perf = 0;
    if((mph = fopen(phf,"r")) != 0) {
        /*printf("phf = %s expf = %s\n",phf,expf); */
        while(fgets(line, 82, mph)) { /* read fn_abc.mph */
            if(line[0] == '%') {
                perf = 1;
                break;
            }
        }
        fclose(mph);
    }
    if(perf) repexp_meb(exf,expf,phf,tmpf);
    return(0);
}

/* Regster fn_exp.mph to meb.mph */
int regstxp_meb(char inf[STLN],char expf[STLN])
//char inf[STLN];
//char expf[STLN];
{
    int  i;
    int  perf;
    char phf[STLN];
    char tmpf[STLN];
    FILE *mph;
    char line[82];
    
    strcpy(phf,inf);
    i = 0;
    while(phf[i] != '_') {
        if(phf[i] == '\0') break;
        if(phf[i] == '\r') break;
        if(phf[i] == '\n') break;
        i++;
        if(i > STLN) break;
    }
    phf[i] = '\0';
    strcpy(tmpf,phf);
    strcat(tmpf,".tmp");
    strcat(phf,"_");
    strcat(phf,expf);
    strcat(phf,".mph");
    perf = 0;
    if((mph = fopen(phf,"r")) != 0) {
        while(fgets(line, 82, mph)) { /* read fn_abc.mph */
            if(line[0] == '%') {
                perf = 1;
                break; 
            }
        }
        fclose(mph);
    }
    if(perf) repexp_meb("meb.mph",expf,phf,tmpf);
    return(0); 
}   

/* Replace expand symbol lines */
int repexp_meb(char exf[STLN],char expf[STLN],char phf[STLN],char tmpf[STLN])
//char exf[STLN];
//char expf[STLN];
//char phf[STLN];
//char tmpf[STLN];
{
    int  af;
    int  eaf;
    int  skc;
    FILE *fr1,*fr2,*fw1,*fw2,*fr3,*fw3,*fph;
    char line[82];
    char expl[82];
    char exw[4];
    int  got;

    if(mv->debug == 1) 
        printf("exf= %s,expf= %s,phf= %s,tmpf= %s\n",exf,expf,phf,tmpf);
    eaf = 0;
    af = 0;
    got = getexpfl_meb(expf,expl);
    if((fr1 = fopen(exf,"r")) != 0) { /* fn.mph exsist ? yes */
        if(mv->debug == 1) printf("exf(%s) -> tmpf(%s)\n",exf,tmpf);
        fw1 = fopen(tmpf,"w");
        skc = 0;
        while(fgets(line, 82, fr1)) { /* fn.mph -> fn.tmp (rmove %line) */
            if(line[0] == ';') {
                exw[0] = line[1];
                exw[1] = line[2];
                exw[2] = line[3];
                exw[3] = '\0';
                if(!strcmp(exw,expf)) {
                    af = 1;
                    eaf = 1;
                }
            }
            if(eaf == 1) {
                /* Skip puts */
                if(line[0] == '%') skc++; 
                else {
                    /* printf("%s",line); */
                    fputs(line,fw1);
                }
                if(skc && line[0] == ';') eaf = 0;
            }
            else {
                /* printf("%s",line); */
                fputs(line,fw1); 
            }
        }
        fclose(fr1);
        fclose(fw1);
        /* fn.tmp -> fn.mph expand file replace */
        fw2 = fopen(exf,"w");
        fr2 = fopen(tmpf,"r");
        if(mv->debug == 1) printf("tmpf(%s) phf(%s) -> exf(%s)\n",tmpf,phf,exf);
        if((fph = fopen(phf,"r")) == 0) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl) {
                printf("Error! %06d expand file %s not found\n",mv->line,phf);
            }
            return(1);
        }
        if(!af) {
            /* Put expand lines to Bottom of fn.mph */
            while(fgets(line,82,fr2)) fputs(line,fw2); 
            fprintf(fw2," !!!!!!!!%s",ttl);
            fprintf(fw2,"%s\n",vel);
            if(got) fprintf(fw2,"%s",expl);
            else fprintf(fw2,"!%s\n",expf);
            while(fgets(line,82,fph)) fputs(line,fw2); 
            fclose(fw2);
            fclose(fr2);
            fclose(fph);
            return(0);
        }
        eaf = 0;
        while(fgets(line, 82, fr2)) {
            if(line[0] == ';') {
                exw[0] = line[1];
                exw[1] = line[2];
                exw[2] = line[3];
                exw[3] = '\0';
                if(!strcmp(exw,expf)) eaf = 1; 
            }
            /* printf("%s",line); */
            fputs(line,fw2);
            if(eaf) {
                while(fgets(line,82,fph)) {
                    /* printf("%s",line); */
                    if(line[0] == '%') fputs(line,fw2);
                }
            }
            eaf = 0;
        }
        fclose(fr2);
        fclose(fph);
        fclose(fw2);
    }
    else { /* Create fn.mph */
        /* printf("exf = %s, phf = %s\n",exf,phf); */
        fw3 = fopen(exf,"w");
        fr3 = fopen(phf,"r");
        fprintf(fw3," Expand file\n");
        fprintf(fw3," !!!!!!!!%s",ttl);
        fprintf(fw3,"%s\n",vel);
        if(got) fprintf(fw3,"%s",expl);
        else fprintf(fw3,"!%s\n",expf);
        while(fgets(line, 82, fr3)) {
            fputs(line,fw3);
        }
        fclose(fw3);
        fclose(fr3);
    }
    return(0);
}

/* Get !abc expand symbol line from meb.mph */
int getexpfl_meb(char expf[STLN],char expl[82])
//char expf[STLN];
//char expl[82];
{
    FILE *fpr;
    char exw[4];

    if((fpr = fopen("meb.mph","r")) != 0) {
        while(fgets(expl,82,fpr)) {
            if(expl[0] == ';') {
                exw[0] = expl[1];
                exw[1] = expl[2];
                exw[2] = expl[3];
                exw[3] = '\0';
            }
            if(!strcmp(exw,expf)) {
                fclose(fpr);
                /* printf("expl=%s",expl); */
                return(1); /* Serch expf */
            } 
        }
        fclose(fpr);
    }
    return(0);
}

int arpeggio_meb()
{
    int  i,j;
    int  tsp,tep;
    int  kg;
    int  af;
    char wk[4];
    int  dv;
    char wv[4];
    char card[82];
    int  df;

    /* printf("%s\n",mv->befr); */
    /* if(!isdigit(mv->befr[1])) return(0); */
    if(mv->befr[1] != 'a' && mv->befr[1] != 'A') return(0);
    if(mv->card[75] == '_') {
        if(mv->card[76] != '.') return(0);
        if(mv->card[77] != '.') return(0);
        if(mv->card[78] != '.') return(0);
    }
    strcpy(card,mv->befr);

    /* printf("%s\n",card); */
    kg = 0;
    i = 2;
    df = 0;
    while(card[i] != ' ') {
        if(card[i] == '+') kg = 1;
        if(card[i] == '-') kg = 2;
        if(isdigit(card[i])) {
            df = 1;
            break;
        }
        if(card[i] == '\0') break; 
        if(card[i] == '\n') break; 
        if(card[i] == '\r') break; 
        i++;
    }
    if(df) {
        wk[0] = card[i]; 
        wk[1] = card[i+1]; 
        wk[2] = '\0'; 
        sscanf(wk,"%2d",&dv);
        sprintf(wv,"%02d",dv);
        /* printf("wk = %s dv = %d wv = %s\n",wk,dv,wv); */
    }
    tsp = 32;
    tep = 68;
    if( mb->bartyp == 'l' ||
        mb->bartyp == 'm' ||
        mb->bartyp == 'h') {
            tsp = 11;
            tep = 68;
    }
    af = 0;
    for(i = tsp; i < tep; i++) {
        if(card[i] == '-') af++;
        if(card[i] == '>') kg = 1; 
        if(card[i] == '<') kg = 2; 
    }
    if(af && kg && df) {
        if(kg == 1) mv->card[75] = '+';
        if(kg == 2) mv->card[75] = '-';
        mv->card[76] = 'a';
        mv->card[77] = wv[0];
        mv->card[78] = wv[1];
    }
    return(0);
}

/* Clear Dynamic Expand Symbol */
int cldexp_meb()
{
    if(mv->card[75] == '+' || mv->card[75] == '-') {
        mv->card[75] = '_';
        mv->card[76] = '.';
        mv->card[77] = '.';
        mv->card[78] = '.';
    }
    return(0);
}

/* Dynamic Expand */
int dynaexp_meb()
{
    int  i,j;
    int  pf;
    int  df;
    int  dv;
    char wk[6];
    char wv[4];
    char card[82];
    char c1;

    c1 = mv->card[1];
    if(c1 == '+' || c1 == '-') {
        artexp_meb();
        return(0);
    }
    if(mv->card[1] == 'a' || mv->card[1] == 'A') return(0); /* arpeggio */
    if(mv->card[2] == ' ') return(0); /* Vertical modify */
    i = 2;
    pf = 0;
    df = 0;
    /* printf("%s\n",mv->card); */
    while(mv->card[i] != ' ') {
        if(mv->card[i] == '+') pf = 1;
        if(mv->card[i] == '-') pf = -1;
        if(isdigit(mv->card[i])) {
            df = 1;
            break;
        }
        if(mv->card[i] == '\0') break; 
        if(mv->card[i] == '\n') break; 
        if(mv->card[i] == '\r') break; 
        i++;
    }
    /*
    if(pf && df) {
        if(pf == 1) mv->next[75] = '+';
        if(pf == -1) mv->next[75] = '-';
        wk[0] = mv->card[i]; 
        wk[1] = mv->card[i+1]; 
        wk[2] = '\0'; 
        sscanf(wk,"%2d",&dv);
        sprintf(wv,"%02d",dv);
        mv->next[76] = mv->card[1];
        mv->next[77] = wv[0];
        mv->next[78] = wv[1];
    }
    */
    if(pf && df) {
        if(pf == 1) dexps[0] = '+';
        if(pf == -1) dexps[0] = '-';
        wk[0] = mv->card[i];
        wk[1] = mv->card[i+1];
        wk[2] = '\0';
        sscanf(wk,"%2d",&dv);
        sprintf(wv,"%02d",dv);
        /* printf("wk = %s dv = %d wv = %s\n",wk,dv,wv); */
        dexps[1] = mv->card[1];
        dexps[2] = wv[0];
        dexps[3] = wv[1];
    }
    i = 2;
    pf = 0;
    mb->dexpn = 0;
    strcpy(card,mv->card);
    beauspace_meb(card);
    while(card[i] != ' ') {
        if(card[i] == '\0') {
            pf = 1;
            break;
        }
        if(card[i] == '\r') {
            pf = 1;
            break;
        }
        if(card[i] == '\n') {
            pf = 1;
            break;
        }
        i++;       
    }
    if(pf) return(0);
    i++;
    j = 0;
    while(isdigit(card[i])) {
        wk[j] = card[i];
        i++;
        j++;
        if(j > 5) break;
    }
    wk[j] = '\0';
    sscanf(wk,"%d",&mb->dexpn);
    dexps[0] = '+';
    /* printf("wk = %s mb->dexpn = %d\n",wk,mb->dexpn); */
    return(0);
}

int artexp_meb()
{
    int  i,j;
    int  len;
    char wk[6];
    char card[82];

    i = 6;
    j = 0;
    strcpy(card,mv->card);
    beauspace_meb(card);
    len = strlen(card);
    if(len > 5) {
        if(card[5] != ' ') {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl) {
                printf("Error! %s\n",mv->card);
            } 
            return(1); /* error *+abc line */
        }
    }
    mb->dexpn = 0;
    while(isdigit(card[i])) {
        wk[j] = card[i];
        i++;
        j++;
        if(j > len) break;
    }
    wk[j] = '\0';
    if(j > 0) sscanf(wk,"%d",&mb->dexpn);
    /* printf("wk = %s j = %d mb->dexpn = %d\n",wk,j,mb->dexpn); */
    dexps[0] = mv->card[1];
    dexps[1] = mv->card[2];
    dexps[2] = mv->card[3];
    dexps[3] = mv->card[4];
    return(0);
}

int setdexs_meb()
{
    mv->card[75] = dexps[0];
    mv->card[76] = dexps[1];
    mv->card[77] = dexps[2];
    mv->card[78] = dexps[3];
    if(mb->dexpn > 0) mb->dexpn--;
    return(0);
}

int addgsym_meb(char *inf,struct mivksn *mk,struct mivsym ms[],char obsf[STLN])
//char *inf;
//struct mivksn *mk; 
//struct mivsym ms[];
//char obsf[STLN];
{
    int  rtc;
    int  i;
    char partn[4];
    char card[82];

    if(mv->debug == 10) {
        printf("//debug10 addgsym_meb() mv->step=%d\n",mv->step);
    }
    if(mv->debug == 3)
        printf("Debug3! addgsym_meb() mv->mmpos = %d mv->mcpos = %d\n",
        mv->mmpos,mv->mcpos);
    if(mv->getsym) {
        while(mv->mmpos < mv->mcpos) {
            rtc = bexpcopy_meb();
            if(rtc) break;
            if(mm->mmov[mv->mmpos] == mb->sequ) {
                strcpy(card,mv->card); /* save */
                strcpy(mv->card,mm->msmb[mv->mmpos]);
                if(mv->card[0] == '{') {
                    if(mv->debug == 3) printf("addgsym(1) %s\n",mv->card);
                    getGKTPv_meb(mk);
                    pcasebeu_meb(mk,ms,obsf);
                    if(mv->debug == 3) ktbdmp_meb(mk);
                }
	        else add_sl_col(mv->next2);
                /* getGKTPv_meb(mk); */
                mprintf_meb(mv->card);
                if(mv->source == 1 || mv->trace) printf("%s\n",mv->card);
                strcpy(mv->card,card); /* back */
            }
            mv->mmpos++;
        }
        return(0);
    } 
    while(mv->mmpos < mv->mcpos) {
        rtc = bexpcopy_meb();
        if(rtc) break;
        /* 2011 renum Test */
        /*
        printf("mv->mmpos = %d mv->mcpos = %d\n",mv->mmpos,mv->mcpos);
        printf("Test++ mm->mbrc[%d]=%d\n",mv->mmpos,mm->mbrc[mv->mmpos]);
        */
        if(mm->onpc[mv->mmpos]) {
            i = 0;
            while(inf[i] != '_') {
                i++;
                if(i > STLN) break;
            }
            partn[0] = inf[i+1];
            partn[1] = inf[i+2];
            partn[2] = inf[i+3];
            partn[3] = '\0';
	    if(mv->debug == 3) printf("partn=%s\n",partn);
	    for(i = 0; i < mm->onpc[mv->mmpos]; i++) {
		if(!strcmp(partn,mm->onpart[mv->mmpos][i])) {
                     if(mm->mmov[mv->mmpos] == mb->sequ) {
		         strcpy(card,mv->card); /* save */
                         strcpy(mv->card,mm->msmb[mv->mmpos]);
                         if(mv->card[0] == '{') {
                             if(mv->debug == 3)
                                 printf("addgsym(2) %s\n",mv->card);
                             getGKTPv_meb(mk);
                             pcasebeu_meb(mk,ms,obsf);
                             if(mv->debug == 3) ktbdmp_meb(mk);
                         }
	                 else add_sl_col(mv->next2);
                         mprintf_meb(mv->card);
                         if(mv->source == 1 || mv->trace)
                             printf("%s\n",mv->card);
                         strcpy(mv->card,card); /* back */
                     }
                     break;
		}
            }
        }
        else {
            if(mm->mmov[mv->mmpos] == mb->sequ) {
                strcpy(card,mv->card); /* save */
                strcpy(mv->card,mm->msmb[mv->mmpos]);
                if(mv->card[0] == '{') {
                    if(mv->debug == 3) printf("addgsym(3) %s\n",mv->card);
                    getGKTPv_meb(mk);
                    pcasebeu_meb(mk,ms,obsf);
                    if(mv->debug == 3) ktbdmp_meb(mk);
                }
	        else add_sl_col(mv->next2);
                mprintf_meb(mv->card);
                if(mv->source == 1 || mv->trace) printf("%s\n",mv->card); 
                strcpy(mv->card,card); /* back */
            }
        }
        mv->mmpos++;
    }
    return(0);
}

/* Add 1998.3 */
int add_sl_col(char *card)
//char *card;
{
    int  i,j;

    /* printf("%s\n",card); */
    i = 0;
    while(mv->card[i]) i++;
    mv->card[i] = ' ';
    for(j = i; j < 70; j++) mv->card[j] = ' ';
    mv->card[j] = '\0';
    switch (mb->bartyp) {
        case  'l' : /* low bar */
                    for(i = 11 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'm' : /* middle */
                    for(i = 11 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'h' : /* high */
                    for(i = 11 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  't' : /* top-synth */
                    for(i = 32 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  's' : /* synth */
                    for(i = 32 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'g' : /* gclef */
                    for(i = 32 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'a' : /* alt */
                    for(i = 32 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'b' : /* bass */
                    for(i = 32 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'u' : /* under-bass */
                    for(i = 32 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'r' : /* rhythm */
                    for(i = 14 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        case  'd' : /* dod   */
                    for(i = 32 ; i < 69; i++)
                        if(mv->card[i] == ' ' && card[i] == ':')
                        mv->card[i] = ':';
                    break;
        default   : break;
    }
    return(0);
}

int define_meb(char *card,char *work)
//char *card;
//char *work;
{
    int  rtn;
    char wk[52];

    if(!strcmp(work,"gate_rate")) {
        ge->def_gr_f = 1;
        rtn = def_gate_rate(card);
	if(rtn == -3) {
            ge->def_gr_f = 11;
	}
        return(rtn);
    }
    // add 2013.11.22 for renpu [ %xx parameter reprm
    if(!strcmp(work,"gate_rate_parameter") ||
       !strcmp(work,"gate_rate_param")) {
        ge->def_rp_f = 1;
        ge->def_rp_v = numdinmb_meb(card);
        if(ge->def_rp_v > 50 || ge->def_rp_v < 0) return(1);
        return(0);
    }
    if(!strcmp(work,"pitch_vend") || !strcmp(work,"pitchvend")) {
	ge->def_pv_f = 1;
        ge->def_pv_v = numdinmb_meb(card);
        if(ge->def_pv_v > 9000 || ge->def_pv_v < -9000) return(2);
        return(0);
    }
    if(!strcmp(work,"modulation")) {
        ge->def_md_f = 1;
        ge->def_md_v = numdinmb_meb(card);
        if(ge->def_md_v > 127 || ge->def_md_v < 0) return(3);
        return(0);
    }
    /* support spell miss 2011.6 */
    if(!strcmp(work,"moduration")) {
        ge->def_md_f = 1;
        ge->def_md_v = numdinmb_meb(card);
        if(ge->def_md_v > 127 || ge->def_md_v < 0) return(3);
        return(0);
    }
    /* ------ */
    if(!strcmp(work,"control_number") || !strcmp(work,"control_no.")) {
        mb->cntlno = numdinmb_meb(card);
        if(mb->cntlno > 32 || mb->cntlno < 0) return(4);
        mb->cntlnod = mb->cntlno;
        return(0);
    }
    if(!strcmp(work,"control_data")) {
        mb->cntldt = numdinmb_meb(card);
        if(mb->cntldt > 127 || mb->cntldt < 0) return(5);
        mb->cntldtd = mb->cntldt;
        return(0);
    }
    if(!strcmp(work,"note_shift")) {
        mb->nsftv = numdinmb_meb(card);
        mb->nsftvd = mb->nsftv;
        return(0);
    }
    if(!strcmp(work,"transpose")) {
        ge->def_tp_f = 1;
        ge->def_tp_v = numdinmb_meb(card);
        if(ge->def_tp_v > 127 || ge->def_tp_v < -127) return(6);
        return(0);
    }
    if(!strcmp(work,"add_duration_rates")) {
        mb->durtev = numdinmb_meb(card);
        if(mb->durtev > 50 || mb->durtev < -50) return(7);
        mb->durtevd = mb->durtev;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity_bases") || !strcmp(work,"velocity_base")) {
	ge->def_vb_f = 1;
        ge->def_vb_v = fnumdin_meb(card);
        /* printf("ge->def_vb_v = %f\n",ge->def_vb_v); */
        if(ge->def_vb_v  > 15.0 || ge->def_vb_v < 0) return(8);
        return(0);
    }
    if(!strcmp(work,"velocity")) {
	ge->def_vl_f = 1;
        ge->def_vl_v = numdinmb_meb(card);
        /* printf("ge->def_vl_v = %d\n",ge->def_vl_v); */
        if(ge->def_vl_v  > 9 || ge->def_vl_v < 0) return(9);
        return(0);
    }
    if(!strcmp(work,"harmonics") || !strcmp(work,"harmonize")) {
	ge->def_hm_f = 1;
        ge->def_hm_v = numdinmb_meb(card);
        /* printf("ge->def_hm_v = %d\n",ge->def_hm_v); */
        if(ge->def_hm_v  > 9 || ge->def_hm_v < 0) return(10);
        return(0);
    }
    if(!strcmp(work,"chord_base_position")) {
        cp->cbp = numdinmb_meb(card);
        /* printf("cp->cbp = %d\n",cp->cbp); */
        if(cp->cbp  > 7 || cp->cbp < -2) return(9);
        return(0);
    }
    if(!strcmp(work,"chord_velocity") ||
        !strcmp(work,"chord_velocity_chars")) {
        rtn = setstr_meb(card,wk);
        if(!rtn) {
            strcpy(cp->cvc,wk);
            if(mv->debug == 6) printf("cp->cvc = %s\n",cp->cvc);
        }
        return(0);
    }
    if(!strcmp(work,"available_note_scale_base_position")) {
        ma->abp = numdinmb_meb(card);
        /* printf("ma->abp = %d\n",ma->abp); */
        if(ma->abp  > 7 || ma->abp < -2) return(9);
        return(0);
    }   
    if(!strcmp(work,"available_note_scale_velocity") || 
        !strcmp(work,"available_note_scale_velocity_chars")) {
        rtn = setstr_meb(card,wk);
        if(!rtn) {
            /* printf("ma->avc = %s\n",ma->avc); */
            strcpy(ma->avc,wk);
            if(mv->debug == 6) printf("ma->avc = %s\n",ma->avc);
        }
        return(0);
    }   
    if(!strcmp(work,"chord_name_voicing_velocity") || 
        !strcmp(work,"chord_name_voicing_velocity_chars")) {
        ma->nvf = 1;
        rtn = setstr_meb(card,wk);
        if(!rtn) {
            /* printf("ma->avc = %s\n",ma->nvc); */
            strcpy(ma->nvc,wk);
            if(mv->debug == 15) printf("ma->nvc = %s\n",ma->nvc);
        }
        return(0);
    }
    if(!strcmp(work,"next_progression_chord")) {
        cp->npc = numdinmb_meb(card);
        /* printf("cp->npc = %d\n",cp->npc); */
        if(cp->npc  > 9 || cp->npc < 0) return(9);
        return(0);
    }   
    return(12);
}

int assign_meb(char *card,char *work)
//char *card;
//char *work;
{
    int  rtn;

    if(!strcmp(work,"gate_rate")) {
        ge->asn_gr_f = 1;
        rtn = asn_gate_rate(card);
	if(rtn == -3) {
            ge->asn_gr_f = 11;
	}
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(rtn);
    }
    if(!strcmp(work,"pitch_vend") || !strcmp(work,"pitchvend")) {
	ge->asn_pv_f = 1;
        ge->asn_pv_v = numdinmb_meb(card);
        if(ge->asn_pv_v > 9000 || ge->asn_pv_v < -9000) return(2);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"modulation")) {
        ge->asn_md_f = 1;
        ge->asn_md_v = numdinmb_meb(card);
        if(ge->asn_md_v > 127 || ge->asn_md_v < 0) return(3);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* support spell miss 2011.6 */
    if(!strcmp(work,"moduration")) {
        ge->asn_md_f = 1;
        ge->asn_md_v = numdinmb_meb(card);
        if(ge->asn_md_v > 127 || ge->asn_md_v < 0) return(3);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* ------- */
    if(!strcmp(work,"control_number") || !strcmp(work,"control_no.")) {
        mb->cntlno = numdinmb_meb(card);
        if(mb->cntlno > 32 || mb->cntlno < 0) return(4);
        mb->cntlnod = mb->cntlno;
        mb->cntlnf = 2;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"control_data")) {
        mb->cntldt = numdinmb_meb(card);
        if(mb->cntldt > 127 || mb->cntldt < 0) return(5);
        mb->cntldtd = mb->cntldt;
        mb->cntldf = 2;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"note_shift")) {
        mb->nsftv = numdinmb_meb(card);
        mb->nsftvd = mb->nsftv;
        mb->nsftf = 2;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"transpose")) {
        ge->asn_tp_f = 1;
        ge->asn_tp_v = numdinmb_meb(card);
        if(ge->asn_tp_v > 127 || ge->asn_tp_v < -127) return(6);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"add_duration_rates")) {
        mb->durtev = numdinmb_meb(card);
        if(mb->durtev > 50 || mb->durtev < -50) return(7);
        mb->durtevd = mb->durtev;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity_bases") || !strcmp(work,"velocity_base")) {
	ge->asn_vb_f = 1;
        ge->asn_vb_v = fnumdin_meb(card);
        /* printf("ge->asn_vb_v = %f\n",ge->asn_vb_v); */
        if(ge->asn_vb_v  > 15.0 || ge->asn_vb_v < 0) return(8);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity")) {
	ge->asn_vl_f = 1;
        ge->asn_vl_v = numdinmb_meb(card);
        /* printf("ge->asn_vl_v = %d\n",ge->asn_vl_v); */
        if(ge->asn_vl_v  > 9 || ge->asn_vl_v < 0) return(9);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"transposition")) {
        ge->asn_ts_f = 1;
        ge->asn_ts_v = numdinmb_meb(card);
        printf("ge->asn_ts_v = %d\n",ge->asn_ts_v);
        if(ge->asn_ts_v  > 12 || ge->asn_ts_v < 0) return(9);
        beaucsym_meb(); /* line shift */
        return(0);
    }   
    return(12);
}

int modify_start_meb(char *card,char *work)
//char *card;
//char *work;
{
    int  rtn;

    if(!strcmp(work,"gate_rate")) {
        ge->mod_gr_f = 1;
        rtn = mod_gate_rate(card);
	/* printf("mv->card = %s rtn = %d\n",mv->card,rtn); */
	if(rtn == -3) {
            ge->mod_gr_f = 11;
	}
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"pitch_vend") || !strcmp(work,"pitchvend")) {
	ge->mod_pv_f = 1;
        ge->mod_pv_v = numdinmb_meb(card);
        if(ge->mod_pv_v > 9000 || ge->mod_pv_v < -9000) return(2);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"modulation")) {
        ge->mod_md_f = 1;
        ge->mod_md_v = numdinmb_meb(card);
        if(ge->mod_md_v > 127 || ge->mod_md_v < 0) return(3);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* support spell miss 2011.6 */
    if(!strcmp(work,"moduration")) {
        ge->mod_md_f = 1;
        ge->mod_md_v = numdinmb_meb(card);
        if(ge->mod_md_v > 127 || ge->mod_md_v < 0) return(3);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* ------ */
    if(!strcmp(work,"control_number")) {
        mb->cntlno = numdinmb_meb(card);
        if(mb->cntlno > 32 || mb->cntlno < 0) return(4);
        if(!mb->cntlnf) mb->cntlnf = 1;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"control_data")) {
        mb->cntldt = numdinmb_meb(card);
        if(mb->cntldt > 127 || mb->cntldt < 0) return(5);
        if(!mb->cntldf) mb->cntldf = 1;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* support spell miss */  
    if(!strcmp(work,"moduration")) {
        ge->mod_md_f = 1;
        ge->mod_md_v = numdinmb_meb(card);
        if(ge->mod_md_v > 127 || ge->mod_md_v < 0) return(3);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"controle_number")) {
        mb->cntlno = numdinmb_meb(card);
        if(mb->cntlno > 32 || mb->cntlno < 0) return(4);
        if(!mb->cntlnf) mb->cntlnf = 1;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"controle_data")) {
        mb->cntldt = numdinmb_meb(card);
        if(mb->cntldt > 127 || mb->cntldt < 0) return(5);
        if(!mb->cntldf) mb->cntldf = 1;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* ---- */
    if(!strcmp(work,"note_shift")) {
        mb->nsftv = numdinmb_meb(card);
        if(!mb->nsftf) mb->nsftf = 1;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"transpose")) {
        ge->mod_tp_f = 1;
        ge->mod_tp_v = numdinmb_meb(card);
        if(ge->mod_tp_v > 127 || ge->mod_tp_v < -127) return(6);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"add_duration_rates")) {
        mb->durtev = numdinmb_meb(card);
        if(mb->durtev > 50 || mb->durtev < -50) return(7);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity_bases") || !strcmp(work,"velocity_base")) {
	ge->mod_vb_f = 1;
        ge->mod_vb_v = fnumdin_meb(card);
        /* printf("ge->mod_vb_v = %f\n",ge->mod_vb_v); */
        if(ge->mod_vb_v  > 15.0 || ge->mod_vb_v < 0) return(8);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity")) {
	ge->mod_vl_f = 1;
        ge->mod_vl_v = numdinmb_meb(card);
        /* printf("ge->mod_vl_v = %d\n",ge->mod_vl_v); */
        if(ge->mod_vl_v  > 9 || ge->mod_vl_v < 0) return(9);
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"note") || !strcmp(work,"notes")) {
        mb->notecv = numdinmb_meb(card);
        if(mb->notecv > 9 || mb->notecv < 0) return(12);
        mb->notecf = 1;  /* notes change flag on */
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    return(12);
}

int modify_end_meb(char *card,char *work)
//char *card;
//char *work;
{
    if(!strcmp(work,"gate_rate")) {
        ge->mod_gr_f = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"pitch_vend") || !strcmp(work,"pitchvend")) {
        ge->mod_pv_f = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"modulation")) {
        ge->mod_md_f = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* support spell miss 2011.6 */
    if(!strcmp(work,"moduration")) {
        ge->mod_md_f = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    /* ------ */
    if(!strcmp(work,"control_number")) {
        if(mb->cntlnf == 2) {
            mb->cntlnf = 1;
            mb->cntlno = mb->cntlnod;
        }
        else mb->cntlnf = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"control_data")) {
        if(mb->cntldf == 2) {
            mb->cntldf = 1;
            mb->cntldt = mb->cntldtd;
        }
        else mb->cntldf = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"note_shift")) {
        if(mb->nsftf == 2) {
            mb->nsftf = 1;
            mb->nsftv = mb->nsftvd;
        }
        else mb->nsftf = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"transpose")) {
        ge->mod_tp_f = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"add_duration_rates")) {
        mb->durtev = mb->durtevd;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity_bases") || !strcmp(work,"velocity_base")) {
        ge->mod_vb_f = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity")) {
	ge->mod_vl_f = 0;
        if(mv->card[0] != '{') beaucsym_meb(); /* line shift */
        return(0);
    }
    return(12);
}

int set_meb(char *card,char *work)
//char *card;
//char *work;
{
    int  rtn;

    if(!strcmp(work,"gate_rate")) {
        ge->set_gr_f = 1;
        rtn = set_gate_rate(card);
	if(rtn == -3) {
            ge->set_gr_f = 11;
	}
        return(rtn);
    }
    // add 2013.11.22 for renpu [ %xx parameter reprm
    if(!strcmp(work,"gate_rate_parameter") ||
       !strcmp(work,"gate_rate_param")) {
        ge->set_rp_f = 1;
        ge->set_rp_v = numdinmb_meb(card);
        if(ge->set_rp_v > 50 || ge->set_rp_v < 0) return(1);
        return(0);
    }
    if(!strcmp(work,"pitch_vend") || !strcmp(work,"pitchvend")) {
	ge->set_pv_f = 1;
        ge->set_pv_v = numdinmb_meb(card);
        if(ge->set_pv_v > 9000 || ge->set_pv_v < -9000) return(2);
        return(0);
    }
    if(!strcmp(work,"modulation")) {
        ge->set_md_f = 1;
        ge->set_md_v = numdinmb_meb(card);
        if(ge->set_md_v > 127 || ge->set_md_v < 0) return(3);
        return(0);
    }
    /* support spell miss 2011.6 */
    if(!strcmp(work,"moduration")) {
        ge->set_md_f = 1;
        ge->set_md_v = numdinmb_meb(card);
        if(ge->set_md_v > 127 || ge->set_md_v < 0) return(3);
        return(0);
    }
    /* ------- */
    if(!strcmp(work,"control_number")) {
        mb->cntlno = numdinmb_meb(card);
        if(mb->cntlno > 32 || mb->cntlno < 0) return(4);
        if(!mb->cntlnf) mb->cntlnf = 1;
        return(0);
    }
    if(!strcmp(work,"control_data")) {
        mb->cntldt = numdinmb_meb(card);
        if(mb->cntldt > 127 || mb->cntldt < 0) return(5);
        if(!mb->cntldf) mb->cntldf = 1;
        return(0);
    }
    if(!strcmp(work,"note_shift")) {
        mb->nsftv = numdinmb_meb(card);
        if(!mb->nsftf) mb->nsftf = 1;
        return(0);
    }
    if(!strcmp(work,"transpose")) {
        ge->set_tp_f = 1;
        ge->set_tp_v = numdinmb_meb(card);
        if(ge->set_tp_v > 127 || ge->set_tp_v < -127) return(6);
        return(0);
    }
    if(!strcmp(work,"add_duration_rates")) {
        mb->durtev = numdinmb_meb(card);
        if(mb->durtev > 50 || mb->durtev < -50) return(7);
        return(0);
    }
    if(!strcmp(work,"velocity_bases") || !strcmp(work,"velocity_base")) {
	ge->set_vb_f = 1;
        ge->set_vb_v = fnumdin_meb(card);
        /* printf("ge->set_vb_v = %f\n",ge->set_vb_v); */
        if(ge->set_vb_v  > 15.0 || ge->set_vb_v < 0) return(8);
        return(0);
    }
    if(!strcmp(work,"velocity")) {
	ge->set_vl_f = 1;
        ge->set_vl_v = numdinmb_meb(card);
        /* printf("ge->set_vl_v = %d\n",ge->set_vl_v); */
        if(ge->set_vl_v  > 9 || ge->set_vl_v < 0) return(9);
        return(0);
    }
    return(13);
}

int unset_meb(char *card,char *work)
//char *card;
//char *work;
{
    if(!strcmp(work,"gate_rate")) {
        ge->set_gr_f = 0;
        return(0);
    }
    if(!strcmp(work,"pitch_vend") || !strcmp(work,"pitchvend")) {
        ge->set_pv_f = 0;
        return(0);
    }
    if(!strcmp(work,"modulation")) {
        ge->set_md_f = 0;
        return(0);
    }
    /* support spell miss 2011.6 */
    if(!strcmp(work,"moduration")) {
        ge->set_md_f = 0;
        return(0);
    }
    /* ------ */
    if(!strcmp(work,"control_number")) {
        mb->cntlnf = 0;
        return(0);
    }
    if(!strcmp(work,"control_data")) {
        mb->cntldf = 0;
        return(0);
    }
    if(!strcmp(work,"note_shift")) {
        mb->nsftf = 0;
        return(0);
    }
    if(!strcmp(work,"transpose")) {
        ge->set_tp_f = 0;
        return(0);
    }
    if(!strcmp(work,"velocity_bases") || !strcmp(work,"velocity_base")) {
        ge->set_vb_f = 0;
        return(0);
    }
    if(!strcmp(work,"velocity")) {
	ge->set_vl_f = 0;
        return(0);
    }
    return(13);
}

int add_sub_clear(char *card,char *work,int f)
//char *card;
//char *work;
//int  f;
{
    int  i;
    int  rtn;
    int  dck;

    if(!strcmp(work,"gate_rate")) {
        ge->asc_gr_f = f;
        rtn = asc_gate_rate(card);
        /*
	printf("mv->card = %s rtn = %d mb->bexp = %d\n",mv->card,rtn,mb->bexp);
        */
	if(rtn == -3) {
            ge->asc_gr_f = f + 10;
	}
        /* beaucsym_meb(); * line shift */
        if(f == 5) return(0); // #reset add 2013.10.18 
        if(mv->step >= 2) beaucsym_meb(); /* modify 2002.11 line shift */
        return(0);
    }
    if(!strcmp(work,"pitch_vend") || !strcmp(work,"pitchvend")) {
	ge->asc_pv_f = f;
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
	ge->asc_pv_v = 0;
        if(dck) {
            ge->asc_pv_v = numdinmb_meb(card);
        }
        if(ge->asc_pv_v > 9000 || ge->asc_pv_v < -9000) return(2);
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"modulation") || !strcmp(work,"moduration")) {
        ge->asc_md_f = f;
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
	ge->asc_md_v = 0;
        if(dck) {
            ge->asc_md_v = numdinmb_meb(card);
        }
        if(ge->asc_md_v > 127 || ge->asc_md_v < 0) return(3);
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"control_number") || !strcmp(work,"control_no.")) {
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
        mb->cntlno = 0;
        if(dck) mb->cntlno = numdinmb_meb(card);
        if(mb->cntlno > 32 || mb->cntlno < 0) return(4);
        if(!mb->cntlnf) mb->cntlnf = 1;
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"control_data")) {
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
        mb->cntldt = 0;
        if(dck) mb->cntldt = numdinmb_meb(card);
        if(mb->cntldt > 127 || mb->cntldt < 0) return(5);
        if(!mb->cntldf) mb->cntldf = 1;
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"program_change")) {
        mb->prgchgf = 1;
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"note_shift")) {
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
        mb->nsftv = 0; /* clear */
        if(dck) mb->nsftv = numdinmb_meb(card);
        if(!mb->nsftf) mb->nsftf = 1;
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"transpose")) {
        ge->asc_tp_f = f;
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
	ge->asc_tp_v = 0;
        if(dck) {
            ge->asc_tp_v = numdinmb_meb(card);
        }
        if(ge->asc_tp_v > 127 || ge->asc_tp_v < -127) return(6);
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"add_duration_rates")) {
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
        mb->durtev = 0;
        if(dck) mb->durtev = numdinmb_meb(card);
        if(mb->durtev > 50 || mb->durtev < -50) return(7);
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity_bases") || !strcmp(work,"velocity_base")) {
	ge->asc_vb_f = f;
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
	ge->asc_vb_v = 0;
        if(dck) {
            ge->asc_vb_v = fnumdin_meb(card);
        }
        /* printf("ge->asc_vb_v = %f\n",ge->asc_vb_v); */
        if(ge->asc_vb_v  > 15.0 || ge->asc_vb_v < 0) return(8);
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    if(!strcmp(work,"velocity")) {
	ge->asc_vl_f = f;
        dck = 0;
        i = 0;
	while(card[i] != '\0') {
            if(isdigit(card[i])) dck++;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
	    i++;
	}
	ge->asc_vl_v = 0;
        if(dck) {
            ge->asc_vl_v = numdinmb_meb(card);
        }
        /* printf("ge->asc_vl_v = %d\n",ge->asc_vl_v); */
        if(ge->asc_vl_v  > 9 || ge->asc_vl_v < -9) return(9);
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        if(f == 5) return(0); // #reset add 2013.10.18
        return(0);
    }
    if(!strcmp(work,"note") || !strcmp(work,"notes")) {
        mb->notecv = numdinmb_meb(card);
        if(mb->notecv > 9 || mb->notecv < 0) return(12);
        mb->notecf = 1;  /* notes change flag on */
        if(f == 5) return(0); // #reset add 2013.10.18
        if(mv->step >= 2) beaucsym_meb(); /* line shift */
        return(0);
    }
    return(14);
}

/* call from define_meb()
 ex.
 #define chord_velocity_chars "4444 567..."
 #define chord_velocity_chars 44434
*/
int setstr_meb(char *card,char *wk)
//char *card;
//char wk[52];
{
    int  i,j;
    int  qf;
    int  af;
    int  len;

    i = 0;
    qf = 0;
    while(1) {
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
        if(card[i] == '\"') qf++;
        if(card[i] == '\'') qf++;
        i++;
        if(i > 79) break;
    }
    if(qf > 2) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! %s\n",card);
        return(1);
    }
    if(qf == 2) {
        af = 0;
        i = 0;
        while(1) {
            if(card[i] == '\0') break;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
            if(card[i] == '\'' || card[i] == '\"') {
                af = 1;
                break;
            }
            i++;
            if(i > 76) break;
        }
        if(af) {
            /* printf("i = %d %c\n",i,card[i]); */
            j = 0;
            i++;
            while(1) {
                if(card[i] == '\"' || card[i] == '\'') break;
                if(card[i] == '\0') break;
                if(card[i] == '\r') break;
                if(card[i] == '\n') break;
                wk[j] = card[i];
                i++;
                if(i > 79) break;
                j++;
                if(j > 50) break;
            }
            wk[j] = '\0';
            /* printf("wk = %s\n",wk); */
            return(0); /* ok */
        }
        return(1); /* ng */
    }
    len = strlen(card);
    /* printf("len = %d\n",len); */
    i = len;
    while(1) {
        if(card[i] == ' ') break;
        i--;
        if(i < 30) break;
    }
    /* printf("i = %d %c\n",i,card[i]); */
    /* i++; */
    j = 0;
    while(1) {
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
        wk[j] = card[i];
        i++;
        if(i < 30) break;
        j++;
        if(j > 50) break;
    }
    wk[j] = '\0';
    /* printf("wk = %s\n",wk); */
    return(0); /* ok */
}

int getGKTPv_meb(struct mivksn *mk)
//struct mivksn *mk; 
{
    char card[82];
    int  i,j;
    int  pf;

    if(mv->card[1] != '$') return(1);
    if(mv->card[2] == ':' || mv->card[3] == ':' ||
        mv->card[4] == ':') return(1);
    i = 0;
    j = 1;
    pf = 0;
    while(1) {
        if(mv->card[j] == '}') break;
        if(mv->card[j] == 'T') pf++;  /* Transposition ? */
        if(mv->card[j] == 'p') pf++;
        card[i] = mv->card[j];
        i++;
        j++;
        if(j > 80) break;
    }
    card[i] = '\0';
    if(!pf) return(1);
    mb->GKTPv = numdinmb_meb(card);
    if(mv->debug == 3)
        printf("getGKTPv() %s mb->GKTPv = %d\n",card, mb->GKTPv); 
    setorgktb_meb(mk); /* set bktb[] */
    return(0);
}

/* set original key table bktb[]  */
int setorgktb_meb(struct mivksn *mk) 
//struct mivksn *mk; 
{   
    int  i,j,m,md;
    char card[82];

    if(mv->card[1] != '$') return(1);
    if(mv->card[2] == ':' || mv->card[3] == ':' ||
        mv->card[4] == ':') return(1);
    /* if(mv->debug == 3) nktbdmp_meb(mk); */
    if(mv->debug == 3) printf("getorgkey() %s\n",mv->card);
    strcpy(card,mv->card);
    mb->ksin = 0;
    i = 2;
    if(card[i] == '#') {
        while(card[i] == '#') {
            i++;
            mb->ksin++;
        }
    }   
    if(card[i] == '&') {
        mb->ksin = 7;
        while(card[i] == '&') {
            i++;
            mb->ksin++;
        }
    }   
    if(mb->ksin < 0 || mb->ksin > 14) {
        mb->ksin = 0;
    }   
    /* set bktb */
    for(j = 0; j < 78; j++) mv->bktb[j] = mv->nktb[j];
    if(mb->ksin < 8) {
        m = -1;
        for(i = 0; i < mb->ksin; i++) {
            m = m + 4;
            md = m % 7;
            for( j = 0 ; j < 11 ; j++) {
                mv->bktb[md] = mv->nktb[md] + 1;
                md = md + 7;
            }
        }
    }
    else {
        m = 3; 
        for(i = 7; i < mb->ksin; i++) {
            m = m + 3; 
            md = m % 7;
            for( j = 0 ; j < 11 ; j++) {
                mv->bktb[md] = mv->nktb[md] - 1;
                md = md + 7;
            }
        }
    }
    if(mv->debug == 3) bktbdmp_meb(mk);
    return(0); 
}

int nktbdmp_meb(struct mivksn *mk)
//struct mivksn *mk;
{   
    int  i,j,k;
    int  ky;
    int  ksin;
    char *pc;
    char wscl[8];
    
    pc = "321Cc*234567";
    wscl[0] = '\0';
    ky = 0;
    switch(mv->mkscl) {
        case  1 : strcat_h(wscl,"_dur");
                  ky = 0;
                  break;
        case  2 : strcat_h(wscl,"_moll");
                  ky = 1;
                  break;
        case  3 : strcat_h(wscl,"_Major");
                  ky = 0;    
                  break;
        case  4 : strcat_h(wscl,"_minor"); 
                  ky = 1;
                  break;
        default : strcat_h(wscl,"_Major ?");
                  ky = 0; 
                  break;
    }
    ksin = mb->ksin;
    k = 0;
    printf("\n    nktbdmp()  %s%s\n",mk->keysin[ky][ksin],wscl);
    printf("    C    D    E    F    G    A    H  : OCT\n");
    for(i = 0; i < 11; i++) {
        for(j = 0; j < 7; j++) printf(" %4d",mv->nktb[k++]);
        printf("  :   %c\n",*pc);
        pc++;
    }
    return(0);
}

int bktbdmp_meb(struct mivksn *mk)
//struct mivksn *mk;
{   
    int  i,j,k;
    int  ky;
    int  ksin;
    char *pc;
    char wscl[8];
    
    if(mv->card[1] != '$') return(0);
    if(mv->card[2] == ':' || mv->card[3] == ':' ||
        mv->card[4] == ':' ) return(0);
    pc = "321Cc*234567";
    wscl[0] = '\0';
    ky = 0;
    switch(mv->mkscl) {
        case  1 : strcat_h(wscl,"_dur");
                  ky = 0;
                  break;
        case  2 : strcat_h(wscl,"_moll");
                  ky = 1;
                  break;
        case  3 : strcat_h(wscl,"_Major");
                  ky = 0;    
                  break;
        case  4 : strcat_h(wscl,"_minor"); 
                  ky = 1;
                  break;
        default : strcat_h(wscl,"_Major ?");
                  ky = 0; 
                  break;
    }
    ksin = mb->ksin;
    printf("mb->ksin = %d\n",mb->ksin);
    k = 0;
    printf("\n    bktbdmp()  %s%s\n",mk->keysin[ky][ksin],wscl);
    printf("    C    D    E    F    G    A    H  : OCT\n");
    for(i = 0; i < 11; i++) {
        for(j = 0; j < 7; j++) printf(" %4d",mv->bktb[k++]);
        printf("  :   %c\n",*pc);
        pc++;
    }
    return(0);
}   

int ktbdmp_meb(struct mivksn *mk)
//struct mivksn *mk;
{   
    int  i,j,k;
    int  ky;
    int  ksin;
    char *pc;
    char wscl[8];
    
    if(mv->card[1] != '$') return(0);
    if(mv->card[2] == ':' || mv->card[3] == ':' ||
        mv->card[4] == ':' ) return(0);
    pc = "321Cc*234567";
    wscl[0] = '\0';
    ky = 0;
    switch(mv->mkscl) {
        case  1 : strcat_h(wscl,"_dur");
                  ky = 0;
                  break;
        case  2 : strcat_h(wscl,"_moll");
                  ky = 1;
                  break;
        case  3 : strcat_h(wscl,"_Major");
                  ky = 0;    
                  break;
        case  4 : strcat_h(wscl,"_minor"); 
                  ky = 1;
                  break;
        default : strcat_h(wscl,"_Major ?");
                  ky = 0; 
                  break;
    }
    ksin = mb->ksin;
    k = 0;
    printf("\n    ktbdmp()  %s%s\n",mk->keysin[ky][ksin],wscl);
    printf("    C    D    E    F    G    A    H  : OCT\n");
    for(i = 0; i < 11; i++) {
        for(j = 0; j < 7; j++) printf(" %4d",mv->ktb[k++]);
        printf("  :   %c\n",*pc);
        pc++;
    }
    return(0);
}   

/* make original transposed key
*/
int mkotrkey_meb(int trv)
//int  trv;
{
    int  kval;
    int  i;

    switch(mb->ksin) {
        case  0 : mb->ogkval = 0;      /* C */
                  break;
        case  1 : mb->ogkval = 7;      /* # G */
                  break;
        case  2 : mb->ogkval = 2;      /* ## D */
                  break;
        case  3 : mb->ogkval = 9;      /* ### A */
                  break;
        case  4 : mb->ogkval = 4;      /* #### E */
                  break;
        case  5 : mb->ogkval = 11;     /* ##### H */
                  break;
        case  6 : mb->ogkval = 6;      /* ###### Fis */
                  break;
        case  7 : mb->ogkval = 1;      /* ####### Cis */
                  break;
        case  8 : mb->ogkval = 5;      /* & F */
                  break;
        case  9 : mb->ogkval = 10;     /* && B */
                  break;
        case 10 : mb->ogkval = 3;      /* &&& Es */ 
                  break;
        case 11 : mb->ogkval = 8;      /* &&&& As */ 
                  break;
        case 12 : mb->ogkval = 1;      /* &&&&& Des */
                  break;
        case 13 : mb->ogkval = 6;      /* &&&&&& Ges */
                  break;
        case 14 : mb->ogkval = 11;     /* &&&&&&& Ces */
                  break;
        default : if(!mv->pass) {
                      mv->errcnt++;
                      mv->msglvl = 3;
                      if(mv->msglvl >= mv->prtlvl)
                      printf("Error! MKTRKEY00 %s mb->ksin = %d\n",
                      mv->card,mb->ksin);
                  }
                  return(-1);
    }
    kval = mb->ogkval - trv;
    if(kval < 0) kval = 12 + (kval % 12);
    else kval %= 12;
    if(mv->debug == 3) {
        printf("kval = %d mb->ogkval = %d trv = %d\n",
            kval,mb->ogkval,trv);
    }
    mb->tpkval = mb->trnst[mb->trktyp][kval];
    if(mb->tpkval > 15) {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! MKTRKEY01 mb->tpkval = %d\n",mb->tpkval);
        }
        return(-1);
    }
    if(mb->tpkval < 8) {
        for(i = 0; i < mb->tpkval; i++) mb->otrkey[i] = '#';
    }
    else for(i = 0; i < mb->tpkval - 7 ; i++) mb->otrkey[i] = '&';
    mb->otrkey[i] = '\0';
    if(mv->debug == 3) {
        printf("mb->ksin=%d mb->ogkval=%d trv=%d\n",
            mb->ksin,mb->ogkval,trv);
        printf("Debug mb->tpkval=%d mb->otrkey=%s\n",
            mb->tpkval,mb->otrkey);
    }
    return(0);
}
