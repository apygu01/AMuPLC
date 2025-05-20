
// ----------------------
//  AMuPLC (meb11.c)
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
extern MEBEDT *ge;
extern MEBSRG *ls;
extern MECVAR *mr;
extern MEBCPS *cp;
extern MEBCSL *cs;
extern MEBANS *ma;
extern MEBVEX *vx;
extern MEBART *ar;
extern char *prtsym;
extern char scard[MSTC][82];
extern MEBGRT *gr,*gd,*ga,*gm,*gs,*gc;

/* main modify smpte */
int modsmpte_meb(char *inf,struct mivksn *mk,struct mivsym ms[])
//char *inf;
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i,j,k;
    FILE *fpi;
    char c1;
    int  dxf;

    if(mv->debug == 12) printf("modsmpte_() inf = %s\n",inf);
    mv->line = 0;
    mb->bnum = 0;
    mb->xbnum = 0;
    i = 0;
    mopen_meb();
    /* while(fgets(mv->card, 82, fpi)) { */
    while(mgets_meb(mv->card)) { // 2025.4.2  
        for(i = 0; i < 82; i++) {
            if(mv->card[i] == '\n' || mv->card[i] == '\r') { 
                mv->card[i] = '\0';
                break;      
            }           
        }               
        mv->line++;
        switch (mv->card[0]) {  
            case '-'  : /* bar */ 
                        mainbar_meb();
                        setbnum_meb();
                        break;
            case 's'  : /* beaumset_meb(); */
                        notesync_meb();
                        break;
            case 'c'  : /* beaumset_meb(); */
                        break;
            case '*'  : /* Vertical modify */
                        break;
            case '('  : /* local */
                        break;
            case '{'  : /* Global */
                        if(mv->card[1] == '$') {
                            beaugkey_meb(mk); 
                            break;
                        }   
                        if(mv->card[1] == 'm' && mv->card[2] == '.' && 
                         mv->card[3] == 'm') {
                             exptmp_meb();         /* {m.m =/ = xx } */
                        }
                        break;
            case 'S'  : if(mv->card[1] == 'm') {
                            smptsval_meb();
                            smptgval_meb();
                            if(mb->clrsml) mb->prflg = 0;
                        }
                        break;
            case '!'  : /* chord */
                        break;
            case '?'  : /* chord select */
                        break; 
            case '$'  : /* $ line */
                        if(mv->card[2] == '$') break;
                        beaukey_meb(mk);
                        key_meb();
                        setkey_cps();
                        break;  
            case '='  : /* ===== */
                        if(mv->card[1] == '=') {
                            smpteset_meb(3);
                        }   
                        break;
            default   : break;
        }
        if(mv->source == 4) printf("%s\n",mv->card);
        mprintf_meb(mv->card);
    }
    mcopy_meb(inf);
    return(0);
}

int finstep_meb(char *inf,struct mivksn *mk,struct mivsym ms[])
//char *inf;
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i;
    int  pf;
    FILE *fpi;
    int  osp;    /* (original) shift start point */
    int  oep;    /* (original) shift end point */
    int  tsp;    /* (terget) shift start point */
    int  tep;    /* (terget) shift end point */

    osp = 11;
    oep = 69;
    if( mb->bartyp == 's' ||
        mb->bartyp == 't' ||
        mb->bartyp == 'g' ||
        mb->bartyp == 'a' ||
        mb->bartyp == 'u' ||
        mb->bartyp == 'b') {
        osp = 32;
    }
    if( mb->bartyp == 'l' ||
        mb->bartyp == 'm' ||
        mb->bartyp == 'h') {
        osp = 11;
    }
    tsp = osp;
    tep = oep;
    ma->nvf = 0;
    ma->cvpf = 0;
    ma->scvf = 0;  /* add 2007.4.14 */
    cp->scnvf = 0;
    mv->line = 0;
    if(mv->debug == 12) printf("finstep_meb() mv->step=%d cp->scnvf=%d\n",
        mv->step,cp->scnvf);
    strcpy(ma->cdinst,"+Default");
    mopen_meb(); /* finstep */
    /* while(fgets(mv->card, 82, fpi)) { */
    //while(mgets_meb(mv->card, 82, fpi)) {
    while(mgets_meb(mv->card)) {  // 2025.4.2
        for(i = 0; i < 82; i++) {
            if(mv->card[i] == '\n' || mv->card[i] == '\r') {
                mv->card[i] = '\0';
                break;
            }
        } 
        mv->line++;
        switch (mv->card[0]) {
             case '-'  : /* bar line */
                         mb->bartyp = mv->card[1];
                         break;
             case '%'  : /* Note line */
                         if(mb->nsftf) {
                             lnshift_meb(osp,oep,tsp,tep,mb->nsftv);   
                         }
                         creen_meb();
                         beaumod_meb();
                         if(mb->clrntsf) clrnotes_meb();
                         if(mv->card[4] == '[') {
                             cldexp_meb();
                         }
                         /* if(cp->scnvf == 3) selcnv_meb(); */
                         if(ma->scvf) selcnv_meb();
                         if(mb->gaterf) gr_rp_set(); // 2014.1.22
                         // gr_rp_set();
                         break;
             case 'R'  : /* Rest line */
                         if(mb->nsftf) {
                             lnshift_meb(osp,oep,tsp,tep,mb->nsftv);   
                         }
                         break;
             case 'r'  : /* rest line */
                         if(mb->nsftf) {
                             lnshift_meb(osp,oep,tsp,tep,mb->nsftv);   
                         }
                         break;
             case '+'  : /* + Note line */
                         if(mb->nsftf) {
                             lnshift_meb(osp,oep,tsp,tep,mb->nsftv);   
                         }
                         break;
             case '{'  : /* { line ? */
                         /* printf("{} cp->scnvf=%d\n",cp->scnvf); */
                         if(cp->scnvf == 4) {
                             if(mv->card[1] == '$') {
                               getnkey_cnv();  /* get key name to ma->nkey */
                               setskey_cnv();  /* set ma->skey (pattern) */
                               setlvN_cnv();   /* set $N:pattern value */
                             }
                             if(mv->card[1] == '!') gCname_meb();
                             break;
                         }
                         beauexp_meb();
                         break;
             case '>'  : /* { line ? */
                         finexp_meb();
                         break;
             case '#'  : /* # line */
                         finexp_meb();
                         break;
             case '$'  : /* $ line */
                         if(mv->card[2] == '$') serial_meb(mv->card);
                         if(mv->card[1] == 'N' && mv->card[2] == ':' ) {
                             setinst_cnv();  /* Chord_Na._Vo */
                             setskey_cnv();
                             setlvN_cnv();
                         }
                         break;
             default   : break;
        }
         pf = 1;
        if(mb->hcpl && mv->card[0] == '{' && mv->card[1] == '!') pf = 0;
        if(pf) {
            if(mv->source == 4) printf("%s\n",mv->card);
            mprintf_meb(mv->card);
        }
    }
    mcopy_meb(inf);
    if(mv->debug == 16) printf("//meb11.c/ PASS-25-c///////////\n");
    // comment out 2025.4.2 core dump why ?
    if(ls->fmxopend) {
        // fclose(ls->fmx);
    }
    /*
    if(ls->fobopend) {   
        fclose(ls->fob);
        ls->fobopend = 0;
    }
    */
    return(0);
}

