
// ----------------------
//  AMuPLC (mecm.c)
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

#include "mec.h"
int _stack = 60000;      /* stack size up to 65536 */
MECARG *mg;
MECVAR *mr;
MECMST *mm;
MECFRS *fr;
MECSYM *ms;   /* symbol table */
MECART *ar;
MECON  *on;
MECOFF *off;
MECCPS *cp;
MECMPV *mp;
MECPCV *pc;
FILE  *fpi;
int  ntrptb[128];        /* note transpose table, use at putcode_mec() */
char scsv[MAXSC][82];    /* card save area */
char nxcard[MAXNX][82];  /* next card save area */
int  scsn;               /* card save number count */
long sync;
long c_sync;  /* current sync */
long max_sync;
char gfile[STLN];
int  exfc;
int  exfovf;  /* Expand symbol table over flag */
long mbnum;
int  Nachf; /* ./)) bar special flag */
char Nachs[82];
int  spsf;  // Smpte B1 Out to first bar 2019.3.31
union meccode sb1,sbf;  // 2019.3.31

int main(int argc,char *argv[])
//int  argc;
//char *argv[];
{
    MECARG marg;   /* modeflag card etc. */
    MECVAR mvar;   /* output codes variable */
    MECMST mmst;   /* bar & line select */
    MECFRS mfrs;   /* fraseing table */
    MECSYM msym;   /* symbol table */
    MECART mart;   /* dynamic expand symbol table */
    MECON  mon;    /* dynamic expand on table */
    MECOFF moff;   /* dynamic expand off table */
    MECCPS mcp;
    MECMPV mmp;
    MECPCV mpc;
    int  i;
    int  j;
    int  k;
    int  l;
    char *pa;
    int  help;
    int  cline;
    char *prtsym;
    char wkc[4];
    char dtopt[STLN];   /* debug or trace option */
    char mfile[STLN];
    char tfile[STLN];   /* Add top file */
    int  argfn[MAXA];
    int  track[MXMB];
    char partf[MXMB][36];
    int  port[MXMB];
    int  channel[MXMB];
    int  member[MXMB];
    int  yrtn;
    int  xrtn;
    int  yfile;
    int  xfile;
    int  yflp;
    int  xflp;
    char *xmf;
    int  slsf;
    int  slslf;
    char wk[8];
    int  pas;
    char wwk[8];

    mg = &marg;
    mr = &mvar;
    mm = &mmst;
    fr = &mfrs;
    ms = &msym;
    ar = &mart;
    on = &mon;
    off = &moff;
    cp = &mcp;
    mp = &mmp;
    pc = &mpc;
    help = 0;
    exfc = 0;         /* Expand symbol count */
    exfovf = 0;
    mbnum = 0;
    mg->mcpos = 0;
    mg->debug = 0;
    mg->trace = 0;
    mg->list = 0;
    mg->source = 0;
    mg->object = 1;
    mg->dump = 0;
    mg->allf = 0;
    mg->impv = 0;
    mg->btyp = 'm';        /* middle bar */
    mg->dped = '.';
    mg->dflg = 'c';
    mg->mtsymb = 0;
    mg->mtempo = 0;
    mg->mkscl = 0;
    mg->mksin = 0;
    mg->mryhm = 4;
    mg->mtime = 4;
    mg->plps = 74;
    mg->mdps = 75;
    mg->prps = 76;
    mg->spos = 77;
    mg->dpos = 78;
    mg->wflag = 0;
    mg->mdymc = 0;
    mg->mtemp = 0;
    mg->tctyp = STCTYP;     /* 30 (drop) flame/sec */
    mg->msglvl = 1;
    mg->msgflg = 0;
    mg->prtlvl = 1;        /* message level 3 (upper Worning3) */
    mg->expf = 1;          /* egnore expand symbols */
    mg->smf = 0;           /* -SMF optin on/off flag */
    mr->port = 0;
    mr->chan = 0;
    mr->movn = 0;
    mr->memb = 0;
    mr->temp = 0;
    mr->tsym = 0;
    mr->egrp = 0;
    mr->expr = 0;
    mr->kscl = 0;
    mr->ksin = 0;
    mr->ryhm = 0;
    mr->time = 1;
    mr->splt = 1;
    mr->rept = 1;
    mr->renp = 1;
    mr->cval = 0;
    mr->wval = 0;
    mr->lag = 0;
    mr->lagb = 0;
    mr->offset = 0;
    mr->offsetb = 0;
    cp->jky = 0;
    cp->iky = 0;
    if(argc == 1) {
        help_main();
        return(0);
    }
    for(i = 0; i < argc; i++) {
        k = i;
        pa = argv[i];
        j = 0;
        argfn[k] = 1;
        while(*pa) {
        dtopt[j] = toupper(*pa);
            j++;
            if(j > 10) break;
            pa++;
        }
        dtopt[j] = '\0';
        if(!strcmp(dtopt,"-H")) help = 2;
        if(!strcmp(SUPPORT,"Debug")) {
        if(!strcmp(dtopt,"-SMF")) mg->smf = 1;
        if(!strcmp(dtopt,"-H")) help = 1;
        if(!strcmp(dtopt,"-S")) mg->source = 1;
        if(!strcmp(dtopt,"-SL")) mg->source = 2;
        if(!strcmp(dtopt,"-D")) mg->debug = 1;
        if(!strcmp(dtopt,"-D2")) mg->debug = 2;
        if(!strcmp(dtopt,"-D3")) mg->debug = 3;
        if(!strcmp(dtopt,"-D4")) mg->debug = 4;
        if(!strcmp(dtopt,"-D5")) mg->debug = 5;
        if(!strcmp(dtopt,"-D6")) mg->debug = 6;  /* Dynamic Expand check */
        if(!strcmp(dtopt,"-D7")) mg->debug = 7;  /* fn(xxx,xxxx) check */
        if(!strcmp(dtopt,"-D8")) mg->debug = 8;  /* multi channel p.v check */
        if(!strcmp(dtopt,"-D9")) mg->debug = 9;  /* program change out */
        if(mg->debug) mg->object = 0;
        if(!strcmp(dtopt,"-T")) mg->trace = 1;
        if(!strcmp(dtopt,"-L")) mg->list = 1;
        if(!strcmp(dtopt,"-NO")) mg->object = 1;
        if(!strcmp(dtopt,"-OBJ")) mg->object = 1;
        if(!strcmp(dtopt,"-O")) mg->object = 1;
        if(!strcmp(dtopt,"-ALL")) mg->allf = 1;
        if(!strcmp(dtopt,"-A")) mg->allf = 1;
        if(!strcmp(dtopt,"-DP")) mg->dump = 1;
        if(!strcmp(dtopt,"-IMPV")) mg->impv = 1;
        if(!strcmp(dtopt,"-IT")) mg->impv = 1;
        if(!strcmp(dtopt,"-M0")) mg->prtlvl = 0;  /* all */
        if(!strcmp(dtopt,"-M1")) mg->prtlvl = 1;  /* upper worning level 1 */
        if(!strcmp(dtopt,"-M2")) mg->prtlvl = 2;  /* upper worning level 2 */
        if(!strcmp(dtopt,"-M3")) mg->prtlvl = 3;  /* upper worning level 3 */
        if(!strcmp(dtopt,"-M4")) mg->prtlvl = 4;  /* upper error level 1 */
        if(!strcmp(dtopt,"-M5")) mg->prtlvl = 5;  /* upper error level 2 */
        if(!strcmp(dtopt,"-M6")) mg->prtlvl = 6;  /* upper error level 3 */
        if(!strcmp(dtopt,"-M7")) mg->prtlvl = 7;  /* upper error level 4 */
        if(!strcmp(dtopt,"-M8")) mg->prtlvl = 8;  /* upper msg level 8 */
        if(!strcmp(dtopt,"-M9")) mg->prtlvl = 9;  /* upper msg level 9 */
        if(!strcmp(dtopt,"-NM")) mg->prtlvl = 99; /* no message */
        if(!strcmp(dtopt,"-NX")) mg->expf = 0;  /* no expand  */
        }
        if(dtopt[0] == '-') argfn[k] = 0;
    }
    if(help) {
        fhelp_mec(help);
        return(0);
    }
    mg->msglvl = 0;
    prtsym = "0123456789abcdefghijklmnopqrstuvwxyz";
    mm->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"PC9801")) mm->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"IBMPC"))  mm->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"SUN"))    mm->comptyp = 1;
    for(i = 1; i < argc; i++) {
        if(argfn[i]) {
            mm->brself = 0;        /* bar number select flag */
            mm->mbendf = 0;        /* bar number end select flag */
            mm->smovn = 1;         /* start movment number */
            mm->sbarn = 1;         /* start bar number */
            mm->emovn = 99;        /* end movment number */
            mm->ebarn = 99999;     /* end bar number */
            mg->hf = 0;            /* Header .meg file read flag off */
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
                    case  7 : for(j = 0, k = 2;j < 6;j++,k++) wwk[j] = wk[k];
                              wk[2] = '\0';
                              sscanf(wk,"%d",&mm->smovn);
                              sscanf(wwk,"%ld",&mm->sbarn);
                              mm->brself = 3;
                              break;
                    default : mg->errcnt++;
                              mg->msglvl = 3;
                              if(mg->msglvl >= mg->prtlvl)
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
                        default : mg->errcnt++;
                                  mg->msglvl = 3;
                                  if(mg->msglvl >= mg->prtlvl)
                                  printf("Error! barn movn end select\n");
                                  return(0);
                    }
                }
                mg->msglvl = 2;
                if(mg->msglvl >= mg->prtlvl) {
                    printf("Compilation range is %d movement %ld bar to ",
                    mm->smovn,mm->sbarn);
                    printf("%d movement %ld bar\n",mm->emovn,mm->ebarn);
                }
            }
            yrtn = mkmultif_h(argv[i],mfile,".meg",-1,&xrtn,&yfile,&xfile);
            xmf = mfile;
            k = 0;
            while(*xmf) {
                if(*xmf == '(') break;
                if(*xmf == '/') break;
                if(*xmf == '_') break;
                if(*xmf == ';' || *xmf == ',') {
                    mg->errcnt++;
                    mg->msglvl = 3;
                    if(mg->msglvl >= mg->prtlvl)
                    printf("Error! filename\n");
                    return(0);
                }
                tfile[k] = *xmf;
                xmf++;
                k++;
            }
            tfile[k] = '\0';
            filemake_h(0,tfile,".meg",mfile,&yflp,&xflp);
            slsf = 3;
            if(yrtn == 99) slsf = 1;
            if(xrtn == 36) slsf = 2;
            xmf = argv[i];
            slslf = 0;
            while(*xmf) {
                if(*xmf == '(') break;
                if(*xmf == '/') slslf = 1;
                if(*xmf == '_') slslf = 1;
                if(*xmf == ';' || *xmf == ',') slsf = 4;
                if(*xmf == ',') *xmf = ';';
                xmf++;
            }
            if(yrtn == 0 && xrtn == 0 && yfile == 0 && xfile == 0) {
                if(slslf == 0) slsf = 0;
            }
            if(mg->debug == 2) {
                printf("mfile = %s yrtn=%d xrtn=%d yfile=%d xfile=%d\n",
                mfile,yrtn,xrtn,yfile,xfile);
                printf("yflp=%d xflp=%d slsf=%d\n",yflp,xflp,slsf);
            }
            cline = selfil_mec(mfile,track,partf,port,channel,member);
            l = 0;
            while(mfile[l++] != '_');
            for(j = 0; j < cline; j++) {
                sprintf(wkc,"%02x",track[j]);  /* 97.10 */
                mfile[l+1] = wkc[0];
                mfile[l+2] = wkc[1];
                for(k = 0; k < 36; k++) {
                    mfile[l] = *(prtsym+k);
                }
            }
            for(k = 0; k < 36; k++) {
                yflp = k;
                mfile[l] = *(prtsym+k);
                mr->trak = k;
                for(j = 0; j < cline; j++) {
                    mr->port = port[j];
                    mr->chan = channel[j];
                    /* mr->chan = channel[j] - 1;   modify to 0-15 */
                    mr->memb = member[j];
                    sprintf(wkc,"%02x",track[j]);  /* 97.10 */
                    xflp = track[j];
                    mfile[l+1] = wkc[0];
                    mfile[l+2] = wkc[1];
                    if(mg->debug == 2) {
                        printf(
                        "cline=%d slsf=%d yflp=%d yfile=%d xflp=%d xfile=%d\n"
                        ,j,slsf,yflp,yfile,xflp,xfile);
                    }
                    switch (slsf) {
                        case  1 : if(yflp == yfile)
                                  mecfchk2_mec(mfile,partf[j][k]);
                                  break;
                        case  2 : if(xflp == xfile)
                                  mecfchk2_mec(mfile,partf[j][k]);
                                  break;
                        case  3 : if(yflp == yfile && xflp == xfile)
                                  mecfchk1_mec(mfile,partf[j][k]);
                                  break;
                        case  4 : if(yflp <= yfile && xflp <= xfile)
                                  mecfchk2_mec(mfile,partf[j][k]);
                                  break;
                        default : mecfchk2_mec(mfile,partf[j][k]);
                                  break;
                    }
                }
            }
        }
    }
    return(0);
}

