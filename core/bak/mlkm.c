
// ----------------------
//  AMuPLC (mlkm.c)
// ----------------------

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

#include "mlk.h"
/* mlkm.c */
/* int _stack = 60000;      DOS stack size up to 65536 */
MLKSRG *ls;
MLKVAR *lv;
MLKFRS *lr;
MECMST *mm;
MLKCDE *lc;
FILE *fpr;
struct SFILE {
    unsigned char cap[MAXF];            /* type C S M */
    unsigned char chan[MAXF];           /* flag & MIDI channel */
    int  port[MAXF];           /* port */
    unsigned int  ship[MAXF];  /* file number */
} fs;
int  ships;           /* files */
int  pships;          /* port files */
int  slsf;            /* kind of file select */
int  maxps;           /* max. port select */

int main(int argc,char *argv[])
//int argc;
//char *argv[];
{
    int  i,j,k;
    int  c;
    int  ckfn[MAXF];
    char mfile[STLN];
    char dfile[STLN];
    char *pa;
    char ws[4];
    char *wh;             /* hex code */
    char *wp;
    int  wf;              /* argument check flag */
    int  helpf;
    unsigned char iword[4];
    char dtopt[STLN];     /* debug or trace option */
    char inf[STLN];
    MLKSRG lsa;
    MLKVAR lva;
    MLKFRS lfr;
    MECMST fls;   /* file line select */
    MLKCDE lca;
    int  yrtn;
    int  xrtn;
    int  yfile;
    int  xfile;
    char *xmf;
    int  yflp;
    int  xflp;
    int  link;
    int  slslf;
    char wk[8];
    int  pas;
    char wwk[8];

    ls = &lsa;
    lv = &lva;
    lr = &lfr;
    mm = &fls;
    lc = &lca;
    helpf = 0;
    yrtn = 0;
    xrtn = 0;
    yfile = 0;
    xfile = 0;
    yflp = 0;
    xflp = 0;
    ls->exef = 1;
    ls->idump = 0;
    ls->odump = 0;
    ls->idchk = 0;
    ls->dchk = 0;
    ls->dckn = 0;
    ls->debug = 0;
    ls->trace = 0;
    ls->optm = 1;
    ls->otrace = 0;
    ls->outfm = 4;
    ls->score = 1;
    ls->mergef = 1;
    ls->repla = 0;
    ls->incld = 0;
    ls->excld = 0;
    ls->lnkmon = 0;
    ls->errcnt = 0;
    ls->wrncnt = 0;
    ls->prtlvl = 1;
    ls->eosf = 0;
    lv->fix = 0;          /* SMF Score option */
    lv->tctyp = 3;        /* 30 (non drop) flame/sec */
    lv->sflame = 30;      /* default 30 non drop flame */
    lv->mtcres = 120;     /* MTC resolution 1/120 flame */
    lv->smfdiv = 480;     /* 480 per quarter-note at _/ = 120 */
    /* lv->mexres = 100;     mex Frame resolution 1/100 flame */
    /* lv->mexres = 3600;    mex resolution 1/3600 Sec */
    /* lv->mexres = 3000;    mex resolution 1/3000 Sec 1/100 flame */
    lv->mexres = 2400;     /* mex resolution 1/3000 Sec 1/80 flame */
    lv->mlp = 1;
    lv->cval = 400000;
    lv->gcval = 400000;   /* $4/4 default value (get from f8 code) */
    lv->time = 1;
    link = 1;
    wh = "0123456789abcdef";
    if(argc <= 1) {
        mlkhelp();
        return(0);
    }
    ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"PC9801")) ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"IBMPC"))  ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"SUN"))    ls->comptyp = 1;
    if(argc > 1) {
        for(i = 1; i < argc; i++) {
            k = i;
            pa = argv[i];
            j = 0;
            ckfn[i] = 1;
            while(*pa) {
                dtopt[j] = *pa;
                /* dtopt[j] = toupper(*pa); */
                j++;
                if(j > 10) break;
                pa++;
            }
            dtopt[j] = '\0';
            if(!strcmp(dtopt,"-h")) helpf = 1;     /* help */
            if(!strcmp(SUPPORT,"Debug")) { // add 2013.12.4
            if(!strcmp(dtopt,"-ne")) ls->exef = 0; /* no execute file */
            if(!strcmp(dtopt,"-mg")) ls->mergef = 1;  /* merge mode out */
            if(!strcmp(dtopt,"-sp")) ls->mergef = 0; /* split mode out */
            if(!strcmp(dtopt,"-idp")) ls->idump = 1; /* input data dump */
            if(!strcmp(dtopt,"-odp")) ls->odump = 1; /* output data check */
            if(!strcmp(dtopt,"-fix")) {
                lv->smfdiv = 480;   /* 480 per quarter-note at _/ = 120 */
                lv->fix = 1;
            }
            if(!strcmp(dtopt,"-idc")) {  /* input data check */
                ls->idchk = 1;
                ls->dchk = 1;
            }
            if(!strcmp(dtopt,"-old")) {
                lv->smfdiv = -30;    /* MTC 30 flame per second */
                lv->mtcres = 120;    /* MTC resolution 1/120 flame */
                ls->outfm = 1; /* old .mex */
            }
            if(!strcmp(dtopt,"-mtc")) {
                ls->outfm = 0; /* mtc .mex */
            }
            if(!strcmp(dtopt,"-new")) {
                ls->outfm = 4; /* new .mex */
            }
            if(!strcmp(dtopt,"-def")) {
                ls->outfm = 4; /* new .mex */
            }
            if(!strcmp(dtopt,"-mtc10")) {           /* .mid */
                lv->smfdiv = -30;    /* MTC 30 flame per second */
                lv->mtcres = 10;     /* MTC resolution 1/10 flame */
                ls->outfm = 2;       /* output file .mid */
            }
            if(!strcmp(dtopt,"-nscore")) ls->score = 0; /* FA F8 to .mex */ 
            if(!strcmp(dtopt,"-smf0")) ls->outfm = 2; /* .mid */ 
            if(!strcmp(dtopt,"-smf1")) ls->outfm = 3; /* output format */
            if(!strcmp(dtopt,"-dc")) ls->dchk = 1;   /* data check */
            if(!strcmp(dtopt,"-d"))  ls->debug = 1;  /* debug */
            if(!strcmp(dtopt,"-d1")) ls->debug = 1;  /* debug 1 */
            if(!strcmp(dtopt,"-d2")) ls->debug = 2;
            if(!strcmp(dtopt,"-d3")) ls->debug = 3;  /* .mlk check */
            if(!strcmp(dtopt,"-d4")) ls->debug = 4;  /* .mex check */
            if(!strcmp(dtopt,"-d5")) ls->debug = 5;  /* slsf check */
            if(!strcmp(dtopt,"-d6")) ls->debug = 6;  /* ddcode check */
            if(!strcmp(dtopt,"-d7")) ls->debug = 7;  /* movn & bar select chk */
            if(!strcmp(dtopt,"-d9")) ls->debug = 9;  /* -new -opt1 chk */
            if(!strcmp(dtopt,"-d8")) {
                ls->debug = 8;  /* smf 0 check */
                /* ls->outfm = 2; -smf0 option */
            }
            if(dtopt[0] == '-' && dtopt[1] == 'r' && dtopt[2] == 'e' && 
              dtopt[3] == 's') { /* -resxxx */
              lv->mexres = fresopt_mex(dtopt);
              if(ls->debug) printf("Resolution lv->mexres=%f Sec\n",lv->mexres);
            }
            if(dtopt[0] == '-' && dtopt[1] == 'd' && 
               dtopt[2] == 't') { /* -dtxxx */
               ls->outfm = 2;
               lv->smfdiv = dtoption_mex(dtopt);
               if(ls->debug) printf("Division(dt) = %d\n",lv->smfdiv);
            }
            if(!strcmp(dtopt,"-t"))  ls->trace = 1;  /* trace */
            if(!strcmp(dtopt,"-opt0")) ls->optm = 0; /* optimize 1 */
            if(!strcmp(dtopt,"-opt1")) ls->optm = 1; /* optimize 1 */
            if(!strcmp(dtopt,"-opt2")) ls->optm = 2; /* optimize 1 */
            if(!strcmp(dtopt,"-opt3")) ls->optm = 3; /* optimize 1 */
            if(!strcmp(dtopt,"-ot")) ls->otrace = 1; /* out trace */
            if(!strcmp(dtopt,"-ot8")) {
                ls->otrace = 8;  /* trace */
                /* ls->outfm = 1; */
            }
            if(!strcmp(dtopt,"-ot9")) {
                ls->otrace = 9;  /* trace */
                ls->outfm = 2; /* -smf0 option */
            }
            if(!strcmp(dtopt,"-r"))  ls->repla = 1;  /* replace */
            if(!strcmp(dtopt,"-i"))  ls->incld = 1;  /* include */
            if(!strcmp(dtopt,"-e"))  ls->excld = 1;  /* exclude */
            if(!strcmp(dtopt,"-lm")) ls->lnkmon = 1;
            if(!strcmp(dtopt,"-m0")) ls->prtlvl = 0;  /* all message */
            if(!strcmp(dtopt,"-m1")) ls->prtlvl = 1;  /* Default level 1 */
            if(!strcmp(dtopt,"-m2")) ls->prtlvl = 2;  /* level 2 */
            if(!strcmp(dtopt,"-nm")) ls->prtlvl = 99; /* no message */
            pa = argv[i];
            if(*pa == '-') {
                pa++;
                if(*pa == 'x') {
                    pa++;
                    wf = 0;
                    wp = wh;
                    for(j = 0; j < 16; j++,wp++) if(*pa == *wp) wf = 1;
                    ws[0] = *pa;
                    pa++;
                    wp = wh;
                    for(j = 0; j < 16; j++,wp++) if(*pa == *wp) wf = 1;
                    if(!wf) {
                        ls->errcnt++;
                        ls->msglvl = 3;
                        if(ls->msglvl >= ls->prtlvl)
                        printf("Error! argument \"%s\"\n",argv[i]);
                    }
                    ws[1] = *pa;
                ws[2] = '\0';
                sscanf(ws,"%02x",&c);
                c &= 0x00ff;
                ls->dckcd[ls->dckn] = c;
                ls->dckn++;
                }
            }
            } // add 2013.12.4
            else { // Relese add 2015.8.17
                if(!strcmp(dtopt,"-smf0")) ls->outfm = 2; /* .mid */
                if(!strcmp(dtopt,"-smf1")) ls->outfm = 3; /* output format */
                if(!strcmp(dtopt,"-opt0")) ls->optm = 0; /* optimize 1 */
                if(!strcmp(dtopt,"-opt1")) ls->optm = 1; /* optimize 1 */
                if(!strcmp(dtopt,"-opt2")) ls->optm = 2; /* optimize 1 */
                if(!strcmp(dtopt,"-opt3")) ls->optm = 3; /* optimize 1 */
                if(!strcmp(dtopt,"-ot")) ls->otrace = 1; /* out trace */
                if(!strcmp(dtopt,"-ot8")) {
                    ls->otrace = 8;  /* trace */
                    /* ls->outfm = 1; */
                }
                if(!strcmp(dtopt,"-ot9")) {
                    ls->otrace = 9;  /* trace */
                    ls->outfm = 2; /* -smf0 option */
                }
                if(!strcmp(dtopt,"-r"))  ls->repla = 1;  /* replace */
                if(!strcmp(dtopt,"-i"))  ls->incld = 1;  /* include */
                if(!strcmp(dtopt,"-e"))  ls->excld = 1;  /* exclude */
            } // add 2015.8.17

            if(dtopt[0] == '&') ckfn[i] = 0;
            if(dtopt[0] == '-') ckfn[i] = 0;
        }
        if(helpf) {
            hlpread_mlk();
            return(0);
        }
        ls->msglvl = 0;
        iword[0] = 'd';
        iword[1] = 'u';
        iword[2] = 'm';
        iword[3] = 'y';
        lv->mlp = lv->mexres / (lv->sflame*100.0);
        for(i = 1; i < argc; i++) {
            /* printf("i=%d ckfn=%d\n",i,ckfn[i]); */
            if(ckfn[i]) {
                ships = 0;
                mm->brself = 0;        /* bar number select flag */
                mm->mbendf = 0;        /* bar number end select flag */
                mm->smovn = 1;         /* start movment number */
                mm->sbarn = 1;         /* start bar number */
                mm->emovn = 99;        /* end movment number */
                mm->ebarn = 99999;     /* end bar number */
                xmf = argv[i];
                while(*xmf) {
                    if(*xmf == ':') slsf = 1;
                    if(*xmf == '(') mm->brself = 1;
                    xmf++;
                }
                if(mm->brself) {
                    xmf = argv[i];
                    while(*xmf) {
                        if(*xmf == '(') break;
                        xmf++;
                    }
                    j = 0;
                    xmf++;
                    pas = 0;
                    while(*xmf) {
                        if(*xmf == ')') break;
                        if(*xmf == ',') {   /* start,end -0100003,010007 */
                            pas = 1;
                            break;
                        }
                        wk[j] = *xmf;
                        xmf++;
                        j++;
                        if(j > 7) break;
                    }
                    wk[j] = '\0';
                    /* printf("wk=%s j=%d\n",wk,j); */
                    switch (j) {
                        case  0 : mm->brself = 0;
                                  break;
                        case  1 : sscanf(wk,"%d",&mm->smovn);
                                  mm->brself = 1;
                                  break;
                        case  2 : sscanf(wk,"%d",&mm->smovn);
                                  mm->brself = 1;
                                  break;
                        case  3 : sscanf(wk,"%ld",&mm->sbarn);
                                  mm->brself = 2;
                                  break;
                        case  4 : sscanf(wk,"%ld",&mm->sbarn);
                                  mm->brself = 2;
                                  break;
                        case  5 : sscanf(wk,"%ld",&mm->sbarn);
                                  mm->brself = 2;
                                  break;
                        case  7 : for(j = 0, k = 2;j < 6;j++,k++)
                                      wwk[j] = wk[k];
                                  wk[2] = '\0';
                                  sscanf(wk,"%d",&mm->smovn);
                                  sscanf(wwk,"%ld",&mm->sbarn);
                                  mm->brself = 3;
                                  break;
                        default : ls->errcnt++;
                                  ls->msglvl = 3;
                                  if(ls->msglvl >= ls->prtlvl)
                                  printf("Error! movn barn start select\n");
                                  return(0);
                    }
                    if(pas) {
                        j = 0;
                        xmf++;
                        while(*xmf) {
                            if(*xmf == ')') break;
                            wk[j] = *xmf;
                            xmf++;
                            j++;
                            if(j > 7) break;
                        }
                        wk[j] = '\0';
                        /* printf("wk=%s j=%d\n",wk,j); */
                        switch (j) {
                            case  0 : mm->brself = 0;
                                      break;
                            case  1 : sscanf(wk,"%d",&mm->emovn);
                                      mm->mbendf = 1;
                                      break;
                            case  2 : sscanf(wk,"%d",&mm->emovn);
                                      mm->mbendf = 1;
                                      break;
                            case  3 : sscanf(wk,"%ld",&mm->ebarn);
                                      mm->mbendf = 2;
                                      break;
                            case  4 : sscanf(wk,"%ld",&mm->ebarn);
                                      mm->mbendf = 2;
                                      break;
                            case  5 : sscanf(wk,"%ld",&mm->ebarn);
                                      mm->mbendf = 2;
                                      break;
                            case  7 : for(j = 0, k = 2;j < 6;j++,k++)
                                          wwk[j] = wk[k];
                                      wk[2] = '\0';
                                      sscanf(wk,"%d",&mm->emovn);
                                      sscanf(wwk,"%ld",&mm->ebarn);
                                      mm->mbendf = 3;
                                      break;
                            default : ls->errcnt++;
                                      ls->msglvl = 3;
                                      if(ls->msglvl >= ls->prtlvl)
                                      printf("Error! barn movn end select\n");
                                      return(0);
                        }
                    }
                    ls->msglvl = 2;
                    if(ls->msglvl >= ls->prtlvl) {
                        printf("Link range is %d movement %ld bar to ",
                        mm->smovn,mm->sbarn);
                        printf("%d movement %ld bar\n",mm->emovn,mm->ebarn);
                    }
                }
                yrtn = mkmultif_h(argv[i],mfile,".mec",-1,&xrtn,&yfile,&xfile);
                xmf = mfile;
                k = 0;
                while(*xmf) {
                    if(*xmf == '/') break;
                    if(*xmf == '(') break;
                    if(*xmf == '_') break;
                    if(*xmf == ';') {
                        ls->errcnt++;
                        ls->msglvl = 3;
                        if(ls->msglvl >= ls->prtlvl)
                        printf("Error! filename\n");
                        return(0);
                    }
                    inf[k] = *xmf;
                    xmf++;
                    k++;
                }
                inf[k] = '\0';
                filemake_h(0,inf,".mec",mfile,&yflp,&xflp);
                fpr = fopen_h(mfile,"rb");
                slsf = 3;
                if(yrtn == 99) slsf = 1;
                if(xrtn == 36) slsf = 2;
                xmf = argv[i];
                slslf = 0;
                while(*xmf) {
                    if(*xmf == '/') slslf = 1;
                    if(*xmf == '(') break;
                    if(*xmf == '_') slslf = 1;
                    if(*xmf == ';') slsf = 4;
                    xmf++;
                }
                if(yrtn == 0 && xrtn == 0 && yfile == 0 && xfile == 0) {
                    if(slslf == 0) slsf = 0;
                }
                /*        fn     slsf = 0
                        fn/x     slsf = 1
                       fn/yy     slsf = 2
                      fn/xyy     slsf = 3
                     fn/x;yy     slsf = 4
                */
                if(ls->debug == 5) {
                    printf("Debug5! ");
                    printf("mfile=%s yrtn=%d xrtn=%d yfile=%d xfile=%d\n",
                    mfile,yrtn,xrtn,yfile,xfile);
                    printf("Debug5! yflp=%d xflp=%d slsf=%d\n",yflp,xflp,slsf);
                }
                k = 0;
                j = 0;
                while((c = fgetc(fpr)) != EOF) {
                    j = k % 4;
                    iword[j] = c;
                    k++;
                    if(j == 3) {
                        k = 0;
                        if(shipset_mlk(iword)) return(-1);
                    }
                }
                fclose(fpr);
                if(ls->idchk || ls->idump) {
                    for(k = 0; k < ships; k++) {
                        filemake_h(fs.ship[k],inf,".mec",dfile,&yflp,&xflp);
                        if(ls->debug == 5) 
                            printf("k=%d yflp=%d yfile=%d xflp=%d xfile=%d\n",
                            k,yflp,yfile,xflp,xfile);
                        switch (slsf) {
                            case  1 : if(yflp == yfile)
                                      mlkdchk_mlk(dfile);
                                      break;
                            case  2 : if(xflp == xfile)
                                      mlkdchk_mlk(dfile);
                                      break;
                            case  3 : if(yflp == yfile && xflp == xfile)
                                      mlkdchk_mlk(dfile);
                                      break;
                            case  4 : if(yflp <= yfile && xflp <= xfile)
                                      lkcfsel_mlk(dfile,fs.cap[k]);
                                      break;
                            default : lkcfsel_mlk(dfile,fs.cap[k]);
                                      break;
                        }
                    }
                    link = 0;
                }
            }
            if(ckfn[i]) {
                if(link) {
                    mlkmain(inf,yfile,xfile);
                    if(ls->optm == 2 && ls->mergef == 1) optimize_mlk(inf);
                }
                ls->msglvl = 1;
                if(ls->msglvl >= ls->prtlvl) {
                    if(ls->errcnt || ls->wrncnt) {
                        printf("Mlk end error(%d) worning(%d)\n",
                        ls->errcnt,ls->wrncnt);
                    }
                    /* else printf("Mlk nomal end file=\"%s\"\n",inf); */
                }
            }
        }
    }
    return(0);
}

