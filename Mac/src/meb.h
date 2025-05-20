// -------------------
// AMuPLC (meb.h)  Mac
// -------------------

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

// 2025.4.3 main.c add mb->ktrnsv = 0; meb9.c xtransp_meb()
// 2025.4.3 clang debug -d16 PASS test for core dump
// 2025.4.2 gcc -> clang Warning handl 
// edit meb.h mebm.c meb1.c meb2.c meb3,c meb4.c meb5.c meb6.c 
//      meb7.c meb8.c meb9.c meb10.c meb11.c meb12.c mebw.c
// 2025.3.30 meb.h MAXLVA 2000 -> 300 for Windows
// 2024.12.1 meb4.c 973 bug fix char wscl[8] -> wscl[12]
// 2024.9.3 meb.h meb1.c meb5.c SetRenumStartVal_meb() add mb->ybnum
// 2024.9.1 meb1.c SetRenumStartVal_meb() ewnumber_start_value 0
// 2021.11.30 inst Transpose Check
// 2020.7.8 mebm.c mebhlp2.dat help 2 
// 2020.7.22 Relese
// 2025.4.2 gcc -> clang Warning fix
// 2019.12.19 meb10.c on chord bug fix

#define VERSION "5.07"
//#define SUPPORT "Debug"
#define SUPPORT "Release"

/*
//#define OS "RaspberryPi"
#define OS "FreeBSD"
#define TACTDIR "/usr/local/etc/tact/"
*/

#define OS "Mac"
#define TACTDIR "/var/AMuPLC/"

/*
#define OS "CentOS"
#define SUPPORT "Release"
#define TACTDIR "/var/www/local/etc/tact/"
*/


/*
#define OS "Windows"
#define TACTDIR "C:/Program Files/AMuPLC/dat/"
*/

#define Sname1 "202504"
#define Sname2 "24$"
#define Snum1 "$V5."
#define Snum2 "05W$"

/* CentOS 
#define TACTDIR "/var/www/local/etc/tact/"
*/

#define COMPUTER_TYPE "IBMPC"
/* v 0.16  max member 256 suport
#define COMPUTER_TYPE "PC9801"
#define COMPUTER_TYPE "IBMPC"
#define COMPUTER_TYPE "SUN"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define Cr '\0'

#define MXMB  256       /* max. member */
#define MAXA  20        /* max. argument number */
#define MAXPH 3000      /* max. phrase symbol table */
#define MAXPL 100       /* max. phrase line */
#define MAXEX 300       /* max. expand file symbol */
#define STLN  20        /* string get length */
#define MEXP  10000     /* max. expression number (megsym.dat size) */
#define MTMP  200       /* max. temp sign. */
#define MSTC  200       /* max. card stack size */
#define MMST  999       /* max. memory symbol tabel */
//#define MMST  300       /* max. memory symbol tabel */
#define MMSN  80        /* max. memory symbol line size (if MSDOS 78 -> 22) */
#define MAXONP 18       /* Max. on port */
#define SMPLAG 150      /* SMPTE time lag */
#define STCTYP 3        /* smpte time code type 30 flame/sec(non dorop) */
#define MAXLV  6        /* max. Chord progression rule level */
#define MAXLVA 1200     /* max.2000 Chord progression variable */
//#define MAXLVA 900    // for Windows 
#define MAXLVL 42       /* max. Chord prog. left value */
#define MAXLVM 72       /* max. Chord prog. middle value */
#define MAXLVR 72       /* max. Chord prog. right value */
#define MAXRL  30000    /* max. Rule file line */
#define MAXSL  20000    /* max. Source file line */
#define MAXCSL 999      /* max. Select chord */
#define MAXVEX 10000    /* max. Vertical Exchange */
#define MAXVCH 60000    /* max. Vertical Exchange Modify str */
#define MAXDXS 200      /* max. dynamic expand symbol */
#define MAXPTN 54       /* max. articulation pattern length */
#define MAXCSP 100      /* max. chord select print */
#define MAXFN 30        // max. file name 
#define CR    0x0d
#define LF    0x0a
#define BS    0x08
#define ESC   0x1b

union MCODE {
    unsigned char wc[4];
    unsigned short wi[2];
};

union mivcode {
    unsigned char zc[4];   /* zc[1].zx[0].zc[3].zc[2] : 8086 */
    unsigned short zi[2];
    unsigned long zl;
};

