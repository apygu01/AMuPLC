/* mexw.c */
#include "mex.h"
extern   MEXSRG *ls;
extern   MEXVAR *lv;
extern   MEXKEY *kv;           /* key control */
extern   MQXFRS *fv;           /* frase data */
extern   SETUPD *su;           /* setup data */
extern   FILE *fopen_h(),*fpi;
extern long moutc;             /* output data count */
extern int  moutf;             /* output on/off flag */
extern int  notef[MAXPT][16][128];
extern int  aport[MAXPT];             /* active port */
extern int  ckport;
extern char space[40];
extern char msg[82];
float tempo;       /* tempo */
float lagfm;       /* flame lag */
long tmptb[22];
int  fjtb[128];    /* flame jump counter */

mqxplay_mex(dfile)
char dfile[STLN];
{
    int  c;
    int  i,j,k;
    union MCODE u;
    unsigned char iword[6];
    int  swbrkf;
    long sync;
    long dsyn;         /* delta sync */
    float fsync;       /* _/ = 120 flame sync */
    long boc;          /* output count in bar */
    long dv;    /* delta value */
    int  qf;    /* quoater flame  SMPTE * 4 */
    long qfs;
    long df;    /* delta falme */
    int  f;
    int  sflame;
    int  pstop; /* play stop at end movn or barn */
    int  chan;
    int  notep;
    int  velo;

    ckport = 0;
    moutc = 0;
    sync = 0;
    boc = 0;
    pstop = 0;
    tempo = 120;    /* m.m. _/ = 120 */
    sflame = SMPTYP;
    if(ls->brself) {
        moutf = 0;
        ls->play = 0;
    }
    else moutf = 1;
    for(i = 0; i < 39; i++) space[i] = ' ';
    space[39] = '\0';
    fv->ftype = 0;
    for(i = 0; i < 4; i++) {
        fv->fchan[i] = 0;     /* frase channel */
        fv->fsval[i] = 0;     /* frase start value */
        fv->fctno[i] = 0;     /* frase control number */
        fv->feval[i] = 0;     /* frase end value */
        fv->fnval[i] = 0;     /* frase note value */
        fv->fincv[i] = 0;     /* frase increment value */
        fv->fintv[i] = 0;     /* frase timecode interval */
        fv->fcval[i] = 0;     /* frase current value */
        fv->fctin[i] = 0;     /* frase current timer interval */
        fv->fudfg[i] = 0;     /* frase up down flag */
        fv->factf[i] = 0;     /* frase activ on/off flag */
    }
    getmexrdf_mex(dfile);
    initactp_mex();
    chkactb_mex();
    if(ls->act232c) emu232c_mex(); /* 232c setup */
    prtmode_mex();
    fpi = fopen_h(dfile,"rb");
    for(k = 0; k < MAXPT; k++) {
        for(i = 0; i < 16; i++) {
            for(j = 0; j < 128; j++) notef[k][i][j] = 0;
        }
    }
    for(i = 0; i < MAXPT; i++) aport[i] = 0;
    swbrkf = 0;           /* switch break flag */
    tmptbset_mex();
    timeset();
    sprintf(msg,"Mex quick play start %s %s file \"%s\"",
        su->date,su->time,dfile);
    dputf_mex(2,4,msg);
    su->counter = 0;
    while((c = fgetc(fpi)) != EOF) {
        iword[0] = c;
        iword[1] = fgetc(fpi);
        iword[2] = fgetc(fpi);
        iword[3] = fgetc(fpi);
        compchk_mex(&u,iword);
        switch (c) {
            case  0xff : ckport = (iword[1] & 0x00f0)/16;
                         if(ckport > MAXPT) {
                             sprintf(msg,"Error! port number over than %d",
                                 MAXPT);
                             dputf_mex(2,6,msg);
                             fclose(fpi);
                             sprintf(msg,"Mex quick play end %s",su->time);
                             dputf_mex(2,7,msg);
                             return(-1);
                         }
                         break;
            case  0xfa : fadecode_mex(&u);
                         tempo = (float)tmptb[lv->tsym] *
                             (float)lv->temp / 10000000.;
                         if(ls->debug == 2)
                             sprintf(msg,"lv->tsym=%d tempo=%7.2f",
                             lv->tsym,tempo);
                             dputf_mex(2,6,msg);
                         break;
            case  0xfb : fbdecode_mex(&u);
                         if(ls->brself) {
                            pstop = 0;
                            if(ls->brself == 1 &&
                                lv->movn > lv->esequ) pstop = 1;
                            if(ls->brself == 2 &&
                                lv->barn == lv->ebarn) pstop = 1;
                            if(ls->brself == 3) if(lv->movn > lv->esequ ||
                                lv->barn == lv->ebarn) pstop = 1;
                         }
                         if(ls->monitor == 1) {
                             sprintf(msg,"\r%02d%05d ",lv->movn,lv->barn);
                             wprintf_mex(msg);
                         }
                         if(ls->monitor == 2) {
                            boc = moutc - boc;
                            sprintf(msg,"\r%02d%05d%5ld ",
                                lv->movn,lv->barn,boc);
                            wprintf_mex(msg);
                         }
                         if(ls->brself) {
                            if(ls->brself == 1 &&
                                lv->movn == lv->ssequ) moutf = 1;
                            if(ls->brself == 2 &&
                                lv->barn == lv->sbarn) moutf = 1;
                            if(ls->brself == 3) if(lv->movn == lv->ssequ &&
                                lv->barn == lv->sbarn) moutf = 1;
                            ls->play = moutf; 
                         }
                         lagfm = 0.0;
                         if(sync > 0) {
                             fsync = 60.0 * (float)sync / tempo;
                             /* lagfm = 120.0 * fsync / 100000.0; */
                             lagfm = 0.0012 * fsync;
                         }
                         if(ls->debug == 1) {
                             sprintf(msg,"sync=%ld lagfm=%9.2f\n",sync,lagfm);
                             gprintf_mex(msg);
                         }
                         su->subclk = (int)lagfm;
                         if(ls->play) {
                             while(su->subclk > 0) {
                                 if(su->f1act) for(j = 0; j < 4; j++)
                                     if(fv->factf[j]) flasing_mex(j);
                                 su->f1act = 0;
                                 pbreak_mex();
                             }
                         }
                         break;
            case  0xfc : if(!moutf) break;
                         fcdecode_mex(&u);
                         dsyn = sync - lv->sync;
                         lagfm = 0.0;
                         if(dsyn > 0) {
                             fsync = 60.0 * (float)dsyn / tempo;
                             /* lagfm = 120.0 * fsync / 100000.0; */
                             lagfm = 0.0012 * fsync;
                         }
                         if(ls->debug == 1) {
                             sprintf(msg,
                               "lv->sync=%ld sync=%ld dsyn=%ld lagfm=%9.2f\n",
                               lv->sync,sync,dsyn,lagfm);
                             gprintf_mex(msg);
                         }
                         sync = lv->sync;
                         su->subclk = (int)lagfm;
                         if(ls->play) {
                             while(su->subclk > 0) {
                                 if(su->f1act) for(j = 0; j < 4; j++)
                                     if(fv->factf[j]) flasing_mex(j);
                                 su->f1act = 0;
                                 pbreak_mex();
                             }
                         }
                         break;
            case  0xe1 : if(!moutf) break;
                         e1decode_mex(&u);
                         break;
            case  0xe2 : if(!moutf) break;
                         e2decode_mex(&u);
                         break;
            case  0xe3 : if(!moutf) break;
                         e3decode_mex(&u);
                         f = fv->ftype;
                         dv = fv->feval[f] - fv->fsval[f];
                         fv->fudfg[f] = 1;
                         if( dv < 0 ) {
                             fv->fudfg[f] = 2;
                             dv = fv->fsval[f] - fv->feval[f];
                         }
                         qf = sflame * 4;
                         qfs = (fv->fnval[f] * qf) / 100000;
                         df = (qfs * 6000) / lv->temp;
                         if( dv > df ) {
                             fv->fincv[f] = dv/df;
                             fv->fintv[f] = 1;
                         }
                         if( dv < df ) {
                             fv->fintv[f] = df/dv;
                             fv->fincv[f] = 1;
                         }
                         fv->factf[f] = 1;
                         if(ls->debug == 1) {
                             sprintf(msg,"dv=%ld qfs=%ld df=%ld ",dv,qfs,df);
                             gprintf_mex(msg);
                             sprintf(msg,"fincv[%d]=%d fintv[%d]=%d ",
                                 f,fv->fincv[f],f,fv->fintv[f]);
                             gprintf_mex(msg);
                             sprintf(msg,"fnval[%d]=%ld\n",f,fv->fnval[f]);
                             gprintf_mex(msg);
                         }
                         break;
            case  0xfd : if(!moutf) break;
                         midiout_mex(iword[1]);
                         midiout_mex(iword[2]);
                         midiout_mex(iword[3]);
                         k = iword[1] & 0xf0;
                         if(k == 0x90) {
                             chan = iword[1] & 0x0f;
                             notep = iword[2] & 0x7f;
                             velo = iword[3] & 0x7f; /* velo */
                             if(velo == 0) notef[ckport][chan][notep]--;
                             else notef[ckport][chan][notep]++;
                             aport[ckport] = 1;
                         }
                         break;
            default    : break;
        }
        if(swbrkf) break;
        if(pstop) break;
    }
    sprintf(msg,"Out=%ld Counter=%ld",moutc,su->counter);
    dputf_mex(2,6,msg);
    midistop_mex();
    timeset();
    sprintf(msg,"Mex quick play end %s",su->time);
    dputf_mex(2,7,msg);
    fclose(fpi);
    return(0);
}

