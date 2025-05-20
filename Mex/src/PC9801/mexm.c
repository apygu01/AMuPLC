/* mexm.c */
#include "mex.h"
extern KEYFLG *kf;             /* check shift,caps,ctrl,grph */
extern SETUPD *su;             /* setup data */
extern char msg[82];
extern int clock;              /* pc9801 clock */
extern int full;
extern int kfull;
extern int dcflg;
MEXSRG *ls;
MEXVAR *lv;
MEXACT *la;
MEXKEY *kv;                    /* key control */
MQXFRS *fv;                    /* frase data */
FILE *fopen_h(),*fpi,*fpr;
int  lcount;
int  slsf;
char *pa = "abcdefghijklmnopqrstuvwxyz";

main(argc,argv)
int argc;
char *argv[];
{
    int  i,j,k,kc,n;
    int  parm;
    int  ckfn[MAXF];
    char mfile[STLN];
    char dfile[STLN];
    int  helpf;
    char inf[STLN];
    MEXSRG lsa;
    MEXVAR lva;
    MEXACT laa;
    MQXFRS fva;
    MEXKEY kvc;
    int  yrtn;
    int  xrtn;
    char *xmf;
    char wk[10];
    char wwk[8];
    int  portn;         /* play port number */
    char sportn[8];
    KEYFLG keyflag;     /* shift caps etc. flags */
    SETUPD setupdt;     /* setup data */
    int  pas;
    int  sbsel;         /* seq.barn select */
    char pc;

    if(argc <= 1) {
        mexhelp();
        return(0);
    }
    ls = &lsa;
    lv = &lva;
    la = &laa;
    fv = &fva;
    kv = &kvc;
    helpf = 0;
    yrtn = 0;
    xrtn = 0;
    ls->dump = 0;
    ls->dchk = 0;
    ls->debug = 0;
    ls->play = 1;             /* OLD .mex */
    ls->quick = 0;            /* quick mode off */
    ls->termode = 0;          /* terminal mode off */
    ls->smpte = 0;
    ls->smptp = 0;
    ls->smpgen = 0;           /* ESU smpte generate */
    ls->smpread = 0;          /* ESU smpte read */
    ls->vectf = 0;            /* vecter flag clear */
    ls->blibf = 0;            /* Boyo lib ACT flag */
    ls->monitor = 1;          /* monitor on/off flag */
    ls->clktyp = 0;           /* real time clock */
    ls->clkmod = 0;
    ls->eflame = 0;
    ls->esec = 0;
    ls->emin = 0;
    ls->ehour = 0;
    ls->etype = 3;            /* ESU generate type 30 non drop */
    ls->edelay = 0;           /* ESU generate delay time */
    ls->act232c = 0;          /* 232c port activ off */
    ls->e232cp = 0;           /* EMU232c port */
    ls->smptef = 30;          /* SMPTE 30 flame */
    ls->mexres = 120;         /* 1/120 flame */
    su->monif = 0;            /* RS232c input monitor off */
    kv->klock = 0;            /* key lock flag */
    kv->xport = 88;           /* execute port all */
    kv->sport = 0;            /* select port */
    kv->gmode = 88;           /* graph key mode none */
    kv->schan = 1;            /* set mode channel */
    kv->kmflg = 0;            /* key mode flag clear */
    ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"I8086"))  ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"MSDOS"))  ls->comptyp = 0;
    if(!strcmp(COMPUTER_TYPE,"M68000")) ls->comptyp = 1;
    if(!strcmp(COMPUTER_TYPE,"UNIX"))   ls->comptyp = 1;
    if(!strcmp(COMPUTER_TYPE,"DEC"))    ls->comptyp = 2;
    if(!strcmp(COMPUTER_TYPE,"VAX"))    ls->comptyp = 2;
    for(i = 1; i < argc; i++) {
        parm = 0;
        ckfn[i] = 0;
        if(!strcmp(argv[i],"-h")) {
            helpf = 1;
            ls->play = 0;
            parm = 1;
        }
        if(!strcmp(argv[i],"-edp")) {  /* mex data dump */
            ls->dump = 1;
            ls->play = 0;
            parm = 1;
        }
        if(!strcmp(argv[i],"-cdp")) {  /* mec data dump */
            ls->dump = 2;
            ls->play = 0;
            parm = 1;
        }
        if(!strcmp(argv[i],"-sdp")) {  /* meg data list */
            ls->dump = 3;
            ls->play = 0;
            parm = 1;
        }
        if(!strcmp(argv[i],"-dc")) {   /* data chek */
            ls->dchk = 1;
            parm = 1;
        }
        if(!strcmp(argv[i],"-d2")) {    /* debug mode */
            ls->debug = 2;
            parm = 1;
        }
        if(!strcmp(argv[i],"-d3")) {    /* debug mode */
            ls->debug = 3;
            parm = 1;
        }
        if(!strcmp(argv[i],"-d4")) {    /* debug mode */
            ls->debug = 4;
            parm = 1;
        }
        if(!strcmp(argv[i],"-d")) {    /* debug mode */
            ls->debug = 1;
            parm = 1;
        }
        if(!strcmp(argv[i],"-q")) {    /* quick mode */
            ls->quick = 1;
            ls->play = 2;
            parm = 1;
        }
        if(!strcmp(argv[i],"-mem")) {   /* Memory play */
            ls->play = 3;
            parm = 1;
        }
        if(!strcmp(argv[i],"-rm")) {   /* 232c input data monitor mode */
            su->monif = 1;
            parm = 1;
        }
        if(!strcmp(argv[i],"-ft")) {
            ls->clktyp = 1;            /* flame time clock */
            parm = 1;
        }
        if(!strcmp(argv[i],"-fx")) {
            ls->clktyp = 3;            /* fixed time clock */
            parm = 1;
        }
        if(!strcmp(argv[i],"-esg")) {  /* esu smpte genarate */
            ls->smpgen = 1;
            parm = 1;
        }
        if(!strcmp(argv[i],"-esr")) {  /* esu smpte read */
            ls->smpread = 1;
            parm = 1;
        }
        if(!strcmp(argv[i],"-tm")) {  /* terminal mode */
            ls->termode = 1;
            ls->play = 0;
            parm = 1;
        }
        xmf = argv[i];
        while(*xmf) {
            if(*xmf == '-') {
                parm = 1;
                break;
            }
            xmf++;
        }
        if(!parm) ckfn[i] = 1;
    }
    if(helpf) {
        hlpread_mex();
        return(0);
    }
    kvinit_mex();             /* MEXKEY table init */
    tinit();                  /* Text Window init Boyo */
    actwindow(2,5,77,5);
    setsdf(25,White|Reverse);
    _tcls();
    /* ------------- */
    kf = &keyflag;
    su = &setupdt;
    su->maskflg = 0;
    full = 0;
    kfull = 0;
    dcflg = 0;              /* xon xoff flag */
    if(ls->termode) {
        set232c(9600,8,'n',0);
        allocbuf();
        setupvec();                    /* setup vector */
        savereg();                     /* save old registors */
        setupcd();                     /* setup keytable */
        setupval();                    /* setup su-> value */
        termode();                     /* terminal mode */
    }
    sprintf(msg," Tact Midi file EXecuter, Version %s\n",VERSION);
    gprintf_mex(msg);
    sprintf(msg," Copyright (C) ht 1988, 1989. All rights reserved.\n");
    gprintf_mex(msg);
    /* port selected ? */
    slsf = 0;
    if(ls->play) {
        _FRbox(1,3,79,8,1);
        _locate(1,9);
        clockset();                    /* timer clock set */
        allocbuf();
        setupvec();                    /* setup vector */
        savereg();                     /* save old registors */
        setupcd();                     /* setup keytable */
        setupval();                    /* setup su-> value */
        /* chktint();                  timer intrupt count check */
        mconfig_mex(argc,argv);        /* config */
    }
    for(i = 1; i < argc; i++) {
        if(ckfn[i]) {
            su->countall = 0;         /* Time clock count all */
            ls->brself = 0;        /* bar number select flag */
            ls->mbendf = 0;        /* bar number end select flag */
            lv->ssequ = 1;         /* start movment number */
            lv->sbarn = 0;         /* start bar number */
            lv->esequ = 99;        /* end movment number */
            lv->ebarn = 99999;     /* end bar number */
            xmf = argv[i];
            while(*xmf) {
                if(*xmf == ':') slsf = 1;
                if(*xmf == '~') *xmf = ',';
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
                sbsel = 0;
                while(*xmf) {
                    if(*xmf == ')') break;
                    /* start,end -0100003,010007 */
                    if(*xmf == '~') *xmf = ',';
                    if(*xmf == ',') {
                        pas = 1;
                        break;
                    }
                    if(*xmf == '.') sbsel = 1;
                    wk[j] = *xmf;
                    xmf++;
                    j++;
                    if(j > 7) {
                        sprintf(msg,"Error! argument %s\n",argv[i]);
                        gprintf_mex(msg);
                        break;
                    }
                }
                wk[j] = '\0';
                /* _dprintf("wk=%s j=%d\n",wk,j); */
                if(sbsel) {
                    ls->brself = 3;
                    k = 0;
                    n = 0;
                    while(1) {
                        if(wk[k] == '.') break;
                        wwk[n] = wk[k];
                        k++;
                        if(k > j) break;
                        n++;
                        /*
                        if(n > 1) {
                            sprintf(msg,"Worning! (1) Seq.Barn argument %s\n",
                                argv[i]);
                            gprintf_mex(msg);
                            break;
                        }
                        */
                    }
                    wwk[n] = '\0';
                    if(isdigit(wwk[0])) sscanf(wwk,"%d",&lv->ssequ);
                    if(isalpha(wwk[0])) {
                        pc = tolower(wwk[0]);
                        lv->ssequ = 0;
                        for(kc = 0; kc < 26; kc++) {
                            if(pc == pa[kc]) {
                                lv->ssequ = kc + 10;
                                break;
                            }
                        }
                    }
                    n = 0;
                    k++;
                    while(1) {
                        if(wk[k] == '\0') break;
                        if(wk[k] == ',') break;
                        wwk[n] = wk[k];
                        k++;
                        if(k > j) break;
                        n++;
                        if(n > 5) {
                            sprintf(msg,"Error! (1) Seq.Barn argument %s\n",
                                argv[i]);
                            gprintf_mex(msg);
                            /* dputf_mex(1,24,msg); */
                            break;
                        }
                    }
                    wwk[n] = '\0';
                    sscanf(wwk,"%ld",&lv->sbarn);
                    /*
                    sprintf(msg,"wwk = %s lv->sbarn = %ld\n",wwk,lv->sbarn);
                    gprintf_mex(msg);
                    */
                }
                else {
                    switch (j) {
                        case  0 : ls->brself = 2;
                                  break;
                        case  1 : /* sscanf(wk,"%d",&lv->ssequ); */
                                  if(isdigit(wk[0])) sscanf(wk,"%d",&lv->ssequ);
                                  if(isalpha(wk[0])) {
                                      pc = tolower(wk[0]);
                                      lv->ssequ = 0;
                                      for(k = 0; k < 26; k++) {
                                          if(pc == pa[k]) {  
                                              lv->ssequ = k + 10;
                                              break;
                                          }
                                      }
                                  }   
                                  ls->brself = 1;
                                  break;
                        case  2 : sscanf(wk,"%d",&lv->ssequ);
                                  ls->brself = 1;
                                  break;
                        case  3 : sscanf(wk,"%ld",&lv->sbarn);
                                  ls->brself = 2;
                                  break;
                        case  4 : sscanf(wk,"%ld",&lv->sbarn);
                                  ls->brself = 2;
                                  break;
                        case  5 : sscanf(wk,"%ld",&lv->sbarn);
                                  ls->brself = 2;
                                  break;
                        case  7 : for(j = 0, k = 2;j < 6;j++,k++) wwk[j] = wk[k];
                                  wk[2] = '\0';
                                  sscanf(wk,"%d",&lv->ssequ);
                                  sscanf(wwk,"%ld",&lv->sbarn);
                                  ls->brself = 3;
                                  break;
                        default : sprintf(msg,"Error! sequ barn start select\n");
                                  gprintf_mex(msg);
                                  ls->brself = 0;
                                  break;
                    }
                }
                if(pas) {
                    j = 0;
                    sbsel = 0;
                    xmf++;
                    while(*xmf) {
                        if(*xmf == ')') break;
                        wk[j] = *xmf;
                        if(*xmf == '.') sbsel = 1;
                        xmf++;
                        j++;
                        if(j > 9) {
                            sprintf(msg,"Error! argument %s\n",argv[i]);
                            gprintf_mex(msg);
                            break;
                        }
                    }
                    wk[j] = '\0';
                    /* _dprintf("wk=%s j=%d\n",wk,j); */
                    if(sbsel) {
                        ls->mbendf = 3;
                        k = 0;
                        n = 0;
                        while(1) {
                            if(wk[k] == '.') break;
                            wwk[n] = wk[k];
                            k++;
                            if(k > j) break;
                            n++;
                            if(n > 2) {
                                 sprintf(msg,
                                "Error! (2) Seq.Barn argument %s\n",argv[i]);
                                gprintf_mex(msg);
                                /* dputf_mex(1,23,msg); */
                                break;
                            }
                        }
                        wwk[n] = '\0';
                        sscanf(wwk,"%d",&lv->esequ);
                        n = 0;
                        k++;
                        while(1) {
                            if(wk[k] == '\0') break;
                            wwk[n] = wk[k];
                            k++;
                            if(k > j) break;
                            n++;
                            if(n > 5) {
                                sprintf(msg,
                                "Error! (3) Seq.Barn argument %s\n",argv[i]);
                                gprintf_mex(msg);
                                /* dputf_mex(1,24,msg); */
                                break;
                            }
                        }
                        wwk[n] = '\0';
                        sscanf(wwk,"%ld",&lv->ebarn);
                        /*
                        sprintf(msg,"wwk = %s lv->ebarn = %ld\n",wwk,lv->ebarn);
                        gprintf_mex(msg);
                        */
                        sprintf(msg,"%s start %d.%ld end %d.%ld\n",
                            argv[i],lv->ssequ,lv->sbarn,lv->esequ,lv->ebarn);
                        gprintf_mex(msg);
                    }
                    else {
                        switch (j) {
                            case  1 : sscanf(wk,"%d",&lv->esequ);
                                      ls->mbendf = 1;
                                      break;
                            case  2 : sscanf(wk,"%d",&lv->esequ);
                                      ls->mbendf = 1;
                                      break;
                            case  3 : sscanf(wk,"%ld",&lv->ebarn);
                                      ls->mbendf = 2;
                                      break;
                            case  4 : sscanf(wk,"%ld",&lv->ebarn);
                                      ls->mbendf = 2;
                                      break;
                            case  5 : sscanf(wk,"%ld",&lv->ebarn);
                                      ls->mbendf = 2;
                                      break;
                            case  7 : for(j = 0, k = 2;j < 6;j++,k++)
                                          wwk[j] = wk[k];
                                      wk[2] = '\0';
                                      sscanf(wk,"%d",&lv->esequ);
                                      sscanf(wwk,"%ld",&lv->ebarn);
                                      ls->mbendf = 3;
                                      break;
                            default : sprintf(msg,"Error! barn sequ end select\n");
                                      gprintf_mex(msg);
                                      ls->mbendf = 0;
                                      break;
                        }
                    }
                /*
                    _dprintf("lv->ssequ=%d lv->sbarn=%ld ",
                        lv->ssequ,lv->sbarn);
                    _dprintf("lv->esequ=%d lv->ebarn=%ld\n",
                        lv->esequ,lv->ebarn);
                */
                }
            }
            if(slsf) {
                xmf = argv[i];
                k = 0;
                slsf = 0;
                while(*xmf) {
                    if(*xmf == ':') {
                        slsf = 7;
                        break;
                    }
                    if(*xmf == '~') *xmf = ',';
                    inf[k] = *xmf;
                    xmf++;
                    k++;
                }
                inf[k] = '\0';
                xmf++;
                if(slsf == 7) {
                    k = 0;
                    while(*xmf) {
                        wk[k] = *xmf;
                        xmf++;
                        k++;
                    }
                    wk[k] = '\0';
                    sscanf(wk,"%d",&portn);
                    sprintf(sportn,"%03d",portn);
                    strcpy(dfile,inf);
                    strcat(dfile,"_");
                    strcat(dfile,sportn);
                    strcat(dfile,".mex");
                    /*
                    _dprintf("inf=%s wk=%s portn=%d sportn=%s dfile=%s\n",
                        inf,wk,portn,sportn,dfile);
                    */
                    mexplay_mex(dfile);
                }
            }
            else {
                if(ls->play == 1 || ls->play == 3)
                    defplay_mex(argv[i],mfile,xrtn,yrtn);
            }
            if(ls->quick) quickplay_mex(argv[i],mfile,xrtn,yrtn);
            if(ls->dump) {
                if(ls->vectf) resetvec();
                if(ls->blibf) tterm();  /* Close Text Window Boyo Lib */
                dumplay_mex(argv[i],mfile);
            }
        }
    }
    if(ls->vectf) resetvec();
    if(ls->blibf) tterm();  /* Close Text Window Boyo Lib */
    return(0);
}

