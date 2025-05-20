
// ----------------------
//  AMuPLC (meg4.c)
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
extern MEBCPS *cp;
extern MEBGRT *gr,*gd,*ga,*gm,*gs,*gc;

/*              0123456789012345678901234567890123456789012345 */
char pwmid[] = " ...  xx xxx.x   xxx.x  xxx.x%  xx:xx:xx/xx.xx";
/* char pwmid[] = "xx xxx.x   xxx.x  xxx.x%  xx:xx:xx/xx.xx"; */
char pwsmp[] = "xxx  ...  xx:xx:xx/xx.xx  xxxx  ....";

/* 
%68 _/                                 xx  xx.x   xxx.x  xxx.x%  xx:xx:xx/xx..
            pno  msg  hh:mm:ss/ff.ff  evno  ainf  tempo    dymc  hh:mm:ss/ff..
s68 _/      xxx  ...  xx:xx:xx/xx.xx  xxxx  ....   38.0  130.0%  xx:xx:xx/xx..
*/
int beausyms_meb()
{
    int  i,j;

    if(mb->smpmid == 0) return(0);
    for(i = 1; i < 12; i++) if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
    if(mb->smpmid == 1) {    /* midi & smpte format */
        for(i = 12; i < 33; i++) mv->card[i] = ' ';
        for(i = 33,j = 0; i < 79; i++,j++) mv->card[i] = pwmid[j];
        mv->card[0] = 's';
    }
    if(mb->smpmid == 2) {    /* quing & smpte format */
        for(i = 12,j = 0; i < 48 ; i++,j++) mv->card[i] = pwsmp[j];
        for(i = 50,j = 11; i < 79 ; i++,j++) mv->card[i] = pwmid[j];
        mv->card[0] = 'c';
    }
    if(mb->smpmid == 3) mv->card[0] = '%';
    mv->card[79] = '\0';
    return(0);
}

/* + line no xxx.x/min  xxx.x%  xx:xx:xx/xx settei
*/
int beauasym_meb()
{
    char *pw;
    int  i;

    for(i = 1; i < 12; i++) if(mv->card[i] <= 0x1f) mv->card[i] = ' ';

    /*
    if(mv->card[36] == ' ') {
        pw = "... ";
        for(i = 34; i < 37; i++,pw++) mv->card[i] = *pw;
    }
    */
    if(mv->card[40] == ' ') {
        pw = "xx";
        for(i = 39; i < 41; i++,pw++) mv->card[i] = *pw;
    }
    if(mv->card[46] == ' ') {
        pw = "xxx.x";
        for(i = 42; i < 47; i++,pw++) mv->card[i] = *pw;
    }
    if(mv->card[54] == ' ') {
        pw = "xxx.x";
        for(i = 50; i < 55; i++,pw++) mv->card[i] = *pw;
    }
    if(mv->card[61] == ' ') {
        pw = "xxx.x%";
        for(i = 57; i < 63; i++,pw++) mv->card[i] = *pw;
    }
    if(mv->card[78] == ' ') {
        pw = "xx:xx:xx/xx.xx";
        for(i = 65; i < 79; i++,pw++) mv->card[i] = *pw;
    }
    mv->card[79] = '\0';
    return(0);
}

int barsync_meb()
{
    char wk[8];
    int  i;

    if(mv->step == 1) {
        if(mb->sync && !(mv->next[0] == '-' || mv->next[0] == '=')
           && mb->mnflg1 == 2) {
            if(mb->bnum != 0) {
                for(i = 0; i < 7; i++) wk[i] = mv->card[i+2];
                wk[7] = '\0';
                if(!mv->pass) {
                    if(!mb->expnl) {
                        mv->wrncnt++;
                        mv->msglvl = 0;
                        if(mv->msglvl >= mv->prtlvl)
                        printf("Worning! %02d%05d Synchronize %ld\n",
                        mb->sequ,mb->bnum,mb->sync);
                    }
                }
            }
        }
    }
    mb->adsyn = 0;
    mb->sync = (400000/mb->time)*mb->ryhm;
    mb->bartyp = mv->card[1];
    for(i = 0; i < 6 ; i++) wk[i] = mv->card[i+4];
    wk[++i] = '\0';
    sscanf(wk,"%d",&mb->bnum);
    return(0);
}

/* beautify symbol put to buffer
*/
int beaumput_meb()
{
    int  i,j,k;
    int  onflg;
    char cw[82];

    onflg = 0;
    if(mv->card[0] == '{') {  /* {} on xxx xxx Check */
        strcpy(cw,mv->card);
	i = 0;
	while(mv->card[i] != '}') {
	    i++;
	    if(i > 74) break;
	}
	j =  i;
	for(i = j; i < 80; i++) {
            if(cw[i] == '\0') break;
	    if((cw[i] == 'o' || cw[i] == 'O') && 
	       (cw[i+1] == 'n' || cw[i+1] == 'N')) {
		onflg = 1;
		break;
	    }
	}
        /* printf("onflg = %d j = %d\n",onflg,j); */
        if(onflg) {
       	    j = i;
	    k = 0;
	    for(i = j; i < 80; i++) {
		if(cw[i] == ',') cw[i] = ' ';
	    }
	    for(i = j; i < 80; i++) {
		if((cw[i] == ' ') &&
		   (cw[i+1] != ' ') &&
		   (cw[i+2] != ' ') && 
		   (cw[i+3] != ' ')) {
		       mm->onpart[mv->mcpos][k][0] = cw[i+1];
		       mm->onpart[mv->mcpos][k][1] = cw[i+2];
		       mm->onpart[mv->mcpos][k][2] = cw[i+3];
		       mm->onpart[mv->mcpos][k][3] = '\0';
		       /*
		       printf("mm->onpart[%d][%d]=%s\n",
			   mv->mcpos,k,mm->onpart[mv->mcpos][k]);
		       */
		       k++;
                }
	    }
            mm->onpc[mv->mcpos] = k;
        }
    }
    mm->mmov[mv->mcpos] = mb->sequ;
    mm->mbrc[mv->mcpos] = mb->bnum;
    mm->msnc[mv->mcpos] = mb->adsyn;
    for(i = 0; i < MMSN; i++) {
        if(mv->card[i] <= 0x1f) break;
        mm->msmb[mv->mcpos][i] = mv->card[i];
        if(mv->card[0] == '{') {
            if(mv->card[i] == '}') {
                i++;
                break;
            }
        }
        else if(mv->card[i] == ' ') break;
    }
    mm->msmb[mv->mcpos][i] = '\0';
    if(mv->debug == 3) {
        printf("Debug3! BMPUTM00 mm->mmov[%d]=%d",
            mv->mcpos,mm->mmov[mv->mcpos]);
        printf(" mm->mbrc[%d]=%ld",mv->mcpos,mm->mbrc[mv->mcpos]);
        printf(" mm->msnc[%d]=%ld\n",mv->mcpos,mm->msnc[mv->mcpos]);
        printf("       mm->msmb[%d]=%s\n",
            mv->mcpos,mm->msmb[mv->mcpos]);
    }
    mv->mcpos++;
    if(mv->mcpos > MMST) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! BMPUTM01 beautify symbol table over check MMST");
        printf(" mcpos = %d limit = %d\n",mv->mcpos,MMST);
        exit(-1);
    }
    return(0);
}

/* movement generate if(mv->card[0] == '=')
*/
int beaumov_meb()
{
    mb->sync = 0;
    mb->adsyn = 0;
    mb->bartyp = mv->card[1];
    return(0);
}

/* beautify expresson symbol copy from master file
   {generate beautify}
*/
int bexpcopy_meb()
{

    while(mb->sequ > mm->mmov[mv->mmpos]) {
        if(mv->debug == 3) printf(
        "Debug3! bexpcopy(1) mb->sequ=%d mm->mmov[%d]=%d sarch next sequence\n",
            mb->sequ,mv->mmpos,mm->mmov[mv->mmpos]);
        mv->mmpos++;
        if(mv->mmpos >= mv->mcpos) break;
        return(0);
    }
    /* test */
    if(mb->sequ != mm->mmov[mv->mmpos]) {
        if(mv->debug == 3) printf(
        "Debug3! bexpcopy(2) mb->sequ=%d mm->mmov[%d]=%d rc=5\n",
        mb->sequ,mv->mmpos,mm->mmov[mv->mmpos]);
        return(5);
    }
    if(mb->bnum < mm->mbrc[mv->mmpos]) {
        if(mv->debug == 3) {
            printf("Debug3! bexpcopy(3) mb->bnum=%d",mb->bnum);
            printf(" mm->mbrc[%d]=%ld rc=4\n",
                mv->mmpos,mm->mbrc[mv->mmpos]);
        }
        return(4);
    }
    if(mb->adsyn < mm->msnc[mv->mmpos]) {
        if(mv->debug == 3) {
        printf("Debug3! bexpcopy(4) mb->adsyn=%ld mm->msnc[%d]=%ld rc=1 or 2 ",
                mb->adsyn,mv->mmpos,mm->msnc[mv->mmpos]);
            printf("mb->mnflg1=%d mb->sequ=%d\n",mb->mnflg1,mb->sequ);
        }
        if(mb->mnflg1 == 2) return(3);
        if(mb->sequ == 0) return(2);
        return(1);
    }
    if(mv->debug == 3) {
        printf("Debug3! bexpcopy(5) mb->adsyn=%ld mm->msnc[%d]=%ld rc=0\n",
        mb->adsyn,mv->mmpos,mm->msnc[mv->mmpos]);
    }
    return(0);
}

/* [xxxx] expression
   {xxxx} tempo
*/
int mkexptmb_meb(char *inf,struct mivsym ms[])
//char *inf;
//struct mivsym ms[];
{
    char card[82];
    char work[8];
    int  i;
    int  ls;
    char scrf[100];
    char rfile[STLN];

    if(mv->symmax != 0) return(0); /* add 1999.7 */
    i = 0;
    while(1) {
        if(inf[i] == '\0') break;
        if(inf[i] == '_') break;
        rfile[i] = inf[i];
        i++;
        if(i > STLN) break;
    }
    rfile[i] = '\0';
    strcat(rfile,"rds.dat");
    if(mv->debug) printf("mexptmb_() inf=%s rfile=%s\n",inf,rfile);
    if((mv->fpe = fopen(rfile,"r")) == NULL) { /* add 2011.7.1 */
        if((mv->fpe = fopen("mebrds.dat","r")) == NULL) {
            sprintf(scrf,"%s",TACTDIR);
            //sprintf(scrf,"%s\0",TACTDIR);
            strcat(scrf,"mebrds.dat");
            if(mv->debug == 4) printf("Debug4! scrf = %s\n",scrf);
            if((mv->fpe = fopen(scrf,"r")) == NULL) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! mkexptmb_2() Can't open %s mode r\n",scrf);
                }
                return(-1);
            }
        }
    }
    ls = 0;
    while(fgets(card,80,mv->fpe)) {
        /*
        if(card[0] == ESC) break;
        if(card[0] == 'f') break;
        */
        if(card[0] == '{') {
            for(i = 0; i < 21; i++) ms[ls].symsgn[i] = card[i];
            ms[ls].symsgn[21] = '\0';
            if(!isdigit(card[29])) ms[ls].symtyp = 0;
            else {
                for(i = 0; i < 6; i++) work[i] = card[i+24];
                work[6] = '\0';
                sscanf(work,"%d",&ms[ls].symtyp);
            }
            if(!isdigit(card[39])) ms[ls].symv1 = 0;
            else {
                for(i = 0; i < 6 ; i++) work[i] = card[i+34];
                work[6] = '\0';
                sscanf(work,"%f",&ms[ls].symv1);
            }
            if(!isdigit(card[49])) ms[ls].symv2 = 0;
            else {
                for(i = 0; i < 6 ; i++) work[i] = card[i+44];
                work[6] = '\0';
                sscanf(work,"%f",&ms[ls].symv2);
            }
            if(!isdigit(card[59])) ms[ls].symv3 = 0;
            else {
                for(i = 0; i < 6 ; i++) work[i] = card[i+54];
                work[6] = '\0';
                sscanf(work,"%f",&ms[ls].symv3);
            }
            ms[ls].symv4[0] = card[66];
            ms[ls].symv4[1] = card[67];
            ms[ls].symv4[2] = card[68];
            ms[ls].symv4[3] = card[69];
            if(mv->debug == 4) {
                printf(" %s %3d %7.1f %7.1f %7.1f",
                ms[ls].symsgn,ms[ls].symtyp,ms[ls].symv1,
                ms[ls].symv2,ms[ls].symv3);
                printf("       %c%c%c%c\n",
                ms[ls].symv4[0],ms[ls].symv4[1],
                ms[ls].symv4[2],ms[ls].symv4[3]);
            }
            ls++;
            if(ls >= MEXP) {
                if(!mv->pass) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                    printf("Error! mebrds.dat line over\n");
                }
                break;
            }
        }
    }
    mv->symmax = ls;
    if(mv->debug == 4) printf("Debug4 mv->symmax=%d\n",mv->symmax);
    fclose(mv->fpe);
    return(0);
}

int keyinimb_meb()
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

int nkeyini_meb()
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

int keysetmb_meb()
{
    int  i,j,m,md;

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
    return(0);
}

int numdinmb_meb(char *card)
//char *card;
{
    int  i,k;
    int  n;
    char *pc;
    char cnum[10];

    k = 0;
    pc = card;
    while(*pc) pc++;
    pc--;
    while(*pc == ' ') pc--;
    for(i = 0; i < 10; i++) cnum[i] = ' ';
    cnum[9] = '\0';
    for(k = 8; k >= 0; k--) {
        if(*pc == ' ') break;
        cnum[k] = *pc;
        /* if(!isdigit(*pc)) return(-2);   -xxx vau */
        pc--;
    }
    sscanf(cnum,"%d",&n);
    return(n);
}

int rengetmb_meb()
{
    int  i,j;
    int  renp;
    int  rept;
    char wk[4];

    i = 4;
    renp = 0;
    rept = 0;
    mb->rept = 1;
    while(mv->card[i]) {
        if(mv->card[i] == '[') renp = 1;
        if(mv->card[i] == ';') rept = 1;
        i++;
        if(i > 9) break;
    }
    if(renp) {
        i = 0;
        while(mv->card[i] != '[') i++;
        j = 0;
        i++;
        while(isdigit(mv->card[i])) wk[j++] = mv->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mb->renp);
    }
    if(rept) {
        i = 0;
        while(mv->card[i] != ';') i++;
        j = 0;
        i++;
        while(isdigit(mv->card[i])) wk[j++] = mv->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mb->rept);
    }
    return(0);
}


/* make transposed key
*/
int mktrkey_meb(int trv)
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
    if(mv->debug == 2) {
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
        for(i = 0; i < mb->tpkval; i++) mb->trkey[i] = '#';
    }
    else for(i = 0; i < mb->tpkval - 7 ; i++) mb->trkey[i] = '&';
    mb->trkey[i] = '\0';
    if(mv->debug == 2) {
        printf("mb->ksin=%d mb->ogkval=%d trv=%d\n",
            mb->ksin,mb->ogkval,trv);
        printf("Debug mb->tpkval=%d mb->trkey=%s\n",
            mb->tpkval,mb->trkey);
    }
    return(0);
}

/* 
 transposition
*/
int beautrns_meb()
{
    int  osp;    /* (original) shift start point */
    int  oep;    /* (original) shift end point */
    int  tsp;    /* (terget) shift start point */
    int  tep;    /* (terget) shift end point */
    int  sf;

    sf = 0;
    osp = 11;
    oep = 69;
    if( mb->bartyp == 's' ||
        mb->bartyp == 't' ||
        mb->bartyp == 'g' ||
        mb->bartyp == 'a' ||
        mb->bartyp == 'u' ||
        mb->bartyp == 'b') {
        osp = 32;
        sf = 1;
    }
    if( mb->bartyp == 'l' ||
        mb->bartyp == 'm' ||
        mb->bartyp == 'h') {
        osp = 11;
        sf = 1;
    }
    tsp = osp;
    tep = oep;
    /* note_shift */
    /*
    if(mb->nsftf) {
        lnshift_meb(osp,oep,tsp,tep,mb->nsftv);
        return(0);
    }
    */
    if(!mb->GKTPv) sf = 0;
    if(sf && mv->step == 1) {
        tnshift_meb(osp); /* Transposition note shift */
    }
    return(0);
}

/* Transposition note shift */
int tnshift_meb(int osp)
//int  osp;
{
    int  i,k;
    int  tpv;
    int  sfv;
    char wk[82];
    int  bf;

    if(mv->debug == 3) printf("beautrns_() mb->GKTPv = %d\n",mb->GKTPv);
    tpv = abs(mb->GKTPv);
    tpv = tpv % 12;
    sfv = 0;
    switch(tpv) {
        case   0 : sfv = 0;
                   break;
        case   1 : sfv = 0;
                   break;
        case   2 : sfv = 1;
                   break;
        case   3 : sfv = 2;
                   break;
        case   4 : sfv = 2;
                   break;
        case   5 : sfv = 3;
                   break;
        case   6 : sfv = 3;
                   break;
        case   7 : sfv = 4;
                   break;
        case   8 : sfv = 5;
                   break;
        case   9 : sfv = 5;
                   break;
        case  10 : sfv = 6;
                   break;
        case  11 : sfv = 7;
                   break;
        default  : break;
    }
    bf = 1;
    if(mb->GKTPv < 0) {
        bf = 0;
        sfv = -sfv;
        tpv = -tpv;
    }
    if(mv->debug == 3) {
    printf("Debug3! mb->GKTPv=%d mb->knsftv=%d sfv=%d osp=%d,tpv=%d,bf=%d\n",
        mb->GKTPv,mb->knsftv,sfv,osp,tpv,bf);
    }
    strcpy(wk,mv->card);
    if(tpv < 0) {
        for(i = osp; i <= 68; i++) mv->card[i] = ' '; 
        /* for(i = osp; i < 68 + sfv; i++) transp_meb(wk,i+1,tpv,sfv,bf); */
        for(i = osp; i < 68; i++) transp_meb(wk,i+1,tpv,sfv,bf); 
    }
    if(tpv >= 0) {
        for(i = osp; i <= 68; i++) mv->card[i] = ' '; 
        /* for(i = 68; i > osp - sfv; i--) transp_meb(wk,i-1,tpv,sfv,bf);  */
        for(i = 68; i > osp; i--) transp_meb(wk,i-1,tpv,sfv,bf); 
    }
    tncheck_men(osp,tpv);
    return(0);
}

int transp_meb(char wk[82],int i,int tpv,int sfv,int bf)
//char wk[82];
//int  i;
//int  tpv;
//int  sfv;
//int  bf;  /* back or forward flag */
{
    int  k1,k2,k3,k4;
    int  civ,kv;
    int  biv,bv;
    int  tiv;
    char ch;
    char c;

    if(wk[i] != ' ') {
        k1 = getkeyp_meb(i,1);       /* get original natural keyp */
        k2 = getkeyp_meb(i,2);       /* get before keyp */
        k3 = getkeyp_meb(i+sfv,1);   /* get current natural keyp */
        k4 = getkeyp_meb(i+sfv,3);   /* get current shift keyp */
        biv = k2 - k1; /* before interval */
        civ = k4 - k3; /* current interval */
        tiv = k4 - k2; /* transpose interval */
        kv = civ - biv;
        if(mv->debug == 3) {
            printf("\nDebug3! transp_() %c i=%d tiv=%d tpv=%d sfv=%d\n",
                wk[i],i,tiv,tpv,sfv);
            printf("        biv(%d) = k2(%d) - k1(%d)\n",biv,k2,k1);
            printf("        civ(%d) = k4(%d) - k3(%d)\n",civ,k4,k3);
            printf("        tiv(%d) = k4(%d) - k2(%d)\n",tiv,k4,k2);
            printf("        kv(%d) = civ(%d) - biv(%d)\n",kv,civ,biv);
        }
        if(tiv == tpv) {
            ch = getkch_meb(kv,wk[i]);
            if(tpv == 0) {
                sfv = 0;
                ch = wk[i];
            }
            mv->card[i+sfv] = ch; 
        }
        else {
            ch = getpvch_meb(tiv,tpv,wk[i]);
            if(tpv == 0) {
                sfv = 0;
                ch = wk[i];
            }
            if(ch == ')' || ch == '(') {
                if(ch == ')') {
                    c = sgetkch_meb(wk[i],i,sfv+1);
                    mv->card[i+sfv+1] = c;
                    wk[i+sfv+1] = ' ';
                }
                if(ch == '(') {
                    c = sgetkch_meb(wk[i],i,sfv-1);
                    mv->card[i+sfv-1] = c;
                    /* mv->card[i+sfv-2] = wk[i-1]; */
                    wk[i+sfv-1] = ' ';
                }
            }
            else mv->card[i+sfv] = ch; 
        }
    }
    return(0);
}

