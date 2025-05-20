
// ----------------------
//  AMuPLC (mebm.c)
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

#include "meb.h"
/* #include "mebc.h" */
/* int _stack = 65000;      stack size */
MIVARG *mv;
MIVMST *mm;
MIVMXM *mx;
MEBVAR *mb;
MEBSRG *ls;
MEBGRT *gr,*gd,*ga,*gm,*gs,*gc;
MEBEDT *ge;
MEBPH  *ph;
MEBCPS *cp;
MEBCSL *cs;
MEBANS *ma;
MECVAR *mr;
MEBVEX *vx;
MEBART *ar;
/* char *prtsym = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; */
char *prtsym = "0123456789abcdefghijklmnopqrstuvwxyz";
int  secnt;        /* section count */
char mfile[STLN];
int  backf;
char mebspa[MAXSL][82];
char mebspb[MAXSL][82];

int main(int argc,char *argv[])
//int  argc;
//char *argv[];
{
    MIVARG marg;   /* modeflag card etc. */
    MIVMST mmst;   /* memory symbol table */
    MIVMXM mxmb;   /* max. member */
    MEBVAR mebvar;     /* meb variable */
    MEBSRG lsa;    /* observe */
    MEBGRT lgr;    /* gate_rate */
    MEBGRT dgr;    /* define gate_rate */
    MEBGRT agr;    /* assign gate_rate */
    MEBGRT mgr;    /* modify gate_rate */
    MEBGRT sgr;    /* set gate_rate */
    MEBGRT cgr;    /* add/sub/clear gate_rate */
    MEBEDT mge;    /* edit val/flag */
    MEBPH  mph;    /* phrase */
    MEBCPS cps;    /* Chord Progress */
    MEBCSL csl;    /* Chord Select */
    MEBANS mar;    /* Available Note Scale */
    MECVAR mvar;   /* output codes variable */
    MEBVEX vex;    /* vertical exchange */
    MEBART mart;   /* dynamic expand symbol */
    int  i;
    int  j;
    int  k;
    int  l;
    int  m;
    char *pa;
    int  help;
    int  cline;
    int  copy;
    int  delf;
    int  tmvcf;
    int  mmvcf;
    char wkc[4];
    char dtopt[STLN];   /* debug or trace option */
    char sfile[STLN];
    char tfile[STLN];   /* Add top file */
    int  argfn[MAXA];
    int  yrtn;
    int  xrtn;
    int  yfile;
    int  xfile;
    int  yflp;
    int  xflp;
    char *xmf;
    int  slslf;
    char wk[8];
    int  pas;
    char wwk[8];
    int  partj;
    int  partk;
    char partc;
    int  Rsqsf; /* Right sequence shift flag */
    int  Lsqsf; /* Left sequence shift flag */

    mv = &marg;
    mm = &mmst;
    mx = &mxmb;
    mb = &mebvar;
    ph = &mph;
    ls = &lsa;
    gr = &lgr;
    ge = &mge;
    gd = &dgr;  /* define */
    ga = &agr;  /* assign */
    gm = &mgr;  /* modify */
    gs = &sgr;  /* set */
    gc = &cgr;  /* add/sub/clear */
    cp = &cps;
    ma = &mar;
    cs = &csl;
    mr = &mvar;
    vx = &vex;
    ar = &mart;
    ls->comptyp = 0;
    /* original alpha dos pascal */
    if(!strcmp(COMPUTER_TYPE,"PC9801")) ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"IBMPC"))  ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"SUN"))    ls->comptyp = 1;
    help = 0;
    backf = 0;
    Rsqsf = 0;
    Lsqsf = 0;
    mb->midi = 0;        /* exchange midi to cueing flag */
    mb->cueing = 0;      /* exchange cueing to midi flag */
    mv->debug = 0;
    mv->trace = 0;
    mv->synck = 0;         /* sync check */
    mv->list = 0;
    mv->source = 0;
    mv->object = 0;
    mv->beaut = 1;         /* beautify on */
    mv->allf = 0;
    mv->hf = 0;
    mv->symmax = 0;        /* Symbol Table max. line */
    mv->getsym = 0;        /* get symbol at part beautify */
    mb->bstflg = 0;
    mv->tctyp = STCTYP;
    mv->prtlvl = 1;        /* message level 3 (upper Worning3) */
    mv->msgflg = 0;
    mv->cspcf = 0;
    copy = 0;
    mv->wrnpf = 0;
    mv->copyf = 0;
    ph->exlc = 0;   /* Expand load file table counter */
    ph->exfc = 0;   /* Expand file table counter */
    ph->exrc = 0;   /* Expand Register & Store file counter */
    ph->esc = 0;    /* Expand Symbol table counter */
    mv->ipf = 0;    /* 0:mebspa 1:mebspb */
    mv->ipc = 0;    /* current spa spb count */
    delf = 0;
    if(argc == 1) {
        help_meb();
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
        if(!strcmp(dtopt,"HELP")) {
            help = 1;
            argfn[k] = 0;
        }
        if(!strcmp(dtopt,"-B")) mv->beaut = -1;
        if(!strcmp(dtopt,"-GS")) mv->getsym = 1;
        if(!strcmp(dtopt,"-H")) help = 2;
        if(!strcmp(SUPPORT,"Debug")) {
         if(!strcmp(dtopt,"-H")) help = 1;
         if(!strcmp(dtopt,"-S")) mv->source  = 4;
         if(!strcmp(dtopt,"-S1")) mv->source = 1;
         if(!strcmp(dtopt,"-S2")) mv->source = 2;
         if(!strcmp(dtopt,"-S3")) mv->source = 3;
         if(!strcmp(dtopt,"-S4")) mv->source = 4;  // main step 3
         if(!strcmp(dtopt,"-I")) mv->source = 1;
         if(!strcmp(dtopt,"-D"))  mv->debug = 1;
         if(!strcmp(dtopt,"-D1")) mv->debug = 1;
         if(!strcmp(dtopt,"-D2")) mv->debug = 2;
         if(!strcmp(dtopt,"-D3")) mv->debug = 3;   // Key_Transposition
         if(!strcmp(dtopt,"-D4")) mv->debug = 4;
         if(!strcmp(dtopt,"-D5")) mv->debug = 5;
         if(!strcmp(dtopt,"-D6")) mv->debug = 6;   // Chord Prg. -s3
         if(!strcmp(dtopt,"-D7")) mv->debug = 7;   // Chord select Prg.
         if(!strcmp(dtopt,"-D8")) mv->debug = 8;   // Chord select Note chk
         if(!strcmp(dtopt,"-D9")) mv->debug = 9;   // Chord select mec  chk
         if(!strcmp(dtopt,"-D10")) mv->debug = 10; // Transpose chk
         if(!strcmp(dtopt,"-D11")) mv->debug = 11; // Vartical modify chk
         if(!strcmp(dtopt,"-D12")) mv->debug = 12; // Decord Dynamic expand
         if(!strcmp(dtopt,"-D13")) mv->debug = 13; // Gate_rate chk
         if(!strcmp(dtopt,"-D14")) mv->debug = 14; // Memory alloc chk
         if(!strcmp(dtopt,"-D15")) mv->debug = 15; // Select Code Name chk
         if(!strcmp(dtopt,"-D16")) mv->debug = 16; // gcc -> clang check
         if(!strcmp(dtopt,"-SCHK")) mv->synck = 1;
         if(!strcmp(dtopt,"-T")) mv->trace = 1;
         if(!strcmp(dtopt,"-L")) mv->list = 1;
         if(!strcmp(dtopt,"-BB")) mv->beaut = -1;
         //if(!strcmp(dtopt,"-B")) mv->beaut = -1;
         if(!strcmp(dtopt,"-BL")) mv->beaut = -2;
         if(!strcmp(dtopt,"-C")) copy = 1;
         if(!strcmp(dtopt,"-ALL")) mv->allf = 1;
         if(!strcmp(dtopt,"-A")) mv->allf = 1;
         //if(!strcmp(dtopt,"-GS")) mv->getsym = 1;
         if(!strcmp(dtopt,"-M0")) mv->prtlvl = 0;  // all
         if(!strcmp(dtopt,"-M1")) mv->prtlvl = 1;  // upper worning level 1
         if(!strcmp(dtopt,"-M2")) mv->prtlvl = 2;  // upper worning level 2
         if(!strcmp(dtopt,"-M3")) mv->prtlvl = 3;  // upper worning level 3
         if(!strcmp(dtopt,"-M4")) mv->prtlvl = 4;  // upper error level 1
         if(!strcmp(dtopt,"-NM")) mv->prtlvl = 99; // no    msg level 9
         if(!strcmp(dtopt,"-CS")) mv->cspcf = 1; // chord select print check
        }
        if(dtopt[0] == '-') argfn[k] = 0;
    }
    if(help) {
        fhelp_meb(help);
        return(0);
    }
    mv->msglvl = 0;
    for(i = 1; i < argc; i++) {
        if(argfn[i]) {
            for(j = 0; j < 100; j++) mm->dblbar[j] = 0;
            mm->dblbc = 0;
            initval_meb();    /* init variables */
            xmf = argv[i];
            while(*xmf) {
                if(*xmf == '(') ls->brself = 1;
                xmf++;
            }
            if(ls->brself) {
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
                    if(j > 6) {
                        mv->errcnt++;
                        mv->msglvl = 3;
                        if(mv->msglvl >= mv->prtlvl)
                            printf("Error! argument %s\n",argv[i]);
                        break;
                    }
                }
                wk[j] = '\0';
                /* printf("wk=%s j=%d\n",wk,j); */
                switch (j) {
                    case  1 : sscanf(wk,"%d",&mm->ssequ);
                              ls->brself = 1;
                              break;
                    case  2 : sscanf(wk,"%d",&mm->ssequ);
                              ls->brself = 1;
                              break;
                    case  3 : sscanf(wk,"%d",&mm->sbarn);
                              ls->brself = 2;
                              break;
                    case  4 : sscanf(wk,"%d",&mm->sbarn);
                              ls->brself = 2;
                              break;
                    case  5 : sscanf(wk,"%d",&mm->sbarn);
                              ls->brself = 2;
                              break;
                    case  7 : for(j = 0, k = 2;j < 6;j++,k++) wwk[j] = wk[k];
                              wk[2] = '\0';
                              sscanf(wk,"%d",&mm->ssequ);
                              sscanf(wwk,"%d",&mm->sbarn);
                              ls->brself = 3;
                              break;
                    default : mv->errcnt++;
                              mv->msglvl = 3;
                              if(mv->msglvl >= mv->prtlvl)
                                printf("Error! sequence barn start select\n");
                              ls->brself = 0;
                              break;
                }
                if(pas) {
                    j = 0;
                    xmf++;
                    while(*xmf) {
                        if(*xmf == ')') break;
                        wk[j] = *xmf;
                        xmf++;
                        j++;
                        if(j > 6) {
                            mv->errcnt++;
                            mv->msglvl = 3;
                            if(mv->msglvl >= mv->prtlvl)
                                printf("Error! argument %s\n",argv[i]);
                            break;
                        }
                    }
                    wk[j] = '\0';
                    /* printf("wk=%s j=%d\n",wk,j); */
                    switch (j) {
                        case  1 : sscanf(wk,"%d",&mm->esequ);
                                  ls->mbendf = 1;
                                  break;
                        case  2 : sscanf(wk,"%d",&mm->esequ);
                                  ls->mbendf = 1;
                                  break;
                        case  3 : sscanf(wk,"%d",&mm->ebarn);
                                  ls->mbendf = 2;
                                  break;
                        case  4 : sscanf(wk,"%d",&mm->ebarn);
                                  ls->mbendf = 2;
                                  break;
                        case  5 : sscanf(wk,"%d",&mm->ebarn);
                                  ls->mbendf = 2;
                                  break;
                        case  7 : for(j = 0, k = 2;j < 6;j++,k++)
                                      wwk[j] = wk[k];
                                  wk[2] = '\0';
                                  sscanf(wk,"%d",&mm->esequ);
                                  sscanf(wwk,"%d",&mm->ebarn);
                                  ls->mbendf = 3;
                                  break;
                        default : mv->errcnt++;
                                  mv->msglvl = 3;
                                  if(mv->msglvl >= mv->prtlvl)
                                  printf("Error! barn sequence end select\n");
                                  ls->mbendf = 0;
                                  break;
                    }
                }
                /*
                if(ls->brself) {
                    printf("mm->ssequ=%d mm->sbarn=%d ",
                        mm->ssequ,mm->sbarn);
                    printf("mm->esequ=%d mm->ebarn=%d\n",
                        mm->esequ,mm->ebarn);
                }
                */
            }
            yrtn = mkmultif_h(argv[i],mfile,".meg",-1,&xrtn,&yfile,&xfile);
            xmf = mfile;
            k = 0;
            while(*xmf) {
                if(*xmf == '/') break;
                if(*xmf == '_') break;
                if(*xmf == '(') break;
                if(*xmf == ';') {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                        printf("Error! filename\n");
                    return(0);
                }
                tfile[k] = *xmf;
                xmf++;
                k++;
            }
            tfile[k] = '\0';
            filemake_h(0,tfile,".meg",mfile,&yflp,&xflp);
            mb->slsf = 3;
            if(yrtn == 99) mb->slsf = 1;
            if(xrtn == 36) mb->slsf = 2;
            xmf = argv[i];
            slslf = 0;
            while(*xmf) {
                if(*xmf == '(') break;
                if(*xmf == '/') slslf = 1;
                if(*xmf == '_') slslf = 1;
                if(*xmf == ';') mb->slsf = 4;
                xmf++;
            }
            if(yrtn == 0 && xrtn == 0 && yfile == 0 && xfile == 0) {
                if(slslf == 0) mb->slsf = 0;
            }
            if(mv->debug == 2) {
                printf("mfile = %s yrtn=%d xrtn=%d yfile=%d xfile=%d\n",
                mfile,yrtn,xrtn,yfile,xfile);
                printf("yflp=%d xflp=%d mb->slsf=%d\n",yflp,xflp,mb->slsf);
            }
            header_meb(mfile); /* check fn.meg file */
            strcpy(sfile,mfile);
            cline = selfil_meb(mfile);
            if(mv->beaut == -1) {
                mebmain(mfile);
            }
            /*
            for(j = 0; j < 36 ;j++) putchar(mx->tmvcp[j]);
            for(j = 0; j < cline; j++) {
                if(!(j % 36)) printf("\n");
                putchar(mx->mmvcp[j]);
            }
            printf("\n");
            */
            tmvcf = 0;
            for(j = 0; j < 36; j++) if(mx->tmvcp[j] != '.') {
                tmvcf = 1;
                if(mx->tmvcp[j] == '>') Rsqsf = 1;
                if(mx->tmvcp[j] == '<') Lsqsf = 1;
            }
            mmvcf = 0;
            for(j = 0; j < cline; j++) if(mx->mmvcp[j] != '.') mmvcf = 1;
            l = 0;
            //return(9999);
            while(mfile[l++] != '_');
            for(j = 0; j < cline; j++) {
                sprintf(wkc,"%02x",mx->track[j]);  /* %02d v_1.45 */
                mfile[l+1] = wkc[0];
                mfile[l+2] = wkc[1];
                for(k = 0; k < 36; k++) {
                    mfile[l] = *(prtsym+k);
                    if(mx->partf[j][k] == '*') {
                        mv->copyf = 1;
                        partj = j;
                        partk = k;
                        strcpy(sfile,mfile);
                    }
                    if(mx->partf[j][k] == '@') {
                        mv->copyf = 2;
                        if(delf != 3) delf = 2;
                    }
                    if(mx->partf[j][k] == '+') {
                        mv->copyf = 3;
                        delf = 3;
                    }
                    if(mx->partf[j][k] == '/') delf = 1;
                    if(mx->partf[j][k] == '&' && k > 0) mv->copyf = 4;
                }
            }
            if(delf && mv->beaut >= 0) {
                for(j = 0; j < cline; j++) {
                    mb->port = mx->port[j];
                    mb->chan = mx->channel[j];
                    /* mb->memb = mx->member[j]; */
                    sprintf(wkc,"%02x",mx->track[j]);  /* %02d v_1.45 */
                    xflp = mx->track[j];
                    mfile[l+1] = wkc[0];
                    mfile[l+2] = wkc[1];
                    for(k = 0; k < 36; k++) {
                        yflp = k;
                        mfile[l] = *(prtsym+k);
                        mb->trak = k;
                        /* to copy() */
                        if(mx->partf[j][k] == '/') mebrmvf_meb(mfile);
                    }
                }
            }
            if(mv->copyf && mv->beaut >= 0) {
                for(j = 0; j < cline; j++) {
                    mb->port = mx->port[j];
                    mb->chan = mx->channel[j];
                    /* mb->memb = mx->member[j]; */
                    sprintf(wkc,"%02x",mx->track[j]);  /* 97.10 */
                    xflp = mx->track[j];
                    mfile[l+1] = wkc[0];
                    mfile[l+2] = wkc[1];
                    for(k = 0; k < 36; k++) {
                        yflp = k;
                        mfile[l] = *(prtsym+k);
                        mb->trak = k;
                        /* to copy() */
                        if(mx->partf[j][k] == '&') {
                            strcpy(tfile,mfile);
                            m = 0;
                            while(tfile[m]) {
                                if(tfile[m] == '_') break;
                                m++;
                            }
                            m++;
                            tfile[m] = '0';    /* Top file */
                            mebcopy_meb(mfile,tfile); /* xch */
                            printf("File %s was copyed to %s\n",
                                tfile,mfile);
                        }
                        if(mx->partf[j][k] == '@' || mx->partf[j][k] == '+') {
                            if(mv->copyf) {
                                mebcopy_meb(mfile,sfile);
                                printf("File %s was copyed to %s\n",
                                    sfile,mfile);
                                if(delf == 2) {
                                    mebrmvf_meb(sfile);
                                    mx->partf[partj][partk] = '.';
                                }
                            }
                            else {
                                mv->errcnt++;
                                mv->msglvl = 3;
                                if(mv->msglvl >= mv->prtlvl)
                                printf("Error! no source file \"*\"\n");
                            }
                        }
                    }
                }
            }
            if(tmvcf && mv->beaut >= 0)
                beautmvc_meb(mfile,cline);  /* MXMB */
            if(mmvcf && mv->beaut >= 0)
                beaummvc_meb(mfile,cline);  /* MXMB */
            if(Rsqsf || Lsqsf) {
                seqshift_meb(mfile,cline);  /* Seq. shift */
            }
            if(mx->gmvcp == '?') {
                gserch_meb(mfile,cline);
            }
            if(mx->gmvcp == '%') {
                gclear_meb(mfile,cline);
            }
            if(mv->hf && mv->beaut != -1) {
                mcline_meb();
                cline = selfil_meb(mfile);   // 2025.4.2 core dump check
            }
            l = 0;
            while(mfile[l++] != '_');
            for(j = 0; j < cline; j++) {
                mb->port = mx->port[j];
                mb->chan = mx->channel[j];
                /* mb->memb = mx->member[j]; */
                sprintf(wkc,"%02x",mx->track[j]);  /* %02d v_1.45 */
                xflp = mx->track[j];
                mfile[l+1] = wkc[0];
                mfile[l+2] = wkc[1];
                for(k = 0; k < 36; k++) {
                    yflp = k;
                    mfile[l] = *(prtsym+k);
                    mb->trak = k;
		    partc = mx->partf[j][k];
		    /*
                    printf("mb->slsf=%d mfile=%s partc='%c'\n",
			mb->slsf,mfile,partc);
                    printf("yflp=%d yfile=%d, xflp=%d xfile=%d\n",
			yflp,yfile,xflp,xfile);
                    */
                    switch (mb->slsf) {
                        case  1 : if(yflp == yfile)
                                  mebfchk_meb(mfile,partc);
                                  break;
                        case  2 : if(xflp == xfile)
                                  mebfchk_meb(mfile,partc);
                                  break;
                        case  3 : if(yflp == yfile && xflp == xfile)
                                  mebfchk_meb(mfile,partc);
                                  break;
                        case  4 : if(yflp <= yfile && xflp <= xfile)
                                  mebfchk_meb(mfile,partc);
                                  break;
                        default : // 2025.4.2
                                  mebfchk_meb(mfile,partc);
                                  break;
                    }
                    if(copy) return(0);
                }
            }
        }
    }
    if(mv->hf && mv->beaut != -1) bottom_meb();
    return(0);
}