int finexp_meb()
{
    int  i,j;
    int  rtn;
    int  sp;
    char cw[82];
        
    sp = 0;
    if(mv->card[0] == '>') sp = 1;
    for(i = 0; i < 80; i++) {
        cw[i] = tolower(mv->card[sp]);
        if(mv->card[sp] == '}') break;
        sp++;
    }
    cw[++i] = '\0';

    /*
    rtn = beauexp_sub1(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub2(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub3(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub4(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub5(cw);
    if(!rtn) return(0);
    */

    rtn = beauexp_sub3(cw);
    if(!rtn) return(0);
    if(mv->card[0] == '#') {
        /* printf("finexp_meb() pass******?\n"); */
        rtn = strck_meb();
        return(rtn);
    }
    return(rtn);
}

/* Main(Conducter) Decode expand lines */
int mdecdxp_meb()
{
    int  i,j;
    int  pf;
    char dxs[4];
    char wk[10];
    char rw[36][24];
    int  rwv[36];
    char brw[36][24];
    int  brwv[36];
    int  pgate;
    int  rtn;
    int  rsum;
    int  rv;
    int  nc;
    float dnv;
    long nval[36];
    long nadd;
    long nv;
    int  adj;
   
    pf = 0;
    if(mv->debug == 12) printf("mdecdxp_() mb->rbtch = %c\n",mb->rbtch);
    if(mv->debug == 12) printf("%s\n",mv->card);
    ar->pmf = 0;
    if(mv->card[33] == '+') ar->pmf = 0;
    if(mv->card[33] == '-') ar->pmf = -1;
    dxs[0] = mv->card[34];
    dxs[1] = mv->card[35];
    dxs[2] = mv->card[36];
    dxs[3] = '\0';
    rtn = setartv_meb(dxs);
    if(!rtn) {
        wk[0] = mv->card[1];
        wk[1] = mv->card[2];
        wk[2] = '\0';
        sscanf(wk,"%d",&pgate);
        if(mv->debug == 12) printf("Debug12 pgate = %d mb->nval = %ld\n",
            pgate,mb->nval);
        i = 0;
        j = 0;
        rsum = 0;
        nc = 0;
        while(1) {
            if(ar->crt[i] == '\0') break;
            rw[nc][j] = ar->crt[i];
            if(ar->crt[i] == ' ') {
                rw[nc][j] = '\0';
                sscanf(rw[nc],"%d",&rv);
                rsum = rsum + rv;
                rwv[nc] = rv;
                if(mv->debug == 12) printf("rw[%d] = %s rv = %d\n",
                    nc,rw[nc],rv);
                nc++;
                if(nc > 36) break;
                j = -1;
            }
            j++;
            if(j > 24) break;
            i++;
            if(i > MAXPTN) break;
        }
        dnv = (float)mb->nval / (float)rsum;
        if(ar->pmf < 0) {
            j = nc - 1;
            for(i = 0 ; i < nc; i++) {
                strcpy(brw[j],rw[i]);
                brwv[j] = rwv[i];
                j--;
                if(j < 0) break;
            }
            for(i = 0 ; i < nc; i++) {
                strcpy(rw[i],brw[i]);
                rwv[i] = brwv[i];
                if(mv->debug == 12) printf("rw[%d] = %s rwv[%d] = %d\n",
                    i,rw[i],i,rwv[i]);
            }
        }
        if(mv->debug == 12) printf("nc = %d rsum = %d dnv = %f\n",nc,rsum,dnv);
        nadd = 0;
        for(i = 0; i < nc; i++) {
            nval[i] = rwv[i] * dnv;
            nadd = nadd + nval[i];
            if(mv->debug == 12)
                printf("nadd =%7ld nval[%d] =%7ld rwv[%d] = %d\n",
                nadd,i,nval[i],i,rwv[i]);
        }
        adj = mb->nval - nadd;
        if(mv->debug == 12) printf("adj = %d\n",adj);
        nval[0] = nval[0] + adj; /* adjust sync */
        for(i = 0; i < nc; i++) {
            nv = nval[i];
            if( 700000 < nv && nv <= 999999) {
                adj = nv - 800000;
                mdecnote_("|O|",adj,i);
            }
            if( 450000 < nv && nv <= 700000) {
                adj = nv - 600000;
                mdecnote_("O.",adj,i);
            }
            if( 350000 < nv && nv <= 450000) {
                adj = nv - 400000;
                mdecnote_("O",adj,i); 
            }
            if( 250000 < nv && nv <= 350000) {
                adj = nv - 300000; 
                mdecnote_("o/.",adj,i);
            }   
            if( 180000 < nv && nv <= 250000) {
                adj = nv - 200000;
                mdecnote_("o/",adj,i);
            }   
            if( 130000 < nv && nv <= 180000) {
                adj = nv - 150000;
                mdecnote_("_/.",adj,i);
            }
            if( 75000 < nv && nv <= 130000) {
                adj = nv - 100000;
                mdecnote_("_/",adj,i);
            }
            if( 0 < nv && nv <= 75000) {
                adj = nv - 50000;
                switch(adj) {
                    case  25000 : mdecnote_("_/).",0,i);
                                  break;
                    case -12500 : mdecnote_("_/)).",0,i);
                                  break;
                    case -25000 : mdecnote_("_/))",0,i);
                                  break;
                    case -28125 : mdecnote_("_/)))..",0,i);
                                  break;
                    case -31250 : mdecnote_("_/))).",0,i);
                                  break;
                    case -37500 : mdecnote_("_/))))",0,i);
                                  break;
                    default     : mdecnote_("_/)",adj,i);
                                  break;
                }
            } 
        }
        pf = 1;
    }
    if(!pf) {
        if(mv->source == 3) printf("%s\n",mv->card);
        mprintf_meb(mv->card);
    }
    return(0);
}