char getkch_meb(int kv,char c1)
//int  kv;
//char c1;
{
    char ch;

    ch = c1;
    switch(kv) {
        case  0 : /*  */
                  if(c1 == '$') ch = '$';
                  break;
        case -1 : /* <- */
                  if(c1 == '$') ch = '&';
                  if(c1 == '#') ch = '$';
                  if(c1 == '&') ch = '@';
                  if(c1 == '*') ch = '#';
                  if(c1 == '@') ch = '?';
                  break;
        case  1 : /* -> */
                  if(c1 == '$') ch = '#';
                  if(c1 == '#') ch = '*';
                  if(c1 == '&') ch = '$';
                  if(c1 == '*') ch = '?';
                  if(c1 == '@') ch = '&';
                  break;
        case -2 : /* <-- */
                  if(c1 == '$') ch = '@';
                  if(c1 == '#') ch = '&';
                  if(c1 == '&') ch = '?';
                  if(c1 == '*') ch = '$';
                  if(c1 == '@') ch = '?';
                  break;
        case  2 : /* --> */
                  if(c1 == '$') ch = '*';
                  if(c1 == '#') ch = '?';
                  if(c1 == '&') ch = '#';
                  if(c1 == '*') ch = '?';
                  if(c1 == '@') ch = '$';
                  break;
        default : break;
    }
    if(mv->debug == 3) printf("getkch_() kv = %d c1 = %c -> ch = %c\n",
        kv,c1,ch);
    return(ch);
}

char sgetkch_meb(char c1,int i,int ssv)
//char c1;
//int  i;
//int  ssv;
{    
    int  k1,k2,k3,k4;
    int  civ,kv;
    int  biv,bv;
    int  tiv;       
    char ch;      

    k1 = getkeyp_meb(i,1);       /* get original natural keyp */
    k2 = getkeyp_meb(i,2);       /* get before keyp */
    k3 = getkeyp_meb(i+ssv,1);   /* get current natural keyp */
    k4 = getkeyp_meb(i+ssv,3);   /* get current shift keyp */
    biv = k2 - k1; /* before interval */ 
    civ = k4 - k3; /* current interval */
    kv = civ - biv;
    if(mv->debug == 3) {
        printf("sgetch_() %c kv=%d\n",c1,kv);
        printf("        biv(%d) = k2(%d) - k1(%d)\n",biv,k2,k1);
        printf("        civ(%d) = k4(%d) - k3(%d)\n",civ,k4,k3);
        printf("        kv(%d) = civ(%d) - biv(%d)\n",kv,civ,biv);
    }
    ch = c1;
    switch(kv) {
        case  0 : /*  */
                  if(c1 == '$') ch = '$';
                  break;
        case -1 : /* <- */
                  if(c1 == '$') ch = '&';
                  if(c1 == '#') ch = '$';
                  if(c1 == '&') ch = '@';
                  if(c1 == '*') ch = '#';
                  if(c1 == '@') ch = '?';
                  break;
        case  1 : /* -> */
                  if(c1 == '$') ch = '#';
                  if(c1 == '#') ch = '*';
                  if(c1 == '&') ch = '$';
                  if(c1 == '*') ch = '?';
                  if(c1 == '@') ch = '&';
                  break;
        case -2 : /* <-- */
                  if(c1 == '$') ch = '@';
                  if(c1 == '#') ch = '&';
                  if(c1 == '&') ch = '?';
                  if(c1 == '*') ch = '$';
                  if(c1 == '@') ch = '?';
                  break;
        case  2 : /* --> */
                  if(c1 == '$') ch = '*';
                  if(c1 == '#') ch = '?';
                  if(c1 == '&') ch = '#';
                  if(c1 == '*') ch = '?';
                  if(c1 == '@') ch = '$';
                  break;
        default : break;
    }
    if(mv->debug == 3) printf("sgetkch_() kv = %d c1 = %c -> ch = %c\n",
        kv,c1,ch);
    return(ch);
}   

/*
  Transpose value
  0123456789#&$*@: 
  JKLNPQUVYZ]}/'A; :  >
  jklnpquvyz[{%"a, :  < 
*/
char getpvch_meb(int tiv,int tpv,char c1)
//int  tiv;
//int  tpv;
//char c1;
{
    char ch;
    int  iv;

    iv = tpv - tiv;
    if(mv->debug == 3) printf("getpvch_() iv=%d tpv=%d tiv=%d\n",iv,tpv,tiv);
    ch = c1;
    switch(iv) {
        case  1 : /* -> */
                  if(isdigit(c1)) {
                      /* ch = '>'; */
                      if(c1 == '0') ch = 'A';
                      if(c1 == '1') ch = 'B';
                      if(c1 == '2') ch = 'C';
                      if(c1 == '3') ch = 'D';
                      if(c1 == '4') ch = 'E';
                      if(c1 == '5') ch = 'F';
                      if(c1 == '6') ch = 'G';
                      if(c1 == '7') ch = 'H';
                      if(c1 == '8') ch = 'I';
                      if(c1 == '9') ch = 'J';
                  }
                  if(c1 == '#') ch = ']';
                  if(c1 == '&') ch = '}';
                  if(c1 == '$') ch = '/';
                  if(c1 == '*') ch = '\'';
                  if(c1 == '@') ch = '!';
                  if(c1 == ':') ch = ';';
                  break;
        case -1 : /* <- */
                  if(isdigit(c1)) {
                      ch = '<';
                      if(c1 == '0') ch = 'a';
                      if(c1 == '1') ch = 'b';
                      if(c1 == '2') ch = 'c';
                      if(c1 == '3') ch = 'd';
                      if(c1 == '4') ch = 'e';
                      if(c1 == '5') ch = 'f';
                      if(c1 == '6') ch = 'g';
                      if(c1 == '7') ch = 'h';
                      if(c1 == '8') ch = 'i';
                      if(c1 == '9') ch = 'j';
                  }
                  if(c1 == '#') ch = '[';
                  if(c1 == '&') ch = '{';
                  if(c1 == '$') ch = '%';
                  if(c1 == '*') ch = '\"';
                  if(c1 == '@') ch = '~';
                  if(c1 == ':') ch = ',';
                  break;
        case  2 : /* --> */
                  if(isdigit(c1)) ch = ')';
                  if(c1 == ':') ch = ')';
                  if(c1 == '$') ch = ')';
                  if(c1 == '#') ch = ')';
                  if(c1 == '&') ch = ')';
                  if(c1 == '*') ch = ')';
                  if(c1 == '@') ch = ')';
                  break;
        case -2 : /* <-- */
                  if(isdigit(c1)) ch = '(';
                  if(c1 == ':') ch = '(';
                  if(c1 == '$') ch = '(';
                  if(c1 == '#') ch = '(';
                  if(c1 == '&') ch = '(';
                  if(c1 == '*') ch = '(';
                  if(c1 == '@') ch = '(';
                  break;
        default : break;
    }
    if(mv->debug == 3) printf("        tpv=%d tiv=%d c1= %c -> ch = %c\n",
        tpv,tiv,c1,ch);
    return(ch);
}

/* {#Key_Transposition xx} */
int keytrnsp_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    int  i,j;
    int  k;
    int  ky;
    char wscl[12];
    char card[82];
    char wkc[80];

    if(mv->card[2] == ':' || mv->card[3] == ':' ||
        mv->card[4] == ':') return(1);
    /* printf("%s mb->GKTPv = %d\n",mv->card,mb->GKTPv); */
    k = mb->GKTPv;
    if(k > 28 || k < -28) {
        printf("Error! Key_Transposition Value %d\n",k);
        return(1);
    }
    mktrkey_meb(-mb->GKTPv);
    sprintf(card,"{$%s%d/%d ",mb->trkey,mb->ryhm,mb->time);
    ky = 0;
    switch(mv->mkscl) {
        case  1 : strcpy(wscl,"_dur");
                  cp->key = 1;
                  ky = 0;
                  break;
        case  2 : strcpy(wscl,"_moll");
                  cp->key = 2;
                  ky = 1;
                  break;
        case  3 : strcpy(wscl,"_Major");
                  cp->key = 1;
                  ky = 0;
                  break;
        case  4 : strcpy(wscl,"_minor");
                  cp->key = 2;
                  ky = 1;
                  break;
        default : strcpy(wscl,"_Major ?"); 
                  cp->key = 0;
                  ky = 0;  
                  break;
    }
    i = 2;  
    mb->ksin = 0;
    if(card[2] == '#') {
        while(card[i] == '#') {
            i++; 
            mb->ksin++;
        }       
    }           
    if(card[2] == '&') {
        mb->ksin = 7;
        while(card[i] == '&') {
            i++;
            mb->ksin++;
        }
    }
    if(mb->ksin < 0 || mb->ksin > 14) {
        mb->ksin = 0;
    }
    strcpy(wkc,mk->keysin[ky][mb->ksin]);
    strcat(wkc,wscl); 
    /* modkey_meb(wkc); */
    strcat(card,wkc);
    sprintf(wkc," Transposition %d}",mb->GKTPv);
    strcat(card,wkc);
    strcpy(mv->card,card);
    /* printf("keytrnsp_() %s\n",mv->card); */
    key_meb();
    setkey_cps();
    return(0);
}

