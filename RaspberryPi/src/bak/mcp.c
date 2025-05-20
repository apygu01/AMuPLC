
// ----------------------
//  AMuPLC (mcp.c)
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

#include "mcp.h"
MMKARG *mk;
FILE  *fpi,*fpo;
int  sn;
char *xabc = "0123456789abcdef";
int  sf; /* Source option flag */
char sname[MAXSN][80];
char sDir[PATHLEN]; /* source Directory */
char tDir[PATHLEN]; /* terget Directory */
char tfile[STLN];   /* Terget_file */
int  help;

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
    int  cline;
    char *prtsym;
    char wkc[4];
    char dtopt[STLN];   /* debug or trace option */
    char mfile[STLN];
    char bfile[STLN];
    char pfile[STLN];   /* Add top file */
    char sfile[STLN];   /* Source_file */
    char wfile[STLN];   /* Terget_file */
    char srcfn[STLN];
    char objfn[STLN];
    int  argfn[MAXA];
    int  track[MXMB];
    char partf[MXMB][36];
    char partc;
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
    char aopt[70];
    int  ak;

    mk = &marg;
    sn = 0;
    mk->debug = 0;
    help = 0;
    mk->allf = 0;
    if(argc == 1) {
        help_main();
        return(0);
    }
    sf = 0;
    sDir[0] = '\0';
    tDir[0] = '\0';
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
        if(!strcmp(dtopt,"-H")) help = 1;
        if(!strcmp(dtopt,"-M")) sf = 1;
        if(!strcmp(dtopt,"-A")) mk->allf = 1;
        if(!strcmp(dtopt,"-D")) mk->debug  = 1;
        if(dtopt[0] == '-') argfn[k] = 0;
    }
    if(help) {
	help_main();
	return(0);
    }
    ak = 0;
    for(i = 0; i < argc; i++) {
        if(argfn[i]) {
            ak++;
            if(ak == 2) strcpy(sfile,argv[i]);
            if(ak > 2) strcpy(tfile,argv[i]);
        }
    }
    if(sfile[0] == '/' || sfile[0] == '~') mksDir_mcp(sfile);
    if(tfile[0] == '/' || tfile[0] == '~') mktDir_mcp();
    /* printf("ak = %d sfile = %s tfile = %s\n",ak,sfile,tfile); */
    if(ak != 3) {
        printf("mcp illegal, usage: mcp Source_file Treget_file [option]\n");
        return(0);
    }
    if(!strcmp(sfile,tfile)){
        printf("mcp illegal, same Source_file Treget_file\n");
        return(0);
    }
    /* prtsym = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; */
    prtsym = "0123456789abcdefghijklmnopqrstuvwxyz";
    xmf = sfile;
    while(*xmf) {
        if(*xmf == ':') slsf = 1;
        xmf++;
    }
    yrtn = mkmultif_h(sfile,mfile,".meg",-1,&xrtn,&yfile,&xfile);
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
        pfile[k] = *xmf;
        xmf++;
        k++;
    }
    pfile[k] = '\0';
    filemake_h(0,pfile,".meg",mfile,&yflp,&xflp);
    slsf = 3;
    if(yrtn == 99) slsf = 1;
    if(xrtn == 36) slsf = 2;
    xmf = sfile;
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
    cline = selfil_mcp(mfile,track,partf,port,channel,member);
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
                          mcpfchk1_mcp(bfile,partc);
                          break;
                case  2 : if(xflp == xfile)
                          mcpfchk1_mcp(bfile,partc);
                          break;
                case  3 : if(yflp == yfile && xflp == xfile)
                          mcpfchk1_mcp(bfile,partc);
                          break;
                case  4 : if(yflp <= yfile && xflp <= xfile)
                          mcpfchk1_mcp(bfile,partc);
                          /* mcpfchk2_mcp(bfile,partc); */
                          break;
                default : mcpfchk1_mcp(bfile,partc);
                          /* mcpfchk2_mcp(bfile,partc); */
                          break;
            }
        }
    }
    /* ///////Core///////////// */
    strcpy(wfile,sfile);
    strcpy(sfile,sDir);
    strcat(sfile,wfile);
    strcpy(wfile,tfile);
    strcpy(tfile,tDir);
    strcat(tfile,wfile);
    if(sf) printf("mcp(1) Source = %s Target = %s\n",sfile,tfile);
    /*
    fmcp_mcp(sfile,".meg"); // director 
    fmcp_mcp(sfile,".dat");
    fmcp_mcp(sfile,".meb");
    fmcp_mcp(sfile,".html");
    fmcp_mcp(sfile,".mex");
    fmcp_mcp(sfile,".mid");
    fmcp_mcp(sfile,"rdf.dat");
    fmcp_mcp(sfile,"rdf.html");
    fmcp_mcp(sfile,"cpj.dat");
    fmcp_mcp(sfile,"cpj.html");
    fmcp_mcp(sfile,"cpi.dat");
    fmcp_mcp(sfile,"cpi.html");
    fmcp_mcp(sfile,"nb.txt");
    fmcp_mcp(sfile,"nb.html");
    */
    //  2025.4.21
    fmcp_mcp(sfile,".meg"); // director
    fmcp_mcp(sfile,"rdf.dat");
    fmcp_mcp(sfile,"cpj.dat");
    fmcp_mcp(sfile,"cpi.dat");
    fmcp_mcp(sfile,"nb.txt");
    if(mk->allf) {  // 2025.4.20
        fmcp_mcp(sfile,".dat");
        fmcp_mcp(sfile,".meb");
        fmcp_mcp(sfile,".html");
        fmcp_mcp(sfile,".mex");
        fmcp_mcp(sfile,".mid");
        fmcp_mcp(sfile,"rdf.html");
        fmcp_mcp(sfile,"cpj.html");
        fmcp_mcp(sfile,"cpi.html");
        fmcp_mcp(sfile,"nb.html");
    }
    /* fmcp_mcp(sfile,"_000.meg"); Conductor */
    for(j = 0; j < sn; j++) {
        anscpy(sname[j],".meg");
        /*
        anscpy(sname[j],".meb");
        anscpy(sname[j],".mec");
        anscpy(sname[j],".dat");
        anscpy(sname[j],".mse");
        // anscpy(sname[j],".mai");
        anscpy(sname[j],".html");
        */
        if(mk->allf) {  // 2025.4.21
            anscpy(sname[j],".meb");
            anscpy(sname[j],".mec");
            anscpy(sname[j],".dat");
            anscpy(sname[j],".mse");
            // anscpy(sname[j],".mai");
            anscpy(sname[j],".html");
        }
    }
    /* //////////////////////// */
    return(0);
}

