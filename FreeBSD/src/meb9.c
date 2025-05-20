
// ----------------------
//  AMuPLC (meb9.c)
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
extern MEBSRG *ls;
extern MEBEDT *ge;
extern MEBPH  *ph;
extern MEBGRT *gr,*gd,*ga,*gm,*gs,*gc;
extern MEBCPS *cp;
extern MEBCSL *cs;
extern MECVAR *mr;
extern MEBVEX *vx;
extern char jrule[MAXRL][82];  /* Major Rule */
extern char irule[MAXRL][82];  /* minor Rule */
extern char scard[MSTC][82];
char acard[MSTC][82];
int  sck; /* scard point */
int  slk; /* select line check */
int  ack; /* acard point */
int  alk; /* aa select line check */
int  prf; /* % line read flag */
char rline[82]; /* bar line */
char bline[82]; /* before */
char sline[82]; /* select line */
char cline[82]; /* current */
char aline[82]; /* after */
char cspl[MAXCSP][82];  /* chord select print check line save */
int  cspv[MAXCSP];
int scale[7] = {2,2,1,2,2,2,1};
extern int  vlc;
extern int  tmc;
extern int  gvc;

/* Global chord select
   get data from fn.mix
*/
int gchdsel_meb()
{
    int  i;
    int  bnc;
    int  cnc;
    int  anc;

    if(ls->fmxopend) mixchdsl_meb();
    bnc = 0;
    cnc = 0;
    anc = 0;
    for(i =0; i < 128; i++) {
        if(cs->bv[i]) bnc++;
        if(cs->cv[i]) cnc++;
        if(cs->av[i]) anc++;
    }
    if(mv->debug == 7) {
        printf("mb->sequ=%d mb->bnum=%d bnc(Befor Note Count)=%d ",
        mb->sequ,mb->bnum,bnc);
        printf("cnc(Current)=%d anc(After)=%d\n",cnc,anc);
    }
    beaugsln_meb(); /* modify mv->card */
    return(0);
}

/* {?} selsect line modify */
int beaugsln_meb()
{
    int  i,j,k;
    int  jk;
    char c;
    char cnb[16];      /* chord number I,II.. */
    char chd[16];      /* M,m,sus4,dim... */
    char wk[MAXLVL];
    char wsc[16];
    char wline[160];
    int  len;

    cs->sck = 0;
    for(i = 0; i < cp->n[2]; i++) {
        /* printf("%s\n",cp->l[2][i]); */
        strcpy(cnb,cp->l[2][i]);
        j = 0;
        while(1) {
            c = cp->l[2][i][j];
            if(c == 'I' || c == 'V' || c == '&' || c == '#') {
                cnb[j] = c;
            }
            else {
                cnb[j] = '\0';
                break;
            }
            j++;
            if(j > 16) break;
        }
        k = 0;
        while(1) {
            c = cp->l[2][i][j];
            chd[k] = c;
            if(c == '\0') break;
            j++;
            if(j > MAXLVL) break;
            k++;
            if(k > 16) break;
        }
        chd[k] = '\0';
        /*
        if(chd[0] == '\0') {
            chd[0] = 'M';
            chd[1] = '\0';
        }
        */
        if(mv->debug == 8) printf("Debug8! cnb = %s chd = %s\n",cnb,chd);
        setscn_meb(cnb,chd);
    }
    mkscpt_meb();
    strcpy(wline,"{?");       
    for(i = 0; i < cs->sck; i++) {
        k = cs->scp[i];
        if(mv->debug == 7)
            printf(" k = %d cs->scp[%d] = %d cs->scn[%d] = %s\n",
            k,i,cs->scp[i],k,cs->scn[k]);
        if(cs->scv[k] > 0) {
            if(i > 0) strcat(wline,",");
            if(i > 2) { /* Chord Name only */
                strcpy(wk,cs->scn[k]);
                j = 0;
                while(1) {
                    if(wk[j] == ' ') break;
                    j++;
                    if(j >= MAXLVL) break;
                }
                j++;
                jk = 0;
                while(1) {
                    if(wk[j] == '\0') break;
                    wsc[jk] = wk[j];
                    j++;
                    if(j >= MAXLVL) break;
                    jk++;
                    if(jk > 8) break;
                }
                wsc[jk] = '\0';
                strcat(wline,wsc);
            }
            else strcat(wline,cs->scn[k]);
        }
        len = strlen(wline);
        if(len > 80) {
            wline[78] = '\0';
            break;
        }
    }
    strcat(wline,"}");
    strcpy(mv->card,wline);
    return(0);
}

int chdsel_meb(struct mivksn *mk,int p)
//struct mivksn *mk;
//int  p;
{
    int  i,j,k;
    int  sv;
    int  ws[MAXCSP];
    int  wp[MAXCSP];

    cs->sck = 0; /* cs->scn[] counter 0 */
    for(k = 0; k < 128; k++) {
        cs->bv[k] = 0;
        cs->cv[k] = 0;
        cs->av[k] = 0;
    }
    mv->cspc = 0;
    if(mv->debug == 7) {
        printf("Debug7! --chdsel(%d) cp->key=%d ------\n",p,cp->key);
        keydmp_mec(mk);
     /* printf("Debug7! cp->n[0] =%4d cp->mk[0] = %s\n",cp->n[0],cp->mk[0]); */
        printf("Debug7! cp->n[1] =%4d cp->mk[1] = %s\n",cp->n[1],cp->mk[1]);
        printf("Debug7! cp->n[2] =%4d cp->mk[2] = %s\n",cp->n[2],cp->mk[2]);
     /* printf("Debug7! cp->n[3] =%4d cp->mk[3] = %s\n",cp->n[3],cp->mk[3]); */
     /* printf("Debug7! cp->n[4] =%4d cp->mk[4] = %s\n",cp->n[4],cp->mk[4]); */
     /* printf("Debug7! cp->n[5] =%4d cp->mk[5] = %s\n",cp->n[5],cp->mk[5]); */
        printf("Debug7! cs->n = %6d cs->mk    = %s\n",cs->n,cs->mk);
        printf("%s\n",rline);
        printf("%s\n",bline);
        printf("%s\n",sline);
        printf("%s\n",cline);
        printf("%s\n",aline);
        printf("--befor select current after lines-----------\n");
    }
    if(bline[0] == '%') {
        strcpy(mv->card,bline);
        // if(mv->card[1] == 'x') beaumod_meb();
        notel_meb(mk,'b');
    }
    if(cline[0] == '%') {
        strcpy(mv->card,cline);
        // if(mv->card[1] == 'x') beaumod_meb();
        notel_meb(mk,'c');
    }
    if(aline[0] == '%') {
        strcpy(mv->card,aline);
        // if(mv->card[1] == 'x') beaumod_meb();
        notel_meb(mk,'a');
    }
    if(sline[0] == '?') beausln_meb(sline); 
    if(mv->debug == 7) printf("-----------\n"); 
    if(mv->cspcf) {
        /* keydmp_mec(mk); */
        printf("%s\n",rline);
        printf("%s\n",bline);
        printf("%s\n",sline);
        printf("%s\n",cline);
        printf("%s\n",aline);
        for(i = 0; i < mv->cspc; i++) ws[i] = cspv[i];
        for(j = 0; j < mv->cspc; j++) {
            k = 0;
            sv = 0;
            for(i = 0; i < mv->cspc; i++) {
                if(ws[i] > sv) {
                    sv = ws[i];
                    k = i;
                }
            }
            ws[k] = -1;
            wp[j] = k;
            /* printf("chdsel() wp[%d] = %d\n",j,wp[j]); */
        }
        for(i = 0; i < mv->cspc; i++) {
            if( i > 16) break;
            printf("%s\n",cspl[wp[i]]);
        }
        /* mv->cspc = 0; */
    }
    return(0);
}