typedef struct {
    int  prflg;        /* print mv->card on/off flag */
    int  ryhm;         /* rithm */
    int  time;         /* rithm time base */
    /* int  temp; */
    int  slsf;         /* select source file flag */
    int  sequ;
    int  memb;
    int  port;
    int  chan;
    int  trak;
    int  egrp;
    int  rept;
    int  kind;
    int  renp;
    int  splt;
    long psyn;
    int  bnum;
    long sync;         /* syncronize */
    long adsyn;        /* add syncronize (for beaumput bexpcopy) */
    long nval;
    long acci;
    long cval;
    int  absf;         /* absolute sub flag */
    int  trsnf;        /* transposed note lines */
    int  xltosf;       /* exchange learge brackets to square */
    int  xstolf;       /* exchange square brackets to learge */
    int  axltosf;      /* exchange learge brackets to square all */
    int  axstolf;      /* exchange square brackets to learge all */
    int  binstf;       /* bar init flag */
    int  bsbgnp;       /* beutify symbol begin point flag */
    int  bstflg;       /* beutify symbol start flag */
    int  abstflg;      /* beutify symbol all start flag */
    int  bexpsf;       /* beautify expression symbol serch flag */
    int  mnflg1;       /* beautify syncronize check on/off flag */
    int  amnflg1;      /* beautify syncronize check on/off flag all */
    int  lbdflg;       /* beautify large symbol delete on/off flag */
    int  albdflg;      /* beautify large symbol delete all on/off flag */
    int  pbdflg;       /* beautify parentesis delete on/off flag */
    int  apbdflg;      /* beautify parentesis delete all on/off flag */
    int  sbdflg;       /* beautify square symbol delete on/off flag */
    int  asbdflg;      /* beautify square symbol delete all on/off flag */
    int  clrntl;       /* beautify clear note lines on/off flag */
    int  aclrntl;      /* beautify clear note lines on/off flag all */
    int  clrstl;       /* beautify clear rest lines on/off flag */
    int  aclrstl;      /* beautify clear rest lines on/off flag all */
    int  clrsml;       /* beautify clear symbol lines on/off flag */
    int  aclrsml;      /* beautify clear symbol lines on/off flag all */
    int  clrpls;       /* beautify clear plus lines on/off flag */
    int  aclrpls;      /* beautify clear plus lines on/off flag all */
    int  clrcom;       /* beautify clear comment lines on/off flag */
    int  aclrcom;      /* beautify clear comment lines on/off flag all */
    int  clrchd;       /* beautify clear chord lines on/off flag */
    int  aclrchd;      /* beautify clear chord lines on/off flag all */
    int  delchk;       /* beautify delete chord check lines on/off flag */
    int  adelchk;      /* beautify delete chord check lines on/off flag all */
    int  delexp;       /* beautify delete expand lines */
    int  adelexp;      /* beautify delete expand lines all */
    int  clrdpl;       /* beautify clear dumper lines on/off flag */
    int  aclrdpl;      /* beautify clear dumper lines on/off flag all */
    int  clrntsf;      /* beautify clear notes on/off flag */
    int  aclrntsf;     /* beautify clear notes all on/off flag */
    int  clrontf;      /* beautify clear observed notes on/off flag */
    int  aclrontf;     /* beautify clear observed notes all on/off flag */
    int  observf;      /* observe other notes on/off flag */
    int  aobservf;     /* observe other notes all on/off flag */
    int  chdobf;       /* observe chord notes flag */
    int  achdobf;      /* observe chord notes flag all */
    int  rstmpf;       /* reset tempo values */
    int  arstmpf;      /* reset tempo values all */
    int  rssmptf;      /* reset smpte values */
    int  arssmptf;     /* reset smpte values all */
    int  rsdynaf;      /* reset dynamic values */
    int  arsdynaf;     /* reset dynamic values all */
    int  durtev;       /* modify add duration rates value */
    int  durtevd;      /* modify add duration rates value define */
    int  tpkval;       /* transpose key val */
    int  lowip;        /* low inst. position */
    int  higip;        /* high inst. position */
    int  ksin;         /* key signatur num */
    int  GTPf;         /* Gloval TransPosition flag */
    int  GTPv;         /* Gloval Transpose value */
    int  STPv;         /* Sequence Transpose value */
    int  GKTPf;        /* Gloval Key TransPosition flag */
    int  GKTPv;        /* Gloval Key TransPosition value */
    int  SKTPv;        /* Sequence Key TransPosition value */
    int  trktyp;       /* #:0  or &:1 */
    int  gtrnsv;       /* gloval transposition value */
    int  ktrnsv;       /* getinstv() transposition value */
    int  ptrnsv;       /* $ Transposition xx value */
    int  knsftv;       /* getinstv() note_shift value */
    int  strnsv;       /* shift transposition value */
    int  tpv;          /* transpose value */
    int  nsftv;        /* note_shift value */
    int  nsftvd;       /* note_shift value define */
    int  nsftf;        /* note_shift flag */
    int  ogkval;       /* original key value */
    char otrkey[8];    /* original transpose key */
    char trkey[8];     /* transpose key */
    int  trnst[2][12]; /* transpose covert table */
    int  renumf;       /* renumber bar lines flag */
    int  arenumf;      /* renumber bar lines all flag */
    int  xrenumf;      /* renumber bar lines xsequ set flag */
    int  xbnum;        /* execute change renumber bar no. */
    int  ybnum;        /* renumber start value */
    int  crenumf;      /* renumber bar lines continuously all set flag */
    int  cbnum;        /* continuously bar no. */
    int  sflame;       /* SMPTE flame (24,25,30) */
    int  smpteh;       /* SMPTE hour */
    int  smptem;       /* SMPTE minit */
    int  smptes;       /* SMPTE second */
    float smptef;      /* SMPTE flame */
    int  gsmpteh;      /* get SMPTE hour */
    int  gsmptem;      /* get SMPTE minit */
    int  gsmptes;      /* get SMPTE second */
    float gsmptef;     /* get SMPTE flame */
    int  smpsth;       /* SMPTE set hour */
    int  smpstm;       /* SMPTE set minit */
    int  smpsts;       /* SMPTE set second */
    float smpstf;      /* SMPTE set flame */
    int  smplagh;      /* SMPTE lag hour */
    int  smplagm;      /* SMPTE lag minit */
    int  smplags;      /* SMPTE lag second */
    float smplagf;     /* SMPTE lag flame */
    int  setsmpf;      /* #set smpte on/off flag */
    char bartyp;       /* bar line type */
    char mbartyp;      /* master defined bar line type */
    char bsyntyp;      /* bar line type (synth or not) */

    int  bexp;         /* beau expression */
    int  bexchf;       /* beautify exchange on/off flag */
    int  abexchf;      /* beautify exchange all on/off flag */

    int  gater;        /* beaut gate_rate */
    int  gaterf;       /* gate_rate on/off flag */
    int  agaterf;      /* gate_rate all on/off flag */

    int  pitchv;       /* default synthesizer part pitch vend */
    int  pitchf;       /* pitch vend flag */
    int  apitchf;      /* all pitch vend flag */

    int  modul;        /* synthesizer part modulation */
    int  modulf;       /* modulation flag */
    int  amodulf;      /* all modulation flag */

    int  cntlno;       /* synthesizer part control change no. */
    int  cntlnod;      /* synthesizer part control change no. define */
    int  cntlnf;       /* control number flag */
    int  acntlnf;      /* all control number flag */

    int  cntldt;       /* synthesizer part control data */
    int  cntldtd;      /* synthesizer part control data define */
    int  cntldf;       /* control data flag */
    int  acntldf;      /* all control data flag */

    int  prgchgf;      /* program change flag */
    int  aprgchgf;     /* all program change flag */

    int  transvf;      /* transpose value flag */
    int  atransvf;     /* all transpose value flag */

    float veltev;      /* modify add velocity base rates value */
    int  veltef;       /* velocity_base value flag */
    int  aveltef;      /* all velocity_base value flag */

    int  notecv;       /* note change value */
    int  notecf;       /* note change flag */
    int  anotecf;      /* all note change flag */

    int  cline;        /* conductor line count */
    int  inclf;        /* include on/off */
    char partp;        /* part type */
    int  midi;         /* exchange midi format to cueing format flag */
    int  cueing;       /* exchange cueing format to midi format flag */
    int  smpmid;       /* smpte or midi bar flag */
    int  gentyp;       /* genarate _/ note lines note type */
    int  gchdf;        /* generate global chord lines */
    int  gchdsf;       /* generate global chord select lines */
    int  gchdlf;       /* delete global chord select lines */
    int  gchdxf;       /* exchange global chord select to progression */
    int  agchdxf;      /* exchange global chord select to progression all */
    int  shiftv;       /* beau note line shift value */
    int  delbarv;      /* delete bar value */
    int  delbarf;      /* delete bar flag */
    int  addbarv;      /* add bar value */
    int  expnl;        /* expanded note lines show/hide/save flag */
    int  aexpnl;       /* expanded note lines all show/hide/save flag */
    int  dexpcl;       /* Dynamic expand symbol clear */
    int  adexpcl;      /* Dynamic expand symbol clear all */
    int  decxp;        /* Decode dynamic expande note lines */
    char rbtch;        /* rubart type char */
    int  dexpn;        /* Dynamic expand symbol number */
    int  expff;        /* load/save expand files flag */
    int  aexpff;       /* load/save expand files flag */
    int  expldf;       /* load expand file flag */
    int  expsvf;       /* save expand note line open flag */
    int  expstf;       /* Store expand flag */
    int  aexpstf;      /* Store expand flag */
    int  exprsf;       /* Register expand flag */
    int  aexprsf;      /* Register expand flag */
    int  clvelf;       /* Clear velocity flag */
    int  velv;         /* Default velocity */
    int  hcpl;         /* Hide chord progression lines */
    int  ahcpl;        /* all Hide chord progression lines */
    int  bclall;       /* Cline file check all */
} MEBVAR;

