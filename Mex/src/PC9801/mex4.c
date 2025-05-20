/* mex4.c */
#include "mex.h"
extern MEXSRG *ls;
extern MEXVAR *lv;
extern MEXACT *la;
extern MQXFRS *fv;
extern SETUPD *su;           /* setup data */
extern unsigned int sport[16];
extern unsigned int dport[16];
extern char msg[82];

getmexrdf_mex(dfile)
char dfile[STLN];
{
    FILE *fopen(),*frp;
    char mfile[STLN];
    int  i;

    for(i = 0; i < STLN; i++) {
        if(dfile[i] == '.' || dfile[i] == '_') break;
        mfile[i] = dfile[i];
    }
    mfile[i] = '\0';
    /* strcat(mfile,"_000.meg"); */
    strcat(mfile,".meg");
    /* _dprintf("dfile=%s mfile=%s\n",dfile,mfile); */
    if((frp = fopen(mfile,"r")) != NULL) getmexcg_mex(frp);
    else {
        sprintf(msg,"Worning %s not found\n",mfile);
        gprintf_mex(msg);
        return(0);
    }
    fclose(frp);
    return(0);
}

/* get mex config data */
getmexcg_mex(frp)
FILE *frp;
{
    char card[82];
    int  i;
    int  line;

    line = 0;
    sportadr_mex();
    while(fgets(card,82,frp)) {
        line++;
        if(line > 200) break;
        for(i = 0; i < 82; i++)
            if(card[i] == '\r' || card[i] == '\n') card[i] = '\0';
        if(card[0] == '$' && card[50] == '$') break; /* EOF */
        if(card[0] == '&' && card[50] == '$') break; /* EOF */
        if(ls->debug == 2) {
            sprintf(msg,"%s\n",card);
            gprintf_mex(msg);
        }
        if(card[0] == '&') optionset_mex(card);
    }
    return(0);
}

/* print execute mode */
prtmode_mex()
{
    if(ls->act232c) {
        if(ls->e232cp == 999) {
            sprintf(msg,"EMU232C Assign port is all( * )\n");
            gprintf_mex(msg);
        }
        else {
            sprintf(msg,"EMU232C Assign port is ( %d )\n",
               ls->e232cp);
            gprintf_mex(msg);
        }
    }
    /* _dprintf("Monitor %d  ",ls->monitor); */
    if(ls->clktyp == 0) {
        sprintf(msg,"Real_time_clock  ");
        gprintf_mex(msg);
    }
    if(ls->clktyp == 1) {
        sprintf(msg,"Flame_time_clock  ");
        gprintf_mex(msg);
    }
    if(ls->clkmod < 0) {
        sprintf(msg,"Tempo up %d ",-ls->clkmod);
        gprintf_mex(msg);
    }
    if(ls->clkmod > 0) {
        sprintf(msg,"Tempo down %d ",ls->clkmod);
        gprintf_mex(msg);
    }
    if(ls->smpte) {
        sprintf(msg,"Mtc(on %d)  ",ls->mtcport);
        gprintf_mex(msg);
    }
    else {
        sprintf(msg,"Mtc(off)  ");
        gprintf_mex(msg);
    }
    if(ls->smptp) {
        sprintf(msg,"Dtc(on %d)\n",ls->dtcport);
        gprintf_mex(msg);
    }
    else {
        sprintf(msg,"Dtc(off)\n");
        gprintf_mex(msg);
    }
    if(ls->smpgen) {
        sprintf(msg,"SMPTE generate start\n");
        gprintf_mex(msg);
    }
    if(ls->smpread) {
        sprintf(msg,"SMPTE read start please play tape recorder\n");
        gprintf_mex(msg);
    }
    sprintf(msg,"Exit(Stop) Pose/Continue(Space) Continue(CR) ");
    gprintf_mex(msg);
    sprintf(msg,"Help(Help)\n");
    gprintf_mex(msg);
    return(0);
}

