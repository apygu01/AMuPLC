/* mex1.c */
#include "mex.h"
extern MEXSRG *ls;
extern MEXVAR *lv;
extern MEXACT *la;
extern MEXKEY *kv;           /* key control */
extern SETUPD *su;           /* setup data */
extern KEYFLG *kf;           /* check shift,caps,ctrl,grph */
extern unsigned int sport[16];        /* status port address */
extern unsigned int dport[16];        /* data port address */
extern int  notef[MAXPT][16][128];
extern int  aport[MAXPT];             /* active port */
extern char space[40];
extern long moutc;                    /* output data count */
extern long esuflame;
extern long mtcflame;
extern int  ckport;
extern int  mtclag;
extern int  smph;
extern int  sequ;
extern int  barn;
extern int  smpm;
extern int  smps;
extern int  smpf;
char msg[82];
/* terminal mode */
char shiftcd[4][18];
char nomalcd[4][18];
char kanacd[4][18];
char decdkey();
char ctrlkey();

termode()
{
    char c;
    int  k;
    int  lcount;

    while(1) {
        lcount = 0;
        while(lcount++ != 100 && (c = qretrive()) != -1) {
            if(su->monif) monitor(c);
            /* else _putchr(c); */
            else putchar(c);
            su->counter = 0;
        }
        if(!su->maskflg) {
            if((k = keyin()) != -1) {
                if((c = decdkey(k)) != 0) {
                    rs232tx(c);
                }
                su->counter = 0;
            }
        }
        /* if(su->counter >= 10800 && su->scsvf == 0) savescrn(); 3 minits */
    }
}

/* keytable setup routine */
setupcd()
{
    char *cp[4];

      /*    "0123456789abcdef"       */
    cp[0] = " 1234567890-^\   ";
    cp[1] = "qwertyuiop@[ asd";
    cp[2] = "fghjkl;:]zxcvbnm";
    cp[3] = ",./             ";
    strcpy(nomalcd[0],cp[0]);
    strcpy(nomalcd[1],cp[1]);
    strcpy(nomalcd[2],cp[2]);
    strcpy(nomalcd[3],cp[3]);

    cp[0] = " !\"#$%&'() =^|  ";
    cp[1] = "QWERTYUIOP~{ ASD";
    cp[2] = "FGHJKL+*}ZXCVBNM";
    cp[3] = "<>?_            ";
    strcpy(shiftcd[0],cp[0]);
    strcpy(shiftcd[1],cp[1]);
    strcpy(shiftcd[2],cp[2]);
    strcpy(shiftcd[3],cp[3]);

    cp[0] = " ÇÌ±³´µÔÕÖÜÎÍŽ°   ";
    cp[1] = "ÀÃ²½¶ÝÅÆ×¾Þß ÁÄŽ¼";
    cp[2] = "Ê·¸ÏÉØÚ¹ÑÂ»¿ËºÐÓ";
    cp[3] = "ÈÙÒÛ            ";
    strcpy(kanacd[0],cp[0]);
    strcpy(kanacd[1],cp[1]);
    strcpy(kanacd[2],cp[2]);
    strcpy(kanacd[3],cp[3]);
    return(0);
}

