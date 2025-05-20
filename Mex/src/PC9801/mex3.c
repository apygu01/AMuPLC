/* mex3.c */
#include "mex.h"
extern   MEXSRG *ls;
extern   MEXVAR *lv;
extern   MEXACT *la;
extern   MEXKEY *kv;           /* key control */
extern   KEYFLG *kf;           /* check shift,caps,ctrl,grph */
extern   SETUPD *su;           /* setup data */
extern   FILE   *fopen_h(),*fpi;
extern   char msg[82];
unsigned int sport[16];        /* status port address */
unsigned int dport[16];        /* data port address */
int  notef[MAXPT][16][128];
int  aport[MAXPT];             /* active port */
char space[40];
char xs[26];
long moutc;                    /* output data count */
long routc;                    /* Rs232c output data count */
long f1cnt;
long lscnt;
float lstime;
long esuflame;
long mtcflame;
int  moutf;                    /* m output flag */
int  swbrkf;
int  ckport;
int  cktrak;
int  ckmemb;
int  sequ;
int  barn;
int  pfsync;
int  efsync;
int  smph;
int  smpm;
int  smps;
int  smpf;
int  mtclag;

mexplay_mex(dfile)
char dfile[STLN];
{
    int  c;
    int  nflg;
    int  n,i,j,k;
    int  c1;
    union MCODE u;
    unsigned char iword[6];
    int  svoutf;   /* save moutf */
    int  nxtgetf;

    if(ls->debug == 4) {
        sprintf(msg,"dfile = %s ls->play = %d ls->brself = %d\n",
            dfile,ls->play,ls->brself);
        gprintf_mex(msg);
    }
    fpi = fopen_h(dfile,"rb");
    i = 0;
    k = 0;
    while(1) {
        if((c = fgetc(fpi)) == EOF) {
            sprintf(msg,"Error! data %s\n",dfile);
            wprintf_mex(msg);
        }
        else {
            if(c == 'M' && i == 0) k++;
            if(c == 'X' && i == 1) k++;
            if(c == 'h' && i == 2) k++;
            if(c == 'd' && i == 3) k++;
            if(i == 12) {
                c = 0xff & c;
                ls->smptef = 0xffffff00 | c;
            }
            if(i == 13) ls->mexres = 0xff & c;
            i++;
            if(i > 14) break;
        }
    }
    fclose(fpi);
    ls->smptef = -ls->smptef;
    /*
    if(ls->smptef == -24) ls->smptef = 24;
    if(ls->smptef == -25) ls->smptef = 25;
    if(ls->smptef == -29) ls->smptef = 30;
    if(ls->smptef == -30) ls->smptef = 30;
    */
    if(ls->smptef == 24) ls->smptyp = 0;
    if(ls->smptef == 25) ls->smptyp = 1;
    if(ls->smptef == 29) ls->smptyp = 2;
    if(ls->smptef == 30) ls->smptyp = 3;
    if(k == 4) {
        if(ls->clktyp == 0) ls->clktyp = 4;
        clockset();
        if(ls->play == 3) mexplay3_mex(dfile);  /* Memory Play */
        else mexplay2_mex(dfile);
        return(0);
    }
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
                        sprintf(msg,"Mexplay end %s\n",su->time);
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
                if(ls->brself == 2 && barn >= lv->sbarn) break;
                if(ls->brself == 3)
                   if(sequ == lv->ssequ && barn >= lv->sbarn) break;
            }
        }
        if(c == EOF) {
            sprintf(msg,"Error! movement or bar number not found");
            dputf_mex(1,20,msg);
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
    sprintf(msg,"Mexplay start %s %s file \"%s\"",su->date,su->time,dfile);
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
    while(1) {
        if(nxtgetf == 1) c = c1;
        else if((c = fgetc(fpi)) == EOF) break;
        nxtgetf = 0;
        switch (c) {
            case  0xf1 : while(!su->clockflg) pbreak_mex();
                         su->clockflg = 0;
                         f1cnt++;
                         if(ls->smpte) smpteout_mex(c);
                         c = fgetc(fpi);
                         qftime_mex(c);
                         if(ls->smpte) smpteout_mex(c);
                         break;
            case  0xf0 : if(ls->smpte) smpteout_mex(c);
                         c = fgetc(fpi);
                         if(c == 0x7f) {
                             if(ls->smpte) smpteout_mex(c);
                             fltime_mex();
                         }
                         break;
            case  0xfb : iword[0] = c;           /* fb code check */
                         iword[1] = fgetc(fpi);
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
                         iword[1] = fgetc(fpi);
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
            /*
            case  0xe4 : iword[0] = c;
                         iword[1] = fgetc(fpi);
                         svoutf = moutf;
                         moutf = 1;
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             midiout_mex(iword[0]);
                             nxtgetf = 1;
                         }
                         else {
                             iword[2] = fgetc(fpi);
                             iword[3] = fgetc(fpi);
                             iword[4] = fgetc(fpi);
                             ckport = iword[1] & 0x7f;
                             midiout_mex(iword[2]);
                             midiout_mex(iword[3]);
                             midiout_mex(iword[4]);
                         }
                         moutf = svoutf;
                         break;
            */
            case  0xe5 : iword[0] = c;           /* fd code check */
                         iword[1] = fgetc(fpi);
                         svoutf = moutf;
                         moutf = 1;
                         c1 = iword[1] & 0x80;
                         if(!c1) {
                             midiout_mex(iword[0]);
                             nxtgetf = 1;
                         }
                         else ckport = iword[1] & 0x7f;
                         moutf = svoutf;
                         break;
            case  0xe6 : iword[0] = c;           /* e6 code check */
                         iword[1] = fgetc(fpi);
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
    if(ls->monitor) {
        lscnt = su->counter - f1cnt;
        lstime = lscnt / 1.2;
        posprint_mex();
        if(ls->act232c) {
            sprintf(msg,
                "Out=%ld (232c=%ld) Lost_Count=%ld Lost_Time(msec)=%-8.2f",
                moutc,routc,lscnt,lstime);
            dputf_mex(2,6,msg);
        }
        else {
            sprintf(msg,"Out=%ld Lost_Count=%ld Lost_Time(msec)=%-8.2f",
                moutc,lscnt,lstime);
            dputf_mex(2,6,msg);
        }
    }
    midistop_mex();
    timeset();
    sprintf(msg,"Mexplay end %s%s",su->time,space);
    dputf_mex(2,7,msg);
    if(ls->smpgen || ls->smpread) comout_mex(0xff); /* Reset Esu */
    fclose(fpi);
    gotoxy(0,22);
    return(0);
}

/*              xrtn   trak   memb  su->xrtn
   fn       :      0      0      0         0
   fn/1     :      0      1      0         1
   fn/2,3   :      3      2      3         2
   fn/a10   :      0     10     10         3
   fn/10    :     36      0     10         4
*/
setmoutf_mex()
{
    moutf = 0;
    switch (su->xrtn) {
       case   0 : moutf = 1;
                  break;
       case   1 : if(cktrak == su->trak) moutf = 1;
                  break;
       case   2 : if(ckmemb == su->memb) moutf = 1;
                  break;
       case   3 : if(cktrak == su->trak &&
                      ckmemb == su->memb) moutf = 1;
                  break;
       case   4 : if(cktrak <= su->trak &&
                      ckmemb <= su->memb) moutf = 1;
                  break;
       default  : moutf = 0;
                  break;
    }
    return(0);
}

smpteout_mex(c)
char c;
{
    int  w;

    w = ckport;
    ckport = ls->mtcport;
    midiout_mex(c);
    ckport = w;
    return(0);
}

midiout_mex(c)
char c;
{
    char x;
    int  ack;
    int  k;

    if(!moutf) return(0);
    if(kv->xport != 88) {
        if(kv->kmflg == 1) {
            for(k = 0; k < MAXPT; k++) portnoff_mex(k);
            kv->kmflg = 0;
        }
        if(kv->xport != ckport) return(0);
    }
    x = c & 0xff;
    if(ls->act232c) {
        if(ls->e232cp == 999) em232tx(x);
        else if(ckport == ls->e232cp) em232tx(x);
    }
    if(!la->p_act[ckport]) return(0); /* port active ? */
    while((ack = inp(sport[ckport])) & 0x40); /* send ok ? */
    outp(dport[ckport],x);
    if(ls->monitor == 0) {
        moutc++;
        return(0);
    }
    if(ls->monitor == 2) putchr('.');
    if(ls->monitor == 1) if(!(moutc % 10)) putchr('.');
    moutc++;
    ack = inp(dport[ckport]);
    return(ack);
}

putmtc_mex(c)
int  c;
{   
    unsigned char x; 
    int  ack;
    int  k; 
    char wk[40];
    
    if(!moutf) return(0);
    if(kv->xport != 88) {
        if(kv->kmflg == 1) { 
            for(k = 0; k < MAXPT; k++) portnoff_mex(k);
            kv->kmflg = 0;
        }       
        if(kv->xport != ls->mtcport) return(0);
    }
    x = c & 0xff; 
    if(!la->p_act[ls->mtcport]) return(0); /* port active ? */
    while((ack = inp(sport[ls->mtcport])) & 0x40); /* send ok ? */ 
    outp(dport[ls->mtcport],x);
    if(ls->debug == 3) {
        xs[su->xp] = x;
        su->xp++;
        if(su->xp > 15) {
            sprintf(msg,"%02x %02x %02x %02x %02x %02x %02x %02x ",
            xs[0] & 0xff,xs[1] & 0xff,xs[2] & 0xff,xs[3] & 0xff,xs[4] & 0xff,
            xs[5] & 0xff,xs[6] & 0xff,xs[7] & 0xff);
            sprintf(wk," %02x %02x %02x %02x %02x %02x %02x %02x ",
            xs[8] & 0xff,xs[9] & 0xff,xs[10] & 0xff,xs[11] & 0xff,
            xs[12] & 0xff,xs[13] & 0xff,xs[14] & 0xff,xs[15] & 0xff);
            strcat(msg,wk);
            dputf_mex(1,su->yp,msg);
            su->yp++;
            if(su->yp > 22) su->yp = 14;
            su->xp = 0;
        }
    }
    moutc++;
    ack = inp(dport[ls->mtcport]);
    return(ack);
}   

fltime_mex()
{
    int  c;

    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    c = fgetc(fpi);
    if(ls->smpte) smpteout_mex(c);
    if(c != 0xf7) putstr("Error! full time message\n");
    else {
        if(f1cnt && ls->monitor) {
            lscnt = su->counter - f1cnt;
            lstime = lscnt / 1.2;
            posprint_mex();
            if(ls->act232c) {
               sprintf(msg,
                 "Out=%ld (232c=%ld) Lost_Count=%ld Lost_Time(msec)=%-8.2f",
                  moutc,routc,lscnt,lstime);
               dputf_mex(2,6,msg);
            }
            else {
              sprintf(msg,"Out=%ld Lost_Count=%ld Lost_Time(msec)=%-8.2f",
                  moutc,lscnt,lstime);
              dputf_mex(2,6,msg);
            }
        }
        moutc = 0;
        su->clockflg = 0;
        while(su->clockflg < FTWAIT);
        f1cnt = 0;
        su->counter = 0;
        pfsync = 0;
    }
    return(0);
}

/*
SMPTE quoater flame message
 f1 0x : (0000 yyyy) flame LS xxx    : undefined
    1x : (0001 xxxy) flame MS yyyyy  : flame no. (0-29)
    2x : (0010 yyyy) sec LS   xx     : undefined
    3x : (0011 xxyy) sec MS   yyyyyy : sec count (0-59)
    4x : (0100 yyyy) min LS   xx     : undefined
    5x : (0101 xxyy) min MS   yyyyyy : min count (0-59)
    6x : (0110 zzzz) hour LS  x      : undefined
    7x : (0111 xyyz)          yy     : timecode type
                                       0 - 24 flame /sec
                                       1 - 25 flame /sec
                                       2 - 30 flame(drop)/sec
                                       3 - 30 flame(non drop)/sec
                              zzzz   : hour (0-23)
*/
qftime_mex(c)
int  c;
{
    int  x;
    int  w;
    static int fls = 0;
    static int fms = 0;
    static int sls = 0;
    static int sms = 0;
    static int mls = 0;
    static int mms = 0;
    static int hls = 0;
    static int hms = 0;
    static int wbarn = 9999;

    x = c & 0x00f0;
    x = x >> 4;
    switch (x) {
        case   0 : fls = c & 0x000f;
                   mtclag = 0;
                   break;
        case   1 : fms = c & 0x0001;
                   fms = fms * 16;
                   mtclag = 0;
                   break;
        case   2 : sls = c & 0x000f;
                   mtclag = 0;
                   break;
        case   3 : sms = c & 0x0003;
                   sms = sms * 16;
                   mtclag = 1;
                   if(pfsync) {
                       w = ckport;
                       ckport = ls->dtcport;  /* default */
                       midiout_mex(0xf8); /* performer advance code */
                       ckport = w;
                   }
                   if(ls->smpread) esuread_mex();
                   break;
        case   4 : mls = c & 0x000f;
                   mtclag = 1;
                   break;
        case   5 : mms = c & 0x0003;
                   mms = mms * 16;
                   mtclag = 1;
                   break;
        case   6 : hls = c & 0x000f;
                   mtclag = 1;
                   break;
        case   7 : hms = c & 0x0001;
                   hms = hms * 16;
                   su->qhour = hms + hls;
                   su->qsec = sms + sls;
                   su->qmin = mms + mls;
                   su->qflame = fms + fls;
                   su->qtype = (c >> 1) & 0x0003;
                   mtclag = 2;
                   if(ls->smptp) { /* performer's direct sync mode ? */
                       pfsync++;
                       w = ckport;
                       ckport = ls->dtcport;  /* default */
                       if(pfsync == 1) sposmsg_mex(); /* put position msg */
                       midiout_mex(0xf8); /* performer advance code */
                       ckport = w;
                   }
                   if(ls->monitor == 2) posprint_mex();
                   if(ls->monitor == 1) {
                       if(barn != wbarn) {
                           sprintf(msg,"\r%s%s",space,space);
                           wprintf_mex(msg);
                           posprint_mex();
                       }
                       wbarn = barn;
                   }
                   if(ls->smpread) esuread_mex();
                   break;
        defalt   : break;
    }
    return(0);
}

compchk_mex(um,iword)
union MCODE *um;
unsigned char iword[4];
{
    if(ls->comptyp == 0) {
        um->wc[0] = iword[1];
        um->wc[1] = iword[0];
        um->wc[2] = iword[3];
        um->wc[3] = iword[2];
    }
    if(ls->comptyp == 1) {
        um->wc[0] = iword[0];
        um->wc[1] = iword[1];
        um->wc[2] = iword[2];
        um->wc[3] = iword[3];
    }
    if(ls->comptyp == 2) {
        um->wc[0] = iword[1];
        um->wc[1] = iword[0];
        um->wc[2] = iword[3];
        um->wc[3] = iword[2];
    }
    return(0);
}

/* put smpte position message (direct time lock) */
sposmsg_mex()
{
    midiout_mex(0xf0);
    midiout_mex(0x28);
    midiout_mex(0x15);
    midiout_mex((char)su->qhour);
    midiout_mex((char)su->qmin);
    midiout_mex((char)su->qsec);
    midiout_mex((char)su->qflame);
    midiout_mex(0x00);
    midiout_mex(0xf7);
    /* waitloop_mex(); */
    return(0);
}

/* wait until mac(performer) ready */
waitloop_mex()
{
    /* su->clockflg = 0; */
    while(su->clockflg < 1); /* 1/120 sec */
    return(0);
}

/* put full time message
SMPTE full time code
 f0 7f : real time universal header
 xx    : channel
 01    : sub-id #1 (SMPTE)
 01    : sub-i2 #2 (full time code)
 hh    : 0xyyzzzz 
         x    : undefined
         yy   : 0 - 24 flame /sec
                1 - 25 flame /sec
                2 - 30 flame(drop)/sec
                3 - 30 flame(non drop)/sec
         zzzz : hour (0-23)
 mm    : minites(0-59)
 ss    : sec(0-59)
 ff    : flame(0-29)
 f7    : eox
*/
putflmsg_mex()
{
    int c;

    midiout_mex(0xf0);
    midiout_mex(0x7f);
    midiout_mex(0x00);
    midiout_mex(0x01);
    midiout_mex(0x01);
    c = su->qhour | (su->qtype << 4);
    /*
    _dprintf("su->qhour=%d su->qtype=%d c=%x\n",
        su->qhour,su->qtype,c);
    */
    midiout_mex((char)c);
    midiout_mex((char)su->qmin);
    midiout_mex((char)su->qsec);
    midiout_mex((char)su->qflame);
    midiout_mex(0xf7);
    return(0);
}

fullmsg_mex()
{       
    int c;
        
    putmtc_mex(0xf0);  
    putmtc_mex(0x7f);
    putmtc_mex(0x00);
    putmtc_mex(0x01);
    putmtc_mex(0x01);
    c = ls->mtchh | (ls->smptyp << 4);
    /*
    _dprintf("su->qhour=%d su->qtype=%d c=%x\n",
        su->qhour,su->qtype,c);
    */
    putmtc_mex((char)c);
    putmtc_mex((char)ls->mtcmm);
    putmtc_mex((char)ls->mtcss);
    putmtc_mex((char)ls->mtcff);
    putmtc_mex(0xf7);
    return(0);
}

esuinit_mex()
{
    inp(DATPORT);    /* Esu dummy read */
    inp(DPORT0);     /* Emu port1 dummy read */
    inp(DPORT1);     /* Emu port2 dummy read */
    inp(DPORT2);     /* Emu port3 dummy read */
    inp(DPORT3);     /* Emu port4 dummy read */
    inp(DPORT4);     /* Emu port5 dummy read */
    inp(DPORT5);     /* Emu port6 dummy read */
    inp(DPORT6);     /* Emu port7 dummy read */
    inp(DPORT7);     /* Emu port8 dummy read */
    inp(DPORT8);     /* Emu port9 dummy read */
    inp(DPORT9);     /* Emu port10 dummy read */
    inp(DPORTA);     /* Emu port11 dummy read */
    inp(DPORTB);     /* Emu port12 dummy read */
    inp(DPORTC);     /* Emu port13 dummy read */
    inp(DPORTD);     /* Emu port14 dummy read */
    inp(DPORTE);     /* Emu port15 dummy read */
    inp(DPORTF);     /* Emu port16 dummy read */
    comout_mex(0xff); /* Reset Esu */
    return(0);
}

/* 
    ls->etype = 0 - 24 flame /sec
                1 - 25 flame /sec
                2 - 30 flame(drop)/sec
                3 - 30 flame(non drop)/sec
*/
smpgen_mex()
{
    switch(ls->etype) {
        case  3 : comout_mex(0xf2);
                  break;
        case  2 : comout_mex(0xf3);
                  break;
        case  1 : comout_mex(0xf4);
                  break;
        case  0 : comout_mex(0xf5);
                  break;
        default : return(-1);
    }
    genstart_mex();  /* generate smpte signal */
    return(0);
}

comout_mex(a)
int a;
{
    bitchk_mex(0x40);     /* bit 6 check roop */
    outp(STSPORT,a);
    bitchk_mex(0x80);     /* bit 7 check roop */
    inp(DATPORT);
    return(0);
}

/* set start hh:mm:ss:ff smpte signal (write smpte to tape recoder) */
genstart_mex()
{
    int  flw,fup;
    int  slw,sup;
    int  mlw,mup;
    int  hlw,hup;

    fup = ls->eflame/10;
    flw = ls->eflame - (fup * 10);
    sup = ls->esec/10;
    slw = ls->esec - (sup * 10);
    mup = ls->emin/10;
    mlw = ls->emin - (mup * 10);
    hup = ls->ehour/10;
    hlw = ls->ehour - (hup * 10);
    datout_mex(flw);
    datout_mex(fup);
    datout_mex(slw);
    datout_mex(sup);
    datout_mex(mlw);
    datout_mex(mup);
    datout_mex(hlw);
    datout_mex(hup);
    return(0);
}

/* esu smpte read */
esuread_mex()
{
    int  m;

    su->clockflg = 0;
    while(su->clockflg < 120 * 30) {  /* 30 sec */
        pbreak_mex();
        m = comdut_mex(0xf1);
        if(m == 0xfe) {
            dataread_mex();
            break;
        }
    }
    if(m != 0xfe) {
        putstr("Time over SMPTE signal not catch\n");
        swbrkf = 1;           /* switch break flag */
    }
    return(0);
}

/* Esu smpte signal read (read smpte from tape recoder) */
dataread_mex()
{
    int  lw,up;

    lw = smpin_mex();
    up = smpin_mex();
    smpf = up * 10 + lw;   /* flame */
    lw = smpin_mex();
    up = smpin_mex();
    smps = up * 10 + lw;   /* sec */
    lw = smpin_mex();
    up = smpin_mex();
    smpm = up * 10 + lw;   /* min */
    lw = smpin_mex();
    up = smpin_mex();
    smph = up * 10 + lw;   /* hour */
    return(0);
}

/* esu data out */
datout_mex(a)
int a;
{
    bitchk_mex(0x40);    /* bit 6 check roop */
    outp(DATPORT,a);
    return(0);
}

/* esu status check */
bitchk_mex(bitdata)
int bitdata;
{
    int s;

    while(s=inp(STSPORT),(s&=bitdata)!=0);
    return(0);
}

comdut_mex(a)
int a;
{
    int p=0;

    bitchk_mex(0x40);
    outp(STSPORT,a);
    bitchk_mex(0x80);
    p=inp(DATPORT);
    return(p);
}

smpin_mex()
{
    int data;

    bitchk_mex(0x80);
    data=inp(DATPORT);
    return(data);
}