/* Decode Dynamic expand note lines */
int decdxp_meb()
{  
    int  i,j;
    int  pf;
    char dxs[4];
    char wk[10];
    char rw[36][24];
    int  rwv[36];
    char brw[36][24];
    int  brwv[36];
    int  pgate;
    int  rtn;
    int  rsum;
    int  rv;
    int  nc;
    float dnv;
    long nval[36];
    long nadd;
    long nv;
    int  adj;
   
    pf = 0;
    if(mv->debug == 12) printf("%s\n",mv->card);
    ar->pmf = 0;
    if(mv->card[75] == '+') ar->pmf = 0;
    if(mv->card[75] == '-') ar->pmf = -1;
    dxs[0] = mv->card[76];
    dxs[1] = mv->card[77];
    dxs[2] = mv->card[78];
    dxs[3] = '\0';
    rtn = setartv_meb(dxs);
    if(!rtn) {
        wk[0] = mv->card[1];
        wk[1] = mv->card[2];
        wk[2] = '\0';
        sscanf(wk,"%d",&pgate);
        if(mv->debug == 12) {
            printf("Debug12 decdxp_() pgate = %d mb->nval = %ld\n",
            pgate,mb->nval);
        }
        i = 0;
        j = 0;
        rsum = 0;
        nc = 0;
        /* if(mv->debug == 12) printf("ar->crt=%s\n",ar->crt); */
        while(1) {
            if(ar->crt[i] == '\0') break;
            rw[nc][j] = ar->crt[i];
            if(ar->crt[i] == ' ') {
                rw[nc][j] = '\0';
                sscanf(rw[nc],"%d",&rv);
                rsum = rsum + rv;
                rwv[nc] = rv;
                if(mv->debug == 12) printf("rw[%d] = %s rv = %d\n",
                    nc,rw[nc],rv);
                nc++;
                if(nc > 36) break;
                j = -1;
            }
            j++;
            if(j > 24) break;
            i++;
            if(i > MAXPTN) break;
        }
        dnv = (float)mb->nval / (float)rsum;
        if(ar->pmf < 0) {
            j = nc - 1;
            for(i = 0 ; i < nc; i++) {
                strcpy(brw[j],rw[i]);
                brwv[j] = rwv[i];
                j--;
                if(j < 0) break;
            }
            for(i = 0 ; i < nc; i++) {
                strcpy(rw[i],brw[i]);
                rwv[i] = brwv[i];
                if(mv->debug == 12) printf("rw[%d] = %s rwv[%d] = %d\n",
                    i,rw[i],i,rwv[i]);
            }
        }
        if(mv->debug == 12) printf("nc = %d rsum = %d dnv = %f\n",nc,rsum,dnv);
        nadd = 0;
        for(i = 0; i < nc; i++) {
            nval[i] = rwv[i] * dnv;
            nadd = nadd + nval[i];
            if(mv->debug == 12)
                printf("nadd =%7ld nval[%d] =%7ld rwv[%d] = %d\n",
                nadd,i,nval[i],i,rwv[i]);
        }
        adj = mb->nval - nadd;
        if(mv->debug == 12) printf("adj = %d\n",adj);
        nval[0] = nval[0] + adj; /* adjust sync */
        for(i = 0; i < nc; i++) {
            nv = nval[i];
            /*
            if( 700000 < nv && nv <= 999999) {
                adj = nv - 800000;
                decnote_("|O|",adj,i);
            }
            if( 450000 < nv && nv <= 700000) {
                adj = nv - 600000;
                decnote_("O.",adj,i);
            }
            */
            if( 350000 < nv && nv <= 9999999) {
                adj = nv - 400000;
                decnote_("O",adj,i); 
            }
            if( 250000 < nv && nv <= 350000) {
                adj = nv - 300000; 
                decnote_("o/.",adj,i);
            }   
            if( 180000 < nv && nv <= 250000) {
                adj = nv - 200000;
                decnote_("o/",adj,i);
            }   
            if( 130000 < nv && nv <= 180000) {
                adj = nv - 150000;
                decnote_("_/.",adj,i);
            }
            if( 75000 < nv && nv <= 130000) {
                adj = nv - 100000;
                decnote_("_/",adj,i);
            }
            if( 0 < nv && nv <= 75000) {
                adj = nv - 50000;
                switch(adj) {
                    case  25000 : decnote_("_/).",0,i);
                                  break;
                    case -12500 : decnote_("_/)).",0,i);
                                  break;
                    case -25000 : decnote_("_/))",0,i);
                                  break;
                    case -28125 : decnote_("_/)))..",0,i);
                                  break;
                    case -31250 : decnote_("_/))).",0,i);
                                  break;
                    case -37500 : decnote_("_/))))",0,i);
                                  break;
                    default     : decnote_("_/)",adj,i);
                                  break;
                }
            } 
            /*
            if( 13400 < nv && nv <= 32000) {
                adj = nv - 25000;
                decnote_("_/))",adj,i);
            } 
            if( 0 < nv && nv <= 13400) {
                adj = nv - 12500;
                decnote_("_/))",adj,i);
            } 
            */
        }
        pf = 1;
    }
    if(!pf) {
        if(mv->source == 2 || mv->source == 3) printf("%s\n",mv->card);
        mprintf_meb(mv->card);
    }
    return(0);
}  