mconfig_mex(argc,argv)
int  argc;
char *argv[];
{
    int i,j;
    FILE *fopen(),*frp;

    for(i = 0; i < 16; i++) la->p_act[i] = 0;
    if((frp = fopen("mexrdf.dat","r")) != NULL) {
        getmexcg_mex(frp);
        fclose(frp);
    }
    else {
        sprintf(msg,"Worning mexrdf.dat not found\n");
        gprintf_mex(msg);
    }
    for(i = 1; i < argc; i++) {
        if(!strcmp(argv[i],"-m")) ls->monitor = 2;  /* monitor */
        if(!strcmp(argv[i],"-nm")) ls->monitor = 0; /* no monitor */
        if(!strcmp(argv[i],"-mtc")) ls->smpte = 1;  /* send midi time code */
        if(!strcmp(argv[i],"-dtc")) ls->smptp = 1; /* send direct time code */
        if(!strcmp(argv[i],"-rt")) ls->clktyp = 0;  /* real time clock */
        if(!strcmp(argv[i],"-ft")) ls->clktyp = 1;  /* flame time clock */
        if(!strcmp(argv[i],"-fx")) ls->clktyp = 3;  /* fixed time clock */
    }
    /* if(!ls->quick) prtmode_mex(); */
    return(0);
}

