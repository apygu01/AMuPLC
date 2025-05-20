
// ----------------------
//  AMuPLC (mec1.c)
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
extern MECSYM *ms;   /* symbol table */
extern MECCPS *cp;
extern MECMPV *mp;
extern int ntrptb[128];
/* check main file xxx_x00.xxx 
        00 : mg->mfflg = 1;
*/
extern char gfile[STLN];
int tfdyn; /* {!txx} xx : dyna */
int Tfdyn; /* {!Txx} xx : dyna */
int sfdyn;
int Sfdyn;
int wfdyn;
int Wfdyn;
int mfdyn;
int Mfdyn;
int rfdyn;
int Rfdyn;
char *xabc = "0123456789abcdef";
long nvt[22];
unsigned char prctb[MAXPRC][4];

int chkmfile_mec(char *fname)
//char *fname;
{
    char c1,c2;
    char *pc;
    int  i,j;

    i = 0;
    pc = fname;
    while(*pc) {
        i++;
        pc++;
    }
    j = 0;
    pc = fname;
    while(*pc != '_') {
        pc++;
        j++;
        if( j >= i) break;
    }
    pc = pc + 2;
    c1 = *pc;
    pc++;
    c2 = *pc;
    if(c1 == '0' && c2 == '0') return(1); /* xxx_x00.xxx file(main) */
    return(0);
}

int mkeyset_mec()
{
    union meccode f8;
    int  j;

    f8.zi[0] = 0xf800;
    j = mg->mkscl << 4;
    j &= 0x00f0;
    f8.zi[0] |= j;
    mg->mksin &= 0x000f;
    f8.zi[0] |= mg->mksin;
    f8.zi[1] = 0x0000;
    j = mg->mryhm << 8;
    f8.zi[1] |= j;
    f8.zi[1] |= mg->mtime;
    if(mg->trace) {
        printf("Trace! F8(%04x%04x) mkscl=%d, ",
        f8.zi[0],f8.zi[1],mg->mkscl);
        printf("mksin=%d, mryhm=%d, mtime=%d\n",
        mg->mksin,mg->mryhm,mg->mtime);
    }
    putcode_mec(&f8);
    return(0);
}

int selfil_mec(char *mfile,int track[MXMB],char partf[MXMB][36],int port[MXMB],int channel[MXMB],int member[MXMB])
//char *mfile;
//int  track[MXMB];
//char partf[MXMB][36];
//int  port[MXMB];
//int  channel[MXMB];
//int  member[MXMB];
{
    int  i;
    int  j;
    int  k;
    char c;
    unsigned char c1,c2;
    int  cx1,cx2;
    int  cline;
    char chw[8];
    FILE *fpi;

    cline = 0;
    for(i = 0; i < MXMB; i++) {
	track[i] = 0;
        port[i] = 0;
        channel[i] = 0;
        member[i] = 0;
        for( j = 0; j < 36; j++) partf[i][j] = ' ';
    }
    strcpy(gfile,mfile);
    i = 0;
    while(gfile[i] != '_') i++;
    gfile[i] = '\0';
    strcat(gfile,".meg");
    /* printf("gfile = %s\n",gfile); */
    if((fpi = fopen(gfile,"r")) == NULL) fpi = fopen_h(mfile,"r");
    else mg->hf = 1; /* Read fn.meg */
    /* fpi = fopen_h(mfile,"rb"); */
    while(fgets(mg->card, 82, fpi)) {
        c = mg->card[0];
        if(c == 'C' && (mg->card[4] != ' ')) {
            c1 = mg->card[3]; 
            c2 = mg->card[4]; 
            for(i = 0; i < 16; i++) {
                if(c1 == xabc[i]) cx1 = i;
                if(c2 == xabc[i]) cx2 = i;
            }
            track[cline] = cx1 * 16 + cx2;
            /* printf("track[%d] = %d\n",cline,track[cline]); */
            if(track[cline] >= MXMB) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! track over than %d\n",MXMB);
                fclose(fpi);
                return(1);
	    }
            i = 8;
            for(k = 0; k < 36; k++) {
                partf[cline][k] = mg->card[i];
                i++;
            }
            i = 46;
            for(k = 0; k < 16; k++) { /* channel */
                if(mg->card[i] == 's') channel[cline] = k;
                if(mg->card[i] == 'S') channel[cline] = k;
                i++;
            }
            while(mg->card[i] == ' ') i++;
            j = 0;
            while(isdigit(mg->card[i++])) chw[j++] = mg->card[i-1];
            chw[j] = '\0';
            sscanf(chw,"%d",&port[cline]);
            while(mg->card[i] == ' ') i++;
            j = 0;
            while(isdigit(mg->card[i++])) chw[j++] = mg->card[i-1];
            chw[j] = '\0';
            sscanf(chw,"%d",&member[cline]);
            cline++;
            if(cline >= MXMB) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! cline over than %d\n",MXMB);
                return(1);
            }
        }
        if(c == '_') break;
        if(c == 'f') break;
        if(c == ESC) break;
    }
    fclose(fpi);
    return(cline);
}

int meccopy_mec(char *sfile,char *ofile)
//char *sfile;
//char *ofile;
{
    FILE *fpi,*fpo;
    //char card[82];
    char card[128];

    fpi = fopen_h(sfile,"rb");
    fpo = fopen_h(ofile,"wb");
    while(fgets(card,128,fpi)) {
        if(card[78] != '!') {
            fputs(card,fpo);
            if(mg->source || mg->list) printf("%s",card);
        }
    }
    mg->msglvl = 1;
    if(mg->msglvl >= mg->prtlvl)
    printf(" File \"%s\" copyed from \"%s\"\n",ofile,sfile);
    fclose(fpi);
    fclose(fpo);
    return(0);
}

int strcat_h(char *tos,char *from)
//char *tos,*from;
{
    while(*tos) tos++;
    while(*from) {
        *tos = *from;
        from++;
        tos++;
    }
    *tos = '\0';
    return(0);
}