typedef struct {
    char befr[82];
    char card[82];
    char next[82];
    char next2[82];
    char temp[82];
    char incf[82];     /* include file name */
    char cw[82];       /* work */
    int  list;
    int  source;
    int  line;
    int  ipf;         /* mebspa or mebspb exchange flag */
    int  ipa;         /* mebspa line count */
    int  ipb;         /* mebspb line count */
    int  ipc;         /* mgets_meb() current count */
    int  debug;
    int  trace;
    int  cspc;        /* Chord select print count flag */
    int  cspcf;       /* Chord select print check flag */
    int  synck;       /* sync check flag */
    int  object;
    int  beaut;
    int  copyf;       /* copy or move mode flag */
    int  allf;        /* select all flag */
    int  getsym;      /* get {symbol} at part beautify */
    int  mcpos;       /* memory symbol table current position cond. */
    int  mmpos;       /* memory symbol table current position memb. */
    int  symmax;
    int  mtsymb;
    int  stemp;
    int  mkscl;
    int  mksin;
    int  mryhm;
    int  mtime;
    int  plps;        /* pedal  position */
    int  prps;
    int  mdps;        /* modulation pos */
    int  dpos;        /* dumper pos */
    int  spos;        /* shift pos */
    float velo;       /* velociti */
    float mdymc;      /* master dynamic */
    float mtemp;      /* master tempo */
    float tempo;      /* tempo */
    char dped;
    char dflg;        /* beautify line delete on/off flag */
    int  wflag;       /* work flag */
    int  mfflg;       /* main(master) file flag */
    int  hf;          /* Header change flag */
    int  ktb[78];     /* key table */
    int  bktb[78];    /* work key table */
    int  nktb[78];    /* natural key table */
    int  ktbbak[78];  /* me syousetu key */
    int  ntb[128];    /* note on/off & pos */
    int  quep;
    long ques[22];    /* queue sync */
    int  tctyp;       /* time code type 0 - 3 */
    FILE *fpo;
    FILE *fpe;
    long smpte;       /* SMPTE h + m + s + f */
    int  smpteh;      /* SMPTE hour */
    int  smptem;      /* SMPTE minit */
    int  smptes;      /* SMPTE second */
    float smptef;     /* SMPTE flame */
    int  pass;        /* beautify pass number(1st or 2,nd) */
    int  step;        /* step 1,2,3 */
    int  byps;        /* beautyfy kine bypass flag fn(xxx,xxx) */
    int  errcnt;      /* error count */
    int  wrnpf;       /* fopen worning pass flag */
    int  wrncnt;      /* worning count */
    int  msglvl;      /* message level */
    int  msgflg;      /* message on/off flag */
    int  prtlvl;      /* print level */
    int  reprm;       // renpu repeart param. add 2013.11.20
} MIVARG;

struct mivksn {
    char *keysin[2][15];
};

struct mivsym {
    char symsgn[22];
    int  symtyp;
    float symv1;
    float symv2;
    float symv3;
    char  symv4[4];
};

typedef struct {
    int  mmov[MMST];
    long mbrc[MMST];
    long msnc[MMST];
    long dblbar[100];   /* duble bar no. save */
    char msmb[MMST][MMSN];
    int  onpc[MMST];          /* on port count */
    char onpart[MMST][MAXONP][4];
    int  ssequ;         /* start sequence number */
    int  sbarn;         /* start bar number */
    int  esequ;         /* end sequence number */
    int  ebarn;         /* end bar number */
    int  dblbc;         /* duble bar count */
} MIVMST;

typedef struct {
    int  track[MXMB];
    char partf[MXMB][36];
    int  port[MXMB];
    int  channel[MXMB];
    int  member[MXMB];
    char gmvcp;         /* gloval move & copy */
    char tmvcp[36];     /* track move & copy */
    char mmvcp[MXMB];   /* member move & copy */
} MIVMXM;