defplay_mex(arg,mfile,xrtn,yrtn)
char *arg;
char mfile[STLN];
int  xrtn;
int  yrtn;
{
    char *xmf;
    int  pas;
    char inf[STLN];
    char dfile[STLN];
    int  k;
    int  sp;

    yrtn = mkmultif_h(arg,mfile,".mec",-1,&xrtn,&su->trak,&su->memb);
    su->xrtn = 3;
    if(yrtn == 99) su->xrtn = 1;
    if(xrtn == 36) su->xrtn = 2;
    xmf = arg;
    pas = 0;
    sp = 0;
    while(*xmf) {
        if(*xmf == '(') {
            sp = 1;
            break;
        }
        if(*xmf == ';' || *xmf == ',') su->xrtn = 4;
        if(*xmf == '/' || *xmf == '_') pas = 1;
        if(*xmf == '.') pas = 2;
        xmf++;
    }
    if(pas == 0) su->xrtn = 0;
    /*              xrtn   trak   memb  su->xrtn
       fn       :      0      0      0         0
       fn/1     :      0      1      0         1
       fn/2,3   :      3      2      3         2
       fn/a10   :      0     10     10         3
       fn/10    :     36      0     10         4
    */
    /* add 2005.7 sp */
    if(sp) {
        xmf = arg;
        sp = 0;
        while(*xmf) {
            if(*xmf == ')') break;
            if(*xmf == ';' || *xmf == ',') sp = 2;
            xmf++;
        }
    }
    if(!sp) {
        if(su->xrtn != 4) {
            if(lv->ssequ <= su->trak) lv->ssequ = su->trak + 1;
            if(lv->esequ <= su->trak) lv->esequ = su->trak + 1;
        }
    }
    if(ls->debug == 2 || ls->debug == 4) {
        sprintf(msg,"arg= %s xrtn=%d su->trak=%d su->memb=%d",
            arg,xrtn,su->trak,su->memb);
        gprintf_mex(msg);
        sprintf(msg," yrtn=%d su->xrtn=%d\n",yrtn,su->xrtn);
        gprintf_mex(msg);
        sprintf(msg,"ls->mbendf=%d,lv->ssequ=%d lv->sbarn=%ld ",
            ls->mbendf,lv->ssequ,lv->sbarn);
        gprintf_mex(msg);
        sprintf(msg,"lv->esequ=%d lv->ebarn=%ld\n",
            lv->esequ,lv->ebarn);
        gprintf_mex(msg);
    }
    if(pas == 2) {
        su->xrtn = 0;
        mexplay_mex(arg);
        return(0);
    }
    xmf = mfile;
    k = 0;
    while(*xmf) {
        if(*xmf == '/') break;
        if(*xmf == '_') break;
        if(*xmf == '(') break;
        inf[k] = *xmf;
        xmf++;
        k++;
    }
    inf[k] = '\0';
    strcpy(dfile,inf);
    strcat(dfile,".mex");
    mexplay_mex(dfile);
    return(0);
}