int init_gate_rate()
{
    ge->def_bexp =  82;
    ge->asn_bexp =  82;
    ge->mod_bexp =  82;
    ge->set_bexp =  82;

    gr->ww    = 99;     /* |O| */
    gr->w     = 96;     /*  O  */
    gr->wp    = 97;     /*  O. */
    gr->wpp   = 98;     /*  O.. */
    gr->h     = 94;     /*  o/ */
    gr->hp    = 95;     /*  o/. */
    gr->hpp   = 96;     /*  o/.. */
    gr->q     = 90;     /*  _/ */
    gr->qp    = 91;     /*  _/. */
    gr->qpp   = 92;     /*  _/.. */
    gr->n8    = 82;     /*  _/) */
    gr->n8p   = 84;     /*  _/). */
    gr->n8pp  = 86;     /*  _/).. */
    gr->n16   = 66;     /*  _/)) */
    gr->n16p  = 68;     /*  _/)). */
    gr->n16pp = 70;     /*  _/)).. */
    gr->n32   = 60;     /*  _/))) */
    gr->n32p  = 62;     /*  _/))). */
    gr->n32pp = 64;     /*  _/))).. */
    gr->n64   = 54;     /*  _/)))) */
    gr->n64p  = 56;     /*  _/)))). */
    gr->n64pp = 58;     /*  _/)))).. */
    gr->t8    = 72;     /*  ./) */
    gr->t8p   = 74;     /*  ./). */
    gr->t8pp  = 76;     /*  ./).. */
    gr->t16   = 66;     /*  ./)) */
    gr->t16p  = 68;     /*  ./)). */
    gr->t16pp = 70;     /*  ./)).. */
    gr->t32   = 60;     /*  ./))) */
    gr->t32p  = 62;     /*  ./))). */
    gr->t32pp = 64;     /*  ./))).. */
    gr->t64   = 54;     /*  ./)))) */
    gr->t64p  = 56;     /*  ./)))). */
    gr->t64pp = 58;     /*  ./)))).. */
    gr->a8    = 72;     /*  ./> */
    gr->a8p   = 74;     /*  ./>. */
    gr->a8pp  = 76;     /*  ./>.. */
    gr->a16   = 66;     /*  ./>> */
    gr->a16p  = 68;     /*  ./>>. */
    gr->a16pp = 70;     /*  ./>>.. */
    gr->a32   = 60;     /*  ./>>> */
    gr->a32p  = 62;     /*  ./>>>. */
    gr->a32pp = 64;     /*  ./>>>.. */
    gr->a64   = 54;     /*  ./>>>> */
    gr->a64p  = 56;     /*  ./>>>>. */
    gr->a64pp = 58;     /*  ./>>>>.. */
    /* set modify Gate_rate on/off flag */
    gr->fww    = 0;     /* |O| */
    gr->fw     = 0;     /*  O  */
    gr->fwp    = 0;     /*  O. */
    gr->fwpp   = 0;     /*  O.. */
    gr->fh     = 0;     /*  o/ */
    gr->fhp    = 0;     /*  o/. */
    gr->fhpp   = 0;     /*  o/.. */
    gr->fq     = 0;     /*  _/ */
    gr->fqp    = 0;     /*  _/. */
    gr->fqpp   = 0;     /*  _/.. */
    gr->fn8    = 0;     /*  _/) */
    gr->fn8p   = 0;     /*  _/). */
    gr->fn8pp  = 0;     /*  _/).. */
    gr->fn16   = 0;     /*  _/)) */
    gr->fn16p  = 0;     /*  _/)). */
    gr->fn16pp = 0;     /*  _/)).. */
    gr->fn32   = 0;     /*  _/))) */
    gr->fn32p  = 0;     /*  _/))). */
    gr->fn32pp = 0;     /*  _/))).. */
    gr->fn64   = 0;     /*  _/)))) */
    gr->fn64p  = 0;     /*  _/)))). */
    gr->fn64pp = 0;     /*  _/)))).. */
    gr->ft8    = 0;     /*  ./) */
    gr->ft8p   = 0;     /*  ./). */
    gr->ft8pp  = 0;     /*  ./).. */
    gr->ft16   = 0;     /*  ./)) */
    gr->ft16p  = 0;     /*  ./)). */
    gr->ft16pp = 0;     /*  ./)).. */
    gr->ft32   = 0;     /*  ./))) */
    gr->ft32p  = 0;     /*  ./))). */
    gr->ft32pp = 0;     /*  ./))).. */
    gr->ft64   = 0;     /*  ./)))) */
    gr->ft64p  = 0;     /*  ./)))). */
    gr->ft64pp = 0;     /*  ./)))).. */
    gr->fa8    = 0;     /*  ./> */
    gr->fa8p   = 0;     /*  ./>. */
    gr->fa8pp  = 0;     /*  ./>.. */
    gr->fa16   = 0;     /*  ./>> */
    gr->fa16p  = 0;     /*  ./>>. */
    gr->fa16pp = 0;     /*  ./>>.. */
    gr->fa32   = 0;     /*  ./>>> */
    gr->fa32p  = 0;     /*  ./>>>. */
    gr->fa32pp = 0;     /*  ./>>>.. */
    gr->fa64   = 0;     /*  ./>>>> */
    gr->fa64p  = 0;     /*  ./>>>>. */
    gr->fa64pp = 0;     /*  ./>>>>.. */

    gd->ww    = 99;     /* |O| */
    gd->w     = 96;     /*  O  */
    gd->wp    = 97;     /*  O. */
    gd->wpp   = 98;     /*  O.. */
    gd->h     = 93;     /*  o/ */
    gd->hp    = 94;     /*  o/. */
    gd->hpp   = 95;     /*  o/.. */
    gd->q     = 90;     /*  _/ */
    gd->qp    = 91;     /*  _/. */
    gd->qpp   = 92;     /*  _/.. */
    gd->n8    = 82;     /*  _/) */
    gd->n8p   = 84;     /*  _/). */
    gd->n8pp  = 86;     /*  _/).. */
    gd->n16   = 66;     /*  _/)) */
    gd->n16p  = 68;     /*  _/)). */
    gd->n16pp = 70;     /*  _/)).. */
    gd->n32   = 60;     /*  _/))) */
    gd->n32p  = 62;     /*  _/))). */
    gd->n32pp = 64;     /*  _/))).. */
    gd->n64   = 54;     /*  _/)))) */
    gd->n64p  = 56;     /*  _/)))). */
    gd->n64pp = 58;     /*  _/)))).. */
    gd->t8    = 72;     /*  ./) */
    gd->t8p   = 74;     /*  ./). */
    gd->t8pp  = 76;     /*  ./).. */
    gd->t16   = 66;     /*  ./)) */
    gd->t16p  = 68;     /*  ./)). */
    gd->t16pp = 70;     /*  ./)).. */
    gd->t32   = 60;     /*  ./))) */
    gd->t32p  = 62;     /*  ./))). */
    gd->t32pp = 64;     /*  ./))).. */
    gd->t64   = 54;     /*  ./)))) */
    gd->t64p  = 56;     /*  ./)))). */
    gd->t64pp = 58;     /*  ./)))).. */
    gd->a8    = 72;     /*  ./> */
    gd->a8p   = 74;     /*  ./>. */
    gd->a8pp  = 76;     /*  ./>.. */
    gd->a16   = 66;     /*  ./>> */
    gd->a16p  = 68;     /*  ./>>. */
    gd->a16pp = 70;     /*  ./>>.. */
    gd->a32   = 60;     /*  ./>>> */
    gd->a32p  = 62;     /*  ./>>>. */
    gd->a32pp = 64;     /*  ./>>>.. */
    gd->a64   = 54;     /*  ./>>>> */
    gd->a64p  = 56;     /*  ./>>>>. */
    gd->a64pp = 58;     /*  ./>>>>.. */
    /* set modify Gate_rate on/off flag */
    gd->fww    = 0;     /* |O| */
    gd->fw     = 0;     /*  O  */
    gd->fwp    = 0;     /*  O. */
    gd->fwpp   = 0;     /*  O.. */
    gd->fh     = 0;     /*  o/ */
    gd->fhp    = 0;     /*  o/. */
    gd->fhpp   = 0;     /*  o/.. */
    gd->fq     = 0;     /*  _/ */
    gd->fqp    = 0;     /*  _/. */
    gd->fqpp   = 0;     /*  _/.. */
    gd->fn8    = 0;     /*  _/) */
    gd->fn8p   = 0;     /*  _/). */
    gd->fn8pp  = 0;     /*  _/).. */
    gd->fn16   = 0;     /*  _/)) */
    gd->fn16p  = 0;     /*  _/)). */
    gd->fn16pp = 0;     /*  _/)).. */
    gd->fn32   = 0;     /*  _/))) */
    gd->fn32p  = 0;     /*  _/))). */
    gd->fn32pp = 0;     /*  _/))).. */
    gd->fn64   = 0;     /*  _/)))) */
    gd->fn64p  = 0;     /*  _/)))). */
    gd->fn64pp = 0;     /*  _/)))).. */
    gd->ft8    = 0;     /*  ./) */
    gd->ft8p   = 0;     /*  ./). */
    gd->ft8pp  = 0;     /*  ./).. */
    gd->ft16   = 0;     /*  ./)) */
    gd->ft16p  = 0;     /*  ./)). */
    gd->ft16pp = 0;     /*  ./)).. */
    gd->ft32   = 0;     /*  ./))) */
    gd->ft32p  = 0;     /*  ./))). */
    gd->ft32pp = 0;     /*  ./))).. */
    gd->ft64   = 0;     /*  ./)))) */
    gd->ft64p  = 0;     /*  ./)))). */
    gd->ft64pp = 0;     /*  ./)))).. */
    gd->fa8    = 0;     /*  ./> */
    gd->fa8p   = 0;     /*  ./>. */
    gd->fa8pp  = 0;     /*  ./>.. */
    gd->fa16   = 0;     /*  ./>> */
    gd->fa16p  = 0;     /*  ./>>. */
    gd->fa16pp = 0;     /*  ./>>.. */
    gd->fa32   = 0;     /*  ./>>> */
    gd->fa32p  = 0;     /*  ./>>>. */
    gd->fa32pp = 0;     /*  ./>>>.. */
    gd->fa64   = 0;     /*  ./>>>> */
    gd->fa64p  = 0;     /*  ./>>>>. */
    gd->fa64pp = 0;     /*  ./>>>>.. */

    ga->ww    = 99;     /* |O| */
    ga->w     = 96;     /*  O  */
    ga->wp    = 97;     /*  O. */
    ga->wpp   = 98;     /*  O.. */
    ga->h     = 93;     /*  o/ */
    ga->hp    = 94;     /*  o/. */
    ga->hpp   = 95;     /*  o/.. */
    ga->q     = 80;     /*  _/ */
    ga->qp    = 81;     /*  _/. */
    ga->qpp   = 82;     /*  _/.. */
    ga->n8    = 72;     /*  _/) */
    ga->n8p   = 74;     /*  _/). */
    ga->n8pp  = 76;     /*  _/).. */
    ga->n16   = 66;     /*  _/)) */
    ga->n16p  = 68;     /*  _/)). */
    ga->n16pp = 70;     /*  _/)).. */
    ga->n32   = 60;     /*  _/))) */
    ga->n32p  = 62;     /*  _/))). */
    ga->n32pp = 64;     /*  _/))).. */
    ga->n64   = 54;     /*  _/)))) */
    ga->n64p  = 56;     /*  _/)))). */
    ga->n64pp = 58;     /*  _/)))).. */
    ga->t8    = 72;     /*  ./) */
    ga->t8p   = 74;     /*  ./). */
    ga->t8pp  = 76;     /*  ./).. */
    ga->t16   = 66;     /*  ./)) */
    ga->t16p  = 68;     /*  ./)). */
    ga->t16pp = 70;     /*  ./)).. */
    ga->t32   = 60;     /*  ./))) */
    ga->t32p  = 62;     /*  ./))). */
    ga->t32pp = 64;     /*  ./))).. */
    ga->t64   = 54;     /*  ./)))) */
    ga->t64p  = 56;     /*  ./)))). */
    ga->t64pp = 58;     /*  ./)))).. */
    ga->a8    = 72;     /*  ./> */
    ga->a8p   = 74;     /*  ./>. */
    ga->a8pp  = 76;     /*  ./>.. */
    ga->a16   = 66;     /*  ./>> */
    ga->a16p  = 68;     /*  ./>>. */
    ga->a16pp = 70;     /*  ./>>.. */
    ga->a32   = 60;     /*  ./>>> */
    ga->a32p  = 62;     /*  ./>>>. */
    ga->a32pp = 64;     /*  ./>>>.. */
    ga->a64   = 54;     /*  ./>>>> */
    ga->a64p  = 56;     /*  ./>>>>. */
    ga->a64pp = 58;     /*  ./>>>>.. */
    /* set modify Gate_rate on/off flag */
    ga->fww    = 0;     /* |O| */
    ga->fw     = 0;     /*  O  */
    ga->fwp    = 0;     /*  O. */
    ga->fwpp   = 0;     /*  O.. */
    ga->fh     = 0;     /*  o/ */
    ga->fhp    = 0;     /*  o/. */
    ga->fhpp   = 0;     /*  o/.. */
    ga->fq     = 0;     /*  _/ */
    ga->fqp    = 0;     /*  _/. */
    ga->fqpp   = 0;     /*  _/.. */
    ga->fn8    = 0;     /*  _/) */
    ga->fn8p   = 0;     /*  _/). */
    ga->fn8pp  = 0;     /*  _/).. */
    ga->fn16   = 0;     /*  _/)) */
    ga->fn16p  = 0;     /*  _/)). */
    ga->fn16pp = 0;     /*  _/)).. */
    ga->fn32   = 0;     /*  _/))) */
    ga->fn32p  = 0;     /*  _/))). */
    ga->fn32pp = 0;     /*  _/))).. */
    ga->fn64   = 0;     /*  _/)))) */
    ga->fn64p  = 0;     /*  _/)))). */
    ga->fn64pp = 0;     /*  _/)))).. */
    ga->ft8    = 0;     /*  ./) */
    ga->ft8p   = 0;     /*  ./). */
    ga->ft8pp  = 0;     /*  ./).. */
    ga->ft16   = 0;     /*  ./)) */
    ga->ft16p  = 0;     /*  ./)). */
    ga->ft16pp = 0;     /*  ./)).. */
    ga->ft32   = 0;     /*  ./))) */
    ga->ft32p  = 0;     /*  ./))). */
    ga->ft32pp = 0;     /*  ./))).. */
    ga->ft64   = 0;     /*  ./)))) */
    ga->ft64p  = 0;     /*  ./)))). */
    ga->ft64pp = 0;     /*  ./)))).. */
    ga->fa8    = 0;     /*  ./> */
    ga->fa8p   = 0;     /*  ./>. */
    ga->fa8pp  = 0;     /*  ./>.. */
    ga->fa16   = 0;     /*  ./>> */
    ga->fa16p  = 0;     /*  ./>>. */
    ga->fa16pp = 0;     /*  ./>>.. */
    ga->fa32   = 0;     /*  ./>>> */
    ga->fa32p  = 0;     /*  ./>>>. */
    ga->fa32pp = 0;     /*  ./>>>.. */
    ga->fa64   = 0;     /*  ./>>>> */
    ga->fa64p  = 0;     /*  ./>>>>. */
    ga->fa64pp = 0;     /*  ./>>>>.. */

    gm->ww    = 99;     /* |O| */
    gm->w     = 96;     /*  O  */
    gm->wp    = 97;     /*  O. */
    gm->wpp   = 98;     /*  O.. */
    gm->h     = 93;     /*  o/ */
    gm->hp    = 94;     /*  o/. */
    gm->hpp   = 95;     /*  o/.. */
    gm->q     = 80;     /*  _/ */
    gm->qp    = 81;     /*  _/. */
    gm->qpp   = 82;     /*  _/.. */
    gm->n8    = 72;     /*  _/) */
    gm->n8p   = 74;     /*  _/). */
    gm->n8pp  = 76;     /*  _/).. */
    gm->n16   = 66;     /*  _/)) */
    gm->n16p  = 68;     /*  _/)). */
    gm->n16pp = 70;     /*  _/)).. */
    gm->n32   = 60;     /*  _/))) */
    gm->n32p  = 62;     /*  _/))). */
    gm->n32pp = 64;     /*  _/))).. */
    gm->n64   = 54;     /*  _/)))) */
    gm->n64p  = 56;     /*  _/)))). */
    gm->n64pp = 78;     /*  _/)))).. */
    gm->t8    = 72;     /*  ./) */
    gm->t8p   = 74;     /*  ./). */
    gm->t8pp  = 76;     /*  ./).. */
    gm->t16   = 66;     /*  ./)) */
    gm->t16p  = 68;     /*  ./)). */
    gm->t16pp = 70;     /*  ./)).. */
    gm->t32   = 60;     /*  ./))) */
    gm->t32p  = 62;     /*  ./))). */
    gm->t32pp = 64;     /*  ./))).. */
    gm->t64   = 54;     /*  ./)))) */
    gm->t64p  = 56;     /*  ./)))). */
    gm->t64pp = 58;     /*  ./)))).. */
    gm->a8    = 72;     /*  ./> */
    gm->a8p   = 74;     /*  ./>. */
    gm->a8pp  = 76;     /*  ./>.. */
    gm->a16   = 66;     /*  ./>> */
    gm->a16p  = 68;     /*  ./>>. */
    gm->a16pp = 70;     /*  ./>>.. */
    gm->a32   = 60;     /*  ./>>> */
    gm->a32p  = 62;     /*  ./>>>. */
    gm->a32pp = 64;     /*  ./>>>.. */
    gm->a64   = 54;     /*  ./>>>> */
    gm->a64p  = 56;     /*  ./>>>>. */
    gm->a64pp = 58;     /*  ./>>>>.. */
    /* set modify Gate_rate on/off flag */
    gm->fww    = 0;     /* |O| */
    gm->fw     = 0;     /*  O  */
    gm->fwp    = 0;     /*  O. */
    gm->fwpp   = 0;     /*  O.. */
    gm->fh     = 0;     /*  o/ */
    gm->fhp    = 0;     /*  o/. */
    gm->fhpp   = 0;     /*  o/.. */
    gm->fq     = 0;     /*  _/ */
    gm->fqp    = 0;     /*  _/. */
    gm->fqpp   = 0;     /*  _/.. */
    gm->fn8    = 0;     /*  _/) */
    gm->fn8p   = 0;     /*  _/). */
    gm->fn8pp  = 0;     /*  _/).. */
    gm->fn16   = 0;     /*  _/)) */
    gm->fn16p  = 0;     /*  _/)). */
    gm->fn16pp = 0;     /*  _/)).. */
    gm->fn32   = 0;     /*  _/))) */
    gm->fn32p  = 0;     /*  _/))). */
    gm->fn32pp = 0;     /*  _/))).. */
    gm->fn64   = 0;     /*  _/)))) */
    gm->fn64p  = 0;     /*  _/)))). */
    gm->fn64pp = 0;     /*  _/)))).. */
    gm->ft8    = 0;     /*  ./) */
    gm->ft8p   = 0;     /*  ./). */
    gm->ft8pp  = 0;     /*  ./).. */
    gm->ft16   = 0;     /*  ./)) */
    gm->ft16p  = 0;     /*  ./)). */
    gm->ft16pp = 0;     /*  ./)).. */
    gm->ft32   = 0;     /*  ./))) */
    gm->ft32p  = 0;     /*  ./))). */
    gm->ft32pp = 0;     /*  ./))).. */
    gm->ft64   = 0;     /*  ./)))) */
    gm->ft64p  = 0;     /*  ./)))). */
    gm->ft64pp = 0;     /*  ./)))).. */
    gm->fa8    = 0;     /*  ./> */
    gm->fa8p   = 0;     /*  ./>. */
    gm->fa8pp  = 0;     /*  ./>.. */
    gm->fa16   = 0;     /*  ./>> */
    gm->fa16p  = 0;     /*  ./>>. */
    gm->fa16pp = 0;     /*  ./>>.. */
    gm->fa32   = 0;     /*  ./>>> */
    gm->fa32p  = 0;     /*  ./>>>. */
    gm->fa32pp = 0;     /*  ./>>>.. */
    gm->fa64   = 0;     /*  ./>>>> */
    gm->fa64p  = 0;     /*  ./>>>>. */
    gm->fa64pp = 0;     /*  ./>>>>.. */

    gs->ww    = 99;     /* |O| */
    gs->w     = 96;     /*  O  */
    gs->wp    = 97;     /*  O. */
    gs->wpp   = 98;     /*  O.. */
    gs->h     = 93;     /*  o/ */
    gs->hp    = 94;     /*  o/. */
    gs->hpp   = 95;     /*  o/.. */
    gs->q     = 80;     /*  _/ */
    gs->qp    = 81;     /*  _/. */
    gs->qpp   = 82;     /*  _/.. */
    gs->n8    = 72;     /*  _/) */
    gs->n8p   = 74;     /*  _/). */
    gs->n8pp  = 76;     /*  _/).. */
    gs->n16   = 66;     /*  _/)) */
    gs->n16p  = 68;     /*  _/)). */
    gs->n16pp = 70;     /*  _/)).. */
    gs->n32   = 60;     /*  _/))) */
    gs->n32p  = 62;     /*  _/))). */
    gs->n32pp = 64;     /*  _/))).. */
    gs->n64   = 54;     /*  _/)))) */
    gs->n64p  = 56;     /*  _/)))). */
    gs->n64pp = 58;     /*  _/)))).. */
    gs->t8    = 72;     /*  ./) */
    gs->t8p   = 74;     /*  ./). */
    gs->t8pp  = 76;     /*  ./).. */
    gs->t16   = 66;     /*  ./)) */
    gs->t16p  = 68;     /*  ./)). */
    gs->t16pp = 70;     /*  ./)).. */
    gs->t32   = 60;     /*  ./))) */
    gs->t32p  = 62;     /*  ./))). */
    gs->t32pp = 64;     /*  ./))).. */
    gs->t64   = 54;     /*  ./)))) */
    gs->t64p  = 56;     /*  ./)))). */
    gs->t64pp = 58;     /*  ./)))).. */
    gs->a8    = 72;     /*  ./> */
    gs->a8p   = 74;     /*  ./>. */
    gs->a8pp  = 76;     /*  ./>.. */
    gs->a16   = 66;     /*  ./>> */
    gs->a16p  = 68;     /*  ./>>. */
    gs->a16pp = 70;     /*  ./>>.. */
    gs->a32   = 60;     /*  ./>>> */
    gs->a32p  = 62;     /*  ./>>>. */
    gs->a32pp = 64;     /*  ./>>>.. */
    gs->a64   = 54;     /*  ./>>>> */
    gs->a64p  = 56;     /*  ./>>>>. */
    gs->a64pp = 58;     /*  ./>>>>.. */
    /* set modify Gate_rate on/off flag */
    gs->fww    = 0;     /* |O| */
    gs->fw     = 0;     /*  O  */
    gs->fwp    = 0;     /*  O. */
    gs->fwpp   = 0;     /*  O.. */
    gs->fh     = 0;     /*  o/ */
    gs->fhp    = 0;     /*  o/. */
    gs->fhpp   = 0;     /*  o/.. */
    gs->fq     = 0;     /*  _/ */
    gs->fqp    = 0;     /*  _/. */
    gs->fqpp   = 0;     /*  _/.. */
    gs->fn8    = 0;     /*  _/) */
    gs->fn8p   = 0;     /*  _/). */
    gs->fn8pp  = 0;     /*  _/).. */
    gs->fn16   = 0;     /*  _/)) */
    gs->fn16p  = 0;     /*  _/)). */
    gs->fn16pp = 0;     /*  _/)).. */
    gs->fn32   = 0;     /*  _/))) */
    gs->fn32p  = 0;     /*  _/))). */
    gs->fn32pp = 0;     /*  _/))).. */
    gs->fn64   = 0;     /*  _/)))) */
    gs->fn64p  = 0;     /*  _/)))). */
    gs->fn64pp = 0;     /*  _/)))).. */
    gs->ft8    = 0;     /*  ./) */
    gs->ft8p   = 0;     /*  ./). */
    gs->ft8pp  = 0;     /*  ./).. */
    gs->ft16   = 0;     /*  ./)) */
    gs->ft16p  = 0;     /*  ./)). */
    gs->ft16pp = 0;     /*  ./)).. */
    gs->ft32   = 0;     /*  ./))) */
    gs->ft32p  = 0;     /*  ./))). */
    gs->ft32pp = 0;     /*  ./))).. */
    gs->ft64   = 0;     /*  ./)))) */
    gs->ft64p  = 0;     /*  ./)))). */
    gs->ft64pp = 0;     /*  ./)))).. */
    gs->fa8    = 0;     /*  ./> */
    gs->fa8p   = 0;     /*  ./>. */
    gs->fa8pp  = 0;     /*  ./>.. */
    gs->fa16   = 0;     /*  ./>> */
    gs->fa16p  = 0;     /*  ./>>. */
    gs->fa16pp = 0;     /*  ./>>.. */
    gs->fa32   = 0;     /*  ./>>> */
    gs->fa32p  = 0;     /*  ./>>>. */
    gs->fa32pp = 0;     /*  ./>>>.. */
    gs->fa64   = 0;     /*  ./>>>> */
    gs->fa64p  = 0;     /*  ./>>>>. */
    gs->fa64pp = 0;     /*  ./>>>>.. */

    gc->ww    =  0;     /* |O| */
    gc->w     =  0;     /*  O  */
    gc->wp    =  0;     /*  O. */
    gc->wpp   =  0;     /*  O.. */
    gc->h     =  0;     /*  o/ */
    gc->hp    =  0;     /*  o/. */
    gc->hpp   =  0;     /*  o/.. */
    gc->q     =  0;     /*  _/ */
    gc->qp    =  0;     /*  _/. */
    gc->qpp   =  0;     /*  _/.. */
    gc->n8    =  0;     /*  _/) */
    gc->n8p   =  0;     /*  _/). */
    gc->n8pp  =  0;     /*  _/).. */
    gc->n16   =  0;     /*  _/)) */
    gc->n16p  =  0;     /*  _/)). */
    gc->n16pp =  0;     /*  _/)).. */
    gc->n32   =  0;     /*  _/))) */
    gc->n32p  =  0;     /*  _/))). */
    gc->n32pp =  0;     /*  _/))).. */
    gc->n64   =  0;     /*  _/)))) */
    gc->n64p  =  0;     /*  _/)))). */
    gc->n64pp =  0;     /*  _/)))).. */
    gc->t8    =  0;     /*  ./) */
    gc->t8p   =  0;     /*  ./). */
    gc->t8pp  =  0;     /*  ./).. */
    gc->t16   =  0;     /*  ./)) */
    gc->t16p  =  0;     /*  ./)). */
    gc->t16pp =  0;     /*  ./)).. */
    gc->t32   =  0;     /*  ./))) */
    gc->t32p  =  0;     /*  ./))). */
    gc->t32pp =  0;     /*  ./))).. */
    gc->t64   =  0;     /*  ./)))) */
    gc->t64p  =  0;     /*  ./)))). */
    gc->t64pp =  0;     /*  ./)))).. */
    gc->a8    =  0;     /*  ./> */
    gc->a8p   =  0;     /*  ./>. */
    gc->a8pp  =  0;     /*  ./>.. */
    gc->a16   =  0;     /*  ./>> */
    gc->a16p  =  0;     /*  ./>>. */
    gc->a16pp =  0;     /*  ./>>.. */
    gc->a32   =  0;     /*  ./>>> */
    gc->a32p  =  0;     /*  ./>>>. */
    gc->a32pp =  0;     /*  ./>>>.. */
    gc->a64   =  0;     /*  ./>>>> */
    gc->a64p  =  0;     /*  ./>>>>. */
    gc->a64pp =  0;     /*  ./>>>>.. */
    /* set modify Gate_rate on/off flag */
    gc->fww    = 0;     /* |O| */
    gc->fw     = 0;     /*  O  */
    gc->fwp    = 0;     /*  O. */
    gc->fwpp   = 0;     /*  O.. */
    gc->fh     = 0;     /*  o/ */
    gc->fhp    = 0;     /*  o/. */
    gc->fhpp   = 0;     /*  o/.. */
    gc->fq     = 0;     /*  _/ */
    gc->fqp    = 0;     /*  _/. */
    gc->fqpp   = 0;     /*  _/.. */
    gc->fn8    = 0;     /*  _/) */
    gc->fn8p   = 0;     /*  _/). */
    gc->fn8pp  = 0;     /*  _/).. */
    gc->fn16   = 0;     /*  _/)) */
    gc->fn16p  = 0;     /*  _/)). */
    gc->fn16pp = 0;     /*  _/)).. */
    gc->fn32   = 0;     /*  _/))) */
    gc->fn32p  = 0;     /*  _/))). */
    gc->fn32pp = 0;     /*  _/))).. */
    gc->fn64   = 0;     /*  _/)))) */
    gc->fn64p  = 0;     /*  _/)))). */
    gc->fn64pp = 0;     /*  _/)))).. */
    gc->ft8    = 0;     /*  ./) */
    gc->ft8p   = 0;     /*  ./). */
    gc->ft8pp  = 0;     /*  ./).. */
    gc->ft16   = 0;     /*  ./)) */
    gc->ft16p  = 0;     /*  ./)). */
    gc->ft16pp = 0;     /*  ./)).. */
    gc->ft32   = 0;     /*  ./))) */
    gc->ft32p  = 0;     /*  ./))). */
    gc->ft32pp = 0;     /*  ./))).. */
    gc->ft64   = 0;     /*  ./)))) */
    gc->ft64p  = 0;     /*  ./)))). */
    gc->ft64pp = 0;     /*  ./)))).. */
    gc->fa8    = 0;     /*  ./> */
    gc->fa8p   = 0;     /*  ./>. */
    gc->fa8pp  = 0;     /*  ./>.. */
    gc->fa16   = 0;     /*  ./>> */
    gc->fa16p  = 0;     /*  ./>>. */
    gc->fa16pp = 0;     /*  ./>>.. */
    gc->fa32   = 0;     /*  ./>>> */
    gc->fa32p  = 0;     /*  ./>>>. */
    gc->fa32pp = 0;     /*  ./>>>.. */
    gc->fa64   = 0;     /*  ./>>>> */
    gc->fa64p  = 0;     /*  ./>>>>. */
    gc->fa64pp = 0;     /*  ./>>>>.. */
    return(0);
}