typedef struct {
    int  ww;     /* |O| */
    int  w;      /*  O  */
    int  wp;     /*  O. */
    int  wpp;    /*  O.. */
    int  h;      /*  o/ */
    int  hp;     /*  o/. */
    int  hpp;    /*  o/.. */
    int  q;      /*  _/ */
    int  qp;     /*  _/. */
    int  qpp;    /*  _/.. */
    int  n8;     /*  _/) */
    int  n8p;    /*  _/). */
    int  n8pp;   /*  _/).. */
    int  n16;    /*  _/)) */
    int  n16p;   /*  _/)). */
    int  n16pp;  /*  _/)).. */
    int  n32;    /*  _/))) */
    int  n32p;   /*  _/))). */
    int  n32pp;  /*  _/))).. */
    int  n64;    /*  _/)))) */
    int  n64p;   /*  _/)))). */
    int  n64pp;  /*  _/)))).. */
    int  t8;     /*  ./) */
    int  t8p;    /*  ./). */
    int  t8pp;   /*  ./).. */
    int  t16;    /*  ./)) */
    int  t16p;   /*  ./)). */
    int  t16pp;  /*  ./)).. */
    int  t32;    /*  ./))) */
    int  t32p;   /*  ./))). */
    int  t32pp;  /*  ./))).. */
    int  t64;    /*  ./)))) */
    int  t64p;   /*  ./)))). */
    int  t64pp;  /*  ./)))).. */
    int  a8;     /*  ./> */
    int  a8p;    /*  ./>. */
    int  a8pp;   /*  ./>.. */
    int  a16;    /*  ./>> */
    int  a16p;   /*  ./>>. */
    int  a16pp;  /*  ./>>.. */
    int  a32;    /*  ./>>> */
    int  a32p;   /*  ./>>>. */
    int  a32pp;  /*  ./>>>.. */
    int  a64;    /*  ./>>>> */
    int  a64p;   /*  ./>>>>. */
    int  a64pp;  /*  ./>>>>.. */
    int  fww;     /* |O| */
    int  fw;      /*  O  */
    int  fwp;     /*  O. */
    int  fwpp;    /*  O.. */
    int  fh;      /*  o/ */
    int  fhp;     /*  o/. */
    int  fhpp;    /*  o/.. */
    int  fq;      /*  _/ */
    int  fqp;     /*  _/. */
    int  fqpp;    /*  _/.. */
    int  fn8;     /*  _/) */
    int  fn8p;    /*  _/). */
    int  fn8pp;   /*  _/).. */
    int  fn16;    /*  _/)) */
    int  fn16p;   /*  _/)). */
    int  fn16pp;  /*  _/)).. */
    int  fn32;    /*  _/))) */
    int  fn32p;   /*  _/))). */
    int  fn32pp;  /*  _/))).. */
    int  fn64;    /*  _/)))) */
    int  fn64p;   /*  _/)))). */
    int  fn64pp;  /*  _/)))).. */
    int  ft8;     /*  ./) */
    int  ft8p;    /*  ./). */
    int  ft8pp;   /*  ./).. */
    int  ft16;    /*  ./)) */
    int  ft16p;   /*  ./)). */
    int  ft16pp;  /*  ./)).. */
    int  ft32;    /*  ./))) */
    int  ft32p;   /*  ./))). */
    int  ft32pp;  /*  ./))).. */
    int  ft64;    /*  ./)))) */
    int  ft64p;   /*  ./)))). */
    int  ft64pp;  /*  ./)))).. */
    int  fa8;     /*  ./> */
    int  fa8p;    /*  ./>. */
    int  fa8pp;   /*  ./>.. */
    int  fa16;    /*  ./>> */
    int  fa16p;   /*  ./>>. */
    int  fa16pp;  /*  ./>>.. */
    int  fa32;    /*  ./>>> */
    int  fa32p;   /*  ./>>>. */
    int  fa32pp;  /*  ./>>>.. */
    int  fa64;    /*  ./>>>> */
    int  fa64p;   /*  ./>>>>. */
    int  fa64pp;  /*  ./>>>>.. */
} MEBGRT;

typedef struct {
    int  comptyp;
    int  sequ;
    int  bnum;
    long meas;
    long sync;
    int  fdtyp;
    int  fdch;
    int  keyp;
    int  voff;
    int  veon;
    int  port;
    int  trak;
    int  memb;
    int  trnsv;
    int  brself;       /* bar & movement number select flag */
    int  mbendf;       /* bar & movement end number select flag */
    FILE *fob;         /* observe fn_xyz.obj file */
    int  fobopend;     /* observe opened flag */
    FILE *fmx;         /* observe fn.mix file */
    int  fmxopend;     /* observe opened flag */
    int  obstb[128];   /* observe table */
} MEBSRG;

typedef struct {
    int  def_bexp;     /* define %xx */
    int  asn_bexp;     /* assign %xx */
    int  mod_bexp;     /* modify %xx */
    int  set_bexp;     /* set %xx */
    int  asc_bexp;     /* add/sub/clear %xx */

    int  def_rp_v;     /* define %xx [ parameter */
    int  def_rp_f;     /* define %xx [ flag */
    int  set_rp_v;     /* set    %xx [ parameter */
    int  set_rp_f;     /* set    %xx [ flag */

    int  def_gr_v;     /* define gate_rate value */
    int  def_gr_f;     /* define gate_rate flag */
    int  def_md_v;     /* define modulation value */
    int  def_md_f;     /* define modulation flag */
    int  def_pv_v;     /* define pitch_vend value */
    int  def_pv_f;     /* define pitch_vend flag */
    int  def_tp_v;     /* define transpose value */
    int  def_tp_f;     /* define transpose flag */
    float def_vb_v;    /* define velocity_base value */
    int  def_vb_f;     /* define velocity_base flag */
    int  def_vl_v;     /* define velocity value */
    int  def_vl_f;     /* define velocity flag */
    int  def_bt_v;     /* define beat value */
    int  def_bt_f;     /* define beat flag */
    int  def_hm_v;     /* define harmonize value */
    int  def_hm_p;     /* define harmonize position */
    int  def_hm_f;     /* define harmonize flag */

    int  asn_gr_v;     /* assign gate_rate value */
    int  asn_gr_f;     /* assign gate_rate flag */
    int  asn_md_v;     /* assign modulation value */
    int  asn_md_f;     /* assign modulation flag */
    int  asn_pv_v;     /* assign pitch_vend value */
    int  asn_pv_f;     /* assign pitch_vend flag */
    int  asn_tp_v;     /* assign transpose value */
    int  asn_tp_f;     /* assign transpose flag */
    int  asn_ts_v;     /* assign (key) transposition value */
    int  asn_ts_f;     /* assign (key) transposition flag */
    float asn_vb_v;    /* assign velocity_base value */
    int  asn_vb_f;     /* assign velocity_base flag */
    int  asn_vl_v;     /* assign velocity value */
    int  asn_vl_f;     /* assign velocity flag */
    int  asn_bt_v;     /* assign beat value */
    int  asn_bt_f;     /* assign beat flag */
    int  asn_hm_v;     /* assign harmonize value */
    int  asn_hm_p;     /* assign harmonize position */
    int  asn_hm_f;     /* assign harmonize flag */

    int  mod_gr_v;     /* modify gate_rate value */
    int  mod_gr_f;     /* modify gate_rate flag */
    int  mod_md_v;     /* modify modulation value */
    int  mod_md_f;     /* modify modulation flag */
    int  mod_pv_v;     /* modify pitch_vend value */
    int  mod_pv_f;     /* modify pitch_vend flag */
    int  mod_tp_v;     /* modify transpose value */
    int  mod_tp_f;     /* modify transpose flag */
    float mod_vb_v;    /* modify velocity_base value */
    int  mod_vb_f;     /* modify velocity_base flag */
    int  mod_vl_v;     /* modify velocity value */
    int  mod_vl_f;     /* modify velocity flag */
    int  mod_bt_v;     /* modify beat value */
    int  mod_bt_f;     /* modify beat flag */
    int  mod_hm_v;     /* modify harmonize value */
    int  mod_hm_p;     /* modify harmonize position */
    int  mod_hm_f;     /* modify harmonize flag */

    int  set_gr_v;     /* set/un gate_rate value */
    int  set_gr_f;     /* set/un gate_rate flag */
    int  set_md_v;     /* set/un modulation value */
    int  set_md_f;     /* set/un modulation flag */
    int  set_pv_v;     /* set/un pitch_vend value */
    int  set_pv_f;     /* set/un pitch_vend flag */
    int  set_tp_v;     /* set/un transpose value */
    int  set_tp_f;     /* set/un transpose flag */
    float set_vb_v;    /* set/un velocity_base value */
    int  set_vb_f;     /* set/un velocity_base flag */
    int  set_vl_v;     /* set/un velocity value */
    int  set_vl_f;     /* set/un velocity flag */
    int  set_bt_v;     /* set/un beat value */
    int  set_bt_f;     /* set/un beat flag */
    int  set_hm_v;     /* set/un harmonize value */
    int  set_hm_p;     /* set/un harmonize position */
    int  set_hm_f;     /* set/un harmonize flag */

    int  asc_gr_v;     /* add/sub/clear gate_rate value */
    int  asc_gr_f;     /* add/sub/clear gate_rate flag */
    int  asc_md_v;     /* add/sub/clear modulation value */
    int  asc_md_f;     /* add/sub/clear modulation flag */
    int  asc_pv_v;     /* add/sub/clear pitch_vend value */
    int  asc_pv_f;     /* add/sub/clear pitch_vend flag */
    int  asc_tp_v;     /* add/sub/clear transpose value */
    int  asc_tp_f;     /* add/sub/clear transpose flag */
    float asc_vb_v;    /* add/sub/clear velocity_base value */
    int  asc_vb_f;     /* add/sub/clear velocity_base flag */
    int  asc_vl_v;     /* add/sub/clear velocity value */
    int  asc_vl_f;     /* add/sub/clear velocity flag */
    int  asc_bt_v;     /* add/sub/clear beat value */
    int  asc_bt_f;     /* add/sub/clear beat flag */
    int  asc_hm_v;     /* add/sub/clear harmonize value */
    int  asc_hm_p;     /* add/sub/clear harmonize potioon */
    int  asc_hm_f;     /* add/sub/clear harmonize flag */
} MEBEDT;

