
// ----------------------
//  AMuPLC (mec8.c)
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
extern MECMPV *mp;
extern MECMPV *mp;
extern MECPCV *pc;
char jrule[MAXRL][82];  /* Major Rule */
char irule[MAXRL][82];  /* minor Rule */
int  itw[4][128];

/* @line inst program change add 2007.12 */
/*
 Instruments        File     Bar   Port  Chan  Memb  Device     PC#           !
 1234567890123456789012345678901234567890123456789012345678901234567890
@()                 001_...  S_.   1_..  13_..    2  GM          72           !
*/
int mpchg_mec()
{
    int  n;
    char port[4]; /* card position 34-35 */
    char chan[4]; /* card position 41-42 */
    char pcgn[4]; /* card position 64-66 */

    n = pc->n;
    if(mg->debug == 9) {
        printf("mpchg_mec() pc->n=%d\n",pc->n);
        printf("%s\n",mg->card);
    }
    port[0] = mg->card[34];
    port[1] = mg->card[35];
    port[2] = '\0';
    chan[0] = mg->card[41];
    chan[1] = mg->card[42];
    chan[2] = '\0';
    pcgn[0] = mg->card[64];
    pcgn[1] = mg->card[65];
    pcgn[2] = mg->card[66];
    pcgn[3] = '\0';
    if(mg->debug == 9) {
        printf("mpchg_mec() port=%s chan=%s pcgn=%s\n",
        port,chan,pcgn);
    }
    sscanf(port,"%d",&pc->port[n]);
    sscanf(chan,"%d",&pc->chan[n]);
    pc->af[n] = 0;
    if(isdigit(pcgn[2])) {
        sscanf(pcgn,"%d",&pc->pcgn[n]);
        if(pc->port >= 0 && pc->port[n] <= 16) {
            if(pc->chan[n] >= 0 && pc->chan[n] <= 16) {
                if(pc->pcgn[n] >= 0 && pc->pcgn[n] <= 128) pc->af[n] = 1;
            }
        }
    }
    if(mg->debug == 9) {
        printf("n=%d pc->af=%d pc->port=%d pc->chan=%d pc->pcgn=%d\n",
        n,pc->af[n],pc->port[n],pc->chan[n],pc->pcgn[n]);
    }
    pc->n++;
    if(pc->n > MXMB) {
        printf("Error! mpchg_() MXMB over\n");
        pc->n = 0;
    }
    return(0);
}

/* add 2007.12
 @instruments                   PC#
Prog.change   : FDCxyyzz
 x(chan)      : channel no.
 yy(prgn)     : program number
 zz(....)     :
*/
int outpcgn_mec()
{
    union meccode fd;
    int  i,j;
    int  chan;

    if(mg->debug == 9) {
        printf("outpcgn_mec() mg->btyp=%c ",mg->btyp);
        printf("mr->bnum=%d mr->port=%d mr->chan=%d\n",
        mr->bnum,mr->port,mr->chan);
    }
    if(mg->btyp == 'r') return(0); // add 2013.7.31
    if(pc->bf) return(0);
    for(i = 0; i < pc->n ;i++) {
        /*
        printf("i=%d pc->af[i]=%d pc->pcgn[i]=%d pc->chan[i]=%d\n",
            i,pc->af[i],pc->pcgn[i],pc->chan[i]);
        */
        if(pc->af[i]) {
            chan = pc->chan[i] - 1;
            if(mr->port == pc->port[i] && mr->chan == chan) {
                mr->pgiv = pc->pcgn[i] - 1;
                if(mr->pgiv < 0 || mr->pgiv > 128) {
                    printf("Error! outpcgn_mev() Program change data %d\n",
                    mr->pgiv);
                    return(-1);
                }
                fd.zi[0] = 0xfdc0;
                fd.zi[1] = 0;
                mr->chan &= 0x000f;
                fd.zi[0] |= mr->chan;
                mr->pgiv &= 0x007f;
                j = mr->pgiv << 8;
                fd.zi[1] |= j;
                fd.zi[1] |= 0x00ff; /* 0xff system reset must ignore at mlk */
                if(mg->debug) {
                    printf(
                    "Debug! outpcgn_mec() mr->chan=%d mr->pgiv=%d\n",
                         mr->chan,mr->pgiv);
                }
                if(mg->trace) {
                    printf(
                    "Trace! FD(%04x%04x) program change chan=%d mr->pgiv=%d",
                    fd.zi[0],fd.zi[1],mr->chan,mr->pgiv);
                    printf(" dumy_code=0xff\n");
                }
                putcode_mec(&fd);
                pc->bf = 1;
                /* pc->af[i] = 0; */
                return(0);
            }
        }
    }
    return(0);
}