int bar_mec()
{
    union meccode fb;
    char wk[8];
    int  i;
    long sync;

    mr->pprc = mr->prc;  /* repeat code count */
    mr->prc = 0;  /* clear put repeat code count */
    mr->lagb = 0; /* clear lag value */
    mr->offsetb = 0; /* clear offset value */
    if(mg->debug) printf("bar_mec() mr->sync=%ld\n",mr->sync);
    if(mm->byrd == 1) {
        sync = mr->sync;
        if(!mg->trace) if(sync < 6 && sync > -6) sync = 0; /* swing hosei */
        if(sync && !(mg->next[0] == '-' || mg->next[0] == '=')) {
            if(mr->bnum != 0) {
                mg->wrncnt++;
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl) {
                    /*
                    printf("Worning! line %d rhythm not synchronize,",mg->line);
                    printf(" sync = %ld bnum = %ld\n",mr->sync, mr->bnum);
                    */
                    printf("Worning! line %d rhythm not synchronize,",mg->line);
                    // 2025.4.6 (int)mr->sync
                    if(mr->sync > 0) printf(" less %d ",abs((int)mr->sync));
                    else printf(" over %d ",abs((int)mr->sync));
                    printf(" bnum = %d\n", mr->bnum);
                }
            }
        }
    }
    if(mr->time != 0) mr->sync = (400000/mr->time)*mr->ryhm;
    else {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! mr->time value 0 (bar_mec)\n");
    }
    mg->btyp = mg->card[1];
    wk[0] = mg->card[2];
    wk[1] = mg->card[3];
    wk[2] = '\0';
    sscanf(wk,"%d",&mr->movn);
    for(i = 0; i < 6 ; i++) wk[i] = mg->card[i+4];
    wk[++i] = '\0';
    sscanf(wk,"%d",&mr->bnum);
    fb.zi[0] = 0xfb00;
    fb.zi[0] |= mr->movn;
    fb.zi[1] = (short)mr->bnum;
    if(mg->trace)
        printf("Trace! FB(%04x%04x) movn=%d, bnum=%d , btyp(%c)\n"
           ,fb.zi[0], fb.zi[1], mr->movn, mr->bnum, mg->btyp);
    if(mm->brself) mbselck_mec();
    putcode_mec(&fb);
    return(0);
}

int mbselck_mec()
{
    long  cbn;
    long  sbn;
    long  ebn;

    if(mm->byrd == 2) return(0);
    switch(mm->brself) {
        case  1 : if(mr->movn > mm->emovn) mm->byrd = 2;
                  else if(mr->movn >= mm->smovn) mm->byrd = 1;
                  break;
        case  2 : cbn = mr->bnum;   /* for compiler bug ? */
                  sbn = mm->sbarn;
                  ebn = mm->ebarn;
                  if(cbn >= ebn) mm->byrd = 2;
                  else if(cbn >= sbn) mm->byrd = 1;
                  break;
        case  3 : cbn = mr->movn * 100000 + mr->bnum;
                  sbn = mm->smovn * 100000 + mm->sbarn;
                  ebn = mm->emovn * 100000 + mm->ebarn;
                  if(cbn >= ebn) mm->byrd = 2;
                  else if(cbn >= sbn) mm->byrd = 1;
                  /*
                  if(mr->movn <= mm->emovn && cbn >= ebn) mm->byrd = 2;
                  else if(mr->movn > mm->smovn || cbn >= sbn)
                      mm->byrd = 1;
                  */
                  break;
        default : break;
    }
    if(mg->debug == 7) {
        printf("mm->brself=%d cbn=%ld sbn=%ld,ebn=%ld ",
            mm->brself,cbn,sbn,ebn);
        printf("mm->byrd=%d (lib1)\n",mm->byrd);
    }
    return(0);
}

/* [xxxx] expression
   {xxxx} tempo
*/
int mkexpt_mec()
{
    char card[82];
    char work[8];
    int  i;
    int  ls;
    char scrf[100];

    mg->symmax = 0;
    ls = 0;
    if((mg->fpe = fopen("mecsymf.dat","r")) == NULL) {
        sprintf(scrf,"%s",TACTDIR);
        //sprintf(scrf,"%s\0",TACTDIR);
        strcat(scrf,"mecsymf.dat");
        mg->fpe = fopen_h(scrf,"r");
    }
    while(fgets(card,80,mg->fpe)) {
        if(card[0] == ESC) break;
        if(card[0] == 'f') break;
        if(card[0] == '{') {
            for(i = 0; i < 19; i++) ms->symsgn[ls][i] = card[i];
            ms->symsgn[ls][19] = '\0';
            if(!isdigit(card[29])) ms->symtyp[ls] = 0;
            else {
                for(i = 0; i < 6; i++) work[i] = card[i+24];
                work[6] = '\0';
                sscanf(work,"%d",&ms->symtyp[ls]);
            }
            if(!isdigit(card[39])) ms->symv1[ls] = 0;
            else {
                for(i = 0; i < 6 ; i++) work[i] = card[i+34];
                work[6] = '\0';
                sscanf(work,"%f",&ms->symv1[ls]);
            }
            if(!isdigit(card[49])) ms->symv2[ls] = 0;
            else {
                for(i = 0; i < 6 ; i++) work[i] = card[i+44];
                work[6] = '\0';
                sscanf(work,"%f",&ms->symv2[ls]);
            }
            if(!isdigit(card[59])) ms->symv3[ls] = 0;
            else {
                for(i = 0; i < 6 ; i++) work[i] = card[i+54];
                work[6] = '\0';
                sscanf(work,"%f",&ms->symv3[ls]);
            }
            ms->symv4[ls][0] = card[66];
            ms->symv4[ls][1] = card[67];
            ms->symv4[ls][2] = card[68];
            ms->symv4[ls][3] = card[69];
            if(mg->debug == 2) {
                printf(" %s %3d %7.1f %7.1f %7.1f",
                ms->symsgn[ls],ms->symtyp[ls],ms->symv1[ls],
                ms->symv2[ls],ms->symv3[ls]);
                printf("       %c%c%c%c\n",
                ms->symv4[ls][0],ms->symv4[ls][1],
                ms->symv4[ls][2],ms->symv4[ls][3]);
            }
            ls++;
            if(ls >= MEXP) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! mecsymf.dat record over\n");
                break;
            }
        }
    }
    mg->symmax = ls;
    fclose(mg->fpe);
    return(0);
}

