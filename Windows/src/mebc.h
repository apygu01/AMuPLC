
#define TACTDIR "/usr/local/etc/tact/"
#define MXMB  256       /* max. member */
#define MAXA  20        /* max. argument number */
#define STLN  20        /* string get length */
#define CR    0x0d
#define LF    0x0a
#define BS    0x08
#define ESC   0x1b
#define STCTYP 3        /* smpte time code type 30 flame/sec(non dorop) */
#define MFRS  20        /* max. phrase words */
#define MAXSC 100       /* maximam save card line number */
#define MAXNX 500       /* maximam next card */
#define MAXEXF 200      /* max. expand symbol */
#define MAXDXS 200      /* max. dynamic expand symbol */
#define MAXPTN 54       /* max. articulation pattern length */ 
#define MAXAR 54        /* max. articulation table */ 
#define MAXNC 24        /* max. articulation notes */
#define MAXDV 256       /* max. articulation delta note values */

/* Controller numbers table */
#define MODUR 1         /* moduration depth */
#define POLTM 5         /* poltament time */
#define DENTR 6         /* data entry (RPN/NRPN) */
#define VOLUM 7         /* main volume */
#define BALAN 8         /* balance controle */
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
    int  mdps;        /* moduration pos */
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
    int  bitrns;      /* beau instrments transpose val */
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
    long bnum;
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
    int  mitv;        /* moduration init value */
    int  citv;        /* controle init value */
    int  pgiv;        /* program change init value */
    long tcnt;
    int  ffflg;
    int  lag;         /* gloval & local lag */
    int  lagb;        /* in bar lag */
    int  offset;      /* gloval & local offset */
    int  offsetb;     /* in bar offset */
    int  sutyp;       /* shift unit type */
} MECVAR;

typedef struct {
    char symsgn[MEXP][22];
    int  symtyp[MEXP];
    float symv1[MEXP];
    float symv2[MEXP];
    float symv3[MEXP];
    char  symv4[MEXP][4];
} MECSYM;

typedef struct {
    int  smovn;         /* start movment number */
    long sbarn;         /* start bar number */
    int  emovn;         /* end movment number */
    long ebarn;         /* end bar number */
    int  comptyp;
    long bnum;
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
    int  typ[MAXDXS];     /* arpeggio fixed or split variable */
    char dxs[MAXDXS][4];  /* Dynamic expand symbol name */
    char rt[MAXDXS][MAXPTN];  /* ryhthm pattern */
    char nt[MAXDXS][MAXPTN];  /* Notes pattern */
} MECART; /* Dynamic Expand (Articulation) */

typedef struct {
   int  sc;           /* Split count */
   int  dvc;          /* dv count */
   int  nc[MAXAR];    /* Note count */
   int  rc[MAXAR];    /* Repeat note count */
   int  dc[MAXAR];    /* delta note count */
   int  dv[MAXDV];    /* delta note value */
   int  sync[MAXDV];  /* delta sync */
   int  dnc[MAXDV];   /* delta note count */
   int  np[MAXDV][MAXNC]; /* Note position */
   int  nv[MAXDV][MAXNC]; /* Note value */
} MECON;

typedef struct {  
   char mc[MAXAR];    /* Marc char */
   int  rs[MAXDV];    /* repeat dv sum */
   int  dv[MAXDV];    /* delta note value */
   int  sync[MAXDV];  /* delta sync */
   int  dnc[MAXDV];   /* delta note count */
   int  np[MAXDV][MAXNC]; /* Note position */
   int  nv[MAXDV][MAXNC]; /* Note value */ 
} MECOFF;