int mpvgate_mec(char *inf)
//char *inf;
{
    int  i,j,n;
    int  cycl;
    int  start;
    int  sclp;
    int  pf;

    if(mg->debug == 8) printf("mpvgate_() inf=%s cp->key=%d mr->ksin=%d\n",
        inf,cp->key,mr->ksin);
    /* printf("%s",mg->card); */
    if(mg->impv) return(0);   /* Ignore Multi channel Pure Scale */
    opencp_mec(inf);
    setlvl_cps();
    pf = mpvset_mec();
    if(!pf) {
        mp->mpvf = 0;
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl) {
            printf("Error! mpv $M: data\n");
        }
        return(-1);  /* No $M: param */
    }
    return(0);
}

int mpvset_mec()
{
    int  i,j,n;
    int  cycl;
    int  start;
    int  sclp;
    int  pf;

    if(mg->debug == 8) printf("mpvset_() cp->key=%d mr->ksin=%d\n",
        cp->key,mr->ksin);
    /* printf("%s",mg->card); */
    /* get cycle etc.
    $M:Cycle = 12
    $M:Start = Root/Relative
               Keyp  Chan  Pitch.V.
    $M:KCP =  "   1     1       0"
    */
    for(i = 0; i < 128; i++) {
        mp->mpvcp[i] = 0; /* clear mpvcp table */
        mp->mpvcm[i] = 0; /* clear mpvcm table */
    }
    cycl = 0;
    start = 0;
    i = 0;
    n = 0;
    pf = 0;
    while(1) {
        if(cp->l[i][0] == 'C' && cp->l[i][1] == 'y') { /* Cycle */
            cycl = mpvcycl_mec(cp->r[i]);
        }
        if(cp->l[i][0] == 'S' && cp->l[i][1] == 't') { /* Start_Position */
            start = mpvstart_mec(cp->r[i]);
        }
        if(cp->l[i][1] == 'C' && cp->l[i][2] == 'P') {  /* SCP ? */
            mpvitw_mec(n,cp->r[i]);  /* set itw[] table */
            n++;
            pf = 1;
        }
        i++;
        if(i >= cp->n) break;
    }
    if(!pf) {
        return(-1);  /* No $M: param */
    }
    /* set mpvch[] mpvsp[] */
    /* set mpvcm[] mpvsm[] */
    if(cycl) {  /* cycle != 0 */
        i = 0;
        if(start > 0 && start < 12) j = start;
        else j = 0;
        while(1) {
            /* mp->mpvch[] */
            mp->mpvch[j] = itw[1][i];
            mp->mpvsp[j] = itw[2][i];
            mp->mpvsm[j] = itw[3][i];
            i++;
            if(i >= cycl) i = 0;
            j++;
            if(j > 127) break;
        }
    }
    else {
        for(i = 0; i < n; i++) {
            sclp = itw[0][i];
            if(sclp > 0 && sclp < 127) {
                mp->mpvch[sclp] = itw[1][i];
                mp->mpvsp[sclp] = itw[2][i];
                mp->mpvsm[sclp] = itw[3][i];
            }
        }
    }
    if(mg->debug == 8) {
        for(i = 0; i < 127; i++) {
            printf("sclp = %3d  mp->mpvch = %2d  mpvsp = %d  mpvsm = %d\n",
                i,mp->mpvch[i],mp->mpvsp[i],mp->mpvsm[i]);
        }
    }
    return(pf);
}

int mpvcycl_mec(char *val)
//char *val;
{
    int cycl;

    sscanf(val,"%d",&cycl);
    if(mg->debug == 8) printf("mpvcycl_() val = %s cycl = %d\n",val,cycl);
    return(cycl);
}

