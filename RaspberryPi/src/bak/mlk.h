// ----------------------
//  AMuPLC (mlk.h) RaspberryPi
// ----------------------

#define VERSION "1.55"
//#define SUPPORT "Debug"
#define SUPPORT "Release"

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

// 2025.4.9 mlkm.c mlkhelp() modify
// 2025.4.8 gcc -> clang
// 2024.11.20 mlk5.c mlk8.c Trace printf Member cut

#define OS "RaspberryPi"
//#define OS "FreeBSD"
#define TACTDIR "/usr/local/etc/tact/"

/*
#define OS "Windows"
#define TACTDIR "C:/Program Files/AMuPLC/dat/"
*/

/*
#define OS "Mac"
#define TACTDIR "/var/AMuPLC/"
*/

/*
#define OS "CentOS"
#define SUPPORT "Release"
#define TACTDIR "/var/www/local/etc/tact/"
*/

#define COMPUTER_TYPE "IBMPC"
#define TURBO_C   0
#define UNIX_C    1

#if TURBO_C == 1
#include <stdio.h>
#include <float.h>
#define MAXPT 8         /* max midi port */
#endif

#if UNIX_C == 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXPT 16        /* max midi port */
#endif

#define MAXF  256
#define MAXP  200      // 2015.8.6 100 -> 200 
#define MAXAB 8000     /* max data in meas. */
#define MAXBC 8000     /* max data in meas. */
/* #define MAXAB 32000      max data in meas. */
/* #define MAXBC 32000      max data in meas. */
#define MAXSC 5000      /* max sync data in meas. */
#define MAXSBF 1500     /* max smpte (between f1 and f1) byte count */
/* #define MAXPT 8         max midi port */
#define STLN  20
/* Controller numbers table */
#define MODUR 1         /* modulation depth */
#define POLTM 5         /* poltament time */
#define DENTR 6         /* data entry (RPN/NRPN) */
#define VOLUM 7         /* main volume */
#define BALAN 8         /* balance control */
#define PANPT 10        /* panpot */
#define EXPRS 11        /* expression */
#define DUMPR 64        /* hold 1 (dumper) */
#define POLTA 65        /* poltament */
#define SOSTN 66        /* sosutenute (chord hold) */
#define SOFTP 67        /* soft pedal */
#define HOLD2 69        /* hold2 (flease) */
#define EXTEF 91        /* external efect depth */
#define TREMO 92        /* toremolo depth */
#define CHORS 93        /* chorus depth */
#define SELES 94        /* seleste depth */
#define FETHR 95        /* feather depth */
#define PITCH 150       /* pitch vend */

/* Flame interval (1/120 Sec) */
#define R_MODUR 8       /* modulation depth */
#define R_POLTM 1       /* poltament time */
#define R_DENTR 8       /* data entry (RPN/NRPN) */
#define R_VOLUM 2       /* main volume */
#define R_BALAN 8       /* balance control */
#define R_PANPT 8       /* panpot */
#define R_EXPRS 8       /* expression */
#define R_DUMPR 8       /* hold 1 (dumper) */
#define R_POLTA 1       /* poltament */
#define R_SOSTN 8       /* sosutenute (chord hold) */
#define R_SOFTP 8       /* soft pedal */
#define R_HOLD2 8       /* hold2 (flease) */
#define R_EXTEF 8       /* external efect depth */
#define R_TREMO 8       /* toremolo depth */
#define R_CHORS 8       /* chorus depth */
#define R_SELES 8       /* seleste depth */
#define R_FETHR 8       /* feather depth */
#define R_PITCH 1       /* pitch vend */