int mcpfchk1_mcp(char mfile[STLN],char partc)
//char mfile[STLN];
//char partc;
{
    int  all;

    all = mk->allf;
    if(partc == '@' && all) mcpmain(mfile);
    if(partc == '*' && all) mcpmain(mfile);
    if(partc == '+' && all) mcpmain(mfile);
    if(partc == 'c' && all) mcpmain(mfile);
    if(partc == 'l' && all) mcpmain(mfile);
    if(partc == 'm' && all) mcpmain(mfile);
    if(partc == 'h' && all) mcpmain(mfile);
    if(partc == 'r' && all) mcpmain(mfile);
    if(partc == 'd' && all) mcpmain(mfile);
    if(partc == 's' && all) mcpmain(mfile);
    if(partc == 'g' && all) mcpmain(mfile);
    if(partc == 'a' && all) mcpmain(mfile);
    if(partc == 'b' && all) mcpmain(mfile);
    if(partc == 'x' && all) mcpmain(mfile);
    if(partc == 'C') mcpmain(mfile);
    if(partc == 'L') mcpmain(mfile);
    if(partc == 'M') mcpmain(mfile);
    if(partc == 'H') mcpmain(mfile);
    if(partc == 'R') mcpmain(mfile);
    if(partc == 'D') mcpmain(mfile);
    if(partc == 'S') mcpmain(mfile);
    if(partc == 'G') mcpmain(mfile);
    if(partc == 'A') mcpmain(mfile);
    if(partc == 'B') mcpmain(mfile);
    if(partc == 'X') mcpmain(mfile);
    return(0);
}

