// ---------------------
// AMuPLC (mec.h) Mac  
// ---------------------

#define VERSION "3.06"
#define SUPPORT "Release"
//#define SUPPORT "Debug"

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

// 2025.4.7 
// 2025.4.5 gcc -> lcang
// 2020.7.29 add mechlp2.dat
// SMPTE B1 full time message Code out to Seq. start 2019.4.2

/*
//#define OS "RaspberryPi"
#define OS "FreeBSD"
#define TACTDIR "/usr/local/etc/tact/"
*/

/*
#define OS "Windows"
#define TACTDIR "C:/Program Files/AMuPLC/dat/"
*/

#define OS "Mac"
#define TACTDIR "/var/AMuPLC/"

/*
#define OS "CentOS"
#define SUPPORT "Release"
#define TACTDIR "/var/www/local/etc/tact/"
*/

/*
#define COMPUTER_TYPE "PC9801"
#define COMPUTER_TYPE "IBMPC"
#define COMPUTER_TYPE "SUN"
*/

#define COMPUTER_TYPE "IBMPC"
#define LATTICE_C 0
#define TURBO_C   0
#define MS_C      0
#define UNIX_C    1

#if LATTICE_C == 1
#include "stdio.h"
#include "ctype.h"
#endif

#if TURBO_C == 1
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#endif

#if UNIX_C == 1
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif

#define MXMB  256       /* max. member */
#define MAXA  20        /* max. argument number */
#define STLN  20        /* string get length */
#define MEXP  800       /* max. expression number (megsym.dat size) */
#define MTMP  100       /* max. temp sign. */
#define MSTC  100       /* max. card stack size */
#define CR    0x0d
#define LF    0x0a
#define BS    0x08
#define ESC   0x1b
#define STCTYP 3        /* smpte time code type 30 flame/sec(non dorop) */
#define MFRS  20        /* max. phrase words */
#define MAXSC 100       /* maximam save card line number */
#define MAXNX 500       /* maximam next card */
#define MAXEXF 2000     /* max. expand symbol */
#define MAXDXS 2000     /* max. dynamic expand symbol */
#define MAXPTN 80       /* max. articulation pattern length */ 
#define MAXAR 80        /* max. articulation table */ 
#define MAXNC 80        /* max. articulation notes */
#define MAXDV 256       /* max. articulation delta note values */
#define MAXPRC 4096     /* max. repeat code count */
#define MAXFN 30        // max file name

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
/* MPV */
#define MAXLVA 2000     /* max. Chord progression variable */
#define MAXLVL 32       /* max. Chord prog. left value */
#define MAXLVR 72       /* max. Chord prog. right value */ 
#define MAXRL  30000    /* max. Rule file line */

union meccode {
    unsigned char zc[4];   /* zc[1].zx[0].zc[3].zc[2] : 8086 */
    unsigned short zi[2];
    unsigned long zl;
};