int numdin_mec(char *card)
//char *card;
{
    int  i,k;
    int  n;
    char *pc;
    char cnum[10];

    k = 0;
    pc = card;
    while(*pc) {
        pc++;
        k++;
        if(k > 80) break;
    }
    pc--;
    if(!isdigit(*pc)) {
        while(!isdigit(*pc)) {
            pc--;
            k--;
            if(k <= 0) break;
        }
    }
    for(i = 0; i < 10; i++) cnum[i] = ' ';
    cnum[9] = '\0';
    for(k = 8; k >= 0; k--) {
        if(*pc == ' ') break;
        cnum[k] = *pc;
        pc--;
    }
    sscanf(cnum,"%d",&n);
    /* printf("cnum=%s n=%d\n",cnum,n); */
    return(n);
}

int putcode_mec(union meccode *fx)
//union meccode *fx;
{
    unsigned char c;
    int  notenb;
    int  k;
    unsigned char zc[4];

    /* printf("Put pass byrd=%d\n",mm->byrd); */
    if(mm->byrd != 1) return(0);
    mr->tcnt++;
    if(!mr->ffflg) {
        if(mr->tcnt == 10) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl) {
                printf("Error! ffxxyyzz code not outputed before\n");
                if(mr->port == 0 && mr->trak > 0)
                printf("Check conducters track change '+' -> 'C' ?\n");
            }
        }
        return(0);
    }
    if(mm->comptyp == 0) {
        zc[0] = fx->zc[1];
        zc[1] = fx->zc[0];
        zc[2] = fx->zc[3];
        zc[3] = fx->zc[2];
    }
    if(mm->comptyp == 1) {
        zc[0] = fx->zc[0];
        zc[1] = fx->zc[1];
        zc[2] = fx->zc[2];
        zc[3] = fx->zc[3];
    }
    if(mm->comptyp == 2) {
        zc[0] = fx->zc[1];
        zc[1] = fx->zc[0];
        zc[2] = fx->zc[3];
        zc[3] = fx->zc[2];
    }
    if(zc[0] == 0xfd) {
        /* note on, note off, pol. key pres. transpose */
        c = zc[1] & 0xf0;
        switch (c) {
            case  0x80 : /* note off */
                         k = zc[2];
                         if(k < 0 || k > 127) {
                         mg->errcnt++;
                         mg->msglvl = 3;
                         if(mg->msglvl >= mg->prtlvl) printf(
                         "Error! note number %d at putcode_mec()\n",k);
                         return(0);
                         }
                         notenb = ntrptb[k];
                         ntrptb[k] = 0;
                         zc[2] = notenb & 0x007f;
                         break;
            case  0x90 : /* note on */
                         k = zc[2];
                         if(k < 0 || k > 127) {
                             mg->errcnt++;
                             mg->msglvl = 3;
                             if(mg->msglvl >= mg->prtlvl) {
                                 printf("Error! note number %d",k);
                                 printf(" at putcode_mec()\n");
                             }
                             return(0);
                         }
                         if(zc[3] == 0x00) { /* note off */
                             notenb = ntrptb[k];
                             ntrptb[k] = 0;
                             zc[2] = notenb & 0x007f;
                         }
                         else { /* note on */
                             if(ntrptb[k] != 0) {  /* note off out */
                                 notenb = ntrptb[k];
                                 zc[2] = notenb & 0x007f;
                                 /* channel chg */
                                 if(mp->mpvf) mpvchan_mec(zc[0],
                                     &zc[1],zc[2],0x00);
                                 if(mg->object) {
                                     fputc(zc[0], mg->fpo);
                                     fputc(zc[1], mg->fpo);
                                     fputc(zc[2], mg->fpo);
                                     fputc(0x00, mg->fpo);
                                     mr->tcnt++;
                                 }
                             }
                             /*
                             notenb = fx->zc[3] + mr->trnsv + mg->bitrns;
                             */
                             notenb = zc[2] + mr->trnsp + mr->trnsv;
                             if( notenb < 0 || notenb > 127) return(0);
                             ntrptb[k] = notenb;
                             zc[2] = notenb & 0x007f;
                         }
                         break;
            case  0xa0 : /* polyphonic key pressure / aftertouch */
                         /* 
                         notenb = fx->zc[3] + mr->trnsv + mg->bitrns;
                         */
                         notenb = zc[2] + mr->trnsp + mr->trnsv;
                         if( notenb < 0 || notenb > 127) return(0);
                         zc[2] = notenb & 0x007f;
                         break;
            default    : break;
        }
    }
    prctb[mr->prc][0] = zc[0];
    prctb[mr->prc][1] = zc[1];
    prctb[mr->prc][2] = zc[2];
    prctb[mr->prc][3] = zc[3];
    mr->prc++;
    if(mr->prc > MAXPRC) {
        mg->wrncnt++;
        mg->msglvl = 2;
        if(mg->msglvl >= mg->prtlvl)
        printf("Worning!  putcode() mr->prc %d over than MAXPRC\n",mr->prc);
        mr->prc = 0;
    }
    if(mp->mpvf) mpvchan_mec(zc[0],&zc[1],zc[2],zc[3]);  /* chan chg */
    if(mg->object) {
        fputc(zc[0], mg->fpo);
        fputc(zc[1], mg->fpo);
        fputc(zc[2], mg->fpo);
        fputc(zc[3], mg->fpo);
        mr->tcnt++;
    }
    if(mg->debug == 1) printf("debug!   (%02x%02x%02x%02x) mr->tcnt=%ld\n",
        zc[0],zc[1],zc[2],zc[3],mr->tcnt);
    return(0);
}