/*
 12345678901234567890123456789012345678901234567890123456789012 
-c0000000   0____.____1____.____2 Exp  tp  velo   tempo    dymc  hh:mm:ss/ff.ff
*/
int mdecnote_(char *nsym,int adj,int cnp)
//char *nsym;
//int  adj;
//int  cnp;
{
    int  i; 
    char wk[12];
    char line[82];
    float tempo;
    char b;  
    float rb;
    float tp;
    int  rubart;
            
    if(mv->debug == 12) {
        printf("mdecnote_() %s adj = %d mb->bartyp = %c cnp=%d\n",
        nsym,adj,mb->bartyp,cnp);
    }
    wk[0] = mv->card[49];
    wk[1] = mv->card[50];
    wk[2] = mv->card[51];
    wk[3] = mv->card[52];
    wk[4] = mv->card[53];
    wk[5] = mv->card[54];
    wk[6] = '\0';
    sscanf(wk,"%f",&tempo);
    rb = adj / 100000.0;
    tp = 60.0 / tempo;
    rb = rb * tp;
    rubart = 0;
    switch(mb->rbtch) {
        case 'f' : /* mec unit = 1.0;        flame */
                   rb = rb * 30.0;
                   rb = rb + 0.4;
                   break;
        case 'q' : /* mec unit = 1.0/4.0;    MTC */
                   rb = rb * 30.0;
                   rb = rb * 4.0;
                   rb = rb + 0.4;
                   break;
        case 't' : /* mec unit = 1.0/8.0;    SMPTE Time code */
                   rb = rb * 30.0;
                   rb = rb * 8.0;
                   rb = rb + 0.4;
                   break;
        case 'b' : /* mec unit = 1.0/80.0;   Bit falame */
                   rb = rb * 30.0;
                   rb = rb * 80.0;
                   rb = rb + 0.4;
                   break;
        case 'd' : /* mec unit = 1.0/100.0;  1/100 flame */
                   rb = rb * 30.0;
                   rb = rb * 100.0;
                   rb = rb + 0.4;
                   break;
        case 'm' : /* mec unit  = 30.0/1000.0;  msec */
                   rb = rb * 1000.0;
                   rb = rb + 0.4;
                   break;
        case 's' : /* mec unit = 30.0;       Sec */
                   rb = rb + 0.4;
                   break;
        case 'n' : /* No rubert */
                   rb = 0;
                   break;
        default  : break;
    }
    if(mv->debug == 12) printf("rb = %f wk = %s tempo = %f\n",rb,wk,tempo);
    adj = (int)rb;
    line[0] = mv->card[0];
    line[1] = mv->card[1];
    line[2] = mv->card[2];
    line[3] = mv->card[3];
    line[4] = '\0';
    strcat(line,nsym);
    if(mb->rbtch == 'n') sprintf(wk,"%d",adj);
    else sprintf(wk,"%d%c",adj,mb->rbtch);
    if(adj > 0) strcat(line,"+");
    if(adj) strcat(line,wk);
    strcat(line,"       ");
    if(mb->bartyp == 'c') {
        for(i = 11 ; i < 80; i++) line[i] = mv->card[i];
        for(i = 12; i < 33; i++) line[i] = ' ';
        line[33] = ' ';
        line[34] = '.';
        line[35] = '.';
        line[36] = '.';
        mexpnote_meb(12,line,cnp);
    }
    if(mv->debug == 12) printf("%s\n",line);
    if(mv->source == 3) printf("%s\n",line);
    mprintf_meb(line);
    return(0);
}

/*
 123456789012345678901234567890123456789012345678901234567890123456789012345678
-g0000000 p.v. mod cn:cdt pcg tp2______*______4______5______6______7 velo P_Exp
%80 O        0   0  0:  0 ...  0                                     13.2 .+dex
-m0000002 _-1______0______1______2______*______4______5______6______7velo P_Exp
 123456789012345678901234567890123456789012345678901234567890123456789012345678
-r0000000  tp 3____.____4____.____5____.____6____.____7____.____8____velo P_Exp
%80 _/.-21 30 
    nval = 150000 -21 * 100
*/
int decnote_(char *nsym,int adj,int cnp)
//char *nsym;
//int  adj;
//int  cnp;
{
    int  i;
    char wk[12];
    char line[82];
    char b;
    float rb;      
    float tp;      
    int  swing;   

    if(mv->debug == 12) {
        printf("decnote_() %s adj = %d mb->bartyp = %c ",
        nsym,adj,mb->bartyp);
        printf("mb->rbtch = %c cnp=%d\n",mb->rbtch,cnp);
    }
    rb = adj / 100000.0;
    tp = 60.00 / mv->tempo;
    rb = rb * tp;
    swing = 0;
    switch(mb->rbtch) {
        case 'f' : /* mec unit = 1.0;        flame */
                   rb = rb * 30.0;
                   rb = rb + 0.4;
                   break;
        case 'q' : /* mec unit = 1.0/4.0;    MTC */
                   rb = rb * 30.0;
                   rb = rb * 4.0; 
                   rb = rb + 0.4;
                   break;
        case 't' : /* mec unit = 1.0/8.0;    SMPTE Time code */
                   rb = rb * 30.0;
                   rb = rb * 8.0;
                   rb = rb + 0.4; 
                   break;
        case 'b' : /* mec unit = 1.0/80.0;   Bit falame */
                   rb = rb * 30.0;
                   rb = rb * 80.0;
                   rb = rb + 0.4;
                   break;
        case 'd' : /* mec unit = 1.0/100.0;  1/100 flame */
                   rb = rb * 30.0;
                   rb = rb * 100.0;
                   rb = rb + 0.4;
                   break;
        case 'm' : /* mec unit  = 30.0/1000.0;  msec */
                   rb = rb * 1000.0;
                   rb = rb + 0.4;
                   break;
        case 's' : /* mec unit = 30.0;       Sec */
                   rb = rb + 0.4;
                   break;
        case 'n' : /* No rubert */
                   rb = adj / 100.0; /* nval _/ = 1000 */
                   break;
        default  : break;
    }
    if(mv->debug == 12) {
        printf("%s\n",mv->card);
        printf("rb = %f mv->tempo = %f mb=bartyp=%c\n",
        rb,mv->tempo,mb->bartyp);
    }
    adj = (int)rb;
    line[0] = mv->card[0];
    line[1] = mv->card[1];
    line[2] = mv->card[2];
    line[3] = mv->card[3];
    line[4] = '\0';
    strcat(line,nsym);
    if(mb->rbtch == 'n') sprintf(wk,"%d",adj);
    else sprintf(wk,"%d%c",adj,mb->rbtch);
    if(adj > 0) strcat(line,"+");
    if(adj) strcat(line,wk);
    strcat(line,"       ");
    b = mb->bartyp;
    if(b == 'r') {
        for(i = 11 ; i < 80; i++) line[i] = mv->card[i];
        for(i = 13; i < 68; i++) line[i] = ' ';
        line[75] = '_';
        line[76] = '.';
        line[77] = '.';
        line[78] = '.';
        expnote_meb(13,line,cnp);
    }
    if(b == 't' || b == 'g' || b == 's' || b == 'a' ||
        b == 'b' || b == 'u' || b == 'd') {
        for(i = 11 ; i < 80; i++) line[i] = mv->card[i];
        for(i = 32; i < 68; i++) line[i] = ' ';
        line[75] = '_';
        line[76] = '.';
        line[77] = '.';
        line[78] = '.';
        expnote_meb(32,line,cnp);
    }
    if(b == 'l' || b == 'm' || b == 'h') {
        for(i = 11 ; i < 80; i++) line[i] = mv->card[i];
        for(i = 11; i < 68; i++) line[i] = ' ';
        line[75] = '_';
        line[76] = '.';
        line[77] = '.';
        line[78] = '.';
        expnote_meb(10,line,cnp);
    }
    if(mv->debug == 12) printf("%s\n",line);
    if(mv->source == 3) printf("%s\n",line);
    mprintf_meb(line);
    return(0);
}