int mecfchk1_mec(char mfile[STLN],char partc)
//char mfile[STLN];
//char partc;
{
    if(partc == '@') mecmain(mfile);
    if(partc == '*') mecmain(mfile);
    if(partc == '+') mecmain(mfile);
    if(partc == 'c') mecmain(mfile);
    if(partc == 'l') mecmain(mfile);
    if(partc == 'm') mecmain(mfile);
    if(partc == 'h') mecmain(mfile);
    if(partc == 'r') mecmain(mfile);
    if(partc == 'd') mecmain(mfile);
    if(partc == 't') mecmain(mfile);
    if(partc == 'g') mecmain(mfile);
    if(partc == 's') mecmain(mfile);
    if(partc == 'a') mecmain(mfile);
    if(partc == 'b') mecmain(mfile);
    if(partc == 'u') mecmain(mfile);
    if(partc == 'x') mecmain(mfile);
    if(partc == 'C') mecmain(mfile);
    if(partc == 'L') mecmain(mfile);
    if(partc == 'M') mecmain(mfile);
    if(partc == 'H') mecmain(mfile);
    if(partc == 'R') mecmain(mfile);
    if(partc == 'D') mecmain(mfile);
    if(partc == 'T') mecmain(mfile);
    if(partc == 'G') mecmain(mfile);
    if(partc == 'S') mecmain(mfile);
    if(partc == 'A') mecmain(mfile);
    if(partc == 'B') mecmain(mfile);
    if(partc == 'U') mecmain(mfile);
    if(partc == 'X') mecmain(mfile);
    return(0);
}