typedef struct {
    char befr[82];
    char card[82];
    char next[82];
    char next2[82];
    char temp[82];
    char cw[82];       /* work */
    int  list;
    int  source;
    int  line;
    int  debug;
    int  trace;
    int  object;
    int  beaut;
    int  dump;
    int  allf;
    int  impv;         /* Ignore Multi channel Pitch.v  */
    int  mcpos;       /* memory symbol table current position cond. */
    int  mmpos;       /* memory symbol table current position memb. */
    int  symmax;
    int  mtsymb;
    int  mtempo;
    int  mkscl;
    int  mksin;
    int  mryhm;
    int  mtime;
    int  plps;        /* pitch left pos */
    int  prps;        /* pitch right pos */
    int  mdps;        /* modulation pos */
    int  dpos;        /* dumper pos */
    int  spos;        /* shift pos */
    float velo;       /* velociti */
    float mdymc;      /* master dynamec */
    float mtemp;      /* master tempo */
    char btyp;        /* bar type */
    char smpmid;      /* bar type smpte or midi */
    char dped;
    char dflg;        /* beautify line delete on/off flag */
    int  wflag;       /* work flag */
    int  mfflg;       /* main(master) file flag */
    int  hf;          /* Header read flag fn.meg */
    int  ktb[78];     /* key table */
    int  nktb[78];    /* natural key table */
    int  ktbbak[78];  /* me syousetu key */
    int  ntb[128];    /* note on/off & pos */
    int  quep;
    long ques[22];    /* queue sync */
    int  enoftb[128]; /* efect(tril etc.) note off table */
    int  tctyp;       /* time code type */
    FILE *fpo;
    FILE *fpe;
    float smpte;      /* SMPTE h + m + s + f */
    int  smpteh;      /* SMPTE hour */
    int  smptem;      /* SMPTE minit */
    int  smptes;      /* SMPTE second */
    float smptef;     /* SMPTE flame */
    int  rbaf;        /* tempo rubart active flag */
    int  bitrns;      /* beau instruments transpose val */
    int  errcnt;      /* error count */
    int  wrncnt;      /* worning count */
    int  msglvl;      /* message level */
    int  msgflg;      /* message on/off flag */
    int  prtlvl;      /* print level */
    int  prflg;       /* print flag */
    int  expf;        /* expand symbols */
    int  smf;         /* -SMF option flag */
} MECARG;

typedef struct {
    int  spos;
    int  memb;
    int  port;
    int  chan;
    int  trak;
    int  inst;
    int  trns;
    int  trnsv;        /* transpose value */
    int  trnsp;        /* #define transpose */
    int  mist;
    int  mifd;
    int  misd;
    int  egrp;
    int  expr;
    int  expr1;        /* expression symv1 */
    int  expr2;        /*            symv2 */
    int  expr3;        /*            symv3 */
    int  kscl;
    int  ksin;
    int  ryhm;
    int  time;
    int  movn;
    int  bnum;
    int  temp;
    int  temp1;        /* expression symv1 */
    int  temp2;        /*            symv2 */
    int  temp3;        /*            symv3 */
    long nval;
    int  tsym;
    long clen;
    long cval;
    long wval;         /* work cval (sousyoku) */
    long acci;
    long sous;         /* sousyoku */
    int  splt;
    int  rept;
    int  renp;
    char parc;
    char pard;
    long sync;
    long psyn;
    int  pport;
    int  keyp;
    int  voff;
    int  veon;
    int  kprs;
    int  cntn;
    int  cntv;
    int  prgn;
    int  prsv;
    int  dyna;
    int  phra;
    int  pitv;        /* pitch vend init value */
    int  mitv;        /* modulation init value */
    int  citv;        /* control init value */
    int  pgiv;        /* program change init value */
    long tcnt;
    int  ffflg;
    int  lag;         /* gloval & local lag */
    int  lagb;        /* in bar lag */
    int  offset;      /* gloval & local offset */
    int  offsetb;     /* in bar offset */
    int  sutyp;       /* shift unit type */
    int  prc;         /* put repeat chord counter */
    int  pprc;        /* pre put repeat chord counter */
} MECVAR;

struct mecksn {
    char *keysin[2][15];
};

typedef struct {
    char symsgn[MEXP][22];
    int  symtyp[MEXP];
    float symv1[MEXP];
    float symv2[MEXP];
    float symv3[MEXP];
    char  symv4[MEXP][4];
} MECSYM;

/* Program change # */
typedef struct {
    int n;
    int bf;             /* bar bigin flag */
    int af[MXMB];
    int port[MXMB];
    int chan[MXMB];
    int pcgn[MXMB];
} MECPCV;

typedef struct {
    int  smovn;         /* start movment number */
    long sbarn;         /* start bar number */
    int  emovn;         /* end movment number */
    long ebarn;         /* end bar number */
    int  comptyp;
    int  bnum;
    int  movn;
    int  brself;        /* bar number select flag */
    int  mbendf;        /* bar number end select flag */
    int  byrd;          /* bypass read flag fn(xxx,xxx) */
} MECMST;