int mexpnote_meb(int sp,char line[82],int cnp)  
//int  sp; /* start position */
//char line[82];
//int  cnp;
{   
    int  i,j,k;
    int  nc;
    int  ncc;
    char npv[36][24];
    int  npc[36];  /* Note position count */
    char bnpv[36][24];
    int  bnpc[36]; /* bak Note position count */
    /* */
    int  vc;
    int  vcc;
    char nvt[36][24];
    int  nvc[36];  /* Note velocity position count */
    char bvtv[36][24];
    int  bvtc[36]; /* bak Note velocity position count */
    int  cp;
        
    if(mv->debug == 12) {
        printf("mexpnote_() sp=%d cnp=%d ar->cnt = %s ar->cvt = %s",
        sp,cnp,ar->cnt,ar->cvt);
        printf(" ar->pmf=%d\n",ar->pmf);
    }  
    /* set nvt[] velocity from ar->cvt[] */
    for(i = 0; i < 36; i++) {
        for(j = 0; j < 24; j++) nvt[i][j] = ' ';
    }
    i = 0;
    vcc = 0;
    vc = 0;
    while(1) {
        if(ar->cvt[i] == '\0') break;
        nvt[vc][vcc] = ar->cvt[i];
        if(ar->cvt[i] == ' ') {
            nvc[vc] = vcc;
            /* printf("nvc[%d] = %d\n",vc,nvc[vc]); */
            vc++;
            vcc = -1;
        }
        vcc++;
        i++;
        if( i > MAXPTN) break;
    }
    if(ar->pmf < 0) {  /* -exp reverce */
        j = vc - 1;
        for(i = 0 ; i < vc; i++) {
            bvtc[i] = nvc[j];
            j--;
            if(j < 0) break;
        }
        j = vc - 1;
        for(i = 0 ; i < vc; i++) {
            k = bvtc[i] - 1;
            for(; k >= 0; k--) {
                bvtv[i][k] = nvt[j][k];
                /*
                printf("bvtv[%d][%d] = %c nvt[%d][%d] = %c\n",
                    i,k,bvtv[i][k],j,k,nvt[j][k]);
                */
            }   
            j--;    
            if(j < 0) break;
        }       
        for(i = 0 ; i < nc; i++) {
            for(k = 0; k < bvtc[i]; k++) nvt[i][k] = bvtv[i][k];
            nvc[i] = bvtc[i];
        }
        
    }   
    if(mv->debug == 12) {
        for(i = 0 ; i < vc; i++) {
            for(j = 0; j < nvc[i]; j++) {
                printf("nvc[%d] = %d nvt[%d][%d] = %c\n",
                i,nvc[i],i,j,nvt[i][j]);
            }  
        }
    }
    /* set npv[] note positon from ar->cnt[] */
    i = 0;
    ncc = 0;
    nc = 0;
    while(1) {  
        if(ar->cnt[i] == '\0') break;
        npv[nc][ncc] = ar->cnt[i];
        if(ar->cnt[i] == ' ') {
            npc[nc] = ncc;
            /* printf("npc[%d] = %d\n",nc,npc[nc]); */
            nc++;
            ncc = -1;  
        }
        ncc++;
        i++;
        if( i > MAXPTN) break; 
    }   
    if(ar->pmf < 0) {
        j = nc - 1;
        for(i = 0 ; i < nc; i++) {
            bnpc[i] = npc[j];
            j--;
            if(j < 0) break;
        }
        j = nc - 1; 
        for(i = 0 ; i < nc; i++) {
            k = bnpc[i] - 1;
            for(; k >= 0; k--) {
                bnpv[i][k] = npv[j][k];
                if(mv->debug == 12) {
                    printf("bnpv[%d][%d] = %c npv[%d][%d] = %c\n",
                    i,k,bnpv[i][k],j,k,npv[j][k]);
                }
            }
            j--;
            if(j < 0) break;
        }
        for(i = 0 ; i < nc; i++) {
            for(k = 0; k < bnpc[i]; k++) npv[i][k] = bnpv[i][k];
            npc[i] = bnpc[i];
        } 

    }
    if(mv->debug == 12) {
        printf("----\n");
        for(i = 0 ; i < nc; i++) {
            for(j = 0; j < npc[i]; j++) {
                printf("npc[%d] = %d npv[%d][%d] = %c\n",
                i,npc[i],i,j,npv[i][j]);
            }
        }
    }
    for(j = 0; j < npc[cnp]; j++) {
        cp = -1;
        for(i = sp; i < 33; i++) {
            if(isdigit(mv->card[i])) {
                cp++;
                /*
                if(mv->debug == 12) printf("npv[%d][%d] = %c prtsym[%d] = %c\n",
                    cnp,j,npv[cnp][j],cp,prtsym[cp]);
                */
                /* if(npv[cnp][j] == prtsym[cp]) line[i] = mv->card[i]; */
                /* if(npv[cnp][j] == prtsym[cp]) line[i] = nvt[cnp][j]; */
                if(npv[cnp][j] == prtsym[cp]) {
                    if(nvt[cnp][j] != ' ') line[i] = nvt[cnp][j];
                    else line[i] = mv->card[i];
                }
            }
        }
    }
    return(0);
}