/* make cs->scv[] table */
int mkscvt_meb()
{
    int  i,j;
    float v;
    float vb;
    float vc;
    float va;

    if(mv->debug == 7) printf("\nDebug7! mkscvt() cs->scn[%d] = %s\n",
        cs->sck,cs->scn[cs->sck]);
    va = 0.0;
    vb = 0.0;
    vc = 0.0;
    /* bline */
    for(i = 0; i < 127; i++) {
        if(cs->bv[i] != 0 && cs->cn[i] != 0) {
            v = cs->cn[i] * cs->bv[i];
            if(mv->debug == 7) {
                printf("Debug7! v%7.2f = cs->cn[%d]%6.2f * ",
                    v,i,cs->cn[i]);
                printf("cs->bv[%d]%6.2f\n",
                    i,cs->bv[i]);
            }
            vb = vb + v;
        }
    }
    /* cline */
    for(i = 0; i < 127; i++) {
        if(cs->cv[i] != 0 && cs->cn[i] != 0) {
            v = cs->cn[i] * cs->cv[i];
            if(mv->debug == 7) {
                printf("Debug7! v%7.2f = cs->cn[%d]%6.2f * ",
                    v,i,cs->cn[i]);
                printf("cs->cv[%d]%6.2f\n",
                    i,cs->cv[i]);
            }
            vc = vc + v;
        }
    }
    /* aline */
    for(i = 0; i < 127; i++) {
        if(cs->av[i] != 0 && cs->cn[i] != 0) {
            v = cs->cn[i] * cs->av[i];
            if(mv->debug == 7) {
                printf("Debug7! v%7.2f = cs->cn[%d]%6.2f * ",
                    v,i,cs->cn[i]);
                printf("cs->av[%d]%6.2f\n",
                    i,cs->av[i]);
            }
            va = va + v;
        }
    }
    cs->scv[cs->sck] = va + vb + vc;
    /*
    if(mv->debug == 7) {
        printf("Debug7! %s cs->scv[%d]%7.2f = va%6.2f + vb%6.2f + vc%6.2f\n",
            cs->scn[cs->sck],cs->sck,cs->scv[cs->sck],va,vb,vc);
    }
    */
    if(mv->debug == 7) {
        printf("Debug7! %2d %20s Point:%7.2f = b:%6.2f + c:%6.2f + a:%6.2f\n",
          cs->sck,cs->scn[cs->sck],cs->scv[cs->sck],vb,vc,va);
    }
    if(mv->cspcf) {
        /* printf("cs->sck = %d mv->cspc = %d\n",cs->sck,mv->cspc); */
        if(mv->cspc < MAXCSP) {
            sprintf(cspl[mv->cspc],
                "%2d %20s Point:%7.2f = b:%6.2f + c:%6.2f + a:%6.2f",
                cs->sck,cs->scn[cs->sck],cs->scv[cs->sck],vb,vc,va);
            cspv[mv->cspc] = cs->scv[cs->sck];
            mv->cspc++;
        }
    }
    return(0);
}

/* make cs->scp[] table */
/*
   sc->scv[] wo ookii jyun ni banngou wo cs->scp he ireru
*/
int mkscpt_meb()
{
    int  i,j;
    float wscv[MAXCSL];
    float sv;
    int  k;

    for(i = 0; i < cs->sck; i++) wscv[i] = cs->scv[i]; 
    for(j = 0; j < cs->sck; j++) {
        k = 0;
        sv = 0.0;
        for(i = 0; i < cs->sck; i++) {
            if(wscv[i] > sv) {
                sv = wscv[i];
                k = i;
            }
        }
        wscv[k] = -1.0;
        cs->scp[j] = k;
        if(mv->debug == 7)
            printf("Debug7! mkscpt() cs->scp[%d] = %d cs->scv[%d] = %7.2f\n",
                j,cs->scp[j],k,cs->scv[k]);
    }
    return(0);
}

/* ? selsect line modify */
int beausln_meb(char sline[82])
//char sline[82];
{
    int  i,j,k;
    char c;
    char cnb[16];      /* chord number I,II.. */
    char chd[16];      /* M,m,sus4,dim... */
    char wline[160];
    int  len;

    cs->sck = 0;
    for(i = 0; i < cp->n[2]; i++) {
        /* printf("%s\n",cp->l[2][i]); */
        strcpy(cnb,cp->l[2][i]);
        j = 0;
        while(1) {
            c = cp->l[2][i][j];
            if(c == 'I' || c == 'V' || c == '&' || c == '#') {
                cnb[j] = c;
            }
            else {
                cnb[j] = '\0';
                break;
            }
            j++;
            if(j > 16) break;
        }
        k = 0;
        while(1) {
            c = cp->l[2][i][j];
            chd[k] = c;
            if(c == '\0') break;
            j++;
            if(j > MAXLVL) break;
            k++;
            if(k > 16) break;
        }
        chd[k] = '\0';
        /*
        if(chd[0] == '\0') {
            chd[0] = 'M';
            chd[1] = '\0';
        }
        */
        if(mv->debug == 8) printf("Debug8! cnb = %s chd = %s\n",cnb,chd);
        setscn_meb(cnb,chd);
    }
    mkscpt_meb();
    strcpy(wline,"?");       
    for(i = 0; i < cs->sck; i++) {
        k = cs->scp[i];
        if(mv->debug == 7)
            printf(" k = %d cs->scp[%d] = %d cs->scn[%d] = %s\n",
            k,i,cs->scp[i],k,cs->scn[k]);
        if(cs->scv[k] > 0) {
            if(i > 0) strcat(wline,",");
            strcat(wline,cs->scn[k]);
        }
        len = strlen(wline);
        if(len > 80) {
            wline[78] = '\0';
            break;
        }
    }
    /* strcat(wline,"\n"); */
    strcpy(sline,wline);
    if(mv->debug) printf("Debug! beausln_() sline=%s\n",sline);
    return(0);
}

/* set cs->scn[]
   set available chord from rule data $2:
*/
int setscn_meb(char cnb[16],char chd[16])
//char cnb[16];
//char chd[16];
{
    int  i;
    char wk[MAXLVL];

    for(i = 0; i < cp->n[1]; i++) {
        if(!strcmp(cnb,cp->l[1][i])) {
            /*
            if(mv->debug == 8) printf("%s cp->r[1][%d] = %s\n",
                cnb,i,cp->r[1][i]);
            */
            strcpy(wk,cnb);
            strcat(wk,chd);
            strcat(wk," ");
            strcat(wk,cp->r[1][i]);
            strcat(wk,chd);
            /* printf("wk = %s\n",wk); */
            strcpy(cs->scn[cs->sck],wk);
            if(mv->debug == 8) printf("Debug8! cs->scn[%d] = %s\n",
                cs->sck,cs->scn[cs->sck]);
            setcn_meb(cnb,cp->r[1][i],chd);
            mkscvt_meb();
            cs->sck++;
        }
    }
    return(0);
}