int mpvstart_mec(char *val)
//char *val;
{
    int  start;
    int  pf;

    pf = 0;
    if(val[0] == 'R' && val[1] == 'e') {  /* Relative ? */
        start = -1;
        if(cp->key == 1) {  /* Major */
            start = majroot_mec();
        }
        if(cp->key == 2) {  /* minor */
            start = 3 + minroot_mec();
        }
        pf = 1;
    }
    if(val[0] == 'R' && val[1] == 'o') {  /* Root ? */
        start = -1;
        if(cp->key == 1) {  /* Major */
            start = majroot_mec();
        }
        if(cp->key == 2) {  /* minor */
            start = minroot_mec();
        }
        pf = 1;
    }
    if(!pf) sscanf(val,"%d",&start);
    if(mg->debug == 8) printf("mpvstart_() val = %s start = %d\n",val,start);

    return(start);
}

/*
  cp->key = 1 Major
mr->ksin =  0 : C     C_Major
mr->ksin =  1 : G     G_Major  #
mr->ksin =  2 : D     D_Major  ##
mr->ksin =  3 : A     A_Major  ###
mr->ksin =  4 : E     E_Major  ####
mr->ksin =  5 : H     B_Major  #####
mr->ksin =  6 : Fis   F#_Major ######
mr->ksin =  7 : Cis   C#_Major #######
mr->ksin =  8 : F     F_Major  &
mr->ksin =  9 : B     B&_Major &&
mr->ksin = 10 : Es    E&_Major &&&
mr->ksin = 11 : As    A&_Major &&&&
mr->ksin = 12 : Des   D&_Major &&&&&
mr->ksin = 13 : Ges   G&_Major &&&&&&
mr->ksin = 14 : Ces   C&_Major &&&&&&&
*/
int majroot_mec()
{
    int  root;

    /* printf("mr->ksin = %d\n",mr->ksin); */
    switch(mr->ksin) {
        case  0 : root = 0;
                  break;
        case  1 : root = 7;
                  break;
        case  2 : root = 2;
                  break;
        case  3 : root = 9;
                  break;
        case  4 : root = 4;
                  break;
        case  5 : root = 11;
                  break;
        case  6 : root = 6;
                  break;
        case  7 : root = 1;
                  break;
        case  8 : root = 5;
                  break;
        case  9 : root = 10;
                  break;
        case 10 : root = 3;
                  break;
        case 11 : root = 8;
                  break;
        case 12 : root = 1;
                  break;
        case 13 : root = 6;
                  break;
        case 14 : root = 11;
                  break;
        default : root = -1;
    }
    return(root);
}

/* 
 cp->key = 2 minor
mr->ksin =  0 : a     A_minor   
mr->ksin =  1 : e     E_minor   #
mr->ksin =  2 : h     B_minor   ##
mr->ksin =  3 : fis   F#_minor  ###
mr->ksin =  4 : cis   C#_minor  ####
mr->ksin =  5 : gis   G#_minor  #####
mr->ksin =  6 : dis   D#_minor  ######
mr->ksin =  7 : ais   A#_minor  #######
mr->ksin =  8 : d     D_minor   &
mr->ksin =  9 : g     G_minor   &&
mr->ksin = 10 : c     C_minor   &&&
mr->ksin = 11 : f     F_minor   &&&&
mr->ksin = 12 : b     B&_minor  &&&&&
mr->ksin = 13 : es    E&_minor  &&&&&&
mr->ksin = 14 : as    A&_minor  &&&&&&&
*/
int minroot_mec()
{
    int  root;

    /* printf("mr->ksin = %d\n",mr->ksin); */
    switch(mr->ksin) {
        case  0 : root = 9;
                  break;
        case  1 : root = 4;
                  break;
        case  2 : root = 11;
                  break;
        case  3 : root = 6;
                  break;
        case  4 : root = 1;
                  break;
        case  5 : root = 8;
                  break;
        case  6 : root = 3;
                  break;
        case  7 : root = 10;
                  break;
        case  8 : root = 2;
                  break;
        case  9 : root = 7;
                  break;
        case 10 : root = 0;
                  break;
        case 11 : root = 5;
                  break;
        case 12 : root = 10;
                  break;
        case 13 : root = 3;
                  break;
        case 14 : root = 8;
                  break;
        default : root = -1;
    }
    return(root);
}   