tmptbset_mex()
{
    tmptb[0] = 800000;       /* |O|         */
    tmptb[1] = 700000;       /* O..         */
    tmptb[2] = 600000;       /* O.          */
    tmptb[3] = 400000;       /* O           */
    tmptb[4] = 350000;       /* o/..        */
    tmptb[5] = 300000;       /* o/.         */
    tmptb[6] = 200000;       /* o/          */
    tmptb[7] = 175000;       /* _/..        */
    tmptb[8] = 150000;       /* _/.         */
    tmptb[9] = 100000;       /* _/          */
    tmptb[10] = 87500;       /* _/)..       */
    tmptb[11] = 75000;       /* _/).        */
    tmptb[12] = 50000;       /* _/)         */
    tmptb[13] = 43750;       /* _/))..      */
    tmptb[14] = 37500;       /* _/)).       */
    tmptb[15] = 25000;       /* _/))        */
    tmptb[16] = 21875;       /* _/)))..     */
    tmptb[17] = 18750;       /* _/))).      */
    tmptb[18] = 12500;       /* _/)))       */
    tmptb[19] =  9375;       /* _/)))).     */
    tmptb[20] =  6250;       /* _/))))      */
    tmptb[21] =  3125;       /* _/)))))     */
    return(0);
}
/*
 Make program change & pitch vend code [20 < 80 _/]
    int  ftype;         frase type
    int  fchan[4];      frase channel
    int  fsval[4];      frase start value
    int  fctno[4];      frase control number
    int  feval[4];      frase end value
    long fnval[4];      frase note value
    int  fincv[4];      frase increment value
    int  fintv[4];      frase timecode interval
    int  fcval[4];      frase current value
    int  fctin[4];      frase current timer interval
    int  fudfg[4];      frase up down flag
    int  factf[4];      frase activ on/off flag
*/
flasing_mex(f)
int  f;
{
    int  fctn;

    fctn = fv->fctno[f];
    if(fv->fudfg[f] == 0 && fv->factf[f]) {
        _putstr("Error! at frasing_mex()\n");
        return(0);
    }
    if(fv->factf[f] == 1) {
        fv->fcval[f] = fv->fsval[f];  /* set start val */
        fv->factf[f] = 2;
    }
    else fv->factf[f] = 2;
    if(fv->fudfg[f] == 1) {  /* value up frase */
        if(fv->fintv[f] == 1) {
            flasout_mex(f);
            fv->fcval[f] += fv->fincv[f];
        }
        else {
            if(!(fv->fctin[f] % fv->fintv[f])) {
                flasout_mex(f);
                fv->fcval[f] += fv->fincv[f];
                fv->fctin[f] = 0;
            }
            fv->fctin[f]++;
        }
        if(fv->fcval[f] >= fv->feval[f]) {
            fv->factf[f] = 0;          /* act off */
            fv->fctin[f] = 0;          /* clear current interval counter */
            if(fctn > 101) fctn = 0;   /* pitch vend (150) */
            fjtb[fctn] = 0;            /* clear flame jump counter */
        }
    }
    if(fv->fudfg[f] == 2) {  /* value down frase */
        if(fv->fintv[f] == 1) {
            flasout_mex(f);
            fv->fcval[f] -= fv->fincv[f];
        }
        else {
            if(!(fv->fctin[f] % fv->fintv[f])) {
                flasout_mex(f);
                fv->fcval[f] -= fv->fincv[f];
                fv->fctin[f] = 0;
            }
            fv->fctin[f]++;
        }
        if(fv->fcval[f] <= fv->feval[f]) {
            fv->factf[f] = 0;          /* act off */
            fv->fctin[f] = 0;          /* clear current interval counter */
            if(fctn > 101) fctn = 0;   /* pitch vend (150) */
            fjtb[fctn] = 0;            /* clear flame jump counter */
        }
    }
    return(0);
}