char decdkey(k)
char k;
{
    int  k1,k2;
    char c;

    if(k == 0x1c) return(0x0d);       /* CR */
    if(k == 0x34) return(0x20);       /* space */
    k1 = (k & 0xf0) >> 4;
    k2 = k & 0x0f;
    if(kf->shift) {
        c = shiftcd[k1][k2];
        return(c);
    }
    if(kf->kana) {
        c = kanacd[k1][k2];
        return(c);
    }
    if(k == 0x0e) return(BS);         /* BS */
    if(k == 0x00) return(0);          /* ESC */
    if(k == 0x0f) return(HT);         /* TAB:HT */
    if(k == 0x34) return(0);          /* NFER ? */
    if(k == 0x35) return(0);          /* XFER */
    if(k == 0x36) return(0);          /* ROLL UP */
    if(k == 0x37) return(0);          /* ROLL DOWN */
    if(k == 0x38) return(0);          /* INS */
    if(k == 0x39) return(DEL);        /* DEL */
    if(k == 0x3a) return(0x0b);       /* Cursor up */
    if(k == 0x3d) return(0x0a);       /* Cursor down */
    if(k == 0x3b) return(0x08);       /* Cursor Left */
    if(k == 0x3c) return(0x0c);       /* Cursor right */
    if(k == 0x61) return(0);          /* COPY:break */
                                  /* ten keys */
    if(k == 0x4e) return('0');    /* 0 */
    if(k == 0x4a) return('1');    /* 1 */
    if(k == 0x4b) return('2');    /* 2 */
    if(k == 0x4c) return('3');    /* 3 */
    if(k == 0x46) return('4');    /* 4 */
    if(k == 0x47) return('5');    /* 5 */
    if(k == 0x48) return('6');    /* 6 */
    if(k == 0x42) return('7');    /* 7 */
    if(k == 0x43) return('8');    /* 8 */
    if(k == 0x44) return('9');    /* 9 */
    if(k == 0x3e) return(0);      /* HOME CLR */
    if(k == 0x3f) return(0);      /* HELP */
    if(k == 0x40) return('-');    /* - */
    if(k == 0x41) return('/');    /* / */
    if(k == 0x45) return('*');    /* * */
    if(k == 0x49) return('+');    /* + */
    if(k == 0x4d) return('=');    /* = */
    if(k == 0x4f) return(',');    /* , */
    if(k == 0x50) return('.');    /* . */
    c = nomalcd[k1][k2];
    if(k == 0x60) {        /* stop */
        if(ls->vectf) resetvec();
        if(ls->blibf) tterm();  /* Close Text Window Boyo Lib */
        exit(0);
    }
    if(kf->ctrl) {
        c = ctrlkey(k);
        if(c) return(c);
    }
    return(c);
}

char ctrlkey(k)
char k;
{
    if(k == 0x1d) return(0x01);   /* A:SH */
    if(k == 0x2d) return(0x02);   /* B:SX */
    if(k == 0x2b) return(0x03);   /* C:EX */
    if(k == 0x1f) return(0x04);   /* D:ET */
    if(k == 0x12) return(0x05);   /* E:EQ */
    if(k == 0x20) return(0x06);   /* F:AK */
    if(k == 0x21) return(0x07);   /* G:BL */
    if(k == 0x22) return(0x08);   /* H:BS */
    if(k == 0x17) return(0x09);   /* I:HT */
    if(k == 0x23) return(0x0a);   /* J:LF */
    if(k == 0x24) return(0x0b);   /* K:HM */
    if(k == 0x25) return(0x0c);   /* L:CL */
    if(k == 0x2f) return(0x0d);   /* M:CR */
    if(k == 0x2e) return(0x0e);   /* N:SO */
    if(k == 0x18) return(0x0f);   /* O:SI */
    if(k == 0x19) return(0x10);   /* P:DE */
    if(k == 0x10) return(0x11);   /* Q:D1 */
    if(k == 0x13) return(0x12);   /* R:D2 */
    if(k == 0x1e) return(0x13);   /* S:D3 */
    if(k == 0x14) return(0x14);   /* T:D4 */
    if(k == 0x16) return(0x15);   /* U:NK */
    if(k == 0x2c) return(0x16);   /* V:SN */
    if(k == 0x11) return(0x17);   /* W:EB */
    if(k == 0x2a) return(0x18);   /* X:CN */
    if(k == 0x15) return(0x19);   /* Y:EM */
    if(k == 0x29) return(0x1a);   /* Z:SB */
    if(k == 0x1b) return(0x1b);   /* [:EC */
    return(0);
}


/* XFER mode switch */
xfermdsw_mex()
{
    long l;
    int  port;
    int  chan;

    port = kv->sport;
    chan = kv->schan;
    switch(kv->gmode) {
        case 88 : esuflame = smph*3600 + smpm*60 + smps;
                  esuflame = esuflame * 30 + smpf;
                  mtcflame = su->qhour*3600 + su->qmin*60 + su->qsec;
                  mtcflame = mtcflame * 30 + su->qflame + mtclag;
                  l = mtcflame - esuflame;
                  posprint_mex();
                  sprintf(msg,"Clock=%d MTC - ESU = %ld",ls->curclk,l);
                  dputf_mex(2,6,msg);
                  break;
        case  1 : posprint_mex();
                  sprintf(msg,"Program change port=%d channel=%d value=%d",
                      port,chan,kv->prchg[port]);
                  dputf_mex(2,6,msg);
                  break;
        case  2 : posprint_mex();
                  sprintf(msg,"Modulation port=%d channel=%d value=%d",
                      port,chan,kv->modur[port]);
                  dputf_mex(2,6,msg);
                  break;
        case  4 : posprint_mex();
                  sprintf(msg,"Main volume port=%d channel=%d value=%d",
                      port,chan,kv->volum[port]);
                  dputf_mex(2,6,msg);
                  break;
        default : break;
    }
    return(0);
}