/* /%/ repeat : put repeat chord */
int putrepc_mec()
{
    int  i;

    if(mg->debug == 1) printf("putrepc_mec() mr->pprc = %d\n",mr->pprc);
    mr->prc = mr->pprc;
    mr->sync = 0;
    for( i = 1; i < mr->pprc; i++) { /* i = 0 : cancel fb code */
        if(mp->mpvf) mpvchan_mec(prctb[1][0],
            &prctb[i][1],prctb[i][2],prctb[i][3]); /* chan chg */
        if(mg->object) {
            fputc(prctb[i][0], mg->fpo);
            fputc(prctb[i][1], mg->fpo);
            fputc(prctb[i][2], mg->fpo);
            fputc(prctb[i][3], mg->fpo);  
        }
        mr->tcnt++;
        if(mg->debug == 1) printf("debug!   (%02x%02x%02x%02x) mr->tcnt=%ld\n",
            prctb[i][0],prctb[i][1],prctb[i][2],prctb[i][3],mr->tcnt);
    }
    return(0);
}

int exprnum_mec()
{
    union meccode f9;
    int  n;

    n = numdin_mec(mg->card);
    if(n < 0) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d \"%s\"\n", mg->line, mg->card);
        return(1);
    }
    if(n > 32767) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d expression limit is 32767 (%d)\n",
            mg->line, n);
        return(2);
    }
    f9.zi[0] = 0xf900;
    f9.zi[1] = n;
    mr->expr = n;
    if(mg->trace) printf("Trace! F9(%04x%04x) expression no. %d\n",
        f9.zi[0],f9.zi[1], n);
    return(0);
}

int key_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    union meccode f8;
    char wk[8];
    char c;
    int  i,j;
    int  err;
    int  pas;
    int  tpas;
    int  rf;

    keyini_mec();
    f8.zi[0] = 0xf800;    /* intial value */
    f8.zi[1] = 0;
    err = 0;
    pas = 0;
    tpas = 0;
    i = 0;
    while(mg->card[i]) {
        if(mg->card[i] == '}') break;
        if(mg->card[i] == 'T') tpas = 1;
        if(mg->card[i] == 't') tpas = 1;
        i++;
    }
    while(mg->card[i] != 'r' && mg->card[i] != 'l') {
        i--;
        if(i < 0) break;
        pas = 1;
    }
    wk[0] = '\0';
    if(pas) {
        i = i - 2;
        for(j = 0; j < 3; j++) wk[j] = mg->card[i++];
        wk[3] = '\0';
    }
    if(!strcmp(wk,"dur")) mr->kscl = 1;
    if(!strcmp(wk,"oll")) mr->kscl = 2;
    if(!strcmp(wk,"jor")) mr->kscl = 3;
    if(!strcmp(wk,"nor")) mr->kscl = 4;
    if(!strcmp(wk,"dur")) cp->key = 1;
    if(!strcmp(wk,"oll")) cp->key = 2;
    if(!strcmp(wk,"jor")) cp->key = 1;
    if(!strcmp(wk,"nor")) cp->key = 2;
    j = mr->kscl << 4;
    j &= 0x00f0;
    f8.zi[0] |= j;
    if(mg->trace) printf("Trace! F8(%04x%04x) kscl=%d\n",
        f8.zi[0], f8.zi[1], mr->kscl);
    i = 1;
    if(mg->card[1] == '$') i = 2;
    mr->ksin = 0;
    if( mg->card[i] == '#') {
        while(mg->card[i] == '#') {
            i++;
            mr->ksin++;
        }
    }
    if(mg->card[i] == '&') {
        mr->ksin = 7;
        while(mg->card[i] == '&') {
            i++;
            mr->ksin++;
        }
    }
    if(mr->ksin < 0 || mr->ksin > 14) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d key signature\n",mg->line);
        mr->ksin = 0;
        return(-1);
    }
    mr->ksin &= 0x000f;
    f8.zi[0] |= mr->ksin;
    if(mg->trace) printf("Trace! F8(%04x%04x) ksin=%d\n",
        f8.zi[0], f8.zi[1], mr->ksin);
    c = mg->card[i];
    rf = 0; /* ryhm pass flah $##4/4 or $## ? */
    if(isalnum(c)) {
        rf = 1;
        if(c == 'c' || c == 'C') {
            mr->ryhm = 4;
            mr->time = 4;
        }
        else {
            if(!isdigit(c)) err = 1;
            j = 0;
            mr->ryhm = 0;
            while(mg->card[i]) {
                if(mg->card[i] == '/') break;
                wk[j] = mg->card[i];
                i++;
                j++;
                if(j > 8) err =2;
            }
            if(j == 0 || j > 3) err = 3;
            if(err) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d rithm data error\n", mg->line);
                return(err);
            }
            wk[j] = '\0';
            sscanf(wk,"%d",&mr->ryhm);
            if(mr->ryhm > 255) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d rithm greater than 255\n",mg->line);
                return(3);
            }
            j = 0;
            i++;
            while(isdigit(mg->card[i])) {
                wk[j] = mg->card[i];
                i++;
                j++;
                if(j > 8) {
                    mg->errcnt++;
                    mg->msglvl = 3;
                    if(mg->msglvl >= mg->prtlvl)
                    printf("Error! %06d rithm(time) data error\n",
                    mg->line);
                    return(4);
                }
            }
            if(j == 0 || j > 3) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d rithm(time) data error\n",
                mg->line);
                return(5);
            }
            wk[j] = '\0';
            sscanf(wk,"%d",&mr->time);
            if(mr->time > 255) {
                mg->errcnt++;
                mg->msglvl = 3;
                if(mg->msglvl >= mg->prtlvl)
                printf("Error! %06d ryhm(time) greater than 255\n",
                mg->line);
                return(6);
            }
        }
        f8.zi[1] = 0x0000;
        j = mr->ryhm << 8;
        f8.zi[1] |= j;
        if(mg->trace) printf("Trace! F8(%04x%04x) ryhm=%d\n",
            f8.zi[0], f8.zi[1], mr->ryhm);
        f8.zi[1] &= 0xff00;
        f8.zi[1] |= (mr->time & 0x00ff);
        if(mg->trace) printf("Trace! F8(%04x%04x) time=%d\n",
            f8.zi[0], f8.zi[1], mr->time);
    }
    if(!mg->mtime) {
        mg->mkscl = mr->kscl;
        mg->mksin = mr->ksin;
        mg->mryhm = mr->ryhm;
        mg->mtime = mr->time;
    }
    if(tpas) {
        pas = 0;
        i = 8;
        while(mg->card[i]) {
            if(mg->card[i] == 'T' || mg->card[i] == 't') {
                while(mg->card[i] != ' ') i++;
                pas = 1;
                break;
            }
            i++;
        }
        wk[0] = '\0';
        if(pas) {
            j = 0;
            while(mg->card[i]) {
                wk[j] = mg->card[i];
                i++;
                j++;
                if(j > 8) break;
            }
            wk[j] = '\0';
            sscanf(wk,"%d",&mg->bitrns);
            if(mg->trace) printf("Trace! beau inst. trns mg->bitrns=%d\n",
                mg->bitrns);
        }
    }
    putcode_mec(&f8);
    keyset_mec();
    if(mg->debug == 1) keydmp_mec(mk);
    return(rf);
}