/* set port default address */
sportadr_mex()
{
    int i;

  /* dumy port data address */
  /* dumy port status address */
    for(i = 0; i < 16; i++) {
        dport[i] = 0xe0d1;
        sport[i] = 0xe0d3;
    }
    return(0);
}
/*
    set mex patrameter
*/
optionset_mex(card)
char card[82];
{
    int  i,j,n;
    char work[80];

    n = 0;
    while(card[n]) n++;
    j = 0;
    while(card[j] != ' ') {
        work[j] = tolower(card[j]);
        j++;
    }
    work[j] = '\0';
    if(!strcmp(work,"&define")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) {
                sprintf(msg,"Error! mexrdf.dat #define statement\n");
                gprintf_mex(msg);
                return(1);
            }
        }
        i = 0;
        while(card[j] != ' ') {
            work[i++] = card[j];
            j++;
            if(j > n) {
                sprintf(msg,"Error! mexrdf.dat #define statement\n");
                gprintf_mex(msg);
                return(1);
            }
        }
        work[i] = '\0';
        if(!strcmp(work,"emu232c_port")) {
            ls->act232c = 1;   /* 232c port active */
            for(i = 18; i < 25; i++) {
                if(card[i] == '*') {
                    ls->e232cp = 999;
                    return(0);
                }
            }
            ls->e232cp = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"mpupc98_e0d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(0,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_e4d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(1,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_e8d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(2,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_ecd0_port")) {
            n = numdin_mex(card);
            mpuport_mex(3,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_f0d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(4,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_f4d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(5,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_f8d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(6,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_fcd0_port")) {
            n = numdin_mex(card);
            mpuport_mex(7,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_d0d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(8,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_d4d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(9,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_d8d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(10,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_dcd0_port")) {
            n = numdin_mex(card);
            mpuport_mex(11,n);
            return(0);
         }
        if(!strcmp(work,"mpupc98_c0d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(12,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_c4d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(13,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_c8d0_port")) {
            n = numdin_mex(card);
            mpuport_mex(14,n);
            return(0);
        }
        if(!strcmp(work,"mpupc98_ccd0_port")) {
            n = numdin_mex(card);
            mpuport_mex(15,n);
            return(0);
        }
        if(!strcmp(work,"midi_time_code_type")) {
            su->qtype = numdin_mex(card);
            if(su->qtype == 24) su->qtype = 0; /* 24 flame/sec */
            if(su->qtype == 25) su->qtype = 1; /* 25 flame/sec */
            if(su->qtype == 29) su->qtype = 2; /* 30 flame/sec drop */
            if(su->qtype == 30) su->qtype = 3; /* 30 non drop */
            return(0);
        }
        if(!strcmp(work,"direct_time_lock_port")) {
            ls->dtcport = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"midi_time_code_port")) {
            ls->mtcport = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"monitor")) {
            ls->monitor = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"clock_type")) {
            ls->clktyp = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"start_tempo_clock_up")) {
            ls->clkmod = -(numdin_mex(card));
            ls->curclk += ls->clkmod;
            chgtimert_mex();
            return(0);
        }
        if(!strcmp(work,"start_tempo_clock_down")) {
            ls->clkmod = numdin_mex(card);
            ls->curclk += ls->clkmod;
            chgtimert_mex();
            return(0);
        }
        if(!strcmp(work,"default_clock")) {
            ls->orgclk = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"esu_start_time")) {
            esugent_mex(card);
            return(0);
        }
        if(!strcmp(work,"esu_gen_delay_time")) {
            ls->edelay = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"esu_read_delay_time")) {
            ls->erdelay = numdin_mex(card);
            return(0);
        }
    }
    if(!strcmp(work,"&send")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) {
                sprintf(msg,"Error! mexrdf.dat #send statement\n");
                gprintf_mex(msg);
                return(1);
            }
        }
        i = 0;
        while(card[j] != ' ') {
            work[i++] = card[j];
            j++;
            if(j > n) {
                sprintf(msg,"Error! mexrdf.dat #send statement\n");
                gprintf_mex(msg);
                return(1);
            }
        }
        work[i] = '\0';
        if(!strcmp(work,"midi_time_code")) {
            ls->smpte = numdin_mex(card);
            return(0);
        }
        if(!strcmp(work,"direct_time_lock")) {
            ls->smptp = numdin_mex(card);
            return(0);
        }
    }
    return(0);
}