int mebrmvf_meb(char mfile[STLN])
//char mfile[STLN];
{
    int  m;
    char wk[STLN];

    m = 0;
    while(mfile[m]) {
        if(mfile[m] == '.') break;
        m++;
    }
    m += 3;
    mfile[m] = 'b';
    if(!remove(mfile)) printf("Meb file %s removed\n",mfile);
    mfile[m] = 'c';
    if(!remove(mfile)) printf("Mec file %s removed\n",mfile);
    mfile[m] = 'g';
    if(!remove(mfile)) printf("Meg file %s removed\n",mfile);
    m = 0;
    while(mfile[m]) {
        wk[m] = mfile[m];
        if(mfile[m] == '.') break;
        m++;
    }
    wk[m] = '\0';
    strcat(wk,".html");
    if(!remove(wk)) printf("html file %s removed\n",wk);
    return(0);
}

int mebmain(char *inf)
//char *inf;
{
    char outf[STLN];
    char bfile[STLN];
    char gfile[STLN];
    FILE *fpi,*fpo;
    int  i;
    int  kj;
    struct mivksn mk;         /* key signeture table */
    struct mivsym ms[MEXP];   /* symbol table */
    int  bk1,bk2;

    mv->line = 0;
    mb->bnum = 0;
    mb->psyn = 0;
    mb->acci = 0;
    mv->pass = 0;
    mv->step = 0;
    if(mv->beaut == -1) {
        //if(!backf == 1) {
        if(backf == 0) {  // 2025.4.1
            backf = 1;
            kj = 0;
            while(1) {
                if(mfile[kj] == '\0') break;
                gfile[kj] = mfile[kj];
                bfile[kj] = mfile[kj];
                if(mfile[kj] == '_') break;
                kj++;
                if(kj > STLN) break;
            }
            gfile[kj] = '\0';
            bfile[kj] = '\0';
            strcat(gfile,".meg");
            strcat(bfile,".meb");
            /* printf("mfile = %s gfile = %s\n",mfile,gfile); */
            remove(gfile);
            fpo = fopen_h(gfile,"w");
            fpi = fopen_h(bfile,"r");
            if(mv->trace || mv->source) printf("\n");
            while(fgets(mv->card, 82, fpi)) {
                mv->line++;
                fputs(mv->card,fpo);  
                if(mv->trace || mv->source)
                    printf("%s", mv->card);
                if(mv->card[0] == '$' && mv->card[2] == '$') break; /* EOF */
            }
            fclose(fpi);
            fclose(fpo);
            printf("Beautify back \"%s\" from \"%s\"\n",
                gfile,bfile);
        }
        beauback_meb(inf);
        mv->wflag = 1;     /* beau back source option enable */
        return(0);
    }
    mv->mfflg = chkmfile_miv(inf);   /* check main file _x00. */
    /*
    if(mv->list) {
        printf("\nList file \"%s\"\n",inf);
        if((fpi = fopen_h(inf,"rb")) != 0) {
            while(fgets(mv->card, 82, fpi)) {
                mv->line++;
                printf("%s", mv->card);
                if(mv->card[2] == '$') break;
            }
            printf("File \"%s\" total line = %d\n",inf, mv->line);
        }
        fclose(fpi);
        return(0);
    }
    */
    if(mv->beaut == 1) {
        mksym_miv(&mk);         /* make key symbol table */
        mkexptmb_meb(inf,ms);       /* make expression table */
        if(mv->mfflg) {
            mv->step = 1;
            if(!mb->GKTPf) mb->GKTPv = 0;
            mbeautif_meb(inf,&mk,ms,0); /* out mebsp1.tmp */
            mv->step = 2;
            cp->chgf = cp->achgf;
            mchdsel_meb(inf,&mk,ms);
            mv->step = 3;
            if(mb->decxp) modsmpte_meb(inf,&mk,ms);
            mv->msglvl = 1;
            if(mv->msglvl >= mv->prtlvl) {
                printf("Beautify %s %d bars, %d lines end\n",
                inf,mb->bnum,mv->line);
            }
        }
        else {
            if(mv->debug == 3)
                printf("mebmain() inf=%s mb->slsf = %d mv->getsym = %d\n",
                inf,mb->slsf,mv->getsym);
            if(mv->getsym == 1) {
                mb->bstflg = 1;
                mb->albdflg = 1;
                if(mb->slsf == 2 || mb->slsf == 3) mgetsym_meb(inf,&mk,ms);
            }
            mv->pass = 0;
            mv->mmpos = 0;
            if(!mb->GKTPf) mb->GKTPv = 0;
            mv->step = 1;
            mv->ipf = 0;
            mv->ipa = 0;
            mv->ipb = 0;
            pbeautif_meb(inf,&mk,ms);
            bk1 = mb->xstolf; /* {} -> () exchange flag */
            bk2 = mb->xltosf; /* save flag */
            mv->step = 2;
            cp->chgf = cp->achgf;
            pchdsel_meb(&mk,ms);
            mb->xstolf = bk1; /* back excange flag */
            mb->xltosf = bk2;
            mv->step = 3;
            trnstep_meb(inf,&mk,ms); 
            mv->step = 4;
            finstep_meb(inf,&mk,ms); 
            mv->msglvl = 1;
            if(mv->msglvl >= mv->prtlvl) {
                printf("Beautify %s %d bars, %d lines end\n",
                inf,mb->bnum,mv->line);
            }
        }
        return(0);
    }
    if(mv->beaut == -2) {
        strcpy(outf,inf);  /* outf : input (back up file) */
        i = 0;
        while(outf[i++] != '.');
        outf[i] = '\0';
        strcat(outf,"meb");
        printf("\n\"%s\" list backup file\n",outf);
        fpi = fopen_h(outf,"r");
        while(fgets(mv->card, 82, fpi)) {
            mv->line++;
            printf("%s", mv->card);
            if(mv->card[2] == '$') break;
        }
        printf("\"%s\" total line = %d\n",outf, mv->line);
        fclose(fpi);
        return(0);
    }
    return(0);
}