int keyini_mec()
{
    int  i;

    mg->ktb[0] = 0;
    mg->ktb[1] = 2;
    mg->ktb[2] = 4;
    mg->ktb[3] = 5;
    mg->ktb[4] = 7;
    mg->ktb[5] = 9;
    mg->ktb[6] = 11;
    for(i = 7; i < 78; i++) mg->ktb[i] = mg->ktb[i-7] + 12;
    return(0);
}

int nkeyini_mec()
{
    int  i;

    mg->nktb[0] = 0;
    mg->nktb[1] = 2;
    mg->nktb[2] = 4;
    mg->nktb[3] = 5;
    mg->nktb[4] = 7;
    mg->nktb[5] = 9;
    mg->nktb[6] = 11;
    for(i = 7; i < 78; i++) mg->nktb[i] = mg->nktb[i-7] + 12;
    return(0);
}

int keyset_mec()
{
    int  i,j,m,md;

    if(mr->ksin < 8) {
        m = -1;
        for(i = 0; i < mr->ksin; i++) {
            m = m + 4;
            md = m % 7;
            for( j = 0 ; j < 11 ; j++) {
                mg->ktb[md] = mg->ktb[md] + 1;
                md = md + 7;
            }
        }
    }
    else {
        m = 3;
        for(i = 7; i < mr->ksin; i++) {
            m = m + 3;
            md = m % 7;
            for( j = 0 ; j < 11 ; j++) {
                mg->ktb[md] = mg->ktb[md] - 1;
                md = md + 7;
            }
        }
    }
    return(0);
}

int mksym_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    int  i,j;

    mk->keysin[0][0] = "C";
    mk->keysin[0][1] = "G";
    mk->keysin[0][2] = "D";
    mk->keysin[0][3] = "A";
    mk->keysin[0][4] = "E";
    mk->keysin[0][5] = "H";
    mk->keysin[0][6] = "Fis";
    mk->keysin[0][7] = "Cis";
    mk->keysin[0][8] = "F";
    mk->keysin[0][9] = "B";
    mk->keysin[0][10] = "Es";
    mk->keysin[0][11] = "As";
    mk->keysin[0][12] = "Des";
    mk->keysin[0][13] = "Ges";
    mk->keysin[0][14] = "Ces";
    mk->keysin[1][0] = "a";
    mk->keysin[1][1] = "e";
    mk->keysin[1][2] = "h";
    mk->keysin[1][3] = "fis";
    mk->keysin[1][4] = "cis";
    mk->keysin[1][5] = "gis";
    mk->keysin[1][6] = "dis";
    mk->keysin[1][7] = "ais";
    mk->keysin[1][8] = "d";
    mk->keysin[1][9] = "g";
    mk->keysin[1][10] = "c";
    mk->keysin[1][11] = "f";
    mk->keysin[1][12] = "b";
    mk->keysin[1][13] = "es";
    mk->keysin[1][14] = "as";
    if(mg->debug == 2) {
        for(i = 0; i < 2; i++) {
            for(j = 0; j < 15 ; j++) 
            printf("mk->keysin[%d][%d] = %s\n",i,j,mk->keysin[i][j]);
        }
    }
    return(0);
}

int keydmp_mec(struct mecksn *mk)
//struct mecksn *mk;
{
    int  i,j,k;
    int  ky;
    int  ksin;
    char *pc;
    char wscl[8];

    pc = "321Cc*234567";
    wscl[0] = '\0';
    ky = 0;
    switch(mr->kscl) {
        case  1 : strcat_h(wscl,"_dur");
                  ky = 0;
                  break;
        case  2 : strcat_h(wscl,"_moll");
                  ky = 1;
                  break;
        case  3 : strcat_h(wscl,"_major");
                  ky = 0;
                  break;
        case  4 : strcat_h(wscl,"_minor");
                  ky = 1;
                  break;
        default : strcat_h(wscl,"_dur ?");
                  break;
    }
    /* turboc bug ***** */
    if(mg->debug == 1) {
        for(i = 0; i < 2; i++) {
            for(j = 0; j < 15 ; j++) 
            printf("mk->keysin[%d][%d] = %s\n",i,j,mk->keysin[i][j]);
        }
        printf("ky = %d  mr->ksin = %d\n",ky,mr->ksin);
        printf("%s\n",mk->keysin[ky][mr->ksin]);
    }
    /* end bug */
    ksin = mr->ksin;
    k = 0;
    if(mr->ksin) {
        if(mg->debug == 2) printf("Debug2! mr->ksin=%d\n",mr->ksin);
        printf("    %s%s (",mk->keysin[ky][ksin],wscl);
        if(mr->ksin < 8) for(i = 0; i < mr->ksin; i++) putchar('#');
        else for(i = 8 ; i <= mr->ksin ; i++) putchar('&');
        printf(")\n");
    }
    else printf("    %s%s\n",mk->keysin[ky][ksin],wscl);
    printf("    C    D    E    F    G    A    H  : OCT\n");
    for(i = 0; i < 11; i++) {
        for(j = 0; j < 7; j++) printf(" %4d",mg->ktb[k++]);
        printf("  :   %c\n",*pc);
        pc++;
    }
    return(0);
}

