#define VERSION "1.21"
#include "stdio.h"
#include "dos.h"
#include "float.h"
#include "crt.h"
#include "bslib.h"
#define COMPUTER_TYPE "MSDOS"

/*
#include <stdio.h>
#include <dos.h>
#include <float.h>
#include <lib\crt.h>
#include <lib\bslib.h>

#define COMPUTER_TYPE "I8086"
#define COMPUTER_TYPE "MSDOS"
#define COMPUTER_TYPE "M68000"
#define COMPUTER_TYPE "UNIX"
*/

/*          LC "dos.h"
struct XREG
	{
	short ax,bx,cx,dx,si,di;
	};
union REGS
	{
	struct XREG x;
	struct HREG h;
	};
*/

struct XREGS
        {
            short ax,bx,cx,dx,si,di,ds,es;
        };
struct HREG
	{
	unsigned char al,ah,bl,bh,cl,ch,dl,dh;
	};
union REGSS
        {
            struct XREGS x;
            struct HREG h;
        };

/*                TC <dos.h>
struct WORDREGS {
	unsigned int	ax, bx, cx, dx, si, di, cflag, flags;
};

struct BYTEREGS {
	unsigned char	al, ah, bl, bh, cl, ch, dl, dh;
};

union	REGS	{
	struct	WORDREGS x;
	struct	BYTEREGS h;
};

struct	SREGS	{
	unsigned int	es;
	unsigned int	cs;
	unsigned int	ss;
	unsigned int	ds;
};
*/
#define  DATPORT 0xe0d8        /* DATA PORT   for ESU */
#define  STSPORT 0xe0da        /* STATUS PORT for ESU */
#define  DPORT0 0xe0d0
#define  SPORT0 0xe0d2
#define  DPORT1 0xe4d0         /* data port address */
#define  SPORT1 0xe4d2         /* status port address */
#define  DPORT2 0xe8d0
#define  SPORT2 0xe8d2
#define  DPORT3 0xecd0
#define  SPORT3 0xecd2
#define  DPORT4 0xf0d0
#define  SPORT4 0xf0d2
#define  DPORT5 0xf4d0
#define  SPORT5 0xf4d2
#define  DPORT6 0xf8d0
#define  SPORT6 0xf8d2
#define  DPORT7 0xfcd0
#define  SPORT7 0xfcd2
#define  DPORT8 0xd0d0
#define  SPORT8 0xd0d2
#define  DPORT9 0xd4d0
#define  SPORT9 0xd4d2
#define  DPORTA 0xd8d0
#define  SPORTA 0xd8d2
#define  DPORTB 0xdcd0
#define  SPORTB 0xdcd2

/* c0d0 address : not check, read MPU mamual */
#define  DPORTC 0xc0d0
#define  SPORTC 0xc0d2
#define  DPORTD 0xc4d0
#define  SPORTD 0xc4d2
#define  DPORTE 0xc8d0
#define  SPORTE 0xc8d2
#define  DPORTF 0xccd0
#define  SPORTF 0xccd2

#define I8251D   0x30   /* 8251 data  i/o addr. */
#define I8251S   0x32   /* 8251 status & command reg. addr. */
#define I8255S   0x35   /* 8255 systemport */
#define I8259M   0x00   /* 8259A master addr. */
#define I8259S   0x08   /* 8259A sleve addr. */
#define I8253C2  0x75   /* 8253 counter 2 addr. */
#define I8253CM  0x77   /* 8253 command mode addr. */
#define K8251D   0x41   /* 8251 keyboard data addr. */
#define K8251S   0x43   /* 8251 keyboard status & command addr. */
#define RSVECNO  0xc    /* rs232c vector number */
#define BUFSIZE  500    /* recive buffer size */
#define OFFLIM1  350    /* recive flow level1 */
#define OFFLIM2  425    /* level2 */
#define OFFLIM3  465    /* level3 */
#define ONLIMIT  100    /* xon limit */
/* #define FTWAIT   120     Full time send wait loop count (1 sec)*/
#define FTWAIT   0      /* Full time send wait loop count (1 sec) */
#define XON      0x11
#define XOFF     0x13
#define ESC      0x1b
#define BS       0x08
#define HT       0x09
#define BEL      0x07
#define LF       0x0a
#define VT       0x0b
#define FF       0x0c
#define CR       0x0d
#define SO       0x0e
#define SI       0x0f
#define CAN      0x18
#define SUB      0x1a
#define DEL      0x7f
#define CSI      0x9b
#define TRUE     1
#define FALSE    0
#define I8253C0  0x71   /* 8253 counter 0 addr. */
#define SLAVEINT 0      /* 8259 slave interrupt enable/disable */
#define DEBUG    0      /* debug mode */
#define IMRMMASK 0x2c   /* master 8259 mask (timer key IRQ4 rs232c) */
#define IMRSMASK 0xf5   /* slave  8259 mask (f5) */
#define KEYVNO   0x09   /* key vector number */
#define KBUFSIZE 500    /* keyboud buffer size */
#define KDSIZE   16     /* keyboaud work buffer size */
#define CNTL_O   0x0f   /* monitor mode */
#define PRPORTB  0x42   /* printer oprt B (CPU clock check) */
#define SMPTCLK  2457600  /* 10MHz clock */
#define SMPTCLK8 1996800  /* 8MHz  clock */
#define BCLKINT  120      /* smpte clock base (1/120 sec) */
#define TCLOCK   40960  /* 10MHz (2.4576 MHz/ 60 sec) */
#define TCLOCK8  33280  /* 8MHz  (1.9968 MHz/ 60 sec) */
#define TIMERVEC 0x08   /* time vector number */
#define MAXPT    15     /* Max. port number */
#define MAXF 100
#define STLN 20

