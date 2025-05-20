
// ----------------------
//  AMuPLC (meb1.c)
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
extern MIVARG *mv;
extern MIVMST *mm;
extern MIVMXM *mx;
extern MEBVAR *mb;
extern MEBSRG *ls;
extern MEBEDT *ge;
extern MEBPH  *ph;
extern MEBGRT *gr,*gd,*ga,*gm,*gs,*gc;
extern MEBCSL *cs;
int wktrnst[2][12] = {   /* transpose table */
    {0,7,2,10,4,8,6,1,11,3,9,5},
    {0,12,2,10,4,8,13,1,11,3,9,14}
};
char scard[MSTC][82];
int  scdn;  /* scard number */
char sectitl[36][55];   /* section title */
/* char secsymt[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; */
extern char *prtsym;
char Title1[] = "Title/////////////////////////////////////";
char Title2[] = "////////////////////////////////Title";
extern int  secnt;        /* section count */

/* main (conductor) beautify */
int mbeautif_meb(char *inf,struct mivksn *mk,struct mivsym ms[],int gsf)
//char *inf;
//struct mivksn *mk;
//struct mivsym ms[];
//int  gsf;  /* -gs option */
{
    int  i;
    int  j;
    int  k;
    int  eoflg;
    int  mnflg2;
    int  ren;
    int  rept;
    int  memb;
    char outf[STLN];
    char obsf[STLN];
    char *pf;
    char df;
    char card[82];
    FILE *fpi;
    char w[4];
    int  genloop;      /* genarate note lines loop count */
    int  secno;        /* section no. */
    int  rf; /* ryhm flag */
    int  dlflg;        /* Delete line flag */

    startval_meb();    /* set start value */
    mb->bexp = 50;     /* {%50}  */
    for(j = 0; j < 2; j++ ) {
        for(i = 0; i < 12 ; i++) mb->trnst[j][i] = wktrnst[j][i];
    }
    pf = inf;
    i = 0;
    while(*pf != '_') {
        obsf[i] = *pf;
        i++;
        pf++;
    }
    obsf[i] = '\0';
    strcat(obsf,".mix");
    /*  printf("obsf = %s\n",obsf); */
    if(mb->observf) {
        if((ls->fmx = fopen(obsf,"rb")) == NULL) {
            mv->wrncnt++;
            mv->msglvl = 2;
            if(mv->msglvl >= mv->prtlvl)
            printf("Worning! observe file \"%s\" not found\n",obsf);
            mb->observf = 0;
            ls->fmxopend = 0;
        }
        else ls->fmxopend = 1;
    }
    for(i = 0; i < 128; i++) ls->obstb[i] = 0;
    i = 0;
    /* mb->renp = 0; */
    eoflg = 0;
    dlflg = 0;
    mb->setsmpf = 0;   /* #set smpte flag off */
    mb->smpstf = 0;
    mb->smpsts = 0;
    mb->smpstm = 0;
    mb->smpsth = 0;
    mb->cline = 0;
    mb->SKTPv = 0;
    mb->rstmpf = mb->arstmpf;
    mb->rssmptf = mb->arssmptf;
    mb->rsdynaf = mb->arsdynaf;
    /* cond's beautify symbol begin point flag on */
    /* mb->bsbgnp = 1;     add 98 6 */
    pf = inf;
    while(*pf != '.') {
        outf[i] = *pf;
        i++;
        pf++;
    }
    outf[i] = '\0';
    pf = inf;
    while(*pf != '_') {
        pf++;
    }
    pf++;
    /* mb->partp = toupper(*pf); */
    mb->partp = *pf;
    for(i = 0; i < 36; i++) if(mb->partp == prtsym[i]) secno = i;
    //printf("sequ=%d xbnum=%d\n",mb->sequ,mb->xbnum);
    /*
    if(mv->debug) printf("sequ=%d xbnum=%d cbnum=%d\n",
        mb->sequ,mb->xbnum,mb->cbnum);
    */
    mb->sequ = secno;
    mb->xbnum = 0;
    pf++;
    w[0] = *pf;
    pf++;
    w[1] = *pf;
    w[2] = '\0';
    sscanf(w,"%d",&memb);
    if(memb == 0) mb->lbdflg = 0;
    memb = memb + 1; /* start member is 1 */
    if(memb != mb->memb) mv->mmpos = 0;
    if(mv->debug == 3) printf("Debug3! memb=%d mb->memb=%d mv->mmpos=%d\n",
        memb,mb->memb,mv->mmpos);
    mb->memb = memb;
    strcat_h(outf,".meb");
    mopen_meb(); /* mbeuat */
    fpi = fopen_h(inf,"r");
    if(mv->source == 1 || mv->trace) printf("\n");
    fgets(mv->card,82,fpi);
    fgets(mv->next,82,fpi);
    while(fgets(mv->next2, 82, fpi)) {
        mv->line++;
        mb->prflg = 1;
        mnflg2 = 0;
        mb->bexpsf = 1; /* beautify expression symbol serch on */
        ren = 0;
        rept = 1;
        df = mv->dflg;
        for(i = 0; i < 82; i++) {
            k = i;
            if(mv->card[k] == '\n' || mv->card[k] == '\r') {
                mv->card[k] = '\0';
                break;
            }
        }
        if(mv->next[0] == 'f') eoflg = 1;
        scdn = 2;   /* scard number use at meblib1(rensync) */
        switch (mv->card[0]) {
            case 's'  : /* beaumset_meb(); */
                        mb->renp = 1;
                        renget_meb();
                        if(mb->renp >= MSTC) {
                            mv->errcnt++;
                            mv->msglvl = 3;
                            if(mv->msglvl >= mv->prtlvl)
                                printf("Error! %06d renpu max is %d\n",
                                mv->line,MSTC);
                            break;
                        }
                        if(mb->renp > 1) {
                            rensync_meb(fpi);
                            strcpy(mv->card,scard[0]);
                        }
                        else for(i = 0; i < mb->rept; i++) {
                            notesync_meb();
                            if(mb->cueing) excsmpnl_meb();
                        }
                        mb->mnflg1 = 2;
                        if(mb->clrntl) mb->prflg = 0;
                        if(mb->durtev) duration_meb();
                        edc_transpose();
			edc_velocity();
                        break;
            case 'c'  : /* beaumset_meb(); */
                        mb->renp = 1;
                        renget_meb();
                        if(mb->renp >= MSTC) {
                            mv->errcnt++;
                            mv->msglvl = 3;
                            if(mv->msglvl >= mv->prtlvl)
                            printf("Error! %06d renpu max is %d\n",
                                mv->line,MSTC);
                            break;
                        }
                        if(mb->renp > 1) {
                            rensync_meb(fpi);
                            strcpy(mv->card,scard[0]);
                        }
                        else for(i = 0; i < mb->rept; i++) {
                            notesync_meb();
                            if(mb->midi) excmidnl_meb();
                        }
                        mb->mnflg1 = 2;
                        if(mb->clrntl) mb->prflg = 0;
                        if(mb->durtev) duration_meb();
                        break;
            case '/'  : ren = beaubar_meb();
                        mb->renp = ren;
                        beausyms_meb();
                        beaumset_meb();
                        notesync_meb();
                        break;
            case 'O'  : modcard_meb();
                        ren = beaubar_meb();
                        mb->renp = ren;
                        beausyms_meb();
                        beaumset_meb();
                        notesync_meb();
                        break;
            case 'o'  : modcard_meb();
                        ren = beaubar_meb();
                        mb->renp = ren;
                        beausyms_meb();
                        beaumset_meb();
                        notesync_meb();
                        break;
            case '|'  : if(mv->card[1] == 'O') {
                            modcard_meb();
                            ren = beaubar_meb();
                            mb->renp = ren;
                            beausyms_meb();
                            beaumset_meb();
                            notesync_meb();
                        }
                        else {
                            beausyms_meb();
                            beaumset_meb();
                            /* notesync_meb(); */
                            if(mb->cueing) excsmpnl_meb();
                            if(mb->midi) excmidnl_meb();
                            if(mb->clrsml) mb->prflg = 0;
                        }
                        break;
            case '-'  : if(mv->card[2] == '\0' || (mv->card[79] != '\0'))
                        {
                            genbar_meb(mb->bartyp,mv->card);
                        }
                        if(mv->card[2] == ' ' && (mv->card[60] == ' ')) {
                            genbar_meb(mb->bartyp,mv->card);
                        }
                        if(mb->mnflg1 == 1) mnflg2 = 1;
                        mainbar_meb();
                        break;
            case '.'  : ren = beauabar_meb();  /* acciaccatura */
                        beausyms_meb();
                        beaumset_meb();
                        notesync_meb();
                        if(mb->cueing) excsmpnl_meb();
                        if(mb->midi) excmidnl_meb();
                        break;
            case ','  : ren = beaunbar_meb();  /* nachschlag */
                        beausyms_meb();
                        beaumset_meb();
                        if(mb->cueing) excsmpnl_meb();
                        if(mb->midi) excmidnl_meb();
                        break;
            case 'R'  : if(mb->clrstl) {
                            mb->prflg = 0;
                            break;
                        }
                        if(mb->smpmid == 1 || mb->smpmid == 2) {
                            beausyms_meb();
                            beaumset_meb();
                            restsync_meb();
                            if(mb->cueing) excsmpnl_meb();
                            if(mb->midi) excmidnl_meb();
                        } else pcaserst_meb();
                        break;
            case 'r'  : if(mb->clrstl) {
                            mb->prflg = 0;
                            break;
                        }
                        if(mb->smpmid == 1 || mb->smpmid == 2) {
                            beausyms_meb();
                            beaumset_meb();
                            restsync_meb();
                            if(mb->cueing) excsmpnl_meb();
                            if(mb->midi) excmidnl_meb();
                        } else pcaserst_meb();
                        break;
            case '+'  : if(isdigit(mv->card[1])) {
                            if(card[0] == '/' || card[0] == 'R' ||
                            card[0] == 'r') {
                                rept = beaurep_meg(card);
                                ren = mb->renp;
                            }
                            else {
                                rept = beaurep_meg(mv->card);  
                                rept--;
                                if(rept < 0) rept = 0;
                                ren = 0; 
                            }
                            mb->renp = 0;     
                            if(rept > 1) rept = rept - 1; /* multip */
                        }
                        beauasym_meb();
                        beaumset_meb();
                        beauspvl_meb();
                        if(mb->cueing) excsmpnl_meb();
                        if(mb->midi) excmidnl_meb();
                        if(mb->clrpls) mb->prflg = 0;
                        break;
            case ':'  : beausyms_meb();
                        beaumset_meb();
                        /* notesync_meb(); */
                        /* if(mb->cueing) excsmpnl_meb(); */
                        if(mb->clrsml) mb->prflg = 0;
                        break;
            case '['  : beaufras_meb();
                        if(mb->sbdflg && mb->bsbgnp) {
                            /* if(mv->card[77] != '}') mb->prflg = 0; */
                            mb->prflg = 0;
                        }
                        break;
            case '{'  : /* Gloval */
                        if(mb->gchdxf == 1 && mv->card[1] == '?') {
                            mv->card[1] = '!';
                        }
                        if(mb->gchdxf == 2 && mv->card[1] == '!') {
                            mv->card[1] = '?';
                        }
                        if(mv->card[1] == '$') {
                            rf = beaugkey_meb(mk);
                            if(rf) {
                                mb->sync = (400000/mb->time)*mb->ryhm;
                                mb->adsyn = 0;
                            }
                            if(mv->debug == 3) {
                                printf("mb->GKTPf = %d mb->SKTPv = %d\n",
                                mb->GKTPf,mb->SKTPv);
                            }
                            if(!mb->GKTPf) mb->GKTPv = mb->SKTPv;
                            if(mb->GKTPv) keytrnsp_meb(mk); /* meb4.c */
                            if(gsf) beaumput_meb();
                            break;
                        }
                        if(mv->card[1] == '!') {
                            if(mb->clrchd == 1) mb->prflg = 0;
                            if(gsf) beaumput_meb();
                            break;
                        }
                        /* if(mb->bsbgnp && (mv->card[77] != '}') && */
                        mb->bexpsf = beauexp_meb();
                        if(mb->bexpsf) beauexpr_meb(ms);
                        if(mb->lbdflg && mb->bsbgnp) {
                            /* if(mv->card[77] == '}') mb->prflg = 0; */
                            mb->prflg = 0;
                        }
                        if(mv->card[1] == '/') {
                            /* cond's beautify symbol begin point flag on */
                            mb->bsbgnp = 1;
                        }
                        if(gsf) beaumput_meb();
                        break;
            case '>'  : if(mv->card[1] == '{') {
                            beauexp_meb();
                            if(mb->bexpsf) beauexpr_meb(ms);
                            if(mb->lbdflg && mb->bsbgnp) {
                                /* if(mv->card[77] != '}') mb->prflg = 0; */
                                mb->prflg = 0;
                            }
                        }
                        break;
            case '('  : /* beauspace_meb(); */
                        if(mb->pbdflg && mb->bsbgnp) {
                            /* if(mv->card[77] != '}') mb->prflg = 0; */
                            mb->prflg = 0;
                            break;
                        }
                        mv->card[0] = '{';
                        for(i = 0; i < 79; i++) {
                            if(mv->card[i] == ')') {
                                mv->card[i] = '}';
                                break;
                            }
                        }
                        if(mv->card[1] == '$') {
                            rf = beaugkey_meb(mk);
                            if(rf) {
                                mb->sync = (400000/mb->time)*mb->ryhm;
                                mb->adsyn = 0;
                            }
                            lbrtosbr_meb();
                            break;
                        }
                        if(mv->card[1] == '!') {
                            if(mb->clrchd == 1) mb->prflg = 0;
                            break;
                        }
                        beauexp_meb();
                        if(mb->bexpsf) beauexpr_meb(ms);
                        lbrtosbr_meb();
                        /* if(mv->card[1] == 'm') exptmp_meb(); */
                        break;
            case '<'  : /* if(mb->clrsml) mb->prflg = 0; */
                        sexclsum_meb();
                        break;
            case '#'  : strck_meb();
                        if(mb->clrsml) mb->prflg = 0;
                        break;
            case '$'  : if(mv->card[2] == '$') break;
                        if(mv->card[1] == ':' || mv->card[2] == ':') {
                            setlvl_cps();
                        }
                        rf = beaukey_meb(mk);
                        key_meb();
                        setkey_cps();
                        if(rf) mb->sync = (400000/mb->time)*mb->ryhm;
                        mb->adsyn = 0;
                        break;
            case 'C'  : beaupula_meb(inf);
                        break;
            case ' '  : if(mb->clrcom) {
                           if(mv->card[78] == '!' ||
                              mv->card[78] == '>' ||
                              mv->card[78] == '|') break;
                           mb->prflg = 0;
                           break;
                        }
                        if(mv->card[2] == '/' &&
                           mv->card[78] == '|') setbtitl_meb(mv->card,inf);
                        break;
            case 'S'  : if(mv->card[1] == 'm') {
                            smptsval_meb();
                            smptgval_meb();
                            if(mb->clrsml) mb->prflg = 0;
                        }
                        /* 2011 Section -> Sequenc */
                        if(mv->card[1] == 'e') {
                            for(i = 0; i < 55; i++)
                                sectitl[secnt][i] = mv->card[i+14];
                            secnt++;
                            if(secnt > 36) {
                                mv->errcnt++;
                                mv->msglvl = 3;
                                if(mv->msglvl >= mv->prtlvl)
                                printf("Error! Section barnum count %d\n",
                                    secnt);
                                secnt--;
                            }
                        }
                        if(mv->card[1] == 't') { /* Start */
                            mb->bsbgnp = 1;
                        }
                        break;
            case 'T'  : if(mv->card[1] == 'i') {  /* Title ? */
                            if(secnt) for(i = 0; i < 55; i++)
                                mv->card[i+14] = sectitl[secno][i];
                            /* cond's beautify symbol begin point flag on */
                            mb->bsbgnp = 1;
                        }
                        break;
            case '!'  : /* chord */
                        if(mb->clrchd == 1) mb->prflg = 0; 
                        break;
            case '?'  : /* chord select */
                        if(mb->delchk == 1) mb->prflg = 0; 
                        break;
            case '='  : if(mb->mnflg1 == 1) mnflg2 = 1;
                        if(mv->card[1] == '=') {
                            mb->cbnum = mb->bnum; /* add 2011.11 */
                            beaumov_meb();
                            sequchg_meb();
                            smpteset_meb(3);
                        }
                        else {
                            sglbar_meb();
                            mainbar_meb();
                            dblbar_meb();
                            dblbsv_meb();
                        }
                        break;
            case '\"' : if(mv->card[1] == '@') {
                            if(df == 'm' || df == 'c') mv->dped = '@';
                            if(df == 'd' || df == 'r') mb->prflg = 0;
                            if(mb->clrdpl) mb->prflg = 0;
                        }
                        if(mv->card[1] == '*') {
                            if(df == 'm' || df == 'c') mv->dped = '*';
                            if(df == 'd' || df == 'r') mb->prflg = 0;
                            if(mb->clrdpl) mb->prflg = 0;
                        }
                        break;
            case 'd'  : mb->prflg = 0; /* Delete line */
                        if(mv->card[2] == ' ') {
                        if(mv->card[1] == 's' || mv->card[1] == 'S') dlflg = 1;
                        if(mv->card[1] == 'e' || mv->card[1] == 'E') dlflg = 0;
                        }
                        break;
            case 'D'  : mb->prflg = 0;
                        if(mv->card[2] == ' ') {
                        if(mv->card[1] == 's' || mv->card[1] == 'S') dlflg = 1;
                        if(mv->card[1] == 'e' || mv->card[1] == 'E') dlflg = 0;
                        }
                        break;
            case 'f'  : if(mv->card[1] == 'i') { /* final ? */
                            mb->bsbgnp = 0;
                        }
                        break;
            case 0x1a : eoflg = 1;
                        break;     /* ESC */
            default   : break;
        }
        if(dlflg) mb->prflg = 0;
        if(mb->delbarf == 0) {  /* delete bar flag off ? */
            strcpy(card,mv->card);
            // if(mnflg2 && mb->bnum != 1 && mb->ryhm > 0) {
            if(mnflg2 && mb->bnum != mb->ybnum && mb->ryhm > 0) { // 2024.9.4
                for(i = 0; i < 79; i++) mv->card[i] = ' ';
                genloop = mb->ryhm * 4 / mb->time;
                if(mb->gentyp == 1) {
                    genloop = mb->ryhm * 1 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = 'w';
                    mv->card[2] = '\0';
                }
                if(mb->gentyp == 2) {
                    genloop = (mb->ryhm * 4) / (mb->time * 3);
                    mv->card[0] = '/';
                    mv->card[1] = 'h';
                    mv->card[2] = '.';
                    mv->card[3] = '\0';
                }
                if(mb->gentyp == 3) {
                    genloop = mb->ryhm * 2 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = 'h';
                    mv->card[2] = '\0';
                }
                if(mb->gentyp == 4) {
                    genloop = mb->ryhm * 4 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '\0';
                }
                if(mb->gentyp == 8) {
                    genloop = mb->ryhm * 8 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '\0';
                }
                if(mb->gentyp == 16) {
                    genloop = mb->ryhm * 16 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '\0';
                }
                if(mb->gentyp == 32) {
                    genloop = mb->ryhm * 32 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '/';
                    mv->card[4] = '\0';
                }
                if(mb->gentyp == 64) {
                    genloop = mb->ryhm * 64 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '/';
                    mv->card[4] = '/';
                    mv->card[5] = '\0';
                }
                if(mb->gentyp == 128) {
                    genloop = mb->ryhm * 128 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '/';
                    mv->card[4] = '/';
                    mv->card[5] = '/';
                    mv->card[6] = '\0';
                }
                /*
                printf("genloop = %d mb->ryhm = %d mb->time = %d\n",
                    genloop,mb->ryhm,mb->time);
                */
                if(mb->gentyp == 1000) mebrgen_meb(&genloop); /* rest gen */
                else beaubar_meb();
                beausyms_meb();
                beaumset_meb();
                for(i = 0 ; i < genloop ; i++) {
                    if(mb->gentyp == 1000) restsync_meb();
                    else notesync_meb();
                    mprintf_meb(mv->card);
                    if(mv->source == 1 || mv->trace) printf("%s\n", mv->card);
                }
                strcpy(mv->card,card);
            }
            if(mb->prflg) {
                /*
                if(mv->card[0] == '(' && mb->xstolf) {
                    sbrtolbr_meb();
                }
                if(mv->card[0] == '{' && mb->xltosf && 
                    (mv->card[1] != '/') && (mv->card[75] != '/')) {
                    lbrtosbr_meb();
                }
                */
                strcpy(scard[0],mv->card);
                for(k = 0; k < rept; k++) {
                    for(i = 0; i < scdn - 1; i++) {
                        for(j = 0; j < 82; j++) {
                            if(scard[i][j] == '\n' || scard[i][j] == '\r') {
                                scard[i][j] = '\0';
                                break;
                            }
                        }
                        if(rept > 1) {
                            if(scard[i][0] == '%') notesync_meb();
                            if(scard[i][0] == 'R' ||
                                scard[i][0] == 'r') restsync_meb();
                        }
                        mprintf_meb(scard[i]);
                        if(scard[i][0] == '-' && mb->gchdf) {
                            mprintf_meb("{!}");
                            if(mv->source == 1 || mv->trace) printf("{!}\n");
                        }
                        if(scard[i][0] == '-' && mb->gchdsf) {
                            mprintf_meb("{?}");
                            if(mv->source == 1 || mv->trace) printf("{?}\n");
                        }
                        if(mv->source == 1 || mv->trace)
                            printf("%s\n",scard[i]);
                    }
                    if(ren) {
                        j = 4;
                        while(mv->card[j] != ' ') j++;
                        for(; j > 3; j--) mv->card[j] = ' ';
                        mv->card[4] = '[';
                        for(i = 1 ; i < ren ; i++) {
                            mv->line++;
                            mprintf_meb(mv->card);
                            if(mv->source == 1 || mv->trace)
                                printf("%s\n", mv->card);
                            notesync_meb();
                        }
                    }
                }
            }
            if(mb->inclf) incfile_meb();
        }
        if(mb->addbarv) addbar_meb();
        strcpy(mv->befr,mv->card);
        for(i = 0; i < 79; i++) mv->card[i] = ' ';
        strcpy(mv->card,mv->next);
        strcpy(mv->next,mv->next2);
        for(i = 0; i < 82; i++) {
            k = i;
            if(mv->card[k] == '\n' || mv->card[k] == '\r') {
                mv->card[k] = '\0';
                break;
            }
            if(mv->next[k] == '\n' || mv->next[k] == '\r') {
                mv->next[k] = '\0';
                break;
            }
        }
        if(eoflg) break;
    }
    mv->line++;
    mprintf_meb(mv->card);
    if(mv->source == 1 || mv->trace) printf("%s\n",mv->card);
    mv->line++;
    if(mv->next[2] == '$') setbtitl_meb(mv->next,inf);
    mprintf_meb(mv->next);
    if(mv->source == 1 || mv->trace) printf("%s\n",mv->next);
    fclose(fpi);
    if(ls->fmxopend) {
        //fclose(ls->fmx);   // 2025.4.2
    }
    if(mv->line) {
        if(!gsf) {
            remove(outf);
            rename(inf,outf);
            /* rename("mebsp1.tmp",inf); */
            if(mv->debug) printf("mbeautif_meb() // rename %s -> %s //\n",
            inf,outf);
        }
        if(!mv->pass) {
            mv->msglvl = 0;
            if(mv->msglvl >= mv->prtlvl) {
                if(mv->errcnt || mv->wrncnt)
                    printf("Beautify %s Error(%d) worning(%d)\n",
                    inf,mv->errcnt,mv->wrncnt);
                else {
                    printf("Beautify %s (%d) step %d end\n",
                    inf,mv->line,mv->step);
                }
            }
        }
        /* mcopy_meb("mebsp1.tmp"); */
    }
    else {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! input file \"%s\" is no record.\n",inf);
        }
    }
    return(0);
}