/* FExxxxxx code no sakusei
   */
int final_mec()
{
    union meccode fe;
    long lw;

    lw = mr->tcnt;
    lw++;
    lsconv_h(&fe,lw);
    fe.zi[0] |= 0xfe00;
    if(mg->trace) 
        printf("Trace! FE(%04x%04x) out code count=%ld\n",
        fe.zi[0], fe.zi[1], lw);
    putcode_mec(&fe);
    return(0);
}

/* check {xxxxxx} by symbol table */
int expression_mec(union meccode *ff)
//union meccode *ff;
{
    union meccode fa,f7,f9;
    int  i,j,k;
    int  pas;
    int  symv1;
    int  symv2;
    int  symv3;
    char swk[80];
    char swc[80];
    unsigned int ffzc;

    if(mg->card[1] == '/') return(0);   /* {/00 Seq/////////..////} line */
    if(mg->card[1] == '%') return(0);    /* {%xx} line */
    if(mg->card[1] == '\\') return(0);   /* {\kxx} line kadenz */
    if(mg->card[1] == '*') return(0);    /* {*xx} line comment */
    if(mg->card[1] == '#') return(0);    /* {#assin} & define  */
    if(mg->card[1] == '&') {
         /* {&define ???} mec command */
         return(0);
    }
    if(mg->card[1] == '!') {
         /* {!I} Chord progression  */
         return(0);
    }
    if(mg->card[1] == ';') {
         efdyna_mec();    /* {!xx} phrase */
         return(0);
    }
    spacecard_mec();
    for(i = 0 ; i < 5; i++) swk[i] = tolower(mg->card[i]);
    swk[i] = '\0';
    if(!strcmp(swk,"{lag ")) {
        lag_offset_mec();
        return(0);
    }
    if(!strcmp(swk,"{offs")) {
        lag_offset_mec();
        return(0);
    }
    if(!strcmp(swk,"{m.m.")) {
        if(mr->bnum == 0) exptemp_mec();
        return(0);
    }
    i = 0;
    j = 0;
    while(mg->card[i] != '}') {
        if(mg->card[i] != ' ') {
            swc[j] = tolower(mg->card[i]);
            j++;
        }
        i++;
        if(i >= 30) break;
        if(j >= 19) break;
    }
    swc[j++] = '}';
    swc[j] = '\0';
    pas = 0;
    for(i = 0; i < mg->symmax ; i++) {
        j = 0;
        k = 0;
        while(ms->symsgn[i][j] != '}') {
            if(ms->symsgn[i][j] != ' ') {
                swk[k] = tolower(ms->symsgn[i][j]);
                k++;
            }
            j++;
            if(j >= 30) break;
            if(k >= 19) break;
        }
        swk[k++] = '}';
        swk[k] = '\0';
        if(!strcmp(swc,swk)) {
            mr->egrp = ms->symtyp[i];
            symv1 = ms->symv1[i];
            symv2 = ms->symv2[i];
            symv3 = ms->symv3[i];
            pas = 1;
        }
    }
    if(!pas) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d symbol not found \"%s\"\n",
            mg->line,mg->card);
        return(1);
    }
    if(mr->egrp >= 1 && mr->egrp <= 8) {
        /* save dyna */
        tfdyn = fr->tfdyn;
        Tfdyn = fr->Tfdyn;
        sfdyn = fr->sfdyn;
        Sfdyn = fr->Sfdyn;
        wfdyn = fr->wfdyn;
        Wfdyn = fr->Wfdyn;
        mfdyn = fr->mfdyn;
        Mfdyn = fr->Mfdyn;
        rfdyn = fr->rfdyn;
        Rfdyn = fr->Rfdyn;
        /* Set dyna */
        fr->tfdyn = symv3;
        fr->Tfdyn = symv3;
        fr->sfdyn = symv3;
        fr->Sfdyn = symv3;
        fr->wfdyn = symv3;
        fr->Wfdyn = symv3;
        fr->mfdyn = symv3;
        fr->Mfdyn = symv3;
        fr->rfdyn = symv3;
        fr->Rfdyn = symv3;
        if(mg->trace) printf("Trace! efect{xx} dyna=%d\n",symv3);
    }
    ffzc = ff->zi[0] & 0xff00;
    if(mr->egrp >= 200 && mr->egrp < 1000) {
       mr->expr1 = symv1;
       mr->expr2 = symv2;
       mr->expr3 = symv3;
       f9.zi[0] = 0xf900;
       f9.zi[1] = mr->egrp;
       if(mg->trace)
           printf("Trace! F9(%04x%04x) egrp=%d\n",
           f9.zi[0],f9.zi[1],mr->egrp);
       if(ffzc == 0xff00) putcode_mec(&f9);
       return(0);
    }
    if(mr->egrp >= 100 && mr->egrp < 200) {
        mr->tsym = mr->egrp;
        mr->temp1 = symv1;
        mr->temp2 = symv2 * 100;
        mr->temp3 = symv3;
        fa.zi[0] = 0xfa00;
        mr->tsym &= 0x00ff;
        fa.zi[0] |= mr->tsym;
        fa.zi[1] = mr->temp2;
        if(mg->trace)
        printf("Trace! FA(%04x%04x) tsym=%d, temp2=%d\n",
        fa.zi[0],fa.zi[1], mr->tsym, mr->temp2);
        if(ffzc == 0xff00) putcode_mec(&fa);
        if(!mg->mtempo) {
            mg->mtsymb = mr->tsym;
            mg->mtempo = mr->temp2;
        }
        return(0);
    }
    if(mr->egrp >= 1000 && mr->egrp < 10000) {
       f7.zi[0] = 0xf700;
       mr->inst = mr->egrp;
       mr->trns = symv3;
       mr->trns &= 0x00ff;
       f7.zi[0] |= mr->trns;
       f7.zi[1] = mr->inst;
       if(mg->trace) printf(
           "Trace! F7(%04x%04x) inst=%d, trns=%d\n",
           f7.zi[0],f7.zi[1], mr->inst, mr->trns);
       if(ffzc == 0xff00) putcode_mec(&f7);
    }
    return(0);
}