/* frasing code out */
/* Controller numbers table
   MODUR 1          modulation depth
   POLTM 5          poltament time
   DENTR 6          data entry (RPN/NRPN)
   VOLUM 7          main volume
   BALAN 8          balance control
   PANPT 10         panpot
   EXPRS 11         expression
   DUMPR 64         hold 1 (dumper)
   POLTA 65         poltament
   SOSTN 66         sosutenute (chord hold)
   SOFTP 67         soft pedal
   HOLD2 69         hold2 (flease)
   EXTEF 91         external efect depth
   TREMO 92         toremolo depth
   CHORS 93         chorus depth
   SELES 94         seleste depth
   FETHR 95         feather depth
*/
flasout_mex(f)
int  f;   /* flase */
{
    int  fctn;

    fctn = fv->fctno[f];
    /* _dprintf("fv->fcval[%d]=%d\n",f,fv->fcval[f]); */
    switch (fctn) {
        case MODUR : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case POLTM : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case DENTR : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case VOLUM : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case BALAN : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case PANPT : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case EXPRS : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case DUMPR : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case POLTA : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case SOSTN : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case SOFTP : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case HOLD2 : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case EXTEF : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case TREMO : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case CHORS : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case SELES : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case FETHR : if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
        case PITCH : if(!(fjtb[0] % 4)) pitch_mex(f);
                     fjtb[0]++;
                     break;
        default    : if(fctn > 102) {
                         sprintf(msg,"Error! control no. %d\n",fctn);
                         gprintf_mex(msg);
                         break;
                     }
                     if(!(fjtb[fctn] % 4)) flcout_mex(f);
                     fjtb[fctn]++;
                     break;
    }
    return(0);
}