upmdsw_mex()   /* Cursor up */
{
    switch(kv->gmode) {
        case 88 : ls->curclk = ls->curclk - 1;
                  chgtimert_mex();
                  break;
        case  1 : kv->prchg[kv->sport]++;     /* program change */
                  if(kv->prchg[kv->sport] > 127) kv->prchg[kv->sport] = 0;
                  send2msg_mex(kv->prchg[kv->sport]);
                  break;
        case  2 : kv->modur[kv->sport]++;     /* modulation */
                  if(kv->modur[kv->sport] > 127) kv->modur[kv->sport] = 0;
                  contlmsg_mex(1,kv->modur[kv->sport]);
                  break;
        case  4 : kv->volum[kv->sport]++;     /* main volume */
                  if(kv->volum[kv->sport] > 127) kv->volum[kv->sport] = 0;
                  contlmsg_mex(7,kv->volum[kv->sport]);
                  break;
        default : break;
    }
    return(0);
}

downmdsw_mex()   /* Cursor down mode switch */
{
    switch(kv->gmode) {
        case 88 : ls->curclk = ls->curclk + 1;
                  chgtimert_mex();
                  break;
        case  1 : kv->prchg[kv->sport]--;  /* program change */
                  if(kv->prchg[kv->sport] < 0) kv->prchg[kv->sport] = 127;
                  send2msg_mex(kv->prchg[kv->sport]);
                  break;
        case  2 : kv->modur[kv->sport]--;  /* modulation */
                  if(kv->modur[kv->sport] < 0) kv->modur[kv->sport] = 127;
                  contlmsg_mex(1,kv->modur[kv->sport]);
                  break;
        case  4 : kv->volum[kv->sport]--;     /* main volume */
                  if(kv->volum[kv->sport] < 0) kv->volum[kv->sport] = 127;
                  contlmsg_mex(7,kv->volum[kv->sport]);
                  break;
        default : break;
    }
    return(0);
}

leftmdsw_mex()   /* Cursor left mode switch */
{
    switch(kv->gmode) {
        case 88 : ls->curclk = ls->curclk + 100;
                  chgtimert_mex();
                  break;
        case  1 : kv->prchg[kv->sport] = kv->prchg[kv->sport] - 10;
                  if(kv->prchg[kv->sport] < 0) kv->prchg[kv->sport] = 127;
                  send2msg_mex(kv->prchg[kv->sport]);  /* program change */
                  break;
        case  2 : kv->modur[kv->sport] = kv->modur[kv->sport] - 10;
                  if(kv->modur[kv->sport] < 0) kv->modur[kv->sport] = 127;
                  contlmsg_mex(1,kv->modur[kv->sport]);
                  break;
        case  4 : kv->volum[kv->sport] = kv->volum[kv->sport] - 10;
                  if(kv->volum[kv->sport] < 0) kv->volum[kv->sport] = 127;
                  contlmsg_mex(7,kv->volum[kv->sport]);
                  break;
        default : break;
    }
    return(0);
}

rightmdsw_mex()   /* Cursor right mode switch */
{
    switch(kv->gmode) {
        case 88 : ls->curclk = ls->curclk - 100;
                  chgtimert_mex();
                  break;
        case  1 : kv->prchg[kv->sport] = kv->prchg[kv->sport] + 10;
                  if(kv->prchg[kv->sport] > 127) kv->prchg[kv->sport] = 0;
                  send2msg_mex(kv->prchg[kv->sport]);  /* program change */
                  break;
        case  2 : kv->modur[kv->sport] = kv->modur[kv->sport] + 10;
                  if(kv->modur[kv->sport] > 127) kv->modur[kv->sport] = 0;
                  contlmsg_mex(1,kv->modur[kv->sport]);
                  break;
        case  4 : kv->volum[kv->sport] = kv->volum[kv->sport] + 10;
                  if(kv->volum[kv->sport] > 127) kv->volum[kv->sport] = 0;
                  contlmsg_mex(7,kv->volum[kv->sport]);
                  break;
        default : break;
    }
    return(0);
}