union MCODE {
    unsigned char wc[4];
    unsigned short wi[2];
    unsigned int  wl;
};
typedef struct {
    unsigned char c[4];
    unsigned short s[2];
    int  i[2];
    long l;
} MLKCDE;
typedef struct {
    int  comptyp;
    int  exef;         /* make execute file */
    int  mergef;       /* include fd data flag */
    int  idump;        /* input dump */
    int  odump;        /* output dump */
    int  idchk;        /* input data check */
    int  dchk;         /* data check */
    int  debug;
    int  trace;
    int  optm;         /* optimize */
    int  otrace;       /* output trace */
    int  outfm;        /* output format */
    int  startf;
    unsigned char dckcd[20];    /* data check code */
    int  dckn;         /* data check num */
    int  repla;        /* replace flag */
    int  incld;        /* include flag */
    int  excld;        /* exclude flag */
    int  lnkmon;       /* link monitor */
    int  errcnt;       /* error count */
    int  wrncnt;       /* worning count */
    int  msglvl;       /* message level */
    int  msgflg;       /* message on/off flag */
    int  prtlvl;       /* print level */
    int  eosf;         /* eos flag */
    int  score;        /* put smf score data to .mex */
} MLKSRG;
typedef struct {
    int  port;         /* midi port number */
    int  memb;         /* member */
    int  trak;         /* track */
    int  mmemb;        /* master member */
    int  gate;         /* gate */
    int  chan;         /* channel */
    int  mtrak;        /* master track */
    int  f0typ;        /* f0 code file type  */
    int  f0num;        /* f0 code file number */
    long smpte;        /* f1 code SMPTE */
    int  mvelo;        /* master velocity */
    int  kind;         /* note or rest */
    long cval;         /* code value */
    long gcval;        /* get from key code, code value */
    int  mist;         /* midi dtatus data */
    int  mifd;         /* midi first data */
    int  misd;         /* midi second data */
    int  trns;         /* transpose */
    int  inst;         /* instrment no */
    int  kscl;         /* moll or dur */
    int  ksin;         /* key signeture */
    int  ritm;         /* rithm */
    int  time;         /* time base */
    int  egrp;         /* expression group */
    unsigned int expr; /* expression number */
    int  tsym;         /* tempo symbol no. or note type */
    unsigned int temp; /* tempo val */
    int  movn;         /* movement number */
    unsigned int bnum; /* bar number */
    long meas;         /* measure (movn * 10000 + bnum) */
    int  mmovn;        /* master movement number */
    unsigned int mbnum; /* master bar number */
    long mmeas;        /* master measure (mmovn * 10000 + mbnum) */
    long sync;         /* synchronize */
    long msync;        /* master synchronize */
    int  fdtyp;        /* FD data type */
    int  fdch;         /* FD channnel no. */
    int  keyp;         /* Note number */
    int  voff;         /* Note off velocity */
    int  veon;         /* Note on velocity */
    int  kprs;         /* Poliphonic key pressure */
    int  cntn;         /* Controle change no. */
    int  cntv;         /* Controle change val */
    int  prgn;         /* Program change no. */
    int  prgv;         /* Program change val */
    int  ptyp;         /* Channel pressure type */
    int  prsv;         /* Channel pressure val */
    int  pvls;         /* Pitch vend LSB val */
    int  pvms;         /* Pitch vend MSB val */
    long tcnt;         /* total input code count */
    long occnt;        /* total output code count */
    long stlag;        /* Smpte time lag */
    long qfcnt;        /* Quoater flame out put count */
    long qfsmp;        /* Quoater flame smpte */
    int  tctyp;        /* SMPTE time code type */
    int  sflame;       /* SMPTE and MIDI time Code flame per second */
    int  smfdiv;       /* Standard MIDI File Division fixed tempo _/ = 120 */
    int  fix;          /* Standard MIDI File fix tempo option */
    float mexres;      /* mex flame resolution -fres option */
    float mlp;         /* lag = ds * plag * mlp / dsc */
    int  mtcres;       /* MIDI Time Code resolution */
    int  e6cn;         /* E6 code control no. */
    int  e6d1;         /* E6 code data 1 */
    int  e6d2;         /* E6 code data 2 */
} MLKVAR;