int mecfchk2_mec(char mfile[STLN],char partc)
//char mfile[STLN];
//char partc;
{
    int  all;

    all = mg->allf;
    if(partc == '@' && all) mecmain(mfile);
    if(partc == '*' && all) mecmain(mfile);
    if(partc == '+' && all) mecmain(mfile);
    if(partc == 'c' && all) mecmain(mfile);
    if(partc == 'l' && all) mecmain(mfile);
    if(partc == 'm' && all) mecmain(mfile);
    if(partc == 'h' && all) mecmain(mfile);
    if(partc == 'r' && all) mecmain(mfile);
    if(partc == 'd' && all) mecmain(mfile);
    if(partc == 't' && all) mecmain(mfile);
    if(partc == 'g' && all) mecmain(mfile);
    if(partc == 's' && all) mecmain(mfile);
    if(partc == 'a' && all) mecmain(mfile);
    if(partc == 'b' && all) mecmain(mfile);
    if(partc == 'u' && all) mecmain(mfile);
    if(partc == 'x' && all) mecmain(mfile);
    if(partc == 'C' && all) mecmain(mfile);
    if(partc == 'L' && all) mecmain(mfile);
    if(partc == 'M' && all) mecmain(mfile);
    if(partc == 'H' && all) mecmain(mfile);
    if(partc == 'R' && all) mecmain(mfile);
    if(partc == 'D' && all) mecmain(mfile);
    if(partc == 'T' && all) mecmain(mfile);
    if(partc == 'G' && all) mecmain(mfile);
    if(partc == 'S' && all) mecmain(mfile);
    if(partc == 'A' && all) mecmain(mfile);
    if(partc == 'B' && all) mecmain(mfile);
    if(partc == 'U' && all) mecmain(mfile);
    if(partc == 'X' && all) mecmain(mfile);
    /* To main() */
    if(partc == 'C') if(!all) mecmain(mfile);
    if(partc == 'L') if(!all) mecmain(mfile);
    if(partc == 'M') if(!all) mecmain(mfile);
    if(partc == 'H') if(!all) mecmain(mfile);
    if(partc == 'R') if(!all) mecmain(mfile);
    if(partc == 'D') if(!all) mecmain(mfile);
    if(partc == 'T') if(!all) mecmain(mfile);
    if(partc == 'G') if(!all) mecmain(mfile);
    if(partc == 'S') if(!all) mecmain(mfile);
    if(partc == 'A') if(!all) mecmain(mfile);
    if(partc == 'B') if(!all) mecmain(mfile);
    if(partc == 'U') if(!all) mecmain(mfile);
    if(partc == 'X') if(!all) mecmain(mfile);
    return(0);
}

