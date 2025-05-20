#include <stdio.h>
#include <dos.h>
#include <lib\crt.h>
#include <lib\bslib.h>
#include "mex.h"
long dt_all;  /* dt counter */
extern MEXSRG *ls;
extern MEXVAR *lv;
extern MEXACT *la;
extern MQXFRS *fv;
extern SETUPD *su;           /* setup data */
extern FILE   *fopen_h(),*fpi;
extern unsigned int sport[16];
extern unsigned int dport[16];
extern char msg[82];
extern int  notef[MAXPT][16][128];
extern int  aport[MAXPT];             /* active port */
extern char space[40];
extern long moutc;                    /* output data count */
extern long routc;                    /* Rs232c output data count */
extern long f1cnt;
extern long lscnt;
extern float lstime;
extern int  moutf;                    /* m output flag */
extern int  swbrkf;
extern int  ckport;
extern int  cktrak;
extern int  ckmemb;
extern int  sequ;
extern int  barn;
extern int  pfsync;
extern int  efsync;
extern int  mtclag;

mexplay2_mex(dfile)
char dfile[STLN];
{
    int  nflg;
    int  n,i,j,k;
    int  c1;
    union MCODE u;
    union CHTOL {
        unsigned char wc[4];
        long wl;
    } d;
    unsigned char iword[6];
    int  svoutf;   /* save moutf */
    int  nxtgetf;
    register int c;
    register long dt;
    register long lt;
    int  hh,mm,ss,ff;
    int  dh,dm,ds,df;

    ckport = 0;
    cktrak = 0;
    sequ = 0;
    barn = 0;
    ckmemb = 0;
    f1cnt = 0;
    moutc = 0;
    routc = 0;
    moutf = 1;            /* output flag on */
    nflg = 0;
    pfsync = 0;
    efsync = 0;
    nxtgetf = 0;
    su->qhour = 0;
    su->qsec = 0;
    su->qmin = 0;
    su->qflame = 0;
    su->qtype = 3;  /* 30 flame non drop */
    mtclag = 0;
    if(ls->smpgen || ls->smpread) esuinit_mex();
    if(ls->smpread) comout_mex(0xf0);  /* set ESU read mode */
    for(i = 0; i < 39; i++) space[i] = ' ';
    space[39] = '\0';
    getmexrdf_mex(dfile);
    initactp_mex();
    chkactb_mex();
    if(ls->act232c) emu232c_mex(); /* 232c setup */
    prtmode_mex();
    fpi = fopen_h(dfile,"rb");
    if(su->xrtn == 1 || su->xrtn == 3) {    /* track select */
        while((c = fgetc(fpi)) != EOF) {
            if(c == 0xfd) {
                iword[0] = c;           /* fd code check */
                iword[1] = fgetc(fpi);
                iword[2] = fgetc(fpi);
                iword[3] = fgetc(fpi);
                c1 = iword[1] & 0x80;
                if(c1) {
                    ckport = iword[1] & 0x7f;
                    if(ckport > MAXPT) {
                        sprintf(msg,"Error! port number over than %d\n",MAXPT);
                        wprintf_mex(msg);
                        sprintf(msg,"MexSMF_Play end %s\n",su->time);
                        wprintf_mex(msg);
                        fclose(fpi);
                        return(-1);
                    }
                    cktrak = iword[2];
                    ckmemb = iword[3];
                    if(cktrak == su->trak) break;
                }
            }
        }
    }
    fclose(fpi);
    fpi = fopen_h(dfile,"rb");
    for(i = 0; i <= 22 ; i++) c = fgetc(fpi); /* SMF Heder pass */
    if(ls->brself) {
        while((c = fgetc(fpi)) != EOF) {
            if(c == 0xfb) {
                iword[0] = c;           /* fb code check */
                iword[1] = fgetc(fpi);
                iword[2] = fgetc(fpi);
                iword[3] = fgetc(fpi);
                iword[0] &= 0x00;
                iword[1] &= 0x7f;
                compchk_mex(&u,iword);
                sequ = u.wi[0];
                barn = u.wi[1];
                /* _dprintf("%02d%05d\n",sequ,barn); */
                if(ls->brself == 1 && sequ == lv->ssequ) break;
                if(ls->brself == 2 && barn == lv->sbarn) break;
                if(ls->brself == 3)
                   if(sequ == lv->ssequ && barn == lv->sbarn) break;
            }
        }
        if(c == EOF) {
            sprintf(msg,"Error! movement or bar number not found");
            dputf_mex(2,7,msg);
            return(0);
        }
    }
    for(k = 0; k < MAXPT; k++) {
        for(i = 0; i < 16; i++) {
            for(j = 0; j < 128; j++) notef[k][i][j] = 0;
        }
    }
    for(i = 0; i < MAXPT; i++) aport[i] = 0;
    swbrkf = 0;           /* switch break flag */
    if(ls->smpread) esuread_mex();
    timeset();
    sprintf(msg,"MexSMF_Play start %s %s file \"%s\"",
        su->date,su->time,dfile);
    dputf_mex(2,4,msg);
    su->counter = 0;
    if(ls->smpgen) {
        smpgen_mex();
        su->clockflg = 0;
        while(su->clockflg < ls->edelay);  /* Esu generate delay loop */
    }
    if(ls->smpread) {
        su->clockflg = 0;
        while(su->clockflg < ls->erdelay);  /* Esu read delay loop */
    }
    dt = 0;
    dt_all = 0;
    su->counter = 0;
    su->subclk = 5; /* first lag */
    su->countall = -5;
    while(1) {
        if(c == EOF) break;
        if(nxtgetf == 1) c = c1;
        else if((c = fgetc(fpi)) == EOF) break;
/*      else c = fgetc(fpi);   eof (ff 2f 00) */
        nxtgetf = 0;
        switch (c) {
            case  0xf1 : iword[0] = c;
                         c = fgetc(fpi);
                         iword[1] = c;
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             midiout_mex(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = fgetc(fpi);
                             iword[3] = fgetc(fpi);
                             d.wc[0] = iword[3];
                             d.wc[1] = iword[2];
                             d.wc[2] = iword[1] & 0x7f;
                             d.wc[3] = 0;
                             dt = d.wl;
                             dt_all = dt_all + dt;
                             /*
                             sprintf(msg,"dt=%ld su->subclk=%d\n",
                                 dt,su->subclk);
                             dputf_mex(2,7,msg);
                             */
                             dt = dt + su->subclk/2;
                             /* while(su->counter < dt) pbreak_mex(); */
                             if( su->counter < dt) posprint_mex();
                             while(su->counter < dt) SMF_break_mex();
                             su->subclk = 0;
                             lscnt += (su->counter - dt);
                             su->counter = 0;
                         }
                         break;
            case  0xf0 : if(ls->smpte) smpteout_mex(c);
                         c = fgetc(fpi);
                         if(c == 0x7f) {
                             if(ls->smpte) smpteout_mex(c);
                             fltime_mex();
                         }
                         break;
            case  0xfb : iword[0] = c;           /* fb code check */
                         c = fgetc(fpi);
                         iword[1] = c;
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             midiout_mex(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = fgetc(fpi);
                             iword[3] = fgetc(fpi);
                             iword[0] &= 0x00;
                             iword[1] &= 0x7f;
                             compchk_mex(&u,iword);
                             sequ = u.wi[0];
                             barn = u.wi[1];
                             /* posprint_mex(); */
                             if(ls->mbendf) {
                                 if(ls->mbendf == 1 && sequ >= lv->esequ) {
                                     swbrkf = 1;
                                     break;
                                 }
                                 if(ls->mbendf == 2 && barn >= lv->ebarn) {
                                     swbrkf = 1;
                                     break;
                                 }
                                 if(ls->mbendf == 3)
                                     if(sequ > lv->esequ ||
                                     barn >= lv->ebarn) {
                                         swbrkf = 1;
                                         break;
                                 }
                             }
                         }
                         break;
            case  0xfd : iword[0] = c;           /* fd code check */
                         c = fgetc(fpi);
                         iword[1] = c;
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             midiout_mex(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = fgetc(fpi);
                             iword[3] = fgetc(fpi);
                             ckport = iword[1] & 0x7f;
                             cktrak = iword[2];
                             ckmemb = iword[3];
                             setmoutf_mex();
                         }
                         break;
            case  0xe6 : iword[0] = c;           /* e6 code check */
                         c = fgetc(fpi);
                         iword[1] = c;
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             midiout_mex(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = fgetc(fpi);
                             iword[3] = fgetc(fpi);
                             iword[1] &= 0x7f;
                             if(iword[1] == 3) {
                                 pfsync = 0;
                                 ls->dtcport = iword[3];
                             }
                             if(iword[1] == 4) ls->mtcport = iword[3];
                             if(iword[1] == 5) {
                                 pfsync = 0;
                                 ls->smptp = 1; /* performer direct sync on */
                                 ls->dtcport = iword[3];
                             }
                             if(iword[1] == 6) {
                                 ls->smpte = 1;
                                 ls->mtcport = iword[3];
                             }
                             if(iword[1] == 7) {
                                 pfsync = 0;
                                 ls->smptp = 0; /* sync off */
                             }
                             if(iword[1] == 8) ls->smpte = 0;
                         }
                         break;
   /* eof (ff 2f 00) code check */
   /*
            case  0xff : iword[0] = c;  
                         iword[1] = fgetc(fpi);
                         iword[2] = fgetc(fpi);
                         if(iword[1] == 0x2f && iword[2] == 0x00) swbrkf = 1;
                         else {
                             midiout_mex(iword[0]);
                             midiout_mex(iword[1]);
                             nxtgetf = 1;
                         }
                         break;
   */
            default    : midiout_mex(c);
                         if(nflg == 2) {
                             if(c == 0) notef[ckport][lv->chan][lv->notep]--;
                             else notef[ckport][lv->chan][lv->notep]++;
                             nflg = 0;
                             aport[ckport] = 1;
                         }
                         if(nflg == 1) {
                             lv->notep = c;
                             nflg = 2;
                         }
                         n = c & 0xf0;
                         if(n == 0x90) {
                             lv->chan = c & 0x0f;
                             nflg = 1;
                         }
                         break;
        }
        if(swbrkf) break;
    }
    timecv_mex(dt_all,&dh,&dm,&ds,&df);
    timecv_mex(su->countall,&hh,&mm,&ss,&ff);
    lstime = lscnt / 3000.;
    posprint_mex();
    sprintf(msg,"dt=%ld %d:%d:%d/%d Clk(lost)=%ld(%ld) %d:%d:%d/%d(%f)",
        dt_all,dh,dm,ds,df,su->countall,lscnt,hh,mm,ss,ff,lstime);
    dputf_mex(2,6,msg);
    midistop_mex();
    timeset();
    if(ls->act232c) {
        sprintf(msg,"Mex end %s Out=%ld (RS232c=%ld)",
        su->time,moutc,routc);
    }
    else {
        sprintf(msg,"Mex end %s Out=%ld",su->time,moutc);
    }
    /* sprintf(msg,"Mexplay end %s%s",su->time,space); */
    dputf_mex(2,7,msg);
    if(ls->smpgen || ls->smpread) comout_mex(0xff); /* Reset Esu */
    fclose(fpi);
    gotoxy(0,22);
    return(0);
}

timecv_mex(dt,hh,mm,ss,ff)
long dt;
int  *hh;
int  *mm;
int  *ss;
int  *ff;
{
    *hh = dt / 10800000; /* (3600 * 3000) */
    *mm = (dt - (*hh * 10800000))/180000;  /* 60 * 3000 */
    *ss = (dt - (*hh * 10800000) - (*mm * 180000))/3000;
    *ff = (dt - (*hh * 10800000) - (*mm * 180000) - (*ss * 3000))/100; 
    *ff = dt - (*hh * 10800000) - (*mm * 180000) - (*ss * 3000);
    *ff = *ff/100; 
    return(0);
}