int mcpfchk2_mcp(char mfile[STLN],char partc)
//char mfile[STLN];
//char partc;
{
    int  all;

    all = mk->allf;
    if(partc == '@' && all) mcpmain(mfile);
    if(partc == '*' && all) mcpmain(mfile);
    if(partc == '+' && all) mcpmain(mfile);
    if(partc == 'c' && all) mcpmain(mfile);
    if(partc == 'l' && all) mcpmain(mfile);
    if(partc == 'm' && all) mcpmain(mfile);
    if(partc == 'h' && all) mcpmain(mfile);
    if(partc == 'r' && all) mcpmain(mfile);
    if(partc == 'd' && all) mcpmain(mfile);
    if(partc == 's' && all) mcpmain(mfile);
    if(partc == 'g' && all) mcpmain(mfile);
    if(partc == 'a' && all) mcpmain(mfile);
    if(partc == 'b' && all) mcpmain(mfile);
    if(partc == 'x' && all) mcpmain(mfile);
    if(partc == 'C' && all) mcpmain(mfile);
    if(partc == 'L' && all) mcpmain(mfile);
    if(partc == 'M' && all) mcpmain(mfile);
    if(partc == 'H' && all) mcpmain(mfile);
    if(partc == 'R' && all) mcpmain(mfile);
    if(partc == 'D' && all) mcpmain(mfile);
    if(partc == 'S' && all) mcpmain(mfile);
    if(partc == 'G' && all) mcpmain(mfile);
    if(partc == 'A' && all) mcpmain(mfile);
    if(partc == 'B' && all) mcpmain(mfile);
    if(partc == 'X' && all) mcpmain(mfile);
    /* To main() */
    if(partc == 'C') if(!all) mcpmain(mfile);
    if(partc == 'L') if(!all) mcpmain(mfile);
    if(partc == 'M') if(!all) mcpmain(mfile);
    if(partc == 'H') if(!all) mcpmain(mfile);
    if(partc == 'R') if(!all) mcpmain(mfile);
    if(partc == 'D') if(!all) mcpmain(mfile);
    if(partc == 'S') if(!all) mcpmain(mfile);
    if(partc == 'G') if(!all) mcpmain(mfile);
    if(partc == 'A') if(!all) mcpmain(mfile);
    if(partc == 'B') if(!all) mcpmain(mfile);
    if(partc == 'X') if(!all) mcpmain(mfile);
    return(0);
}

int mcpmain(char inf[STLN])
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