/* part beautify */
int pbeautif_meb(char *inf,struct mivksn *mk,struct mivsym ms[])
//char *inf;
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i;
    int  j;
    int  k;
    int  eoflg;
    int  mnflg2;
    int  rtn; /* expandn_meb() return */
    int  ren;
    int  len;
    int  rept;
    int  memb;
    int  track;  /* add 97.10 */
    char outf[STLN];
    char obsf[STLN];
    char *pf;
    char df;
    char card[82];
    FILE *fpi;
    char w[4];
    int  genloop;      /* genarate note lines loop count */
    int  secno;        /* section no. */
    int  ioutf;        /* Inst out flag */
    int  psf;
    int  barpsf;       /* bar pass flag */
    int  nf;
    int  dlflg;        /* Delete line flag */

    startval_meb();
    mb->bexp = 82;     /* {%82}  */
    for(j = 0; j < 2; j++ ) {
        for(i = 0; i < 12 ; i++) mb->trnst[j][i] = wktrnst[j][i];
    }
    i = 0;
    mb->renp = 0;
    eoflg = 0;
    pf = inf;
    while(*pf != '.') {
        outf[i] = *pf;
        i++;
	if(i >= STLN) break;
        pf++;
    }
    outf[i] = '\0';
    pf = inf;
    i = 0;
    while(*pf != '_') {
        pf++;
	i++;
	if(i >= STLN) break;
    }
    pf++;
    /* mb->partp = toupper(*pf); */
    mb->partp = *pf;
    for(i = 0; i < 36; i++) if(mb->partp == prtsym[i]) secno = i;
    /* printf("sequ=%d xbnum=%d\n",mb->sequ,mb->xbnum); */
    mb->sequ = secno;
    mb->xbnum = 0;
    pf++;
    w[0] = *pf;
    pf++;
    w[1] = *pf;
    w[2] = '\0';