typedef struct {
    int  ftype[MAXPT];        /* phrase type */
    int  fchan[MAXPT][5];     /* phrase channel */
    int  fsval[MAXPT][5];     /* phrase start value */
    int  fctno[MAXPT][5];     /* phrase control number */
    int  feval[MAXPT][5];     /* phrase end value */
    long fnval[MAXPT][5];     /* phrase note value */
    int  fincv[MAXPT][5];     /* phrase increment value */
    int  fintv[MAXPT][5];     /* phrase timecode interval */
    int  fcval[MAXPT][5];     /* phrase current value */
    int  fctin[MAXPT][5];     /* phrase current timer interval */
    int  fudfg[MAXPT][5];     /* phrase up down flag */
    int  factf[MAXPT][5];     /* phrase activ on/off flag */
    int  fxtyp[MAXPT][5];     /* phrase extention type */
    int  fxval[MAXPT][5];     /* phrase extention value */
} MLKFRS;

typedef struct {
    int  cn[16][5];       /* phrase control number */
    long fl[16][5];       /* phrase length */
    int  iv[16][5];       /* phrase increment value */
    int  dl[16][5];       /* phrase delta length */
    int  cv[16][5];       /* phrase current value */
    int  ev[16][5];       /* phrase end value */
    int  cp[16][5];       /* phrase current point */
    int  af[16][5];       /* phrase activ on/off flag */
} MLKPHS;

typedef struct {
    long ct[MAXPT][16][5];       /* phrase current delta time */
    int  cn[MAXPT][16][5];       /* phrase control number */
    long fl[MAXPT][16][5];       /* phrase length */
    int  iv[MAXPT][16][5];       /* phrase increment value */
    int  dl[MAXPT][16][5];       /* phrase delta length */
    int  cv[MAXPT][16][5];       /* phrase current value */
    int  ev[MAXPT][16][5];       /* phrase end value */
    int  cp[MAXPT][16][5];       /* phrase current point */
    int  af[MAXPT][16][5];       /* phrase activ on/off flag */
    int  pm[MAXPT][16][5];       /* phrase member */
    int  pt[MAXPT][16][5];       /* phrase track */
    int  xt[MAXPT][16][5];       /* phrase extention type */
    int  xv[MAXPT][16][5];       /* phrase extention value */
} MLKMPH;

typedef struct {
    unsigned char c0;
    unsigned char c1;
    unsigned char c2;
    unsigned char c3;
} SYNSORT;
typedef struct {
    int  smovn;         /* start movment number */
    long sbarn;         /* start bar number */
    int  emovn;         /* end movment number */
    long ebarn;         /* end bar number */
    int  bnum;
    int  movn;
    int  brself;        /* bar number select flag */
    int  mbendf;        /* bar number end select flag */
    int  byrd;          /* bypass read flag fn(xxx,xxx) */
    int  f1;            /* flame LSB */
    int  f2;            /* flame MSB */
    int  s1;            /* sec LSB */
    int  s2;            /* sec MSB */
    int  m1;            /* min. LSB */
    int  m2;            /* min. MSB */
    int  h1;            /* hour LSB */
    int  h2;            /* hour MSB */
    int  f;             /* smpte flame */
    int  s;             /* smpte sec */
    int  m;             /* smpte min */
    int  h;             /* smpte hour */
    int  tcbit;         /* smpte time code type */
    int  flc;           /* flame count (flame position */
    long fflame;        /* flactional flame (1/100 flame) */
    long smpte;         /* readed last smpte value */
} MECMST;

//  mlkm.c 
int main(int argc,char *argv[]);
int shipset_mlk(unsigned char iword[4]);
int mlkmain(char inf[STLN],int yfile,int xfile);
int lkcfsel_mlk(char dfile[STLN],char ftyp);
int prmkselsw_mlk(char dfile[STLN],char ftyp,int portnb,int j);
int mkselsw_mlk(char dfile[STLN],int portnb,int pcn);
int selmfnc_mlk(char dfile[STLN],int portnb,int sw,int pcn);
int selfunc_mlk(char dfile[STLN],int portnb,int sw);
int mlkdchk_mlk(char dfile[STLN]);
float fresopt_mex(char *dtopt);
int dtoption_mex(char *dtopt);
int mlkhelp();