/* set cs->cn[] */
int setcn_meb(char cnb[16],char bcd[16],char chd[16])
//char cnb[16];  /* Chord Number */
//char bcd[16];  /* Base Chord Name */
//char chd[16];  /* Chord Name */
{
    int  i,j,k;
    int  bp;        /* Base chord position */
    int  rp;        /* Root chord position */
    int  ef;
    int  ap[24];    /* Available Note position ". & ."  */
    float av[24];   /* Available Note vale      5,5,5 */
    int  apk;
    int  avk;
    float v;
    char wk[12];
    char sm[MAXLVM];  /* 5,5,5.. */
    char sr[MAXLVR];  /* ". & ." */

    if(mv->debug == 8) printf("Debug8! setcn_() cnb = %s bcd = %s chd = %s\n",
        cnb,bcd,chd);
    bp = 0;
    if(bcd[0] == 'C') bp = 0;
    if(bcd[0] == 'D') bp = 2;
    if(bcd[0] == 'E') bp = 4;
    if(bcd[0] == 'F') bp = 5;
    if(bcd[0] == 'G') bp = 7;
    if(bcd[0] == 'A') bp = 9;
    if(bcd[0] == 'B') bp = 11;
    if(bcd[1] == '#') bp = bp + 1;
    if(bcd[1] == '&') bp = bp - 1;
    if(bcd[1] == '*') bp = bp + 2;
    if(bcd[1] == '@') bp = bp - 2;
    if(chd[0] == '\0') {
        chd[0] = 'M';
        chd[1] = '\0';
    }
    for(i = 0; i < cs->n; i++) {
        if(!strcmp(chd,cs->l[i])) {
            strcpy(sm,cs->m[i]);
            strcpy(sr,cs->r[i]);
        }
    }
    if(mv->debug == 8) printf("Debug8! chd = %s sm = %s sr = %s\n",chd,sm,sr);
    /* set ap[] table */
    i = 0;
    j = 0;
    k = 0;
    apk = 0;
    ef = 0;
    while(1) {
        switch(sr[i]) {
            case   '.' : ap[apk] = bp + k; 
                         if(ap[apk] > 127) ap[apk] = 127;
                         if(ap[apk] < 0) ap[apk] = 0;
                         apk++;
                         break;
            case   '&' : ap[apk] = bp + k - 1; 
                         if(ap[apk] > 127) ap[apk] = 127;
                         if(ap[apk] < 0) ap[apk] = 0;
                         apk++;
                         break;
            case   '#' : ap[apk] = bp + k + 1; 
                         if(ap[apk] > 127) ap[apk] = 127;
                         if(ap[apk] < 0) ap[apk] = 0;
                         apk++;
                         break;
            case   ' ' : /* */
                         break;
            case  '\0' : /* */
                         ef = 1;
                         break;
            default    : /* Error */
                         setcnmsg_meb(1);
                         printf("sr[%d] = %c(0x%02x)\n",i,sr[i],sr[i]);
                         break;
        }
        if(ef) break;
        j = j % 7; 
        k = k + scale[j];
        /* printf("sr[%d] k = %d scale[%d] = %d\n",i,k,j,scale[j]); */
        j++;
        i++;
        if(i > MAXLVR) break;
        if(apk > 24) {
            setcnmsg_meb(2);
            printf("apk=%d\n",apk);
            break;
        }
    }
    i = 0;
    k = 0;
    ef = 0;
    avk = 0;
    /* set av[] 5,5.02,5,5 */
    while(1) {
        if(sm[i] == '\0') break;
        if(isdigit(sm[i])) {
            j = 0;
            while(1) {
                if(sm[i] == ',' || sm[i] == ' ') break;
                if(sm[i] == '\0') {
                    ef = 1;
                    break;
                }
                wk[j] = sm[i];
                j++;
                if(j > 12) {
                    setcnmsg_meb(3);
                    printf("sm[%d] = %c\n",i,sm[i]);
                    break;
                }
                i++;
                if(i > MAXLVM) break;
            }
            wk[j] = '\0';
            sscanf(wk,"%f",&v);
            av[avk] = v;
            if(mv->debug == 8)
                printf("Debug8! setcn() ap[%d] = %d av[%d] =%5.1f v =%5.1f\n",
                avk,ap[avk],avk,av[avk],v);
            avk++;
            if(avk > 24) {
                setcnmsg_meb(4);
                printf("sm[%d = %c\n",i,sm[i]);
                break;
            }
        }
        if(ef) break;
        i++;
        if(i > MAXLVR) break;
        if(k > 127) break;
    }
    /* printf("apk = %d avk = %d\n",apk,avk); */
    if(apk != avk) {
        setcnmsg_meb(5);
        printf("apk = %d avk = %d\n",apk,avk);
    }
    /* set c->cn[] */
    for(i = 0; i < 127; i++) cs->cn[i] = 0.0;
    j = 0;
    for(k = 0; k < apk; k++) {
        i = ap[k];
        while(1) {
            cs->cn[i] = av[j];
            i = i + 12;
            if(i > 127) break;
            /*
            printf("cs->cn[%d] =%5.1f ap[%d] = %d av[%d] =%5.1f\n",
                i,cs->cn[i],k,ap[k],j,av[j]);
            */
        }
        j++;
        if(j > avk) break;
    }
    if(mv->debug == 8) {
        k = 0;
        ef = 0;
        printf("Debug8! cs->cn[] Table dump\n");
        printf("   C         D         E    F         G         A         B\n");
        printf("   0    1    2    3    4    5    6    7    8    9   10   11\n");
        while(1) {
            for(i = 0; i < 12; i++) {
                printf("%4.1f ",cs->cn[k]);
                k++;
                if(k > 127) {
                    ef = 1;
                    break;
                }
            }
            printf("\n");
            if(ef) break;
            j++;
            if(j > 11) break;
        }
    }
    return(0);
}