int initval_meb()
{
    int  i;

    for(i = 0; i < MMST; i++) mm->onpc[i] = 0; /* clear on part count */
    mb->ksin = 0;
    mb->ogkval = 0;      /* C */
    mb->cline = 0;       // 2025.4.2
    mb->amnflg1 = 0;
    mb->aclrntl = 0;     /* beautify clear note lines all flag off */
    mb->aclrstl = 0;     /* beautify clear rest lines all flag off */
    mb->aclrsml = 0;     /* beautify clear symbol lines all flag off */
    mb->aclrpls = 0;     /* clear plus lines all flag off */
    mb->aclrcom = 0;     /* clear comment lines all flag off */
    mb->aclrchd = 0;     /* clear chord lines all flag off */
    mb->adelchk = 0;     /* del chord check lines all flag off */
    mb->adelexp = 0;     /* delete expand lines all flag off */
    mb->aclrdpl = 0;
    mb->arenumf = 0;
    mb->xrenumf = 0;
    mb->crenumf = 0;
    mb->albdflg = 0;
    mb->apbdflg = 0;
    mb->asbdflg = 0;
    mb->axstolf = 0;
    mb->axltosf = 0;
    mb->abstflg = 0;
    mb->abexchf = 0;
    mb->agaterf = 0;
    mb->apitchf = 0;
    mb->amodulf = 0;
    mb->acntlnf = 0;
    mb->acntldf = 0;
    mb->aprgchgf = 0;     /* progran change flag off */
    mb->atransvf = 0;
    mb->aveltef = 0;
    mb->anotecf = 0;
    mb->arstmpf = 0;
    mb->arssmptf = 0;
    mb->arsdynaf = 0;
    mb->aclrntsf = 0;
    mb->aobservf = 0;
    mb->achdobf = 0;
    mb->aexpnl = 0;      /* expanded note line all show/hide flag */
    mb->expldf = 0;      /* load expand flag */
    mb->expsvf = 0;      /* save expand flag */
    mb->expnl = 0;       /* expanded note line all show/hide flag */
    mb->dexpcl = 0;      /* Dynamic expand symbol clear flag */
    mb->adexpcl = 0;     /* Dynamic expand symbol clear flag all */
    mb->ahcpl = 0;       /* Hide chord progression lines */
    mb->rbtch = 'n';     /* Rubart type : f q t b d m s n _/ = 1000 */
    mb->decxp = 0;       /* Decode Dynamic expand note lines */
    mb->dexpn = 0;
    mb->expff = 0;
    mb->aexpff = 0;
    mb->expstf = 0;
    mb->aexpstf = 0;
    mb->exprsf = 0;
    mb->aexprsf = 0;
    mb->clrontf = 0;
    mb->aclrontf = 0;
    mb->agchdxf = 0;
    mb->smpteh = 0;
    mb->smptem = 0;
    mb->smptes = 0;
    mb->smptef = 0.0;
    mb->smplagh = 0;
    mb->smplagm = 0;
    mb->smplags = 0;
    mb->smplagf = 0.0;
    mb->mnflg1 = 0;
    mb->rept = 1;
    mb->sync = 0;
    mb->adsyn = 0;
    mb->memb = 0;
    mb->gentyp = 4;
    mb->shiftv = 0;
    mb->ktrnsv = 0; // Important 2025.4.3
    mb->ptrnsv = 0; // 2025.4.3
    mb->knsftv = 0; // 2025.4.3
    mb->strnsv = 0; // 2025.4.3
    mb->tpv = 0;    // 2025.4.3
    mb->nsftv = 0;  // 2025.4.3
    mb->nsftvd = 0; // 2025.4.3
    mb->nsftf = 0;  // 2025.4.3
    mv->mcpos = 0;
    mv->line = 0;   // 2025.4.2
    mv->mmpos = 0;
    mv->dped = '.';
    mv->dflg = 'c';
    mv->mtsymb = 0; 
    mv->mkscl = 0;
    mv->mksin = 0;
    mv->mryhm = 0;
    mv->mtime = 0;
    mv->plps = 74;
    mv->mdps = 75;
    mv->prps = 76;
    mv->spos = 77;
    mv->dpos = 78;
    mv->wflag = 0;
    mv->mdymc = 0;
    mv->mtemp = 600.00;
    mv->tempo = 600.00;
    //------- %xx Legart 16 --> 50 Stac meb11.c ---
    mv->reprm = 25;      // Renpu repeat gate rate Set param. rpv 2013.11.20
    //-------
    mb->sflame = 30;     /* flame */
    mb->port = 0;
    mb->chan = 0;
    mb->sequ = 0;        /* init */
    mb->xbnum = 0;       /* renumber */
    mb->ybnum = 1;       /* renumber start value */
    mb->cbnum = 0;       /* continuous_renumber */
    mb->splt = 1;
    mb->renp = 1;
    mb->bartyp = 'm';
    if(mv->getsym) mv->getsym = 1;  /* get symbol at part option */
    ls->brself = 0;      /* bar number select flag */
    ls->mbendf = 0;      /* bar number end select flag */
    mm->ssequ = 0;       /* start sequence number */
    mm->sbarn = 1;       /* start bar number */
    mm->esequ = 36;      /* end sequence number */
    mm->ebarn = 9999;    /* end bar number */
    secnt = 0;           /* section count */
    keyinimb_meb();      /* ktb init */
    mb->delbarv = 0;     /* clear delete bar value */
    mb->delbarf = 0;     /* clear delete bar flag */
    mb->addbarv = 0;     /* clear add bar value */
    mv->errcnt = 0;
    mv->wrncnt = 0;
    mb->clvelf = 0;
    mb->velv = 5;
    ph->exlc = 0;        /* Expand load file table counter */
    ph->exfc = 0;        /* Expand file table counter */
    ph->exrc = 0;        /* Expand Register & Store file counter */
    ph->esc = 0;         /* Expand Symbol table counter */
    mb->gchdf = 0;       /* Generate global chord lines */
    mb->gchdsf = 0;      /* Generate global chord select lines */
    mb->gchdlf = 0;
    mb->gtrnsv = 0;      /* global transposition value */
    mb->GTPf = 0;        /* Gloval TransPosition flag */
    mb->GTPv = 0;        /* Gloval TransPosition value */
    mb->GKTPf = 0;       /* Gloval Key TransPosition flag */
    mb->GKTPv = 0;       /* Gloval Key TransPosition value */
    mb->SKTPv = 0;       /* Sequence Key TransPosition value */
    ma->ansf = 0;        /* Available Note Scale flag off */
    ma->aaf = 0;
    ma->scvf = 0;
    cp->ascnvf = 0;
    cp->achgf = 0;
    init_gate_rate();    /* gate_rate */
    initcp_meb();
    mb->bclall = 0;      /* Beau Cline file check */
    return(0);
}