typedef struct {
    int  exfc;         /* expand file counter */
    int  exlc;         /* expand file counter */
    int  exrc;         /* expand Register & Store files counter */
    char exfs[MAXEX][4];   /* expand file table */
    char exls[MAXEX][4];   /* expand load file table */
    char exrs[MAXEX][4];   /* expand register & store file table */
    int  esc;          /* expand symbol counter */
    char es[MAXEX][4]; /* expand symbol */
    long nval[MAXEX];  /* note value */
    int  bc[MAXEX];    /* bar(meas) count */
    int  nc[MAXEX];    /* note lines */
    int  pr[MAXEX];    /* priority */
    int  hr[MAXEX];    /* > < >= <= Compear */
    int  af[MAXEX];    /* activ flag */
} MEBPH;

typedef struct {
    int  ocpf;                     /* $R: Rule file name open flag */
    char ocp[80];                  /* Rule file name fncpx.dat */
    int  key;                      /* Major/Minor key */
    int  jky;                      /* Major key rule flag */
    int  iky;                      /* minor key rule flag */
    int  pol;                      /* Poly chord flag */
    int  caf;                      /* Chord active flag */
    char cnl[82];                  /* Chord ovserve note line */
    int  crp;                      /* Chord root position */
    int  cbp;                      /* Chord base position */
    char cvc[52];                  /* Chord velocity char */
    int  ef[MAXLV];                /* Level end flag @x:END */
    char mk[MAXLV][MAXLVL];        /* Level marker */
    int  n[MAXLV];                 /* Variable Counter */
    char l[MAXLV][MAXLVA][MAXLVL]; /* Left Value */
    char r[MAXLV][MAXLVA][MAXLVR]; /* Right Value */
    int  bf;                       /* on bass flag */
    char bg[16];                   /* on bass chord number */
    char bc[16];                   /* on bass chord name */
    int  mf;                       /* Multi Chord flag */
    char star[8];                  /* Multi Chord str */
    int  scnvf;                    /* Select chord Name voicing flag */
    int  ascnvf;                   /* Select chord Name voicing flag all */
    int  chgf;                     /* Change chord progression lines */
    int  achgf;                    /* Change chord progression lines all */
    int  npc;                      /* Next progression chord */
    int  nnc;                      /* Number of next chord */
    char npcd[10][MAXLVL];         /* Next progression chord name */
} MEBCPS;

typedef struct {
    float klp;                /* Note length */
    float knp;                /* Key position */
    float kvp;                /* Velocity */
    float kgc;                /* Global Current Chord */
    float kgb;                /* Global Before Chord */
    float kcp;                /* Current Chord */
    float kbp;                /* Before Chord */
    float kap;                /* After Chord */
    float bv[128];            /* before note */
    float av[128];            /* after note */
    float cv[128];            /* current note */
    float cn[128];            /* chord note */
    int   sck;                /* select chord count */
    float scv[MAXCSL];        /* select chord value */ 
    char  scn[MAXCSL][MAXLVL];     /* select chord name */
    int   scp[MAXCSL];        /* select priority */
    char  mk[MAXLVL];         /* $6: Level marker */
    int   n;                  /* Variable Counter */
    char  l[MAXLVA][MAXLVL];  /* $6: Left Value */
    char  m[MAXLVA][MAXLVM];  /* $6: Middle Value */
    char  r[MAXLVA][MAXLVR];  /* $6: Right Value */
} MEBCSL;