int mecmain(char *inf)
//char *inf;
{
    char outf[STLN];
    char mwkf[STLN];
    char *pf;
    int  i,j;
    int  stflg;
    int  eoflg;
    struct mecksn mk;         /* key signeture table */
    union meccode ff;
    int  nx;
    int  nxp;
    int  top;

    stflg = 0;
    eoflg = 0;
    mr->tcnt = 0;
    mr->pprc = 0;
    mr->prc = 0;
    mr->inst = 0;
    mr->trns = 0;
    mr->trnsv = 0;      /* note line trans */
    mg->bitrns = 0;     /* beau instruments trans */
    mg->line = 0;
    mg->errcnt = 0;
    mg->wrncnt = 0;
    mr->bnum = 0;
    mm->bnum = 0;
    mr->sync = 0;
    mr->psyn = 0;
    mr->acci = 0;
    mr->keyp = 0;
    mr->dyna = 0;
    mr->phra = 0;
    mr->voff = 0;
    mr->veon = 0;
    mr->kprs = 0;
    mr->cntn = 0;
    mr->cntv = 0;
    mr->prgn = 0;
    mr->prsv = 0;
    mr->pitv = 0;           /* pitch v. */
    mr->mitv = 0;           /* modulation */
    mr->citv = 0;           /* control */
    mr->pgiv = 0;           /* program change */
    mr->ffflg = 0;
    mr->trnsp = 0;          /* define transpose */
    mr->lag = 0;
    mr->lagb = 0;
    mr->offset = 0;
    mr->offsetb = 0;
    mg->rbaf = 0;
    mg->smpmid = 's';       /* default smpte */
    ar->nmb = 0;           /* clear articulation symbol count */
    ar->ctyp = 0;          /* current typ */
    mp->mpvf = 0;
    cp->key = 1;           /* Major */
    mp->rp = 0;            /* Root position */
    pc->bf = 0;            /* program change begin flag */
    mg->mfflg = chkmfile_mec(inf);   /* check main file _x00. */
    /* printf("inf=%s mfflg=%d\n",inf,mg->mfflg); */
    if(mg->list) {
        printf("\nList file \"%s\"\n",inf);
        fpi = fopen_h(inf,"rb");
        while(fgets(mg->card, 82, fpi)) {
            mg->line++;
            printf("%s", mg->card);
            if(mg->card[2] == '$') break;
        }
        printf("File \"%s\" total line = %d\n",inf, mg->line);
        fclose(fpi);
        return(0);
    }
    if(mg->dump) {
        dump_mec(inf);
        return(0);
    }
    for(i = 0; i <128; i++) ntrptb[i] = 0;  /* clear note transpose table */
    if(mg->debug) {
        printf("\nMec start in_file=\"%s\" port=%d channel=%d track=%d ",
        inf,mr->port,mr->chan,mr->trak);
        printf("member=%d\n",mr->memb);
    }
    outf[0] = '\0';
    if(mm->brself) {
        mr->pitv = 0;        /* pitch v. */
        mr->mitv = 0;        /* modulation */
        mr->citv = 0;        /* control */
        mr->pgiv = 0;        /* program change */
        mm->byrd = 0;
        mm->movn = 0;
        mm->bnum = 0;
        i = 0;
        while(inf[i]) {
            outf[i] = inf[i];
            if(outf[i] == '.') break;
            i++;
            if(i >= STLN) break;
        }
        outf[++i] = '\0';
        strcat(outf,"mec");
        i = 0;
        while(inf[i]) {
            mwkf[i] = inf[i];
            if(mwkf[i] == '.') break;
            i++;
            if(i >= STLN) break;
        }
        mwkf[++i] = '\0';
        strcat(mwkf,"mwk");
        remove(mwkf);
        rename(outf,mwkf);
        if(mg->object) {
            mg->fpo = fopen_h(outf,"wb");
        }
        mecfread_mec(mwkf,1);
    }
    else {
        mm->byrd = 1;
        if(mg->object) {
            i = 0;
            pf = inf;
            while(*pf != '.') {
                outf[i] = *pf;
                i++;
                pf++;
            }
            outf[i] = '\0';
            strcat_h(outf,".mec");
            mg->fpo = fopen_h(outf,"wb");
        }
    }
    nkeyini_mec();          /* natural key init */
    mksym_mec(&mk);         /* make key symbol table */
    mkexpt_mec();           /* make expression table */
    fpi = fopen_h(inf,"rb");
    ffcode_mec(&ff);
    for(i = 0; i < 128; i++) mg->ntb[i] = 0; /* ntb clear */
    for(i = 0; i < 128; i++) mg->enoftb[i] = 0;
    /* frase table clear */
    for(j = 0; j < 5; j++) {
        fr->fwsyn[j] = 0;
        fr->fract[j] = 0;
        fr->frp[j] = 0;
        fr->fsmax[j] = 0;
        fr->csyn[j] = 0;
        fr->renp = 1;
        fr->rept = 1;
        for(i = 0; i < MFRS; i++) {
            fr->faf[j][i] = 0;
            fr->fd[j][i] = 0;
            fr->ed[j][i] = 0;
            fr->frnv[j][i] = 0;
            fr->fsyn[j][i] = 0;
        }
    }
    initdyna_mec();
    if(mg->hf) { /* Header C code out */
        header_mec();
        mg->hf = 0;
    }
    mg->line = 0;
    Nachf = 0;
    top = 1;
    fgets(mg->card,82,fpi);
    fgets(mg->next,82,fpi);
    fgets(mg->next2,82,fpi);
    /* while(fgets(mg->next, 82, fpi)) { */
    while(1) {
        nx = nextcard_mec(fpi);
        if(mg->debug == 2) printf("Debug2! nx = %d\n",nx);
        if(nx < 0) {
            if(nx == -1) {
                nx = 1;
                eoflg = 1;
            }
            if(nx == -2) {
                nx = 1;
                eoflg = 2;
            }
        }
        nxp = 0;
        while(nx > 0) {
            if(!top) strcpy(mg->card,nxcard[nxp]);
            /* printf("%s",mg->card);  */
            top = 0; /* First line off  */
            mg->prflg = 1;  /* print flag */
            nxp++;
            nx--;
            if(mg->next[0] == 'f') eoflg = 1;
            //if(Nachf == 9) Nachsbar_mec(mk);
            if(Nachf == 9) Nachsbar_mec(&mk); //2025.4.5
            if(mg->mfflg == 1) { 
              switch (mg->card[0]) {
                case '-'  : /* 000 */ 
                            if(!stflg) {
                                faf7code_mec();
                                mkeyset_mec(); /* 1999.12.25 */
                                stflg = 1;
                            }
                            syncout_mec(0,2);  /* add 1996.10 */
                            bar_mec();
                            syncout_mec(max_sync,1); /* add 1996.10 */
                            outpcgn_mec(); /* 2007.12 */
                            if(mm->byrd != 1) break;
                            if(mg->card[12] == 'p') mg->smpmid = 's';
                            if(mg->card[12] == '0') mg->smpmid = 'm';
                            if(spsf) {  // 2019.3.30
                                if(mg->trace) printf("Trace! B1 Smpte out\n");
                                putcode_mec(&sb1);
                                putcode_mec(&sbf);
                                spsf = 0;
                            }
                            break;
                case '%'  : if(mm->byrd != 1) break;
                            mcaper_mec(&mk,fpi);
                            break;
                case 's'  : if(mm->byrd != 1) break;
                            mcaper_mec(&mk,fpi);
                            break;
                case 'c'  : if(mm->byrd != 1) break;
                            mcaper_mec(&mk,fpi);
                            break;
                case 'R'  : if(mm->byrd == 1) rest_mec(&mk);
                            break;
                case 'r'  : if(mm->byrd == 1) rest_mec(&mk);
                            break;
                case '['  : if(mm->byrd == 1) frasing_mec();
                            break;
                case '{'  : /* Gloval */
                            if(mg->card[1] == '?') break; /* Skip Chord line */
                            if(mg->card[1] == '$' && 
                                mg->card[2] == 'M') mpvgate_mec(inf);
                            if(mg->card[1] == '$' && mg->card[2] == ':') break;
                            if(mg->card[1] == '$' && mg->card[3] == ':') break;
                            if(mg->card[1] == '$') {
                                keyryhm_mec(&mk);
                                stflg = 1;
                                break;
                            }
                            else expression_mec(&ff);
                            break;
                case '('  : /* local */
                            if(mg->card[1] == '?') break; /* Skip Chord line */
                            if(mg->card[1] == '$' && 
                                mg->card[2] == 'M') mpvgate_mec(inf);
                            if(mg->card[1] == '$' && mg->card[2] == ':') break;
                            if(mg->card[1] == '$' && mg->card[3] == ':') break;
                            if(mm->byrd != 1) break;
                            sbrtolbr_mec();
                            if(mg->card[1] == '$') {
                                keyryhm_mec(&mk);
                                stflg = 1;
                                break;
                            }
                            else expression_mec(&ff);
                            lbrtosbr_mec();
                            break;
                case '<'  : if(mm->byrd == 1) midic_mec();
                            break;
                case '#'  : if(mm->byrd == 1) strcheck_mec();
                            break;
                case '$'  : /* */
                            if(mg->card[1] == 'M') mpvgate_mec(inf);
                            if(mg->card[1] == ':') break;
                            if(mg->card[2] == ':') break;
                            keyryhm_mec(&mk);
                            stflg = 1;
                            break;
                case '+'  : if(mm->byrd != 1) break;
                            befnote_mec(&mk);
                            /*
                            polfkey_mec();
                            anoteon_mec();
                            */
                            break;
                case ' '  : break;                      /* comment */
                case '*'  : if(mg->card[1] == '+' || mg->card[1] == '-')
                                addexp_mec();
                            if(mg->smf) smfmeta_mec();
                            break;
                case '='  : /* if(mm->byrd != 1) break; */
                            syncout_mec(0,2);  /* add 1996.10 */
                            if(mg->card[1] == '=') movbar_mec();
                            else bar_mec();
                            f1code_mec();
                            noteclr_mec();
                            break;
                case 'f'  : eoflg = 1;
                            break;
                case 'S'  : if(mm->byrd == 1 && mg->card[1] == 'm')
                                smptcode_mec();
                            break;
                case 'C'  : if(mm->byrd == 1) mfilec_mec();
                            break;
                default   : break;
              }
            }
            else {
              switch (mg->card[0]) {
                case '-'  : /* bar line */
                            if(mg->next[0] == '+' && mg->next[4] == '.')
                                Nachsch_mec();
                            if(mg->next[4] == '.' && mg->next[6] == ')')
                                Nachsch_mec();
                            if(Nachf == 1) break;
                            if(!stflg) {
                                faf7code_mec();
                                /* ffwrite_mec(&ff); */
                                /* mkeyset_mec(); 1999.12.25 */
                                stflg = 1;
                            }
                            rsphdyna_mec(); /* Reset phrase dyna */
                            bar_mec();
                            if(mg->debug == 7) printf("mm->byrd=%d\n",mm->byrd);
                            if(mm->byrd != 1) break;
                            /*
                            if(mr->bnum == 0) {
                                mr->pitv = 777;        out pitch v.
                                mr->mitv = 777;        out modulation
                                mr->citv = 777;        out control
                                mr->pgiv = 777;        out program change
                            }
                            */
                            for(j = 0; j < 78; j++)
                                mg->ktbbak[j] = mg->ktb[j]; 
                            keyini_mec();
                            keyset_mec();
                            if(mg->debug == 3) keydmp_mec(&mk);
                            c_sync = mr->sync; /* lag offset sync check */
                            break;
                case '%'  : if(mm->byrd == 1) {
                                if(mg->card[75] == '+' || mg->card[75] == '-')
                                dexpand_mec(&mk);
                                else ptcper_mec(inf,&mk);
                            }
                            if(mg->card[4] == '.' && mg->card[6] == ')')
                                Nachf = 2;
                            if(mg->card[4] == '[' && Nachf) Nachf = 2;
                            break;
                case 'R'  : if(mm->byrd == 1) ptcrest_mec(&mk);
                            break;
                case 'r'  : if(mm->byrd == 1) ptcrest_mec(&mk);
                            break;
                case '/'  : if(mm->byrd == 1) {
                                if(mg->card[1] == '%' &&
                                    mg->card[2] == '/') putrepc_mec();
                            }
                            break;
                case '['  : if(mm->byrd == 1) frasing_mec();
                            break;
                case '{'  : /* Gloval */
                            if(mg->card[1] == '?') break; /* Skip Chord line */
                            if(mg->card[1] == '$' && 
                                mg->card[2] == 'M') mpvgate_mec(inf);
                            if(mg->card[1] == '$' && mg->card[2] == 'R') {
                                dynacp_mec();
                                break;
                            }
                            if(mg->card[1] == '$' && mg->card[2] == ':') break;
                            if(mg->card[1] == '$' && mg->card[3] == ':') break;
                            if(mg->card[1] == '$') {
                                keyryhm_mec(&mk);
                                if(mg->card[3] != ':') { /* +abc Dynamic ex. */
                                    opencp_mec(inf);
                                    getexp_mec();
                                }
                                stflg = 1;
                                break;
                            }
                            else expression_mec(&ff);
                            break;
                case '('  : if(mm->byrd != 1) break;
                            if(mg->card[1] == '$' && 
                                mg->card[2] == 'M') mpvgate_mec(inf);
                            if(mg->card[1] == '$' && mg->card[2] == 'R') {
                                dynacp_mec();
                                break;
                            }
                            if(mg->card[1] == '$' && mg->card[2] == ':') break;
                            if(mg->card[1] == '$' && mg->card[3] == ':') break;
                            sbrtolbr_mec();
                            if(mg->card[1] == '$') {
                                keyryhm_mec(&mk);
                                if(mg->card[3] != ':') {
                                    opencp_mec(inf);
                                    getexp_mec();
                                }
                                stflg = 1;
                                break;
                            }
                            else expression_mec(&ff);
                            lbrtosbr_mec();
                            break;
                case '<'  : if(mm->byrd == 1) midic_mec();
                            break;
                case '#'  : if(mm->byrd == 1) strcheck_mec();
                            break;
                case '$'  : /* ffwrite_mec(&ff); */
                            if(mg->card[1] == 'M') mpvgate_mec(inf);
                            if(mg->card[1] == ':') break;
                            if(mg->card[2] == ':') break;
                            keyryhm_mec(&mk);
                            stflg = 1;
                            break;
                case '+'  : if(mm->byrd != 1) break;
                            befnote_mec(&mk);
                            if( mg->btyp == 's' ||
                                mg->btyp == 't' ||
                                mg->btyp == 'g' ||
                                mg->btyp == 'a' ||
                                mg->btyp == 'b' ||
                                mg->btyp == 'u' ||
                                mg->btyp == 'd') {
                                syncepv_mec();   /* pitch vend */
                                syncecnt_mec();  /* modul. control cg. */
                                syncepcg_mec();  /* program change */
                                syncetrs_mec();     /* transpose */
                            }
                            pedalck_mec();
                            /*
                            polfkey_mec();
                            anoteon_mec();
                            */
                            break;
                case '*'  : if(mg->card[1] == '+' || mg->card[1] == '-')
                                addexp_mec();
                            if(mg->smf) smfmeta_mec();
                            break;
                case '='  : /* if(mm->byrd != 1) break; */
                            if(mg->card[1] == '=') movbar_mec();
                            else bar_mec();
                            noteclr_mec();
                            c_sync = mr->sync; /* lag offset sync check */
                            syncout_mec(0,2);  /* add 1998.8 */
                            break;
                default   : break;
              }
              if(eoflg) break;
            }
            mg->line++;
            /* printf("Nachf = %d\n",Nachf); */
            if(Nachf) {
                if(mg->next[4] != '.' && mg->next[4] != '[') Nachf = 9;
            }
            if(mg->prflg) pr_card(mg->card);
            strcpy(mg->befr,mg->card);
        }
        if(eoflg) break;
    }
    if(mg->befr[0] != '=') { /* Duble === line ? */
        mg->line++;
        pr_card(mg->next);
    }
    if(eoflg == 1) final_mec(); 
    mg->line++;
    pr_card(mg->next2);
    if(eoflg == 1) {
        fgets(mg->card,82,fpi);
        mg->line++;
        pr_card(mg->card);
    }
    if(mm->byrd == 2) mecfread_mec(mwkf,9);
    if(mg->mfflg == 1) mbnum = mr->bnum;
    else {
        if(mbnum != 0) {
            if(mr->bnum != mbnum) {
                mg->errcnt++;
                mg->msglvl = 5;
                if(mg->msglvl >= mg->prtlvl)
                    printf("Error! bar number %d\n",mr->bnum);
            }
        }
    }
    mg->msglvl = 1;
    if(mg->msglvl >= mg->prtlvl) {
        if(mg->errcnt || mg->wrncnt) {
            printf("Mec %s %d bars, %d ilines end\n",
                inf,mr->bnum-1,mg->line+1);
            printf("error(%d) worning(%d)\n",mg->errcnt,mg->wrncnt);
        }
        else {
            printf("Mec %s %d bars, %d lines end\n",
                inf,mr->bnum-1,mg->line+1);
        }
    }
    if(mg->object) fclose(mg->fpo);
    fclose(fpi);       /* close .meg file */
    return(0);
}