/*--------------------------------------------------*/
    sscanf(w,"%x",&memb);
    memb = memb + 1;
    if(memb != mb->memb) mv->mmpos = 0;
    mb->memb = memb;
    if(mv->debug == 3) printf("Debug3! memb=%d mb->memb=%d mv->mmpos=%d\n",
        memb,mb->memb,mv->mmpos);
    /* mb->mbartyp = mx->partf[mb->memb-1][mb->sequ]; */
/*--------------------------------------------------*/

    sscanf(w,"%x",&track);
    for(i = 0; i < MXMB; i++) {
	if(mx->track[i] == track) mb->mbartyp = mx->partf[i][mb->sequ];
    }
    /* printf("mbartyp='%c'\n",mb->mbartyp); */

    if(ls->brself) mv->byps = 0;
    else mv->byps = 1;
    ls->sequ = 0;
    ls->bnum = 0;
    strcat_h(outf,".meb");
    mopen_meb(); /* pbeaut */
    fpi = fopen_h(inf,"r"); /* step 1 must read from file */
    pf = inf;
    i = 0;
    while(*pf != '_') {
        obsf[i] = *pf;
        i++;
        pf++;
    }
    obsf[i] = '\0';
    strcat(obsf,".mix");
    /* printf("obsf = %s\n",obsf); */
    ioutf = 0;
    dlflg = 0;
    for(i = 0; i < 128; i++) ls->obstb[i] = 0;
    nkeyini_meb();          /* natural key init */
    barpsf = 0; /* bar pass flag */
    fgets(mv->card,82,fpi);
    fgets(mv->next,82,fpi);
    if(mv->source == 1 || mv->trace) printf("\n");
    // mv->line = 0;  // 2025.4.2
    while(fgets(mv->next2, 82, fpi)) {
        //if(mv->debug == 10)  printf("%s\n",mv->next2);  // 2025.4.3
        mv->line++;
        mb->prflg = 1;
        mb->bexpsf = 1; /* beautify expression symbol serch on */
        ren = 0;
        mnflg2 = 0;
        rept = 1;
        df = mv->dflg;
        for(i = 0; i < 82; i++) {
            k = i;
            if(mv->card[k] == '\n' || mv->card[k] == '\r') {
                mv->card[k] = '\0';
                break;
            }
        }
        if(mv->next[0] == 'f') eoflg = 1;
        scdn = 2;   /* scard number use at meblib1(rensync) */
        switch (mv->card[0]) {
            case '%'  : if(mv->byps != 1) break;
                        rtn = 0;
                        if(mb->expnl) rtn = expandn_meb(inf);
                        if(!rtn) {
                            if(mb->clrntl) {
                                mb->prflg = 0;
                                break;
                            }
                            if(mb->clrntsf) {
                                clrnotes_meb();
                                /* break; */
                            }
                            chkes_meb();
                            nf = 0;
                            w[0] = mv->card[75];
                            w[1] = mv->card[76];
                            w[2] = mv->card[77];
                            w[3] = mv->card[78];
                            if(w[0] == '_' && w[1] == '.' &&
                               w[2] == '.' && w[3] == '.') nf = 1;
                            if(w[0] == '+' && w[1] != '.') nf = 1;
                            if(w[0] == '-' && w[1] != '.') nf = 1;
                            if(nf) pcaseper_meb(fpi);
                            else notesync_meb(); /* Add 2002.11.2 */
                        }
                        if(mv->card[75] == '_') svexrs_meb(); /* Sv Exp Syb */
                        if(mv->befr[0] == '*') arpeggio_meb();
                        if(mb->dexpcl) cldexp_meb(); /* Reset Dynamic Expand */
                        if(mb->dexpn) setdexs_meb();
                        break;
            case 's'  : if(mv->byps != 1) break;
                        if(mb->clrntl) mb->prflg = 0;
                        break;
            case '/'  : if(mv->byps != 1) break;
                        if(mv->card[1] == '%') {   /* /%/ repeat */
                            len = strlen(mv->card);
                            if(len < 3) {
                                mv->card[2] = '/';
                                mv->card[3] = '\0';
                            }
                            if(mv->card[2] == ' ') mv->card[2] = '/';
                            mb->sync = 0;
                            break;
                        }
                        ren = pcasesh_meb();
                        break;
            case 'O'  : if(mv->byps != 1) break;
                        modcard_meb();
                        ren = pcasesh_meb();
                        break;
            case 'o'  : if(mv->byps != 1) break;
                        modcard_meb();
                        ren = pcasesh_meb();
                        break;
            case '|'  : if(mv->byps != 1) break;
                        modcard_meb();
                        ren = pcasesh_meb();
                        break;
            case '-'  : if(mv->card[2] == '\0' || (mv->card[79] != '\0'))
                        {
                            genbar_meb(mb->mbartyp,mv->card);
                        }
                        if(mv->card[2] == ' ' && (mv->card[60] == ' ')) {
                            genbar_meb(mb->mbartyp,mv->card);
                        }
                        barpsf++;
                        if(mb->mnflg1 == 1) mnflg2 = 1;
                        if(mv->copyf == 0 && (mb->mbartyp != 'x'))
                            mv->card[1] = tolower(mb->mbartyp);
                        pcasebar_meb();
                        if(mv->card[69] != 'v') brepear_meb();
                        mb->bsbgnp = 1;  /* Symcopy Start 97.3 */
                        if(mm->dblbc) dblbchk_meb();
                        break;
            case 'R'  : if(mv->byps != 1) break;
                        pcaserst_meb();
                        break;
            case 'r'  : if(mv->byps != 1) break;
                        pcaserst_meb();
                        break;
            case '['  : if(mv->byps != 1) break;
                        beaufras_meb();
                        if(mb->sbdflg) mb->prflg = 0;
                        break;
            case '{'  : /* Gloval */ 
                        if(mb->gchdxf == 1 && mv->card[1] == '?') {
                            mv->card[1] = '!';
                        }
                        if(mb->gchdxf == 2 && mv->card[1] == '!') {
                            mv->card[1] = '?';
                        }
                        if(mv->card[1] == '!') {
                            if(mb->clrchd == 1) mb->prflg = 0;
                            /* else chord_cps(); */
			    if(mb->bstflg) mb->prflg = 0;
                            break;
                        }
                        if(mv->card[1] == '$' && mv->card[2] == ':') {
                            /* setlvl_cps(); */
			    if(mb->bstflg) mb->prflg = 0;
                            break;
                        } 
                        if(mv->card[1] == '$' && mv->card[3] == ':') {
                            /* setlvl_cps(); */
			    if(mb->bstflg) mb->prflg = 0;
                            break;
                        } 
                        if(mv->card[72] == '/' && mv->card[78] == '>') {
                            /* Exchane old Title to New line */
                            mv->card[0] = '\0';
                            strcpy(mv->card,Title1);
                            strcat(mv->card,Title2);
                            mb->bsbgnp = 1;
                            break;
                        }
                        if(mv->card[1] == 'm' && mv->card[3] != '.') {
			    if(mb->bstflg) mb->prflg = 0;
                            break;
                        }
                        if(mv->card[1] == '$' && mb->GKTPv) {
                            setorgktb_meb(mk); /* set bktb[] */
                            mkotrkey_meb(0);
                            if(mv->debug == 3) {
                                 printf("base key %s\n",mv->card);
                                 bktbdmp_meb(mk);
                            }
                        }
                        else pcasebeu_meb(mk,ms,obsf);
                        if(mv->card[1] == '$' && mv->card[3] != ':') {
			    if(mb->bstflg) {
			        mv->card[1] = '#'; /* for delete */
			        mb->prflg = 0;
			    }
			}
                        /*
                        printf("mb->bstflg=%d mb->prflg=%d\n",mb->bstflg,
			mb->prflg); 
                        */
                        break;
            case '>'  : if(mv->byps != 1) break;
                        if(mv->card[1] == '{') {
                            beauexp_meb();
                            if(mb->bexpsf) beauexpr_meb(ms);
                            if(mb->lbdflg && mb->bsbgnp) {
                                /* if(mv->card[77] != '}') mb->prflg = 0; */
                                mb->prflg = 0;
                            }
                        }
                        break;
            case '('  : /* Local */ 
                        if(mb->pbdflg == 1) {
                            mb->prflg = 0;
                            break;
                        }
                        if(mv->card[1] == '!') {
                            if(mb->clrchd == 1) mb->prflg = 0;
                            /* else chord_cps(); */
                            break;
                        }
                        if(mv->card[1] == '$' && mv->card[2] == ':') {
                            /* setlvl_cps(); */
			    if(mb->bstflg) mb->prflg = 0;
                            break;
                        } 
                        if(mv->card[1] == '$' && mv->card[3] == ':') {
                            /* setlvl_cps(); */
			    if(mb->bstflg) mb->prflg = 0;
                            break;
                        } 
                        pcaselbu_meb(mk,ms);
                        break;
            case '<'  : /* if(mb->clrsml) mb->prflg = 0; */
                        if(mv->byps != 1) break;
                        sexclsum_meb();
                        break;
            case '#'  : if(mv->byps != 1) break;
                        strck_meb();
                        if(mb->clrsml) mb->prflg = 0;
                        break;
            case '$'  : if(mv->card[2] == '$') break;
                        if(mv->card[1] == ':' || mv->card[2] == ':' ||
                            mv->card[3] == ':') {
                            /* setlvl_cps(); */
                            break;
                        }
                        /* mb->prflg = 0; remove key line */
                        break;
            case '.'  : if(mv->byps != 1) break;
                        ren = beauabar_meb();  /* acciaccatura */
                        if(mb->clrntsf) {
                            clrnotes_meb();
                            break;
                        }
                        if(mb->clrontf) clronts_meb();
                        noteshft_meb();
                        beaudpset_meb();
                        beaudymc_meb();
                        beautrns_meb();
                        if( mb->bsyntyp == 'v') beausynt_meb();
                        if(mb->bartyp == 'r') beaurtmv_meb();
                        notesync_meb();
                        break;
            case ','  : if(mv->byps != 1) break;
                        ren = beaunbar_meb();  /* nachschlag */
                        if(mb->clrntsf) {
                            clrnotes_meb();
                            break;
                        }
                        if(mb->clrontf) clronts_meb();
                        noteshft_meb();
                        beaudpset_meb();
                        beaudymc_meb();
                        beautrns_meb();
                        if( mb->bsyntyp == 'v') beausynt_meb();
                        if(mb->bartyp == 'r') beaurtmv_meb();
                        /* notesync_meb(); */
                        break;
            case '+'  : if(mv->byps != 1) break;
                        if(isdigit(mv->card[1])) {
                            if(card[0] == '/' || card[0] == 'R' ||
                            card[0] == 'r') {
                                rept = beaurep_meg(card);
                                ren = mb->renp; // chk 2013.10.20
                            }
                            else {
                                rept = beaurep_meg(mv->card);
                                rept--; 
                                if(rept < 0) rept = 0;
                                ren = 0;
                            }
                            // if(rept > 1) rept = rept - 1;  out 2013.10.20
                            mb->renp = 0;
                        }
                        if(mb->clrntsf) {
                            clrnotes_meb();
                            break;
                        }
                        if(mb->clrontf) clronts_meb();
                        noteshft_meb();
                        beauadd_meb();
                        bedymst_meb();
                        beaudymc_meb();
                        beautrns_meb();
                        if( mb->bsyntyp == 'v') beausynt_meb();
                        if(mb->bartyp == 'r') beaurtmv_meb();
                        if(mv->next[0] == '%' && mv->next[4] == '.' &&
                            mv->next[5] == '/' && mv->next[6] == ')')
                            mv->card[4] = '.';
                        if(mb->clrpls) mb->prflg = 0;
                        break;
            case '*'  : if(mv->byps != 1) break;
                        dynaexp_meb();
                        break;
            case ':'  : if(mv->byps != 1) break;
                        beauadd_meb();
                        if(mb->clrntsf) {
                            clrnotes_meb();
                            break;
                        }
                        if(mb->clrontf) clronts_meb();
                        noteshft_meb();
                        beaudymc_meb();
                        beautrns_meb();
                        if( mb->bsyntyp == 'v') beausynt_meb();
                        if(mb->bartyp == 'r') beaurtmv_meb();
                        if(mb->clrsml) mb->prflg = 0;
                        break;
            case '\"' : if(mv->card[1] == '@') {
                            if(df == 'm' || df == 'c') mv->dped = '@';
                            if(df == 'd' || df == 'r') mb->prflg = 0;
                            if(mb->clrdpl) mb->prflg = 0;
                        }
                        if(mv->card[1] == '*') {
                            if(df == 'm' || df == 'c') mv->dped = '*';
                            if(df == 'd' || df == 'r') mb->prflg = 0;
                            if(mb->clrdpl) mb->prflg = 0;
                        }
                        break;
            case 'T'  : if(mv->card[1] == 'i') { /* Title ? */
                            if(mv->debug)
                            printf("Test++ secnt=%d secno=%d\n",secnt,secno);
                            if(secnt-1 == secno) for(i = 0; i < 55; i++)
                                mv->card[i+14] = sectitl[secno][i];
                            /* cond's beautify symbol begin point flag on */
                            mb->bsbgnp = 1;
                        }
                        break;
            case 'S'  : if(mv->card[1] == 't') { /* Start ? */
                            mb->bsbgnp = 1;
                        }
                        break;
            case '@'  : /* out @inst line to fn.mit */
                        if(!ioutf) instout_meb(inf,0);
                        ioutf = 1;
                        getinstv_meb(ms);
                        break;
            case ';'  : /* Set Expand Symbol */
                        if(mb->delexp == 1) mb->prflg = 0;
                        else setexp_meb(inf);
                        break;
            case '!'  : /* chord */
                        if(mb->clrchd == 1) mb->prflg = 0;
                        /* else chord_cps(); */
                        break;
            case '?'  : /* chord select */
                        if(mb->delchk == 1) mb->prflg = 0;
                        break;
            case '='  : if(mb->mnflg1 == 1) mnflg2 = 1;
                        if(mv->card[1] == '=') {
                            beaumov_meb();
                            sequchg_meb();
                        }
                        else {
                            if(mv->copyf == 0 && (mb->mbartyp != 'x'))
                                mv->card[1] = tolower(mb->mbartyp);
                            sglbar_meb();
                            pcasebar_meb();
                            dblbar_meb();
                            if(mm->dblbc) sglbchk_meb();
                        }
                        break;
            case ' '  : if(mb->clrcom) {
                            if(mv->card[78] == '!' ||
                               mv->card[78] == '>' ||
                               mv->card[78] == '|') break;
                            mb->prflg = 0;
                            break;
                        }
                        if(mv->card[2] == '/' &&
                            mv->card[78] == '|') setbtitl_meb(mv->card,inf);
                        break;
            case 'd'  : mb->prflg = 0; /* Delete line */
                        if(mv->card[2] == ' ') {
                        if(mv->card[1] == 's' || mv->card[1] == 'S') dlflg = 1;
                        if(mv->card[1] == 'e' || mv->card[1] == 'E') dlflg = 0;
                        }
                        break;
            case 'D'  : mb->prflg = 0;
                        if(mv->card[2] == ' ') {
                        if(mv->card[1] == 's' || mv->card[1] == 'S') dlflg = 1;
                        if(mv->card[1] == 'e' || mv->card[1] == 'E') dlflg = 0;
                        }
                        break;
            case 'f'  : if(mv->card[1] == 'i') { /* final ? */
                            mb->bsbgnp = 0;
                        }
                        break;
            case 0x1a : eoflg = 1;
                        break;     /* ESC */
            default   : break;
        }
        if(dlflg) mb->prflg = 0;
        if(mb->delbarf == 0) {
            strcpy(card,mv->card);
            /* if(mnflg2 && mb->bnum != 1 */
            if(mnflg2 && barpsf > 1
                && mb->ryhm > 0 && mv->byps == 1) {
                for(i = 0; i < 79; i++) mv->card[i] = ' ';
                genloop = mb->ryhm * 4 / mb->time;
                if(mb->gentyp == 1) {
                    genloop = mb->ryhm * 1 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = 'w';
                    mv->card[2] = '\0';
                }
                if(mb->gentyp == 2) {
                    genloop = (mb->ryhm * 4) / (mb->time * 3);
                    mv->card[0] = '/';
                    mv->card[1] = 'h';
                    mv->card[2] = '.';
                    mv->card[3] = '\0';
                }
                if(mb->gentyp == 3) {
                    genloop = mb->ryhm * 2 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = 'h';
                    mv->card[2] = '\0';
                }
                if(mb->gentyp == 4) {
                    mv->card[0] = '/';
                    mv->card[1] = '\0';
                }
                if(mb->gentyp == 8) {
                    genloop = mb->ryhm * 8 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '\0';
                }
                if(mb->gentyp == 16) {
                    genloop = mb->ryhm * 16 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '\0';
                }
                if(mb->gentyp == 32) {
                    genloop = mb->ryhm * 32 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '/';
                    mv->card[4] = '\0';
                }
                if(mb->gentyp == 64) {
                    genloop = mb->ryhm * 64 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '/';
                    mv->card[4] = '/';
                    mv->card[5] = '\0';
                }
                if(mb->gentyp == 128) {
                    genloop = mb->ryhm * 128 / mb->time;
                    mv->card[0] = '/';
                    mv->card[1] = '/';
                    mv->card[2] = '/';
                    mv->card[3] = '/';
                    mv->card[4] = '/';
                    mv->card[5] = '/';
                    mv->card[6] = '\0';
                }
                /*
                printf("genloop = %d mb->ryhm = %d mb->time = %d\n",
                    genloop,mb->ryhm,mb->time);
                */
                if(mb->gentyp == 1000) {
                    mebrgen_meb(&genloop); /* rest gen */
                    beauadd_meb();
                }
                else {
                    beaubar_meb();
                    beaudpset_meb();
                }
                beaudymc_meb();
                for(i = 0 ; i < genloop ; i++) {
                    if( mb->bsyntyp == 'v') beausynt_meb();
                    if(mb->bartyp == 'r') beaurtmv_meb();
                    if(mb->gentyp == 1000) restsync_meb();
                    else notesync_meb();
                    mprintf_meb(mv->card);
                    if(mv->source == 1 || mv->trace) printf("%s\n", mv->card);
                }
                strcpy(mv->card,card);
            }
            if(mb->prflg) {
                if(mv->byps == 1) {
                    if(mv->card[0] == '(' && mb->xstolf) sbrtolbr_meb();
                    if(mv->card[0] == '{' && mb->xltosf && 
                        (mv->card[1] != '/') && (mv->card[75] != '/')) {
                        lbrtosbr_meb();
                    }
                }
                strcpy(scard[0],mv->card);
                for(k = 0; k < rept; k++) {
                    for(i = 0; i < scdn - 1; i++) {
                        for(j = 0; j < 82; j++) {
                            if(scard[i][j] == '\n' || scard[i][j] == '\r') {
                                scard[i][j] = '\0';
                                break;
                            }
                        }
                        if(rept > 1) {
                            if(scard[i][0] == '%') notesync_meb();
                            if(scard[i][0] == 'R' ||
                                scard[i][0] == 'r') restsync_meb();
                        }
                        mprintf_meb(scard[i]);
                        if(mv->source == 1 || mv->trace)
                            printf("%s\n",scard[i]);
                    }
                    if(ren) {
                        j = 4;
                        while(mv->card[j] != ' ') j++;
                        for(; j > 3; j--) mv->card[j] = ' ';
                        mv->card[4] = '[';
                        for(i = 1 ; i < ren ; i++) {
                            mv->line++;
                            mprintf_meb(mv->card);
                            if(mv->source == 1 || mv->trace)
                                printf("%s\n", mv->card);
                            notesync_meb();
                        }
                    }
                }
            }
            /*   2011 */
            /*
            printf("mb->bstflg=%d mb->bsbgnp=%d mb->renumf=%d\n",
               mb->bstflg,mb->bsbgnp,mb->renumf);
            */
            if(mb->bstflg && mb->bsbgnp == 1) {
                /* {!Chord} line ? */
                if(mv->debug == 10) printf("->%s\n",mm->msmb[mv->mmpos]);
                psf = 0;
                if(mm->msmb[mv->mmpos][1] == '!') {
                    if(mv->next[0] == '%') {
                        addgsym_meb(inf,mk,ms,obsf);
                        psf = 1;
                    }
                }
                if(!psf) addgsym_meb(inf,mk,ms,obsf);
            }
            if(mb->inclf) incfile_meb();
        }
        if(mb->addbarv) addbar_meb();
        strcpy(mv->befr,mv->card);
        for(i = 0; i < 79; i++) mv->card[i] = ' ';
        strcpy(mv->card,mv->next);
        strcpy(mv->next,mv->next2);
        for(i = 0; i < 82; i++) {
            k = i;
            if(mv->card[k] == '\n' || mv->card[k] == '\r') {
                mv->card[k] = '\0';
                break;
            }
            if(mv->next[k] == '\n' || mv->next[k] == '\r') {
                mv->next[k] = '\0';
                break;
            }
            if(mv->next2[k] == '\n' || mv->next2[k] == '\r') {
                mv->next2[k] = '\0';
                break;
            }
        }
        if(eoflg) break;
    }
    if(!ioutf) instout_meb(inf,1); 
    mv->line++;
    mprintf_meb(mv->card);
    if(mv->source == 1 || mv->trace) printf("%s\n",mv->card);
    mv->line++;
    if(mv->next[2] == '$') setbtitl_meb(mv->next,inf);
    mprintf_meb(mv->next);
    if(mv->source == 1 || mv->trace) printf("%s\n",mv->next);
    /* comment out 2025.4.2 core dump why ?
    if(ls->fmxopend) {
        //fclose(ls->fmx);  // 2025.4.2
        ls->fmxopend = 0;
    }
    */
    if(mb->expstf) {
        /* printf("Store expand files to fn.mph\n"); */
        /* printf("ph->exrc = %d\n",ph->exrc); */
        for(i = 0; i < ph->exrc; i++) {
            /* printf("ph->exrs[%d] = %s\n",i,ph->exrs[i]); */
            storexp_meb(inf,ph->exrs[i]);
        }
    }   
    if(mb->exprsf) {
        /* printf("Register expand files to meb.mph\n"); */
        /* printf("ph->exrc = %d\n",ph->exrc); */
        for(i = 0; i < ph->exrc; i++) {
            /* printf("ph->exrs[%d] = %s\n",i,ph->exrs[i]); */
            regstxp_meb(inf,ph->exrs[i]);
        }
    }
    if(mv->line) {
        remove(outf);
        rename(inf,outf); /* fn_xyz.meg -> fn_xyz.meb */
        if(mv->debug) printf("pbeautif_meb() // rename %s -> %s //\n",
            inf,outf);
        /* mcopy_meb("mebsp1.tmp"); */
        if(!mv->pass) {
            mv->msglvl = 0;
            if(mv->msglvl >= mv->prtlvl) {
                if(mv->errcnt || mv->wrncnt)
                    printf("Beautify %s Error(%d) worning(%d)\n",
                    inf,mv->errcnt,mv->wrncnt);
                else {
                    printf("Beautify %s (%d) step %d end\n",
                    inf, mv->line,mv->step);
                }
            }
        }
    }
    else {
        if(!mv->pass) {
           mv->errcnt++;
           mv->msglvl = 3;
           if(mv->msglvl >= mv->prtlvl)
           printf("Error! input file \"%s\" is no record.\n",inf);
        }
    }
    return(0);
}