int startval_meb()
{
    int  i;

    mb->lowip = 0;     /* low instrument area position */
    mb->higip = 128;   /* high instrument area position */
    mb->cline = 0;
    mb->tpv = 0;       /* transpose value */
    mb->ptrnsv = 0;    /* xx transposition value */
    mb->strnsv = 0;    /* shift transpose value */
    mb->knsftv = 0;
    mb->nsftv = 0;
    mb->nsftvd = 0;
    mb->nsftf = 0;
    mb->trktyp = 0;    /* transpose key type */
    /* mb->renp = 0; */
    mb->durtev = 0;    /* #modify_start add duration rates */
    mb->durtevd = 0;   /* #modify_start add duration rates default */
    mb->veltev = 12.8;  /* #modify_start add velocity base rates */
    mb->notecv = 0;
    mb->notecf = 0;
    mb->anotecf = 0;
    mb->trsnf = 0;
    mb->renumf = mb->arenumf;
    mb->inclf = 0;
    mb->pitchv = 0;    /* default synth pitch vend */
    mb->modul = 0;     /* synth modulation */
    mb->cntlno = 0;    /* synth control change no. */
    mb->cntlnod = 0;   /* synth control change no. default */
    mb->cntldt = 0;    /* synth control change data */
    mb->cntldtd = 0;   /* synth control change data default */
    mb->prgchgf = 0;   /* progran change flag off */
    mb->smpmid = 0;    /* smpte format or midi format flag, 0:smpte 1:midi */
    mb->delbarv = 0;   /* clear delete bar value */
    mb->delbarf = 0;   /* clear delete bar flag */
    mb->addbarv = 0;   /* clear add bar value */
    mb->sequ = 0;
    mb->bnum = 0;
    /* ls->sequ = 0; */
    /* ls->bnum = 0; */
    mb->bsbgnp = 0;
    if(mb->abstflg) mb->bstflg = 1;
    mv->dflg = 'm';
    mb->lbdflg = mb->albdflg;
    mb->pbdflg = mb->apbdflg;
    mb->sbdflg = mb->asbdflg;
    mv->velo = 12.8;
    mv->mdymc = 100.0;
    mb->ryhm = 0;
    mb->time = 4;
    mv->tempo = 600.00;
    mb->binstf = -1;
    mb->sync = 0;
    ls->sync = 0;
    mb->adsyn = 0;
    mb->mbartyp = 'C';
    mv->msglvl = 1;
    mv->errcnt = 0;
    mv->wrncnt = 0;
    mv->line = 0;
    mb->xltosf = mb->axltosf;
    mb->xstolf = mb->axstolf;
    mb->mnflg1 = mb->amnflg1;
    mb->bexchf = mb->abexchf;
    mb->gaterf = mb->agaterf;
    mb->pitchf = mb->apitchf;
    mb->modulf = mb->amodulf;
    mb->cntlnf = mb->acntlnf;
    mb->cntldf = mb->acntldf;
    mb->prgchgf = mb->aprgchgf;
    mb->transvf = mb->atransvf;
    mb->veltef = mb->aveltef;
    mb->notecf = mb->anotecf;
    mb->clrntl = mb->aclrntl;
    mb->clrstl = mb->aclrstl;
    mb->clrsml = mb->aclrsml;
    mb->clrpls = mb->aclrpls;
    mb->clrcom = mb->aclrcom;
    mb->clrchd = mb->aclrchd;
    mb->delchk = mb->adelchk;
    mb->delexp = mb->adelexp;
    mb->clrdpl = mb->aclrdpl;
    mb->clrntsf = mb->aclrntsf;
    mb->clrontf = mb->aclrontf;
    mb->observf = mb->aobservf;
    mb->chdobf = mb->achdobf;
    mb->expnl = mb->aexpnl;
    mb->dexpcl = mb->adexpcl;
    mb->dexpn = 0;
    mb->expff = mb->aexpff;
    mb->expstf = mb->aexpstf;
    mb->exprsf = mb->aexprsf;
    mb->hcpl = mb->ahcpl;
    cp->scnvf = cp->ascnvf;
    cp->chgf = cp->achgf;
    cp->npc = 0;
    cp->ocpf = 0;   /* clear Rule file name flag */
    mb->expldf = 0;      /* load expand flag */
    mb->expsvf = 0;      /* save expand flag */
    mb->clvelf = 0;
    mb->velv = 5;
    ph->exlc = 0;   /* Expand load file table counter */
    ph->exfc = 0;   /* Expand file table counter */
    ph->exrc = 0;   /* Expand Register & Store file counter */
    ph->esc = 0;    /* Expand Symbol table counter */
    mb->SKTPv = 0;  /* Sequence Key TransPosition value */
    ma->nsf = ma->ansf;  /* Availavle Note Scale on/off flag */
    edtval_init();
    initcp_meb();
    ma->aaf = 0;    /* Available Note Scale active flag */
    ma->arp = 60; /* C */
    ma->abp = 4;  /* * */
    for(i = 0; i < 52; i++) ma->avc[i] = '.';
    mb->gchdf = 0;       /* Generate global chord lines */
    mb->gchdsf = 0;      /* Generate global chord select lines */
    for(i = 0; i < 10; i++) cp->npcd[i][0] = '\0';
    vx->gvc = 0;
    vx->tmc = 0;
    vx->dxc = 0;
    vx->vlc = 0;
    vx->pvc = 0;
    vx->mdc = 0;
    return(0);
}