flcout_mex(f)
int  f;
{
    int  bit14;
    int  bx;
    int  cno;
    int  msb;
    int  lsb;
    static int wmsb = 0;

    bit14 = 0;
    if((fv->fsval[f] > 127) || (fv->feval[f] > 127)) bit14 = 1;
    if(bit14) {   /* 14 bit code */
        bx = 0x0f & fv->fchan[f];
        bx |= 0x00b0;
        msb = fv->fcval[f] / 128;  /* MSB */
        cno = 0x7f & fv->fctno[f];
        if(msb != wmsb) {
            wmsb = msb;
            midiout_mex((char)bx);   /* Bx */
            midiout_mex((char)cno);  /* control # */
            midiout_mex((char)msb);
        }
        midiout_mex((char)bx);
        midiout_mex((char)(cno+32));
        lsb = fv->fcval[f] - (msb * 128);  /* LSB */
        midiout_mex((char)lsb);
    }
    else {
        bx = 0x0f & fv->fchan[f];
        bx |= 0x00b0;
        midiout_mex((char)bx);       /* Bx */
        cno = 0x7f & fv->fctno[f];
        midiout_mex((char)cno);      /* control # */
        msb = 0x007f & fv->fcval[f]; /* MSB */
        midiout_mex((char)msb);
    }
    return(0);
}