int shipset_mlk(unsigned char iword[4])
//unsigned char iword[4];
{
    union MCODE v;

    if(ls->comptyp == 0) {
        v.wc[0] = iword[1];
        v.wc[1] = iword[0];
        v.wc[2] = iword[3];
        v.wc[3] = iword[2];
    }
    if(ls->comptyp == 1) {
        v.wc[0] = iword[0];
        v.wc[1] = iword[1];
        v.wc[2] = iword[2];
        v.wc[3] = iword[3];
    }
    if(ls->comptyp == 2) {
        v.wc[0] = iword[0];
        v.wc[1] = iword[1];
        v.wc[2] = iword[3];
        v.wc[3] = iword[2];
    }
    if(iword[0] == 0xf0) {
        fs.cap[ships] = iword[1];
        fs.ship[ships] = v.wi[1];
        /* fs.cap : File type 'C' 's' 
           fs.ship : File number 000 011
        */  
        if(ls->debug) {
            printf("ships=%d fs.cap[ships]=%c ship[ships]=%d ",
                ships,fs.cap[ships],fs.ship[ships]);
        }
    }
    if(iword[0] == 0xf3) {
        fs.chan[ships] = iword[1];
        fs.port[ships] = v.wi[1];
        if(ls->debug) {
            printf("chan[ships]=%d port[ships]=%d\n",
                (int)fs.chan[ships],fs.port[ships]);
        }
        ships++;
        if(ships >= MAXF) {
            ls->errcnt++;
            ls->msglvl = 3;
            if(ls->msglvl >= ls->prtlvl)
                printf("Error! ships(%d) greater than %d\n",ships,MAXF);
            return(-1);
        }
    }
    return(0);
}