tk0mdsw_mex()
{
    switch(kv->gmode) {
        case 88 : ls->curclk = ls->orgclk;
                  chgtimert_mex();
                  break;
        case  1 : kv->prchg[kv->sport] = 0;
                  send2msg_mex(kv->prchg[kv->sport]);  /* program change */
                  break;
        case  2 : kv->modur[kv->sport] = 0;
                  contlmsg_mex(1,kv->modur[kv->sport]);
                  break;
        case  4 : kv->volum[kv->sport] = 100;
                  contlmsg_mex(7,kv->volum[kv->sport]);
                  break;
        default : break;
    }
    return(0);
}

send2msg_mex(v)
int  v;
{
    int  k;
    char c;

    k = (kv->schan - 1) & 0x0f;
    c = 0xc0 | (char)k;
    k = kv->sport & 0x0f;
    midiclr_mex(k,c);
    c = 0x7f & (char)v;   /* value */
    midiclr_mex(k,c);
    sprintf(msg,".%d.",v);
    wprintf_mex(msg);
    return(0);
}

/* control message */
contlmsg_mex(n,v)
int  n;     /* control number */
int  v;     /* control value */
{
    int  k;
    char c;

    k = (kv->schan - 1) & 0x0f;
    c = 0xb0 | (char)k;
    k = kv->sport & 0x0f;
    midiclr_mex(k,c);
    c = 0x7f & (char)n;    /* number */
    midiclr_mex(k,c);
    c = 0x7f & (char)v;    /* value */
    midiclr_mex(k,c);
    sprintf(msg,".%d.",v);
    wprintf_mex(msg);
    return(0);
}

/* midi stop */
midistop_mex()
{
    int  i,j;
    int  m;
    char c1,c2;

    for(m = 0; m < MAXPT ; m++) {
        ckport = m;
        if(aport[m] && la->p_act[m]) {
            moutc = 0;
            for(i = 0; i < 16; i++) {
                for(j = 0; j < 128; j++) {
                    while(notef[m][i][j] > 0) {
                        c1 = 0x90 | (char)i;
                        midiclr_mex(m,c1);
                        c2 = 0x7f & (char)j;
                        midiclr_mex(m,c2);
                        midiclr_mex(m,0x00);
                       /* dprintf("note off send %02x %02x 00\n",c1,c2); */
                        notef[m][i][j]--;
                    }
                }
            }
            if(ls->debug == 2 && moutc) {
                sprintf(msg,"%s%s\r",space,space);
                wprintf_mex(msg);
                sprintf(msg,"port %d note off out=%ld",m,moutc/3);
                wprintf_mex(msg);
            }
        }
    }
    return(0);
}

/* all note off */
anoteoff_mex(k)
int  k;
{                   /* Home clr */
    int  i,j;
    char c1,c2;

    if(!la->p_act[k]) return(0);
    for(i = 0; i < 16; i++) {
        sprintf(msg,"\rNote off port=%2d channel=%2d",k,i);
        wprintf_mex(msg);
        for(j = 0; j < 127; j++) {
            c1 = 0x80 | (char)i;
            midiclr_mex(k,c1);
            c2 = 0x7f & (char)j;
            midiclr_mex(k,c2);
            midiclr_mex(k,0x00);
        }
        c1 = 0xb0 | (char)i;    /* Channel mode message */
        midiclr_mex(k,c1);
        c1 = 0x7f & (char)123;  /* all notes off */
        midiclr_mex(k,c1);
        midiclr_mex(k,0x00);
    }
    return(0);
}

portnoff_mex(k)
int  k;   /* port number */
{
    char c1;
    int  i;

    if(!la->p_act[k]) return(0);
    for(i = 0; i < 16; i++) {
        c1 = 0xb0 | (char)i;    /* Channel mode message */
        midiclr_mex(k,c1);
        c1 = 0x7f & (char)123;  /* all notes off */
        midiclr_mex(k,c1);
        midiclr_mex(k,0x00);
    }
    return(0);
}