typedef struct {
    int  nsf;                /* Availabe Note Scale flag */
    int  ansf;               /* Availabe Note Scale flag all */
    int  scvf;               /* Select Chord voicing active flag */
    int  arp;                /* Available Note Scale root position */
    int  abp;                /* Available Note Scale base position */
    int  aaf;                /* Available Note Scate active flag */
    char anl[82];            /* Available Note Scale line */
    char avc[62];            /* Available Note Scale velocity char */
    int  n;                  /* Variable Counter */
    int  cvpf;               /* Chord voicing pattern active flag */
    char nkey[MAXLVM];       /* $N: key name */
    char skey[MAXLVM];       /* $N: serch key */
    char cdinst[MAXLVM];     /* $N:xx_major/minor_instruments */
    int  nvf;                /* Chord Name Voicing velocity flag */
    char nvc[62];            /* Chord Name Voicing velocity char */
    char l[MAXLVA][MAXLVL];  /* $x: Left Value */
    char m[MAXLVA][MAXLVM];  /* $x: Middle Value */
    char r[MAXLVA][MAXLVR];  /* $x: Right Value */
    char o[MAXLVA][MAXLVR];  /* $A: Over Value */
} MEBANS;

typedef struct {
    float grate;       /* Gate rate */
    int  dyna;
    int  keyp;
    int  veon;
    int  tpv;          /* transpose value */
} MECVAR;

/* Vartical Exchange vx-> */
typedef struct {
    int  gulf;              /* Upper Lower case flag */
    int  tulf;              /* Upper Lower case flag */
    int  dulf;              /* Upper Lower case flag */
    int  vulf;              /* Upper Lower case flag */
    int  pulf;              /* Upper Lower case flag */
    int  mulf;              /* Upper Lower case flag */
    int  gvc;
    int  tmc;
    int  dxc;
    int  vlc;
    int  pvc;
    int  mdc;
    char vgv[MAXVEX][8];    /* Gloval_Velocity */
    char vtm[MAXVEX][8];    /* Tempo */
    char vdx[MAXVEX][6];    /* Dynamic Exchange */
    char vvl[MAXVEX][6];    /* Velocity_Rate */
    char vpv[MAXVEX][8];    /* Pitchvend */
    char vmd[MAXVEX][4];    /* Modulation */
} MEBVEX;

typedef struct {
    int  nmb;             /* number */
    int  af;              /* activ flag */
    int  pmf;             /* plus minus flag */
    char cdxs[4];         /* current dxs */
    int  ctyp;            /* current type */
    char crt[MAXPTN];     /* current ryhthm pattern */
    char cnt[MAXPTN];     /* current Notes pattern */
    char cvt[MAXPTN];     /* current Velocity pattern */
    int  typ[MAXDXS];     /* arpeggio fixed or split variable */
    char dxs[MAXDXS][4];  /* Dynamic expand symbol name */
    char rt[MAXDXS][MAXPTN];  /* ryhthm pattern */
    char nt[MAXDXS][MAXPTN];  /* Notes pattern */
    char vt[MAXDXS][MAXPTN];  /* Velocoty pattern */
} MEBART; /* Dynamic Expand (Articulation) */

// main.c
int mebrmvf_meb(char mfile[STLN]);
int mebmain(char *inf);
int initval_meb();
int startval_meb();
int edtval_init();
int initcp_meb();
int mopen_meb();
int mprintf_meb(char card[]);
int mgets_meb(char card[82]);
int mcopy_meb(char outf[STLN]);
int help_meb();
int fhelp_meb(int help);

// meb1.c
int mbeautif_meb(char *inf,struct mivksn *mk,struct mivsym ms[],int gsf);
int pbeautif_meb(char *inf,struct mivksn *mk,struct mivsym ms[]);
int SetRenumStartVal_meb();

// meb2.c
int header_meb(char mfile[STLN]);
int modmit_meb();
int putcline_meb();
int chgfn_meb(char card[82]);
int cpfn_meb(char card[82]);
int instout_meb(char *inf,int sf);
int getdat_meb(FILE *fpg,int port,int chan,char wk[],char cm[],char pcg[]);
int mcline_meb();
int beaucl_meb();
int getcline_meb(char *fn,char *btyp,int *chan,int *port,int *memb);
int bottom_meb();
int beaudpset_meb();
int keysgn_meb(struct mivksn *mk);
int lbrtosbr_meb();
int sbrtolbr_meb();
int mebfchk_meb(char mfile[STLN],char partc);
int beauexpr_meb(struct mivsym msi[]);
int getinstv_meb(struct mivsym ms[]);
int barinst_meb();
int beauspace_meb(char card[82]);
int mgetsym_meb(char *inf,struct mivksn *mk,struct mivsym ms[]);
int selfil_meb(char *mfile);
int mebcopy_meb(char *ofile,char *sfile);
int rename_meb(char *sfile,char *dfile);
int strcat_h(char *tos,char *from);
int mebrgen_meb(int *genloop);
int sexclsum_meb();
FILE *fopen_h(char *file, char *mode);

// meb3,c
int mainbar_meb();
int pcasebar_meb();
int pcaseper_meb(FILE *fpi);
int duration_meb();
int edc_velocity();
int ed_velocity_base();
int pcasesh_meb();
int pcaserst_meb();
int pcasebeu_meb(struct mivksn *mk,struct mivsym ms[],char obsf[STLN]);
int keychk_meb(struct mivksn *mk);
int pcaselbu_meb(struct mivksn *mk,struct mivsym ms[]);
int modcard_meb();
int rensync_meb(FILE *fpi);
int renget_meb();
int notesync_meb();
int nsync2_meb();
int restsync_meb();
int beaudymc_meb();
int beaupos_meb();
int beaurep_meg(char card[82]);
int beaugkey_meb(struct mivksn *mk);
int beaukey_meb(struct mivksn *mk);
int key_meb();
int key2_meb();
int beaubar_meb();
int beauabar_meb();
int beaunbar_meb();
int modkey_meb(char keysgn[]);

// meb4.c
int beausyms_meb();
int beauasym_meb();
int barsync_meb();
int beaumput_meb();
int beaumov_meb();
int bexpcopy_meb();
int mkexptmb_meb(char *inf,struct mivsym ms[]);
int keyinimb_meb();
int nkeyini_meb();
int keysetmb_meb();
int numdinmb_meb(char *card);
int rengetmb_meb();
int mktrkey_meb(int trv);
int beautrns_meb();
int tnshift_meb(int osp);
int transp_meb(char wk[82],int i,int tpv,int sfv,int bf);
char getkch_meb(int kv,char c1);
char sgetkch_meb(char c1,int i,int ssv);
char getpvch_meb(int tiv,int tpv,char c1);
int keytrnsp_meb(struct mivksn *mk);
int init_gate_rate();
int gate_rate_get();
int def_gr_get();
int asn_gr_get();
int mod_gr_get();
int set_gr_get();
int asc_gr_get();
int def_gate_rate(char *card);
int asn_gate_rate(char *card);
int mod_gate_rate(char *card);
int set_gate_rate(char *card);
int asc_gate_rate(char *card);
int tncheck_men(int osp,int tpv);
int tlshift_meb(int tpv,int i);
int syndefgr_meb();
int synasngr_meb();
int synmodgr_meb();
int synsetgr_meb();
int synascgr_meb();
int gr_w_set(char *w);