/* set mpu port number */
mpuport_mex(mpun,n)
int  mpun; /* Mpu Number */
int  n;    /* Assign Port Number "mexrdf.dat" */
{
    /* _dprintf("mpun = %d, n=%d\n",mpun,n); */
    if(n < 0 || n > 16) {
        sprintf(msg,"Error mexrdf.dat &define mpu port number %d\n",n);
        gprintf_mex(msg);
        return(0);
    }
    if(dport[n] != 0xe0d1) {
        sprintf(msg,"Worning Dupulicate port assign %d\n",n);
        gprintf_mex(msg);
    }
    la->p_act[n] = 1; /* port n active flag on */
    switch(mpun) {
        case  0 : dport[n] = DPORT0;
                  sport[n] = SPORT0;
                  break;
        case  1 : dport[n] = DPORT1;
                  sport[n] = SPORT1;
                  break;
        case  2 : dport[n] = DPORT2;
                  sport[n] = SPORT2;
                  break;
        case  3 : dport[n] = DPORT3;
                  sport[n] = SPORT3;
                  break;
        case  4 : dport[n] = DPORT4;
                  sport[n] = SPORT4;
                  break;
        case  5 : dport[n] = DPORT5;
                  sport[n] = SPORT5;
                  break;
        case  6 : dport[n] = DPORT6;
                  sport[n] = SPORT6;
                  break;
        case  7 : dport[n] = DPORT7;
                  sport[n] = SPORT7;
                  break;
        case  8 : dport[n] = DPORT8;
                  sport[n] = SPORT8;
                  break;
        case  9 : dport[n] = DPORT9;
                  sport[n] = SPORT9;
                  break;
        case 10 : dport[n] = DPORTA;
                  sport[n] = SPORTA;
                  break;
        case 11 : dport[n] = DPORTB;
                  sport[n] = SPORTB;
                  break;
        case 12 : dport[n] = DPORTC;
                  sport[n] = SPORTC;
                  break;
        case 13 : dport[n] = DPORTD;
                  sport[n] = SPORTD;
                  break;
        case 14 : dport[n] = DPORTE;
                  sport[n] = SPORTE;
                  break;
        case 15 : dport[n] = DPORTF;
                  sport[n] = SPORTF;
                  break;
        default : break;
    }
    return(0);
}

/* check active port table */
chkactb_mex()
{
    int i,j;

    if(!ls->quick) {
        ls->actpn = 0;
        for(i = 0; i < 16; i++) if(la->p_act[i]) ls->actpn++;
        if(ls->actpn) {
            sprintf(msg,"MIDI Active Port (");
            gprintf_mex(msg);
            for(i = 0; i < 16; i++) {
                if(la->p_act[i]) {
                    ls->actpn++;
                    sprintf(msg," %d",i);
                    gprintf_mex(msg);
                }
            }
            sprintf(msg," )\n");
            gprintf_mex(msg);
        }
    }
    /*
    _dprintf("monitor %d  clock %d  mtc(%d %d)  dtc(%d %d)\n",
        ls->monitor,ls->clktyp,ls->smpte,ls->mtcport,ls->smptp,ls->dtcport);
    */
    return(0);
}

/* port active check and initiarize */
initactp_mex()
{
    int i;

    for(i = 0 ; i < 16; i++) {
        if(la->p_act[i]) {
            /* _dprintf("Port %d checking\n",i); */
            su->clockflg = 0;
            /* *(char *)sport[i] */
            while((inp(sport[i])) & 0x40) {
                if(su->clockflg >= 10) {
                    la->p_act[i] = 0;  /* port active flag off */
                    break; /* 1/12 sec over */
                }
            }
            outp(sport[i],0xff);    /* Reset */
            outp(sport[i],0x3f);    /* UART Mode */
            su->clockflg = 0;
            while((inp(dport[i])) != 0xFE) {  /* ack ? */
                if(su->clockflg >= 10) {
                    la->p_act[i] = 0;  /* port active flag off */
                    break; /* 1/12 sec over */
                }
            }
        }
    }
    /* Esu active check */
    return(0);
}

numdin_mex(card)
char *card;
{
    int  i,k;
    int  n;
    char *pc;
    char cnum[10];

    k = 0;
    pc = card;
    while(*pc) {
        pc++;
        k++;
        if(k > 70) break;
    }
    pc--;
    if(*pc == ' ') {
        while(*pc == ' ') {
            pc--;
            k--;
            if(k <= 0) break;
        }
    }
    for(i = 0; i < 10; i++) cnum[i] = ' ';
    cnum[9] = '\0';
    for(k = 8; k >= 0; k--) {
        if(*pc == ' ') break;
        cnum[k] = *pc;
        pc--;
    }
    sscanf(cnum,"%d",&n);
    /* _dprintf("cnum=%s n=%d\n",cnum,n); */
    return(n);
}