int mpvitw_mec(int n,char *val)
//int  n;
//char *val;
{
    int  i,j;
    int  sclp;
    int  chan;
    int  pv;
    int  mod;
    char wk[12];

    /* if(mg->debug == 8) printf("mpvitw_() val = %s\n",val); */
    sscanf(val,"%d %d %d %d",&sclp,&chan,&pv,&mod);
    if(mg->debug == 8) {
        printf("mpvitw_() val=%s",val);
        printf("sclp=%3d  chan=%2d  pv=%d  mod=%d\n",
            sclp,chan,pv,mod);
    }
    itw[0][n] = sclp;
    itw[1][n] = chan;
    itw[2][n] = pv;
    itw[3][n] = mod;
    return(0);
}

/* $x: Set Level */
int setlvl_cps()
{   
    int  i,j;
    char lvc;
    char skey[MAXLVL]; 
        
    /* printf("%s\n",mg->card); */
    i = 3;  
    if(mg->card[0] == '{' || mg->card[0] == '(') {
        i = 4;  
    }       
    lvc = '\0';
    if(i == 4) {
        lvc = mg->card[2];
    }
    else {
        lvc = mg->card[1];
    }
    j = 0;
    while(1) {
        if(mg->card[i] == '\0') break;
        if(mg->card[i] == '\r') break;
        if(mg->card[i] == '\n') break;
        if(mg->card[i] == ' ') break;
        if(mg->card[i] == '}') break;
        if(mg->card[i] == ')') break;
        skey[j] = tolower(mg->card[i]);
        j++;
        if(j >= MAXLVL) break;
        i++;
        if(i > 80) break;
    }
    skey[j] = '\0';
    if(mg->debug == 8) printf("lvc = %c skey = %s\n",lvc,skey);
    if(lvc == 'M') setlvm_cps(skey);
    return(0);
}   

int setlvm_cps(char skey[MAXLVL])
//char skey[MAXLVL];
{   
    int  i,k;     
    int  af; 
    int  sf;
    int  ef;      
    int  ac; 
    int  rtn;
    char line[82];
    
    af = 0; 
    ac = 0;
    k = 0;
    if(mg->debug == 8) {
        printf("setlvm_cps() skey=%s ",skey);
        if(cp->key == 1) printf("cp->jky=%d\n",cp->jky);
        if(cp->key == 2) printf("cp->iky=%d\n",cp->iky);
    }
    mp->mpvf = 0;
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]);
        if(cp->key == 2) strcpy(line,irule[k]);
        k++; 
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        /* $M: Temperament */
        ef = 0;
        sf = 0;
        if(line[0] == '$' && line[1] == 'M' && line[2] == ':') sf = 1;
        if(sf) {
            /* printf("%s",line); */
            i = 3;
            while(1) {
                if(line[i] == '=') {
                    ef = 1;
                    break;
                }
                if(line[i] == '\0') break;
                if(line[i] == '\r') break;
                if(line[i] == '\n') break;
                i++;
                if(i > 80) break; 
            }
            if(!ef) {
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) cp->n = 0;
                    setmval_cps(line);    /* set variable level M */
                    ac++;
                    mp->mpvf = 1;
                }
            }
        }
    }
    return(0);
}

int setmval_cps(char line[82])
//char line[82];
{  
    int  i,j;
    int  af;
    char sl[MAXLVL];
    char sr[MAXLVR];
   
    i = 3;  
    j = 0;  
    if(line[0] == '$' && line[2] != ':') i = 1;
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
        if(line[i] != '\'' && line[i] != '\"') sr[j++] = line[i];
        i++;    
        if(i > 80) break;
        if(j >= MAXLVR) break;
    }
    sr[j] = '\0'; 
    /* printf("sl = %s sr = %s\n",sl,sr); */
    strcpy(cp->l[cp->n],sl);
    strcpy(cp->r[cp->n],sr);
    if(mg->debug == 8) { 
        printf("n=%d %s %s\n",
            cp->n,cp->l[cp->n],cp->r[cp->n]);
    }
    cp->n++;
    if(cp->n > MAXLVA) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl) {
            printf("Error! mpv rule argument over than %d\n",MAXLVA);
        }       
        cp->n--;
    }
    return(0);
}  