// meb5.c
int mksym_miv(struct mivksn *mk);
int chkmfile_miv(char *fname);
int beauexp_meb();
int beauexp_sub1(char *cw);
int beauexp_sub2(char *cw);
int beauexp_sub3(char *cw);
int beauexp_sub4(char *cw);
int beauexp_sub5(char *cw);
int beaucsym_meb();
int beautemp_meb();
int exptmp_meb();
int exptmp2_meb();
int beauadd_meb();
int beaumod_meb();
int beaupula_meb(char *inf);
int beauback_meb(char *outf);
int strck_meb();
int strck2_meb();
int smpflame_meb();
int beaumset_meb();
float fnumdin_meb(char *card);
int bsyntyp_meb();

// meb6.c
int clrnotes_meb();
int clronts_meb();
int noteshft_meb();
int lnshift_meb(int osp,int oep,int tsp,int tep,int sfv);
int barcheck_meb();
int excsmpbr_meb();
int excmidbr_meb();
int excmidnl_meb();
int excsmpnl_meb();
int sequchg_meb();
int bedymst_meb();
int beauspvl_meb();
int beausynt_meb();
int ed_pitch_vend();
int ed_modulation();
int edc_transpose();
int ed_transpose();
int ed_velocity();
int get_npv_meb(int pf,int *pos,int *val);
int beaurtmv_meb();
int mkmultif_h(char *infn,char *outf,char *ftyp,int fnmb,
    int *prtn,int *yfile,int *xfile);
int mkyxfnum_h(char fname[STLN],int *yfile,int *xfile,char *ps);
int filemake_h(int n,char infi[],char fext[],char dfile[],
    int *yflp,int *xflp);
int smpteset_meb(int schfg);
int smptaget_meb();
int smpteget_meb();
int smptsval_meb();
int smptgval_meb();
int beauobsv_meb();
int beauobst_meb();
int compchk_mlk(union MCODE *um,unsigned char iwordi[4]);
int fbdecode_mlk(union MCODE *u);
int fcdecode_mlk(union MCODE *u);
int fddecode_mlk(union MCODE *u);
int dddecode_mlk(union MCODE *u);

// meb7.c
long getnval_meb(char card[]);
int setbtitl_meb(char card[82],char *inf);
int dblbsv_meb();
int dblbchk_meb();
int sglbchk_meb();
int sglbar_meb();
int dblbar_meb();
int addbar_meb();
int genbar_meb(char bartyp,char card[82]);
int beautmvc_meb(char mfile[STLN],int cline);
int beaummvc_meb(char mfile[],int cline);
int seqshift_meb(char mfile[STLN],int cline);
int gserch_meb(char mfile[STLN],int cline);
int gclear_meb(char mfile[STLN],int cline);
int beaufras_meb();
int nbchg_meb(char wk[82]);
int bfunc_meb(char s[82],int *pv1,int *pv2);
int setbyps_meb();
int incfile_meb();

// meb8.c
int setexp_meb(char *inf);
int savexs_meb(char *card,char exp[4]);
int chkes_meb();
int setes_meb(int k);
int expandn_meb(char *inf);
int loadexpf_meb(char expf[MAXFN]);
int loadsym_meb(char expf[MAXFN]);
int savesym_meb(char expf[MAXFN]);
int showexp_meb(char expf[MAXFN]);
int inclexp_meb(char expf[MAXFN]);
int verln_meb(int rf,char line[82],int lc);
int hidexp_meb();
int rstexp_meb();
int savexp_meb(char expf[MAXFN]);
int mkexpfn_mmm(char *inf,char outf[MAXFN],char *str);
int getmph_meb(char *inf,char card[82],char exp[4]);
int expcard_meb(char *inf,char card[82],char exp[4],char expf[STLN]);
int expserch_meb(char *text,FILE *mph,char exp[4],char exlin[82]);
int txserch_meb(char line[82],char *text);
int brepear_meb();
int svexrs_meb();
int storexp_meb(char inf[STLN],char expf[STLN]);
int regstxp_meb(char inf[STLN],char expf[STLN]);
int repexp_meb(char exf[],char expf[STLN],char phf[STLN],char tmpf[STLN]);
int getexpfl_meb(char expf[STLN],char expl[82]);
int arpeggio_meb();
int cldexp_meb();
int dynaexp_meb();
int artexp_meb();
int setdexs_meb();
int addgsym_meb(char *inf,struct mivksn *mk,struct mivsym ms[],char obsf[STLN]);
int add_sl_col(char *card);
int define_meb(char *card,char *work);
int assign_meb(char *card,char *work);
int modify_start_meb(char *card,char *work);
int modify_end_meb(char *card,char *work);
int set_meb(char *card,char *work);
int unset_meb(char *card,char *work);
int add_sub_clear(char *card,char *work,int f);
int setstr_meb(char *card,char *wk);
int getGKTPv_meb(struct mivksn *mk);
int setorgktb_meb(struct mivksn *mk);
int nktbdmp_meb(struct mivksn *mk);
int bktbdmp_meb(struct mivksn *mk);
int ktbdmp_meb(struct mivksn *mk);
int mkotrkey_meb(int trv);