int expnote_meb(int sp,char line[82],int cnp)
//int  sp; /* start position */
//char line[82];
//int  cnp;
{
    int  i,j,k;
    int  nc;
    int  ncc;
    char npv[36][24];
    int  npc[36];  /* Note position count */
    char bnpv[36][24];
    int  bnpc[36]; /* bak Note position count */
    /* */
    int  vc;
    int  vcc;
    char nvt[36][24];
    int  nvc[36];  /* Note velocity position count */
    char bvtv[36][24];
    int  bvtc[36]; /* bak Note velocity position count */
    int  cp;

    if(mv->debug == 12) {
        printf("expnote_() sp=%d cnp=%d ar->cnt = %s ar->cvt = %s",
        sp,cnp,ar->cnt,ar->cvt);
        printf(" ar->pmf=%d\n",ar->pmf); 
        printf("%s\n",line); 
    }   
    /* set nvt[] velocity from ar->cvt[] */
    for(i = 0; i < 36; i++) {
        for(j = 0; j < 24; j++) nvt[i][j] = ' ';
    }
    i = 0;
    vcc = 0;
    vc = 0;
    while(1) {
        if(ar->cvt[i] == '\0') break;
        nvt[vc][vcc] = ar->cvt[i];
        if(mv->debug == 12) printf("nvt[%d][%d] = %c\n",vc,vcc,nvt[vc][vcc]);
        if(ar->cvt[i] == ' ') {
            nvc[vc] = vcc;
            /* if(mv->debug == 12) printf("nvc[%d] = %d\n",vc,nvc[vc]); */
            vc++;   
            vcc = -1;
        }
        vcc++; 
        i++;
        if( i > MAXPTN) break;
    }               
    if(ar->pmf < 0) {  /* -exp reverce */
        j = vc - 1;
        for(i = 0 ; i < vc; i++) {
            bvtc[i] = nvc[j];
            j--;
            if(j < 0) break;
        }
        j = vc - 1;
        for(i = 0 ; i < vc; i++) {
            k = bvtc[i] - 1;
            for(; k >= 0; k--) {
                bvtv[i][k] = nvt[j][k];
                /*
                printf("bvtv[%d][%d] = %c nvt[%d][%d] = %c\n",
                    i,k,bvtv[i][k],j,k,nvt[j][k]);
                */
                if(mv->debug == 12) { 
                    printf("bvtv[%d][%d] = %c nvt[%d][%d] = %c\n",
                        i,k,bvtv[i][k],j,k,nvt[j][k]);
                }
            }
            j--;
            if(j < 0) break;
        }
        for(i = 0 ; i < nc; i++) {
            for(k = 0; k < bvtc[i]; k++) nvt[i][k] = bvtv[i][k];
            nvc[i] = bvtc[i];
        }
    
    }
    if(mv->debug == 12) {
        /* printf("#### Check #### vc = %d\n",vc); */
        for(i = 0 ; i < vc; i++) {
            for(j = 0; j < nvc[i]; j++) {
                printf("nvc[%d] = %d nvt[%d][%d] = %c\n",
                i,nvc[i],i,j,nvt[i][j]);
            }
        }
    }       
    /* set npv[] note positon from ar->cnt[] */
    i = 0;
    ncc = 0;
    nc = 0;
    while(1) {
        if(ar->cnt[i] == '\0') break;
        npv[nc][ncc] = ar->cnt[i];
        if(ar->cnt[i] == ' ') {
            npc[nc] = ncc;
            /* printf("npc[%d] = %d\n",nc,npc[nc]); */
            nc++;
            ncc = -1;
        }
        ncc++;
        i++;
        if( i > MAXPTN) break;
    }
    if(ar->pmf < 0) {  /* reverce */
        j = nc - 1;
        for(i = 0 ; i < nc; i++) {
            bnpc[i] = npc[j];
            j--;
            if(j < 0) break;
        }
        j = nc - 1;  
        for(i = 0 ; i < nc; i++) {
            k = bnpc[i] - 1;
            for(; k >= 0; k--) {
                bnpv[i][k] = npv[j][k];
                /*
                printf("bnpv[%d][%d] = %c npv[%d][%d] = %c\n",
                    i,k,bnpv[i][k],j,k,npv[j][k]);
                */
            }
            j--;
            if(j < 0) break;
        }
        for(i = 0 ; i < nc; i++) {
            for(k = 0; k < bnpc[i]; k++) npv[i][k] = bnpv[i][k];
            npc[i] = bnpc[i];
        }
    
    }
    if(mv->debug == 12) {
        for(i = 0 ; i < nc; i++) {
            for(j = 0; j < npc[i]; j++) {
                printf("npc[%d] = %d npv[%d][%d] = %c\n",
                i,npc[i],i,j,npv[i][j]);
            }
        }
    }
    if(mv->debug == 12) {
        printf("%s\n",mv->card);
        printf("%s\n",line);
    }
    for(j = 0; j < npc[cnp]; j++) {
        cp = -1;
        for(i = sp; i < 68; i++) {
            if(isdigit(mv->card[i])) {
                cp++;
                if(mv->debug == 12) {
                    printf("npv[%d][%d] = %c prtsym[%d] = %c ",
                    cnp,j,npv[cnp][j],cp,prtsym[cp]);
                    printf("nvt[%d][%d] = %c\n",cnp,j,nvt[cnp][j]);
                }
                /* if(npv[cnp][j] == prtsym[cp]) line[i] = mv->card[i]; */
                if(npv[cnp][j] == prtsym[cp]) {
                    if(nvt[cnp][j] != ' ') line[i] = nvt[cnp][j];
                    else line[i] = mv->card[i];
                }
            }
        }
    }
    return(0);
}