pitch_mex(f)     /* pitch vend */
int  f;
{
    int  ex;
    int  msb;
    int  lsb;

    ex = 0x0f & fv->fchan[f];
    ex |= 0x00e0;
    msb = fv->fcval[f] / 128;  /* MSB */
    midiout_mex((char)ex);   /* Ex */
    midiout_mex((char)msb);
    lsb = fv->fcval[f] - (msb * 128);  /* LSB */
    midiout_mex((char)lsb);
    return(0);
}

/* nomal key */
mportset_mex(k)
char k;
{
    switch(k) {   /* Port set */
        case 0x00 : kv->xport = 88;        /* ESC */
                    kv->gmode = 88;
                    kv->kmflg = 0;
                    ls->monitor = 1;
                    ls->curclk = ls->orgclk;
                    chgtimert_mex();
                    _putstr("Clear_all");
                    return(0);
        case 0x61 : kv->xport = 0;         /* COPY */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x62 : kv->xport = 1;         /* f.1 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x63 : kv->xport = 2;         /* f.2 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x64 : kv->xport = 3;         /* f.3 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x65 : kv->xport = 4;         /* f.4 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x66 : kv->xport = 5;         /* f.5 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x67 : kv->xport = 6;         /* f.6 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x68 : kv->xport = 7;         /* f.7 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x69 : kv->xport = 8;         /* f.8 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x6a : kv->xport = 9;         /* f.9 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x6b : kv->xport = 10;        /* f.10 */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x36 : kv->xport = 11;        /* ROLL UP */
                    kv->kmflg = 1;         /* port select start */
                    break;
        case 0x37 : kv->xport = 12;        /* ROLL DOWN */
                    kv->kmflg = 1;         /* port select start */
                    break;
        default   : return(1);
    }
    kv->sport = kv->xport;
    sprintf(msg,"Port_%d",kv->xport);
    gprintf_mex(msg);
    return(0);
}

shiftkey(k)
char k;
{
    switch(k) {   /* Port set */
        case 0x62 : kv->schan = 1;         /* f.1 */
                    break;
        case 0x63 : kv->schan = 2;         /* f.2 */
                    break;
        case 0x64 : kv->schan = 3;         /* f.3 */
                    break;
        case 0x65 : kv->schan = 4;         /* f.4 */
                    break;
        case 0x66 : kv->schan = 5;         /* f.5 */
                    break;
        case 0x67 : kv->schan = 6;         /* f.6 */
                    break;
        case 0x68 : kv->schan = 7;         /* f.7 */
                    break;
        case 0x69 : kv->schan = 8;         /* f.8 */
                    break;
        case 0x6a : kv->schan = 9;         /* f.9 */
                    break;
        case 0x6b : kv->schan = 10;        /* f.10 */
                    break;
        case 0x36 : kv->schan = 11;        /* ROLL UP */
                    break;
        case 0x37 : kv->schan = 12;        /* ROLL DOWN */
                    break;
        case 0x38 : kv->schan = 13;        /* INS */
                    break;
        case 0x39 : kv->schan = 14;        /* DEL */
                    break;
        case 0x3e : kv->schan = 15;        /* HOME CLR */
                    break;
        case 0x3f : kv->schan = 16;        /* HELP */
                    break;
        default   : return(1);
    }
    sprintf(msg,"Channel_%d",kv->schan);
    gprintf_mex(msg);
    return(0);
}