int header_mec()
{
    FILE *fhg;

    /* printf("gfile = %s\n",gfile); */
    mg->line = 0;
    pc->n = 0;
    fhg = fopen_h(gfile,"rb");
    while(fgets(mg->card, 82, fhg)) {
	pr_card(mg->card);
        mg->line++;
        switch(mg->card[0]) {
            case 'C'  : mfilec_mec();
                        break;
            case '@'  : mpchg_mec();
                        break;
            default   : break;
        }
    }
    mg->msglvl = 0;
    if(mg->msglvl >= mg->prtlvl) {
        printf("Mec %s %d %d end\n",gfile,mr->bnum-1,mg->line+1);
    }
    return(0);
}

int mcaper_mec(struct mecksn *mk,FILE *fpi)
//struct mecksn *mk;         /* key signeture table */
//FILE *fpi;
{
    int  i;

    mr->renp = 1;
    renget_mec();
    if(mr->renp >= MSTC) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
            printf("Error! %06d renpu max is %d\n",mg->line,MSTC);
        return(0);
    }
    if(mr->renp > 1) {
        renproc_mec(mk,fpi);
        return(0);
    }
    for(i = 0; i < mr->rept; i++) mnote_mec(mk);
    return(0);
}

int keyryhm_mec(struct mecksn *mk)
//struct mecksn *mk;         /* key signeture table */
{
    int  j;
    int  rf;  /* ryhm flag */
    union meccode fc;

    if(mg->card[0] == '$' && mg->card[2] == ':') return(0);
    if(mg->card[1] == '$' && mg->card[3] == ':') return(0);
    /* faf7code_mec(); */
    rf = key_mec(mk);
    for(j = 0; j < 78; j++) mg->ktbbak[j] = mg->ktb[j];
    if(rf == 0) return(0); /* {$##No Ryhtm} */
    if(mr->time != 0) mr->sync = (400000/mr->time)*mr->ryhm;
    else {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
            printf("Error! mr->time value 0 (mecmain)\n");
    }
    if(max_sync != mr->sync) {
        lsconv_h(&fc,mr->sync);
        fc.zi[0] &= 0x00ff;
        fc.zi[0] |= 0xfc00;
        if(mg->trace) printf("Trace! FC(%04x%04x) mr->sync=%ld\n",
        fc.zi[0],fc.zi[1],mr->sync);
        if(mg->card[0] != '(') putcode_mec(&fc);
    }
    max_sync = mr->sync;
    if(mp->mpvf) mpvset_mec();
    return(0);
}