/* main global {?} chord select */
int mchdsel_meb(char *inf,struct mivksn *mk,struct mivsym ms[])
//char *inf;
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i,j,k;
    FILE *fpi;
    char obsf[STLN];
    char c1;
    int  dxf;

    if(mv->debug == 7) printf("Debug7! mchdsel inf = %s\n",inf);
    mv->line = 0;
    mb->bnum = 0;
    mb->xbnum = 0;
    vx->gvc = 0;
    vx->tmc = 0;
    vx->vlc = 0;
    vx->gulf = 0;
    vx->tulf = 0;
    vx->vulf = 0;
    gvc = 0;
    tmc = 0;
    vlc = 0;
    i = 0;
    while(inf[i] != '_') {
        obsf[i] = inf[i];
        i++;
    }
    obsf[i] = '\0';
    strcat(obsf,".mix");
    /* printf("obsf = %s\n",obsf); */
    if((ls->fmx = fopen(obsf,"rb")) == NULL) {
        mv->wrncnt++;
        mv->msglvl = 0;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Worning! Observe file \"%s\" not found\n",obsf);
        }
        mb->observf = 0;
    }
    else ls->fmxopend = 1;
    setlv6_cps("default_chord_select");
    getNotew_meb(); /* get ka,kb,kc,kd from rule file */
    mopen_meb();
    while(mgets_meb(mv->card)) {   // get mv->card  2025.4.2
        for(i = 0; i < 82; i++) {
            if(mv->card[i] == '\n' || mv->card[i] == '\r') { 
                mv->card[i] = '\0';
                break;      
            }           
        }               
        mv->line++;
        etcard_meb(mk);
        switch (mv->card[0]) {  
            case '-'  : /* bar */ 
                        mainbar_meb();
                        setbnum_meb();
                        break;
            case 's'  : /* beaumset_meb(); */
                        mb->renp = 1;
                        renget_meb();
                        if(mb->renp >= MSTC) {
                            mv->errcnt++;
                            mv->msglvl = 3;
                            if(mv->msglvl >= mv->prtlvl)
                                printf("Error! %05d renpu max is %d\n",
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
                        mexpmod_meb();
                        mvlcmod_meb();
                        tmcmod_meb();
                        gvcmod_meb();
                        if(mb->expnl == 3) gexpclr_meb();
                        break;
            case 'c'  : /* beaumset_meb(); */
                        mb->renp = 1;
                        renget_meb();
                        if(mb->renp >= MSTC) {
                            mv->errcnt++;
                            mv->msglvl = 3;
                            if(mv->msglvl >= mv->prtlvl)
                            printf("Error! %05d renpu max is %d\n",
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
            case '*'  : /* Vertical modify */
                        /* mvlcmod_meb(); need check c or s line ? */
                        tmcmod_meb();
                        gvcmod_meb();
                        c1 = mv->card[1];
                        if(c1 == '+' || c1 == '-') addexp_meb();
                        if(c1 == 'T' || c1 == 'G' || c1 == 'V' || c1 == 'E' ||
                            c1 == 't' || c1 == 'g' || c1 == 'v' || c1 == 'e') {
                            verexp_meb(); 
                        }
                        break;
            case '('  : /* local */
                        if(mv->card[2] == ':' || mv->card[3] == ':' ||
                            mv->card[4] == ':') {
                            sbrtolbr_meb();
                            if(mv->card[1] == '$') setlvl_cps();
                            lbrtosbr_meb();
                            break;
                        }
                        if(mv->card[1] == '$') {
                            sbrtolbr_meb();
                            beaugkey_meb(mk); 
                            lbrtosbr_meb();
                            break;
                        }   
                        sbrtolbr_meb();
                        if(mv->card[1] == '!') {
                            chord_cps();    
                        }
                        if(mv->card[1] == '?') {
                            /* global chord select */
                            gchdsel_meb();
                        }
                        if(mv->card[1] == 'm' && mv->card[2] == '.' &&
                         mv->card[3] == 'm') {
                             exptmp_meb();         /* {m.m =/ = xx } */
                             /* mb->bexpsf = 0; */
                        }
                        lbrtosbr_meb();
                        break;
            case '{'  : /* Global */
                        if(mv->card[2] == ':' || mv->card[3] == ':' ||
                            mv->card[4] == ':') {
                            if(mv->card[1] == '$') setlvl_cps();
                            beaumput_meb();
                            break;
                        }
                        if(mv->card[1] == '$') {
                            if(mv->debug == 3)
                                printf("mchdsel_() %s\n",mv->card);
                            beaumput_meb();
                            beaugkey_meb(mk); 
                            break;
                        }   
                        if(mv->card[1] == '!') {
                            chord_cps();
                        }   
                        if(mv->card[1] == '?') {
                            /* global chord select */ 
                            gchdsel_meb();
                        }   
                        if(mv->card[1] == 'm' && mv->card[2] == '.' && 
                         mv->card[3] == 'm') {
                             exptmp_meb();         /* {m.m =/ = xx } */
                             /* mb->bexpsf = 0; */
                        }
                        beaumput_meb();
                        if(mv->card[1] == '#') {
                            strck_meb();
                        }   
                        break;
                        /* add 2004.5.22 beauexp */
            case '>'  : if(mv->card[1] == '{') beauexp_meb();
                        break;
            case '!'  : /* chord */
                        chord_cps();
                        break;
            case '#'  : strck_meb();
                        break;
            case '?'  : /* chord select */
                        break; 
            case '$'  : if(mv->card[2] == '$') break;
                        if(mv->card[1] == ':' || mv->card[2] == ':') {
                            setlvl_cps();
                        }
                        beaukey_meb(mk);
                        key_meb();
                        setkey_cps();
                        break;  
            default   : break;
        }
        dxf = 0;
        if(mv->card[33] == '+') dxf++;
        if(mv->card[33] == '-') dxf++;
        if(mb->decxp && mv->card[0] == 's' && dxf) mdecdxp_meb();
        else {
            if(mv->source == 2 || mv->source == 3) printf("%s\n",mv->card);
            mprintf_meb(mv->card);
        }
    }
    mcopy_meb(inf);
    // comment out 2025.4.2 core dump ?
    if(ls->fmxopend) {
        //fclose(ls->fmx);  // 2025.4.2
        ls->fmxopend = 0;
    }
    return(0);
}

/* part chord select */
int pchdsel_meb(struct mivksn *mk,struct mivsym ms[])
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i,j,k;
    FILE *fpi;
    int  brik;
    int  prik;
    int  arik;
    int  apf;
    char wk[82];

    if(mv->debug == 7) printf("Debug7! pchdsel mv->step = %d\n",mv->step);
    mopen_meb();
    /* startval_meb(); */
    cp->chgf = cp->achgf;
    /* printf("HHHHH mb->ktrnsv = %d\n",mb->ktrnsv); */
    setlv6_cps("default_chord_select");
    getNotew_meb(); /* get ka,kb,kc,kd from rule file */
    for( i = 0; i < 80; i++ ) { // 2015.6.4
        bline[i] = ' ';
        sline[i] = ' ';
        cline[i] = ' ';
        aline[i] = ' ';
    }
    bline[0] = '\0';
    sline[0] = '\0';
    cline[0] = '\0';
    aline[0] = '\0';
    mb->ptrnsv = 0;
    mv->line = 0;
    mv->cspc = 0;
    prik = 0;
    arik = 0;
    prf = 1;
    while(1) {
        strcpy(bline,cline);
        strcpy(cline,aline);
        if(prik == 0) brik = bfget_meb(fpi,mk,ms);
        if(brik < 0) break;
        prik = afget_meb(fpi,mk);
        if(mv->debug == 7) printf("Debug7! prik = %d sck = %d slk = %d\n",
            prik,sck,slk);
        chdsel_meb(mk,1);
        lineout_meb(sline,mk,ms);
        apf = 0;
        ack = 0;
        alk = 0;
        for(i = 0; i < sck; i++) {
            if(scard[i][0] == '?') {
                if(prf == 0) {
                    mv->wrncnt++;
                    mv->msglvl = 2;
                    strcpy(wk,scard[i]);
                    j = 0;
                    while(1) {
                        if(wk[j] == '\r') break;
                        if(wk[j] == '\n') break;
                        j++;
                        if(j > 16) break;
                    }
                    wk[j] = '\0';
                    strcat(wk," ..");
                    if(mv->msglvl >= mv->prtlvl) 
                        printf("Worning! %s chord select line, removed\n",
                        wk);
                }
                else {
                    apf = 1;
                    strcpy(bline,cline);
                    strcpy(cline,aline);
                    strcpy(sline,scard[i]);
                    arik = aafget_meb(fpi,mk);
                    if(mv->debug == 7)
                        printf("Debug7! arik = %d ack = %d alk = %d\n",
                        arik,ack,alk);
                    chdsel_meb(mk,2);
                    lineout_meb(sline,mk,ms);
                }
            }
            else {
                lineout_meb(scard[i],mk,ms);
            }
            if(arik < 0) prik = -3; 
            if(scard[i][0] == '?') prf = 0;
            if(scard[i][0] == '%') prf = 1;
            strcpy(mv->card,scard[i]);
        }
        for(i = 0; i < ack; i++) {
            if(acard[i][0] == '?') {
                strcpy(bline,cline);
                strcpy(cline,aline);
                strcpy(sline,acard[i]);
                aline[0] = '\0';
                k = 0;
                for(j = i; j < ack; j++) {
                    if(acard[j][0] == '%') {
                        if(k == 1) strcpy(aline,acard[j]);
                        k++;
                    }
                }
                chdsel_meb(mk,3);
                strcpy(acard[i],sline);
            }
            lineout_meb(acard[i],mk,ms);
            if(acard[i][0] == '?') prf = 0;
            if(acard[i][0] == '%') prf = 1;
        }
        if(prik < 0) break;
    }
    return(0);
}

int bfget_meb(FILE *fpi,struct mivksn *mk,struct mivsym ms[])
//FILE *fpi;
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i;
    char card[82];
    char c0;

    if(mv->debug == 7) printf("Debug7! bfget()\n");
    /* while(fgets(card, 82, fpi)) { */
    while(mgets_meb(card)) {  // 2025.4.2
        for(i = 0; i < 82; i++) {
            if(card[i] == '\n' || card[i] == '\r') { 
                card[i] = '\0';
                break;      
            }           
        }               
        strcpy(mv->card,card);
        etcard_meb(mk);
        mv->line++;
        c0 = card[0];
        if(c0 == '?') prf = 0;
        if(c0 == '%') {
            strcpy(bline,card);
            modline_meb(bline,"bline");
            prf = 1;
        }
        if(c0 == '?') {
            strcpy(sline,card);
            return(0); 
        }
        lineout_meb(card,mk,ms);
    }
    return(-1); /* EOF */
}

int keyln_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    if(!mb->ktrnsv) return(0);
    if(mv->card[2] != '$') {
        if(mv->debug == 9) printf("%s",mv->card);               
        if(mv->card[1] == ':' || mv->card[2] == ':' || mv->card[3] == ':') {
            if(mv->debug == 9) mv->debug = 6;
            setlvl_cps();
            if(mv->debug == 6) mv->debug = 9;
        }       
        else {
            /* getNotew_meb(); get ka,kb,kc,kd from rule file */
            chdkey_mec(mk);
        }
    }       
    return(0);
}