typedef struct {
    int  fract[5];       /* phrase active on/off flag */
    int  frp[5];         /* phrase counter point */
    int  faf[5][MFRS];   /* phrase access flag */
    int  fd[5][MFRS];    /* fast data */
    int  ed[5][MFRS];    /* end data */
    long frnv[5][MFRS];  /* phrase note value */
    long fsyn[5][MFRS];  /* phrase sync value */
    long fsmax[5];       /* phrase sync max value */
    long csyn[5];        /* phrase cullent sync value */
    long fwsyn[5];       /* phrase sync work */
    int  fxtyp[5];       /* phrase extention type */
    int  fxval[5];       /* phrase extention value */
    int  rept;           /* phrase note repeart */
    int  renp;           /* phrase remp */
    int  tfdyn;          /* {!txx} tril dyna */
    int  Tfdyn;          /* {!Txx} Tril dyna */
    int  sfdyn;          /* {!sxx} turn dyna */
    int  Sfdyn;          /* {!Sxx} Turn dyna */
    int  mfdyn;          /* {!mxx} mordent dyna */
    int  Mfdyn;          /* {!Mxx} Mordent dyna */
    int  wfdyn;          /* {!wxx} duble turn dyna */
    int  Wfdyn;          /* {!Wxx} Duble turn dyna */
    int  rfdyn;          /* {!rxx} tril... */
    int  Rfdyn;          /* {!Rxx} Tril... */
} MECFRS;

union MCODE {
    unsigned char wc[4];
    unsigned short wi[2];
};

struct MECEXP {
    char exf[4];
    char exd[MSTC][82];
    int  lc;
};

typedef struct {
    int  nmb;             /* number */
    int  af;              /* activ flag */
    int  pmf;             /* plus minus flag */
    char cdxs[4];         /* current dxs */
    int  ctyp;            /* current type */
    int  crate;           /* current gate rate */
    long nval;            /* current note val */
    char crt[MAXPTN];     /* current ryhthm pattern */
    char cnt[MAXPTN];     /* current Notes pattern */
    char cvt[MAXPTN];     /* current Velocity pattern */
    char fbt[MAXPTN];     /* current float Velocity base pattern */
    int  typ[MAXDXS];     /* arpeggio fixed or split variable */
    char dxs[MAXDXS][4];  /* Dynamic expand symbol name */
    char rt[MAXDXS][MAXPTN];  /* ryhthm pattern */
    char nt[MAXDXS][MAXPTN];  /* Notes pattern */
    char vt[MAXDXS][MAXPTN];  /* Velocoty pattern */
    char ft[MAXDXS][MAXPTN];  /* float Velocoty pattern */
} MECART; /* Dynamic Expand (Articulation) */

typedef struct {
   int  sc;                /* Split count */
   int  dvc;               /* dv count */
   int  nc[MAXAR];         /* Note count */
   int  rc[MAXAR];         /* Repeat note count */
   int  dc[MAXAR];         /* delta note count */
   int  dv[MAXDV];         /* delta note value */
   int  nvv[MAXAR][MAXNC]; /* Note velocity value */
   float fvb[MAXAR][MAXNC]; /* float Note velocity base value */
   int  sync[MAXDV];       /* delta sync */
   int  dnc[MAXDV];        /* delta note count */
   int  np[MAXDV][MAXNC];  /* Note position */
   int  nv[MAXDV][MAXNC];  /* Note value */
} MECON;

typedef struct {  
   char mc[MAXAR];        /* Marc char */
   int  rs[MAXDV];        /* repeat dv sum */
   int  dv[MAXDV];        /* delta note value */
   int  sync[MAXDV];      /* delta sync */
   int  dnc[MAXDV];       /* delta note count */
   int  np[MAXDV][MAXNC]; /* Note position */
   int  nv[MAXDV][MAXNC]; /* Note value */ 
} MECOFF;