/* part case % */
int ptcper_mec(char *inf,struct mecksn *mk)
//char *inf;
//struct mecksn *mk;         /* key signeture table */
{
    int  i;
    int  rtn;

    if(mg->card[76] != '.' && mg->card[77] != '.' &&
       mg->card[78] != '.') {
       rtn = expand_mec(inf,mk);
       if(rtn == 0) return(0); /* Nomal return */
    }
    mr->renp = 1;
    outpcgn_mec(); /* 2007.12 */
    if( mg->btyp == 's' ||
        mg->btyp == 't' ||
        mg->btyp == 'g' ||
        mg->btyp == 'a' ||
        mg->btyp == 'b' ||
        mg->btyp == 'u' ||
        mg->btyp == 'd') {
        syncepv_mec();   /* pitch vend */
        syncecnt_mec();  /* modul. control cg. */
        syncepcg_mec();  /* program change */
        syncetrs_mec();     /* transpose */
    }
    pedalck_mec();
    renget_mec();
    if(mr->renp >= MSTC) {
        mg->errcnt++;
        mg->msglvl = 3;
        if(mg->msglvl >= mg->prtlvl)
        printf("Error! %06d renpu max is %d\n",mg->line,MSTC);
        return(0);
    }
    if(mr->renp > 1) {
        renproc_mec(mk,fpi);
        return(0);
    }
    for(i = 0; i < mr->rept; i++) {
        if(mg->btyp == 'r' || mg->btyp == 'd')
            dnote_mec(mk,0,"dumy");
        else note_mec(mk,0,"dumy");
    }
    apedalck_mec();  //2013.8.19
    return(0);
}