int afget_meb(FILE *fpi,struct mivksn *mk)
//FILE *fpi;
//struct mivksn *mk;
{
    int  i,k;
    char c0;

    if(mv->debug == 7) printf("Debug7! afget()\n");
    k = 0;
    sck = 0;
    slk = 0;
    /* while(fgets(mv->card, 82, fpi)) { */
    while(mgets_meb(mv->card)) {   // 2025.4.2
        for(i = 0; i < 82; i++) {
            if(mv->card[i] == '\n' || mv->card[i] == '\r') { 
                mv->card[i] = '\0';
                break;      
            }           
        }               
        strcpy(scard[sck],mv->card);
        etcard_meb(mk);
        mv->line++;
        sck++;
        if(sck > MSTC) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
                printf("Error! afget() line over than %d\n",MSTC);
            return(-2);
        }
        c0 = mv->card[0];
        if(c0 == '%') {
            if(k == 0) {
                /* strcpy(bline,cline); */
                strcpy(cline,mv->card);
                modline_meb(cline,"cline");
            }
            if(k == 1) {
                strcpy(aline,mv->card);
                modline_meb(aline,"aline");
            }
            k++;
            if(k >= 2) return(0);
        }
        if(c0 == '?') slk++;
        if(c0 == '$' && mv->card[1] == '$' &&
            mv->card[78] == '$') return(-1); 
    }
    return(-1);
}

int aafget_meb(FILE *fpi,struct mivksn *mk)
//FILE *fpi;
//struct mivksn *mk;
{
    int  i,k;
    char c0;

    if(mv->debug == 7) printf("Debug7! aafget()\n");
    k = 0;
    /*
    ack = 0;
    alk = 0;
    */
    /* while(fgets(mv->card, 82, fpi)) { */
    while(mgets_meb(mv->card)) {  // 2025.4.2
        for(i = 0; i < 82; i++) {
            if(mv->card[i] == '\n' || mv->card[i] == '\r') { 
                mv->card[i] = '\0';
                break;      
            }           
        }
        strcpy(acard[ack],mv->card);
        etcard_meb(mk);
        mv->line++;
        ack++;
        if(ack > MSTC) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
                printf("Error! aafget() line over than %d\n",MSTC);
            return(-2);
        }
        c0 = mv->card[0];
        if(c0 == '%') {
            strcpy(aline,mv->card);
            modline_meb(aline,"aline");
            return(0);
        }
        if(c0 == '?') alk++;
        if(c0 == '$' && mv->card[1] == '$' &&
            mv->card[78] == '$') return(-1); 
    }
    return(-1);
}

/* get Note Weight */
int getNotew_meb()
{
    int  i,k;
    int  v;
    int  v1,v2,v3,v4;
    float klp;                /* Note length */
    float knp;                /* Key position */
    float kvp;                /* Velocity */
    float kgc;                /* Global Current Chord */
    float kgb;                /* Global Before Chord */ 
    /* float kgap;               Global After Chord */  
    float kcp;                /* Current Chord */
    float kbp;                /* Before Chord */ 
    float kap;                /* After Chord */  
    char wline[82];
    int  sf;
    int  rv;

    klp = 0.0002;
    knp = -0.2;
    kvp = 0.2;
    kgc = 0.2;
    kgb = 0.05;
    kcp = 0.2;
    kbp = 0.1;
    kap = 0.1;
    k = 0;
    while(1) {
        if(cp->key == 1) strcpy(wline,jrule[k]);
        if(cp->key == 2) strcpy(wline,irule[k]);
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        sf = 0;
        if(wline[0] == '$' && wline[1] == '7' && wline[2] == ':') sf = 1;
        if(sf) {
            klp = getselp_meb(wline,"klp",klp);
            knp = getselp_meb(wline,"knp",knp);
            kvp = getselp_meb(wline,"kvp",kvp);
            kgc = getselp_meb(wline,"kgc",kgc);
            kgb = getselp_meb(wline,"kgb",kgb);
            kcp = getselp_meb(wline,"kcp",kcp);
            kbp = getselp_meb(wline,"kbp",kbp);
            kap = getselp_meb(wline,"kap",kap);
        }
    } 
    cs->klp = klp;
    cs->knp = knp;
    cs->kvp = kvp;
    cs->kgc = kgc;
    cs->kgb = kgb;
    cs->kcp = kcp;
    cs->kbp = kbp;
    cs->kap = kap;
    if(mv->debug == 7 || mv->debug == 9) {
        printf("Debug! cs->klp=%f knp=%3.2f kvp=%3.2f\n",
            cs->klp,cs->knp,cs->kvp);
        printf("       cs->kgc=%3.2f kgb=%3.2f ",cs->kgc,cs->kgb);
        printf("kcp=%3.2f kbp=%3.2f kap=%3.2f\n",
            cs->kcp,cs->kbp,cs->kap);
    }
    return(0);
}

float getselp_meb(char wline[82],char *prm,float vf)
//char wline[82];
//char *prm;
//float vf;
{
    int  i,j;
    char wk[16];
    char ww[16];
    int  af;

    wk[0] = wline[3];
    wk[1] = wline[4];
    wk[2] = wline[5];
    wk[3] = '\0';
    if(!strcmp(wk,prm)) {
        /* printf("%s",wline); */
        i = 5;
        af = 0;
        while(1) {
            if(wline[i] == '=') {
                af = 1;
                break;
            }
            if(wline[i] == '\0') break;
            i++;
            if(i > 70) break;
        }
        if(af) {
            i++;
            while(wline[i] == ' ') {
                i++;
                if(i > 70) break;
            }
            j = 0; 
            while(1) {
                if(wline[i] == ' ') break;
                if(wline[i] == '\0') break;
                ww[j] = wline[i];
                i++;
                if(i > 78) break;
                j++;
                if(j > 16) break;
            }
            ww[j] = '\0';
            sscanf(ww,"%f",&vf);
            /* printf("ww = %s vf = %f\n",ww,vf); */
        }
    }
    return(vf);
}