int edtval_init()
{
    ge->asn_gr_v = 0;
    ge->asn_gr_f = 0;
    ge->asn_md_v = 0;
    ge->asn_md_f = 0;
    ge->asn_pv_v = 0;
    ge->asn_pv_f = 0;
    ge->asn_tp_v = 0;
    ge->asn_tp_f = 0;
    ge->asn_vb_v = 0;
    ge->asn_vb_f = 0;
    ge->asn_vl_v = 0;
    ge->asn_vl_f = 0;
    ge->asn_bt_v = 0;
    ge->asn_bt_f = 0;
    ge->asn_hm_v = 0;
    ge->asn_hm_p = 0;
    ge->asn_hm_f = 0;

    ge->def_gr_v = 0;
    ge->def_gr_f = 0;
    ge->def_rp_v = 0;
    ge->def_rp_f = 0;
    ge->def_md_v = 0;
    ge->def_md_f = 0;
    ge->def_pv_v = 0;
    ge->def_pv_f = 0;
    ge->def_tp_v = 0;
    ge->def_tp_f = 0;
    ge->def_vb_v = 0;
    ge->def_vb_f = 0;
    ge->def_vl_v = 0;
    ge->def_vl_f = 0;
    ge->def_bt_v = 0;
    ge->def_bt_f = 0;
    ge->def_hm_v = 0;
    ge->def_hm_p = 0;
    ge->def_hm_f = 0;

    ge->mod_gr_v = 0;
    ge->mod_gr_f = 0;
    ge->mod_md_v = 0;
    ge->mod_md_f = 0;
    ge->mod_pv_v = 0;
    ge->mod_pv_f = 0;
    ge->mod_tp_v = 0;
    ge->mod_tp_f = 0;
    ge->mod_vb_v = 0;
    ge->mod_vb_f = 0;
    ge->mod_vl_v = 0;
    ge->mod_vl_f = 0;
    ge->mod_bt_v = 0;
    ge->mod_bt_f = 0;
    ge->mod_hm_v = 0;
    ge->mod_hm_p = 0;
    ge->mod_hm_f = 0;

    ge->set_gr_v = 0;
    ge->set_gr_f = 0;
    ge->set_rp_v = 0;
    ge->set_rp_f = 0;
    ge->set_md_v = 0;
    ge->set_md_f = 0;
    ge->set_pv_v = 0;
    ge->set_pv_f = 0;
    ge->set_tp_v = 0;
    ge->set_tp_f = 0;
    ge->set_vb_v = 0;
    ge->set_vb_f = 0;
    ge->set_vl_v = 0;
    ge->set_vl_f = 0;
    ge->set_bt_v = 0;
    ge->set_bt_f = 0;
    ge->set_hm_v = 0;
    ge->set_hm_p = 0;
    ge->set_hm_f = 0;

    ge->asc_gr_v = 0;
    ge->asc_gr_f = 0;
    ge->asc_md_v = 0;
    ge->asc_md_f = 0;
    ge->asc_pv_v = 0;
    ge->asc_pv_f = 0;
    ge->asc_tp_v = 0;
    ge->asc_tp_f = 0;
    ge->asc_vb_v = 0;
    ge->asc_vb_f = 0;
    ge->asc_vl_v = 0;
    ge->asc_vl_f = 0;
    ge->asc_bt_v = 0;
    ge->asc_bt_f = 0;
    ge->asc_hm_v = 0;
    ge->asc_hm_p = 0;
    ge->asc_hm_f = 0;
    return(0);
}