quickplay_mex(arg,mfile,xrtn,yrtn)
char *arg;
char mfile[STLN];
int  xrtn;
int  yrtn;
{
    char *xmf;
    int  pas;
    int  i;

    mkmultif_h(arg,mfile,".mec",-1,&xrtn,&su->trak,&su->memb);
    su->xrtn = 3;
    if(yrtn == 99) su->xrtn = 1;
    if(xrtn == 36) su->xrtn = 2;
    xmf = arg;
    pas = 0;
    while(*xmf) {
        if(*xmf == '(') break;
        if(*xmf == ';' || *xmf == ',') su->xrtn = 4;
        if(*xmf == '/' || *xmf == '_') pas = 1;
        if(*xmf == '.') pas = 2;
        xmf++;
    }
    if(pas == 0) su->xrtn = 0;
    /*              xrtn   trak   memb  su->xrtn
       fn       :      0      0      0         0
       fn/1     :      0      1      0         1
       fn/2,3   :      3      2      3         2
       fn/a10   :      0     10     10         3
       fn/10    :     36      0     10         4
    */
    if(su->xrtn != 4) {
        if(lv->ssequ <= su->trak) lv->ssequ = su->trak + 1;
        if(lv->esequ <= su->trak) lv->esequ = su->trak + 1;
    }
    if(ls->debug == 2) {
        sprintf(msg,"arg= %s xrtn=%d su->trak=%d su->memb=%d",
            arg,xrtn,su->trak,su->memb);
        gprintf_mex(msg);
        sprintf(msg," yrtn=%d su->xrtn=%d\n",yrtn,su->xrtn);
        gprintf_mex(msg);
        sprintf(msg,"ls->mbendf=%d,lv->ssequ=%d lv->sbarn=%ld ",
            ls->mbendf,lv->ssequ,lv->sbarn);
        gprintf_mex(msg);
        sprintf(msg,"lv->esequ=%d lv->ebarn=%ld\n",
            lv->esequ,lv->ebarn);
        gprintf_mex(msg);
    }
    if(pas == 2) {
        su->xrtn = 0;
        mqxplay_mex(arg);
        return(0);
    }
    i = 0;
    xrtn = 0;
    while(mfile[i]) {
        if(mfile[i] == '(') xrtn = 1;
        if(mfile[i] == '.') break;
        i++;
    }
    if(xrtn) {
        i = 0;
        while(mfile[i]) {
           if(mfile[i] == '(') break;
           i++;
        }
        mfile[i] = '\0';
        strcat(mfile,".mec");
    }
    mqxplay_mex(mfile);
    return(0);
}