/* get esu generate start time
#define esu_start_time 00:00:00/00
*/
esugent_mex(card)
char *card;
{
    char wk[4];
    int  i;
    int  pas;
    char *pc;

    pas = 0;
    pc = card;
    for(i = 0; i < 80; i++) {
        if(*pc == ':') {
            pas = 1;
            break;
        }
        pc++;
    }
    if(*(pc+3) != ':') pas = 0;
    if(*(pc+6) != '/') pas = 0;
    wk[0] = *(pc-2);
    wk[1] = *(pc-1);
    wk[2] = '\0';
    sscanf(wk,"%d",&ls->ehour);
    wk[0] = *(pc+1);
    wk[1] = *(pc+2);
    wk[2] = '\0';
    sscanf(wk,"%d",&ls->emin);
    wk[0] = *(pc+4);
    wk[1] = *(pc+5);
    wk[2] = '\0';
    sscanf(wk,"%d",&ls->esec);
    wk[0] = *(pc+7);
    wk[1] = *(pc+8);
    wk[2] = '\0';
    sscanf(wk,"%d",&ls->eflame);
    if(ls->ehour > 24) pas = 0;
    if(ls->emin > 60) pas = 0;
    if(ls->esec > 60) pas = 0;
    if(ls->eflame > 30) pas = 0;
    if(!pas) {
        sprintf(msg,"Error! mexrdf.dat %s\n",card);
        gprintf_mex(msg);
        return(-1);
    }
    if(ls->smpgen) {
        sprintf(msg,"Esu generate start time %02d:%02d:%02d/%02d\n",
            ls->ehour,ls->emin,ls->esec,ls->eflame);
    }
    return(0);
}

/*
    make multi file
        ex0_x   ->  ex0_x00.xxx return(99)  prtn = 0
        ex0_xx  ->  ex0_0xx.xxx return(0)   prtn = 36
        ex0_xxx ->  ex0_xxx.xxx return(0)   prtn = 0
        ex0/Y;X ->  ex0_002.xxx retnrn(Y)   prtn = X (fnmb = 0,fnmb = -1)
        ex0/Y;X ->  ex0_002.xxx retnrn(Y-1) prtn = X (fnmb = 1)
*/
mkmultif_h(infn,outf,ftyp,fnmb,prtn,yfile,xfile)
char *infn;
char *outf;
char *ftyp;
int  fnmb;
int  *prtn;
int  *yfile;
int  *xfile;
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
        if(*xmf == '/') vflg = 1;   /* 1st dimensional vectoe _xx or _x */
        if(*xmf == '_') vflg = 2;   /* 1st dimensional vectoe _xx or _x */
        if(*xmf == '(') vflg = 3;   /* 1st dimensional vectoe _xx or _x */
        if(*xmf == ';') xflg = 1;   /* 2nd dimensional vector /Y;X */
    }
    if(ls->debug == 4) {
        sprintf(msg,"infn = %s vflg = %d xflg = %d\n",infn,vflg,xflg);
        dputf_mex(1,10,msg);
    }
    if(vflg == 1 && xflg == 0) {
        xmf = infn;
        while(*(xmf++)) if(*xmf == '/') *xmf = '_';
    }
    if(vflg == 3) {
        /* set xleng */
    }
    if(vflg == 1 || vflg == 2) {
        /* _xx */
        mflg = 0;
        fleng = 0;
        pmf = infn;
        while(*(pmf++)) {
            fleng++;
            if(*pmf == '(') break;
        }
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
        /* _dprintf("w = %s xleng = %d\n",w,xleng); */
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
            *xfile = 0;
            return(99);
        }
    }
    if(xflg) {
        i = 0;
        xmf = infn;
        while(*(xmf++)) if(*xmf == ';') break;
        while(*(xmf++)) xw[i++] = *xmf;
    }
    sscanf(xw,"%d",prtn);
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
        sprintf(msg,"Error! file name \"%s\"\n",infn);
        gprintf_mex(msg);
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
        xw[j] = '\0';
        sprintf(msg,"i = %d  fnum = %s xw = %s\n",i,fnum,xw);
        gprintf_mex(msg);
        if(i == 1) {
            for(j = 0; j < 36; j++,ps++) if(fnum[0] == *ps) break;
            n = j;
        }
        else sscanf(fnum,"%d",&n);
        *yfile = n;
        sscanf(xw,"%d",xfile);
        sprintf(fnum,"%03d",fnmb+1);
        strcpy(outf,w);
        strcat_h(outf,"_");
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

mkyxfnum_h(fname,yfile,xfile,ps)
char fname[STLN];
int  *yfile;
int  *xfile;
char *ps;
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
    sscanf(w,"%x",xfile);
    return(0);
}
/*
Frase data 1   : E1xyzzzz
 x(ftype)      : reserve(frase type)
 y(fchan)      : channel
 zzzz(fsval)   : frase start value
*/
e1decode_mex(u)
union MCODE *u;
{
    int  f;