/* part case r or R */
int ptcrest_mec(struct mecksn *mk)
//struct mecksn *mk;         /* key signeture table */
{
    rest_mec(mk);
    pedalck_mec();
    if( mg->btyp == 's' ||
        mg->btyp == 't' ||
        mg->btyp == 'g' ||
        mg->btyp == 'a' ||
        mg->btyp == 'b' ||
        mg->btyp == 'u' ||
        mg->btyp == 'd') {
        syncepv_mec();   /* pitch vend */
        syncecnt_mec();  /* modul. control cg. */
        syncepcg_mec();  /* program change */
        syncetrs_mec();     /* transpose */
    }
    return(0);
}

/* fn_xxx.mec file read and skip line for line select option */
int mecfread_mec(char mwkf[STLN],int tbflg)
//char mwkf[STLN];
//int  tbflg;      /* top bottom flag 1:top 9:bottom */
{
    union MCODE u;
    unsigned char iword[4];
    int  j,k;
    int  c;
    int  rcnt;
    int  pf;
    FILE *fwc;

    k = 0;
    j = 0;
    rcnt = 0;
    pf = 0;
    if(mg->debug == 7) printf("mwkf=%s\n",mwkf);
    fwc = fopen_h(mwkf,"rb");
    while((c = fgetc(fwc)) != EOF) {
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            rcnt++;
            k = 0;
            if(mm->comptyp == 0) {
                u.wc[0] = iword[1];
                u.wc[1] = iword[0];
                u.wc[2] = iword[3];
                u.wc[3] = iword[2];
            }
            if(mm->comptyp == 1) {
                u.wc[0] = iword[0];
                u.wc[1] = iword[1];
                u.wc[2] = iword[2];
                u.wc[3] = iword[3];
            }
            if(u.wc[1] == 0xfb) fbdecode_mec(&u);
            if(u.wc[1] == 0xfd) fddecode_mec(&u);
            if(tbflg == 1) if(topchk_mec(iword)) {
                fclose(fwc);   /* close .mwk file */
                return(0);
            }
            if(tbflg == 9) btmchk_mec(iword,&pf);
        }
    }
    fclose(fwc);   /* close .mwk file */
    if(mg->debug == 7) printf("%s read count=%d\n",mwkf,rcnt);
    return(0);
}