dumplay_mex(arg,mfile)
char *arg;
char mfile[STLN];
{
    char dfile[STLN];
    int  xdm;
    int  ydm;
    int  k;
    int  xrtn;

    mkmultif_h(arg,mfile,".mex",-1,&xrtn,&ydm,&xdm);
    for(k = 0; k < STLN; k++) {
        dfile[k] = mfile[k];
        if(mfile[k] == '.') break;
    }
    dfile[k] = '\0';
    if(ls->dump == 2) strcat(dfile,".mec");
    if(ls->dump == 3) strcat(dfile,".meg");
    /* _dprintf("dfile=%s\n",dfile); */
    mexdchk_mex(dfile);
    return(0);
}

mexdchk_mex(dfile)
char dfile[STLN];
{
    int  c;
    int  i;

    if(ls->dump == 1) {
        for(i = 0; i < STLN; i++) {
            if(dfile[i] == '_') break;
            if(dfile[i] == '\0') break;
        }
        dfile[i] = '\0';
        strcat(dfile,".mex");
        dump_mex(dfile);
        return(0);
    }
    if(ls->dump == 2) {
        dump_mex(dfile);
        return(0);
    }
    if(ls->dump == 3) {
        fpi = fopen_h(dfile,"rb");
        sprintf(msg,"\nMec source list file \"%s\"\n",dfile);
        gprintf_mex(msg);
        while((c = fgetc(fpi)) != EOF) _putchr(c);
        fclose(fpi);
    }
    return(0);
}