grphkey(k)
char k;
{
    int  kvcler;

    kvcler = 0;
    switch(k) {
        case 0x61 : kv->gmode = 0;         /* COPY */
                    break;
        case 0x62 : kv->gmode = 1;         /* f.1 */
                    kvcler = 1;
                    _putstr("Program_change");
                    break;
        case 0x63 : kv->gmode = 2;         /* f.2 */
                    kvcler = 1;
                    _putstr("Modulation");
                    break;
        case 0x64 : kv->gmode = 3;         /* f.3 */
                    kvcler = 1;
                    _putstr("Pitch_vend");
                    break;
        case 0x65 : kv->gmode = 4;         /* f.4 */
                    kvcler = 1;
                    _putstr("Volume");
                    break;
        case 0x66 : kv->gmode = 5;         /* f.5 */
                    kvcler = 1;
                    _putstr("Controle");
                    break;
        case 0x67 : ls->monitor = 0;       /* f.6 */
                    _putstr("Monitor_0");
                    break;
        case 0x68 : ls->monitor = 1;       /* f.7 */
                    _putstr("Monitor_1");
                    break;
        case 0x69 : ls->monitor = 2;       /* f.8 */
                    _putstr("Monitor_2");
                    break;
        case 0x6a : kv->gmode = 9;         /* f.9 */
                    break;
        case 0x6b : kv->gmode = 10;        /* f.10 */
                    break;
        case 0x36 : kv->gmode = 11;        /* ROLL UP */
                    _putstr("Midi_monitor(out)");
                    break;
        case 0x37 : kv->gmode = 12;        /* ROLL DOWN */
                    _putstr("Midi_monitor(in)");
                    break;
        case 0x38 : kv->gmode = 13;        /* INS */
                    break;
        case 0x39 : kv->gmode = 14;        /* DEL */
                    break;
        case 0x3e : kv->gmode = 15;        /* HOME CLR */
                    _tcls();
                    break;
        case 0x51 : kv->klock = 1;         /* NFER */
                    _putstr("key_lock(enable:G_XFER)\n");
                    return(0);
        default   : return(1);
    }
    if(kvcler) kvinit_mex();
    return(0);
}

keyhelp_mex()
{
  char c;

  dsppage(PAGE_2);
  actpage(PAGE_2);
  _tcls();
  _putstr(" <<< Tact key >>>  ESC:Exit help\n");
  helpkmap_mex();

  _dprintf("%s\n%s\n%s\n%s\n%s\n%s\n",
  "__________________________________________________________________________",
  "| [S]   STOP     :Exit this program  | [C]    COPY     :Port 0           |",
  "| [f1]  f.1      :Port 1             | [f2]   f.2      :Port 2           |",
  "| [f3]  f.3      :Port 3             | [f4]   f.4      :Port 4           |",
  "| [f5]  f.5      :Port 5             | [f6]   f.6      :Port 6           |",
  "| [f7]  f.7      :Port 7             | [f8]   f.8      :Port 8           |");
  _dprintf("%s\n%s\n%s\n%s\n",
  "| [f9]  f.9      :Port 9             | [f10]  f.10     :Port 10          |",
  "| [U]   ROLL UP  :Port 11            | [D]    ROLL DOWN:Port 12          |",
  "| [ES]  ESC      :Clear_all          | [Space]         :Pose/Continue    |",
  "| [I]   INS      :                   | [0]    Ten key  :Value reset      |");
  _dprintf("%s\n%s\n%s\n%s\n%s\n",
  "| [R]   HOME CLR :All note off       | [H]    HELP     :Help             |",
  "| [UP]  Cursor   :Value up  +1       | [DOWN] Cursor   :Value down -1    |",
  "| [<]   Cursor   :Value down (-10)   | [>]    Cursor   :Value up (+10)   |",
  "| [XFER]         :Midi message check | [CR]   return   :Play continue    |",
  "--------------------------------------------------------------------------");
  while((c = keyin()) == -1);
  if(c == 0x00) {
      _tcls();
      dsppage(PAGE_1);
      actpage(PAGE_1);
      return(0);
  }
  helpskey();
  while((c = keyin()) == -1);
  if(c == 0x00) {
      _tcls();
      dsppage(PAGE_1);
      actpage(PAGE_1);
      return(0);
  }
  helpgkey();
  while((c = keyin()) == -1);
  _tcls();
      dsppage(PAGE_1);
      actpage(PAGE_1);
  return(0);
}