int linechk_cps(char line[82],char skey[MAXLVL])
//char line[82];
//char skey[MAXLVL];
{
    int  i,j;
    int  n;
    char tkey[MAXLVL];

    i = 3;
    j = 0;
    tkey[0] = (char)islower(line[3]);
    tkey[1] = (char)islower(line[4]);
    tkey[2] = (char)islower(line[5]);
    tkey[3] = '\0';
    if(!strcmp(tkey,"end")) return(0);  /* END */
    beauspace_meb(line);
    n = strlen(line);
    /* 1 */
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf("linechk skey = %s tkey = %s",skey,tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 2 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 3 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 4 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 5 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 6 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 7 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 8 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    /* 9 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mg->debug == 8) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mg->debug == 8) printf("\n");
        return(1);  /* Match */
    }
    if(mg->debug == 8) printf("\n");
    return(0);
}
 
int opencp_mec(char *inf)
//char *inf;
{   
    int  i,j;
    FILE *fcp; 
    char cfn[STLN];
    char cmf[STLN];
    char line[82]; 
    char tkey[MAXLVL];
    char scrf[100];
    
    /*
    if(cp->key == 1 && cp->jky) return(0);
    if(cp->key == 2 && cp->iky) return(0);
    */
    if(cp->key != 1 && cp->key != 2) return(0);
    if(cp->key == 1) cp->jky = 0;
    if(cp->key == 2) cp->iky = 0;
    i = 0;
    while(1) {
        if(inf[i] == '_') break;
        if(inf[i] == '\0') break;
        if(inf[i] == '\r') break;
        if(inf[i] == '\n') break;
        cfn[i] = inf[i];
        i++;
    }       
    cfn[i] = '\0';
    strcpy(cmf,cfn);
    if(cp->key == 1) { 
        strcat(cfn,"cpj.dat");
        strcpy(cmf,"mebcpj.dat");
    }
    if(cp->key == 2) {
        strcat(cfn,"cpi.dat");
        strcpy(cmf,"mebcpi.dat");
    }
    if(mg->debug == 8) printf("cp->key = %d inf = %s cfn = %s cmf = %s\n",
        cp->key,inf,cfn,cmf);
    if((fcp = fopen(cfn,"r")) == NULL) {
        if((fcp = fopen(cmf,"r")) == NULL) { /* mebcpj.dat or mebcpi.dat */
            sprintf(scrf,"%s",TACTDIR);
            //sprintf(scrf,"%s\0",TACTDIR);
            strcat(scrf,cmf);
            /* strcpy(scrf,"/usr/local/etc/tact/"); */
            if(mg->debug == 8) printf("scrf = %s\n",scrf);
            if((fcp = fopen(scrf,"r")) == NULL) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl) {
                    printf("Error! Can't open %s mode r\n",scrf);
                }
                return(-1);
            }
        }
    }
    if(mg->debug == 8) printf("cp->key = %d\n",cp->key);
    if(cp->key == 1) {
        while(fgets(jrule[cp->jky], 82, fcp)) { /* Major */
            /* if(mg->debug == 8) printf("%s",jrule[cp->jky]); */
            cp->jky++;
            if(cp->jky > MAXRL) break;
        }
        if(mg->debug == 8) printf("opencp_mec() cp->jky=%d\n",cp->jky);
    }
    if(cp->key == 2) {
        while(fgets(irule[cp->iky], 82, fcp)) { /* minor */
            /* if(mg->debug == 8) printf("%s",irule[cp->iky]); */
            cp->iky++;
            if(cp->iky > MAXRL) break;
        }
        if(mg->debug == 8) printf("opencp_mec() cp->iky=%d\n",cp->iky);
    }
    fclose(fcp);
    return(0);
}   