typedef struct {
    int  key;                      /* Major/Minor key */
    int  jky;                      /* Major key rule flag */
    int  iky;                      /* minor key rule flag */
    int  n;                        /* Variable Counter */
    char l[MAXLVA][MAXLVL];        /* Left Value */
    char r[MAXLVA][MAXLVR];        /* Right Value */ 
} MECCPS;

typedef struct {
    int  mpvf;         /* multi channel pitchi.v flag */
    int  rp;           /* root position */
    int  mpvch[128];   /* channel table */
    int  mpvsp[128];   /* saved pitch.vend table */
    int  mpvcp[128];   /* current p.v table */
    int  mpvsm[128];   /* saved modulation table */
    int  mpvcm[128];   /* current modulation table */
} MECMPV;

// main.c
int mecfchk1_mec(char mfile[STLN],char partc);
int mecfchk2_mec(char mfile[STLN],char partc);
int mecmain(char *inf);
int header_mec();
int mcaper_mec(struct mecksn *mk,FILE *fpi);
int keyryhm_mec(struct mecksn *mk);
int ptcper_mec(char *inf,struct mecksn *mk);
int ptcrest_mec(struct mecksn *mk);
int mecfread_mec(char mwkf[STLN],int tbflg);
int topchk_mec(unsigned char iword[4]);
int btmchk_mec(unsigned char iword[4],int *pf);
int fbdecode_mec(union MCODE *u);
int fddecode_mec(union MCODE *u);
int btfputc_mec(unsigned char iword[4]);
int pr_card(char *card);
int Nachsch_mec();
int Nachsbar_mec(struct mecksn *mk);

// mec1.c
FILE *fopen_h(char *file, char *mode);
int chkmfile_mec(char *fname);
int mkeyset_mec();
int selfil_mec(char *mfile,int track[],char partf[MXMB][36],int port[],int channel[],int member[]);
int meccopy_mec(char *sfile,char *ofile);
int strcat_h(char *tos,char *from);
int bar_mec();
int mbselck_mec();
int mkexpt_mec();
int numdin_mec(char *card);
int putcode_mec(union meccode *fx);
int putrepc_mec();
int exprnum_mec();
int key_mec(struct mecksn *mk);
int keyini_mec();
int nkeyini_mec();
int keyset_mec();
int mksym_mec(struct mecksn *mk);
int keydmp_mec(struct mecksn *mk);
int final_mec();
int expression_mec(union meccode *ff);
int initdyna_mec();
int rsphdyna_mec();
int exptemp_mec();
int dump_mec(char *inf);
int renget_mec();
int movbar_mec();
int lsconv_h(union meccode *fx,unsigned long lval);
int help_main();
int fhelp_mec(int help);

// mec2.c
int pasread_mec();
int note_mec(struct mecksn *mk,int exf,char exp[4]);
int pasproc_mec(struct mecksn *mk);
int accid_mec(struct mecksn *mk);
int noteon_mec();
int anoteon_mec();
int polfkey_mec();
int noteoff_mec();
int fd9off_mec();
int noteclr_mec();
int rest_mec(struct mecksn *mk);
int befnote_mec(struct mecksn *mk);
int queset_mec(long cval);
int efect_mec();
int alefec_mec();
int fdcode_mec();
int fd9code_mec();
int startkp_mec(int *startp,int *lastp);
long getswing_mec();
long getnacv_mec(char *card);

// mec3.c
int mnote_mec(struct mecksn *mk);
int mpasproc_mec(struct mecksn *mk);
int stickon_mec();
int stickoff_mec();
int f1code_mec();
int smptcode_mec();
int mstmpo_mec();
int mdynamic_mec();
int mfilec_mec();
int mfilec3_mec();
int ffcode_mec(union meccode *ff);
int faf7code_mec();
int prints_mec(int line,char source[82]);
int strcheck_mec();
int mkmultif_h(char *infn,char *outf,char *ftyp,int fnmb,int *prtn,int *yfile,int *xfile);
int putsync_mec(int k);