int initdyna_mec()
{
    fr->tfdyn = 5;      /* {!txx} tril dyna */
    fr->Tfdyn = 5;      /* {!Txx} Tril dyna */
    fr->sfdyn = 5;      /* {!sxx} turn dyna */
    fr->Sfdyn = 5;      /* {!Sxx} Turn dyna */
    fr->mfdyn = 5;      /* {!mxx} mordent dyna */
    fr->Mfdyn = 5;      /* {!Mxx} Mordent dyna */
    fr->wfdyn = 5;      /* {!wxx} duble turn dyna */
    fr->Wfdyn = 5;      /* {!Wxx} Duble turn dyna */
    fr->rfdyn = 5;      /* {!rxx} tril... */
    fr->Rfdyn = 5;      /* {!Rxx} Tril... */
    tfdyn = 5;          /*  tril dyna */
    Tfdyn = 5;          /*  Tril dyna */
    sfdyn = 5;          /*  turn dyna */
    Sfdyn = 5;          /*  Turn dyna */
    mfdyn = 5;          /*  mordent dyna */
    Mfdyn = 5;          /*  Mordent dyna */
    wfdyn = 5;          /*  duble turn dyna */
    Wfdyn = 5;          /*  Duble turn dyna */
    rfdyn = 5;          /*  tril... */
    Rfdyn = 5;          /*  Tril... */
    return(0);
}

/* Reset phrase dyna */
int rsphdyna_mec()
{
    /* Restore dyna */
    fr->tfdyn = tfdyn;
    fr->Tfdyn = Tfdyn;
    fr->sfdyn = sfdyn;
    fr->Sfdyn = Sfdyn;
    fr->wfdyn = wfdyn;
    fr->Wfdyn = Wfdyn;
    fr->mfdyn = mfdyn;
    fr->Mfdyn = Mfdyn;
    fr->rfdyn = rfdyn;
    fr->Rfdyn = Rfdyn;
    return(0);
}