/* call from beaumod_meb() */
/* set mb->bexp %xx data depend on mv->card sync value & defined gate_rate */
/* %xx _/).. */
int gate_rate_get()
{
    int  i,j;
    char w[12];

    /* printf("%s\n",mv->card); */
    if(mv->card[0] != '%' && mv->card[0] != 's') return(0);
    i = 4;
    j = 0;
    while(mv->card[i] != ' ') {
        w[j] = mv->card[i];
        i++;
        j++;
        if(j > 10) break;
    }    
    w[j] = '\0';
    gr_w_set(w);
    /* printf("w=%s\n",w); */
    if(!strcmp(w,"|O|"))      mb->bexp = gr->ww; 
    if(!strcmp(w,"O"))        mb->bexp = gr->w;
    if(!strcmp(w,"O."))       mb->bexp = gr->wp;
    if(!strcmp(w,"O.."))      mb->bexp = gr->wpp;
    if(!strcmp(w,"o/"))       mb->bexp = gr->h;
    if(!strcmp(w,"o/."))      mb->bexp = gr->hp;
    if(!strcmp(w,"o/.."))     mb->bexp = gr->hpp;
    if(!strcmp(w,"_/"))       mb->bexp = gr->q;
    if(!strcmp(w,"_/."))      mb->bexp = gr->qp;
    if(!strcmp(w,"_/.."))     mb->bexp = gr->qpp;
    if(!strcmp(w,"_/)"))      mb->bexp = gr->n8;
    if(!strcmp(w,"_/)."))     mb->bexp = gr->n8p;
    if(!strcmp(w,"_/).."))    mb->bexp = gr->n8pp;
    if(!strcmp(w,"_/))"))     mb->bexp = gr->n16;
    if(!strcmp(w,"_/))."))    mb->bexp = gr->n16p;
    if(!strcmp(w,"_/)).."))   mb->bexp = gr->n16pp;
    if(!strcmp(w,"_/)))"))    mb->bexp = gr->n32;
    if(!strcmp(w,"_/)))."))   mb->bexp = gr->n32p;
    if(!strcmp(w,"_/))).."))  mb->bexp = gr->n32pp;
    if(!strcmp(w,"_/))))"))   mb->bexp = gr->n64;
    if(!strcmp(w,"_/))))."))  mb->bexp = gr->n64p;
    if(!strcmp(w,"_/))))..")) mb->bexp = gr->n64pp;
    if(!strcmp(w,"./)"))      mb->bexp = gr->t8;
    if(!strcmp(w,"./)."))     mb->bexp = gr->t8p;
    if(!strcmp(w,"./).."))    mb->bexp = gr->t8pp;
    if(!strcmp(w,"./))"))     mb->bexp = gr->t16;
    if(!strcmp(w,"./))."))    mb->bexp = gr->t16p;
    if(!strcmp(w,"./)).."))   mb->bexp = gr->t16pp;
    if(!strcmp(w,"./)))"))    mb->bexp = gr->t32;
    if(!strcmp(w,"./)))."))   mb->bexp = gr->t32p;
    if(!strcmp(w,"./))).."))  mb->bexp = gr->t32pp;
    if(!strcmp(w,"./))))"))   mb->bexp = gr->t64;
    if(!strcmp(w,"./))))."))  mb->bexp = gr->t64p;
    if(!strcmp(w,"./))))..")) mb->bexp = gr->t64pp;
    if(!strcmp(w,"./>"))      mb->bexp = gr->a8;
    if(!strcmp(w,"./>."))     mb->bexp = gr->a8p;
    if(!strcmp(w,"./>.."))    mb->bexp = gr->a8pp;
    if(!strcmp(w,"./>>"))     mb->bexp = gr->a16;
    if(!strcmp(w,"./>>."))    mb->bexp = gr->a16p;
    if(!strcmp(w,"./>>.."))   mb->bexp = gr->a16pp;
    if(!strcmp(w,"./>>>"))    mb->bexp = gr->a32;
    if(!strcmp(w,"./>>>."))   mb->bexp = gr->a32p;
    if(!strcmp(w,"./>>>.."))  mb->bexp = gr->a32pp;
    if(!strcmp(w,"./>>>>"))   mb->bexp = gr->a64;
    if(!strcmp(w,"./>>>>."))  mb->bexp = gr->a64p;
    if(!strcmp(w,"./>>>>..")) mb->bexp = gr->a64pp;
    return(0);
}

/* call from beaumod_meb() */
/* set ge->def_bexp %xx data depend on mv->card sync value & defined gate_rate */
/* %xx _/).. */
int def_gr_get()
{
    int  i,j;
    int  gflag;
    char w[12];
    int  gf;

    /* printf("%s\n",mv->card); */
    if(mv->card[0] != '%' && mv->card[0] != 's') return(0);
    i = 4;
    j = 0;
    gf = 0;
    while(mv->card[i] != ' ') {
        if(mv->card[i] == '[') gf = 1;
        w[j] = mv->card[i];
        i++;
        j++;
        if(j > 10) break;
    }    
    w[j] = '\0';
    gr_w_set(w);
    if(gf) {
        syndefgr_meb();
        return(0);
    }
    /* printf("w=%s\n",w); */
    if(!strcmp(w,"|O|"))      ge->def_bexp = gd->ww; 
    if(!strcmp(w,"O"))        ge->def_bexp = gd->w;
    if(!strcmp(w,"O."))       ge->def_bexp = gd->wp;
    if(!strcmp(w,"O.."))      ge->def_bexp = gd->wpp;
    if(!strcmp(w,"o/"))       ge->def_bexp = gd->h;
    if(!strcmp(w,"o/."))      ge->def_bexp = gd->hp;
    if(!strcmp(w,"o/.."))     ge->def_bexp = gd->hpp;
    if(!strcmp(w,"_/"))       ge->def_bexp = gd->q;
    if(!strcmp(w,"_/."))      ge->def_bexp = gd->qp;
    if(!strcmp(w,"_/.."))     ge->def_bexp = gd->qpp;
    if(!strcmp(w,"_/)"))      ge->def_bexp = gd->n8;
    if(!strcmp(w,"_/)."))     ge->def_bexp = gd->n8p;
    if(!strcmp(w,"_/).."))    ge->def_bexp = gd->n8pp;
    if(!strcmp(w,"_/))"))     ge->def_bexp = gd->n16;
    if(!strcmp(w,"_/))."))    ge->def_bexp = gd->n16p;
    if(!strcmp(w,"_/)).."))   ge->def_bexp = gd->n16pp;
    if(!strcmp(w,"_/)))"))    ge->def_bexp = gd->n32;
    if(!strcmp(w,"_/)))."))   ge->def_bexp = gd->n32p;
    if(!strcmp(w,"_/))).."))  ge->def_bexp = gd->n32pp;
    if(!strcmp(w,"_/))))"))   ge->def_bexp = gd->n64;
    if(!strcmp(w,"_/))))."))  ge->def_bexp = gd->n64p;
    if(!strcmp(w,"_/))))..")) ge->def_bexp = gd->n64pp;
    if(!strcmp(w,"./)"))      ge->def_bexp = gd->t8;
    if(!strcmp(w,"./)."))     ge->def_bexp = gd->t8p;
    if(!strcmp(w,"./).."))    ge->def_bexp = gd->t8pp;
    if(!strcmp(w,"./))"))     ge->def_bexp = gd->t16;
    if(!strcmp(w,"./))."))    ge->def_bexp = gd->t16p;
    if(!strcmp(w,"./)).."))   ge->def_bexp = gd->t16pp;
    if(!strcmp(w,"./)))"))    ge->def_bexp = gd->t32;
    if(!strcmp(w,"./)))."))   ge->def_bexp = gd->t32p;
    if(!strcmp(w,"./))).."))  ge->def_bexp = gd->t32pp;
    if(!strcmp(w,"./))))"))   ge->def_bexp = gd->t64;
    if(!strcmp(w,"./))))."))  ge->def_bexp = gd->t64p;
    if(!strcmp(w,"./))))..")) ge->def_bexp = gd->t64pp;
    if(!strcmp(w,"./>"))      ge->def_bexp = gd->a8;
    if(!strcmp(w,"./>."))     ge->def_bexp = gd->a8p;
    if(!strcmp(w,"./>.."))    ge->def_bexp = gd->a8pp;
    if(!strcmp(w,"./>>"))     ge->def_bexp = gd->a16;
    if(!strcmp(w,"./>>."))    ge->def_bexp = gd->a16p;
    if(!strcmp(w,"./>>.."))   ge->def_bexp = gd->a16pp;
    if(!strcmp(w,"./>>>"))    ge->def_bexp = gd->a32;
    if(!strcmp(w,"./>>>."))   ge->def_bexp = gd->a32p;
    if(!strcmp(w,"./>>>.."))  ge->def_bexp = gd->a32pp;
    if(!strcmp(w,"./>>>>"))   ge->def_bexp = gd->a64;
    if(!strcmp(w,"./>>>>."))  ge->def_bexp = gd->a64p;
    if(!strcmp(w,"./>>>>..")) ge->def_bexp = gd->a64pp;
    /* Check set & modify flag */
    if(!strcmp(w,"|O|"))      gflag = gd->fww; 
    if(!strcmp(w,"O"))        gflag = gd->fw;
    if(!strcmp(w,"O."))       gflag = gd->fwp;
    if(!strcmp(w,"O.."))      gflag = gd->fwpp;
    if(!strcmp(w,"o/"))       gflag = gd->fh;
    if(!strcmp(w,"o/."))      gflag = gd->fhp;
    if(!strcmp(w,"o/.."))     gflag = gd->fhpp;
    if(!strcmp(w,"_/"))       gflag = gd->fq;
    if(!strcmp(w,"_/."))      gflag = gd->fqp;
    if(!strcmp(w,"_/.."))     gflag = gd->fqpp;
    if(!strcmp(w,"_/)"))      gflag = gd->fn8;
    if(!strcmp(w,"_/)."))     gflag = gd->fn8p;
    if(!strcmp(w,"_/).."))    gflag = gd->fn8pp;
    if(!strcmp(w,"_/))"))     gflag = gd->fn16;
    if(!strcmp(w,"_/))."))    gflag = gd->fn16p;
    if(!strcmp(w,"_/)).."))   gflag = gd->fn16pp;
    if(!strcmp(w,"_/)))"))    gflag = gd->fn32;
    if(!strcmp(w,"_/)))."))   gflag = gd->fn32p;
    if(!strcmp(w,"_/))).."))  gflag = gd->fn32pp;
    if(!strcmp(w,"_/))))"))   gflag = gd->fn64;
    if(!strcmp(w,"_/))))."))  gflag = gd->fn64p;
    if(!strcmp(w,"_/))))..")) gflag = gd->fn64pp;
    if(!strcmp(w,"./)"))      gflag = gd->ft8;
    if(!strcmp(w,"./)."))     gflag = gd->ft8p;
    if(!strcmp(w,"./).."))    gflag = gd->ft8pp;
    if(!strcmp(w,"./))"))     gflag = gd->ft16;
    if(!strcmp(w,"./))."))    gflag = gd->ft16p;
    if(!strcmp(w,"./)).."))   gflag = gd->ft16pp;
    if(!strcmp(w,"./)))"))    gflag = gd->ft32;
    if(!strcmp(w,"./)))."))   gflag = gd->ft32p;
    if(!strcmp(w,"./))).."))  gflag = gd->ft32pp;
    if(!strcmp(w,"./))))"))   gflag = gd->ft64;
    if(!strcmp(w,"./))))."))  gflag = gd->ft64p;
    if(!strcmp(w,"./))))..")) gflag = gd->ft64pp;
    if(!strcmp(w,"./>"))      gflag = gd->fa8;
    if(!strcmp(w,"./>."))     gflag = gd->fa8p;
    if(!strcmp(w,"./>.."))    gflag = gd->fa8pp;
    if(!strcmp(w,"./>>"))     gflag = gd->fa16;
    if(!strcmp(w,"./>>."))    gflag = gd->fa16p;
    if(!strcmp(w,"./>>.."))   gflag = gd->fa16pp;
    if(!strcmp(w,"./>>>"))    gflag = gd->fa32;
    if(!strcmp(w,"./>>>."))   gflag = gd->fa32p;
    if(!strcmp(w,"./>>>.."))  gflag = gd->fa32pp;
    if(!strcmp(w,"./>>>>"))   gflag = gd->fa64;
    if(!strcmp(w,"./>>>>."))  gflag = gd->fa64p;
    if(!strcmp(w,"./>>>>..")) gflag = gd->fa64pp;
    if(mv->debug == 13) printf("def_gr_get() gflag=%d bexp=%d\n",
        gflag,ge->def_bexp);
    if(gflag != ge->def_bexp) return(1);
    return(0);
}

/* call from beaumod_meb() */
/* set ge->asn_bexp %xx data depend on mv->card sync value & defined gate_rate */
/* %xx _/).. */
int asn_gr_get()
{
    int  i,j;
    int  gflag;
    char w[12];
    int  gf;

    /* printf("%s\n",mv->card); */
    if(mv->card[0] != '%' && mv->card[0] != 's') return(0);
    gf = 0;
    i = 4;
    j = 0;
    while(mv->card[i] != ' ') {
        if(mv->card[i] == '[') gf = 1;
        w[j] = mv->card[i];
        i++;
        j++;
        if(j > 10) break;
    }    
    w[j] = '\0';
    gr_w_set(w);
    if(gf) {
        synasngr_meb();
        return(0);
    }
    /* printf("w=%s\n",w); */
    if(!strcmp(w,"|O|"))      ge->asn_bexp = ga->ww; 
    if(!strcmp(w,"O"))        ge->asn_bexp = ga->w;
    if(!strcmp(w,"O."))       ge->asn_bexp = ga->wp;
    if(!strcmp(w,"O.."))      ge->asn_bexp = ga->wpp;
    if(!strcmp(w,"o/"))       ge->asn_bexp = ga->h;
    if(!strcmp(w,"o/."))      ge->asn_bexp = ga->hp;
    if(!strcmp(w,"o/.."))     ge->asn_bexp = ga->hpp;
    if(!strcmp(w,"_/"))       ge->asn_bexp = ga->q;
    if(!strcmp(w,"_/."))      ge->asn_bexp = ga->qp;
    if(!strcmp(w,"_/.."))     ge->asn_bexp = ga->qpp;
    if(!strcmp(w,"_/)"))      ge->asn_bexp = ga->n8;
    if(!strcmp(w,"_/)."))     ge->asn_bexp = ga->n8p;
    if(!strcmp(w,"_/).."))    ge->asn_bexp = ga->n8pp;
    if(!strcmp(w,"_/))"))     ge->asn_bexp = ga->n16;
    if(!strcmp(w,"_/))."))    ge->asn_bexp = ga->n16p;
    if(!strcmp(w,"_/)).."))   ge->asn_bexp = ga->n16pp;
    if(!strcmp(w,"_/)))"))    ge->asn_bexp = ga->n32;
    if(!strcmp(w,"_/)))."))   ge->asn_bexp = ga->n32p;
    if(!strcmp(w,"_/))).."))  ge->asn_bexp = ga->n32pp;
    if(!strcmp(w,"_/))))"))   ge->asn_bexp = ga->n64;
    if(!strcmp(w,"_/))))."))  ge->asn_bexp = ga->n64p;
    if(!strcmp(w,"_/))))..")) ge->asn_bexp = ga->n64pp;
    if(!strcmp(w,"./)"))      ge->asn_bexp = ga->t8;
    if(!strcmp(w,"./)."))     ge->asn_bexp = ga->t8p;
    if(!strcmp(w,"./).."))    ge->asn_bexp = ga->t8pp;
    if(!strcmp(w,"./))"))     ge->asn_bexp = ga->t16;
    if(!strcmp(w,"./))."))    ge->asn_bexp = ga->t16p;
    if(!strcmp(w,"./)).."))   ge->asn_bexp = ga->t16pp;
    if(!strcmp(w,"./)))"))    ge->asn_bexp = ga->t32;
    if(!strcmp(w,"./)))."))   ge->asn_bexp = ga->t32p;
    if(!strcmp(w,"./))).."))  ge->asn_bexp = ga->t32pp;
    if(!strcmp(w,"./))))"))   ge->asn_bexp = ga->t64;
    if(!strcmp(w,"./))))."))  ge->asn_bexp = ga->t64p;
    if(!strcmp(w,"./))))..")) ge->asn_bexp = ga->t64pp;
    if(!strcmp(w,"./>"))      ge->asn_bexp = ga->a8;
    if(!strcmp(w,"./>."))     ge->asn_bexp = ga->a8p;
    if(!strcmp(w,"./>.."))    ge->asn_bexp = ga->a8pp;
    if(!strcmp(w,"./>>"))     ge->asn_bexp = ga->a16;
    if(!strcmp(w,"./>>."))    ge->asn_bexp = ga->a16p;
    if(!strcmp(w,"./>>.."))   ge->asn_bexp = ga->a16pp;
    if(!strcmp(w,"./>>>"))    ge->asn_bexp = ga->a32;
    if(!strcmp(w,"./>>>."))   ge->asn_bexp = ga->a32p;
    if(!strcmp(w,"./>>>.."))  ge->asn_bexp = ga->a32pp;
    if(!strcmp(w,"./>>>>"))   ge->asn_bexp = ga->a64;
    if(!strcmp(w,"./>>>>."))  ge->asn_bexp = ga->a64p;
    if(!strcmp(w,"./>>>>..")) ge->asn_bexp = ga->a64pp;
    /* Check set & modify flag */
    if(!strcmp(w,"|O|"))      gflag = ga->fww; 
    if(!strcmp(w,"O"))        gflag = ga->fw;
    if(!strcmp(w,"O."))       gflag = ga->fwp;
    if(!strcmp(w,"O.."))      gflag = ga->fwpp;
    if(!strcmp(w,"o/"))       gflag = ga->fh;
    if(!strcmp(w,"o/."))      gflag = ga->fhp;
    if(!strcmp(w,"o/.."))     gflag = ga->fhpp;
    if(!strcmp(w,"_/"))       gflag = ga->fq;
    if(!strcmp(w,"_/."))      gflag = ga->fqp;
    if(!strcmp(w,"_/.."))     gflag = ga->fqpp;
    if(!strcmp(w,"_/)"))      gflag = ga->fn8;
    if(!strcmp(w,"_/)."))     gflag = ga->fn8p;
    if(!strcmp(w,"_/).."))    gflag = ga->fn8pp;
    if(!strcmp(w,"_/))"))     gflag = ga->fn16;
    if(!strcmp(w,"_/))."))    gflag = ga->fn16p;
    if(!strcmp(w,"_/)).."))   gflag = ga->fn16pp;
    if(!strcmp(w,"_/)))"))    gflag = ga->fn32;
    if(!strcmp(w,"_/)))."))   gflag = ga->fn32p;
    if(!strcmp(w,"_/))).."))  gflag = ga->fn32pp;
    if(!strcmp(w,"_/))))"))   gflag = ga->fn64;
    if(!strcmp(w,"_/))))."))  gflag = ga->fn64p;
    if(!strcmp(w,"_/))))..")) gflag = ga->fn64pp;
    if(!strcmp(w,"./)"))      gflag = ga->ft8;
    if(!strcmp(w,"./)."))     gflag = ga->ft8p;
    if(!strcmp(w,"./).."))    gflag = ga->ft8pp;
    if(!strcmp(w,"./))"))     gflag = ga->ft16;
    if(!strcmp(w,"./))."))    gflag = ga->ft16p;
    if(!strcmp(w,"./)).."))   gflag = ga->ft16pp;
    if(!strcmp(w,"./)))"))    gflag = ga->ft32;
    if(!strcmp(w,"./)))."))   gflag = ga->ft32p;
    if(!strcmp(w,"./))).."))  gflag = ga->ft32pp;
    if(!strcmp(w,"./))))"))   gflag = ga->ft64;
    if(!strcmp(w,"./))))."))  gflag = ga->ft64p;
    if(!strcmp(w,"./))))..")) gflag = ga->ft64pp;
    if(!strcmp(w,"./>"))      gflag = ga->fa8;
    if(!strcmp(w,"./>."))     gflag = ga->fa8p;
    if(!strcmp(w,"./>.."))    gflag = ga->fa8pp;
    if(!strcmp(w,"./>>"))     gflag = ga->fa16;
    if(!strcmp(w,"./>>."))    gflag = ga->fa16p;
    if(!strcmp(w,"./>>.."))   gflag = ga->fa16pp;
    if(!strcmp(w,"./>>>"))    gflag = ga->fa32;
    if(!strcmp(w,"./>>>."))   gflag = ga->fa32p;
    if(!strcmp(w,"./>>>.."))  gflag = ga->fa32pp;
    if(!strcmp(w,"./>>>>"))   gflag = ga->fa64;
    if(!strcmp(w,"./>>>>."))  gflag = ga->fa64p;
    if(!strcmp(w,"./>>>>..")) gflag = ga->fa64pp;
    if(mv->debug == 13) printf("asn_gr_get() gflag=%d bexp=%d\n",
        gflag,ge->asn_bexp);
    if(gflag != ge->asn_bexp) return(1);
    return(0);
}