midiclr_mex(k,c)
int  k;
char c;
{
    char x;
    int  ack;

    if(!la->p_act[k]) return(0);
    x = c & 0xff;
    while((ack = inp(sport[k])) & 0x40); /* send ok ? */
    outp(dport[k],x);
    if(ls->act232c) {
        if(ls->e232cp == 999) em232tx(x);
        else if(k == ls->e232cp) em232tx(x);
    }
    ack = inp(dport[k]);
    return(ack);
}

posprint_mex()
{
    sprintf(msg,"\r%02d%05d %02d:%02d:%02d/%02d ",sequ,
        barn,su->qhour,su->qmin,su->qsec,su->qflame);
    wprintf_mex(msg);
    return(0);
}

dump_mex(dfile)
char dfile[STLN];
{
    int  c;
    long l;
    int  i,m;
    unsigned char cw[4];
    long oct;
    int  mod;
    FILE *fopen_h(),*fpi;

    i = 0;
    oct = 0;
    mod = 0;
    sprintf(msg,"\n\"%s\" dump start",dfile);
    gprintf_mex(msg);
    l = 0;
    fpi = fopen_h(dfile,"rb");
    while((c = fgetc(fpi)) != EOF) {
        m = i % 4;
        cw[m] = c & 0xff;
        i++;
        if(m == 3) {
            i = 0;
            if(!mod) printf("\n%07ld",oct);
            printf(" %02x%02x%02x%02x",cw[0],cw[1],cw[2],cw[3]);
            l++;
            oct++;
            mod = oct % 8;
        }
    }
    printf("\n\"%s\" input code count = %ld\n",dfile,l);
    fclose(fpi);
    return(0);
}

filemake_mex(n,inf,fext,dfile,yflp,xflp)
int  n;
char inf[STLN];
char fext[STLN];
char dfile[STLN];
int  *yflp;
int  *xflp;
{
    char *pc;
    unsigned char wfnum[8];
    unsigned char wk[8];
    int  i;
    int  trk;
    int  memb;

    trk = n/1000;
    memb = n - trk * 1000;
    *yflp = trk;
    *xflp = memb;
    pc = "0123456789abcdefghijklmnopqrstuvwxyz";
    for(i = 0; i < trk ; i++) pc++;
    wfnum[0] = *pc;
    wfnum[1] = '\0';
    sprintf(wk,"%02x",memb);
    strcat(wfnum,wk);
    dfile[0] = '\0';
    strcat(dfile,inf);
    strcat(dfile,"_");
    strcat(dfile,wfnum);
    strcat(dfile,fext);
    return(0);
}

FILE *fopen_h(file, mode)
char *file;
char *mode;
{
    FILE *fp,*fopen();

    if((fp = fopen(file, mode)) != NULL) return(fp);
    sprintf(msg,"Mex: can't open file %s mode %s\n",file, mode);
    gprintf_mex(msg);
    if(ls->vectf) resetvec();
    if(ls->blibf) tterm();  /* Close Text Window Boyo Lib */
    exit(0);
    return(fp);
}

strcat_h(tos,from)
char *tos,*from;
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

hlpread_mex()
{
    int  c;
    FILE *fopen_h(),*fpi;
    int  i;
    unsigned char k;

    fpi = fopen_h("mexhlp.dat","rb");
    i = 1;
    while((c = fgetc(fpi)) != EOF) {
        putchar(c);
        if(c == 0x0d) {
            i++;
            if((i % 24) == 0) k = getchar();
            if(k == ESC) break;   /* dummy */
        }
    }
    fclose(fpi);
    return(0);
}

dputf_mex(x,y,msg)
int  x;
int  y;
char *msg;
{
    dputs(2,y,76);
    dputf(x,y,"%s",msg);
    return(0);
}

/* Window */
wprintf_mex(msg)
char *msg;
{
    dprintf("%s",msg);
    return(0);
}

/* Gamen */
gprintf_mex(msg)
char *msg;
{
    _dprintf("%s",msg);
    return(0);
}