// mlk1.c
int datachk_mlk();
int ffdecode_mlk();
int f0decode_mlk();
int f1decode_mlk();
int b1decode_mlk();
int f2decode_mlk();
int f3decode_mlk();
int f4decode_mlk();
int f5decode_mlk();
int f6decode_mlk();
int f7decode_mlk();
int f8decode_mlk();
int f9decode_mlk();
int fadecode_mlk();
int fbdecode_mlk();
int mfbdecode_mlk();
int fcdecode_mlk();
int mfcdecode_mlk();
int fddecode_mlk();
int fedecode_mlk();
int dddecode_mlk();
int e1decode_mlk();
int e2decode_mlk();
int e3decode_mlk();
int e4decode_mlk();
int e6decode_mlk();
int dump_mlk(char dfile[STLN]);
int filemake_h(unsigned int n,char inf[STLN],char fext[],char dfile[STLN],int *yflp,int *xflp);
FILE *fopen_h(char *file, char *mode);
int strcat_h(char *tos,char *from);
int hlpread_mlk();
int smptubit_mlk(union MCODE *u,FILE *fpo);
int mkmultif_h(char *infn,char *outf,char *ftyp,int fnmb,int *prtn,int *yfile,int *xfile);
int mkyxfnum_h(char fname[STLN],int *yfile,int *xfile,char *ps);
int getdigit_h(char c,int *digit);

// mlk2.c
int cpconv0_mlk(char dfile[STLN],int portnb);
int cpconv1_mlk(char dfile[STLN],int portnb);
int cpconv2_mlk(char dfile[STLN],int portnb);
int cpconv3_mlk(char dfile[STLN],int portnb);
int cpconv4_mlk(char dfile[STLN],int portnb);
int cpconv5_mlk(char dfile[STLN],int portnb);
int inmftowb_mlk(char dfile[STLN],int portnb);
int cpconv10_mlk(char dfile[STLN],int portnb);
int cpconv11_mlk(char dfile[STLN],int portnb);
int cpconv12_mlk(char dfile[STLN],int portnb);
int cpconv13_mlk(char dfile[STLN],int portnb);
int cpconv14_mlk(char dfile[STLN],int portnb);
int cpconv15_mlk(char dfile[STLN],int portnb);
int ctoval_mlk(unsigned char iword[4]);
int midfconv_mlk(char dfile[STLN],char ofile[STLN],int portnb);
int incmstf_mlk(char dfile[STLN],char ofile[STLN],int portnb);
int midfadd_mlk(char dfile[STLN],char mfile[STLN],char ofile[STLN],int portnb);
int mfocsel_mlk(unsigned char iword[4],FILE *fpo);
int fputcd_mlk(unsigned char iword[4],FILE *fpo);
int outcdump_mlk(char dfile[STLN]);
int mkptmscd_mlk(char bfile[STLN],int portnb);
int midfmix_mlk(char dfile[STLN],char mfile[STLN],char ofile[STLN],int portnb);
int bfsmout_mlk(FILE *fpb,FILE *fpo);
int bfschout_mlk(FILE *fpb,FILE *fpo);
int atsmout_mlk(FILE *fpo);
int btsmout_mlk(FILE *fpo);
int savema_mlk(unsigned char iword[4]);
int savemb_mlk(unsigned char iword[4]);
int abtsort_mlk(FILE *fpo);
int outsctf_mlk(FILE *fpo,int isync);
int fputsc_mlk(int i,FILE *fpo);
int bsynchk_mlk();
int baddtoc_mlk(int sp);
int sortmova_mlk();
int sortmovb_mlk();
int ddasave_mlk();
int ddbsave_mlk();

// mlk3.c
int exsmpte_mlk(char ifile[STLN],int portnb);
int spachk_mlk(FILE *fpo);
int spbchk_mlk(FILE *fpo);
int mkmidif_mlk(char ifile[STLN]);
int submkmid_mlk(FILE *fpi,FILE *fpo,FILE *fpw);
int smptfull_mlk(FILE *fpo);
int quatflag_mlk(FILE *fpo,int last);
int compsmp1_mlk(long fm);
int compsmp2_mlk(long fm);
int ltocdec_mlk(unsigned char cword[4],unsigned char fcode,long lval);