/* call from beaumod_meb() */
/* set ge->mod_bexp %xx data depend on mv->card sync value & defined gate_rate */
/* %xx _/).. */
int mod_gr_get()
{
    int  i,j;
    int  gflag;
    char w[12];
    int  gf;

    /* printf("%s\n",mv->card); */
    if(mv->card[0] != '%' && mv->card[0] != 's') return(0);
    gf = 0;
    i = 4;
    j = 0;
    while(mv->card[i] != ' ') {
        if(mv->card[i] == '[') gf = 1;
        w[j] = mv->card[i];
        i++;
        j++;
        if(j > 10) break;
    }    
    w[j] = '\0';
    gr_w_set(w);
    if(gf) {
        synmodgr_meb();
        return(0);
    }
    /* printf("w=%s\n",w); */
    if(!strcmp(w,"|O|"))      ge->mod_bexp = gm->ww; 
    if(!strcmp(w,"O"))        ge->mod_bexp = gm->w;
    if(!strcmp(w,"O."))       ge->mod_bexp = gm->wp;
    if(!strcmp(w,"O.."))      ge->mod_bexp = gm->wpp;
    if(!strcmp(w,"o/"))       ge->mod_bexp = gm->h;
    if(!strcmp(w,"o/."))      ge->mod_bexp = gm->hp;
    if(!strcmp(w,"o/.."))     ge->mod_bexp = gm->hpp;
    if(!strcmp(w,"_/"))       ge->mod_bexp = gm->q;
    if(!strcmp(w,"_/."))      ge->mod_bexp = gm->qp;
    if(!strcmp(w,"_/.."))     ge->mod_bexp = gm->qpp;
    if(!strcmp(w,"_/)"))      ge->mod_bexp = gm->n8;
    if(!strcmp(w,"_/)."))     ge->mod_bexp = gm->n8p;
    if(!strcmp(w,"_/).."))    ge->mod_bexp = gm->n8pp;
    if(!strcmp(w,"_/))"))     ge->mod_bexp = gm->n16;
    if(!strcmp(w,"_/))."))    ge->mod_bexp = gm->n16p;
    if(!strcmp(w,"_/)).."))   ge->mod_bexp = gm->n16pp;
    if(!strcmp(w,"_/)))"))    ge->mod_bexp = gm->n32;
    if(!strcmp(w,"_/)))."))   ge->mod_bexp = gm->n32p;
    if(!strcmp(w,"_/))).."))  ge->mod_bexp = gm->n32pp;
    if(!strcmp(w,"_/))))"))   ge->mod_bexp = gm->n64;
    if(!strcmp(w,"_/))))."))  ge->mod_bexp = gm->n64p;
    if(!strcmp(w,"_/))))..")) ge->mod_bexp = gm->n64pp;
    if(!strcmp(w,"./)"))      ge->mod_bexp = gm->t8;
    if(!strcmp(w,"./)."))     ge->mod_bexp = gm->t8p;
    if(!strcmp(w,"./).."))    ge->mod_bexp = gm->t8pp;
    if(!strcmp(w,"./))"))     ge->mod_bexp = gm->t16;
    if(!strcmp(w,"./))."))    ge->mod_bexp = gm->t16p;
    if(!strcmp(w,"./)).."))   ge->mod_bexp = gm->t16pp;
    if(!strcmp(w,"./)))"))    ge->mod_bexp = gm->t32;
    if(!strcmp(w,"./)))."))   ge->mod_bexp = gm->t32p;
    if(!strcmp(w,"./))).."))  ge->mod_bexp = gm->t32pp;
    if(!strcmp(w,"./))))"))   ge->mod_bexp = gm->t64;
    if(!strcmp(w,"./))))."))  ge->mod_bexp = gm->t64p;
    if(!strcmp(w,"./))))..")) ge->mod_bexp = gm->t64pp;
    if(!strcmp(w,"./>"))      ge->mod_bexp = gm->a8;
    if(!strcmp(w,"./>."))     ge->mod_bexp = gm->a8p;
    if(!strcmp(w,"./>.."))    ge->mod_bexp = gm->a8pp;
    if(!strcmp(w,"./>>"))     ge->mod_bexp = gm->a16;
    if(!strcmp(w,"./>>."))    ge->mod_bexp = gm->a16p;
    if(!strcmp(w,"./>>.."))   ge->mod_bexp = gm->a16pp;
    if(!strcmp(w,"./>>>"))    ge->mod_bexp = gm->a32;
    if(!strcmp(w,"./>>>."))   ge->mod_bexp = gm->a32p;
    if(!strcmp(w,"./>>>.."))  ge->mod_bexp = gm->a32pp;
    if(!strcmp(w,"./>>>>"))   ge->mod_bexp = gm->a64;
    if(!strcmp(w,"./>>>>."))  ge->mod_bexp = gm->a64p;
    if(!strcmp(w,"./>>>>..")) ge->mod_bexp = gm->a64pp;
    /* Check set & modify flag */
    if(!strcmp(w,"|O|"))      gflag = gm->fww; 
    if(!strcmp(w,"O"))        gflag = gm->fw;
    if(!strcmp(w,"O."))       gflag = gm->fwp;
    if(!strcmp(w,"O.."))      gflag = gm->fwpp;
    if(!strcmp(w,"o/"))       gflag = gm->fh;
    if(!strcmp(w,"o/."))      gflag = gm->fhp;
    if(!strcmp(w,"o/.."))     gflag = gm->fhpp;
    if(!strcmp(w,"_/"))       gflag = gm->fq;
    if(!strcmp(w,"_/."))      gflag = gm->fqp;
    if(!strcmp(w,"_/.."))     gflag = gm->fqpp;
    if(!strcmp(w,"_/)"))      gflag = gm->fn8;
    if(!strcmp(w,"_/)."))     gflag = gm->fn8p;
    if(!strcmp(w,"_/).."))    gflag = gm->fn8pp;
    if(!strcmp(w,"_/))"))     gflag = gm->fn16;
    if(!strcmp(w,"_/))."))    gflag = gm->fn16p;
    if(!strcmp(w,"_/)).."))   gflag = gm->fn16pp;
    if(!strcmp(w,"_/)))"))    gflag = gm->fn32;
    if(!strcmp(w,"_/)))."))   gflag = gm->fn32p;
    if(!strcmp(w,"_/))).."))  gflag = gm->fn32pp;
    if(!strcmp(w,"_/))))"))   gflag = gm->fn64;
    if(!strcmp(w,"_/))))."))  gflag = gm->fn64p;
    if(!strcmp(w,"_/))))..")) gflag = gm->fn64pp;
    if(!strcmp(w,"./)"))      gflag = gm->ft8;
    if(!strcmp(w,"./)."))     gflag = gm->ft8p;
    if(!strcmp(w,"./).."))    gflag = gm->ft8pp;
    if(!strcmp(w,"./))"))     gflag = gm->ft16;
    if(!strcmp(w,"./))."))    gflag = gm->ft16p;
    if(!strcmp(w,"./)).."))   gflag = gm->ft16pp;
    if(!strcmp(w,"./)))"))    gflag = gm->ft32;
    if(!strcmp(w,"./)))."))   gflag = gm->ft32p;
    if(!strcmp(w,"./))).."))  gflag = gm->ft32pp;
    if(!strcmp(w,"./))))"))   gflag = gm->ft64;
    if(!strcmp(w,"./))))."))  gflag = gm->ft64p;
    if(!strcmp(w,"./))))..")) gflag = gm->ft64pp;
    if(!strcmp(w,"./>"))      gflag = gm->fa8;
    if(!strcmp(w,"./>."))     gflag = gm->fa8p;
    if(!strcmp(w,"./>.."))    gflag = gm->fa8pp;
    if(!strcmp(w,"./>>"))     gflag = gm->fa16;
    if(!strcmp(w,"./>>."))    gflag = gm->fa16p;
    if(!strcmp(w,"./>>.."))   gflag = gm->fa16pp;
    if(!strcmp(w,"./>>>"))    gflag = gm->fa32;
    if(!strcmp(w,"./>>>."))   gflag = gm->fa32p;
    if(!strcmp(w,"./>>>.."))  gflag = gm->fa32pp;
    if(!strcmp(w,"./>>>>"))   gflag = gm->fa64;
    if(!strcmp(w,"./>>>>."))  gflag = gm->fa64p;
    if(!strcmp(w,"./>>>>..")) gflag = gm->fa64pp;
    if(mv->debug == 13) printf("mod_gr_get() gflag=%d bexp=%d\n",
        gflag,ge->mod_bexp);
    if(gflag != ge->mod_bexp) return(1);
    return(0);
}

/* call from beaumod_meb() */
/* set ge->set_bexp %xx data depend on mv->card sync value & defined gate_rate */
/* %xx _/).. */
int set_gr_get()
{
    int  i,j;
    int  gflag;
    char w[12];
    int  gf;
    int  rep;

    /* printf("%s\n",mv->card); */
    if(mv->card[0] != '%' && mv->card[0] != 's') return(0);
    gf = 0;
    i = 4;
    j = 0;
    while(mv->card[i] != ' ') {
        if(mv->card[i] == '[') gf = 1;
        w[j] = mv->card[i];
        i++;
        j++;
        if(j > 10) break;
    }    
    w[j] = '\0';
    gr_w_set(w);
    if(gf) {
        synsetgr_meb();
        return(0);
    }
    /* printf("w=%s\n",w); */
    if(!strcmp(w,"|O|"))      ge->set_bexp = gs->ww; 
    if(!strcmp(w,"O"))        ge->set_bexp = gs->w;
    if(!strcmp(w,"O."))       ge->set_bexp = gs->wp;
    if(!strcmp(w,"O.."))      ge->set_bexp = gs->wpp;
    if(!strcmp(w,"o/"))       ge->set_bexp = gs->h;
    if(!strcmp(w,"o/."))      ge->set_bexp = gs->hp;
    if(!strcmp(w,"o/.."))     ge->set_bexp = gs->hpp;
    if(!strcmp(w,"_/"))       ge->set_bexp = gs->q;
    if(!strcmp(w,"_/."))      ge->set_bexp = gs->qp;
    if(!strcmp(w,"_/.."))     ge->set_bexp = gs->qpp;
    if(!strcmp(w,"_/)"))      ge->set_bexp = gs->n8;
    if(!strcmp(w,"_/)."))     ge->set_bexp = gs->n8p;
    if(!strcmp(w,"_/).."))    ge->set_bexp = gs->n8pp;
    if(!strcmp(w,"_/))"))     ge->set_bexp = gs->n16;
    if(!strcmp(w,"_/))."))    ge->set_bexp = gs->n16p;
    if(!strcmp(w,"_/)).."))   ge->set_bexp = gs->n16pp;
    if(!strcmp(w,"_/)))"))    ge->set_bexp = gs->n32;
    if(!strcmp(w,"_/)))."))   ge->set_bexp = gs->n32p;
    if(!strcmp(w,"_/))).."))  ge->set_bexp = gs->n32pp;
    if(!strcmp(w,"_/))))"))   ge->set_bexp = gs->n64;
    if(!strcmp(w,"_/))))."))  ge->set_bexp = gs->n64p;
    if(!strcmp(w,"_/))))..")) ge->set_bexp = gs->n64pp;
    if(!strcmp(w,"./)"))      ge->set_bexp = gs->t8;
    if(!strcmp(w,"./)."))     ge->set_bexp = gs->t8p;
    if(!strcmp(w,"./).."))    ge->set_bexp = gs->t8pp;
    if(!strcmp(w,"./))"))     ge->set_bexp = gs->t16;
    if(!strcmp(w,"./))."))    ge->set_bexp = gs->t16p;
    if(!strcmp(w,"./)).."))   ge->set_bexp = gs->t16pp;
    if(!strcmp(w,"./)))"))    ge->set_bexp = gs->t32;
    if(!strcmp(w,"./)))."))   ge->set_bexp = gs->t32p;
    if(!strcmp(w,"./))).."))  ge->set_bexp = gs->t32pp;
    if(!strcmp(w,"./))))"))   ge->set_bexp = gs->t64;
    if(!strcmp(w,"./))))."))  ge->set_bexp = gs->t64p;
    if(!strcmp(w,"./))))..")) ge->set_bexp = gs->t64pp;
    if(!strcmp(w,"./>"))      ge->set_bexp = gs->a8;
    if(!strcmp(w,"./>."))     ge->set_bexp = gs->a8p;
    if(!strcmp(w,"./>.."))    ge->set_bexp = gs->a8pp;
    if(!strcmp(w,"./>>"))     ge->set_bexp = gs->a16;
    if(!strcmp(w,"./>>."))    ge->set_bexp = gs->a16p;
    if(!strcmp(w,"./>>.."))   ge->set_bexp = gs->a16pp;
    if(!strcmp(w,"./>>>"))    ge->set_bexp = gs->a32;
    if(!strcmp(w,"./>>>."))   ge->set_bexp = gs->a32p;
    if(!strcmp(w,"./>>>.."))  ge->set_bexp = gs->a32pp;
    if(!strcmp(w,"./>>>>"))   ge->set_bexp = gs->a64;
    if(!strcmp(w,"./>>>>."))  ge->set_bexp = gs->a64p;
    if(!strcmp(w,"./>>>>..")) ge->set_bexp = gs->a64pp;
    /* Check set & modify flag */
    if(!strcmp(w,"|O|"))      gflag = gs->fww; 
    if(!strcmp(w,"O"))        gflag = gs->fw;
    if(!strcmp(w,"O."))       gflag = gs->fwp;
    if(!strcmp(w,"O.."))      gflag = gs->fwpp;
    if(!strcmp(w,"o/"))       gflag = gs->fh;
    if(!strcmp(w,"o/."))      gflag = gs->fhp;
    if(!strcmp(w,"o/.."))     gflag = gs->fhpp;
    if(!strcmp(w,"_/"))       gflag = gs->fq;
    if(!strcmp(w,"_/."))      gflag = gs->fqp;
    if(!strcmp(w,"_/.."))     gflag = gs->fqpp;
    if(!strcmp(w,"_/)"))      gflag = gs->fn8;
    if(!strcmp(w,"_/)."))     gflag = gs->fn8p;
    if(!strcmp(w,"_/).."))    gflag = gs->fn8pp;
    if(!strcmp(w,"_/))"))     gflag = gs->fn16;
    if(!strcmp(w,"_/))."))    gflag = gs->fn16p;
    if(!strcmp(w,"_/)).."))   gflag = gs->fn16pp;
    if(!strcmp(w,"_/)))"))    gflag = gs->fn32;
    if(!strcmp(w,"_/)))."))   gflag = gs->fn32p;
    if(!strcmp(w,"_/))).."))  gflag = gs->fn32pp;
    if(!strcmp(w,"_/))))"))   gflag = gs->fn64;
    if(!strcmp(w,"_/))))."))  gflag = gs->fn64p;
    if(!strcmp(w,"_/))))..")) gflag = gs->fn64pp;
    if(!strcmp(w,"./)"))      gflag = gs->ft8;
    if(!strcmp(w,"./)."))     gflag = gs->ft8p;
    if(!strcmp(w,"./).."))    gflag = gs->ft8pp;
    if(!strcmp(w,"./))"))     gflag = gs->ft16;
    if(!strcmp(w,"./))."))    gflag = gs->ft16p;
    if(!strcmp(w,"./)).."))   gflag = gs->ft16pp;
    if(!strcmp(w,"./)))"))    gflag = gs->ft32;
    if(!strcmp(w,"./)))."))   gflag = gs->ft32p;
    if(!strcmp(w,"./))).."))  gflag = gs->ft32pp;
    if(!strcmp(w,"./))))"))   gflag = gs->ft64;
    if(!strcmp(w,"./))))."))  gflag = gs->ft64p;
    if(!strcmp(w,"./))))..")) gflag = gs->ft64pp;
    if(!strcmp(w,"./>"))      gflag = gs->fa8;
    if(!strcmp(w,"./>."))     gflag = gs->fa8p;
    if(!strcmp(w,"./>.."))    gflag = gs->fa8pp;
    if(!strcmp(w,"./>>"))     gflag = gs->fa16;
    if(!strcmp(w,"./>>."))    gflag = gs->fa16p;
    if(!strcmp(w,"./>>.."))   gflag = gs->fa16pp;
    if(!strcmp(w,"./>>>"))    gflag = gs->fa32;
    if(!strcmp(w,"./>>>."))   gflag = gs->fa32p;
    if(!strcmp(w,"./>>>.."))  gflag = gs->fa32pp;
    if(!strcmp(w,"./>>>>"))   gflag = gs->fa64;
    if(!strcmp(w,"./>>>>."))  gflag = gs->fa64p;
    if(!strcmp(w,"./>>>>..")) gflag = gs->fa64pp;
    if(mv->debug == 13) printf("set_gr_get() gflag=%d bexp=%d\n",
        gflag,ge->set_bexp);
    if(gflag != ge->set_bexp) return(1);
    return(0);
}