int exptemp_mec()
{
    union meccode fa;
    int  i,j;
    int  k,l;
    long ll;
    int  temp;
    int  pasf;
    char wk[10];

    nvt[0] = 800000;    /* 0 : |O| */
    nvt[1] = 700000;    /* 1 : O.. */
    nvt[2] = 600000;    /* 2 : O.  */
    nvt[3] = 400000;    /* 3 : whole */
    nvt[4] = 350000;    /* 4 : o/.. */
    nvt[5] = 300000;    /* 5 : o/.  */
    nvt[6] = 200000;    /* half */
    nvt[7] = 175000;    /* _/.. */
    nvt[8] = 150000;    /* _/.  */
    nvt[9] = 100000;    /* 4th */
    nvt[10] = 87500;    /* _/).. */
    nvt[11] = 75000;    /* _/).  */
    nvt[12] = 50000;    /* 8th */
    nvt[13] = 43750;    /* _/)).. */
    nvt[14] = 37500;    /* _/)). */
    nvt[15] = 25000;    /* 16th */
    nvt[16] = 21875;    /* _/))).. */
    nvt[17] = 18750;    /* _/))). */
    nvt[18] = 12500;    /* 32th */
    nvt[19] =  9375;    /* _/)))). */
    nvt[20] =  6250;    /* 64th */
    nvt[21] =  3125;    /* 128th */
    i = 0;
    pasf = 0;
    mr->tsym = 0;   /* time symbol no. 0 */
    k = 0;
    l = 0;
    mr->nval = 0;
    while(mg->card[k]) k++;
    while(mg->card[i] != ' ') i++;
    i++;
    if(mg->card[i] == '_') {
        i = i + 2;
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
    if(mg->card[i] == 'O') {
        i++;
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
    if(mg->card[i] == 'o') {
        i = i + 2;
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
    if(mg->card[i] == '|') {
        i = i + 3;
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
    while(mg->card[i] != '=') {
        i++;
        if(i > k) {
            mg->errcnt++;
            mg->msglvl = 3;
            if(mg->msglvl >= mg->prtlvl)
            printf("Error! %06d tempo \'=\' symbol not found\n",mg->line);
            return(1);
        }
    }
    while(!isdigit(mg->card[i])) i++;
    j = 0;
    while(isdigit(mg->card[i])) {
        wk[j] = mg->card[i];
        i++;
        j++;
    }
    wk[j] = '\0';
    sscanf(wk,"%d",&temp);
    for(j = 0; j < 21; j++) if(mr->nval == nvt[j]) break;
    mr->tsym = j;
    mr->temp = temp * 100;
    /* mr->temp = temp*nvt[j]/10000;      *10 */
    fa.zi[0] = 0xfa00;
    fa.zi[0] |= j;
    fa.zi[1] = mr->temp;
    if(mg->trace)
    printf("Trace! FA(%04x%04x) tsym=%d temp=%d nval=%ld\n",
    fa.zi[0],fa.zi[1], mr->tsym, mr->temp, mr->nval);
    if(!mg->mtempo) {
        mg->mtsymb = mr->tsym;
        mg->mtempo = mr->temp;
    }
    if(pasf) {
        putcode_mec(&fa);
        return(0);
    }
    mg->errcnt++;
    mg->msglvl = 3;
    if(mg->msglvl >= mg->prtlvl) printf("Error! %06d tempo\n",mg->line);
    return(1);
}

int dump_mec(char *inf)
//char *inf;
{
    int  c1,c2,c3,c4;
    long l;
    int  i;
    char wk[STLN];
    long oct;
    int  mod;
    FILE *fpi;

    i = 0;
    oct = 0;
    mod = 0;
    strcpy(wk,inf);
    while(wk[i++] != '.');
    wk[i] = '\0';
    strcat(wk,"mec");
    printf("\"%s\" dump start",wk);
    l = 0;
    fpi = fopen_h(wk,"rb");
    while(1) {
        c1 = fgetc(fpi);
        c2 = fgetc(fpi);
        c3 = fgetc(fpi);
        c4 = fgetc(fpi);
        if(!mod) printf("\n%07ld",oct);
        printf(" %02x%02x%02x%02x",c1,c2,c3,c4);
        l++;
        oct++;
        mod = oct % 8;
        if(c1 == 0xfe) break;
    }
    printf("\n\"%s\" input word count = %ld\n",wk,l);
    fclose(fpi);
    return(0);
}

FILE *fopen_h(char *file, char *mode)
//char *file;
//char *mode;
{
    FILE *fp;

    if((fp = fopen(file, mode)) == NULL) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Can't open file %s mode %s\n",file, mode);
        exit(1);
    }
    return(fp);
}

int renget_mec()
{
    int  i,j;
    int  renp;
    int  rept;
    char wk[4];

    i = 4;
    renp = 0;
    rept = 0;
    mr->renp = 1;
    mr->rept = 1;
    while(mg->card[i]) {
        if(mg->card[i] == '[') renp = 1;
        if(mg->card[i] == '*') rept = 1;
        i++;
        if(i > 9) break;
    }
    if(renp) {
        i = 0;
        while(mg->card[i] != '[') i++;
        j = 0;
        i++;
        while(isdigit(mg->card[i])) wk[j++] = mg->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mr->renp);
    }
    if(rept) {
        i = 0;
        while(mg->card[i] != '*') i++;
        j = 0;
        i++;
        while(isdigit(mg->card[i])) wk[j++] = mg->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mr->rept);
    }
    return(0);
}

int movbar_mec()
{
    union meccode fb,fc;
    int  i;
    int  dari;
    char wk[8];

    dari = 0;
    mr->sync = 0;
    mg->btyp = mg->card[1];
    wk[0] = mg->card[2];
    wk[1] = mg->card[3];
    wk[2] = '\0';
    sscanf(wk,"%d",&mr->movn);
    fb.zi[0] = 0xfb00;
    fb.zi[0] |= mr->movn;
    for(i = 0; i < 6 ; i++) wk[i] = mg->card[i+4];
    wk[++i] = '\0';
    sscanf(wk,"%d",&mr->bnum);
    fb.zi[1] = (short)mr->bnum;
    if(mg->trace)
     printf("Trace! FB(%04x%04x) movn=%d bnum=%d btyp(%c)\n"
     ,fb.zi[0], fb.zi[1], mr->movn, mr->bnum, mg->btyp);
    if(mm->brself) mbselck_mec();
    putcode_mec(&fb);
    for(i = 0; i < 128; i++) if(mg->ntb[i] == 1) dari = 1;
    if(dari) {
        syncout_mec(mr->sync,1); /* lag */
        fraseck_mec(mr->sync);
    }
    return(0);
}

int lsconv_h(union meccode *fx,unsigned long lval)
//union meccode *fx;
//unsigned long lval;
{
    union {
        unsigned long  l;
        unsigned short s[2];
    } w;

    w.l = lval;
    if(mm->comptyp == 0) {
        fx->zi[0] = w.s[1];
        fx->zi[1] = w.s[0];
    }
    if(mm->comptyp == 1) {
        fx->zi[0] = w.s[0];
        fx->zi[1] = w.s[1];
    }
    if(mm->comptyp == 2) {
        fx->zi[0] = w.s[1];
        fx->zi[1] = w.s[0];
    }
    return(0);
}

int help_main()
{
    printf("\n Music Edit File Compiler V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    if(!strcmp(SUPPORT,"Release")) {
        printf(" mec  fn    [fn/xyy]\n");
        printf("             fn : File name\n");
        printf("                x:Sequence yy:Track\n");
        printf(" usage: e.g.\n");
        printf("        mec www\n");
        printf("        mec www/015\n");
        printf("\n");
        return(0);
    }
    printf(" mec  fn    [,fn] [options]\n");
    printf("      fn1/x  fn2  [ -it   ]\n");
    printf("      fn/yy       [ -dp   ]\n");
    printf("      fn/x;y      [ -s    ]\n");
    printf("      fn_x,y      [ -sl   ]\n");
    printf("      fn/xyy      [ -l    ]\n");
    printf("      fn_xyy      [ -o -s ]\n");
    printf("      fn(sss,eee) [ -t ] sss:start movement eee:end movement\n");
    printf("      options : -smf(Put SMF Meta Events)\n");
    printf("                -t(trace) -s(source) -l(list) -o(object)\n");
    printf("                -sl(source line number)\n");
    printf("                -d(debug d1,d2,d3,d4,d5)\n");
    printf("                -d(d6:Dynamic Expand -d8:Multi chan. P.V)\n");
    printf("                -dp(dump) -no(nobject) -nx(no expand) -h(help)\n");
    printf("                -m0 .. -m3 (message level 0 to 3 default:-m1)\n");
    printf("                -it,-impv(Ignore Temperament/Multi chan Pit.V)\n");
    printf(" ex.1 mec inf -t -d -s     : Trace & Debug & Source option\n");
    printf("      mec inf -it          : Ignore Temperment\n");
    printf(" ex.2 mec inf inf2 -dump   : Dump middle data file\n");
    printf("\n");
    return(0);
}

int fhelp_mec(int help)
//int help;
{
    int  c;
    FILE *fpd;
    char scrf[100];
    
    printf("\n Music Edit File Compiler V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    if(help == 2) {
      if((fpd = fopen("mechlp2.dat","r")) == NULL) {
        sprintf(scrf,"%s",TACTDIR);
        strcat(scrf,"mechlp2.dat");
        fpd = fopen_h(scrf,"r");
      }
    }
    else {
        if((fpd = fopen("mechlp.dat","r")) == NULL) {
          sprintf(scrf,"%s",TACTDIR); 
          //sprintf(scrf,"%s\0",TACTDIR); 
          strcat(scrf,"mechlp.dat");
          /* printf("scrf = %s\n",scrf); */
          fpd = fopen_h(scrf,"r");
        }
    }
    while((c = fgetc(fpd)) != EOF) putchar(c);
    fclose(fpd);
    return(0);
}