int mlkmain(char inf[STLN],int yfile,int xfile)
//char inf[STLN];
//int  yfile;
//int  xfile;
{
    int  i,j;
    int  yflp;
    int  xflp;
    int  pfg;                  /* port serch flag */
    int  pn;
    int  maxpf;                /* max. port file */
    int  psel[MAXP];           /* port select table */
    char dfile[STLN];
    unsigned int  pftb[MAXP];  /* port file table */
    int  portnb;
    int  sships;               /* slsf max ships number */
    int  pwork;                /* pships work */

    /* flag set */
    if(ls->debug) printf("mlkmain() inf=%s yfile=%d xfile=%d slsf=%d\n",
        inf,yfile,xfile,slsf);
    for(j = 0; j < MAXP; j++) {
        pfg = 0;
        for(i = 0; i < ships; i++) {
            pn = fs.port[i];
            if(pfg == 1 && pn == j) fs.chan[i] |= 0x40; /* flag set */
            if(pfg == 0 && pn == j) {
                pfg = 1;
                fs.chan[i] |= 0x20;
            }
        }
    }
    /* make port select table 
       port(gate) number  0 .. 1 .. 2 .. */
    maxps = 0;
    for(i = 0; i < ships; i++) {
        pfg = fs.chan[i] & 0x20;     /* flag check */
        if(pfg) {
            psel[maxps] = fs.port[i];
            if(ls->debug == 5) printf("Debug5! psel[%d] = %d\n",
                maxps,psel[maxps]);
            maxps++;
            if(maxps >= MAXP) {
                ls->errcnt++;
                ls->msglvl = 3;
                if(ls->msglvl >= ls->prtlvl)
                printf("Error! maxps(%d) greater than MAXP(%d)\n",maxps,MAXP);
                return(0);
            }
        }
    }
    /* make port file table */
    for(j = 0; j < maxps; j++) {
        maxpf = 0;
        for(i = 0; i < ships; i++) {
            if(psel[j] == fs.port[i]) {
                pftb[maxpf] = fs.ship[i];
                if(ls->debug == 5) {
                    printf("Debug5! ");
                    printf("psel[%d]=%d fs.port[%d]=%d pftb[%d]=%d\n",
                    j,psel[j],i,fs.port[i],maxpf,pftb[maxpf]);
                }
                maxpf++;
                if(maxpf >= MAXP) {
                    ls->errcnt++;
                    ls->msglvl = 3;
                    if(ls->msglvl >= ls->prtlvl)
                    printf("Error! maxpf(%d) greater than MAXP(%d)\n",
                        maxpf,MAXP);
                    return(0);
                }
            }
        }
        /* make each slsf sships number */
        portnb = psel[j];
        sships = -1;
        for(i = 0; i < maxpf; i++) {
            filemake_h(pftb[i],inf,".mec",dfile,&yflp,&xflp);
            switch (slsf) {
                case  1 : if(yflp == yfile) sships++;
                          break;
                case  2 : if(xflp == xfile) sships++;
                          break;
                case  3 : if(yflp == yfile && xflp == xfile) sships++;
                          break;
                case  4 : if(yflp <= yfile && xflp <= xfile) sships++;
                          break;
                default : sships++;
                          break;
            }
        }
        portnb = psel[j];
        pships = -1;
        for(i = 0; i < maxpf; i++) {
            filemake_h(pftb[i],inf,".mec",dfile,&yflp,&xflp);
            if(ls->debug == 5) {
                printf("Debug5! dfile=%s yflp=%d xflp=%d pships=%d sships=%d\n",
                dfile,yflp,xflp,pships,sships);
            }
            switch (slsf) {
                case  1 : if(yflp == yfile) {
                              pships++;
                              if(pships == sships) {
                                  if(sships == 0) pships = -1;
                                  else pships = -9;  /* last */
                              }
                              ls->eosf = 1; /* add 2015.8.9 */
                              mkselsw_mlk(dfile,portnb,j);
                          }
                          break;
                case  2 : if(xflp == xfile) {
                              pships++;
                              if(pships == sships) {
                                  if(sships == 0) pships = -1;
                                  else pships = -9;  /* last */
                              }
                              mkselsw_mlk(dfile,portnb,j);
                          }
                          break;
                case  3 : if(ls->incld) {
                              if(yflp == yfile && xflp == xfile) {
                                  finclude_mlk(dfile,portnb);
                              }
                              break;
                          }
                          if(ls->excld) {
                              if(yflp == yfile && xflp == xfile) {
                                  fexclude_mlk(dfile,portnb);
                              }
                              break;
                          }
                          if(ls->repla) {
                              if(yflp == yfile && xflp == xfile) {
                                  freplace_mlk(dfile,portnb);
                              }
                              break;
                          }
                          if(yflp == yfile && xflp == xfile) {
                              pships++;
                              if(pships == sships) {
                                  if(sships == 0) pships = -1;
                                  else pships = -9;  /* last */
                              }
                              ls->eosf = 1; /* add 2005.5 */
                              mkselsw_mlk(dfile,portnb,j);
                          }
                          else {   /* port 0 */
                              if(yflp == yfile && xflp == 0) {
                                  pwork = pships;
                                  pships = -9; /* last */
                                  mkselsw_mlk(dfile,portnb,j);
                                  pships = pwork;
                              }
                          }
                          break;
                case  4 : if(yflp <= yfile && xflp <= xfile) {
                              pships++;
                              if(pships == sships) {
                                  if(sships == 0) pships = -1;
                                  else pships = -9;  /* last */
                              }
                              prmkselsw_mlk(dfile,fs.cap[i],portnb,j);
                          }
                          break;
                default : pships++;
                          if(pships == sships) {
                              if(sships == 0) pships = -1;
                              else pships = -9;  /* last */
                          }
                          prmkselsw_mlk(dfile,fs.cap[i],portnb,j);
                          break;
            }
        }
    }
    return(0);
}