int initcp_meb()
{
    int  i;

    cp->ocpf = 0;
    cp->key = 0;
    cp->jky = 0;
    cp->iky = 0;
    cp->caf = 0;
    cp->crp = 60; /* C */
    cp->cbp = 1;  /* * */
    for(i = 0; i < 52; i++) cp->cvc[i] = '.';
    for(i = 0; i < MAXLV; i++) {
        cp->ef[i] = 0;
        cp->n[i] = 0;
    }
    cp->bf = 0;
    /* cp->chgf = 0; */
    cp->npc = 0;
    cp->nnc = 0;
    return(0);
}

int mopen_meb()
{
    int  ipf;

    ipf = mv->ipf;
    if(ipf == 0) mv->ipf = 1;
    else mv->ipf = 0;
    mv->ipc = 0;
    if(mv->debug == 14) {
        printf("Debug14 mopen_meb() mv->step = %d mv->ipf = %d ",
        mv->step,mv->ipf);
    }
    if(mv->ipf == 0) {
        mv->ipa = 0; /* clear mebspa line count */
        if(mv->debug == 14) printf("mv->ipb = %d\n",mv->ipb);
    }
    if(mv->ipf == 1) {
        mv->ipb = 0; /* clear mebspb line count */
        if(mv->debug == 14) printf("mv->ipa = %d\n",mv->ipa);
    }
    return(0);
}