int lineout_meb(char line[82],struct mivksn *mk,struct mivsym ms[])
//char line[82];
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i,j;
    int  pas;
    int  pf; /* print flag */
    char wk[8];
    int  trnsbk;

    strcpy(mv->card,line);
    if(mv->mfflg == 0 && mv->card[0] == ' ' && mv->card[1] == '{' &&
       mv->card[2] == '#') return(0); /* remove global {#...} line  */
    /* Add Transpose key $xx Transpose xx */
    pf = 1;
    switch (mv->card[0]) {
        case '%'  : /* Note line */
                    if(mv->card[1] == 'x') beaumod_meb();
                    if(mb->ptrnsv) insutrns_meb();
                    accid_meb(mk);
                    break;
        case '{'  : /* Gloval */
                    /*
                    printf("mv->step = %d\n",mv->step);
                    printf("%s\n",mv->card);
                    */
                    /* printf("lineout_() %s\n",mv->card); */
                    /* add setlvl_() for -cs option 2003.6 */
                    if(mv->card[3] == ':') {
                        setlvl_cps(); /* add 2003.6 */
                        break;
                    }
                    if(mv->card[1] == '$') {
                        // keydmp_mec(mk); 
                        setorgktb_meb(mk); // set bktb[] 
                        setkey_cps();
                        keyini_mec();
                        keyset_meb(); 
                        if(mv->debug == 7) keydmp_mec(mk);
                    }   
                    if(!mb->ktrnsv) break;
                    if(mv->card[2] == ':' || mv->card[3] == ':' ||
                        mv->card[4] == ':') {
                        break;
                    }   
                    if(mv->card[1] == '$' && mb->ktrnsv) {
                        fprint_meb(mv->card); 
                        xtransp_meb(mk);
                        fprint_meb(mv->card); 
                        pf = 0;
                        break;
                    }
                    beauexp_meb();
                    break;
        case '>'  : if(mv->card[1] == '{') {
                        beauexp_meb();
                        break;
                    }
        case '('  : /* Local */
                    if(!mb->ktrnsv) break; 
                    if(mv->card[2] == ':' || mv->card[3] == ':' ||
                        mv->card[4] == ':') {
                        break;
                    }
                    if(mv->card[1] == '$' && mb->ktrnsv) {
                        fprint_meb(mv->card); 
                        xtransp_meb(mk);
                        fprint_meb(mv->card); 
                        pf = 0;
                    }
                    break;
        case '$'  : /* remove Transpose line */
                    if(mv->card[1] == '$') break;
                    if(mv->card[2] == ':') setlvl_cps(); /* add 2003.6 */
                    if(mv->card[1] == ':' || mv->card[2] == ':' ||
                        mv->card[3] == ':') {
                        break; 
                    }       
                    i = 0;  
                    pas = 0;
                    while(mv->card[i]) i++;
                    while(mv->card[i] != 'T' && mv->card[i] != 'A') {
                        i--;
                        if(i < 1) break;
                        pas = 1; 
                    }   
                    if(pas) {
                        for(j = 0; j < 5; j++) wk[j] = mv->card[i++];
                        wk[j] = '\0';
                        if(!strcmp(wk,"Trans")) {
                            pf = 0;
                            trnsbk = numdinmb_meb(mv->card); 
                            mb->ptrnsv = -trnsbk;
                            /* printf("trnsbk = %d\n",trnsbk); */
                        }
                    }
                    break;
        case '@'  : /* out @inst line to fn.mit */
                    getinstv_meb(ms);
                    break;
        case '-'  : /* Bar line */
                    mb->bartyp = mv->card[1];
                    if(mv->cspcf) strcpy(rline,mv->card);
                    break;
        case 'R'  : /* Rest */
                    if(mb->ptrnsv) insutrns_meb();
                    accid_meb(mk);
                    break;
        case 'r'  : /* rest */
                    if(mb->ptrnsv) insutrns_meb();
                    accid_meb(mk);
                    break;
        case '+'  : /* Plus line */
                    if(mb->ptrnsv) insutrns_meb();
                    accid_meb(mk);
                    break;
        case ':'  : if(mv->byps != 1) break;
                    if(mb->ptrnsv) insutrns_meb();
                    break;
        default   : break;
    }  
    if(pf) fprint_meb(mv->card); 
    return(0);
}

/* $xx transpose  */
int xtransp_meb(struct mivksn *mk)
//struct mivksn *mk;
{       
    char  wk[82];
        
    mktrkey_meb(mb->ktrnsv);
    sprintf(mv->card,"$%s%d/%d",mb->trkey,mb->ryhm,mb->time);
    beaukey_meb(mk);      
    sprintf(wk,"%s Transpose %d",mv->card,mb->ktrnsv);
    strcpy(mv->card,wk);
    key_meb();
    setkey_cps();
    return(0);
}   

int fprint_meb(char line[82])
//char line[82];
{
    if(mv->source == 2) printf("%s\n",line);
    mprintf_meb(line);
    if(mv->debug == 7) printf("%s\n",line);
    return(0);
}

int setcnmsg_meb(int errf)
//int  errf;
{
    mv->errcnt++;
    mv->msglvl = 3;
    if(mv->msglvl >= mv->prtlvl)
        printf("Error! setcn(%d) Rule file $6: data ",errf);
    return(0);
}

int etcard_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    int  i;

    if(mv->card[0] == '-') {
        mb->bartyp = mv->card[1]; 
        keyini_mec();
        keyset_meb();
        for(i = 0; i < 78; i++) mv->ktbbak[i] = mv->ktb[i];
    }
    switch(mv->card[0]) {
        case  '$' : /* key */ 
                    keyln_meb(mk);
                    break;
        /*
        case  '+' : accid_meb(mk);
                    break;
        */
        case  '{' : /* key */
                    if(mv->card[1] == '$') keyln_meb(mk);
                    break;
        default   : break;
    }
    return(0);
}

int setbnum_meb()
{
    char wk[8];

    if(mv->card[0] != '-') return(0);
    wk[0] = mv->card[2];
    wk[1] = mv->card[3];
    wk[2] = '\0';
    sscanf(wk,"%d",&mb->sequ);
    wk[0] = mv->card[4];
    wk[1] = mv->card[5];
    wk[2] = mv->card[6];
    wk[3] = mv->card[7];
    wk[4] = mv->card[8];
    wk[5] = '\0';
    sscanf(wk,"%d",&mb->bnum);
    return(0);
}

