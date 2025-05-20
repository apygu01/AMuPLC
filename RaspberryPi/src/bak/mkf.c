
// ----------------------
//  AMuPLC (mkf.c)
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

#include "mkf.h"
MMKARG *mk;
FILE  *fpi,*fpo;
char sname[MAXSN][80];
int  sn;
char *xabc = "0123456789abcdef";
int  sf; /* Source option flag */

int main(int argc,char *argv[])
//int  argc;
//char *argv[];
{
    MMKARG marg;
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
    char bfile[STLN];
    char tfile[STLN];   /* Add top file */
    int  argfn[MAXA];
    int  track[MXMB];
    char partf[MXMB][36];
    char partc;
    int  port[MXMB];
    int  channel[MXMB];
    int  member[MXMB];
    char objfn[MXMB];
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
    int  smf;  /* -smf option */
    char aopt[70];

    mk = &marg;
    sn = 0;
    mk->debug = 0;
    help = 0;
    smf = 0;
    mk->allf = 0;
    mk->epf = 0;  /* add full pass */
    if(argc == 1) {
        help_main();
        return(0);
    }
    sf = 0;
    aopt[0] = '\0';
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
        if(mk->epf) {
            pa = argv[i];
            j = 0;
            while(*pa) {
                mk->ep[j] = *pa;
                j++;
                if(j > PATHLEN) break;
                pa++;
            }
            mk->ep[j] = '\0';
            dtopt[0] = '-'; /* Not file name */
            /* printf("main() mk->ep=%s\n",mk->ep); */
        }
        if(!strcmp(dtopt,"-H")) help = 1;
        if(!strcmp(dtopt,"-S")) sf = 1;
        if(!strcmp(dtopt,"-D")) mk->debug  = 1;
        if(!strcmp(dtopt,"-EP")) mk->epf = 1; 
        if(!strcmp(dtopt,"-SMF0")) strcat(aopt," -smf0");
        if(!strcmp(dtopt,"-SMF1")) strcat(aopt," -smf1");
        if(!strcmp(dtopt,"-SMF2")) strcat(aopt," -smf2");
        if(!strcmp(dtopt,"-NEW")) strcat(aopt," -new");
        if(!strcmp(dtopt,"-OLD")) strcat(aopt," -old");
        if(!strcmp(dtopt,"-FRS")) strcat(aopt," -frs");
        if(!strcmp(dtopt,"-PHR")) strcat(aopt," -phr");
        if(!strcmp(dtopt,"-OT")) strcat(aopt," -ot");
        if(dtopt[0] == '-') argfn[k] = 0;
    }
    if(help) {
	help_main();
	return(0);
    }
    fpo = fopen_h("makefile","wb");
    /* prtsym = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; */
    prtsym = "0123456789abcdefghijklmnopqrstuvwxyz";
    for(i = 1; i < argc; i++) {
        if(argfn[i]) {
            xmf = argv[i];
            while(*xmf) {
                if(*xmf == ':') slsf = 1;
                xmf++;
            }
            yrtn = mkmultif_h(argv[i],mfile,".meg",-1,&xrtn,&yfile,&xfile);
            xmf = mfile;
            k = 0;
            while(*xmf) {
                if(*xmf == '(') break;
                if(*xmf == '/') break;
                if(*xmf == '_') break;
                if(*xmf == ';' || *xmf == ',') {
                    mk->errcnt++;
                    printf("Error3! filename\n");
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
            if(mk->debug) {
                printf("mfile = %s yrtn=%d xrtn=%d yfile=%d xfile=%d\n",
                mfile,yrtn,xrtn,yfile,xfile);
                printf("yflp=%d xflp=%d slsf=%d\n",
                yflp,xflp,slsf);
            }
            cline = selfil_mec(mfile,track,partf,port,channel,member);
            if(mk->debug) printf("cline = %d mfile = %s\n",cline,mfile);
            l = 0;
            while(mfile[l++] != '_');
            for(j = 0; j < cline; j++) {
                sprintf(wkc,"%02x",track[j]);  /* 97.11 */
                mfile[l+1] = wkc[0];
                mfile[l+2] = wkc[1];
                /*
                for(k = 0; k < 36; k++) {
                    mfile[l] = *(prtsym+k);
                }
                */
            }
            for(j = 0; j < cline; j++) {
                mk->port = port[j];
                mk->chan = channel[j];
                /* mk->chan = channel[j] - 1;   modify to 0-15 */
                mk->memb = member[j];
                sprintf(wkc,"%02x",track[j]);  /* %02d v_1.14 */
                xflp = track[j];
                mfile[l+1] = wkc[0];
                mfile[l+2] = wkc[1];
                for(k = 0; k < 36; k++) {
                    yflp = k;
                    mfile[l] = *(prtsym+k);
                    mk->trak = k;
                    partc = partf[j][k];
                    strcpy(bfile,mfile);
                    if(mk->debug) {
                        printf("bfile = %s l = %d partf[%d][%d] = %c\n",
                        bfile,l,j,k,partf[j][k]);
                    }
                    switch (slsf) {
                        case  1 : if(yflp == yfile)
                                  mecfchk1_mec(bfile,partc);
                                  break;
                        case  2 : if(xflp == xfile)
                                  mecfchk1_mec(bfile,partc);
                                  break;
                        case  3 : if(yflp == yfile && xflp == xfile)
                                  mecfchk1_mec(bfile,partc);
                                  break;
                        case  4 : if(yflp <= yfile && xflp <= xfile)
                                  mecfchk2_mec(bfile,partc);
                                  break;
                        default : mecfchk2_mec(bfile,partc);
                                  break;
                    }
                }
            }
            fprintf(fpo,"# %s makefile\n",argv[i]);
            if(sf) printf("# %s makefile\n",argv[i]);
            fprintf(fpo,"SNAME = %s\n",argv[i]);
            if(sf) printf("SNAME = %s\n",argv[i]);
            if(!smf) {
	        fprintf(fpo,"PROGRAM = %s.mex\n",argv[i]);
	        if(sf) printf("PROGRAM = %s.mex\n",argv[i]);
            }
            else {
	        fprintf(fpo,"PROGRAM = %s.mid\n",argv[i]);
	        if(sf) printf("PROGRAM = %s.mid\n",argv[i]);
	    }
            fprintf(fpo,"SOURCES = ");
            if(sf) printf("SOURCES = ");

            fprintf(fpo,"%s \\\n",sname[0]);
            if(sf) printf("%s \\\n",sname[0]);
            for(j = 1; j < sn-1; j++) {
	        fprintf(fpo,"          %s \\\n",sname[j]);
	        if(sf) printf("          %s \\\n",sname[j]);
	    }
            fprintf(fpo,"          %s\n",sname[sn-1]);
            if(sf) printf("          %s\n",sname[sn-1]);

            fprintf(fpo,"OBJECTS = ");
            if(sf) printf("OBJECTS = ");
            adsufix(objfn,sname[0]);
            fprintf(fpo,"%s \\\n",objfn);
            if(sf) printf("%s \\\n",objfn);
            for(j = 1; j < sn-1; j++) {
                adsufix(objfn,sname[j]);
                fprintf(fpo,"          %s \\\n",objfn);
                if(sf) printf("          %s \\\n",objfn);
            }
            adsufix(objfn,sname[sn-1]);
            fprintf(fpo,"          %s\n",objfn);
            if(sf) printf("          %s\n",objfn);
            if(!smf) {
                if(mk->epf) {
	            fprintf(fpo,"MEC = %smec -m1\n",mk->ep);
	            if(sf) printf("MEC = %smec -m1\n",mk->ep);
                }
                else {
	            fprintf(fpo,"MEC = mec -m1\n");
	            if(sf) printf("MEC = mec -m1\n");
                }
	    }
            else {
                if(mk->epf) {
	            fprintf(fpo,"MEC = %smec -smf -m1\n",mk->ep);
	            if(sf) printf("MEC = %smec -smf -m1\n",mk->ep);
                }
                else {
	            fprintf(fpo,"MEC = mec -smf -m1\n");
	            if(sf) printf("MEC = mec -smf -m1\n");
                }
	    }
            if(mk->epf) {
	        fprintf(fpo,"MLK = %smlk -m1%s\n",mk->ep,aopt);
	        if(sf) printf("MLK = %smlk -m1%s\n",mk->ep,aopt);
            }
            else {
	        fprintf(fpo,"MLK = mlk -m1%s\n",aopt);
	        if(sf) printf("MLK = mlk -m1%s\n",aopt);
            }

            fprintf(fpo,".SILENT:\n");
            if(sf) printf(".SILENT:\n");

            fprintf(fpo,".SUFFIXES:\n");
            if(sf) printf(".SUFFIXES:\n");

            fprintf(fpo,".SUFFIXES: .meg .mec\n"); 
            if(sf) printf(".SUFFIXES: .meg .mec\n"); 

            fprintf(fpo,".meg.mec:\n"); 
            if(sf) printf(".meg.mec:\n"); 

            fprintf(fpo,"\t $(MEC) $<\n"); 
            if(sf) printf("\t $(MEC) $<\n"); 

            fprintf(fpo,"all: $(PROGRAM)\n");
            if(sf) printf("all: $(PROGRAM)\n");

            fprintf(fpo,"$(PROGRAM): $(OBJECTS)\n");
            if(sf) printf("$(PROGRAM): $(OBJECTS)\n");

            /*
	    fprintf(fpo,"\t    mlk $(SNAME) -m0%s\n",aopt);
	    if(sf) printf("\t    mlk $(SNAME) -m0%s\n",aopt);
	    fprintf(fpo,"\t    mlk $(SNAME)%s\n",aopt);
	    if(sf) printf("\t    mlk $(SNAME)%s\n",aopt);
            */
            if(mk->epf) {
	        fprintf(fpo,"\t    %smlk $(SNAME)%s\n",mk->ep,aopt);
	        if(sf) printf("\t    %smlk $(SNAME)%s\n",mk->ep,aopt);
            }
            else {
	        fprintf(fpo,"\t    mlk $(SNAME)%s\n",aopt);
	        if(sf) printf("\t    mlk $(SNAME)%s\n",aopt);
            }

            for(j = 1; j < sn-1; j++) {
                adsufix(objfn,sname[j]);
                fprintf(fpo,"%s:%s_000.meg\n",objfn,tfile);
                if(sf) printf("%s:%s_000.meg\n",objfn,tfile);
            }
            adsufix(objfn,sname[sn-1]);
            fprintf(fpo,"%s:%s_000.meg\n",objfn,tfile);
            if(sf) printf("%s:%s_000.meg\n",objfn,tfile);
	}
    }
    fclose(fpo);
    return(0);
}

int mecfchk1_mec(char mfile[STLN],char partc)
//char mfile[STLN];
//char partc;
{
    if(partc == '@') mkfmain(mfile);
    if(partc == '*') mkfmain(mfile);
    if(partc == '+') mkfmain(mfile);
    if(partc == 'c') mkfmain(mfile);
    if(partc == 'l') mkfmain(mfile);
    if(partc == 'm') mkfmain(mfile);
    if(partc == 'h') mkfmain(mfile);
    if(partc == 'r') mkfmain(mfile);
    if(partc == 'd') mkfmain(mfile);
    if(partc == 's') mkfmain(mfile);
    if(partc == 'g') mkfmain(mfile);
    if(partc == 'a') mkfmain(mfile);
    if(partc == 'b') mkfmain(mfile);
    if(partc == 'x') mkfmain(mfile);
    if(partc == 'C') mkfmain(mfile);
    if(partc == 'L') mkfmain(mfile);
    if(partc == 'M') mkfmain(mfile);
    if(partc == 'H') mkfmain(mfile);
    if(partc == 'R') mkfmain(mfile);
    if(partc == 'D') mkfmain(mfile);
    if(partc == 'S') mkfmain(mfile);
    if(partc == 'G') mkfmain(mfile);
    if(partc == 'A') mkfmain(mfile);
    if(partc == 'B') mkfmain(mfile);
    if(partc == 'X') mkfmain(mfile);
    return(0);
}

int mecfchk2_mec(char mfile[STLN],char partc)
//char mfile[STLN];
//char partc;
{
    int  all;

    all = mk->allf;
    if(partc == '@' && all) mkfmain(mfile);
    if(partc == '*' && all) mkfmain(mfile);
    if(partc == '+' && all) mkfmain(mfile);
    if(partc == 'c' && all) mkfmain(mfile);
    if(partc == 'l' && all) mkfmain(mfile);
    if(partc == 'm' && all) mkfmain(mfile);
    if(partc == 'h' && all) mkfmain(mfile);
    if(partc == 'r' && all) mkfmain(mfile);
    if(partc == 'd' && all) mkfmain(mfile);
    if(partc == 's' && all) mkfmain(mfile);
    if(partc == 'g' && all) mkfmain(mfile);
    if(partc == 'a' && all) mkfmain(mfile);
    if(partc == 'b' && all) mkfmain(mfile);
    if(partc == 'x' && all) mkfmain(mfile);
    if(partc == 'C' && all) mkfmain(mfile);
    if(partc == 'L' && all) mkfmain(mfile);
    if(partc == 'M' && all) mkfmain(mfile);
    if(partc == 'H' && all) mkfmain(mfile);
    if(partc == 'R' && all) mkfmain(mfile);
    if(partc == 'D' && all) mkfmain(mfile);
    if(partc == 'S' && all) mkfmain(mfile);
    if(partc == 'G' && all) mkfmain(mfile);
    if(partc == 'A' && all) mkfmain(mfile);
    if(partc == 'B' && all) mkfmain(mfile);
    if(partc == 'X' && all) mkfmain(mfile);
    /* To main() */
    if(partc == 'C') if(!all) mkfmain(mfile);
    if(partc == 'L') if(!all) mkfmain(mfile);
    if(partc == 'M') if(!all) mkfmain(mfile);
    if(partc == 'H') if(!all) mkfmain(mfile);
    if(partc == 'R') if(!all) mkfmain(mfile);
    if(partc == 'D') if(!all) mkfmain(mfile);
    if(partc == 'S') if(!all) mkfmain(mfile);
    if(partc == 'G') if(!all) mkfmain(mfile);
    if(partc == 'A') if(!all) mkfmain(mfile);
    if(partc == 'B') if(!all) mkfmain(mfile);
    if(partc == 'X') if(!all) mkfmain(mfile);
    return(0);
}

int mkfmain(char inf[STLN])
//char inf[STLN];
{
    strcpy(sname[sn],inf);
    if(mk->debug) printf("sname = %s sn = %d\n",sname[sn],sn);
    sn++;
    if(sn >= MAXSN) {
        printf("Error! file over than %d\n",MAXSN);
        exit(0);
    }
    return(0);
}

int selfil_mec(char *mfile,int track[MXMB],char partf[MXMB][36],
    int port[MXMB],int channel[MXMB],int member[MXMB])
//char *mfile;
//int  track[MXMB];
//char partf[MXMB][36];
//int  port[MXMB];
//int  channel[MXMB];
//int  member[MXMB];
{
    int  i;
    int  j;
    int  k;
    char c;
    int  cline;
    unsigned char c1,c2;
    int  cx1,cx2;
    char chw[8];
    FILE *fpi;
    char  gfile[STLN];

    cline = 0;
    for(i = 0; i < MXMB; i++) {
        port[i] = 0;
        track[i] = 0;
        channel[i] = 0;
        member[i] = 0;
        for( j = 0; j < 36; j++) partf[i][j] = ' ';
    }
    strcpy(gfile,mfile);
    i = 0;
    while(gfile[i] != '_') i++;
    gfile[i] = '\0';   
    strcat(gfile,".meg");
    /* printf("gfile = %s\n",gfile); */
    if((fpi = fopen(gfile,"r")) == NULL) fpi = fopen_h(mfile,"r");
    /* fpi = fopen_h(mfile,"rb"); */
    while(fgets(mk->card, 82, fpi)) {
        c = mk->card[0];
        if(c == 'C' && (mk->card[4] != ' ')) {
            c1 = mk->card[3];
            c2 = mk->card[4];
            for(i = 0; i < 16; i++) {
                if(c1 == xabc[i]) cx1 = i;
                if(c2 == xabc[i]) cx2 = i;
            }
            track[cline] = cx1 * 16 + cx2;
            /* printf("track[%d] = %d\n",cline,track[cline]); */
            if(track[cline] >= MXMB) {
                printf("Error! track over than %d\n",MXMB);
                fclose(fpi);
                return(1);
            }
            i = 8;
            for(k = 0; k < 36; k++) {
                partf[cline][k] = mk->card[i];
                i++;
            }
            while(mk->card[i] == ' ') i++;
            for(k = 0; k < 16; k++) { /* channel */
                if(mk->card[i] == 's') channel[cline] = k;
                if(mk->card[i] == 'S') channel[cline] = k;
                i++;
            }
            while(mk->card[i] == ' ') i++;
            j = 0;
            while(isdigit(mk->card[i++])) chw[j++] = mk->card[i-1];
            chw[j] = '\0';
            sscanf(chw,"%d",&port[cline]);
            while(mk->card[i] == ' ') i++;
            j = 0;
            while(isdigit(mk->card[i++])) chw[j++] = mk->card[i-1];
            chw[j] = '\0';
            sscanf(chw,"%d",&member[cline]);
            cline++;
            if(cline > 255) {
                mk->errcnt++;
                printf("Error3! cline over 255\n");
                return(1);
            }
        }
        if(c == '_') break;
        if(c == 'f') break;
        if(c == ESC) break;
    }
    fclose(fpi);
    return(cline);
}

int filemake_h(int n,char *inf,char *fext,
    char *dfile,int *yflp,int *xflp)
//int  n;
//char inf[STLN];
//char fext[STLN];
//char dfile[STLN];
//int  *yflp;
//int  *xflp;
{
    char *pc;
    char wfnum[8];
    char wk[8];
    // unsigned char wfnum[8];
    // unsigned char wk[8];
    int  i;
    int  trk;
    int  memb;

    trk = n/1000;  /* n/100 v_1.14 */
    memb = n - trk * 1000;  /* n - trk * 100 v_1.14 */
    *yflp = trk;
    *xflp = memb;
    pc = "0123456789abcdefghijklmnopqrstuvwxyz";
    for(i = 0; i < trk ; i++) pc++;
    wfnum[0] = *pc;
    wfnum[1] = '\0';
    sprintf(wk,"%02x",memb);  /* %02d v_1.14 */
    strcat(wfnum,wk);
    dfile[0] = '\0';
    strcat(dfile,inf);
    strcat(dfile,"_");
    strcat(dfile,wfnum);
    strcat(dfile,fext);
    return(0);
}

int mkyxfnum_h(char fname[STLN],int *yfile,int *xfile,char *ps)
//char fname[STLN];
//int  *yfile;
//int  *xfile;
//char *ps;
{
    char *pf;
    char w[4];
    int  i;

    pf = fname;
    while(*(pf++)) if(*pf == '_') break;
    pf++;
    for(i = 0; i < 36; i++,ps++) if(*pf == *ps) break;
    *yfile = i;
    /* getdigit_h(*pf,yfile); */
    pf++;
    w[0] = *pf;
    pf++;
    w[1] = *pf;
    w[2] = '\0';
    sscanf(w,"%x",xfile);   /* %d v_1.14 */
    return(0);
}

int adsufix(char pmf[80],char *infn)
//char pmf[80];
//char *infn;
{
    int  i;
    char *pw;

    i = 0;
    pw = infn;
    while(*(pw)) {
        pmf[i] = *pw;
        i++;
        pw++;
        if(*pw == '.') break;
    }
    pmf[i] = '\0';
    strcat(pmf,".mec");
    return(0);
}

int strcat_h(char *tos,char *from)
//char *tos,*from;
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

FILE *fopen_h(char *file, char *mode)
//char *file;
//char *mode;
{
    FILE *fp;

    if((fp = fopen(file, mode)) == NULL) {
        mk->errcnt++;
        printf("Error4! can't open file %s mode %s\n",file, mode);
        exit(1);
    }
    return(fp);
}

/*
    make multi file
        ex0_x   ->  ex0_x00.xxx return(99)  prtn = 0
        ex0_xx  ->  ex0_0xx.xxx return(0)   prtn = 36
        ex0_xxx ->  ex0_xxx.xxx return(0)   prtn = 0
        ex0/Y;X ->  ex0_002.xxx retnrn(Y)   prtn = X (fnmb = 0,fnmb = -1)
        ex0/Y;X ->  ex0_002.xxx retnrn(Y-1) prtn = X (fnmb = 1)
    modify 1989.9.26 v_1.14  _axx. : hex suport
*/
int mkmultif_h(char *infn,char *outf,char *ftyp,
    int fnmb,int *prtn,int *yfile,int *xfile)
//char *infn;
//char *outf;
//char *ftyp;
//int  fnmb;
//int  *prtn;
//int  *yfile;
//int  *xfile;
{
    int  i,j,n;
    char w[25];
    int  mflg;
    int  xflg;
    int  vflg;
    char *pmf;
    char *xmf;
    char xw[10];
    char *fnum;
    int  fleng;
    int  pleng;
    int  xleng;
    char *ps;

    ps = "0123456789abcdefghijklmnopqrstuvwxyz";
    xw[0] = '\0';
    xmf = infn;
    *prtn = 0;
    vflg = 0;
    xflg = 0;
    while(*(xmf++)) {
        if(*xmf == '(') break;
        if(*xmf == '/') vflg = 1;   /* 1st dimensional vector _xx or _x */
        if(*xmf == '_') vflg = 2;   /* 1st dimensional vector _xx or _x */
        if(*xmf == ';') xflg = 1;   /* 2nd dimensional vector /Y;X */
        if(*xmf == ',') {
            xflg = 1;   /* 2nd dimensional vector /Y;X */
            *xmf = ';';
        }
    }
    if(vflg == 1 && xflg == 0) {
        xmf = infn;
        while(*(xmf++)) if(*xmf == '/') *xmf = '_';
    }
    if(vflg) {
        /* _xx */
        mflg = 0;
        fleng = 0;
        pmf = infn;
        while(*(pmf++)) fleng++;
        pleng = 0;
        i = 0;
        pmf = infn;
        while(*pmf) {
            pleng++;
            if(*pmf == '_') {
                mflg = 1;
                break;
            }
            w[i++] = *pmf;
            pmf++;
        }
        w[i] = '\0';
        xleng = fleng - pleng;
        /* printf("w = %s xleng = %d\n",w,xleng); */
        if(xleng == 3) {
            *prtn = 0;
            strcpy(outf,infn);
            strcat_h(outf,ftyp);
            mkyxfnum_h(outf,yfile,xfile,ps);
            return(0);
        }
        if(xleng == 2) {
            *prtn = 36;
            strcpy(outf,w);
            xmf = infn;
            while(*(xmf++)) if(*xmf == '_') break;
            xmf++;
            w[0] = *xmf;
            xmf++;
            w[1] = *xmf;
            w[2] = '\0';
            strcat_h(outf,"_0");
            strcat_h(outf,w);
            strcat_h(outf,ftyp);
            mkyxfnum_h(outf,yfile,xfile,ps);
            return(0);
        }
        if(xleng == 1) {
            *prtn = 0;
            strcpy(outf,infn);
            strcat_h(outf,"00");
            strcat_h(outf,ftyp);
            mkyxfnum_h(outf,yfile,xfile,ps);
            return(99);
        }
    }
    if(xflg) {
        i = 0;
        xmf = infn;
        while(*(xmf++)) if(*xmf == ';') break;
        while(*(xmf++)) xw[i++] = *xmf;
    }
    sscanf(xw,"%x",prtn);   /* %d v_1.14 */
    mflg = 0;
    i = 0;
    pmf = infn;
    fnum = "000";
    while(*pmf) {
        if(*pmf == '/') {
            mflg = 1;
            break;
        }
        w[i++] = *pmf;
        pmf++;
    }
    if(mflg) if(!xflg) {
        printf("Error! file name \"%s\"\n",infn);
        exit(1);
    }
    w[i] = '\0';
    if(mflg) {
        i = 0;
        pmf = infn;
        while(*(pmf++)) if(*pmf == '/') break;
        while(*(pmf++)) {
            if(*pmf == ';') break;
            fnum[i] = *pmf;
            i++;
        }
        fnum[i] = '\0';
        j = 0;
        while(*(pmf++)) {
            if(*pmf == '\0') break;
            xw[j] = *pmf;
            j++;
        }
        /* printf("i = %d  fnum = %s xw = %s\n",i,fnum,xw); */
        fnum[1] = '\0';
        if(i == 1) {
            for(j = 0; j < 36; j++,ps++) if(fnum[0] == *ps) break;
            n = j;
        }
        else sscanf(fnum,"%d",&n);
        *yfile = n;
        sscanf(xw,"%x",xfile);  /* %d v_1.14 */
        strcpy(outf,w);
        strcat_h(outf,"_");
        fnum[1] = '\0';
        strcat_h(outf,fnum);
        sprintf(fnum,"%02x",fnmb+1);  /* %03d v_1.14 */
        strcat_h(outf,fnum);
        strcat_h(outf,ftyp);
        return(n-fnmb);
    }
    strcpy(outf,w);
    if(fnmb == -1) strcat_h(outf,"_000");
    if(fnmb == 0) strcat_h(outf,"_001");
    if(fnmb == 1) strcat_h(outf,"_002");
    strcat_h(outf,ftyp);
    mkyxfnum_h(outf,yfile,xfile,ps);
    return(0);
}

int help_main()
{
    printf("\n Makefile Generator V.%s-%s-%s\n",VERSION,SUPPORT,OS);
    printf(" mkf fn");
    printf("     fn : AMuPLC file name\n");
    printf("\n");
    printf(" check > cat makefile\n");
    printf(" usage : make -f makefile\n");
    printf("\n");
    return(0);
}