int lkcfsel_mlk(char dfile[STLN],char ftyp)
//char dfile[STLN];
//char ftyp;
{
    switch (ftyp) {
        case 'C' : mlkdchk_mlk(dfile);
                   break;
        case 'L' : mlkdchk_mlk(dfile);
                   break;
        case 'M' : mlkdchk_mlk(dfile);
                   break;
        case 'H' : mlkdchk_mlk(dfile);
                   break;
        case 'R' : mlkdchk_mlk(dfile);
                   break;
        case 'D' : mlkdchk_mlk(dfile);
                   break;
        case 'S' : mlkdchk_mlk(dfile);
                   break;
        case 'A' : mlkdchk_mlk(dfile);
                   break;
        case 'G' : mlkdchk_mlk(dfile);
                   break;
        case 'B' : mlkdchk_mlk(dfile);
                   break;
        default  : break;
    }
    return(0);
}

int prmkselsw_mlk(char dfile[STLN],char ftyp,int portnb,int j)
//char dfile[STLN];
//char ftyp;
//int  portnb;
//int  j;    /* port position */
{
    switch (ftyp) {
        case 'C' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'L' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'M' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'H' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'R' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'D' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'S' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'A' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'G' : mkselsw_mlk(dfile,portnb,j);
                   break;
        case 'B' : mkselsw_mlk(dfile,portnb,j);
                   break;
        default  : break;
    }
    return(0);
}