int beauspace_meb(char card[82])
//char card[82];
{   
    int  i,k;
    int  wfg;
    char wk[80];
    
    wfg = 0;
    k = 0;
    for(i = 0; i < 79; i++) {
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

/* mpv channel & pitch,modulation change */
int mpvchan_mec(unsigned char z0,unsigned char *zr,unsigned char z2,unsigned char z3)
//unsigned char z0,*zr,z2,z3;
{
    unsigned char z,z1;
    unsigned char c1,c2;
    unsigned char x0,x1,x2,x3;
    int  k;
    int chan;

    if(z0 != 0xfd) return(0);
    z1 = *zr;
    c1 = z1 & 0xf0;
    /* multi channel out 8x:note off  9x:Note on ax:Polyphonic key pressure */
    if(c1 == 0x80 || c1 == 0x90 || c1 == 0xa0) {
        /* Channel change mp->mpvch */
        k = z2;
        chan = mp->mpvch[k] - 1;
        c2 = chan & 0x0f;
        z = c1 | c2; 
        *zr = z;
        /* Pitch change mp->mpvsp mp->mpvcp */
        putpitv_mec(k,chan);
        putmodr_mec(k,chan);
        if(mg->trace) {
            printf("Trace!mpv(%02x%02x%02x%02x) note on/off",
                z0,z,z2,z3);
            printf(" channel chg %d->%d mr->tcnt=%ld\n",
                z1&0x0f,chan&0x0f,mr->tcnt);
        }
    }
    return(0);
}

int putpitv_mec(int k,int chan)
//int  k;
//int  chan;
{
    int  j;
    union meccode fd;
    int  spv;
    int  cpv;
    int  pitv;
    int  msb;
    int  lsb;

    spv = mp->mpvsp[k];
    cpv = mp->mpvcp[k];
    // pitv = mr->pitv + spv; 2013.10.22
    pitv = spv;
    if(pitv == cpv) return(0);
    fd.zi[0] = 0xfde0;
    fd.zi[1] = 0;
    chan &= 0x000f;
    fd.zi[0] |= chan;
    lsb = pitv + 8192;
    lsb &= 0x007f;
    j = lsb << 7;  /* 2012.10.16 */
    j &= 0x7f00;
    fd.zi[1] |= j;
    msb = pitv + 8192;    
    msb = msb >> 7;
    msb &= 0x007f;
    fd.zi[1] |= msb;
    /* 2012.10.16 test ng */
    /*
    lsb = pitv + 8192;
    lsb &= 0x007f;
    fd.zi[1] |= lsb;
    msb = pitv + 8192;    
    msb = msb >> 7;
    msb &= 0x007f;
    j = msb << 7;
    fd.zi[1] |= j;
    */
    /*
    if(mg->debug == 1) {
        printf("debug! putpitv_() spv+8192 = %d mr->pitv = %d lsb=%d msb=%d\n",
        j+lsb,mr->pitv,lsb,msb);
    }
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) pitch vend chan=%d",
            fd.zi[0],fd.zi[1],chan);
        printf(" pitv(%d)=mr->pitv(%d)+spv(%d)\n",pitv,mr->pitv,spv);
    }
    */
    // 2013.10.22
    if(mg->debug == 1) {
        printf("debug! putpitv_() spv+8192 = %d lsb=%d msb=%d\n",
            j+lsb,lsb,msb);
    }
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) pitch vend chan=%d",
            fd.zi[0],fd.zi[1],chan);
        printf(" pitv(%d)=spv(%d)\n",pitv,spv);
    }
    mp->mpvcp[k] = pitv; /* save current pitv */
    putcode_mec(&fd);
    return(0);
}

int putmodr_mec(int k,int chan)
//int  k;
//int  chan;
{   
    int  j;
    union meccode fd;
    int  smd;
    int  cmd; 
    int  modr;
    int  contnmb;
    
    smd = mp->mpvsm[k];
    cmd = mp->mpvcm[k];
    modr = mr->mitv + smd;
    if(modr == cmd) return(0);
    fd.zi[0] = 0xfdb0;
    fd.zi[1] = 0;
    chan &= 0x000f;
    fd.zi[0] |= chan;
    contnmb = 1;            /* modulation */
    contnmb &= 0x007f;
    j = contnmb << 8;
    fd.zi[1] |= j;
    modr &= 0x007f;
    fd.zi[1] |= modr;
    if(mg->debug == 1) printf("debug! putmodr_() mr->mitv = %d\n",mr->mitv);
    if(mg->trace) {
        printf("Trace! FD(%04x%04x) modulation chan=%d modr=%d",
        fd.zi[0],fd.zi[1],chan,modr);
        printf(" modr(%d)=mr->mitv(%d)+smd(%d)\n",modr,mr->mitv,smd);
    }
    mp->mpvcm[k] = modr; /* save current modulation */
    putcode_mec(&fd);
    return(0);
}