int selfil_mcp(char *mfile,int track[MXMB],char partf[MXMB][36],
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

int filemake_h(int n,char inf[STLN],char fext[STLN],char dfile[STLN],
    int *yflp,int *xflp)
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
int mkmultif_h(char *infn,char *outf,char *ftyp,int fnmb,
   int *prtn,int *yfile,int *xfile)
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

int fmcp_mcp(char sfile[STLN],char *sfix)
//char sfile[STLN];
//char *sfix;
{
    char srcfn[STLN];
    char objfn[STLN];

    strcpy(srcfn,sfile);
    strcat(srcfn,sfix);
    strcpy(objfn,tfile);
    strcat(objfn,sfix);
    if(mk->debug) printf("fmcp_() srcfn = %s objfn = %s\n",srcfn,objfn);
    if((fpi = fopen(srcfn,"r")) != NULL) {
        if(sf) printf("mcp(1) %s -> %s\n",srcfn,objfn);
        fpo = fopen_h(objfn,"w");
        while(fgets(mk->card, 82, fpi)) {
            fputs(mk->card,fpo);
            if(mk->card[0] == '$' && mk->card[2] == '$') break; /* EOF */
        }
        fclose(fpi);
        fclose(fpo);
    }
    return(0);
}

int mksDir_mcp(char *sfile)
//char *sfile;
{
    int  i,j;
    int  size;

    size = strlen(sfile);
    strcpy(sDir,sfile);
    /*
    if(sfile[0] == '~') {
        for(i = size; i > 0; i--) {
            if(sfile[i] == '/') break;
        }
    }
    */
    if(sfile[0] == '/') {
        for(i = size; i > 0; i--) {
            if(sfile[i] == '/') break;
        }
        sDir[i] = '\0';
        i++;
        for(j = 0; i < size; i++,j++) {
            sfile[j] = sDir[i];
        }
        sfile[j] = '\0';
    }
    strcat(sDir,"/");
    /* printf("mksDir_mcp() sDir=%s sfile = %s\n",sDir,sfile); */
    return(0);
}

int mktDir_mcp()
{
    int  i,j;
    int  size;
    
    size = strlen(tfile);
    strcpy(tDir,tfile);
    /*
    if(tfile[0] == '~') {
        for(i = size; i > 0; i--) {
            if(tfile[i] == '/') break;
        }
    }
    */
    if(tfile[0] == '/') {
        for(i = size; i > 0; i--) {
            if(tfile[i] == '/') break;
        }
        tDir[i] = '\0';
        i++;
        for(j = 0; i < size; i++,j++) {
            tfile[j] = tDir[i];
        }
        tfile[j] = '\0';
    }
    strcat(tDir,"/");
    /* printf("mktDir_mcp() tDir=%s tfile = %s\n",tDir,tfile); */
    return(0);
}

int anscpy(char *sfile,char *sfix)
//char *sfile;
//char *sfix;
{
    int i;
    char wk[STLN];
    char wfile[STLN];   /* Terget_file */
    char srcfn[STLN]; 
    char objfn[STLN];

    if(mk->debug) printf("Debug! anscpy() sfile=%s sfix=%s\n",sfile,sfix);
    i = 0;
    while(1) {
        wk[i] = sfile[i];
        if(wk[i] == '.') break;
        i++;
        if(i > STLN) break;
    }
    wk[i] = '\0';
    strcat(wk,sfix);
    /* printf("Debug! anscpy() wk=%s\n",wk); */
    strcpy(srcfn,wk);
    sufix2(objfn,wk);
    strcpy(wfile,srcfn);
    strcpy(srcfn,sDir);
    strcat(srcfn,wfile);
    if((fpi = fopen(srcfn,"r")) != NULL) {
        if(sf) printf("mcp(2) %s -> %s\n",srcfn,objfn);
        fpo = fopen_h(objfn,"w");
        while(fgets(mk->card, 82, fpi)) {
            fputs(mk->card,fpo);
            if(mk->card[0] == '$' && mk->card[2] == '$') break; /* EOF */
        }
        fclose(fpi);
        fclose(fpo);
    }
    return(0);
}

int sufix2(char pmf[STLN],char *sfixf)
//char pmf[STLN];
//char *sfixf;
{
    int  i,j;
    char wk1[STLN];

    if(mk->debug) printf("sufix2() tfile = %s sfixf = %s\n",tfile,sfixf);
    i = 0;
    while(1) {
        if(sfixf[i] == '_') break;
        if(sfixf[i] == '\0') break;
        i++;
        if(i > STLN) break;
    }
    j = 0;
    while(1) {
        wk1[j] = sfixf[i];
        if(wk1[j] == '\0') break;
        j++;
        i++;
        if(i > STLN) break;
    }
    i = 0;
    while(1) {
        pmf[i] = tfile[i];
        if(pmf[i] == '_') break;
        if(pmf[i] == '\0') break;
        i++;
        if(i > STLN) break;
    }
    pmf[i] = '\0';
    strcat(pmf,wk1);
    /* printf("pmf = %s wk1 = %s\n",pmf,wk1); */
    return(0);
}

int help_main()
{
    int  c;
    FILE *fpd;
    char scrf[100];

    if(help) {
        if((fpd = fopen("mcp.dat","r")) == NULL) {
            sprintf(scrf,"%s",TACTDIR);
            //sprintf(scrf,"%s\0",TACTDIR);
            strcat(scrf,"mcp.dat");
            fpd = fopen_h(scrf,"r");
        }
        while((c = fgetc(fpd)) != EOF) putchar(c);
        fclose(fpd);
        return(0);
    }
    printf( "\n Music Text files Copy V %s-%s\n",VERSION,OS);
    printf(" mcp Source_file Target_file [Options]\n");
    printf("     Options : -a(Copy all files)\n");
    printf("               -m(Message)\n");
    printf("               -h\n");
    printf(" usage: e.g.\n");
    printf("      mcp -h\n");
    printf("      mcp sfn tfn\n");
    printf("      mcp sfn tfn -a -m\n");
    printf("\n");
    return(0);
}