/* add dynamic expand symbol call by *+abc line */
int addexp_meb()
{
    int  i,j;
    int  k;
    int  sf;
    int  pf;
    int  nf;
    int  vf;
    int  len;
    char wk[4];

    wk[0] = mv->card[2];
    wk[1] = mv->card[3];
    wk[2] = mv->card[4];
    wk[3] = '\0';
    /* +axy Arpeggio ? */
    if(wk[0] == 'a' || wk[0] == 'A') {
        if(isdigit(wk[1]) && isdigit(wk[2])) return(0);
    }
    beauspace_meb(mv->card);
    if(mv->debug == 12) printf("Debug12! %s\n",mv->card);
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
    }
    if(mv->debug == 12) printf("Debug12! ar->dxs[%d] = %s\n",k,ar->dxs[k]);
    /* set ar->typ */
    i = 0;
    sf = 0;
    len = strlen(mv->card);
    while(1) {
        if(mv->card[i] == 'r' || mv->card[i] == 'R') {
            if(mv->card[i+1] == '=' || mv->card[i+2] == '=') {
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
            if(mv->card[i] == '\"') {
                while(1) {
                    i++;
                    if(i > len) break;
                    ar->rt[k][j] = mv->card[i];
                    if(mv->card[i] == '\"') {
                        pf = 1;
                        break;
                    }
                    j++;
                    if(j > MAXPTN) {
                        overerr_meb("Pos:13 ar->rt MAXPTN",j);
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
        if(mv->debug == 12) printf("Debug12! ar->rt[%d] = %s\n",k,ar->rt[k]);
    }
    /* set ar->nt */
    i = 0;
    nf = 0;
    while(1) {
        if(mv->card[i] == 'n' || mv->card[i] == 'N') {
            if(mv->card[i+1] == '=' || mv->card[i+2] == '=') {
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
            if(mv->card[i] == '\"') {
                while(1) {
                    i++;
                    if(i > len) break;
                    ar->nt[k][j] = mv->card[i];
                    if(mv->card[i] == '\"') {
                        pf = 1;
                        break;
                    }
                    j++;
                    if(j > MAXPTN) {
                        overerr_meb("Pos:14 ar->nt MAXPTN",j);
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
        if(mv->debug == 12) printf("Debug12! ar->nt[%d] = %s\n",k,ar->nt[k]);
    }
    /* set ar->vt */
    i = 0;  
    vf = 0;
    while(1) {
        if(mv->card[i] == 'v' || mv->card[i] == 'V') {
            if(mv->card[i+1] == '=' || mv->card[i+2] == '=') {
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
            if(mv->card[i] == '\"') {
                while(1) {
                    i++;
                    if(i > len) break;
                    ar->vt[k][j] = mv->card[i];
                    if(mv->card[i] == '\"') {
                        pf = 1;
                        break;
                    }
                    j++;
                    if(j > MAXPTN) {
                        overerr_meb("Pos:14 ar->vt MAXPTN",j);
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
        if(mv->debug == 12) printf("Debug12! ar->vt[%d] = %s\n",k,ar->vt[k]);
    }
    return(0);
}

/* set art val */
int setartv_meb(char dxs[4])
//char dxs[4];
{   
    int  i; 
    int  sf;
    int  len;
    
    ar->af = 0;
    sf = 0; 
    if(mv->debug == 12) printf("setartv_ dxs = %s ar->nmb = %d\n",dxs,ar->nmb);
    for(i = 0 ; i < ar->nmb; i++) {
        if(!strcmp(dxs,ar->dxs[i])) {
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
    if(mv->debug == 12) {
        printf("Debug12! ar->cdxs= %s ar->pmf=%d\n",ar->cdxs,ar->pmf);
        printf("         ar->crt = \"%s\"\n",ar->crt);
        printf("         ar->cnt = \"%s\"\n",ar->cnt);
        printf("         ar->cvt = \"%s\"\n",ar->cvt);
    }
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

int overerr_meb(char msg[80],int ov)
//char msg[80];
//int  ov;
{   
    mv->errcnt++;
    mv->msglvl = 3;
    if(mv->msglvl >= mv->prtlvl) {
        printf("Error! Arrange %s %06d MAXDV over %d\n",msg,mv->line,ov);
        //printf("Error! Arrange %s %06d MAXDV over %d\n",msg,mv->card,ov);
    }
    return(0);
}

int creen_meb()
{
    int  bt;

    bt = 0;
    if(mb->bartyp == 't') bt = 1;
    if(mb->bartyp == 's') bt = 1;
    if(mb->bartyp == 'g') bt = 1;
    if(mb->bartyp == 'a') bt = 1;
    if(mb->bartyp == 'b') bt = 1;
    if(bt) {
        mv->card[14] = ' ';
        mv->card[18] = ' ';
        mv->card[21] = ':';
        mv->card[25] = ' ';
    }
    mv->card[68] = ' ';
    return(0);
}

/* add 2008.5.4 */
int gr_rp_set()
{
    int  i,j;
    int  rf;
    char w[12];
    char wt[12];
    char ws[12];
    static char wp[12];
    static int  rep;

    if(mv->debug == 13) {
        printf("gr_rp_set()\n");
        printf("%s\n",mv->card);
    }
    i = 4;
    j = 0;
    while(1) {
        w[j] = mv->card[i];
        if(w[j] == '\0') break;
        if(w[j] == ' ') break;
        i++;
        if(i > 12 + 4) break;
        j++;
        if(j > 12) break;
    }
    w[j] = '\0';
    for(i = 0; i < 12; i++) {
        wt[i] = w[i];
        if(wt[i] == '\0') break;
        if(wt[i] == '[') break;
        if(wt[i] == ' ') break;
        if(isdigit(wt[i])) break;
    }
    wt[i] = '\0';
    rf = 0;
    for(i = 0; i < 10; i++) {
        if(w[i] == '\0') break;
        if(w[i] == '[' ) {
            rf = 1;
        }
        if(isdigit(w[i])) {
            rf = 2;
            if(w[i-1] == '[') rf = 3;
            break;
        }
    }
    if(rf == 3) { /*  %xx _/[4 */
        j = 0;
        while(1) {
            ws[j] = w[i];
            if(ws[j] == '\0') break;
            if(!isdigit(ws[j])) break;
            j++;
            i++;
            if(i > 12) break;
        }
        ws[j] = '\0';
        sscanf(ws,"%d",&rep);
        strcpy(wp,wt);
        strcpy(w,wt);
        /* printf("ws=%s rep=%d wp=%s w=%s\n",ws,rep,wp,w); */
        if(rep > 1) gr_p_set(w,rep);
    }
    if(rf == 2) {  /* %xx _/4 */
        strcpy(w,wt);
        /* printf("wt=%s rep=1 w=%s\n",wt,w); */
    }
    if(rf == 1) {  /* %xx [ */
        strcpy(w,wp);
        /* printf("wp=%s rep=%d w=%s\n",wp,rep,w); */
        if(rep > 1) gr_p_set(w,rep);
    }
    return(0);
}

int gr_p_set(char *w,int rep)
//char *w;
//int  rep;
{
    int  bexp;
    int  beb;
    int  bea;
    int  be;
    char ws[8];
    int  rpv;
    int  mrpv;

    // add 2013.11.22
    rpv = mv->reprm;
    if(mb->gaterf && ge->def_rp_f) rpv = ge->def_rp_v;
    if(ge->set_rp_f) rpv = ge->set_rp_v;
    mrpv =  -rpv + 50;  // reverce
    if(mv->debug == 13) printf("gr_p_set() w=%s rep=%d rpv=%d mrpv=%d\n",
        w,rep,rpv,mrpv);
    // ------------
    bexp = 90;
    if(!strcmp(w,"|O|"))      bexp = gd->ww;
    if(!strcmp(w,"O"))        bexp = gd->w;
    if(!strcmp(w,"O."))       bexp = gd->wp;
    if(!strcmp(w,"O.."))      bexp = gd->wpp;
    if(!strcmp(w,"o/"))       bexp = gd->h;
    if(!strcmp(w,"o/."))      bexp = gd->hp;
    if(!strcmp(w,"o/.."))     bexp = gd->hpp;
    if(!strcmp(w,"_/"))       bexp = gd->q;
    if(!strcmp(w,"_/."))      bexp = gd->qp;
    if(!strcmp(w,"_/.."))     bexp = gd->qpp;
    if(!strcmp(w,"_/)"))      bexp = gd->n8;
    if(!strcmp(w,"_/)."))     bexp = gd->n8p;
    if(!strcmp(w,"_/).."))    bexp = gd->n8pp;
    if(!strcmp(w,"_/))"))     bexp = gd->n16;
    if(!strcmp(w,"_/))."))    bexp = gd->n16p;
    if(!strcmp(w,"_/)).."))   bexp = gd->n16pp;
    if(!strcmp(w,"_/)))"))    bexp = gd->n32;
    if(!strcmp(w,"_/)))."))   bexp = gd->n32p;
    if(!strcmp(w,"_/))).."))  bexp = gd->n32pp;
    if(!strcmp(w,"_/))))"))   bexp = gd->n64;
    if(!strcmp(w,"_/))))."))  bexp = gd->n64p;
    if(!strcmp(w,"_/))))..")) bexp = gd->n64pp;
    if(!strcmp(w,"./)"))      bexp = gd->t8;
    if(!strcmp(w,"./)."))     bexp = gd->t8p;
    if(!strcmp(w,"./).."))    bexp = gd->t8pp;
    if(!strcmp(w,"./))"))     bexp = gd->t16;
    if(!strcmp(w,"./))."))    bexp = gd->t16p;
    if(!strcmp(w,"./)).."))   bexp = gd->t16pp;
    if(!strcmp(w,"./)))"))    bexp = gd->t32;
    if(!strcmp(w,"./)))."))   bexp = gd->t32p;
    if(!strcmp(w,"./))).."))  bexp = gd->t32pp;
    if(!strcmp(w,"./))))"))   bexp = gd->t64;
    if(!strcmp(w,"./))))."))  bexp = gd->t64p;
    if(!strcmp(w,"./))))..")) bexp = gd->t64pp;
    if(!strcmp(w,"./>"))      bexp = gd->a8;
    if(!strcmp(w,"./>."))     bexp = gd->a8p;
    if(!strcmp(w,"./>.."))    bexp = gd->a8pp;
    if(!strcmp(w,"./>>"))     bexp = gd->a16;
    if(!strcmp(w,"./>>."))    bexp = gd->a16p;
    if(!strcmp(w,"./>>.."))   bexp = gd->a16pp;
    if(!strcmp(w,"./>>>"))    bexp = gd->a32;
    if(!strcmp(w,"./>>>."))   bexp = gd->a32p;
    if(!strcmp(w,"./>>>.."))  bexp = gd->a32pp;
    if(!strcmp(w,"./>>>>"))   bexp = gd->a64;
    if(!strcmp(w,"./>>>>."))  bexp = gd->a64p;
    if(!strcmp(w,"./>>>>..")) bexp = gd->a64pp;
    /* %xx ->  xx - 13 + (13 / rep) */
    // %xx ->  xx - mrpv + (mrpv / rep) test 2013.11.20
    beb = bexp - mrpv;
    bea = 26 / rep;
    be = beb + bea;
    sprintf(ws,"%02d",be);
    /* printf("bexp=%d beb=%d bea=%d be=%d ws=%s\n",bexp,beb,bea,be,ws); */
    mv->card[1] = ws[0];
    mv->card[2] = ws[1];
    return(0);
}