/* add 2011.11 Continuous_renumber */
int SetRenumStartVal_meb()
{
    int  i;
    char wk1[82];
    char wk2[82];

    if(!mv->mfflg) return(9);
    if(!mb->crenumf) return(9);
    if(mv->debug) {
        printf("SetRenumStartVal() mv->step=%d mb->sequ=%d mb->xbnum=%d ",
            mv->step,mb->sequ,mb->xbnum);
        printf("mb->ybnum=%d ",mb->ybnum);
        printf("mb->bnum=%d mb->dblbc=%d\n",
            mb->bnum,mm->dblbc);
    }
    if(mv->step == 1) {
        /* printf("%s\n",mv->card); */
        /* printf("mb->cbnum=%d\n",mb->cbnum); */
        strcpy(wk1,mv->card);
        /* check value ? */
        for( i= 0; i < 70; i++) {
            if(wk1[i-1] == 'u' && wk1[i] == 'e' && wk1[i+1] == ' ') break;
        }
        wk1[++i] = '\0';
        //sprintf(wk2," %d}\n",mb->cbnum+1);
        //if(mb->sequ == 0) sprintf(wk2," %d}\n",mb->cbnum);  // 2024.9.1
        if(mb->sequ == 0) sprintf(wk2," %d}\n",mb->ybnum);  // 2024.9.4
        else sprintf(wk2," %d}\n",mb->cbnum+1);
        strcat(wk1,wk2);
        strcpy(mv->card,wk1);
        if(mv->debug) printf("--> %s",mv->card);
    }
    return(0);
}