int mprintf_meb(char card[82])
//char card[82];
{
    if(mv->ipf == 0) {
        strcpy(mebspa[mv->ipa],card);
        mv->ipa++;
        if(mv->ipa > MAXSL) {
            mv->wrncnt++;
            mv->msglvl = 2;
            if(mv->msglvl >= mv->prtlvl)
            printf("Worning! source file over than %d\n",MAXSL);
            return(-1);
        }
        if(mv->debug == 16) {
            printf("\n//main.c mebspa[] mv->ipf=%d mv->ipa=%d //\n",
            mv->ipf,mv->ipa); 
            printf("%s\n",mebspa[mv->ipa]); 
        }
    }
    if(mv->ipf == 1) {
        strcpy(mebspb[mv->ipb],card);
        mv->ipb++;
        if(mv->ipb > MAXSL) {
            mv->wrncnt++;
            mv->msglvl = 2;
            if(mv->msglvl >= mv->prtlvl)
            printf("Worning! source file over than %d\n",MAXSL);
            return(-1);
        }
        if(mv->debug == 16) {
            printf("\n//main.c mebspb[] mv->ipf=%d mv->ipb=%d //\n", 
            mv->ipf,mv->ipb); 
            printf("%s\n",mebspb[mv->ipb]); 
        }
    }
    /* if(mv->debug == 14) printf("%s\n",card); */
    return(0);
}