#define SMPTYP   30
/* Controller numbers table */
#define MODUR    1         /* modulation depth */
#define POLTM    5         /* poltament time */
#define DENTR    6         /* data entry (RPN/NRPN) */
#define VOLUM    7         /* main volume */
#define BALAN    8         /* balance control */
#define PANPT    10        /* panpot */
#define EXPRS    11        /* expression */
#define DUMPR    64        /* hold 1 (dumper) */
#define POLTA    65        /* poltament */
#define SOSTN    66        /* sosutenute (chord hold) */
#define SOFTP    67        /* soft pedal */
#define HOLD2    69        /* hold2 (flease) */
#define EXTEF    91        /* external efect depth */
#define TREMO    92        /* toremolo depth */
#define CHORS    93        /* chorus depth */
#define SELES    94        /* seleste depth */
#define FETHR    95        /* feather depth */
#define PITCH    150       /* pitch vend */

typedef struct {
    int  shift,caps,kana,grph,ctrl;
} KEYFLG;

typedef struct {
    int  echo;          /* echo */
    int  kecho;         /* key code echo */
    int  monif;         /* monitor */
    FILE *fpi,*fpo;     /* download/upload file pointer */
    FILE *fpw;          /* Manual download file pointer */
    int  maskflg;       /* transmitt enable flag */
    char date[20];
    char time[12];
    long counter;       /* timer counter value */
    int  mtclk;         /* MTC timer counter */
    long countall;      /* timer count all */
    int  clockflg;      /* timer clock flag */
    int  subclk;        /* timer clock flag for quick optin */
    int  qflame;
    int  qsec;          /* quoater flame sec */
    int  qmin;          /* quoater flame min */
    int  qhour;         /* quoater flame hour */
    int  qtype;         /* quoater flame type */
    int  f1act;         /* f1 code activ flag for quick option */
    int  trak;
    int  memb;
    int  xrtn;
    int  xp;            /* monitor x position */
    int  yp;            /* monitor y position */
} SETUPD;

union MCODE {
    unsigned char wc[4];
    unsigned short wi[2];
};

typedef struct {
    int  comptyp;
    int  dump;
    int  dchk;
    int  debug;
    int  play;
    int  quick;        /* quick execute .mec file */
    int  smpte;
    int  smptp;        /* out performer's direct time lock code */
    int  actpn;        /* active port number */
    int  smpgen;       /* ESU smpte generate */
    int  smpread;      /* ESU smpte read */
    int  eflame;       /* ESU generate start flame */ 
    int  esec;         /* ESU generate start sec */
    int  emin;         /* ESU generate start min */
    int  ehour;        /* ESU generate start hour */
    int  etype;        /* ESU generate flame type */
    int  edelay;       /* ESU generate delay time */
    int  erdelay;      /* ESU read delay time */
    int  act232c;      /* EMU 232c port active flag */
    int  e232cp;       /* EMU 232c port number */
    int  monitor;
    int  mtcport;      /* midi time code send port */
    int  dtcport;      /* direct time loc send port */
    int  brself;       /* bar & movement number select flag */
    int  mbendf;       /* bar & movement end number select flag */
    int  vectf;        /* vecter mode check flag */
    int  blibf;        /* Boyo lib act flag */
    int  clktyp;       /* smpte time or real time */
    int  clkmod;       /* smpte time or real time clock modify */
    int  curclk;       /* courrent clock for tempo change */
    int  orgclk;       /* original clock for a tempo */
    int  termode;      /* terminal mode */
    int  smptyp;       /* SMPTE flame type */
    int  smptef;       /* SMPTE flame number */
    int  modx;         /* ls->mexres/120 */
    long mtcnt;        /* MTC count */
    int  mtcaf;        /* MTC activ flag */
    int  mtchh;        /* MTC hh */
    int  mtcmm;        /* MTC mm */
    int  mtcss;        /* MTC ss */
    int  mtcff;        /* MTC ff */
    float mexres;      /* mex flame resolution */
} MEXSRG;

typedef struct {
    int  memb;         /* member */
    int  gate;         /* gate */
    int  chan;         /* channel */
    int  trak;         /* track */
    int  tsym;         /* tempo symbol no. */
    int  temp;         /* tempo */
    int  movn;         /* movement number */
    int  barn;         /* bar number */
    long sync;         /* sync value */
    int  notep;        /* note position */
    int  f0typ;        /* f0 code file type  */
    int  f0num;        /* f0 code file number */
    int  ssequ;        /* for start movement number select */
    long sbarn;        /* for start bar number select */
    int  esequ;        /* for end movement number select */
    long ebarn;        /* for end bar number select */
} MEXVAR;

typedef struct {
    int  p_act[16];    /* port active flag */
} MEXACT;

typedef struct {
    int  ftype;        /* frase type */
    int  fchan[4];     /* frase channel */
    int  fsval[4];     /* frase start value */
    int  fctno[4];     /* frase control number */
    int  feval[4];     /* frase end value */
    int  fincv[4];     /* frase increment value */
    int  fintv[4];     /* frase timecode interval */
    int  fcval[4];     /* frase current value */
    int  fctin[4];     /* frase current timer interval */
    int  fudfg[4];     /* frase up down flag */
    int  factf[4];     /* frase activ on/off flag */
    long fnval[4];     /* frase note value */
} MQXFRS;

typedef struct {
    int  prchg[MAXPT]; /* program change */
    int  modur[MAXPT]; /* modulation */
    int  volum[MAXPT]; /* volume */
    int  klock;        /* key lock flag */
    int  xport;        /* execute port */
    int  sport;        /* select port */
    int  gmode;        /* graph key mode */
    int  schan;        /* shift key channel */
    int  kmflg;        /* key mode flag */
} MEXKEY;