int mixchdsl_meb()
{
    union MCODE u;
    unsigned char iword[4];
    int c;
    int i,j,k;

    if(mv->debug == 7) printf("mixchdsl-() ls->sequ = %d ls->bnum = %d\n",
        ls->sequ,ls->bnum);
    k = 0;
    for(i = 0; i <128; i++) cs->av[0] = 0;
    while(1) {
        /* if(ls->sequ == mb->sequ && ls->bnum <= mb->bnum) break; */
        if(ls->sequ == mb->sequ && ls->bnum > mb->bnum) break; 
        if((c = fgetc(ls->fmx)) == EOF) {
            mb->observf = 0;
            ls->fmxopend = 0;
            //fclose(ls->fmx);
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

/*   FD code ....................................
Note off      : FD8xyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(voff)     : verociti off (0-120)

Note on       : FD9xyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(veon)     : verociti on : dynamik(0-9)
*/
int gfdset_mlk(union MCODE *u)
//union MCODE *u;
{
    ls->fdtyp = u->wc[0] & 0xf0;
    ls->fdtyp = ls->fdtyp >> 4;
    ls->fdch = u->wc[0] & 0x0f;
    switch (ls->fdtyp) {
        /*
        case  8 : ls->keyp = u->wc[3] & 0xff;
                  ls->voff = u->wc[2] & 0xff;
                  if(ls->keyp < 0) ls->keyp = 0;
                  if(ls->keyp > 127) ls->keyp = 127;
                  cs->av[ls->keyp]--;
                  if(mv->debug == 7) {
                      printf("FD(%04x%04x) Note off / fdtyp=%d fdch=%d",
                          u->wi[0],u->wi[1],ls->fdtyp,ls->fdch);
                      printf(" keyp=%d voff=%d cs->av[%d]=%d\n",
                          ls->keyp,ls->voff,ls->keyp,cs->av[ls->keyp]);
                  }
                  break;
        */
        case  9 : ls->keyp = u->wc[3] & 0xff;
                  ls->veon = u->wc[2] & 0xff;
                  if(ls->keyp < 0) ls->keyp = 0;
                  if(ls->keyp > 127) ls->keyp = 127;
                  if(ls->veon) {
                      gvnote_meb();
                      /*
                      if(mv->debug == 7) {
                          printf("FD(%04x%04x) Note on / fdtyp=%d fdch=%d",
                          u->wi[0],u->wi[1],ls->fdtyp,ls->fdch);
                          printf(" keyp=%d veon=%d cs->av[%d]=%f\n",
                          ls->keyp,ls->veon,ls->keyp,cs->av[ls->keyp]);
                      }
                      */
                  }
                  /* else cs->av[ls->keyp]--; */
                  break;
        default : break;
    }
    return(0);
}

int gvnote_meb()
{
    float nval;
    float velo;
    float keyp;

    /* nval = cs->klp * (mb->nval * mr->grate / 100.0); */

    nval = 30.0;  /* ignore nval */
    velo = cs->kvp * ls->veon;
    keyp = cs->knp * ls->keyp;
    cs->av[ls->keyp] = cs->kbp * (nval + velo + keyp);
    /* printf("kvp = %f knp = %f\n",cs->kvp,cs->knp); */
    if(mv->debug == 7) {
        printf("gvnote() cs->av[%d]=%6.2f nval=%6.2f velo=%6.2f keyp=%6.2f\n",
           ls->keyp,cs->av[ls->keyp],nval,velo,keyp);
    }
    return(0);
}

int chdkey_mec(struct mivksn *mk)
//struct mivksn *mk;
{
    char wk[8];
    char c;
    int  i,j;
    int  err;
    int  pas;

    if(mv->debug == 9) printf("key_mec() \n");
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
    if(mv->debug == 9) printf("Debug9! mb->ksin=%d\n",mb->ksin);
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
                printf("Error! %s rhythm greater than 255\n",mv->card);
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
        if(mv->debug == 9) printf("Debug9! mb->ryhm=%d mb->time=%d\n",
            mb->ryhm,mb->time);
    }
    keyset_meb();
    if(mv->debug == 9) keydmp_mec(mk);
    return(0);
}

int keyini_mec()
{
    int  i;

    mv->ktb[0] = 0;
    mv->ktb[1] = 2;
    mv->ktb[2] = 4;
    mv->ktb[3] = 5;
    mv->ktb[4] = 7;
    mv->ktb[5] = 9;
    mv->ktb[6] = 11;
    for(i = 7; i < 78; i++) mv->ktb[i] = mv->ktb[i-7] + 12;
    return(0);
}

int nkeyini_mec()
{
    int  i;

    mv->nktb[0] = 0;
    mv->nktb[1] = 2;
    mv->nktb[2] = 4;
    mv->nktb[3] = 5;
    mv->nktb[4] = 7;
    mv->nktb[5] = 9;
    mv->nktb[6] = 11;
    for(i = 7; i < 78; i++) mv->nktb[i] = mv->nktb[i-7] + 12;
    return(0);
}

int keyset_meb()
{
    int  i,j,m,md;

    /* printf("keyset_() mb->ksin = %d\n",mb->ksin); */
    if(mb->ksin < 8) {
        m = -1;
        for(i = 0; i < mb->ksin; i++) {
            m = m + 4;
            md = m % 7;
            for( j = 0 ; j < 11 ; j++) {
                mv->ktb[md] = mv->ktb[md] + 1;
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
                mv->ktb[md] = mv->ktb[md] - 1;
                md = md + 7;
            }
        }
    }
    for(j = 0; j < 78; j++) mv->ktbbak[j] = mv->ktb[j];
    return(0);
}

int keydmp_mec(struct mivksn *mk)
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
    if(mb->ksin) {
        if(mv->debug == 9) printf("Debug9! mb->ksin=%d\n",mb->ksin);
        printf("    %s%s (",mk->keysin[ky][ksin],wscl);
        if(mb->ksin < 8) for(i = 0; i < mb->ksin; i++) putchar('#');
        else for(i = 8 ; i <= mb->ksin ; i++) putchar('&');
        printf(")\n");
    }
    else printf("    %s%s\n",mk->keysin[ky][ksin],wscl);
    printf("    C    D    E    F    G    A    H  : OCT\n");
    for(i = 0; i < 11; i++) {
        for(j = 0; j < 7; j++) printf(" %4d",mv->ktb[k++]);
        printf("  :   %c\n",*pc);
        pc++;
    }
    return(0);
}

int notel_meb(struct mivksn *mk,char cf)
//struct mivksn *mk;
//char cf;  /* b:befor c:current a:after */
{
    int  i,j,k;
    int  l,s;
    int  lastp;        /* avalable column */
    long ll;
    int  ecod;
    int  eeod;
    int  pasf;
    static int  accia;
    char wk[10];
    long wval;
    long lagsync;
    long offsync;
    int  pport;
    long nacv;
    int  renp;

    if(mv->debug == 7) printf("%s",mv->card);
    wk[0] = mv->card[1];
    wk[1] = mv->card[2];
    wk[2] = '\0';
    sscanf(wk,"%d",&pport);
    if(pport == 0) mr->grate = 99.999;   /* %00 = 100% */
    if(pport < 0 || pport > 99) {
        printf("Error! %06d gate time rate\n",mv->line);
        //printf("Error! %06d gate time rate\n",mv->card);
    }
    mr->grate = (float)pport;
    /* printf("pport = %d mr->grate=%f\n",pport,mr->grate); */
    if(mv->card[4] == '_') {
        accia = 0;
        i = 6;
        l = 0;  // 2015.6.4
        mb->nval = 100000;
        while(mv->card[i] == ')') {
            i++;
            l++;
            if(l > 5) {
                //printf("Worning! %06d _/))))) is max.\n",mv->line);
                printf("Worning! %06d _/))))) is max.\n",mb->bnum);
                printf("%s\n",mv->card);
                return(1);
            }
            mb->nval = mb->nval / 2;
        }
        if(mv->card[i] == '.') {
            if(l == 5) {
                //printf("Worning! %06d _/))))). is limit over\n",mv->line);
                printf("Worning! %06d _/))))). is limit over\n",mb->bnum);
                printf("%s\n",mv->card);
                return(1);
            }
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                if(l == 4) {
                    //printf("Worning! %06d _/)))).. is limit over\n",mv->line);
                    printf("Worning! %06d _/)))).. is limit over\n",mb->bnum);
                    printf("%s\n",mv->card);
                    return(1);
                }
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == 'O') {
        accia = 0;
        i = 5;
        mb->nval = 400000;
        if(mv->card[i] == '.') {
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == 'o') {
        accia = 0;
        i = 6;
        mb->nval = 200000;
        if(mv->card[i] == '.') {
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == '|') {
        accia = 0;
        i = 7;
        mb->nval = 800000;
        if(mv->card[i] == '.') {
            ll = mb->nval/2;
            mb->nval = ll/2 + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == '.') {
        i = 6;
        /* mb->nval = 50000; */
        mb->nval = 100000;
        while(mv->card[i] == ')' || mv->card[i] == '>') {
            if(mv->card[i] == '>') accia = 1;
            i++;
            l++;
            if(l > 3) {
                printf("Worning! %06d ./>>> is max.\n",mv->line);
                //printf("Worning! %06d ./>>> is max.\n",mv->card);
                return(1);
            }
            mb->nval = mb->nval / 2;
        }
        if(mv->card[i] == '.') {
            if(l == 3) {
                printf("Worning! %06d ./>>>. limit over\n",mv->line);
                //printf("Worning! %06d ./>>>. limit over\n",mv->card);
                return(1);
            }
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                if(l == 2) {
                    printf("Worning! %06d ./>>.. limit over\n",mv->line);
                    //printf("Worning! %06d ./>>.. limit over\n",mv->card);
                    return(1);
                }
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == '[') pasf = 1;
    if(!pasf) return(7);
    renp = 1;
    if(i > 4 && mv->card[i] == '[') {
        j = 0;
        i++; 
        while(isdigit(mv->card[i])) {
            wk[j++] = mv->card[i++];
            if(j > 10) break;
        }
        wk[j] = '\0'; 
        if(j) sscanf(wk,"%d",&renp);
        ll = mb->nval;
        mb->nval = ll / renp;
        if(mv->debug == 7) printf("mb->nval = %ld wk = %s renp = %d\n",
            mb->nval,wk,renp);
    }
    ecod = 0;
    lastp = 69;
    j = 10;
    if( mb->bartyp == 's' ||
        mb->bartyp == 't' ||
        mb->bartyp == 'g' ||
        mb->bartyp == 'a' ||
        mb->bartyp == 'b' ||
        mb->bartyp == 'u' ||
        mb->bartyp == 'd') {
        syncetrs_meb();     /* transpose */
        j = 32;
    }
    for(i = j; i < lastp; i++) {
        if((mv->card[i] != ' ') && (mv->card[i] != '.')) ecod = 1;
    }
    if(mv->trace && ecod) {
        printf("Trace! nval=%ld -----\n ",mb->nval);
    }
    if(ecod) {
        noteon_meb(cf);
    }
    wk[0] = mv->card[69];
    wk[1] = mv->card[70];
    wk[2] = mv->card[71];
    wk[3] = mv->card[72];
    wk[4] = '\0';
    sscanf(wk,"%f",&mv->velo);
    csaccid_meb(mk);
    return(0);
}

int noteon_meb(char cf)
//char cf;  /* b:befor c:current a:after */
{
    int  i,j,k;
    int  lastp;
    int  dyna;

    k = startkp_mec(&j,&lastp);
    /*
    if(mv->debug == 9) {
        printf("Debug9! k = %d j = %d lastp = %d\n",k,j,lastp);
        printf("%s",mv->card);
    }
    */
    for(i = j; i < lastp; i++) {
        if(isdigit(mv->card[i]) || mv->card[i] == '>' ||
            mv->card[i] == '<') {
            mr->dyna = (int)(mv->card[i] - '0');
            mr->keyp = mv->ktb[k];
            if(mv->card[i] == '>') {
                mr->dyna = 5;
                mr->keyp = mr->keyp + 1;
            }
            if(mv->card[i] == '<') {
                mr->dyna = 5;
                mr->keyp = mr->keyp - 1;
            }
            mr->keyp &= 0x00ff;
            j = mr->keyp << 8;
            mr->veon = mr->dyna * mv->velo;
            if(mr->veon > 127) mr->veon = 127;
            mr->veon &= 0x00ff;
            if(mv->debug == 7 || mv->debug == 9) {
               printf("\nnoteon() mb->nval=%ld mr->grate=%2.1f ",
                   mb->nval,mr->grate);
               printf("keyp=%d tpv=%d veon=%d dyna=%d velo=%2.1f\n",
                   mr->keyp,mr->tpv,mr->veon,mr->dyna,mv->velo);
            }
            vnote_meb(cf);
        }
        k++;
    }
    return(0);
}

int vnote_meb(char cf)
//char cf;
{
    float nval;
    float velo;
    float keyp;

    nval = cs->klp * (mb->nval * mr->grate / 100.0);
    velo = cs->kvp * mr->veon;
    keyp = cs->knp * mr->keyp;
    if(cf == 'b') {
        cs->bv[mr->keyp] = cs->kbp * (nval + velo + keyp);  
        if(mv->debug == 7)
            printf("cs->bv[%d]=%6.2f nval=%6.2f velo=%6.2f keyp=%6.2f\n",
               mr->keyp,cs->bv[mr->keyp],nval,velo,keyp);
    }
    if(cf == 'c') {
        cs->cv[mr->keyp] = cs->kcp * (nval + velo + keyp);  
        if(mv->debug == 7)
            printf("cs->cv[%d]=%6.2f nval=%6.2f velo=%6.2f keyp=%6.2f\n",
               mr->keyp,cs->cv[mr->keyp],nval,velo,keyp);
    }
    if(cf == 'a') {
        cs->av[mr->keyp] = cs->kap * (nval + velo + keyp);  
        if(mv->debug == 7)
            printf("cs->av[%d]=%6.2f nval=%6.2f velo=%6.2f keyp=%6.2f\n",
               mr->keyp,cs->av[mr->keyp],nval,velo,keyp);
    }
    return(0);
}

int modline_meb(char line[82],char *msg)
//char line[82];
//char *msg;
{
    int  i,j,k;
    int  lastp;
    int  n;

    k = startkp_mec(&j,&lastp);
    for(i = j; i < lastp; i++) {
        if(line[i] == 't') line[i] = '6';
        if(line[i] == 'T') line[i] = '6';
        if(line[i] == 's') line[i] = '6';
        if(line[i] == 'S') line[i] = '6';
        if(line[i] == 'm') line[i] = '7';
        if(line[i] == 'M') line[i] = '7';
        if(line[i] == 'w') line[i] = '7';
        if(line[i] == 'W') line[i] = '7';
        if(line[i] == 'r') line[i] = '8';
        if(line[i] == 'R') line[i] = '8';
        if(isdigit(line[i])) {
            n = mv->ktbbak[k] - mv->ktb[k];
            if(n == 1) line[i] = '>';
            if(n == -1) line[i] = '<';
            if(mv->debug == 9) {
                printf(
                "modline() %s n = %d mv->ktbbak[%d] = %d mv->ktb[%d] = %d\n",
                msg,n,k,mv->ktbbak[k],k,mv->ktb[k]);
                printf("%s",line);
            }
        }
        k++;
    }
    return(0);
}

int startkp_mec(int *startp,int *lastp)
//int *startp;
//int *lastp;
{
    int  k;

    *startp = 10;
    *lastp = 69;
    k = 5;    /* mb->bartyp == 'm' */
    if(mb->bartyp == 'm') k = 5;
    if(mb->bartyp == 'l') k = 0;
    if(mb->bartyp == 'h') k = 16;
    if(mb->bartyp == 's' || mb->bartyp == 'd') {
        k = 21;
        *startp = 32;
    }
    if(mb->bartyp == 't') {
        k = 35;
        *startp = 32;
    }
    if(mb->bartyp == 'g') {
        k = 28;
        *startp = 32;
    }
    if(mb->bartyp == 'a') {
        k = 14;
        *startp = 32;
    }
    if(mb->bartyp == 'b') {
        k = 7;
        *startp = 32;
    }
    if(mb->bartyp == 'u') {
        k = 0;
        *startp = 32;
    }
    return(k);
}

/* transpose
-s0100001 p.v. mod cn:cdt mod tp
%68 _/)      0   0  0:  0   0xxx
*/
int syncetrs_meb()
{
    char wk[8];

    /* get transpose value */     
    if(mv->card[31] == 'x' || mv->card[31] == 'X' || mv->card[31] == ' ')
        return(0);
    wk[0] = mv->card[29];
    wk[1] = mv->card[30];
    wk[2] = mv->card[31];
    wk[3] = '\0'; 
    sscanf(wk,"%d",&mr->tpv);
    /*
    if(mv->debug == 9) {
        printf("Debug9! mr->tpv=%d\n",mr->tpv);
    }
    */
    return(0);
}

int accid_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    int  i,k;
    char c;
    int  startp;
    int  lastp;

    k = startkp_mec(&startp,&lastp);
    for(i = startp; i < lastp ; i++, k++) {
        c = mv->card[i];
        switch(c) {
            case '#' : mv->ktbbak[k] = mv->nktb[k] + 1; 
                       break;
            case '&' : mv->ktbbak[k] = mv->nktb[k] - 1;
                       break;
            case '*' : mv->ktbbak[k] = mv->nktb[k] + 2;
                       break;
            case '@' : mv->ktbbak[k] = mv->nktb[k] - 2;
                       break;
            case '$' : mv->ktbbak[k] = mv->nktb[k];
                       break;
            default  : break;
        }
    }
    return(0);
}

int csaccid_meb(struct mivksn *mk)
//struct mivksn *mk;  
{
    int  i,k;
    char c;
    int  startp;    
    int  lastp;

    k = startkp_mec(&startp,&lastp);
    for(i = startp; i < lastp ; i++, k++) {
        c = mv->card[i];
        switch(c) {
            case '#' : mv->ktb[k] = mv->nktb[k] + 1;
                       break;
            case '&' : mv->ktb[k] = mv->nktb[k] - 1;
                       break;
            case '*' : mv->ktb[k] = mv->nktb[k] + 2; 
                       break;
            case '@' : mv->ktb[k] = mv->nktb[k] - 2;
                       break;
            case '$' : mv->ktb[k] = mv->nktb[k];
                       break;
            default  : break;
        }   
    }       
    return(0);  
}

/* global expand clear */
int gexpclr_meb()
{
    mv->card[33] = ' ';
    mv->card[34] = '.';
    mv->card[35] = '.';
    mv->card[36] = '.';
    /* printf("%s\n",mv->card); */
    return(0);
}