/* call from beaumod_meb() */
int asc_gr_get()
{
    int  i,j;
    int  gflag;
    char w[12];
    int  gf;

    /* printf("%s\n",mv->card); */
    if(mv->card[0] != '%' && mv->card[0] != 's') return(0);
    gf = 0;
    i = 4;
    j = 0;
    while(mv->card[i] != ' ') {
        if(mv->card[i] == '[') gf = 1;
        w[j] = mv->card[i];
        i++;
        j++;
        if(j > 10) break;
    }    
    w[j] = '\0';
    gr_w_set(w);
    if(gf) {
        synascgr_meb();
        return(0);
    }
    /* printf("w=%s\n",w); */
    if(!strcmp(w,"|O|"))      ge->asc_bexp = gc->ww; 
    if(!strcmp(w,"O"))        ge->asc_bexp = gc->w;
    if(!strcmp(w,"O."))       ge->asc_bexp = gc->wp;
    if(!strcmp(w,"O.."))      ge->asc_bexp = gc->wpp;
    if(!strcmp(w,"o/"))       ge->asc_bexp = gc->h;
    if(!strcmp(w,"o/."))      ge->asc_bexp = gc->hp;
    if(!strcmp(w,"o/.."))     ge->asc_bexp = gc->hpp;
    if(!strcmp(w,"_/"))       ge->asc_bexp = gc->q;
    if(!strcmp(w,"_/."))      ge->asc_bexp = gc->qp;
    if(!strcmp(w,"_/.."))     ge->asc_bexp = gc->qpp;
    if(!strcmp(w,"_/)"))      ge->asc_bexp = gc->n8;
    if(!strcmp(w,"_/)."))     ge->asc_bexp = gc->n8p;
    if(!strcmp(w,"_/).."))    ge->asc_bexp = gc->n8pp;
    if(!strcmp(w,"_/))"))     ge->asc_bexp = gc->n16;
    if(!strcmp(w,"_/))."))    ge->asc_bexp = gc->n16p;
    if(!strcmp(w,"_/)).."))   ge->asc_bexp = gc->n16pp;
    if(!strcmp(w,"_/)))"))    ge->asc_bexp = gc->n32;
    if(!strcmp(w,"_/)))."))   ge->asc_bexp = gc->n32p;
    if(!strcmp(w,"_/))).."))  ge->asc_bexp = gc->n32pp;
    if(!strcmp(w,"_/))))"))   ge->asc_bexp = gc->n64;
    if(!strcmp(w,"_/))))."))  ge->asc_bexp = gc->n64p;
    if(!strcmp(w,"_/))))..")) ge->asc_bexp = gc->n64pp;
    if(!strcmp(w,"./)"))      ge->asc_bexp = gc->t8;
    if(!strcmp(w,"./)."))     ge->asc_bexp = gc->t8p;
    if(!strcmp(w,"./).."))    ge->asc_bexp = gc->t8pp;
    if(!strcmp(w,"./))"))     ge->asc_bexp = gc->t16;
    if(!strcmp(w,"./))."))    ge->asc_bexp = gc->t16p;
    if(!strcmp(w,"./)).."))   ge->asc_bexp = gc->t16pp;
    if(!strcmp(w,"./)))"))    ge->asc_bexp = gc->t32;
    if(!strcmp(w,"./)))."))   ge->asc_bexp = gc->t32p;
    if(!strcmp(w,"./))).."))  ge->asc_bexp = gc->t32pp;
    if(!strcmp(w,"./))))"))   ge->asc_bexp = gc->t64;
    if(!strcmp(w,"./))))."))  ge->asc_bexp = gc->t64p;
    if(!strcmp(w,"./))))..")) ge->asc_bexp = gc->t64pp;
    if(!strcmp(w,"./>"))      ge->asc_bexp = gc->a8;
    if(!strcmp(w,"./>."))     ge->asc_bexp = gc->a8p;
    if(!strcmp(w,"./>.."))    ge->asc_bexp = gc->a8pp;
    if(!strcmp(w,"./>>"))     ge->asc_bexp = gc->a16;
    if(!strcmp(w,"./>>."))    ge->asc_bexp = gc->a16p;
    if(!strcmp(w,"./>>.."))   ge->asc_bexp = gc->a16pp;
    if(!strcmp(w,"./>>>"))    ge->asc_bexp = gc->a32;
    if(!strcmp(w,"./>>>."))   ge->asc_bexp = gc->a32p;
    if(!strcmp(w,"./>>>.."))  ge->asc_bexp = gc->a32pp;
    if(!strcmp(w,"./>>>>"))   ge->asc_bexp = gc->a64;
    if(!strcmp(w,"./>>>>."))  ge->asc_bexp = gc->a64p;
    if(!strcmp(w,"./>>>>..")) ge->asc_bexp = gc->a64pp;
    /* Check set & modify flag */
    if(!strcmp(w,"|O|"))      gflag = gc->fww; 
    if(!strcmp(w,"O"))        gflag = gc->fw;
    if(!strcmp(w,"O."))       gflag = gc->fwp;
    if(!strcmp(w,"O.."))      gflag = gc->fwpp;
    if(!strcmp(w,"o/"))       gflag = gc->fh;
    if(!strcmp(w,"o/."))      gflag = gc->fhp;
    if(!strcmp(w,"o/.."))     gflag = gc->fhpp;
    if(!strcmp(w,"_/"))       gflag = gc->fq;
    if(!strcmp(w,"_/."))      gflag = gc->fqp;
    if(!strcmp(w,"_/.."))     gflag = gc->fqpp;
    if(!strcmp(w,"_/)"))      gflag = gc->fn8;
    if(!strcmp(w,"_/)."))     gflag = gc->fn8p;
    if(!strcmp(w,"_/).."))    gflag = gc->fn8pp;
    if(!strcmp(w,"_/))"))     gflag = gc->fn16;
    if(!strcmp(w,"_/))."))    gflag = gc->fn16p;
    if(!strcmp(w,"_/)).."))   gflag = gc->fn16pp;
    if(!strcmp(w,"_/)))"))    gflag = gc->fn32;
    if(!strcmp(w,"_/)))."))   gflag = gc->fn32p;
    if(!strcmp(w,"_/))).."))  gflag = gc->fn32pp;
    if(!strcmp(w,"_/))))"))   gflag = gc->fn64;
    if(!strcmp(w,"_/))))."))  gflag = gc->fn64p;
    if(!strcmp(w,"_/))))..")) gflag = gc->fn64pp;
    if(!strcmp(w,"./)"))      gflag = gc->ft8;
    if(!strcmp(w,"./)."))     gflag = gc->ft8p;
    if(!strcmp(w,"./).."))    gflag = gc->ft8pp;
    if(!strcmp(w,"./))"))     gflag = gc->ft16;
    if(!strcmp(w,"./))."))    gflag = gc->ft16p;
    if(!strcmp(w,"./)).."))   gflag = gc->ft16pp;
    if(!strcmp(w,"./)))"))    gflag = gc->ft32;
    if(!strcmp(w,"./)))."))   gflag = gc->ft32p;
    if(!strcmp(w,"./))).."))  gflag = gc->ft32pp;
    if(!strcmp(w,"./))))"))   gflag = gc->ft64;
    if(!strcmp(w,"./))))."))  gflag = gc->ft64p;
    if(!strcmp(w,"./))))..")) gflag = gc->ft64pp;
    if(!strcmp(w,"./>"))      gflag = gc->fa8;
    if(!strcmp(w,"./>."))     gflag = gc->fa8p;
    if(!strcmp(w,"./>.."))    gflag = gc->fa8pp;
    if(!strcmp(w,"./>>"))     gflag = gc->fa16;
    if(!strcmp(w,"./>>."))    gflag = gc->fa16p;
    if(!strcmp(w,"./>>.."))   gflag = gc->fa16pp;
    if(!strcmp(w,"./>>>"))    gflag = gc->fa32;
    if(!strcmp(w,"./>>>."))   gflag = gc->fa32p;
    if(!strcmp(w,"./>>>.."))  gflag = gc->fa32pp;
    if(!strcmp(w,"./>>>>"))   gflag = gc->fa64;
    if(!strcmp(w,"./>>>>."))  gflag = gc->fa64p;
    if(!strcmp(w,"./>>>>..")) gflag = gc->fa64pp;
    if(mv->debug == 13) printf("asc_gr_get() gflag=%d bexp=%d\n",
        gflag,ge->asc_bexp);
    if(gflag != ge->asc_bexp) return(1);
    return(0);
}