helpkmap_mex()
{
  _dprintf("  %s\n\n",
  "[S] [C] [f1][f2][f3][f4][f5] [f6][f7][f8][f9][f10] [U][W]");
  _dprintf("  %s\n",
  "[ES][.][.][.][.][.][.][.][.][.][.][.][.][.][BS]    [I][D]  [R][H][-][/]");
  _dprintf("  %s\n",
  "[TAB][q][w][e][r][t][y][u][i][o][p][@][[]  ____            [7][8][9][*]");
  _dprintf("  %s\n",
  "[L][P][a][s][d][f][g][h][j][k][l][;][:][]][ CR ]   [ UP ]  [4][5][6][+]");
  _dprintf("  %s\n",
  "[SHIFT][z][x][c][v][b][n][m][,][.][/][ ][SHIFT]    [<][>]  [1][2][3][=]");
  _dprintf("  %s\n",
  "     [k][G][NFER][    space      ][XFER]           [DOWN]  [0][,][.][E]");
  return(0);
}

/* Shift key help */
helpskey()
{
  _tcls();
  _dprintf("   <<< Tact SHIFT key >>>   ESC:Exit help\n");
  helpkmap_mex();

  _dprintf("%s\n%s\n%s\n%s\n%s\n%s\n",
  "__________________________________________________________________________",
  "| [S]   STOP     :                   | [C]    COPY     :                 |",
  "| [f1]  f.1      :Channel 1          | [f2]   f.2      :Channel 2        |",
  "| [f3]  f.3      :Channel 3          | [f4]   f.4      :Channel 4        |",
  "| [f5]  f.5      :Channel 5          | [f6]   f.6      :Channel 6        |",
  "| [f7]  f.7      :Channel 7          | [f8]   f.8      :Channel 8        |");
  _dprintf("%s\n%s\n%s\n%s\n",
  "| [f9]  f.9      :Channel 9          | [f10]  f.10     :Channel 10       |",
  "| [U]   ROLL UP  :Channel 11         | [D]    ROLL DOWN:Channel 12       |",
  "| [ES]  ESC      :                   | [BS]   BS       :                 |",
  "| [I]   INS      :Channel 13         | [D]    DEL      :Channel 14       |");
  _dprintf("%s\n%s\n%s\n%s\n",
  "| [R]   HOME CLR :Channel 15         | [H]    HELP     :Channel 16       |",
  "| [-]   -        :                   | [/]    /        :                 |",
  "| [TAB] TAB      :                   | [L]    CTRL     :                 |",
  "--------------------------------------------------------------------------");
  return(0);
}

/* grph key help */
helpgkey()
{
  _tcls();
  _dprintf("   <<< Tact GRPH key >>>   ESC:Exit help\n");
  helpkmap_mex();

  _dprintf("%s\n%s\n%s\n%s\n%s\n%s\n",
  "__________________________________________________________________________",
  "| [S]   STOP     :                   | [C]    COPY     :                 |",
  "| [f1]  f.1      :Program change     | [f2]   f.2      :Modulation       |",
  "| [f3]  f.3      :Pitch vend         | [f4]   f.4      :Volume           |",
  "| [f5]  f.5      :Controle           | [f6]   f.6      :Monitor 0        |",
  "| [f7]  f.7      :Monitor 1          | [f8]   f.8      :Monitor 2        |");
  _dprintf("%s\n%s\n%s\n%s\n",
  "| [f9]  f.9      :                   | [f10]  f.10     :                 |",
  "| [U]   ROLL UP  :Midi_monitor(out)  | [D]    ROLL DOWN:Midi_monitor(in) |",
  "| [ES]  ESC      :                   | [BS]   BS       :                 |",
  "| [I]   INS      :                   | [D]    DEL      :                 |");
  _dprintf("%s\n%s\n%s\n%s\n%s\n",
  "| [R]   HOME CLR :Clear screen       | [H]    HELP     :                 |",
  "| [TAB]          :                   | [r]             :                 |",
  "| [NFER]         :Keybord lock       | [XFER]          :Keybord enable   |",
  "| [a]            :                   : [n]             :                 |",
  "--------------------------------------------------------------------------");
  return(0);
}