/*     ships           0    1    2    3    4    5     6
       pships          0    1    2    3    4    5    -9       -9
       in              0   a+1  b+2  a+3  b+4  a+5  b+6  ..  a+x
       out             a    b    a    b    a    b  port     port (final)
       sw(memb == 0)   0    1    2    1    2    1     4        3
       sw(memb != 0)  10   11   12   11   12   11    14       13 (+10)

       ships           0
       pships         -1
       in              0
       out          port
       sw(memb == 0)   5
       sw(memb != 0)  15
                  pships  -1 : first file is end of port files
                          -9 : end of port files
                  in       0 : fn_0xx.mec
                           1 : fn_1xx.mec
                             :
                           n : fn_nxx.mec
                         a+1 : fn_wka.mlk + fn_1xx.mec
                             :
                         b+n : fn_wkb.mlk + fn_nxx.mec
                  out      a : fn_wka.mlk
                           b : fn_wkb.mlk
                  sw       0 : fn_000.mec -> fn_wka.mlk
                           1 : fn_wka.mlk + fn_100.mec -> fn_wkb.mlk
                           2 : fn_wkb.mlk + fn_200.mec -> fn_wka.mlk
                           3 : fn_wka.mlk + fn_x00.mlk -> fn_port.mex
                             : port(000)
                           4 : fn_wkb.mlk + fn_x00.mlk -> fn_port.mex
                             : port(000)
                           5 : fn_000.mec -> fn_port.mex : port(000)
                          10 : fn_txx.mec -> fn_wka.mlk
                          11 : fn_wka.mlk + fn_txx.mec -> fn_wkb.mlk
                          12 : fn_wkb.mlk + fn_txx.mec -> fn_wka.mlk
                          13 : fn_wka.mlk + fn_txx.mec -> fn_port.mex
                             : port(ppp)
                          14 : fn_wkb.mlk + fn_txx.mec -> fn_port.mlk
                               fn_port.mlk -> fn_port.mex
                             : port(ppp)
                          15 : fn_0yy.mec -> fn_port.mlk : port(ppp)
                               fn_port.mlk -> fn_port.mex
                                 t : track
                               ppp : port(gate)
*/
int mkselsw_mlk(char dfile[STLN],int portnb,int pcn)
//char dfile[STLN];
//int  portnb;
//int  pcn;      /* port current number */
{
    int  i;
    int  m;
    int  sw;
    char wk[4];
    static char wkf;
    int  tc;  /* track channel */

    i = 0;
    while(dfile[i]) {
        if(dfile[i] == '_') break;
        i++;
    }
    i++;
    wk[0] = dfile[i];
    wk[1] = '\0';
    sscanf(wk,"%d",&lv->mtrak);
    lv->mtrak &= 0x00ff;
    i++;
    wk[0] = dfile[i];
    i++;
    wk[1] = dfile[i];
    wk[2] = '\0';
    sscanf(wk,"%x",&lv->mmemb);
    lv->mmemb &= 0x00ff;

    i = 0;
    while(dfile[i]) {
        if(dfile[i] == '_') break;
        i++;
    }
    i++;
    wk[0] = dfile[++i];
    wk[1] = dfile[++i];
    wk[2] = '\0';
    sscanf(wk,"%x",&tc);
    
    /* if(portnb == 0) {  mod. 98.8 */
    if(tc == 0) {
        if(pships == 0) {
            sw = 0;
            wkf = 'a';
        }
        if(pships > 0) {
            m = pships % 2;
            if(m == 0) {
                sw = 2;
                wkf = 'a';
            }
            else {
                sw = 1;
                wkf = 'b';
            }
        }
        if(pships ==  -9) {
            sw = 5;
            if(wkf == 'a') sw = 3;
            if(wkf == 'b') sw = 4;
        }
        if(pships == -1) sw = 5;
    }
    else {
        if(pships == 0) {
            sw = 10;
            wkf = 'a';
        }
        if(pships > 0) {
            m = pships % 2;
            if(m == 0) {
                sw = 12;
                wkf = 'a';
            }
            else {
                sw = 11;
                wkf = 'b';
            }
        }
        if(pships == -9) {
            sw = 15;
            if(wkf == 'a') sw = 13;
            if(wkf == 'b') sw = 14;
        }
        if(pships == -1) sw = 15;
    }
    if(portnb == 0 && sw == 5 && slsf == 3 && pships == -1) sw = 6;
    if(ls->debug == 5) {
        printf("mkselsw_() ");
        printf("dfile = %s pships=%d portnb=%d lv->mtrak=%d tc=%d sw=%d\n",
        dfile,pships,portnb,lv->mtrak,tc,sw);
    }
    if(ls->mergef) selmfnc_mlk(dfile,portnb,sw,pcn);
    else selfunc_mlk(dfile,portnb,sw);
    return(0);
}