/* get .mec object bar number check & code out */
int topchk_mec(unsigned char iword[4])
//unsigned char iword[4];
{
    long cbn;
    long sbn;

    switch(mm->brself) {
        case  1 : if(mm->movn >= mm->smovn) return(9);
                  break;
        case  2 : cbn = mm->bnum;
                  sbn = mm->sbarn;
                  if(cbn >= sbn) return(9);
                  break;
        case  3 : cbn = mm->movn * 100000 + mm->bnum;
                  sbn = mm->smovn * 100000 + mm->sbarn;
                  if(cbn >= sbn) return(9);
                  /* if(mm->movn > mm->smovn || cbn >= sbn) return(9); */
                  break;
        default : break;
    }
    if(mg->object) {
        fputc(iword[0],mg->fpo);
        fputc(iword[1],mg->fpo);
        fputc(iword[2],mg->fpo);
        fputc(iword[3],mg->fpo);
        mr->tcnt++;
    }
    return(0);
}

/* .mec object bottom bar number check & code out */
int btmchk_mec(unsigned char iword[4],int *pf)
//unsigned char iword[4];
//int  *pf;
{   /* bottom */
    long cbn;
    long ebn;

    if(*pf == 0) {
        switch(mm->mbendf) {
            case  1 : if(mm->movn <= mm->emovn) *pf = 1;
                      break;
            case  2 : cbn = mm->bnum;
                      ebn = mm->ebarn;
                      if(cbn >= ebn) *pf = 1;
                      break;
            case  3 : cbn = mm->movn * 100000 + mm->bnum;
                      ebn = mm->emovn * 100000 + mm->ebarn;
                      if(cbn >= ebn) *pf = 1;
                      break;
            default : break;
        }
    }
    if(*pf) btfputc_mec(iword);
    return(0);
}

/*   FB code ....................................
Bar          : FBxxyyyy
 xx(movn)    : movement number
 yyyy(bnum)  : bar number
 ex. -m0100017-_____________..
         xx(movn) = 1
         yyyy(bnum) = 17
*/
int fbdecode_mec(union MCODE *u)
//union MCODE *u;
{
    mm->movn = u->wc[0] & 0xff;
    mm->bnum = u->wi[1];
    if(mg->debug == 7) {
        printf("FB(%04x%04x) Bar data / movn=%d bnum=%d\n",
        u->wi[0],u->wi[1],mm->movn,mm->bnum);
    }
    return(0);
}

/*   FD code ....................................
Note off      : FD8xyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(voff)     : verociti off (0-120)

Note on       : FD9xyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(veon)     : verociti on : dynamik(0-9)
*/
int fddecode_mec(union MCODE *u)
//union MCODE *u;
{
    int  fdtyp;
    int  fdch;
    int  keyp;
    int  voff;
    int  veon;

    fdtyp = u->wc[0] & 0xf0;
    fdtyp = fdtyp >> 4;
    fdch = u->wc[0] & 0x0f;
    if(fdtyp == 8) {
        keyp = u->wc[3] & 0xff;
        voff = u->wc[2] & 0xff;
        if(mg->debug == 7) {
            printf("FD(%04x%04x) Note off / fdtyp=%d fdch=%d",
                u->wi[0],u->wi[1],fdtyp,fdch);
            printf(" keyp=%d voff=%d\n",keyp,voff);
        }
        mg->ntb[keyp]--;
    }
    if(fdtyp == 9) {
        keyp = u->wc[3] & 0xff;
        veon = u->wc[2] & 0xff;
        if(mg->debug == 7) {
            printf("FD(%04x%04x) Note on / fdtyp=%d fdch=%d",
                u->wi[0],u->wi[1],fdtyp,fdch);
            printf(" keyp=%d veon=%d\n",keyp,veon);
        }
        if(veon) mg->ntb[keyp]++;
        else mg->ntb[keyp]--;
    }
    return(0);
}

int btfputc_mec(unsigned char iword[4])
//unsigned char iword[4];
{
    if(mg->object) {
        fputc(iword[0],mg->fpo);
        fputc(iword[1],mg->fpo);
        fputc(iword[2],mg->fpo);
        fputc(iword[3],mg->fpo);
        mr->tcnt++;
    }
    return(0);
}

int pr_card(char *card)
//char *card;
{
    int  i;

    for(i = 0; i < 82; i++) {
        if(card[i] == '\r' || card[i] == '\n') {
            card[i] = '\0';
            break;
        }
    }
    if(mg->source) {
        if(mg->source == 1) printf("%s\n",card);
        if(mg->source == 2) prints_mec(mg->line,card);
    }
    return(0);
}

/* ./)) move to before bar line */
int Nachsch_mec()
{
    strcpy(Nachs,mg->card); /* save bar line */
    Nachf = 1;
    mg->prflg = 0;  /* print flag */
    return(0);
}

int Nachsbar_mec(struct mecksn *mk)
//struct mecksn mk;         /* key signeture table */
{
    int  j;
    char card[82];

    if(mg->source) {
        if(mg->source == 1) printf("%s",Nachs);
        if(mg->source == 2) prints_mec(mg->line,Nachs);
    }
    strcpy(card,mg->card);
    strcpy(mg->card,Nachs);
    rsphdyna_mec(); /* Reset phrase dyna */
    // bar_mec(); out 2015.8.5
    if(mg->debug == 7) printf("mm->byrd=%d\n",mm->byrd);
    if(mm->byrd != 1) return(0);
    for(j = 0; j < 78; j++)
        mg->ktbbak[j] = mg->ktb[j];
    keyini_mec();
    keyset_mec();
    //if(mg->debug == 3) keydmp_mec(&mk);
    if(mg->debug == 3) keydmp_mec(mk);  // 2025.4.6
    c_sync = mr->sync; /* lag offset sync check */
    Nachf = 0;
    strcpy(mg->card,card);
    return(0);
}