    fv->ftype = 0;
    fv->ftype = (u->wc[0] & 0xf0) >> 4;
    f = fv->ftype;                     /* flame type */
    fv->fchan[f] = u->wc[0] & 0x0f;
    fv->fsval[f] = u->wi[1];
    if(ls->dchk) {
        sprintf(msg,"E1(%04x%04x) Frase E1 / ftype=%d",
            u->wi[0],u->wi[1],fv->ftype);
        gprintf_mex(msg);
        sprintf(msg," fchan[%d]=%d fsval[%d]=%d\n",
            f,fv->fchan[f],f,fv->fsval[f]);
        gprintf_mex(msg);
    }
    return(0);
}

/*
Frase data 2   : E2xxyyyy
 xx(fctno)     : frase control number
 yyyy(feval)   : frase end value
*/
e2decode_mex(u)
union MCODE *u;
{
    int  f;

    f = fv->ftype;
    fv->fctno[f] = u->wc[0] & 0xff;
    fv->feval[f] = u->wi[1];
    if(ls->dchk) {
        sprintf(msg,"E2(%04x%04x) Frase E2 / ftype=%d",
            u->wi[0],u->wi[1],fv->ftype);
        gprintf_mex(msg);
        sprintf(msg," fctno[%d]=%d feval[%d]=%d\n",
            f,fv->fctno[f],f,fv->feval[f]);
        gprintf_mex(msg);
    }
    return(0);
}

/*
Frase data 3   : E3xxxxxx
 xxxxxx(fnval) : frase note value
*/
e3decode_mex(u)
union MCODE *u;
{
    union {
        int  wi[2];
        long wl;
    } w;
    int  f;

    f = fv->ftype;
    w.wi[0] = u->wi[1];
    w.wi[1] = u->wi[0] & 0x00ff;
    fv->fnval[f] = w.wl;
    if(ls->dchk) {
        sprintf(msg,"E3(%04x%04x) Frase E3 / ftype=%d",
            u->wi[0],u->wi[1],fv->ftype);
        gprintf_mex(msg);
        sprintf(msg," fnval[%d]=%ld\n",f,fv->fnval[f]);
        gprintf_mex(msg);
    }
    return(0);
}

/*   FA code ....................................
Tempo        : FAxxyyyy    xx > (100 - 200)
 xx(tsym)    : tempo symbol no.
 yyyy(temp)  : tempo 
 ex. [allegro]  xx(tsym) = 5x, yyyy(temp) = xxxx
              : FAwwyyyy    ww < 50
 ww(tsym)    : note type 0-21
 yyyy(temp)  : tempo val *100
 ex. [m.m. _/ = 90] ww(tsym) = 9, yyyy(temp) = 9000
 use mr->nval,mr->tsym,mr->temp
*/
fadecode_mex(u)
union MCODE *u;
{
    lv->tsym = u->wc[0] & 0xff;
    lv->temp = u->wi[1];
    if(lv->tsym > 21) {
        sprintf(msg,"Error tempo type number %d\n",lv->tsym);
        gprintf_mex(msg);
        return(0);
    }
    if(ls->dchk) {
        sprintf(msg,"FA(%04x%04x) Tempo / tsym=%d temp=%d\n",
        u->wi[0],u->wi[1],lv->tsym,lv->temp);
        gprintf_mex(msg);
    }
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
fbdecode_mex(u)
union MCODE *u;
{
    lv->movn = u->wc[0] & 0xff;
    lv->barn = u->wi[1];
    if(ls->dchk) {
        sprintf(msg,"FB(%04x%04x) Bar data / movn=%d barn=%d\n",
        u->wi[0],u->wi[1],lv->movn,lv->barn);
        gprintf_mex(msg);
    }
    return(0);
}

/*   FC code ....................................
Synchronize  : FCxxxxxx
 xxxxxx(sync): synchronize
 ex. $4/4 = 400000 start value
*/
fcdecode_mex(u)
union MCODE *u;
{
    union {
        int  wi[2];
        long wl;
    } w;

    w.wi[0] = u->wi[1];
    w.wi[1] = u->wi[0] & 0x00ff;
    lv->sync = w.wl;
    if(ls->dchk) {
        sprintf(msg,"FC(%04x%04x) Synchronize / sync=%ld\n",
        u->wi[0],u->wi[1],lv->sync);
        gprintf_mex(msg);
    }
    return(0);
}

getdigit_h(c,digit)
char c;
int  *digit;
{
    if(c >= '0' && c <= '9') *digit = c - '0';
    else if(c >= 'a' && c <= 'f') *digit = c - 'a' + 10;
    else if(c >= 'A' && c <= 'F') *digit = c - 'A' + 10;
    else return(-1);
    return(0);
}