int selmfnc_mlk(char dfile[STLN],int portnb,int sw,int pcn)
//char dfile[STLN];
//int  portnb;
//int  sw;
//int  pcn;
{
    int  eop;  /* end port */

    if(ls->debug == 5) {
        printf("selmfnc_() dfile=%s portnb=%d sw=%d pcn=%d maxps=%d\n",
            dfile,portnb,sw,pcn,maxps);
        /* return(0); */
    }
    eop = 0;
    if(ls->eosf) eop = 1;
    if(pcn >= maxps-1) eop = 1;
    switch (sw) {
        case   0 : fmerge0_mlk(dfile,portnb);
                   break;
        case   1 : fmerge1_mlk(dfile,portnb);
                   break;
        case   2 : fmerge2_mlk(dfile,portnb);
                   break;
        case   3 : fmerge3_mlk(dfile,portnb,eop);
                   break;
        case   4 : fmerge4_mlk(dfile,portnb,eop);
                   break;
        case   5 : fmerge5_mlk(dfile,portnb,eop);
                   break;
        case   6 : fmerge6_mlk(dfile,portnb);
                   break;
        case  10 : fmerge10_mlk(dfile,portnb);
                   break;
        case  11 : fmerge11_mlk(dfile,portnb);
                   break;
        case  12 : fmerge12_mlk(dfile,portnb);
                   break;
        case  13 : fmerge13_mlk(dfile,portnb,eop);
                   break;
        case  14 : fmerge14_mlk(dfile,portnb,eop);
                   break;
        case  15 : fmerge15_mlk(dfile,portnb,eop);
                   break;
        default  : break;
    }
    return(0);
}