// mlk4.c
int phrasing_mlk(int p,int f,FILE *fpo);
int flasout_mlk(int p,int f,FILE *fpo);
int flcout_mlk(int p,int f,FILE *fpo);
int pitch_mlk(int p,int f,FILE *fpo);     /* pitch vend */
int fmerge0_mlk(char dfile[STLN],int portnb);
int fmerge1_mlk(char dfile[STLN],int portnb);
int fmerge2_mlk(char dfile[STLN],int portnb);
int fmerge3_mlk(char dfile[STLN],int portnb,int eop);
int fmerge4_mlk(char dfile[STLN],int portnb,int eop);
int fmerge5_mlk(char dfile[STLN],int portnb,int eop);
int fmerge6_mlk(char dfile[STLN],int portnb);
int fmerge10_mlk(char dfile[STLN],int portnb);
int fmerge11_mlk(char dfile[STLN],int portnb);
int fmerge12_mlk(char dfile[STLN],int portnb);
int fmerge13_mlk(char dfile[STLN],int portnb,int eop);
int fmerge14_mlk(char dfile[STLN],int portnb,int eop);
int fmerge15_mlk(char dfile[STLN],int portnb,int eop);
int mlkfrd1_mlk(FILE *fpo,FILE *fpw);
int mlkfrd2_mlk(FILE *fpo,FILE *fpw);
int topchk_mlk(FILE *fpo,unsigned char iword[4]);
int btmchk_mlk(FILE *fpo,unsigned char iword[4],int *pf);
int mbselck_mlk();
int f1dec_mlk(unsigned char iword[4]);
int setqfsv_mlk();
int fbdec_mlk();
int finclude_mlk(char dfile[STLN],int portnb);
int fexclude_mlk(char dfile[STLN],int portnb);
int freplace_mlk(char dfile[STLN],int portnb);
int mlkexcl_mlk(char afile[STLN],char dfile[STLN],char ofile[STLN],int portnb);
int optimize_mlk(char inf[STLN]);

// mlk5.c
int mkdtx(char ifile[STLN]);
int dtconv_mlk(long dt,int *hh,int *mm,int *ss,int *ff);
int dtxsub1(FILE *fpi,FILE *fpo);
int dtxsub2(FILE *fpi,FILE *fpo);
int LagOut(long dt,FILE *fpo);
int velochg_mlk(unsigned char iword[4]);
int velomsg_mlk();

// mlk6.c
int mksmf0_mid(char ifile[STLN]);
int smf0sub1(FILE *fpi,FILE *fpo);
int smf0sub2(FILE *fpi,FILE *fpo);
int VarLen(long dt,FILE *fpo);
int ph_varlen(long dt,FILE *fpo);
int ph_afset();
int ph_clear(int e,int f);
int ph_out(int channel,int f,FILE *fpo);
int ph_outmsg(int fctn);
int pitch_smf0(int channel,int f,FILE *fpo);    /* pitch vend */
int fddec_mlk();
int WriteVarLen(register long value,FILE *fpo);
int get_length(long len,unsigned char iword[4]);

// mlk7.c
int mksmf0_val(char ifile[STLN]);
int smf0sub1_dt(FILE *fpi,FILE *fpo);
int smf0sub2_dt(FILE *fpi,FILE *fpo);

// mlk8.c
int newmex_mlk(char ifile[STLN]);
int nexsub1(FILE *fpi,FILE *fpo);
int nexsub2(FILE *fpi,FILE *fpo);
int nexsub3(FILE *fpi,FILE *fpo);
int Ph_Lag(long dt,FILE *fpo);
int ckmp_varlen(long cct,FILE *fpo);
int mp_varlen(int p,int e,int f,long cct,FILE *fpo);
int mp_chgpt(int p,int e,int f,FILE *fpo);
int mp_afset();
int mp_clear(int p,int e,int f);
int mp_out(int p,int e,int f,FILE *fpo);
int mp_pitch(int p,int e,int f,FILE *fpo);
int Ph_LagOut(int p,long dt,FILE *fpo);

// mlk9.c
int mksmf1_mlk(char ifile[STLN]);

// mlkw.c
int mkexecf_mlk(char ifile[STLN]);