// meb9.c
int gchdsel_meb();
int beaugsln_meb();
int chdsel_meb(struct mivksn *mk,int p);
int mkscvt_meb();
int mkscpt_meb();
int beausln_meb(char sline[82]);
int setscn_meb(char cnb[16],char chd[16]);
int setcn_meb(char cnb[16],char bcd[16],char chd[16]);
int mchdsel_meb(char *inf,struct mivksn *mk,struct mivsym ms[]);
int pchdsel_meb(struct mivksn *mk,struct mivsym ms[]);
int bfget_meb(FILE *fpi,struct mivksn *mk,struct mivsym ms[]);
int keyln_meb(struct mivksn *mk);
int afget_meb(FILE *fpi,struct mivksn *mk);
int aafget_meb(FILE *fpi,struct mivksn *mk);
int getNotew_meb();
float getselp_meb(char wline[82],char *prm,float vf);
int lineout_meb(char line[82],struct mivksn *mk,struct mivsym ms[]);
int xtransp_meb(struct mivksn *mk);
int fprint_meb(char line[82]);
int setcnmsg_meb(int errf);
int etcard_meb(struct mivksn *mk);
int setbnum_meb();
int mixchdsl_meb();
int gfdset_mlk(union MCODE *u);
int gvnote_meb();
int chdkey_mec(struct mivksn *mk);
int keyini_mec();
int nkeyini_mec();
int keyset_meb();
int keydmp_mec(struct mivksn *mk);
int notel_meb(struct mivksn *mk,char cf);
int noteon_meb(char cf);
int vnote_meb(char cf);
int modline_meb(char line[82],char *msg);
int startkp_mec(int *startp,int *lastp);
int syncetrs_meb();
int accid_meb(struct mivksn *mk);
int csaccid_meb(struct mivksn *mk);
int gexpclr_meb();

// meb10.
int setavns_meb(char skey[MAXLVL]);
int chkaval_meb(int sf,int rt,char Note[MAXLVR]);
int getaval_meb(char Note[MAXLVR]);
int maget_cps(char tk[MAXLVR],char rtey[MAXLVR]);
int setaval2_meb(int sf,int rt,int iv,char Nc);
int setaval3_meb(int rt,int iv,char Nc);
int trnstep_meb(char *inf,struct mivksn *mk,struct mivsym ms[]);
int chdNote_meb();
int avlNote_meb();
int chordsel_cps();
int objchdsl_meb();
int insutrns_meb();
int GTrans_meb();
int trnskey_meb(struct mivksn *mk);
int chdkey2_mec(struct mivksn *mk);
int Special_key(char line[82],char skey[MAXLVL],char wkey[MAXLVL]);
int cdcheck_meb(char skey[MAXLVL],char ck[16],char cd[16]);
int getslsr_meb(char line[82],char sl[MAXLVL],char sr[MAXLVR]);
int srcard_meb(char ck[16],char cd[16],char sr[MAXLVR],char card[82]);
int tkmod_meb(char ck[16],char tk[MAXLVR]);
int cpfind_cps(char tk[MAXLVR],char rtey[MAXLVR]);
int Poly_key(char skey[MAXLVL],char wkey[MAXLVL],char bc[8],char bk[8]);
int upchk_meb(char wk[],char bc[],char bk[]);
int Polyobs_meb();
int verexp_meb();
int tmpovex_meb(char aw[MAXVCH]);
int dynavex_meb(char aw[MAXVCH]);
int glvdvex_meb(char aw[MAXVCH]);
int velovex_meb(char aw[MAXVCH]);
int ptchvex_meb(char aw[MAXVCH]);
int moduvex_meb(char aw[MAXVCH]);
int lexpand_meb(char aw[MAXVCH],char bw[MAXVCH]);
int vspace_meb(char line[MAXVCH]);
int vprint_meb(char *aw,char wk[MAXVCH]);
int vermod_meb();
int vlcmod_meb();
int dxcmod_meb();
int pvcmod_meb();
int mdcmod_meb();
int mexpmod_meb();
int mvlcmod_meb();
int tmcmod_meb();
int gvcmod_meb();
int vexpspace_meb(char card[160]);

// meb11.c
int modsmpte_meb(char *inf,struct mivksn *mk,struct mivsym ms[]);
int finstep_meb(char *inf,struct mivksn *mk,struct mivsym ms[]);
int finexp_meb();
int mdecdxp_meb();
int decdxp_meb();
int mdecnote_(char *nsym,int adj,int cnp);
int decnote_(char *nsym,int adj,int cnp);
int mexpnote_meb(int sp,char line[82],int cnp);
int expnote_meb(int sp,char line[82],int cnp);
int addexp_meb();
int setartv_meb(char dxs[4]);
int spacecmp_mec(char *card,int len);
int overerr_meb(char msg[],int ov);
int creen_meb();
int gr_rp_set();
int gr_p_set(char *w,int rep);

// meb12.c
int chord_cps();
int chdobsev_meb();
int getchord_meb(int acf,char Cn[8]);
int chkival_meb(int sf,int rt,char Note[MAXLVR]);
int setival2_meb(int sf,int rt,int iv,char Nc,char rch);
int setival3_meb(int rt,int iv,char Nc);
int getRoot_meb(int acf,int *rt);
int modRoot_meb(int *rt);
int getkeyp_meb(int p,int nbc);
int setlvl_cps();
int getRcpn_cps();
int setnolv_cps();
int cpserch_cps(char skey[MAXLVL]);
int cpget_cps(char tk[MAXLVR],char rtey[MAXLVR]);
int modget_cps(char tk[MAXLVR],char mk[MAXLVR],char rk[MAXLVR]);
int getmodk_meb(int mnf,char c_key[4],int mod_kn,char rk[MAXLVR]);
int cpserch1_cps(char line[82]);
int cpserch2_cps(char cn[MAXLVL],char wk[82]);
int tkcheck_meb(char tk[MAXLVR],char cd[16]);
int cpserch3_cps(char skey[MAXLVL],char wkey[MAXLVL]);
int setkey_cps();
int setlv0_cps(char wk[]);
int setlv1_cps(char skey[]);
int setlv2_cps(char skey[]);
int linechk_cps(char line[82],char skey[]);
int setval_cps(char line[82],int lv);
int setval6_cps(char line[82]);
int opencp_meb();
int setlv3_cps(char skey[]);
int setlv4_cps(char skey[]);
int setlv5_cps(char skey[]);
int setlv6_cps(char skey[]);
int strcat_cps(char card[],char wk[]);
int getival_meb(int kp,char Note[MAXLVR]);
int bgchk_meb(char bc[16],char bk[16]);
int setlvA_cps(char skey[]);
int setavals_cps(char line[82]);
int setnpcd_meb(char wk[82]);
int chprgres_meb();
int chgserch_cps(char skey[MAXLVL]);
int basechk_meb(char skey[MAXLVL],char wkey[MAXLVL]);
int hlponly_cps(char skey[MAXLVL]);
int serial_meb(char card[82]);

// mebw.c
int selcnv_meb();
int gCname_meb();
int setlvN_cnv();
int setvalN_cnv(char *line);
int setinst_cnv();
int getnkey_cnv();
int setskey_cnv();

///////////////END///meb.h/////////