int selfunc_mlk(char dfile[STLN],int portnb,int sw)
//char dfile[STLN];
//int  portnb;
//int  sw;
{
    static int lfc = 0;   /* link file count */
    static int psv = 0;   /* port number save */

    if(psv != portnb) lfc = 0;
    lfc++;
    psv = portnb;
    if(ls->debug == 5) {
        printf("selfunc_() dfile = %s sw=%d lfc=%d psv=%d\n",dfile,sw,lfc,psv);
        return(0);
    }
    switch (sw) {
        case   0 : cpconv0_mlk(dfile,portnb);
                   break;
        case   1 : cpconv1_mlk(dfile,portnb);
                   break;
        case   2 : cpconv2_mlk(dfile,portnb);
                   break;
        case   3 : cpconv3_mlk(dfile,portnb);
                   break;
        case   4 : cpconv4_mlk(dfile,portnb);
                   break;
        case   5 : cpconv5_mlk(dfile,portnb);
                   break;
        case   6 : fmerge6_mlk(dfile,portnb);
                   break;
        case  10 : cpconv10_mlk(dfile,portnb);
                   break;
        case  11 : cpconv11_mlk(dfile,portnb);
                   break;
        case  12 : cpconv12_mlk(dfile,portnb);
                   break;
        case  13 : if(lfc == 1) {
                       inmftowb_mlk(dfile,portnb);
                       cpconv14_mlk(dfile,portnb);
                   }
                   else cpconv13_mlk(dfile,portnb);
                   break;
        case  14 : if(lfc == 1) inmftowb_mlk(dfile,portnb);
                   cpconv14_mlk(dfile,portnb);
                   break;
        case  15 : cpconv15_mlk(dfile,portnb);
                   break;
        default  : break;
    }
    return(0);
}