// mec4.c
int midic_mec();
int spacecard_mec();
int spacecmp_mec(char *card,int len);
int mkyxfnum_h(char fname[STLN],int *yfile,int *xfile,char *ps);
int filemake_h(int n,char inf[STLN],char *fext,char dfile[STLN],int *yflp,int *xflp);
int tril_mec();
int trilon_mec();
int turn_mec();
int turnon_mec();
int mordent_mec();
int mordon_mec();
int dmordent_mec();
int dmordon_mec();
int dtril_mec();
int dtrilon_mec();
int quecode_mec();
int quecode2_mec();
int syncepv_mec();
int syncecnt_mec();
int syncepcg_mec();
int syncetrs_mec();
int pedalck_mec();
int apedalck_mec();
int pdalout_mec(int cnb,int cdt);
int dnote_mec(struct mecksn *mk,int exf,char exp[4]);

// mec5.c
int fraseck_mec(long syn);
int vfrase_mec(int p,long syn,union meccode *e1,union meccode *e2,union meccode *e3,union meccode *e4);
int putfrcd_mec(int p,int frp,union meccode *e1,union meccode *e2,union meccode *e3,union meccode *e4);
int frasing_mec();
int fraschk_mec();
int nbufrck_mec();
int nbchg_mec(char wk[82]);
int bfunc_mec(char s[82],int *fsval,int *feval);
int getnval_mec(char *wkcd);
int frenget_mec(char card[82]);
int efdyna_mec();
int efdyna2_mec();

// mec6.c
int nextcard_mec(FILE *fpi);
int rfchk_mec(char card[82]);
int renproc_mec(struct mecksn *mk,FILE *fpi);
int lbrtosbr_mec();
int sbrtolbr_mec();
int dodnon_mec();
int dodanon_mec();
int dodpfkey_mec();
int dodnoff_mec();

// mec7.c
int smfmeta_mec();
int f6meta_smf(union meccode *f6);
int syncout_mec(long sync,int typ);
int lag_offset_mec();
int inspr_mec(char *wk);
int uvtonv_mec(int uval);
int spacecut_mec(char *card,int slen);
int expand_mec(char *inf,struct mecksn *mk);
int overln_mec();
int chkexpf_mec(char expf[MAXFN]);
int mkexpfn_mmm(char *inf,char outf[MAXFN],char *str);
int renexp_mec(char expf[MAXFN],struct mecksn *mk);
int trenexp_mec(char expf[MAXFN],struct mecksn *mk,int p);

// mec8.c
int mpchg_mec();
int outpcgn_mec();
int mpvgate_mec(char *inf);
int mpvset_mec();
int mpvcycl_mec(char *val);
int mpvstart_mec(char *val);
int majroot_mec();
int minroot_mec();
int mpvitw_mec(int n,char *val);
int setlvl_cps();
int setlvm_cps(char skey[MAXLVL]);
int setmval_cps(char line[82]);
int linechk_cps(char line[82],char skey[MAXLVL]);
int opencp_mec(char *inf);
int beauspace_meb(char card[82]);
int mpvchan_mec(unsigned char z0,unsigned char *zr,unsigned char z2,unsigned char z3);
int putpitv_mec(int k,int chan);
int putmodr_mec(int k,int chan);

// mecw.c
int dynacp_mec();
int dexpand_mec(struct mecksn *mk);
int dexpchk_mec(char exp[4],long nval);
int setontb_mec(char exps[4]);
int setofftb_mec();
int expsync_mec();
int notepos_mec(int nps[64],int nvn[64]);
int dnotepos_mec(int nps[64],int nvn[64]);
int setartv_mec(char exp[4]);
int dexpap_mec(char exp[4]);
int arptn0_mec(int dt);
int arptn1_mec(int dt);
int arptn2_mec(int dt);
int arptn3_mec(int dt);
int bksync_mec();
int arpsync_mec();
int getexp_mec();
int addexp_mec();
int dexputfd_mec();
int overerr_mec(char *msg,int ov);
//==========================================