/* define gate_rate _/ = xx % */
int def_gate_rate(char *card)
//char *card;
{
    char w[12];
    char wc[80];
    int  i,j;
    int  val;
    int  af;

    af = 0;
    for(i = 0; i < 80; i++) {
	if(card[i] == 'O') af++;
	if(card[i] == '/') af++;
	if(card[i] == ')') af++;
	if(card[i] == '>') af++;
	if(card[i] == '.') af++;
        if(card[i] == '=') break;
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    if(!af) {
	/*
	i = 0;
        while(card[i] != '=') {
            i++;
            if(i > 80) return(12);
        }
	*/
	/* printf("card=%s\n",card); */
        ge->def_bexp = numdinmb_meb(card);
	return(-3);
    }
    i = 0;
    while(card[i] != '=') {
        i++;
        if(i > 80) return(12);
    }
    while(card[i] != 'e') {
        i--;
        if(i < 13) return(12);
    } 
    i++;
    while(card[i] == ' ') {
	i++;
        if(i > 80) return(14);
    }
    j = 0;
    while(card[i] != ' ') {
        w[j] = card[i];
        i++;
        j++;
        if(j > 10)  return(12);
    }
    w[j] = '\0';
    for(i = 0; i < 80; i++) {
        wc[i] = card[i];
        if(card[i] == '%') wc[i] = ' ';
        if(card[i] == '}') wc[i] = ' ';
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    wc[i] = '\0';
    val = numdinmb_meb(wc);
    /* printf("w=%s,val=%d\n",w,val); */
    if(!strcmp(w,"|O|"))      gd->ww = val; 
    if(!strcmp(w,"O"))        gd->w = val;
    if(!strcmp(w,"O."))       gd->wp = val;
    if(!strcmp(w,"O.."))      gd->wpp = val;
    if(!strcmp(w,"o/"))       gd->h = val;
    if(!strcmp(w,"o/."))      gd->hp = val;
    if(!strcmp(w,"o/.."))     gd->hpp = val;
    if(!strcmp(w,"_/"))       gd->q = val;
    if(!strcmp(w,"_/."))      gd->qp = val;
    if(!strcmp(w,"_/.."))     gd->qpp = val;
    if(!strcmp(w,"_/)"))      gd->n8 = val;
    if(!strcmp(w,"_/)."))     gd->n8p = val;
    if(!strcmp(w,"_/).."))    gd->n8pp = val;
    if(!strcmp(w,"_/))"))     gd->n16 = val;
    if(!strcmp(w,"_/))."))    gd->n16p = val;
    if(!strcmp(w,"_/)).."))   gd->n16pp = val;
    if(!strcmp(w,"_/)))"))    gd->n32 = val;
    if(!strcmp(w,"_/)))."))   gd->n32p = val;
    if(!strcmp(w,"_/))).."))  gd->n32pp = val;
    if(!strcmp(w,"_/))))"))   gd->n64 = val;
    if(!strcmp(w,"_/))))."))  gd->n64p = val;
    if(!strcmp(w,"_/))))..")) gd->n64pp = val;
    if(!strcmp(w,"./)"))      gd->t8 = val;
    if(!strcmp(w,"./)."))     gd->t8p = val;
    if(!strcmp(w,"./).."))    gd->t8pp = val;
    if(!strcmp(w,"./))"))     gd->t16 = val;
    if(!strcmp(w,"./))."))    gd->t16p = val;
    if(!strcmp(w,"./)).."))   gd->t16pp = val;
    if(!strcmp(w,"./)))"))    gd->t32 = val;
    if(!strcmp(w,"./)))."))   gd->t32p = val;
    if(!strcmp(w,"./))).."))  gd->t32pp = val;
    if(!strcmp(w,"./))))"))   gd->t64 = val;
    if(!strcmp(w,"./))))."))  gd->t64p = val;
    if(!strcmp(w,"./))))..")) gd->t64pp = val;
    if(!strcmp(w,"./>"))      gd->a8 = val;
    if(!strcmp(w,"./>."))     gd->a8p = val;
    if(!strcmp(w,"./>.."))    gd->a8pp = val;
    if(!strcmp(w,"./>>"))     gd->a16 = val;
    if(!strcmp(w,"./>>."))    gd->a16p = val;
    if(!strcmp(w,"./>>.."))   gd->a16pp = val;
    if(!strcmp(w,"./>>>"))    gd->a32 = val;
    if(!strcmp(w,"./>>>."))   gd->a32p = val;
    if(!strcmp(w,"./>>>.."))  gd->a32pp = val;
    if(!strcmp(w,"./>>>>"))   gd->a64 = val;
    if(!strcmp(w,"./>>>>."))  gd->a64p = val;
    if(!strcmp(w,"./>>>>..")) gd->a64pp = val;
    /* set & modify Gate_rate flag on */
    if(!strcmp(w,"|O|"))      gd->fww = val;
    if(!strcmp(w,"O"))        gd->fw = val;
    if(!strcmp(w,"O."))       gd->fwp = val;
    if(!strcmp(w,"O.."))      gd->fwpp = val;
    if(!strcmp(w,"o/"))       gd->fh = val;
    if(!strcmp(w,"o/."))      gd->fhp = val;
    if(!strcmp(w,"o/.."))     gd->fhpp = val;
    if(!strcmp(w,"_/"))       gd->fq = val;
    if(!strcmp(w,"_/."))      gd->fqp = val;
    if(!strcmp(w,"_/.."))     gd->fqpp = val;
    if(!strcmp(w,"_/)"))      gd->fn8 = val;
    if(!strcmp(w,"_/)."))     gd->fn8p = val;
    if(!strcmp(w,"_/).."))    gd->fn8pp = val;
    if(!strcmp(w,"_/))"))     gd->fn16 = val;
    if(!strcmp(w,"_/))."))    gd->fn16p = val;
    if(!strcmp(w,"_/)).."))   gd->fn16pp = val;
    if(!strcmp(w,"_/)))"))    gd->fn32 = val;
    if(!strcmp(w,"_/)))."))   gd->fn32p = val;
    if(!strcmp(w,"_/))).."))  gd->fn32pp = val;
    if(!strcmp(w,"_/))))"))   gd->fn64 = val;
    if(!strcmp(w,"_/))))."))  gd->fn64p = val;
    if(!strcmp(w,"_/))))..")) gd->fn64pp = val;
    if(!strcmp(w,"./)"))      gd->ft8 = val;
    if(!strcmp(w,"./)."))     gd->ft8p = val;
    if(!strcmp(w,"./).."))    gd->ft8pp = val;
    if(!strcmp(w,"./))"))     gd->ft16 = val;
    if(!strcmp(w,"./))."))    gd->ft16p = val;
    if(!strcmp(w,"./)).."))   gd->ft16pp = val;
    if(!strcmp(w,"./)))"))    gd->ft32 = val;
    if(!strcmp(w,"./)))."))   gd->ft32p = val;
    if(!strcmp(w,"./))).."))  gd->ft32pp = val;
    if(!strcmp(w,"./))))"))   gd->ft64 = val;
    if(!strcmp(w,"./))))."))  gd->ft64p = val;
    if(!strcmp(w,"./))))..")) gd->ft64pp = val;
    if(!strcmp(w,"./>"))      gd->fa8 = val;
    if(!strcmp(w,"./>."))     gd->fa8p = val;
    if(!strcmp(w,"./>.."))    gd->fa8pp = val;
    if(!strcmp(w,"./>>"))     gd->fa16 = val;
    if(!strcmp(w,"./>>."))    gd->fa16p = val;
    if(!strcmp(w,"./>>.."))   gd->fa16pp = val;
    if(!strcmp(w,"./>>>"))    gd->fa32 = val;
    if(!strcmp(w,"./>>>."))   gd->fa32p = val;
    if(!strcmp(w,"./>>>.."))  gd->fa32pp = val;
    if(!strcmp(w,"./>>>>"))   gd->fa64 = val;
    if(!strcmp(w,"./>>>>."))  gd->fa64p = val;
    if(!strcmp(w,"./>>>>..")) gd->fa64pp = val;
    return(0);
}

/* assign gate_rate _/ = xx % */
int asn_gate_rate(char *card)
//char *card;
{
    char w[12];
    char wc[80];
    int  i,j;
    int  val;
    int  af;

    af = 0;
    for(i = 0; i < 80; i++) {
	if(card[i] == 'O') af++;
	if(card[i] == '/') af++;
	if(card[i] == ')') af++;
	if(card[i] == '>') af++;
	if(card[i] == '.') af++;
        if(card[i] == '=') break;
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    if(!af) {
	/*
	i = 0;
        while(card[i] != '=') {
            i++;
            if(i > 80) return(12);
        }
	*/
	/* printf("card=%s\n",card); */
        ge->asn_bexp = numdinmb_meb(card);
	return(-3);
    }
    i = 0;
    while(card[i] != '=') {
        i++;
        if(i > 80) return(12);
    }
    while(card[i] != 'e') {
        i--;
        if(i < 13) return(12);
    } 
    i++;
    while(card[i] == ' ') {
	i++;
        if(i > 80) return(14);
    }
    j = 0;
    while(card[i] != ' ') {
        w[j] = card[i];
        i++;
        j++;
        if(j > 10)  return(12);
    }
    w[j] = '\0';
    for(i = 0; i < 80; i++) {
        wc[i] = card[i];
        if(card[i] == '%') wc[i] = ' ';
        if(card[i] == '}') wc[i] = ' ';
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    wc[i] = '\0';
    val = numdinmb_meb(wc);
    /* printf("w=%s,val=%d\n",w,val); */
    if(!strcmp(w,"|O|"))      ga->ww = val; 
    if(!strcmp(w,"O"))        ga->w = val;
    if(!strcmp(w,"O."))       ga->wp = val;
    if(!strcmp(w,"O.."))      ga->wpp = val;
    if(!strcmp(w,"o/"))       ga->h = val;
    if(!strcmp(w,"o/."))      ga->hp = val;
    if(!strcmp(w,"o/.."))     ga->hpp = val;
    if(!strcmp(w,"_/"))       ga->q = val;
    if(!strcmp(w,"_/."))      ga->qp = val;
    if(!strcmp(w,"_/.."))     ga->qpp = val;
    if(!strcmp(w,"_/)"))      ga->n8 = val;
    if(!strcmp(w,"_/)."))     ga->n8p = val;
    if(!strcmp(w,"_/).."))    ga->n8pp = val;
    if(!strcmp(w,"_/))"))     ga->n16 = val;
    if(!strcmp(w,"_/))."))    ga->n16p = val;
    if(!strcmp(w,"_/)).."))   ga->n16pp = val;
    if(!strcmp(w,"_/)))"))    ga->n32 = val;
    if(!strcmp(w,"_/)))."))   ga->n32p = val;
    if(!strcmp(w,"_/))).."))  ga->n32pp = val;
    if(!strcmp(w,"_/))))"))   ga->n64 = val;
    if(!strcmp(w,"_/))))."))  ga->n64p = val;
    if(!strcmp(w,"_/))))..")) ga->n64pp = val;
    if(!strcmp(w,"./)"))      ga->t8 = val;
    if(!strcmp(w,"./)."))     ga->t8p = val;
    if(!strcmp(w,"./).."))    ga->t8pp = val;
    if(!strcmp(w,"./))"))     ga->t16 = val;
    if(!strcmp(w,"./))."))    ga->t16p = val;
    if(!strcmp(w,"./)).."))   ga->t16pp = val;
    if(!strcmp(w,"./)))"))    ga->t32 = val;
    if(!strcmp(w,"./)))."))   ga->t32p = val;
    if(!strcmp(w,"./))).."))  ga->t32pp = val;
    if(!strcmp(w,"./))))"))   ga->t64 = val;
    if(!strcmp(w,"./))))."))  ga->t64p = val;
    if(!strcmp(w,"./))))..")) ga->t64pp = val;
    if(!strcmp(w,"./>"))      ga->a8 = val;
    if(!strcmp(w,"./>."))     ga->a8p = val;
    if(!strcmp(w,"./>.."))    ga->a8pp = val;
    if(!strcmp(w,"./>>"))     ga->a16 = val;
    if(!strcmp(w,"./>>."))    ga->a16p = val;
    if(!strcmp(w,"./>>.."))   ga->a16pp = val;
    if(!strcmp(w,"./>>>"))    ga->a32 = val;
    if(!strcmp(w,"./>>>."))   ga->a32p = val;
    if(!strcmp(w,"./>>>.."))  ga->a32pp = val;
    if(!strcmp(w,"./>>>>"))   ga->a64 = val;
    if(!strcmp(w,"./>>>>."))  ga->a64p = val;
    if(!strcmp(w,"./>>>>..")) ga->a64pp = val;
    /* set & modify Gate_rate flag on */
    if(!strcmp(w,"|O|"))      ga->fww = val;
    if(!strcmp(w,"O"))        ga->fw = val;
    if(!strcmp(w,"O."))       ga->fwp = val;
    if(!strcmp(w,"O.."))      ga->fwpp = val;
    if(!strcmp(w,"o/"))       ga->fh = val;
    if(!strcmp(w,"o/."))      ga->fhp = val;
    if(!strcmp(w,"o/.."))     ga->fhpp = val;
    if(!strcmp(w,"_/"))       ga->fq = val;
    if(!strcmp(w,"_/."))      ga->fqp = val;
    if(!strcmp(w,"_/.."))     ga->fqpp = val;
    if(!strcmp(w,"_/)"))      ga->fn8 = val;
    if(!strcmp(w,"_/)."))     ga->fn8p = val;
    if(!strcmp(w,"_/).."))    ga->fn8pp = val;
    if(!strcmp(w,"_/))"))     ga->fn16 = val;
    if(!strcmp(w,"_/))."))    ga->fn16p = val;
    if(!strcmp(w,"_/)).."))   ga->fn16pp = val;
    if(!strcmp(w,"_/)))"))    ga->fn32 = val;
    if(!strcmp(w,"_/)))."))   ga->fn32p = val;
    if(!strcmp(w,"_/))).."))  ga->fn32pp = val;
    if(!strcmp(w,"_/))))"))   ga->fn64 = val;
    if(!strcmp(w,"_/))))."))  ga->fn64p = val;
    if(!strcmp(w,"_/))))..")) ga->fn64pp = val;
    if(!strcmp(w,"./)"))      ga->ft8 = val;
    if(!strcmp(w,"./)."))     ga->ft8p = val;
    if(!strcmp(w,"./).."))    ga->ft8pp = val;
    if(!strcmp(w,"./))"))     ga->ft16 = val;
    if(!strcmp(w,"./))."))    ga->ft16p = val;
    if(!strcmp(w,"./)).."))   ga->ft16pp = val;
    if(!strcmp(w,"./)))"))    ga->ft32 = val;
    if(!strcmp(w,"./)))."))   ga->ft32p = val;
    if(!strcmp(w,"./))).."))  ga->ft32pp = val;
    if(!strcmp(w,"./))))"))   ga->ft64 = val;
    if(!strcmp(w,"./))))."))  ga->ft64p = val;
    if(!strcmp(w,"./))))..")) ga->ft64pp = val;
    if(!strcmp(w,"./>"))      ga->fa8 = val;
    if(!strcmp(w,"./>."))     ga->fa8p = val;
    if(!strcmp(w,"./>.."))    ga->fa8pp = val;
    if(!strcmp(w,"./>>"))     ga->fa16 = val;
    if(!strcmp(w,"./>>."))    ga->fa16p = val;
    if(!strcmp(w,"./>>.."))   ga->fa16pp = val;
    if(!strcmp(w,"./>>>"))    ga->fa32 = val;
    if(!strcmp(w,"./>>>."))   ga->fa32p = val;
    if(!strcmp(w,"./>>>.."))  ga->fa32pp = val;
    if(!strcmp(w,"./>>>>"))   ga->fa64 = val;
    if(!strcmp(w,"./>>>>."))  ga->fa64p = val;
    if(!strcmp(w,"./>>>>..")) ga->fa64pp = val;
    return(0);
}

/* modify gate_rate _/ = xx % */
int mod_gate_rate(char *card)
//char *card;
{
    char w[12];
    char wc[80];
    int  i,j;
    int  val;
    int  af;

    af = 0;
    for(i = 0; i < 80; i++) {
	if(card[i] == 'O') af++;
	if(card[i] == '/') af++;
	if(card[i] == ')') af++;
	if(card[i] == '>') af++;
	if(card[i] == '.') af++;
        if(card[i] == '=') break;
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    if(!af) {
	/*
	i = 0;
        while(card[i] != '=') {
            i++;
            if(i > 80) return(12);
        }
	*/
	/* printf("card=%s\n",card); */
        ge->mod_bexp = numdinmb_meb(card);
	return(-3);
    }
    i = 0;
    while(card[i] != '=') {
        i++;
        if(i > 80) return(12);
    }
    while(card[i] != 'e') {
        i--;
        if(i < 13) return(12);
    } 
    i++;
    while(card[i] == ' ') {
	i++;
        if(i > 80) return(14);
    }
    j = 0;
    while(card[i] != ' ') {
        w[j] = card[i];
        i++;
        j++;
        if(j > 10)  return(12);
    }
    w[j] = '\0';
    for(i = 0; i < 80; i++) {
        wc[i] = card[i];
        if(card[i] == '%') wc[i] = ' ';
        if(card[i] == '}') wc[i] = ' ';
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    wc[i] = '\0';
    val = numdinmb_meb(wc);
    /* printf("w=%s,val=%d\n",w,val); */
    if(!strcmp(w,"|O|"))      gm->ww = val; 
    if(!strcmp(w,"O"))        gm->w = val;
    if(!strcmp(w,"O."))       gm->wp = val;
    if(!strcmp(w,"O.."))      gm->wpp = val;
    if(!strcmp(w,"o/"))       gm->h = val;
    if(!strcmp(w,"o/."))      gm->hp = val;
    if(!strcmp(w,"o/.."))     gm->hpp = val;
    if(!strcmp(w,"_/"))       gm->q = val;
    if(!strcmp(w,"_/."))      gm->qp = val;
    if(!strcmp(w,"_/.."))     gm->qpp = val;
    if(!strcmp(w,"_/)"))      gm->n8 = val;
    if(!strcmp(w,"_/)."))     gm->n8p = val;
    if(!strcmp(w,"_/).."))    gm->n8pp = val;
    if(!strcmp(w,"_/))"))     gm->n16 = val;
    if(!strcmp(w,"_/))."))    gm->n16p = val;
    if(!strcmp(w,"_/)).."))   gm->n16pp = val;
    if(!strcmp(w,"_/)))"))    gm->n32 = val;
    if(!strcmp(w,"_/)))."))   gm->n32p = val;
    if(!strcmp(w,"_/))).."))  gm->n32pp = val;
    if(!strcmp(w,"_/))))"))   gm->n64 = val;
    if(!strcmp(w,"_/))))."))  gm->n64p = val;
    if(!strcmp(w,"_/))))..")) gm->n64pp = val;
    if(!strcmp(w,"./)"))      gm->t8 = val;
    if(!strcmp(w,"./)."))     gm->t8p = val;
    if(!strcmp(w,"./).."))    gm->t8pp = val;
    if(!strcmp(w,"./))"))     gm->t16 = val;
    if(!strcmp(w,"./))."))    gm->t16p = val;
    if(!strcmp(w,"./)).."))   gm->t16pp = val;
    if(!strcmp(w,"./)))"))    gm->t32 = val;
    if(!strcmp(w,"./)))."))   gm->t32p = val;
    if(!strcmp(w,"./))).."))  gm->t32pp = val;
    if(!strcmp(w,"./))))"))   gm->t64 = val;
    if(!strcmp(w,"./))))."))  gm->t64p = val;
    if(!strcmp(w,"./))))..")) gm->t64pp = val;
    if(!strcmp(w,"./>"))      gm->a8 = val;
    if(!strcmp(w,"./>."))     gm->a8p = val;
    if(!strcmp(w,"./>.."))    gm->a8pp = val;
    if(!strcmp(w,"./>>"))     gm->a16 = val;
    if(!strcmp(w,"./>>."))    gm->a16p = val;
    if(!strcmp(w,"./>>.."))   gm->a16pp = val;
    if(!strcmp(w,"./>>>"))    gm->a32 = val;
    if(!strcmp(w,"./>>>."))   gm->a32p = val;
    if(!strcmp(w,"./>>>.."))  gm->a32pp = val;
    if(!strcmp(w,"./>>>>"))   gm->a64 = val;
    if(!strcmp(w,"./>>>>."))  gm->a64p = val;
    if(!strcmp(w,"./>>>>..")) gm->a64pp = val;
    /* set & modify Gate_rate flag on */
    if(!strcmp(w,"|O|"))      gm->fww = val;
    if(!strcmp(w,"O"))        gm->fw = val;
    if(!strcmp(w,"O."))       gm->fwp = val;
    if(!strcmp(w,"O.."))      gm->fwpp = val;
    if(!strcmp(w,"o/"))       gm->fh = val;
    if(!strcmp(w,"o/."))      gm->fhp = val;
    if(!strcmp(w,"o/.."))     gm->fhpp = val;
    if(!strcmp(w,"_/"))       gm->fq = val;
    if(!strcmp(w,"_/."))      gm->fqp = val;
    if(!strcmp(w,"_/.."))     gm->fqpp = val;
    if(!strcmp(w,"_/)"))      gm->fn8 = val;
    if(!strcmp(w,"_/)."))     gm->fn8p = val;
    if(!strcmp(w,"_/).."))    gm->fn8pp = val;
    if(!strcmp(w,"_/))"))     gm->fn16 = val;
    if(!strcmp(w,"_/))."))    gm->fn16p = val;
    if(!strcmp(w,"_/)).."))   gm->fn16pp = val;
    if(!strcmp(w,"_/)))"))    gm->fn32 = val;
    if(!strcmp(w,"_/)))."))   gm->fn32p = val;
    if(!strcmp(w,"_/))).."))  gm->fn32pp = val;
    if(!strcmp(w,"_/))))"))   gm->fn64 = val;
    if(!strcmp(w,"_/))))."))  gm->fn64p = val;
    if(!strcmp(w,"_/))))..")) gm->fn64pp = val;
    if(!strcmp(w,"./)"))      gm->ft8 = val;
    if(!strcmp(w,"./)."))     gm->ft8p = val;
    if(!strcmp(w,"./).."))    gm->ft8pp = val;
    if(!strcmp(w,"./))"))     gm->ft16 = val;
    if(!strcmp(w,"./))."))    gm->ft16p = val;
    if(!strcmp(w,"./)).."))   gm->ft16pp = val;
    if(!strcmp(w,"./)))"))    gm->ft32 = val;
    if(!strcmp(w,"./)))."))   gm->ft32p = val;
    if(!strcmp(w,"./))).."))  gm->ft32pp = val;
    if(!strcmp(w,"./))))"))   gm->ft64 = val;
    if(!strcmp(w,"./))))."))  gm->ft64p = val;
    if(!strcmp(w,"./))))..")) gm->ft64pp = val;
    if(!strcmp(w,"./>"))      gm->fa8 = val;
    if(!strcmp(w,"./>."))     gm->fa8p = val;
    if(!strcmp(w,"./>.."))    gm->fa8pp = val;
    if(!strcmp(w,"./>>"))     gm->fa16 = val;
    if(!strcmp(w,"./>>."))    gm->fa16p = val;
    if(!strcmp(w,"./>>.."))   gm->fa16pp = val;
    if(!strcmp(w,"./>>>"))    gm->fa32 = val;
    if(!strcmp(w,"./>>>."))   gm->fa32p = val;
    if(!strcmp(w,"./>>>.."))  gm->fa32pp = val;
    if(!strcmp(w,"./>>>>"))   gm->fa64 = val;
    if(!strcmp(w,"./>>>>."))  gm->fa64p = val;
    if(!strcmp(w,"./>>>>..")) gm->fa64pp = val;
    return(0);
}

/* set gate_rate _/ = xx % */
int set_gate_rate(char *card)
//char *card;
{
    char w[12];
    char wc[80];
    int  i,j;
    int  val;
    int  af;

    af = 0;
    for(i = 0; i < 80; i++) {
	if(card[i] == 'O') af++;
	if(card[i] == '/') af++;
	if(card[i] == ')') af++;
	if(card[i] == '>') af++;
	if(card[i] == '.') af++;
        if(card[i] == '=') break;
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    if(!af) {
	/*
	i = 0;
        while(card[i] != '=') {
            i++;
            if(i > 80) return(13);
        }
	*/
	/* printf("card=%s\n",card); */
        ge->set_bexp = numdinmb_meb(card);
	return(-3);
    }
    i = 0;
    while(card[i] != '=') {
        i++;
        if(i > 80) return(13);
    }
    while(card[i] != 'e') {
        i--;
        if(i < 13) return(13);
    } 
    i++;
    while(card[i] == ' ') {
	i++;
        if(i > 80) return(14);
    }
    j = 0;
    while(card[i] != ' ') {
        w[j] = card[i];
        i++;
        j++;
        if(j > 10)  return(13);
    }
    w[j] = '\0';
    for(i = 0; i < 80; i++) {
        wc[i] = card[i];
        if(card[i] == '%') wc[i] = ' ';
        if(card[i] == '}') wc[i] = ' ';
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    wc[i] = '\0';
    val = numdinmb_meb(wc);
    /* printf("w=%s,val=%d\n",w,val); */
    if(!strcmp(w,"|O|"))      gs->ww = val; 
    if(!strcmp(w,"O"))        gs->w = val;
    if(!strcmp(w,"O."))       gs->wp = val;
    if(!strcmp(w,"O.."))      gs->wpp = val;
    if(!strcmp(w,"o/"))       gs->h = val;
    if(!strcmp(w,"o/."))      gs->hp = val;
    if(!strcmp(w,"o/.."))     gs->hpp = val;
    if(!strcmp(w,"_/"))       gs->q = val;
    if(!strcmp(w,"_/."))      gs->qp = val;
    if(!strcmp(w,"_/.."))     gs->qpp = val;
    if(!strcmp(w,"_/)"))      gs->n8 = val;
    if(!strcmp(w,"_/)."))     gs->n8p = val;
    if(!strcmp(w,"_/).."))    gs->n8pp = val;
    if(!strcmp(w,"_/))"))     gs->n16 = val;
    if(!strcmp(w,"_/))."))    gs->n16p = val;
    if(!strcmp(w,"_/)).."))   gs->n16pp = val;
    if(!strcmp(w,"_/)))"))    gs->n32 = val;
    if(!strcmp(w,"_/)))."))   gs->n32p = val;
    if(!strcmp(w,"_/))).."))  gs->n32pp = val;
    if(!strcmp(w,"_/))))"))   gs->n64 = val;
    if(!strcmp(w,"_/))))."))  gs->n64p = val;
    if(!strcmp(w,"_/))))..")) gs->n64pp = val;
    if(!strcmp(w,"./)"))      gs->t8 = val;
    if(!strcmp(w,"./)."))     gs->t8p = val;
    if(!strcmp(w,"./).."))    gs->t8pp = val;
    if(!strcmp(w,"./))"))     gs->t16 = val;
    if(!strcmp(w,"./))."))    gs->t16p = val;
    if(!strcmp(w,"./)).."))   gs->t16pp = val;
    if(!strcmp(w,"./)))"))    gs->t32 = val;
    if(!strcmp(w,"./)))."))   gs->t32p = val;
    if(!strcmp(w,"./))).."))  gs->t32pp = val;
    if(!strcmp(w,"./))))"))   gs->t64 = val;
    if(!strcmp(w,"./))))."))  gs->t64p = val;
    if(!strcmp(w,"./))))..")) gs->t64pp = val;
    if(!strcmp(w,"./>"))      gs->a8 = val;
    if(!strcmp(w,"./>."))     gs->a8p = val;
    if(!strcmp(w,"./>.."))    gs->a8pp = val;
    if(!strcmp(w,"./>>"))     gs->a16 = val;
    if(!strcmp(w,"./>>."))    gs->a16p = val;
    if(!strcmp(w,"./>>.."))   gs->a16pp = val;
    if(!strcmp(w,"./>>>"))    gs->a32 = val;
    if(!strcmp(w,"./>>>."))   gs->a32p = val;
    if(!strcmp(w,"./>>>.."))  gs->a32pp = val;
    if(!strcmp(w,"./>>>>"))   gs->a64 = val;
    if(!strcmp(w,"./>>>>."))  gs->a64p = val;
    if(!strcmp(w,"./>>>>..")) gs->a64pp = val;
    /* set & modify Gate_rate flag on */
    if(!strcmp(w,"|O|"))      gs->fww = val;
    if(!strcmp(w,"O"))        gs->fw = val;
    if(!strcmp(w,"O."))       gs->fwp = val;
    if(!strcmp(w,"O.."))      gs->fwpp = val;
    if(!strcmp(w,"o/"))       gs->fh = val;
    if(!strcmp(w,"o/."))      gs->fhp = val;
    if(!strcmp(w,"o/.."))     gs->fhpp = val;
    if(!strcmp(w,"_/"))       gs->fq = val;
    if(!strcmp(w,"_/."))      gs->fqp = val;
    if(!strcmp(w,"_/.."))     gs->fqpp = val;
    if(!strcmp(w,"_/)"))      gs->fn8 = val;
    if(!strcmp(w,"_/)."))     gs->fn8p = val;
    if(!strcmp(w,"_/).."))    gs->fn8pp = val;
    if(!strcmp(w,"_/))"))     gs->fn16 = val;
    if(!strcmp(w,"_/))."))    gs->fn16p = val;
    if(!strcmp(w,"_/)).."))   gs->fn16pp = val;
    if(!strcmp(w,"_/)))"))    gs->fn32 = val;
    if(!strcmp(w,"_/)))."))   gs->fn32p = val;
    if(!strcmp(w,"_/))).."))  gs->fn32pp = val;
    if(!strcmp(w,"_/))))"))   gs->fn64 = val;
    if(!strcmp(w,"_/))))."))  gs->fn64p = val;
    if(!strcmp(w,"_/))))..")) gs->fn64pp = val;
    if(!strcmp(w,"./)"))      gs->ft8 = val;
    if(!strcmp(w,"./)."))     gs->ft8p = val;
    if(!strcmp(w,"./).."))    gs->ft8pp = val;
    if(!strcmp(w,"./))"))     gs->ft16 = val;
    if(!strcmp(w,"./))."))    gs->ft16p = val;
    if(!strcmp(w,"./)).."))   gs->ft16pp = val;
    if(!strcmp(w,"./)))"))    gs->ft32 = val;
    if(!strcmp(w,"./)))."))   gs->ft32p = val;
    if(!strcmp(w,"./))).."))  gs->ft32pp = val;
    if(!strcmp(w,"./))))"))   gs->ft64 = val;
    if(!strcmp(w,"./))))."))  gs->ft64p = val;
    if(!strcmp(w,"./))))..")) gs->ft64pp = val;
    if(!strcmp(w,"./>"))      gs->fa8 = val;
    if(!strcmp(w,"./>."))     gs->fa8p = val;
    if(!strcmp(w,"./>.."))    gs->fa8pp = val;
    if(!strcmp(w,"./>>"))     gs->fa16 = val;
    if(!strcmp(w,"./>>."))    gs->fa16p = val;
    if(!strcmp(w,"./>>.."))   gs->fa16pp = val;
    if(!strcmp(w,"./>>>"))    gs->fa32 = val;
    if(!strcmp(w,"./>>>."))   gs->fa32p = val;
    if(!strcmp(w,"./>>>.."))  gs->fa32pp = val;
    if(!strcmp(w,"./>>>>"))   gs->fa64 = val;
    if(!strcmp(w,"./>>>>."))  gs->fa64p = val;
    if(!strcmp(w,"./>>>>..")) gs->fa64pp = val;
    return(0);
}

/* add/sub/clear gate_rate _/ = xx % */
int asc_gate_rate(char *card)
//char *card;
{
    char w[12];
    char wc[80];
    int  i,j;
    int  val;
    int  af;
    int  dgf;

    af = 0;
    for(i = 0; i < 80; i++) {
	if(card[i] == 'O') af++;
	if(card[i] == '/') af++;
	if(card[i] == ')') af++;
	if(card[i] == '>') af++;
	if(card[i] == '.') af++;
        if(card[i] == '=') break;
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    if(!af) {
	i = 0;
        dgf = 0;
        while(card[i] != '\0') {
            if(isdigit(card[i])) dgf = 1;
            if(card[i] == '\r') break;
            if(card[i] == '\n') break;
            i++;
            if(i > 80) return(14);
        }
	/* printf("card=%s\n",card); */
        if(dgf) {
            ge->asc_bexp = numdinmb_meb(card);
	    return(-3);
        }
        else return(0);
    }
    i = 0;
    while(card[i] != '=') {
        i++;
        if(i > 80) return(14);
    }
    while(card[i] != 'e') {
        i--;
        if(i < 13) return(14);
    } 
    i++;
    while(card[i] == ' ') {
	i++;
        if(i > 80) return(14);
    }
    j = 0;
    while(card[i] != ' ') {
        w[j] = card[i];
        i++;
        j++;
        if(j > 10)  return(14);
    }
    w[j] = '\0';
    for(i = 0; i < 80; i++) {
        wc[i] = card[i];
        if(card[i] == '%') wc[i] = ' ';
        if(card[i] == '}') wc[i] = ' ';
        if(card[i] == '\0') break;
        if(card[i] == '\r') break;
        if(card[i] == '\n') break;
    }
    wc[i] = '\0';
    val = numdinmb_meb(wc);
    /* printf("w=%s,val=%d\n",w,val); */
    if(!strcmp(w,"|O|"))      gc->ww = val; 
    if(!strcmp(w,"O"))        gc->w = val;
    if(!strcmp(w,"O."))       gc->wp = val;
    if(!strcmp(w,"O.."))      gc->wpp = val;
    if(!strcmp(w,"o/"))       gc->h = val;
    if(!strcmp(w,"o/."))      gc->hp = val;
    if(!strcmp(w,"o/.."))     gc->hpp = val;
    if(!strcmp(w,"_/"))       gc->q = val;
    if(!strcmp(w,"_/."))      gc->qp = val;
    if(!strcmp(w,"_/.."))     gc->qpp = val;
    if(!strcmp(w,"_/)"))      gc->n8 = val;
    if(!strcmp(w,"_/)."))     gc->n8p = val;
    if(!strcmp(w,"_/).."))    gc->n8pp = val;
    if(!strcmp(w,"_/))"))     gc->n16 = val;
    if(!strcmp(w,"_/))."))    gc->n16p = val;
    if(!strcmp(w,"_/)).."))   gc->n16pp = val;
    if(!strcmp(w,"_/)))"))    gc->n32 = val;
    if(!strcmp(w,"_/)))."))   gc->n32p = val;
    if(!strcmp(w,"_/))).."))  gc->n32pp = val;
    if(!strcmp(w,"_/))))"))   gc->n64 = val;
    if(!strcmp(w,"_/))))."))  gc->n64p = val;
    if(!strcmp(w,"_/))))..")) gc->n64pp = val;
    if(!strcmp(w,"./)"))      gc->t8 = val;
    if(!strcmp(w,"./)."))     gc->t8p = val;
    if(!strcmp(w,"./).."))    gc->t8pp = val;
    if(!strcmp(w,"./))"))     gc->t16 = val;
    if(!strcmp(w,"./))."))    gc->t16p = val;
    if(!strcmp(w,"./)).."))   gc->t16pp = val;
    if(!strcmp(w,"./)))"))    gc->t32 = val;
    if(!strcmp(w,"./)))."))   gc->t32p = val;
    if(!strcmp(w,"./))).."))  gc->t32pp = val;
    if(!strcmp(w,"./))))"))   gc->t64 = val;
    if(!strcmp(w,"./))))."))  gc->t64p = val;
    if(!strcmp(w,"./))))..")) gc->t64pp = val;
    if(!strcmp(w,"./>"))      gc->a8 = val;
    if(!strcmp(w,"./>."))     gc->a8p = val;
    if(!strcmp(w,"./>.."))    gc->a8pp = val;
    if(!strcmp(w,"./>>"))     gc->a16 = val;
    if(!strcmp(w,"./>>."))    gc->a16p = val;
    if(!strcmp(w,"./>>.."))   gc->a16pp = val;
    if(!strcmp(w,"./>>>"))    gc->a32 = val;
    if(!strcmp(w,"./>>>."))   gc->a32p = val;
    if(!strcmp(w,"./>>>.."))  gc->a32pp = val;
    if(!strcmp(w,"./>>>>"))   gc->a64 = val;
    if(!strcmp(w,"./>>>>."))  gc->a64p = val;
    if(!strcmp(w,"./>>>>..")) gc->a64pp = val;
    /* set & modify Gate_rate flag on */
    if(!strcmp(w,"|O|"))      gc->fww = val;
    if(!strcmp(w,"O"))        gc->fw = val;
    if(!strcmp(w,"O."))       gc->fwp = val;
    if(!strcmp(w,"O.."))      gc->fwpp = val;
    if(!strcmp(w,"o/"))       gc->fh = val;
    if(!strcmp(w,"o/."))      gc->fhp = val;
    if(!strcmp(w,"o/.."))     gc->fhpp = val;
    if(!strcmp(w,"_/"))       gc->fq = val;
    if(!strcmp(w,"_/."))      gc->fqp = val;
    if(!strcmp(w,"_/.."))     gc->fqpp = val;
    if(!strcmp(w,"_/)"))      gc->fn8 = val;
    if(!strcmp(w,"_/)."))     gc->fn8p = val;
    if(!strcmp(w,"_/).."))    gc->fn8pp = val;
    if(!strcmp(w,"_/))"))     gc->fn16 = val;
    if(!strcmp(w,"_/))."))    gc->fn16p = val;
    if(!strcmp(w,"_/)).."))   gc->fn16pp = val;
    if(!strcmp(w,"_/)))"))    gc->fn32 = val;
    if(!strcmp(w,"_/)))."))   gc->fn32p = val;
    if(!strcmp(w,"_/))).."))  gc->fn32pp = val;
    if(!strcmp(w,"_/))))"))   gc->fn64 = val;
    if(!strcmp(w,"_/))))."))  gc->fn64p = val;
    if(!strcmp(w,"_/))))..")) gc->fn64pp = val;
    if(!strcmp(w,"./)"))      gc->ft8 = val;
    if(!strcmp(w,"./)."))     gc->ft8p = val;
    if(!strcmp(w,"./).."))    gc->ft8pp = val;
    if(!strcmp(w,"./))"))     gc->ft16 = val;
    if(!strcmp(w,"./))."))    gc->ft16p = val;
    if(!strcmp(w,"./)).."))   gc->ft16pp = val;
    if(!strcmp(w,"./)))"))    gc->ft32 = val;
    if(!strcmp(w,"./)))."))   gc->ft32p = val;
    if(!strcmp(w,"./))).."))  gc->ft32pp = val;
    if(!strcmp(w,"./))))"))   gc->ft64 = val;
    if(!strcmp(w,"./))))."))  gc->ft64p = val;
    if(!strcmp(w,"./))))..")) gc->ft64pp = val;
    if(!strcmp(w,"./>"))      gc->fa8 = val;
    if(!strcmp(w,"./>."))     gc->fa8p = val;
    if(!strcmp(w,"./>.."))    gc->fa8pp = val;
    if(!strcmp(w,"./>>"))     gc->fa16 = val;
    if(!strcmp(w,"./>>."))    gc->fa16p = val;
    if(!strcmp(w,"./>>.."))   gc->fa16pp = val;
    if(!strcmp(w,"./>>>"))    gc->fa32 = val;
    if(!strcmp(w,"./>>>."))   gc->fa32p = val;
    if(!strcmp(w,"./>>>.."))  gc->fa32pp = val;
    if(!strcmp(w,"./>>>>"))   gc->fa64 = val;
    if(!strcmp(w,"./>>>>."))  gc->fa64p = val;
    if(!strcmp(w,"./>>>>..")) gc->fa64pp = val;
    return(0);
}

/*
  chek & note shift
  1-9        velocity on
  Transpose  velocity on
  0123456789#&$*@:
  ABCDEFGHIJ]}/'!; :  >   
  abcdefghij[{%"~, :  <
*/
int tncheck_men(int osp,int tpv)
//int  osp;
//int  tpv;
{
    int  i;
    char c;
    char h;
    int  pas;

    pas = 0;
    /* printf("%s\n",mv->card); */
    for(i = osp; i <= 68; i++) {
        c = tolower(mv->card[i]);
        h = mv->card[i];
        if( c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' ||
            c == 'f' || c == 'g' || c == 'h' || c == 'i' || c == 'j' ||
            h == ']' || h == '[' || h == '}' || h == '{' || h == '/' ||
            h == '%' || h == '\'' || h == '\"' || h == '!' || h == '~' ||
            h == ';' || h == ',') {
            pas++;
            break;
        }
    }
    if(!pas) return(0);
    if(mv->debug == 3) printf("%s\n",mv->card);
    for(i = osp; i <= 68; i++) {
        c = tolower(mv->card[i]);
        h = mv->card[i];
        if( c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' ||
            c == 'f' || c == 'g' || c == 'h' || c == 'i' || c == 'j' ||
            h == ']' || h == '[' || h == '}' || h == '{' || h == '/' ||
            h == '%' || h == '\'' || h == '\"' ||h == '!' || h == '~' ||
            h == ';' || h == ',') {
            tlshift_meb(tpv,i);
        }
    }
    return(0);
}

int tlshift_meb(int tpv,int i)
//int  tpv;
//int  i;
{
    int  k1,k2,k3,k4;
    int  biv;      
    int  civ;      
    int  tiv;      
    int  kv;
    char c;
    int  sfv;
    char ch;

    c = mv->card[i];
    sfv = -1;
    if(isupper(c)) sfv = 1;
    if( c == ']' || c == '}' || c == '/' ||
        c == '\'' || c == '!' || c == ';') sfv = 1;
    k1 = getkeyp_meb(i,1);       /* get original natural keyp */
    k2 = getkeyp_meb(i,2);       /* get before keyp */
    k3 = getkeyp_meb(i+sfv,1);   /* get current natural keyp */
    k4 = getkeyp_meb(i+sfv,3);   /* get current shift keyp */
    biv = k2 - k1; /* before interval */
    civ = k4 - k3; /* current interval */
    tiv = k4 - k2; /* transpose interval */
    kv = civ - biv;
    if(mv->debug == 3) {
        printf("tlshift_() %c tpv=%d sfv=%d tiv=%d\n",c,tpv,sfv,tiv);
        printf("        biv(%d) = k2(%d) - k1(%d)\n",biv,k2,k1);
        printf("        civ(%d) = k4(%d) - k3(%d)\n",civ,k4,k3);
        printf("        kv(%d) = civ(%d) - biv(%d)\n",kv,civ,biv);
    }
    if(sfv > 0 && tiv == 1) {
        if( mv->card[i+1] == ' ' || mv->card[i+1] == 'o' ||
            mv->card[i+1] == '.' || mv->card[i+1] == 'x' ||
            mv->card[i+1] == '+' || mv->card[i+1] == '-') {
            switch(c) {
                case  'A' : ch = '0';
                            break;
                case  'B' : ch = '1';
                            break;
                case  'C' : ch = '2';
                            break;
                case  'D' : ch = '3';
                            break;
                case  'E' : ch = '4';
                            break;
                case  'F' : ch = '5';
                            break;
                case  'G' : ch = '6';
                            break;
                case  'H' : ch = '7';
                            break;
                case  'I' : ch = '8';
                            break;
                case  'J' : ch = '9';
                            break;
                case  ']' : ch = '#';
                            break;
                case  '}' : ch = '&';
                            break;
                case  '/' : ch = '$';
                            break;
                case '\'' : ch = '*';
                            break;
                case  '!' : ch = '@';
                            break;
                case  ';' : ch = ':';
                            break;
                default   : ch = c;
                            break;
            }
            if(tiv == tpv) ch = getkch_meb(kv,ch);
            mv->card[i+1] = ch;
            mv->card[i] = ' ';
        }
    }
    if(sfv < 0 && tiv == -1) {
        if( mv->card[i-1] == ' ' || mv->card[i-1] == 'o' ||
            mv->card[i-1] == '.' || mv->card[i-1] == 'x' ||
            mv->card[i-1] == '+' || mv->card[i-1] == '-') {
            switch(c) {
                case  'a' : ch = '0';
                            break;
                case  'b' : ch = '1';
                            break;
                case  'c' : ch = '2';
                            break;
                case  'd' : ch = '3';
                            break;
                case  'e' : ch = '4';
                            break;
                case  'f' : ch = '5';
                            break;
                case  'g' : ch = '6';
                            break;
                case  'h' : ch = '7';
                            break;     
                case  'i' : ch = '8'; 
                            break;
                case  'j' : ch = '9';
                            break;
                case  '[' : ch = '#';
                            break;
                case  '{' : ch = '&';
                            break;
                case  '%' : ch = '$';
                            break;
                case '\"' : ch = '*';
                            break;
                case  '~' : ch = '@';
                            break;
                case  ',' : ch = ':';
                            break;
                default   : ch = c;
                            break;
            }
            if(tiv == tpv) ch = getkch_meb(kv,ch);
            mv->card[i-1] = ch;
            mv->card[i] = ' ';
        }
    }
    return(0);
}

/* %xx [ set bexp
note            value        rest     no.      name
 %xx |O|       800,000        Rb        0
 %xx O..       700,000        Rw..      1
 %xx O.        600,000        Rw.       2
 %xx O         400,000        Rw        3      whole
 %xx o/..      350,000        Rh..      4
 %xx o/.       300,000        Rh.       5
 %xx o/        200,000        Rh        6       half
 %xx _/..      175,000        R..       7
 %xx _/.       150,000        R.        8
 %xx _/        100,000        R         9          4
 %xx _/)..      87,500        r..      10
 %xx _/).       75,000        r.       11
 %xx _/)        50,000        r        12          8
 %xx _/))..     43,750        rr..     13  
 %xx _/)).      37,500        rr.      14  
 %xx _/))       25,000        rr       15         16             
 %xx _/)))..    21,875        rrr..    16  
 %xx _/))).     18,750        rrr.     17 
 %xx _/)))      12,500        rrr      18         32
 %xx _/)))).     9,375        rrrr.    19 
 %xx _/))))      6,250        rrrr     20         64
 %xx _/)))))     3,125        rrrrr    21        128
*/
int syndefgr_meb()
{   
    if(mb->nval > 6250) ge->def_bexp = gd->n64;
    if(mb->nval > 9375) ge->def_bexp = gd->n64p;
    if(mb->nval > 12500) ge->def_bexp = gd->n32;
    if(mb->nval > 18750) ge->def_bexp = gd->n32p;
    if(mb->nval > 21875) ge->def_bexp = (gd->n32p + gd->n16)/2; /* n32pp */
    if(mb->nval > 25000) ge->def_bexp = gd->n16;
    if(mb->nval > 37500) ge->def_bexp = gd->n16p; 
    if(mb->nval > 43750) ge->def_bexp = (gd->n16p + gd->n8)/2; /* n16pp */
    if(mb->nval > 50000) ge->def_bexp = gd->n8;
    if(mb->nval > 75000) ge->def_bexp = gd->n8p;
    if(mb->nval > 87500) ge->def_bexp = (gd->n8p + gd->q)/2; /* n8pp */
    if(mb->nval > 100000) ge->def_bexp = gd->q;
    if(mb->nval > 150000) ge->def_bexp = gd->qp;
    if(mb->nval > 175000) ge->def_bexp = (gd->qp + gd->h)/2; /* qpp */
    if(mb->nval > 200000) ge->def_bexp = gd->h;
    if(mb->nval > 300000) ge->def_bexp = gd->hp;
    if(mb->nval > 350000) ge->def_bexp = (gd->hp + gd->w)/2; /* hpp */
    if(mb->nval > 400000) ge->def_bexp = gd->w;
    if(mb->nval > 600000) ge->def_bexp = gd->wp;
    if(mb->nval > 700000) ge->def_bexp = gd->wpp;
    if(mb->nval > 800000)  ge->def_bexp = gd->ww;
    if(mv->debug == 13) printf("syndefgr_() mb->nval=%ld ge->def_bexp=%d\n",
        mb->nval,ge->def_bexp);
    return(0);
}

int synasngr_meb()
{   
    if(mb->nval > 6250) ge->asn_bexp = ga->n64;
    if(mb->nval > 9375) ge->asn_bexp = ga->n64p;
    if(mb->nval > 12500) ge->asn_bexp = ga->n32;
    if(mb->nval > 18750) ge->asn_bexp = ga->n32p;
    if(mb->nval > 21875) ge->asn_bexp = ga->n32pp;
    if(mb->nval > 25000) ge->asn_bexp = ga->n16;
    if(mb->nval > 37500) ge->asn_bexp = ga->n16p; 
    if(mb->nval > 43750) ge->asn_bexp = ga->n16pp;
    if(mb->nval > 50000) ge->asn_bexp = ga->n8;
    if(mb->nval > 75000) ge->asn_bexp = ga->n8p;
    if(mb->nval > 87500) ge->asn_bexp = ga->n8pp; 
    if(mb->nval > 100000) ge->asn_bexp = ga->q;
    if(mb->nval > 150000) ge->asn_bexp = ga->qp;
    if(mb->nval > 175000) ge->asn_bexp = ga->qpp;
    if(mb->nval > 200000) ge->asn_bexp = ga->h;
    if(mb->nval > 300000) ge->asn_bexp = ga->hp;
    if(mb->nval > 350000) ge->asn_bexp = ga->hpp;
    if(mb->nval > 400000) ge->asn_bexp = ga->w;
    if(mb->nval > 600000) ge->asn_bexp = ga->wp;
    if(mb->nval > 700000) ge->asn_bexp = ga->wpp;
    if(mb->nval > 800000)  ge->asn_bexp = ga->ww;
    if(mv->debug == 13) printf("synasngr_() mb->nval=%ld ge->asn_bexp=%d\n",
        mb->nval,ge->asn_bexp);
    return(0);
}

int synmodgr_meb()
{   
    if(mb->nval > 6250) ge->mod_bexp = gm->n64;
    if(mb->nval > 9375) ge->mod_bexp = gm->n64p;
    if(mb->nval > 12500) ge->mod_bexp = gm->n32;
    if(mb->nval > 18750) ge->mod_bexp = gm->n32p;
    if(mb->nval > 21875) ge->mod_bexp = gm->n32pp;
    if(mb->nval > 25000) ge->mod_bexp = gm->n16;
    if(mb->nval > 37500) ge->mod_bexp = gm->n16p; 
    if(mb->nval > 43750) ge->mod_bexp = gm->n16pp;
    if(mb->nval > 50000) ge->mod_bexp = gm->n8;
    if(mb->nval > 75000) ge->mod_bexp = gm->n8p;
    if(mb->nval > 87500) ge->mod_bexp = gm->n8pp; 
    if(mb->nval > 100000) ge->mod_bexp = gm->q;
    if(mb->nval > 150000) ge->mod_bexp = gm->qp;
    if(mb->nval > 175000) ge->mod_bexp = gm->qpp;
    if(mb->nval > 200000) ge->mod_bexp = gm->h;
    if(mb->nval > 300000) ge->mod_bexp = gm->hp;
    if(mb->nval > 350000) ge->mod_bexp = gm->hpp;
    if(mb->nval > 400000) ge->mod_bexp = gm->w;
    if(mb->nval > 600000) ge->mod_bexp = gm->wp;
    if(mb->nval > 700000) ge->mod_bexp = gm->wpp;
    if(mb->nval > 800000)  ge->mod_bexp = gm->ww;
    if(mv->debug == 13) printf("synmodgr_() mb->nval=%ld  ge->mod_bexp=%d\n",
        mb->nval,ge->mod_bexp);
    return(0);
}

int synsetgr_meb()
{
    /* ignore pp _/.. -> _/. */
    if(mb->nval > 6250) ge->set_bexp = gs->n64;
    if(mb->nval > 9375) ge->set_bexp = gs->n64p;
    if(mb->nval > 12500) ge->set_bexp = gs->n32;
    if(mb->nval > 18750) ge->set_bexp = gs->n32p;
    if(mb->nval > 25000) ge->set_bexp = gs->n16;
    if(mb->nval > 37500) ge->set_bexp = gs->n16p;
    if(mb->nval > 50000) ge->set_bexp = gs->n8;
    if(mb->nval > 75000) ge->set_bexp = gs->n8p;
    if(mb->nval > 100000) ge->set_bexp = gs->q;
    if(mb->nval > 150000) ge->set_bexp = gs->qp;
    if(mb->nval > 200000) ge->set_bexp = gs->h;
    if(mb->nval > 300000) ge->set_bexp = gs->hp;
    if(mb->nval > 400000) ge->set_bexp = gs->w;
    if(mb->nval > 600000) ge->set_bexp = gs->wp;
    if(mb->nval > 800000)  ge->set_bexp = gs->ww;
    if(mv->debug == 13) printf("synsetgr_() mb->nval=%ld  ge->set_bexp=%d\n",
        mb->nval,ge->set_bexp);
    return(0);
}

int synascgr_meb()
{
    if(mb->nval > 6250) ge->asc_bexp = gc->n64;
    if(mb->nval > 9375) ge->asc_bexp = gc->n64p;
    if(mb->nval > 12500) ge->asc_bexp = gc->n32;
    if(mb->nval > 18750) ge->asc_bexp = gc->n32p;
    if(mb->nval > 21875) ge->asc_bexp = gc->n32pp;
    if(mb->nval > 25000) ge->asc_bexp = gc->n16;
    if(mb->nval > 37500) ge->asc_bexp = gc->n16p;
    if(mb->nval > 43750) ge->asc_bexp = gc->n16pp;
    if(mb->nval > 50000) ge->asc_bexp = gc->n8;
    if(mb->nval > 75000) ge->asc_bexp = gc->n8p;
    if(mb->nval > 87500) ge->asc_bexp = gc->n8pp;
    if(mb->nval > 100000) ge->asc_bexp = gc->q;
    if(mb->nval > 150000) ge->asc_bexp = gc->qp;
    if(mb->nval > 175000) ge->asc_bexp = gc->qpp;
    if(mb->nval > 200000) ge->asc_bexp = gc->h;
    if(mb->nval > 300000) ge->asc_bexp = gc->hp;
    if(mb->nval > 350000) ge->asc_bexp = gc->hpp;
    if(mb->nval > 400000) ge->asc_bexp = gc->w;
    if(mb->nval > 600000) ge->asc_bexp = gc->wp;
    if(mb->nval > 700000) ge->asc_bexp = gc->wpp;
    if(mb->nval > 800000)  ge->asc_bexp = gc->ww;
    if(mv->debug == 13) printf("synascgr_() mb->nval=%ld ge->asc_bexp=%d\n",
        mb->nval,ge->asc_bexp);
    return(0);
}

/* add 2008.5.4 */
int gr_w_set(char *w)
//char *w;
{
    int  i;

    for(i = 0; i < 12; i++) {
        if(w[i] == '\0') break;
        if(w[i] == '[') break;
        if(w[i] == ' ') break;
        if(isdigit(w[i])) break;
    }
    w[i] = '\0';
    /* printf("gr_w_set() w=%s\n",w); */
    return(0);
}

