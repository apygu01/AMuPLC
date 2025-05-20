#include <stdio.h>
#include <dos.h>
#include <lib\crt.h>
#include <lib\bslib.h>
#include "mex.h"
KEYFLG   *kf;               /* check shift,caps,ctrl,grph */
SETUPD   *su;               /* setup data */
extern   MEXKEY *kv;           /* key control */
extern   MEXSRG *ls;
extern   intime0();         /* timer intterupt 0 assembler routine */
extern   int232c();         /* rs232c intterupt assembler routine */
extern   intkey();          /* keyboud intterupt assembler routine */
extern   int  ckport;
extern   FILE   *fopen_h(),*fpi;
extern   long routc;              /* Rs232c output data count */
extern   char msg[82];
extern   long lscnt;
extern   float lstime;
extern long moutc;                    /* output data count */
extern   long dt_all;      /* mex5.c */

union    REGSS r;           /* for dos call */
union    REGS  regs;
struct   SREGS s;
char     *yobtb[7] = {
    "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};
FILE     *fplog;
int      clock;             /* pc9801 clock */
int      full;              /* recive buffer full flag */
char     *tail;             /* ring buffer tail */
char     *head;             /* ring buffer head */
int      dcflg;             /* xoff level counter */
char     *buffer;           /* buffer address */
char     *top;              /* keyboard top pointer */
char     *bottom;           /* keyboard bottom pointer */
char     *kbuffer;          /* keyboard buffer address */
int      kfull;             /* keyboard buffer full flag */
char     keydata[KDSIZE];   /* keyboard work buffer */
char     *malloc();
char     qretrive();
char     kretrive();
char     SMF_kretrive();
char     keydecd();
unsigned char imrmomsk;     /* IMR master old mask */
unsigned char imrsomsk;     /* IMR slave old mask */
unsigned counter;           /* clock counter */
unsigned tclock;            /* timer clock backup */
void interrupt (far *rvp)(void);  /* rs232c */
void interrupt (far *keyv)(void);  /* key */
void interrupt (far *tvp)(void);  /* timer */

SMF_break_mex()
{
    char c;
    int  k;
    int  lcount;
    int  hh,mm,ss,ff;
    int  dh,dm,ds,df;

    if(!su->maskflg) {
        if((k = SMF_keyin()) != -1) {
            if(k == 0x60) {        /* stop */
                timecv_mex(dt_all,&dh,&dm,&ds,&df);
                timecv_mex(su->countall,&hh,&mm,&ss,&ff);
                lstime = lscnt / 3000.;
                posprint_mex();
                sprintf(msg,
                "dt=%ld %d:%d:%d/%d Clk(lost)=%ld(%ld) %d:%d:%d/%d(%-8.2fsec)",
                dt_all,dh,dm,ds,df,su->countall,lscnt,hh,mm,ss,ff,lstime);
                dputf_mex(2,6,msg);
                midistop_mex();
                if(ls->smpgen || ls->smpread) comout_mex(0xff); /* Esu */
                fclose(fpi);
                if(ls->act232c) {
                    sprintf(msg,"Mex end %s Out=%ld (RS232c=%ld)",
                    su->time,moutc,routc);
                }
                else {
                    sprintf(msg,"Mex end %s Out=%ld",su->time,moutc);
                }
                /* sprintf(msg,"MexSMF_Play end\n",su->time); */
                dputf_mex(2,7,msg);
                gotoxy(0,22);
                resetvec();
                if(ls->blibf) tterm();  /* Close Text Window Boyo Lib */
                exit(0);
            }
        }
    }
    return(0);
}

pbreak_mex()
{
    char c;
    int  k;
    int  lcount;

    lcount = 0;
    while(lcount++ != 50 && (c = qretrive()) != -1) {
        if(su->monif) monitor(c);
    }
    if(!su->maskflg) {
        if((k = keyin()) != -1) {
            if((c = keydecd(k)) != 0) {
                if(c == 0x20) {
                    while(c != 0x0d) {
                        if((k = keyin()) != -1) {
                            c = keydecd(k);
                            if(c == 0x20) break;
                        }
                    }
                }
            }
        }
    }
    return(0);
}

char keydecd(k)
char k;
{
    int  i;
    int  w;

    if(kv->klock) {
        if(kf->grph && k == 0x35) kv->klock = 0;  /* XFER */
        return(0);
    }
    if(kf->shift) {
        shiftkey(k);
        return(k);
    }
    if(kf->grph) {
        grphkey(k);
        return(k);
    }
    if(k == 0x1c) {         /* CR */
        kv->xport = 88;     /* play all */
        /* kv->prchg[kv->sport] = 0; */
        return(0x0d);       /* CR */
    }
    if(k == 0x35) {         /* XFER */
        xfermdsw_mex();
        return(0);
    }
    if(k == 0x3a) {
        upmdsw_mex();   /* Cursor up */
        return(0x0b);
    }
    if(k == 0x3d) {         /* Cursor Down */
        downmdsw_mex();
        return(0x0c);
    }
    if(k == 0x3c) {         /* Cursor Right */
        rightmdsw_mex();
        return(0x0a);
    }
    if(k == 0x3b) {         /* Cursor Left */
        leftmdsw_mex();
        return(0x08);
    }
    if(k == 0x4e) {         /* ten key 0 */
        tk0mdsw_mex();
        return(0);
    }
    if(k == 0x34) {         /* space */
        w = ckport;
        ckport = ls->dtcport;  /* default */
        if(ls->smptp) sposmsg_mex(); /* put position msg */
        ckport = ls->mtcport;  /* default */
        if(ls->smpte) putflmsg_mex();     /* put full time message */
        ckport = w;
        return(0x20);       /* space */
    }
    if(!mportset_mex(k)) return(0);
    if(k == 0x3e) {         /* HOME CLR */
        if(kv->xport == 88) {
            for(i = 0; i < MAXPT; i++) anoteoff_mex(i);
        }
        else anoteoff_mex(kv->xport);
        return(0);
    }
    if(k == 0x60) {        /* stop */
        midistop_mex();
        if(ls->smpgen || ls->smpread) comout_mex(0xff); /* Reset Esu */
        fclose(fpi);
        sprintf(msg,"Mexplay end\n",su->time);
        dputf_mex(2,7,msg);
        /*
        dputf(2,6,"Time %s Out=%ld f1cnt=%ld counter=%ld\n",
            su->time,moutc,f1cnt,su->counter);
        */
        gotoxy(0,22);
        resetvec();
        if(ls->blibf) tterm();  /* Close Text Window Boyo Lib */
        exit(0);
    }
    if(k == 0x3f) {
        keyhelp_mex();
        return(0);                /* HELP */
    }
    return(0);
}

/* * set timer I8253 , at 10MHz 2457600 / 60 = 40960 : 1/60 sec
*/
clockset()
{
    int  k;
    int  v;
    int  vc;

    outp(I8253CM,0x36); /* 8253 mode set (counter0 read/load mode3) */
    v = inp(PRPORTB);   /* get CPU clock */
    vc = v & 0x20;      /* 8 MHz or 10 MHz */
    /*
    if(vc) k = SMPTCLK8 / BCLKINT;
    else k = SMPTCLK / BCLKINT;
    */
    /* real time */
    if(vc) {
        k = 16640;  /* 1996800  / 120 = 16640  8 MHz */
        if(ls->clktyp == 1) k = 16657;  /* smpte flame time */
                    /* 1996800 * 1.001 / 120 = 16656.64 */
        if(ls->clktyp == 3) k = 1996800 / 3000; /* 1/100 flame */
    }
    else {
        k = 20480;    /* 2457600  / 120 = 20480 */
        if(ls->clktyp == 1) k = 20501;  /* smpte flame time */
                      /* 2457600 * 1.001 / 120 = 20500.48 */
        if(ls->clktyp == 3) k = 2457600 / 3000; /*  1/100 flame */
        /* if(ls->clktyp == 3) k = 2457600 / 2950;  1/100(adjust) flame */
    }
    outp_mex(I8253C0,k & 0xff);
    outp(I8253C0,(k >> 8) & 0xff);
    ls->orgclk = k;
    ls->curclk = k;
    /*
    if(ls->play == 3) {
        sprintf(msg,"SMF format Play Timer Clock = %d\n",k);
        dputf_mex(1,10,msg);
    }
    */
    return(0);
}

/* Timer int count check
*/
chktint()
{
    int  i;

    timeset();
    sprintf(msg,"Start Date %s time %s counter = %d\n",
        su->date,su->time,su->counter);
    wprintf_mex(msg);
    for(i = 0; i < 150; i++) {
        timeset();
        sprintf(msg,"End   Date %s time %s su->counter = %d\r",
            su->date,su->time,su->counter);
        wprintf_mex(msg);
    }
    return(0);
}

/* timer0 intrupt routine, called by assembler routine 
*/
timeint0()
{
    su->counter++;
    su->clockflg++;
    su->subclk--;
    su->f1act = 1;    /* smpte f1 code activ flag use only quick mode */
    su->countall++;
    return(0);
}

/* gat date & time */
timeset()
{
    int  year;
    int  month;
    int  date;
    int  youbi;
    int  time;
    int  min;
    int  sec;

    regs.h.ah = 0x2a;      /* get date */
    int86(0x21,&regs,&regs);
    year = regs.x.cx;
    month = regs.h.dh;
    date = regs.h.dl;
    youbi = regs.h.al;
    /* _dprintf("%4d.%d.%d (%s) ",year,month,date,yobtb[youbi]); */
    sprintf(su->date,"%4d.%02d.%02d (%s)\0",year,month,date,yobtb[youbi]);
    regs.h.ah = 0x2c;      /* Get time */
    int86(0x21,&regs,&regs);
    time = regs.h.ch;
    min = regs.h.cl;
    sec = regs.h.dh;
    /* _dprintf("%02d:%02d:%02d\n",time,min,sec); */
    sprintf(su->time,"%02d:%02d:%02d\0",time,min,sec);
    return(0);
}

setupval()
{
    su->monif = 0;          /* monitor */
    su->echo = 0;           /* echo on/off flag */
    su->kecho = 0;          /* key code echo on/off flag */
    su->counter = 0;
    return(0);
}

/* setup rs232c port (Interface Mar. 1986 p.210)
    boud    : 75 - 9600
    datal   : 7 or 8
    parity  : 'n', 'o', 'e'
    stopbit : 1 or 2
*/
set232c(boud,datal,parity,stopbit)
int  boud,datal,stopbit;
char parity;
{
    char c1,c2;
    int  v;
    int  j;
    int  vc;

    parity = toupper(parity);
    if(parity == 'N' || parity == 'E' || parity == 'O');
    else {
        if(datal == 8) parity = 'N';
        else parity = 'E';
    }
    /* peek(0,0x501,&c2,1);     Lattice check memory size */
    c2 = peekb(0,0x501);   /* TC check memory size ? */
    c1 = *(char *)0x500;
    if(ls->debug) {
        sprintf(msg,"memory addr. 0x500 = %02x 0x501 = %02x\n",c1,c2);
        gprintf_mex(msg);
        sprintf(msg,"    (0x500)BIOS flg       (0x501)memory size\n");
        gprintf_mex(msg);
        sprintf(msg,"     bit3 VSYNC            0 : 128k\n");
        gprintf_mex(msg);
        sprintf(msg,"     bit4 extend RAM       1 : 256k\n");
        gprintf_mex(msg);
        sprintf(msg,"     bit5 keyboud over     2 : 384k\n");
        gprintf_mex(msg);
        sprintf(msg,"     bit6 DISKBASIC 0      3 : 512k\n");
        gprintf_mex(msg);
        sprintf(msg,"     bit7 worm(1) cold(0)  4 : 640k\n");
        gprintf_mex(msg);
    }
    /* clock check 8M : 1.9968 MHz, 5M or 10M : 2.4576 MHz */
    v = inp(PRPORTB);
    vc = v & 0x20;     /* vc=1  8 MHz , vc=0  10MHz */
    switch(boud) {
        case 9600 : if(vc) j = 0xd;
                    else  j = 0x10;
                    break;
        case 4800 : if(vc) j = 0x1a;
                    else  j = 0x20;
                    break;
        case 2400 : if(vc) j = 0x34;
                    else  j = 0x40;
                    break;
        case 1200 : if(vc) j = 0x68;
                    else  j = 0x80;
                    break;
        case  600 : if(vc) j = 0xd0;
                    else  j = 0x100;
                    break;
        case  300 : if(vc) j = 0x1a0;
                    else  j = 0x200;
                    break;
        case  200 : if(vc) j = 0x270;
                    else  j = 0x300;
                    break;
        case  150 : if(vc) j = 0x340;
                    else  j = 0x400;
                    break;
        case   75 : if(vc) j = 0x680;
                    else  j = 0x800;
                    break;
        default   : if(vc) j = 0xd;        /* 9600 bps */
                    else  j = 0x10;
                    break;
    }
    outp_mex(I8253CM,0xb6); /* 8253 mode set (counter2 read/load mode3) */
    outp_mex(I8253C2,j & 0xff);
    outp_mex(I8253C2,(j >> 8) & 0xff);
    v = 0;
    if(parity == 'E') v = 0x30;
    if(parity == 'O') v = 0x10;
    if(parity == 'N') v = 0x00;
    if(stopbit == 2) v |= 0xc0;
    else v |= 0x40;
    if(datal == 8) v |= 0xc;
    else v |= 0x8;
    v |= 0x2;      /* x 16 mode */
    outp_mex(I8251S,0xe);
    outp_mex(I8251S,0x40);
    v &= 0xff;
    outp_mex(I8251S,v);
    outp_mex(I8251S,0x37); /* rts on,err reser,tx enable,dtr on,rx enbl */
    return(0);
}

/* set rs232c port for emu232c 31.25 Kbps */
emu232c_mex()
{
    int  v,vc;
    int rate8253;

    /* clock check 8M : 1.9968 MHz, 5M or 10M : 2.4576 MHz */
    v = inp(PRPORTB);
    vc = v & 0x20;            /* vc=1  8 MHz , vc=0  10MHz */
    if(vc) rate8253 = 4;
    else rate8253 = 5;        /* 10M -> 5 (if 8M -> 4) */
    outp_mex(0x37,0);         /* 8255 c port bit0=L Rx_rdy OFF */
    /* 8251 */
    outp_mex(I8251S,0);       /* 8251 set */
    outp_mex(I8251S,0);       /* 0x32 is SIO control */
    outp_mex(I8251S,0);
    outp_mex(I8251S,0x40);    /* reset */
    outp_mex(I8251S,0x4e);    /* stop 1, prty non, ch 8, */
    outp_mex(I8251S,0x35);    /* command 00110101 */
    /* 8253 */
    outp_mex(0x77,0xb6);      /* 0x77 is 8253mode */
    outp_mex(0x75,rate8253);  /* low */
    outp_mex(0x75,0);         /* high */
    outp_mex(0x37,1);         /* 8255 c port bit0=H Rx_rdy ON */
    return(0);
}

/* outp & wait */
outp_mex(o_addr,o_data)
int  o_addr;
unsigned char o_data;
{
    int i;

    outp(o_addr,o_data);
    for(i = 0; i < 500; i++);
    return(0);
}

/* allocate recive buffer
*/
allocbuf()
{
    int  i;

    buffer = malloc(BUFSIZE+2);
    if(buffer == NULL) {
        sprintf(msg,"Revive buffer alloc error\n");
        gprintf_mex(msg);
        return(-1);
    }
    head = buffer;
    tail = buffer;
    if(ls->debug) {
        sprintf(msg,"buffer = %ld head = %ld tail = %ld\n",
            buffer,head,tail);
        gprintf_mex(msg);
    }
    /* tail = buffer + (BUFSIZE/sizeof(char)) - sizeof(char); */

    kbuffer = malloc(KBUFSIZE+2);
    if(kbuffer == NULL) {
        sprintf(msg,"Keyboud buffer alloc error\n");
        gprintf_mex(msg);
        return(-1);
    }
    top = kbuffer;
    bottom = kbuffer;
    /* init keyboard work buffer MSB */
    kf->shift = 0;
    kf->caps = 0;
    kf->kana = 0;
    kf->grph = 0;
    kf->ctrl = 0;
    for(i = 0; i < KDSIZE; i++) keydata[i] = 0x80;
    return(0);
}
chgtimert_mex()
{
    int  k;

    outp_mex(I8253CM,0x36); /* 8253 mode set (counter0 read/load mode3) */
    k = ls->curclk;
    outp_mex(I8253C0,k & 0xff);
    outp_mex(I8253C0,(k >> 8) & 0xff);
    return(0);
}
/* rs232c recive routine, this is called by pcomint.asm routine
*/
pcomrv()
{
    char c;
    int  size;

    c = inp(I8251S);
    if((c & 0x02) != 0) {
        c = inp(I8251D);
        if(!full) {
            size = getsize();
            if(size > OFFLIM1) {
                switch (dcflg) {
                    case   0: rs232tx(XOFF);
                              dcflg++;
                              break;
                    case   1: if(size > OFFLIM2) {
                                  rs232tx(XOFF);
                                  dcflg++;
                              }
                              break;
                    default : if(size > OFFLIM3) rs232tx(XOFF);
                              break;
                }
            }
            *tail = c;
            tail++;
            if(tail == (buffer + BUFSIZE)) tail = buffer;
            if(head == tail) full = TRUE;
        }
    }
    return(0);
}

/* read recive data from buffer
*/
char qretrive()
{
    char c;

    outp_mex(I8259M+2,imrmomsk);         /* master mask reset */
    if(head == tail) {
        if(full) full = FALSE;
        else {
           outp(I8259M+2,IMRMMASK);  /* master 8259 mask set */
           return(-1);
        }
    }
    c = *head;
    head++;
    if(head == (buffer + BUFSIZE)) head = buffer;
    if(dcflg && getsize() < ONLIMIT) {
        rs232tx(XON);
        dcflg = 0;
    }
    outp(I8259M+2,IMRMMASK);         /* master 8259 mask set */
    return(c);
}

/* get recive buffer size
*/
getsize()
{
    int  size;

    if(tail < head) size = BUFSIZE - (head - tail);
    else size = tail - head;
    return(size);
}

/* transmit to rs232c port */
rs232tx(c)
char c;
{
    su->clockflg = 0;
    while((inp(I8251S) & 0x05) != 0x05) {
        if(su->clockflg >= 5) {
            sprintf(msg,"Device RS232C not active\n");
            dputf_mex(2,7,msg);
            break; /* 5/120 sec over */
        }
    }
    outp(I8251D,c);
    return(0);
}

/* transmit to emu232c port */
em232tx(c)
char c;
{
    su->clockflg = 0;
    while((inp(I8251S) & 0x05) != 0x05) {
        if(su->clockflg >= 5) {
            ls->act232c = 0;
            sprintf(msg,"Device EMU232C not active\n");
            dputf_mex(2,7,msg);
            break; /* 5/120 sec over */
        }
    }
    /* while((inp(I8251S) & 0x05) != 0x05); */
    outp(I8251D,c);
    if(ls->monitor == 2) putchr('*');
    if(ls->monitor == 1) if(!(routc % 10)) putchr('*');
    routc++;
    return(0);
}

/* keyboard status check & read keyboard no echo
*/
SMF_keyin()
{
    int  c;

    if((c = SMF_kretrive()) == -1) return(-1);
    return(c);
}

/* keyboard status check & read keyboard no echo
*/
keyin()
{
    int  c;

    if((c = kretrive()) == -1) return(-1);
    return(c);
}

/* keyboud intterupt routine */
pcomkey()
{
    char c;
    int  i;
    int  sflag;

    c = inp(K8251S);
    if((c & 0x38) != 0) outp(K8251S,0x16);  /* 1886.10 comment back */
    c = inp(K8251D);
    if((c & 0x80) == 0) {          /* key on */
        if(c == 0x70) {            /* shift */
            kf->shift = 1;
            return(0);
        }
        if(c == 0x71) {            /* caps */
            kf->caps = 1;
            return(0);
        }
        if(c == 0x72) {            /* kana */
            kf->kana = 1;
            return(0);
        }
        if(c == 0x73) {           /* grph */
            kf->grph = 1;
            return(0);
        }
        if(c == 0x74) {           /* ctrl */
            kf->ctrl = 1;
            return(0);
        }
        for(i = 0; i < KDSIZE; i++) {
            if((keydata[i] & 0x80) != 0) break;
        }
        keydata[i] = c;
        return(0);
    }
    else {                         /* key off */
        c &= 0x7f;
        if(c == 0x70) {            /* shift */
            kf->shift = 0;
            return(0);
        }
        if(c == 0x71) {            /* caps */
            kf->caps = 0;
            return(0);
        }
        if(c == 0x72) {            /* kana */
            kf->kana = 0;
            return(0);
        }
        if(c == 0x73) {           /* grph */
            kf->grph = 0;
            return(0);
        }
        if(c == 0x74) {           /* ctrl */
            kf->ctrl = 0;
            return(0);
        }
        for(i = 0; i < KDSIZE; i++) if(keydata[i] == c) break;
        keydata[i] = c;
    }
    sflag = 0;
    for(i = 0; i < KDSIZE; i++) {
        if((keydata[i] & 0x80) == 0) {
            c = keydata[i];
            keydata[i] = 0x80;
            sflag = 1;
            break;
        }
    }
    if(sflag && kfull == 0) {
        *bottom = c;
        bottom++;
        if(bottom == (kbuffer + KBUFSIZE)) bottom = kbuffer;
        if(top == bottom) kfull = TRUE;
        return(0);
    }
    return(0);
}

/* read keyboard buffer
*/
char SMF_kretrive()
{
    char c;

    /* outp_mex(I8259M+2,0xfd);  master mask */
    if(top == bottom) {
        if(kfull) kfull = FALSE;
        else {
           outp(I8259M+2,IMRMMASK); /* master 8259 mask set */
           return(-1);
        }
    }
    c = *top;
    top++;
    if(top == (kbuffer + KBUFSIZE)) top = kbuffer;
    outp(I8259M+2,IMRMMASK); /* master 8259 mask set */
    return(c);
}

/* read keyboard buffer
*/
char kretrive()
{
    char c;

    outp_mex(I8259M+2,0xfd);          /* master mask */
    if(top == bottom) {
        if(kfull) kfull = FALSE;
        else {
           outp(I8259M+2,IMRMMASK);   /*  master 8259 mask set */
           return(-1);
        }
    }
    c = *top;
    top++;
    if(top == (kbuffer + KBUFSIZE)) top = kbuffer;
    outp(I8259M+2,IMRMMASK);          /*  master 8259 mask set */
    return(c);
}

monitor(c)
int  c;
{

    if(su->monif == 1) {
        if(c == 0x0d || c == 0x0a) {
            putchar(c);
            return(0);
        }
        sprintf(msg,"_%02x_",c);
        gprintf_mex(msg);
        return(0);
    }
    if(su->monif == 2) {
        sprintf(msg,"%d ",c);
        gprintf_mex(msg);
        return(0);
    }
    if(su->monif == 3) {
        if(c > 0x1f && c < 0x7f) {
            sprintf(msg,"_%02x:%c",c,c);
            gprintf_mex(msg);
        }
        else {
            sprintf(msg,"_%02x: ",c);
            gprintf_mex(msg);
        }
        return(0);
    }
    if(su->monif == 4) {
        if(c == 0x1b) {
            sprintf(msg,"\nESC ");
            gprintf_mex(msg);
        }
        else {
            sprintf(msg,"%c ",c);
            gprintf_mex(msg);
        }
    }
    return(0);
}

/*  Direct console I/O
putch2(c)
int  c;
{
    if(c == 0xff) return(-1);
    r.h.ah = 0x06;
    r.h.dl = c;
    intdoss(&r,&r);
    return(0);
}
*/

gotoxy(x,y)
int  x,y;
{
    int  i;

    putchar(0x1e); /* home */
    for(i = 0; i < y; i++) putchar(0x0a);  /* LF */
    for(i = 0; i < x; i++) putchar(0x0c);  /* left */
    return(0);
}

/* setup interrupt vector
*/
setupvec()
{
    int  (*p)();                   /* int232c() program pointer */

    imrmomsk = inp(I8259M+2);      /* save master IMR */
    imrsomsk = inp(I8259S+2);      /* save slave IMR */
    rvp = getvect(RSVECNO);        /* get rs232c old vector p(519) */
    setvect(RSVECNO,(void (interrupt far *)())int232c);

    keyv = getvect(KEYVNO);         /* get key vector p(519) */
    setvect(KEYVNO,(void (interrupt far *)())intkey);
    outp(I8255S,0x09);             /* system portc beep off RxRDY */

    tvp = getvect(TIMERVEC);       /* get timer vector p(519) */
    setvect(TIMERVEC,(void (interrupt far *)())intime0);
    clockset();                    /* timer clock set */
    outp(I8259M+2,IMRMMASK);       /* master 8259 mask set */
/*  outp(I8259S+2,IMRSMASK);          slave 8259 mask set */
    ls->vectf = 1;                 /* vecter on flag */
    return(0);
}

/* reset interrupt vector
*/
resetvec()
{
    int  k;

    tterm();                       /* Close Boyo Lib */
    outp(I8259M+2,0xfd);           /* mask master IMR */
    outp(I8255S,0x08);             /* system portc beep off */
    setvect(RSVECNO,rvp);          /* reset rs232c vector */
    setvect(KEYVNO,keyv);           /* reset key vector */
    setvect(TIMERVEC,tvp);         /* reset timer vector */

    k = clock;                     /* reset clock val. */
    outp_mex(I8253C0,k & 0xff);
    outp_mex(I8253C0,(k >> 8) & 0xff);

    outp_mex(I8259M+2,imrmomsk);   /* reset master IMR */
    outp_mex(I8259S+2,imrsomsk);   /* reset slave IMR */
    ls->vectf = 0;
    return(0);
}