int mlkdchk_mlk(char dfile[STLN])
//char dfile[STLN];
{
    FILE *fpi;
    union MCODE u;
    unsigned char iword[4];
    int  i,j,k;
    int  c;
    long lword;

    if(ls->idump) {
        dump_mlk(dfile);
        return(0);
    }
    fpi = fopen_h(dfile,"rb");
    printf("\nMlkdchk start file=\"%s\"\n",dfile);
    k = 0;
    j = 0;
    lword = 0;
    while((c = fgetc(fpi)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            k = 0;
            ctoval_mlk(iword);   /* set code to value */
            if(!ls->dckn) datachk_mlk();
            else {
                for(i = 0; i < ls->dckn; i++) {
                    if(lc->c[0] == ls->dckcd[i]) datachk_mlk();
                }
            }
            lword++;
        }
    }
    fclose(fpi);
    printf("Mlkdchk end word count=%ld\n",lword);
    return(0);
}

float fresopt_mex(char *dtopt)
//char *dtopt;
{
    int  i,j;
    float dt;
    char wc[18];

    j = 0;
    for(i = 4; i < 20; i++) {
	wc[j] = dtopt[i];
	if(dtopt[i] == '\0') break;
	j++;
    }
    sscanf(wc,"%f",&dt);
    /* printf("wc = %s dt = %f\n",wc,dt); */
    return(dt);
}

int dtoption_mex(char *dtopt)
//char *dtopt;
{
    int  i,j;
    int  dt;
    char wc[18];

    j = 0;
    for(i = 3; i < 20; i++) {
	wc[j] = dtopt[i];
	if(dtopt[i] == '\0') break;
	j++;
    }
    sscanf(wc,"%d",&dt);
    /* printf("wc = %s dt = %d\n",wc,dt); */
    return(dt);
}

int mlkhelp()
{
    printf("\n Music Edit File Linker V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    if(!strcmp(SUPPORT,"Release")) {
     printf(" mlk fn [fn/xyy] [options]\n");
     printf("     fn  : File name\n");
     printf("            x:Sequence yy:Track\n");
     printf("     options :\n");
     printf("         -ot(output trace)\n");
     printf("         -smf0(Format 0) -fix(Fix tempo) -dtxxx(480) .mid\n");
     printf("         -smf1(Format 1) -fix(Fix tempo) -dtxxx(480) .mid\n");
     printf(" usage: e.g.\n");
     printf("      mlk www\n");
     printf("      mlk www -ot\n");
     printf("      mlk www/015 -smf0 -ot\n");
     printf("\n");
     return(0);
    }
    else {
     printf(" mlk fn    [,fn] [options]\n");
     printf("    fn1/x  fn2  [  -ot  ]\n");
     printf("    fn/yy       [ -idp  ]\n");
     printf("    fn/xyy      [ -idc  ]\n");
     printf("    options : -mtc(Put mtc code to .mex file ");
     printf("Resolution quoter flame)\n");
     printf("              -nscore(No Put SMF Score data to .mex)\n");
     printf("              -smf0(Format 0) -fix(Fix tempo) -dtxxx(480) .mid\n");
     printf("              -smf1(Format 1) -fix(Fix tempo) -dtxxx(480) .mid\n");
     printf("              -mtc10(30 flame/sec Resolution 1/10 flame .mid)\n");
     printf("              -old(old .mex)\n");
     printf("              -resxxx.x(Resolution 1/2400.0 Sec Max. 1/7600)\n");
     printf("              -r(replace) -i(include) -e(exclude) -mg(merge)\n");
     printf("              -t(trace) -idp(in dump) -odp(out dump)\n");
     printf("              -ne(no exec file) -idc(in data check) -sp(split)\n");
     printf("              -dc(data check) -ln(link monitor)\n");
     printf("              -opt0 .. -opt3(optimize level 0 to 3 default:1)\n");
     printf("              -d(debug) -d0 .. -d9(-new -opt1)\n");
     printf("              -ot(output trace) -ot8(new .mex) -ot9(smf0)\n");
     printf("              -m0..-m3(message level default:1) -nm(no msg)\n");
     printf(" ex.\n");
     printf(" mlk fn                      (.mex Default optimize 1)\n");
     printf(" mlk fn      -smf0 -ot       (.mid SMF format 0 trace)\n");
     printf(" mlk fn/010  -r              (.mex replace fn_010.meg)\n");
     printf("\n");
     return(0);
    }
}