/* MEXKEY table init */
kvinit_mex()
{
    int  i;

    for(i = 0; i < MAXPT; i++) {
        kv->prchg[i] = 0;            /* program change */
        kv->modur[i] = 0;
        kv->volum[i] = 64;           /* volume */
    }
    return(0);
}

mexhelp()
{
    cls_mex();
    printf("Mex (Midi EXecuter) v %s\n",VERSION);
    printf("mex fn [,fn] [ Options ]\n");
    printf("Options : -m(Monitor) -nm(No Monitor) -rm(Rs232c Mon)\n");
    printf("          -mem(Read .mex data to Memory)\n");
    printf("          -q(Quick execute read fn_xyy.mec file)\n");
    printf("          -mtc(Send Midi Time Code)\n");
    printf("          -ft(Flame Time clock)\n");
    printf("          -fx(Fixed Time clock)\n");
    printf("          -esg(Esu Smpte Generate)\n");
    printf("          -esr(Esu Smpte Read)\n");
    printf("          -dtc(Send Performar's Direct Time lock)\n");
    printf("          -tm(Terminal Mode)\n");
    printf("          -edp(Execute file xxx.mex Dump)\n");
    printf("          -d(Debug) -dc(Data Check)\n");
    printf("          -cdp(Mec Data Dump) -sdp(Meg file list)\n");
    printf("Ex.\n");
    printf("mex fn(sseq.sbarn) (Seq & Barn select)\n");
    printf("mex fn(sseq.sbarn,eseq.ebarn) (Seq & Barn select Start,end)\n");
    printf("bat fn sseq.sbarn~eseq  (Seq select Start,end)\n");
    printf("mex fn/xyy -q     (Point)\n");
    printf("mex fn/xx -mtc    (X:Vector)\n");
    printf("mex fn/y(003,005) (Y:Vector & Start,end barn select)\n");
    return(0);
}

cls_mex()
{
    printf("%c[2J",ESC);  /* clear screen */
    return(0);
}