int mgets_meb(char card[82])
//char card[82];
{
    if(mv->ipf == 0) {
        strcpy(card,mebspb[mv->ipc]);
        mv->ipc++;
        if(mv->ipc > mv->ipb) return(0); /* Eof */
        //if(mv->list) printf("%s\n",card);
        /*
        if(mv->debug == 16) {
            printf("mgets_() mebspb[] mv->ipf=%d mv->ipc=%d mv->ipb=%d\n",
            mv->ipf,mv->ipc,mv->ipb);
            printf("%s\n",card);
        }
        */
    }
    if(mv->ipf == 1) {
        strcpy(card,mebspa[mv->ipc]);
        mv->ipc++;
        if(mv->ipc > mv->ipa)  return(0); /* Eof */
        /*
        if(mv->debug == 16) {
            printf("mgets_() mebspa[] mv->ipf=%d mv->ipc=%d mv->ipa=%d\n",
            mv->ipf,mv->ipc,mv->ipa);
            printf("%s\n",card);
        }
        */
    }
    return(1);
}

int mcopy_meb(char outf[STLN])
//char outf[STLN];
{
    int  i;
    FILE *fpw;

    fpw = fopen_h(outf,"w");
    if(mv->debug == 14) printf("Debug14 mcopy_meb(%s) mv->ipf = %d ",
        outf,mv->ipf);
    if(mv->ipf == 0) {
        if(mv->debug == 14) printf("mv->ipa = %d\n",mv->ipa);
        for(i = 0; i < mv->ipa; i++) {
            fprintf(fpw,"%s\n",mebspa[i]);
            /* printf("%s\n",mebspa[i]); */
        }
    }
    if(mv->ipf == 1) {
        if(mv->debug == 14) printf("mv->ipb = %d\n",mv->ipb);
        for(i = 0; i < mv->ipb; i++) {
            fprintf(fpw,"%s\n",mebspb[i]);
            /* printf("%s\n",mebspb[i]); */
        }
    }
    fclose(fpw);
    return(0);
}

int help_meb()
{
    printf("\n Music Edit File Beautifuler V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    if(!strcmp(SUPPORT,"Release")) {
     printf(" meb  fn : [fn/xyy]\n");
     printf("           fn File name\n");
     printf("               x:Sequence yy:Track\n");
     printf("      Option :  -h(help)\n\n");
     printf("          original              edit               result\n");
     printf("        0123456789ABCD      0123456789ABCD      0123456789ABCD\n");
     printf("     (.)..............    . ..../.}.......    . ..............\n");
     printf("   00 . CCCCCCC.......    . CCCCCCC.......    . CCCC.Cc.......\n");
     printf("   01 . M.............    . *.+...........    . M.M...........\n");
     printf("   02 . S.....S.......    . S...S.S.......    . S.....s.......\n");
     printf("   03 . BB............    / BB............    . ..............\n");
     printf("\n");
     printf("       (?): Check and find files all\n");
     printf("       (%c): Remove sleep files all\n",'%');
     printf("        & : Add file from Members top file\n");
     printf("        * : Copy & Move source file\n");
     printf("        + : Copy destination file\n");
     printf("        @ : Move destination file\n");
     printf("        ? : Check and find file\n");
     printf("        / : Remove file\n");
     printf("        > : Sequence right shift\n");
     printf("        < : Sequence left shift\n");
     printf("        } : Active to Sleep file (Large char to Small char)\n");
     printf("        { : Sleep to Active file (Small char to Large char)\n");
     printf("        Not support mix bar types in same member  . SGM... \n");
     printf("        Bar Types (L M H T G S A B U R D)\n");
     printf("\n");
     return(0);
    }
    printf(" meb  fn [,fn] [options]\n");
    printf("      fn/xyy      [-s -gs]\n");
    printf("      options : -b(beautifuler back) -s(source s1,s2,s3)\n");
    printf("                -h(help) -d(debug) -gs(get global symbol)\n");
    printf("                -l(list) -bl(backup list) -nm(no message)\n");
    printf("                -m0..-m3(message level 0 to 3 default:-m1)\n");
    printf("                -cs(chord select print check)\n");
    printf("                -ma(memory allocate)\n");
    printf(" ex.1 meb fn -s         : beautifuler & source\n");
    printf(" ex.2 meb fn -b         : beautifuler back\n");
    printf(" ex.3 meb -c : Copy or remove file at conductor file(fn.meg)\n");
    printf("          original              edit               result\n");
    printf("        0123456789ABCD      0123456789ABCD      0123456789ABCD\n");
    printf("     (.)..............    . ..../.}.......    . ..............\n");
    printf("   00 . CCCCCCC.......    . CCCCCCC.......    . CCCC.Cc.......\n");
    printf("   01 . M.............    . *.+...........    . M.M...........\n");
    printf("   02 . S.....S.......    . S...S.S.......    . S.....s.......\n");
    printf("   03 . BB............    / BB............    . ..............\n");
    printf("\n");
    printf("       (?): Check and find files all\n");
    printf("       (%c): Remove sleep files all\n",'%');
    printf("        & : Add file from Members top file\n");
    printf("        * : Copy & Move source file\n");
    printf("        + : Copy destination file\n");
    printf("        @ : Move destination file\n");
    printf("        ? : Check and find file\n");
    printf("        / : Remove file\n");
    printf("        > : Sequence right shift\n");
    printf("        < : Sequence left shift\n");
    printf("        } : Active to Sleep file (Large char to Small char)\n");
    printf("        { : Sleep to Active file (Small char to Large char)\n");
    printf("        Not support mix bar types in same member  . SGM... \n");
    printf("        Bar Types (L M H T G S A B U R D)\n");
    printf("\n");
    return(0);
}

int fhelp_meb(int help)
//int  help;
{
    int  c;
    FILE *fpd;
    char scrf[100];

    printf("\n Music Edit File Beautifuler V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    if(help == 2) {
      if((fpd = fopen("mebhlp2.dat","r")) == NULL) {
        sprintf(scrf,"%s",TACTDIR);
        strcat(scrf,"mebhlp2.dat");
        fpd = fopen_h(scrf,"r");
      }
    } 
    else {
      if((fpd = fopen("mebhlp.dat","r")) == NULL) {
        sprintf(scrf,"%s",TACTDIR);
        //sprintf(scrf,"%s\0",TACTDIR);  2015.1.4
        /* strcpy(scrf,"/usr/local/etc/tact/"); */
        strcat(scrf,"mebhlp.dat");
        /* printf("scrf = %s\n",scrf); */
        fpd = fopen_h(scrf,"r");
      }
    }
    while((c = fgetc(fpd)) != EOF) putchar(c);
    fclose(fpd);
    return(0);
}
