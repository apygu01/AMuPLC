
// ----------------------
//  AMuPLC (meb2.c)
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
/* meb command no operand de '-beautify' mataha -b toshitabaai
   .meg no file wo beautify suru.
*/
extern MIVARG *mv;
extern MIVMST *mm;
extern MIVMXM *mx;
extern MEBVAR *mb;
extern MEBSRG *ls;
char gfile[STLN];  /* fn.meg */
char bfile[STLN];  /* fn.meb */
char tfile[STLN];  /* fn.dat */
FILE *fpt;
char mit[MXMB][82];
char mitq[MXMB][4];
int  mitc;  /* mit counter */
int  mitqc; /* mitq counter */
char prsynpmd[] = "   x xxx xx:xxx ... xx";
char prsyndat[] = "xx.x ._...";
char *xabc = "0123456789abcdef";
/* extern char secsymt[]; */
extern char *prtsym;
extern char mcline[36];

/*
C  01 . S...................................  .S..............    0      2    !
012345678901234567890123456789012345678901234567890123456789012345678901234
@                   000_...  C_.   0_..   1_..    1                           !
*/
int header_meb(char mfile[STLN])
//char mfile[STLN];
{
    int  i;
    FILE *fhg,*fpo;
    char card[82];

    for(i = 0; i < STLN; i++) {
        if(mfile[i] == '_') break;
        gfile[i] = mfile[i];
        bfile[i] = mfile[i];
        tfile[i] = mfile[i];
    }
    gfile[i] = '\0';
    bfile[i] = '\0';
    tfile[i] = '\0';
    strcat(gfile,".meg");
    strcat(bfile,".meb");
    strcat(tfile,".dat");
    /* printf("gfile = %s\n",gfile); */
    if(mv->debug) printf("header_meb() mfile=%s bfile=%s\n",mfile,bfile);
    if((fhg = fopen(gfile,"r")) == NULL) return(1);
    if(mv->beaut != -1) if((fpo = fopen_h(bfile,"w")) == NULL) return(1);
    mv->hf = 1;  /* set fn.meg read flag */
    mitc = 0;
    mitqc = 0;
    while(fgets(card,82,fhg)) {  /* Modify check */
        if(card[0] == '@') {
            strcpy(mit[mitc],card); 
            mitc++;
            if(mitc > MXMB) {
                printf("Error! @line over than MXMB(%d) at header()\n",MXMB);
                exit(0);
            }
        }
        /* printf("%s",card); */
        if(mv->beaut != -1) fprintf(fpo,"%s",card);
    }
    fclose(fhg);
    if(mv->beaut != -1) fclose(fpo);
    /* printf("mb->slsf = %d\n",mb->slsf); */
    if(mb->slsf) {
        /* initval_meb();    init variables */
        return(0); /* Skip meb fn/xyy */
    }
    if(mv->beaut != -1) modmit_meb();
    for(i = 0; i < mitc; i++) { /* Modify Check */
        strcpy(card,mit[i]);
        if(card[24] != '.' || card[25] != '.' || card[26] != '.') {
             // chgfn_meb(card); // No suport 2019.5.5
        }
        if(card[31] != '.') {
             card[29] = card[31];
             card[31] = '.';
        }
        if(card[37] != '.' || card[38] != '.') {
             if(card[37] == '.' || card[38] == '.') {
                 card[34] = ' ';
                 if(card[37] != '.') card[35] = card[37];
                 if(card[38] != '.') card[35] = card[38];
             }
             else {
                 card[34] = card[37];
                 card[35] = card[38];
             }
             card[37] = '.';
             card[38] = '.';
        }
        if(card[44] != '.' || card[45] != '.') {
             if(card[44] == '.' || card[45] == '.') {
                 card[41] = ' ';
                 if(card[44] != '.') card[42] = card[44];
                 if(card[45] != '.') card[42] = card[45];
             }
             else {
                 card[41] = card[44];
                 card[42] = card[45];
             }
             card[44] = '.';
             card[45] = '.';
        }
        /* printf("%s",card); */
        strcpy(mit[i],card);
    }
    /*  No suport 2019.5.5
    for(i = 0; i < mitc; i++) { // Modify Check
        strcpy(card,mit[i]);
        if(card[24] != '.' || card[25] != '.' || card[26] != '.') {
             cpfn_meb(card);
        }
    }
    */
    putcline_meb();
    mitc = 0;

    return(0);
}

/* mod mit[] by Cline */
int modmit_meb()
{
    int  i,j,k;    
    FILE *fpm;
    char card[82];
    char wk[4];

    fpm = fopen_h(gfile,"r");
    while(fgets(card, 82, fpm)) {
        /* Cline Check */
        if(card[0] == 'C') {
            for(i = 0; i < mitc; i++) {
                if(card[3] == mit[i][21] && card[4] == mit[i][22]) {
                    for(j = 0; j < 36; j++) {
                        if(mit[i][20] == prtsym[j]) {
                            mit[i][29] = card[j + 8];
                            /* printf("card[j+8] = %c\n",card[j+8]); */
                            break;
                        }
                    }
                    /* Chan */
                    j = 46;
                    for(k = 0; k < 16; k++) {
                        if(card[j + k] == 'S') {
                            sprintf(wk,"%2d",k+1);
                            mit[i][41] = wk[0];
                            mit[i][42] = wk[1];
                            /* printf("modmit() chan = %s\n",wk); */
                            break;
                        }
                    }
                    /* Port */
                    mit[i][34] = card[65];
                    mit[i][35] = card[66];
                }
            }
        }
    }
    fclose(fpm);
    return(0);
}

int putcline_meb()
{
    int  i,j,k;
    FILE *fpg,*fpw;
    char card[82];
    char wk[4];
    int  mf;

    /* for(i = 0 ; i < mitc; i++) printf("%s",mit[i]);  Check mit[] Table */
    fpw = fopen_h("mebtmp.bak","w");
    fpg = fopen_h(gfile,"r");
    while(fgets(card, 82, fpg)) {
        /* Cline Check & Skip Conducter */
        /* if(card[0] == 'C' && (card[3] != '0' || card[4] != '0')) { */
        if(card[0] == 'C') { 
            /* if(card[4] != ' ') for(i = 46; i < 62; i++) card[i] = '.'; */
            for(i = 0; i < mitqc; i++) {
                if(card[3] == mitq[i][1] && card[4] == mitq[i][2]) {
                    /* Clear Seq */
                    for(j = 0; j < 36; j++) {
                        if(mitq[i][0] == prtsym[j]) {
                            /*
                            if(isalpha(card[j + 8]))
                                card[j + 8] = tolower(mit[i][29]);
                            */
                            card[j + 8] = '.';
                            break;
                        }
                    }
                }
            }
            for(i = 0; i < mitc; i++) {
                if(card[3] == mit[i][21] && card[4] == mit[i][22]) {
                    for(j = 0; j < 36; j++) {
                        if(mit[i][20] == prtsym[j]) {
                            card[j + 8] = mit[i][29];
                            break;
                        }
                    }
                }
            }
            mf = 0;
            for(i = 0; i < mitc; i++) {
                if(card[3] == mit[i][25] && card[4] == mit[i][26]) {
                    mf = 1;
                    card[3] = mit[i][25]; 
                    card[4] = mit[i][26]; 

                    mit[i][20] = mit[i][24];
                    mit[i][21] = mit[i][25];
                    mit[i][22] = mit[i][26];
                    mit[i][24] = '.';
                    mit[i][25] = '.';
                    mit[i][26] = '.';
                    break;
                }
            }
            /* Comment 2000.2.27 */
            /*
            if(!mf) {
                if(card[4] != ' ') {
                    for(i = 0; i < 16; i++) {
                        if(card[4] == xabc[i]) card[i + 46] = 'S';
                    }
                }
            }
            printf("%s",card);
            */
        }
        fprintf(fpw,"%s",card);
    }
    fclose(fpg);
    fclose(fpw);
    rename("mebtmp.bak",gfile);  /* mebtmp.bak -> fn.meg */
    fpw = fopen_h("mebtmp.bak","w");
    fpg = fopen_h(gfile,"r");
    while(fgets(card, 82, fpg)) {
        if(card[0] == 'C') { 
            mf = 0;
            /* if(card[4] != ' ') for(i = 46; i < 62; i++) card[i] = '.'; */
            for(i = 0; i < mitc; i++) {
                if(card[3] == mit[i][21] && card[4] == mit[i][22]) {
                    for(k = 46; k < 62; k++) card[k] = '.';
                    mf = 1;
                    /* Seq */
                    for(j = 0; j < 36; j++) if(mit[i][20] == prtsym[j]) {
                        if(isalpha(card[j + 8])) card[j + 8]  = mit[i][29];
                        if(card[j + 8] == '.') card[j + 8]  = '?'; 
                    } 
                    /* Chan */
                    wk[0] = mit[i][41];
                    wk[1] = mit[i][42];
                    wk[2] = '\0';
                    sscanf(wk,"%d",&j); /* j : Chan */
                    if(j < 1) j = 1;
                    if(j > 16) j = 16;
                    j = j - 1; /* Ch. 1 - 16 */
                    /* printf("wk = %s chan = %d\n",wk,j); */
                    card[j + 46] = 'S';
                    /* Port */
                    card[65] = mit[i][34];
                    card[66] = mit[i][35];
                    /*
                    printf("%s",mit[i]);
                    printf("%s",card);
                    */
                    break;
                }
            }
            /*
            if(!mf) {
                if(card[4] != ' ') {
                    for(i = 0; i < 16; i++) {
                        if(card[4] == xabc[i]) card[i + 46] = 'S'; 
                    }
                }
            }
            */
        }
        /* printf("%s",card); */
        fprintf(fpw,"%s",card);
    }
    fclose(fpg);
    fclose(fpw);
    rename("mebtmp.bak",gfile);  /* mebtmp.bak -> fn.meg */
    return(0);
}
 
int chgfn_meb(char card[82])
//char card[82];
{
    int  i;
    char orgf[STLN];
    char newf[STLN];

    if(mv->debug) printf("chgfn_meb() card=%s\n",card);
    for(i = 0; i < STLN; i++) {
        orgf[i] = gfile[i];
        if(gfile[i] == '.') break;
    }
    orgf[i++] = '_';
    orgf[i++] = card[20];
    orgf[i++] = card[21];
    orgf[i++] = card[22];
    orgf[i++] = '\0';
    strcat(orgf,".meg");

    for(i = 0; i < STLN; i++) {
        newf[i] = gfile[i];
        if(gfile[i] == '.') break;
    }
    newf[i++] = '_';
    newf[i++] = card[24];
    newf[i++] = card[25];
    newf[i++] = card[26];
    newf[i++] = '\0';
    strcat(newf,".meb");

    if(mv->debug) printf("chgfn_meb() card=%s orgf = %s newf = %s\n",
        card,orgf,newf);
    /* printf("Pass 1 rename orgf = %s newf = %s\n",orgf,newf); */
    rename(orgf,newf);
    mitq[mitqc][0] = card[20];
    mitq[mitqc][1] = card[21];
    mitq[mitqc][2] = card[22];
    mitq[mitqc][3] = '\0';
    /* printf("mitq[%d] = %s\n",mitqc,mitq[mitqc]); */
    mv->msglvl = 1;
    if(mv->msglvl >= mv->prtlvl) {
       printf("File %s was renamed to %s\n",orgf,newf);
    }
    mitqc++;
    if(mitqc > MXMB) {
        printf("Error! mitq[MXMB] table over than MXMB(%d)\n",MXMB);
        exit(0);
    }
    return(0);
}

int cpfn_meb(char card[82])
//char card[82];
{
    int  i;
    char orgf[STLN];
    char newf[STLN];
    FILE *fpg,*fpw;

    if(mv->debug) printf("cpfn_meb() card=%s\n",card);
    for(i = 0; i < STLN; i++) {
        orgf[i] = gfile[i];
        if(gfile[i] == '.') break;
    }
    orgf[i++] = '_';
    orgf[i++] = card[24];
    orgf[i++] = card[25];
    orgf[i++] = card[26];
    orgf[i++] = '\0';
    strcat(orgf,".meb");

    for(i = 0; i < STLN; i++) {
        newf[i] = gfile[i];
        if(gfile[i] == '.') break;
    }
    newf[i++] = '_';
    newf[i++] = card[24];
    newf[i++] = card[25];
    newf[i++] = card[26];
    newf[i++] = '\0';
    strcat(newf,".meg");
    if(mv->debug) printf("cpfn_meb() card=%s orgf = %s newf = %s\n",
        card,orgf,newf);
    /* printf("Pass 2 copy orgf = %s newf = %s\n",orgf,newf); */
    fpw = fopen_h(newf,"w");
    fpg = fopen_h(orgf,"r"); 
    while(fgets(card, 82, fpg)) { /* Copy */
        fputs(card,fpw);
    }
    fclose(fpg);
    fclose(fpw);
    mv->msglvl = 1;
    if(mv->msglvl >= mv->prtlvl) {
       printf("File %s was copyed to %s\n",orgf,newf);
    }
    return(0);
}

int instout_meb(char *inf,int sf)
//char *inf;
//int  sf;
{
    int  i,j,k;
    FILE *fpg;
    char mitw[82];
    char wk[21];
    char cm[26];
    int  pf,ppf;
    char psc[8];
    char btyp;
    char btp[8];
    int  chan;
    char chn[8];
    int  port;
    char prt[8];
    int  memb;
    char mem[8];
    char ccm[26];
    char pcg[4]; /* add 2007.12 */
    char scrf[100];

    if(mv->debug == 4) printf("Debug4! instout_meb() inf=%s sf=%d\n",inf,sf);
    for(i = 0; i < 26; i++) ccm[i] = ' ';
    ccm[0] = '\0';
    wk[0] = '\0';
    if(!sf) {
        pf = 0;
        ppf = 0;
        for(i = 0; i < 21; i++) {
            if(mv->card[i] == '\0') pf = 1;
            if(!pf) {
                if(mv->card[i] == ' ') if(!ppf) ppf = i;
                if(!ppf) wk[i] = mv->card[i];
                else wk[i] = ' ';
            }
            else wk[i] = ' '; 
        }
        wk[20] = '\0';
        if(ppf != 0) {
            for(k = 0,i = ppf; k < 26; k++,i++) {
                ccm[k] = mv->card[i];
            }
            ccm[k] = '\0';
        }
    }
    else {
        for(i = 0; i < 21; i++) {
            wk[i] = ' ';
        }
        wk[0] = '@';
        wk[20] = '\0';
        cm[0] = '\0';
    }

    i = 0;
    while(inf[i] != '_') i++;
    psc[0] = inf[i+1];
    psc[1] = inf[i+2];
    psc[2] = inf[i+3];
    psc[3] = '\0';
    getcline_meb(psc,&btyp,&chan,&port,&memb); /* Read fn.meg & check cline */
    if(mv->debug == 4) printf("Debug4! btyp = %c chan=%d port=%d memb=%d\n",
        btyp,chan,port,memb);

    cm[0] = '\0';
    if((fpg = fopen(tfile,"r")) == NULL) {
        if((fpg = fopen("meb.dat","r")) == NULL) {
            sprintf(scrf,"%s",TACTDIR);
            //sprintf(scrf,"%s\0",TACTDIR);
            strcat(scrf,"meb.dat");
            if(mv->debug == 4) printf("Debug4! scrf = %s\n",scrf);
            if((fpg = fopen(scrf,"r")) == NULL) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl) {
                    printf("Error! instout_() Can't open %s mode r\n",scrf);
                }
                return(-1);
            }
        }
    }
    pcg[0] = ' ';
    pcg[1] = ' ';
    pcg[2] = ' ';
    pcg[3] = ' ';
    getdat_meb(fpg,port,chan,wk,cm,pcg);
    fclose(fpg);
    for(i = 0; i < 26; i++) {
        if(cm[i] == '\0') break;
    }
    /* Commet out 2009.3.14 
    j = 0;
    for(k = i; k < 26; k++) {
        cm[k] = ccm[j];
        j++;
    }
    */
    cm[25] = '\0';
    if(mv->debug == 4) printf("Debug4! wk=%s cm=%s ccm=%s pcg=%s\n",
        wk,cm,ccm,pcg);
    strcpy(mitw,wk);
    strcat(psc,"_..."); // 2019.05.05 Check 
    /* printf("psc = %s\n",psc); */
    strcat(mitw,psc);

    btp[0] = btyp;
    btp[1] = '\0';
    strcat(btp,"_.");
    strcat(mitw,"  ");
    strcat(mitw,btp);

    sprintf(prt,"%2d",port);
    strcat(prt,"_..");
    strcat(mitw,"  ");
    strcat(mitw,prt);

    sprintf(chn,"%2d",chan);
    strcat(chn,"_..");
    strcat(mitw,"  ");
    strcat(mitw,chn);
     
    sprintf(mem,"%3d",memb);
    strcat(mitw,"  ");
    strcat(mitw,mem);

    strcat(mitw,"  ");
    strcat(mitw,cm);
    for(k = 46; k < 78; k++) {
        if(mitw[k] == '\n') break;
        if(mitw[k] == '\r') break;
        if(mitw[k] == '\0') break;
    }
    for(;k < 78; k++) mitw[k] = ' ';
    mitw[78] = '!';
    mitw[79] = '\0';

    /* add PC# 2007.12 */
    mitw[64] = pcg[0];
    mitw[65] = pcg[1];
    mitw[66] = pcg[2];

    /* add ccm 2009.3.14 */
    i = 0;
    for(j = 67; j < 77; j++) {
        mitw[j] = ccm[i];
        if(mitw[j] == '\0') mitw[j] = ' '; 
        if(mitw[j] == '\r') mitw[j] = ' '; 
        if(mitw[j] == '\n') mitw[j] = ' '; 
        i++;
    }
    strcpy(mit[mitc],mitw);
    mitc++;
    if(mitc > MXMB) {
        printf("Error! @line over than MXMB(%d)\n",MXMB);
        exit(0);
    }
    return(0);
}

/* Read fn.dat */
int getdat_meb(FILE *fpg,int port,int chan,char wk[],char cm[],char pcg[])
//FILE *fpg;
//int  port;
//int  chan;
//char wk[21];
//char cm[26];
//char pcg[4];
{
    int  i,j,k;
    char card[82];
    char ww[4];
    int  tport;
    int  tchan;

    while(fgets(card, 82, fpg)) {
        if(card[0] == '@') {
            ww[0] = card[29]; /* Port */
            ww[1] = card[30];
            ww[2] = '\0';
            sscanf(ww,"%d",&tport);
            ww[0] = card[38];  /* Chan */
            ww[1] = card[39];
            ww[2] = '\0';
            sscanf(ww,"%d",&tchan);
            if(port == tport && chan == tchan) {
                if(mv->debug == 4)
                   printf("Debug4! getdat_meb() Port = %d Chan = %d\n",
                   port,chan);
                /* strcat wk */
                for(i = 0; i < 8; i++) {
                    if(wk[i] == ' ') break;
                    if(wk[i] == '\0') break;
                }
                wk[i++] = '(';
                for(j = i,k = 1; j < 17; j++,k++) {
                    if(card[k] == ' ') break;
                    wk[j] = card[k];
                }
                wk[j++] = ')';
                for(; j < 20; j++) wk[j] = ' ';
                wk[20] = '\0';
                /* printf("wk = %s\n",wk); */

                /* strcat cm */
                for(j = 0,k = 14; j < 25; j++,k++) {
                    cm[j] = card[k];
                    if(card[k] == ' ') break;
                }
                cm[++j] = '\0';
                /* cm[25] = '\0'; */
                for(j = 0,k = 46; k < 49; j++,k++) {
                    pcg[j] = card[k];
                }
                pcg[j] = '\0';
                /* printf("pcg=%s\n",pcg); */
            }
        }
    }
    return(0);
}

int mcline_meb()
{
    int  i;
    FILE *fpg,*fpw;

    /* startval_meb();    set start value */
    fpw = fopen_h("mebtmp.bak","w");
    fpg = fopen_h(gfile,"r");
    /* if(mv->source == 1 || mv->trace) printf("\n"); */
    mv->line = 0;
    while(fgets(mv->card, 82, fpg)) {
        mv->line++;
        switch(mv->card[0]) {
            case 'C'  : beaucl_meb();
                        break;
            case '{'  : if(mb->slsf) break; // 98.6 skip beauexp_meb()
                        beauexp_meb();  
                        break;
            case '>'  : if(mb->slsf) break;  // 98.6 
                        if(mv->card[1] == '{') {
                            beauexp_meb(); 
                        }
                        break;
            default   : break;
        }
        ///if(mv->source == 1 || mv->trace) printf("%s",mv->card);
        fprintf(fpw,"%s",mv->card);
    }
    fclose(fpg);
    fclose(fpw);
    mb->bclall = 0;
    /* rename(gfile,bfile); fn.meb */
    rename("mebtmp.bak",gfile);  /* mebtmp.bak -> fn.meg */
    return(0);
}

int beaucl_meb()
{
    FILE *fic;
    int  i,j;
    char wkc[4];
    char xfile[STLN];
    int  l;
    char mc;
    char wcard[82];

    if(mv->card[0] == 'C' && mv->card[4] == ' ') {
        if(mv->card[6] == '?') mb->bclall = 1;   /* all file check on */
        /* printf("mb->bclall = %d\n",mb->bclall); */
        mv->card[6] = '.';
        for(i = 8 ; i < 44; i++) mv->card[i] = '.';
    }
    else {
        strcpy(xfile,gfile);
        for(i = 8,j = 0; i < 44 ; i++,j++) {
            //if(mv->debug == 16) printf("mb->cline=%d\n",mb->cline);
            mv->card[i] = mx->partf[mb->cline][j];
        }
        mv->card[6] = '.';
        i = 8;
        for(j = 0 ; j < 36; j++) {
            mc = mv->card[i];
            if(mb->cline == 0) mcline[j] = mc;
            if(mc == '/') mv->card[i] = '.';
            if(mb->bclall == 1 || mc == '&' || mc == '*' || mc == '@'
                || mc == '?' || mc == '+') {
                /*
                printf("*(prtsym+j) = %c mb->cline = %d\n",
                    *(prtsym+j),mb->cline);
                */
                /*
                l = 0;
                while(xfile[l++] != '.');
                */
                l = 0;
                while(xfile[l] != '.' && xfile[l] != '_') l++;
                l++;
                sprintf(wkc,"%02x",mx->track[mb->cline]);
                xfile[l-1] = '_';
                xfile[l] = *(prtsym+j);
                xfile[l+1] = wkc[0];
                xfile[l+2] = wkc[1];
                xfile[l+3] = '\0';
                strcat(xfile,".meg");
                //printf("xfile = %s\n",xfile);
                //if((fic = fopen_h(xfile,"r")) != NULL) { 
                if((fic = fopen(xfile,"r")) != NULL) {
                    while(fgets(wcard, 82, fic)) {
                        if(wcard[2] == '$') break;
                        if(wcard[2] == '_') break;
                        if(wcard[1] == '/' && wcard[78] == '|') break;
                    }
                    /*
                    if(!isalpha(mv->card[i])) mv->card[i] = tolower(wcard[4]);
                    mv->card[i] = wcard[4];
                    */ 
                    if(mb->bclall == 1) {
                        if(!isalpha(mv->card[i])) {
                            if(!islower(mv->card[i]))
                                mv->card[i] = tolower(wcard[4]);
                        }
                    }
                    else mv->card[i] = wcard[4];
                    /*
                    if(mc == '?') {
                        if(!isalpha(mv->card[i]))
                            mv->card[i] = tolower(wcard[4]);
                    }
                    else mv->card[i] = wcard[4];
                    */
                    fclose(fic);
                }
                else mv->card[i] = '.';
            }
            i++;
        }
        mb->cline++;
	/* renumber member */
        sprintf(wkc,"%3d",mb->cline);
        mv->card[71] = wkc[0];
        mv->card[72] = wkc[1];
        mv->card[73] = wkc[2];
    }
    return(0);
}

int getcline_meb(char *fn,char *btyp,int *chan,int *port,int *memb)
//char *fn;
//char *btyp;
//int  *chan;
//int  *port;
//int  *memb;
{
    FILE *fpg;
    char card[82];
    int af;
    char c1,c2;
    int  k;
    char wk[4];

    if(mv->debug == 4) printf("getcline_() fn = %s\n",fn);
    af = 0;
    fpg = fopen_h(gfile,"r");
    while(fgets(card, 82, fpg)) {
        if(card[0] == 'C') {
            c1 = card[3]; /* port */
            c2 = card[4]; /* channel */
            if(c1 == fn[1] && c2 == fn[2]) {
                af = 1;
                break;
            }
        }
    }
    if(af) {
        if(mv->debug == 4) printf("%s",card);
        for(k = 0; k < 36; k++) {
            if(prtsym[k] == fn[0]) break;
        }
        k = k + 8;
        *btyp = card[k];
        if(mv->debug == 4) printf("btyp = %c ",*btyp);
        if(mv->debug == 16) printf("btyp = %c ",*btyp);  // 2025.4.1

        for(k = 0; k < 16; k++) {
            if(card[k+46] == 'S') *chan = k + 1; /* Channel : 1-16 */
        }
        if(mv->debug == 4) printf("chan = %d ",*chan);

        wk[0] = card[65];
        wk[1] = card[66];
        wk[2] = '\0';
        sscanf(wk,"%d",port);
        if(mv->debug == 4) printf("port = %d ",*port);

        wk[0] = card[71];
        wk[1] = card[72];
        wk[2] = card[73];
        wk[3] = '\0';
        sscanf(wk,"%d",memb);
        if(mv->debug == 4) printf("memb = %d\n",*memb);
    }
    fclose(fpg);
    return(0);
}

int bottom_meb()
{
    int  i;
    int  df;
    FILE *fpg,*fpw;
    char card[82];
    
    if(mb->slsf) return(0);
    fpw = fopen_h("mebtmp.bak","w");
    fpg = fopen_h(gfile,"r");
    mv->line = 0;
    if(mv->source == 1 || mv->trace) printf("\n");
    if(mv->source == 1 || mv->trace) {
        printf(" ////////////////////////////////////////");
        printf("/////////////////////////////////////!\n");
    }
    fprintf(fpw," ////////////////////////////////////////");
    fprintf(fpw,"/////////////////////////////////////!\n");
    mv->line++;
    if(mv->source == 1 || mv->trace) {
        printf(" =====Move & Change File,Bar,Port,Chan(1-16)=======");
        printf("                           !\n");
    }
    fprintf(fpw," =====Move & Change File,Bar,Port,Chan(1-16)=======");
    fprintf(fpw,"                           !\n");
    mv->line++;
    if(mv->source == 1 || mv->trace) {
        printf(" Instruments        File     Bar   Port  Chan  Memb  ");
        printf("Device     PC# @inst cm. !\n");
    }   
    fprintf(fpw," Instruments        File     Bar   Port  Chan  Memb  ");
    fprintf(fpw,"Device     PC# @inst cm. !\n");
    mv->line++;
    for(i = 0; i < mitc; i++) {
        if(mv->source == 1 || mv->trace) printf("%s\n",mit[i]);
        fprintf(fpw,"%s\n",mit[i]);
        mv->line++;
    }
    /* printf("mitc = %d\n",mitc); */
    if(mv->source == 1 || mv->trace) {
        printf(" ==================================");
        printf("Read instruments from meb.dat(fn.dat) file !\n");
    }
    fprintf(fpw," ==================================");
    fprintf(fpw,"Read instruments from meb.dat(fn.dat) file !\n");
    mv->line++;
    df = 0;
    while(fgets(card, 82, fpg)) {
        if(card[0] == '@') df = 1;
        if(card[1] == 'I' && card[2] == 'n' && card[3] == 's') df = 1;
        if(card[1] == '=' && card[2] == '=' && card[3] == '=') df = 1;
        if(card[1] == '/' && card[2] == '/' && card[3] == '/') df = 1;
        if(!df) {
            mv->line++;
            if(mv->source == 1 || mv->trace) printf("%s",card);
            fprintf(fpw,"%s",card);
        }
        df = 0;
    }
    if(mv->source == 1 || mv->trace) {
        printf(" ////////////////////////////////////////");
        printf("/////////////////////////////////////!\n");
    }
    fprintf(fpw," ////////////////////////////////////////");
    fprintf(fpw,"/////////////////////////////////////!\n");
    mv->line++;
    fclose(fpg);
    fclose(fpw);
    remove(gfile);
    rename("mebtmp.bak",gfile);  /* mebtmp.bak -> fn.meg */
    mv->msglvl = 2;
    if(mv->msglvl >= mv->prtlvl) {
        printf("Beautify %s %d bars, %d lines end\n",
        gfile,mb->bnum,mv->line);
    }
    return(0);
}

int beaudpset_meb()
{
    char bwk[4];
    int  i;
    int  rtn;
    char *ps;

    mv->card[0] = '%';
    mv->card[3] = ' ';
    /* beaumod_meb(); */
    if(mb->bsyntyp == 'v') {
        if((mv->card[21] != ':') && (mv->card[28] != '.')) {
            ps = prsynpmd;
            for(i = 10; i < 32; i++) {
                if(mv->card[i] == ' ' && i > 12) mv->card[i] = *ps;
                ps++;
            }
        }
    }
    if(mv->card[74] == ' ') {
        ps = prsyndat;
        for(i = 69; i < 80; i++) {
            mv->card[i] = *ps;
            ps++;
        }
    }

/* turboc bub test
    printf("TEST **** mv->dflg = %c mv->dpos = %d mv->dped = %c\n",
     mv->dflg,mv->dpos,mv->dped);
*/

    if(mv->dflg == 'm')
        if(mv->card[mv->dpos] == '.') mv->card[mv->dpos] = mv->dped;
    if(mv->dflg == 'c') mv->card[mv->dpos] = mv->dped;
    if(mv->dped == '@') mv->dped = ':';
    if(mv->dped == '*') mv->dped = '.';
    if(mv->dflg == 'r') mv->card[mv->dpos] = '.';
    mv->card[79] = '\0';
    return(0);
}

int keysgn_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    int  i; 
    int  rtn;
    int  tabfl;
  
    i = 3;
    tabfl = 0;
    while(mv->card[i]) {
        if(mv->card[i] == 'T' && mv->card[i+1] == 'r') tabfl = 1;
        i++;
        if(i > 79) break;
    }
    if(tabfl) mb->prflg = 0;
    /* set base key */
    /*
    if(tabfl) mb->ptrnsv = numdinmb_meb(mv->card);
    else for(i = 7; i < 78; i++) mv->bktb[i] = mv->ktb[i];
    */
    if(mv->card[0] == '{') beaugkey_meb(mk);
    else {
        if(mv->card[1] == ':' || mv->card[2] == ':') {
            setlvl_cps();    
        }       
        beaukey_meb(mk);
        key_meb();
        setkey_cps();
    }
    mb->adsyn = 0;
    return(0);
}

/* large bra. to small bracket */
int lbrtosbr_meb()
{
    int  k;

    if(mv->card[0] == '{') mv->card[0] = '(';
    for(k = 80; k > 0; k--) {
        if(mv->card[k] == '}') {
            mv->card[k] = ')';
            break;
        }
    }
    return(0);
}

/* small bra. to large bracket */
int sbrtolbr_meb()
{
    int  k;

    if(mv->card[0] == '(') mv->card[0] = '{';
    for(k = 80; k > 0; k--) {
        if(mv->card[k] == ')') {
            mv->card[k] = '}';
            break;
        }
    }
    return(0);
}

int mebfchk_meb(char mfile[STLN],char partc)
//char mfile[STLN];
//char partc;
{
    if(partc == 'C') mebmain(mfile);
    if(partc == 'L') mebmain(mfile);
    if(partc == 'M') mebmain(mfile);
    if(partc == 'H') mebmain(mfile);
    if(partc == 'R') mebmain(mfile);
    if(partc == 'D') mebmain(mfile);
    if(partc == 'T') mebmain(mfile);
    if(partc == 'S') mebmain(mfile);
    if(partc == 'G') mebmain(mfile);
    if(partc == 'A') mebmain(mfile);
    if(partc == 'B') mebmain(mfile);
    if(partc == 'U') mebmain(mfile);
    if(partc == 'X') mebmain(mfile);
    if(mv->allf) {
        if(partc == 'c') mebmain(mfile);
        if(partc == 'l') mebmain(mfile);
        if(partc == 'm') mebmain(mfile);
        if(partc == 'h') mebmain(mfile);
        if(partc == 'r') mebmain(mfile);
        if(partc == 'd') mebmain(mfile);
        if(partc == 't') mebmain(mfile);
        if(partc == 's') mebmain(mfile);
        if(partc == 'g') mebmain(mfile);
        if(partc == 'a') mebmain(mfile);
        if(partc == 'b') mebmain(mfile);
        if(partc == 'u') mebmain(mfile);
        if(partc == 'x') mebmain(mfile);
        if(partc == '@') mebmain(mfile);
        if(partc == '*') mebmain(mfile);
        if(partc == '+') mebmain(mfile);
        if(partc == '&') mebmain(mfile);
    }
    return(0);
}

/* check (expression) line by symbol table
      0 -  100 : reserve
    100 -  200 : dynamic
    200 -  300 : expression
    300 -  500 : tempo
    500 -  999 : reserve
   1000 - 9999 : instruments
*/
int beauexpr_meb(struct mivsym ms[])
//struct mivsym ms[];
{
    int  i,j,k;
    int  exchg;
    int  pas;
    float symv1;
    float symv2;
    float symv3;
    char symv4[8];
    int  same;
    int  unsame;
    char swk[80];
    char swc[80];
    char wc[4];

    if(mv->card[1] == '%') return(0);  /*  gate_rate */
    if(mv->card[1] == '\\') return(0); /*  kadenz  */
    if(mv->card[1] == '*') return(0);  /*  comment */
    if(mv->card[1] == '!') return(0);  /*  chord prog. */
    if(mv->card[1] == '?') return(0);  /*  chord select */
    if(mv->card[1] == '#') return(0);  /*  modify & assign & define */
    if(mv->card[1] == '&') return(0);  /*  define */
    wc[0] = tolower(mv->card[1]);
    wc[1] = tolower(mv->card[2]);
    wc[2] = tolower(mv->card[3]);
    wc[3] = '\0';
    if(!strcmp(wc,"lag")) return(0); /* lag */
    if(!strcmp(wc,"off")) return(0); /* offset */
    j = 0;
    exchg = 0;
    while(mv->card[j]) {
        if(mv->card[j] == '?') exchg++;
        j++;
        if(j > 79) break;
    }
    if(exchg == 1) {
        i = 0;
        while(mv->card[i] != '}') {
            i++;
            if( i >= j) goto excgpass;
        }
        while(mv->card[i] != '{') {
            i++;
            if( i >= j) goto excgpass;
        }
        k = 0;
        while(mv->card[i] != '}') {
            swk[k++] = mv->card[i];
            i++;
            if(i >= j) goto excgpass;
        }
        swk[k++] = '}';
        swk[k] = '\0';
        strcpy(mv->card,swk);
    }
    excgpass:
    i = 0;
    j = 0;
    while(mv->card[i] != '}') {
        if(mv->card[i] != ' ') {
            swc[j] = tolower(mv->card[i]);
            j++;
        }
        i++;
        if(i >= 30) break;
        if(j >= 19) break;
    }
    swc[j++] = '}';
    swc[j] = '\0';
    pas = 0;
    for(i = 0; i < mv->symmax ; i++) {
        j = 0;
        k = 0;
        while(ms[i].symsgn[j] != '}') {
            if(ms[i].symsgn[j] != ' ') {
                swk[k] = tolower(ms[i].symsgn[j]);
                k++;
            }
            j++;
            if(j >= 30) break;
            if(k >= 19) break;
        }
        swk[k++] = '}';
        swk[k] = '\0';
        if(!strcmp(swc,swk)) {
            if(mv->debug == 4) printf("Debug4! Same swc = %s swk = %s\n",
                swc,swk);
            mb->egrp = ms[i].symtyp;
            symv1 = ms[i].symv1;
            symv2 = ms[i].symv2;
            symv3 = ms[i].symv3;
            symv4[0] = ms[i].symv4[0];
            symv4[1] = ms[i].symv4[1];
            symv4[2] = ms[i].symv4[2];
            symv4[3] = ms[i].symv4[3];
            symv4[5] = '\0';
            pas = 1;
        }
    }
    if(!pas) {
        for(i = 0; i < mv->symmax ; i++) {
            j = 0;
            k = 0;
            while(ms[i].symsgn[j] != '}') {
                if(ms[i].symsgn[j] != ' ') {
                    swk[k] = tolower(ms[i].symsgn[j]);
                    k++;
                }
                j++;
                if(j >= 30) break;
                if(k >= 19) break;
            }
            swk[k++] = '}';
            swk[k] = '\0';
            if(!strcmp(swc,swk)) {
                same = 0;
                unsame = 0;
                j = 0;
                while(swk[j]) {
                    if(swc[j] != swk[j]) unsame++;
                    else same++;
                    j++;
                }
                if(same > 4 && unsame < 3 ) {
                    for(j = 0; j < 79; j++)
                        if(mv->card[j] == '}') break;
                    mv->card[++j] = ' ';
                    mv->card[++j] = '-';
                    mv->card[++j] = '>';
                    mv->card[++j] = ' ';
                    k = 0;
                    while(ms[i].symsgn[k]) {
                        j++;
                        mv->card[j] = ms[i].symsgn[k];
                        k++;
                        if(k >= 20) break;
                    }
                    mv->card[++j] = '?';
                    mv->card[++j] = '\0';
                    mb->egrp = ms[i].symtyp;
                    symv1 = ms[i].symv1;
                    symv2 = ms[i].symv2;
                    symv3 = ms[i].symv3;
                    symv4[0] = ms[i].symv4[0];
                    symv4[1] = ms[i].symv4[1];
                    symv4[2] = ms[i].symv4[2];
                    symv4[3] = ms[i].symv4[3];
                    symv4[5] = '\0';
                    pas = 2;
                }
            }
        }
    }
    if(mv->debug == 4) {
        printf("Debug4! mb->egrp=%d symv1=%f symv2=%f symv3=%f",
        mb->egrp,symv1,symv2,symv3);
        printf(" pas=%d\n",pas);
    }
    if(!pas) {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! %05d symbol not found \"%s\"\n",
            mv->line,mv->card);
        }
        return(-1);
    }
    if(mb->egrp < 10) {  /* dynamic sym. */
        mv->velo = symv1;
        mv->mdymc = symv2;
        return(0);
    }
    if(mb->egrp >= 100 && mb->egrp < 200) {  /* tempo sym. */
        mv->stemp = symv2 * 10;
        /*
        if(!mv->stemp) {
            mv->mtemp = mv->stemp;
        }
        */
        return(0);
    }
    return(0);
}

/* Get instrument value by symbol table
   @ line
   1000 - 9999 : instruments
*/
int getinstv_meb(struct mivsym ms[])
//struct mivsym ms[];
{
    int  i,j,k;
    int  exchg;
    int  pas;
    float symv1;
    float symv2;
    float symv3;
    char symv4[8];
    int  same;
    int  unsame;
    char swk[80];
    char swc[80];
    char wc[4];

    i = 1;
    j = 0;
    while(1) {
        if(mv->card[i] == '\0') break;
        if(mv->card[i] == '}') break;
        if(mv->card[i] == ' ') break;
        swc[j] = tolower(mv->card[i]);
        j++;
        /*
        if(mv->card[i] != ' ') {
            swc[j] = tolower(mv->card[i]);
            j++;
        }
        */
        i++;
        if(i >= 30) break;
        if(j >= 19) break;
    }
    swc[j] = '\0';
    if(mv->debug == 4) printf("Debug4! swc = %s mv->symmax=%d\n",
        swc,mv->symmax);
    pas = 0;
    for(i = 0; i < mv->symmax ; i++) {
        j = 1;
        k = 0;
        while(ms[i].symsgn[j] != '}') {
            if(ms[i].symsgn[j] != ' ') {
                swk[k] = tolower(ms[i].symsgn[j]);
                k++;
            }
            j++;
            if(j >= 30) break;
            if(k >= 19) break;
        }
        swk[k] = '\0';
        /* printf("swc=%s swk=%s\n",swc,swk); */
        if(!strcmp(swc,swk)) {
            if(mv->debug == 4) printf("Debug4! Same swc = %s swk = %s\n",
                swc,swk);
            mb->egrp = ms[i].symtyp;
            symv1 = ms[i].symv1;
            symv2 = ms[i].symv2;
            symv3 = ms[i].symv3;
            symv4[0] = ms[i].symv4[0];
            symv4[1] = ms[i].symv4[1];
            symv4[2] = ms[i].symv4[2];
            symv4[3] = ms[i].symv4[3];
            symv4[5] = '\0';
            pas = 1;
        }
    }
    if(!pas) {
        if(!mv->pass) {
            mv->wrncnt++;
            mv->msglvl = 0;
            if(mv->msglvl >= mv->prtlvl) {
                printf("Worning! %s {} Instrument not found",mv->card);
                printf(" in fn.dat or mebrdf.dat\n");
            }
        }
        return(-1);
    }
    if(mb->egrp >= 1000 && mb->egrp < 10000) {  /* instruments sym. */
        mb->lowip = symv1;
        mb->higip = symv2;
        mb->ktrnsv = symv3;
        for(i = 0; i < 4; i++) if(symv4[i] == 'x') symv4[i] = ' ';
        sscanf(symv4,"%d",&mb->knsftv);
        if(mv->debug == 4)
            printf("Bebug4! mb->ktrnsv=%d mb->knsftv = %d\n",
            mb->ktrnsv,mb->knsftv);
        if(pas == 1) {
            /*
            i = 0;
            while(mv->card[i] != '}') {
                if(i > 20) break;
                i++;
            }
            mv->card[++i] = '\0';
            mktrkey_meb();
            */
            mb->binstf = mb->sequ;
        }
    }
    return(0);
}

/* bar line change to each instruments
   ex. {v.n} -m010001-____ -> -m010001-....
       {vn.} -> [Vn.]      transpose = 0
*/
int barinst_meb()
{
    int  i,j,k;
    int  keyp;

    k = 5;  /* mb->bartyp == 'm' */
    j = 11;
    if(mb->bartyp == 'l') k = 0;
    if(mb->bartyp == 'h') k = 16;
    if(mb->bartyp == 't') {
        k = 35;
        j = 32;
    }
    if(mb->bartyp == 'g') {
        k = 28;
        j = 32;
    }
    if(mb->bartyp == 's') {
        k = 21;
        j = 32;
    }
    if(mb->bartyp == 'a') {
        k = 14;
        j = 32;
    }
    if(mb->bartyp == 'b') {
        k = 7;
        j = 32;
    }
    if(mb->bartyp == 'u') {
        k = 0;
        j = 32;
    }
    for(i = j; i < 69; i++,k++) {
        keyp = mv->ktb[k];
        if(mv->card[i] == '.') mv->card[i] = '_';
        if(keyp < mb->lowip || keyp > mb->higip)
            if(mv->card[i] == '_') mv->card[i] = '.';
    }
    /*
    printf("mb->lowip=%d mb->higip=%d\n",mb->lowip,mb->higip);
    printf("%s",mv->card);
    */
    return(0);
}

int beauspace_meb(char card[82])
//char card[82];
{
    int  i,k;
    int  wfg;
    char wk[80];

    wfg = 0;
    k = 0;
    for(i = 0; i < 79; i++) {
        if(card[i] != ' ') {
            wk[k++] = card[i];
            wfg = 1;
        }
        else {
            if(wfg) wk[k++] = ' ';
            wfg = 0;
        }
    }
    wk[k] = '\0';
    strcpy(card,wk);
    return(0);
}

int mgetsym_meb(char *inf,struct mivksn *mk,struct mivsym ms[])
//char *inf;
//struct mivksn *mk;
//struct mivsym ms[];
{
    char tmpf[STLN];
    int  i;
    int  p;
    int  source;
    int  trace;
    int  debug;
    int  prtlvl;

    source = mv->source; /* save source flag */
    trace = mv->trace;   /* save trace flag */
    debug = mv->debug;   /* save debug flag */
    prtlvl = mv->prtlvl; /* save print level */
    /*
    mv->source = 0;
    mv->trace = 0;
    mv->debug = 0;
    */
    mv->prtlvl = 9;     /* message supress */
    i = 0;
    while(inf[i] != '_') {
        tmpf[i] = inf[i];
        i++;
    }
    tmpf[i] = inf[i];
    i++;
    tmpf[i] = inf[i];
    p = i;
    i++;
    tmpf[i] = '\0';
    strcat(tmpf,"00.meg");
    if(mb->slsf == 2) {
        for(i = 0; i < 36; i++) {
            if(mcline[i] != '.') {
                tmpf[p] = prtsym[i];
                printf("inf=%s tmpf=%s\n",inf,tmpf);
                mbeautif_meb(tmpf,mk,ms,1);
            }
        }
    }
    else mbeautif_meb(tmpf,mk,ms,1);
    mv->errcnt = 0;      /* reset error count */
    mv->wrncnt = 0;      /* reset worning count */
    mv->line = 0;        /* reset line count */
    mv->getsym++;
    mv->source = source; /* back source flag */
    mv->trace = trace;   /* back trace flag */
    mv->debug = debug;   /* back debug flag */
    mv->prtlvl = prtlvl; /* back print level */
    if(mv->debug == 3) printf("mgetsym() inf=%s tmpf=%s\n",inf,tmpf);
    return(0);
}

int selfil_meb(char *mfile)
//char *mfile;
{
    int  i;
    int  j;
    int  k;
    char c;
    unsigned char c1,c2;
    int  cx1,cx2;
    int  cline;
    char chw[8];
    FILE *fpg;

    cline = 0;
    for(i = 0; i < MXMB; i++) {
	mx->track[i] = 0;
        mx->port[i] = 0;
        mx->channel[i] = 0;
        mx->member[i] = 0;
        for( j = 0; j < 36; j++) mx->partf[i][j] = '.';
        mx->mmvcp[i] = '.';
    }
    for( j = 0; j < 36; j++) mx->tmvcp[j] = '.';
    if(!mv->hf) {
        fpg = fopen_h(mfile,"r");
        //printf("mfile = %s\n",mfile);
    }
    else {
        fpg = fopen_h(gfile,"r");  // fn.meg 
        //printf("gfile = %s\n",gfile);
    }
    if(fpg == 0) return(0);
    mv->line = 0;

    while(!feof(fpg)) {
        fgets(mv->card, 82, fpg);
        mv->line++;
        c = mv->card[0];
        if(c == 'C' && mv->card[4] == ' ') {
            mx->gmvcp = mv->card[6];
            // mv->card[6] = '.'; 
            for(i = 8,k = 0; k < 36; k++,i++) mx->tmvcp[k] = mv->card[i];
        }
        if(c == 'C' && (mv->card[4] != ' ')) {
            //
            //i = 0;
            //while(mv->card[i++]) if(isdigit(mv->card[i])) break;
            //while(mv->card[i++]) if(mv->card[i] == ' ') break;
            //while(mv->card[i] == ' ') i++;
            //
            c1 = mv->card[3]; 
            c2 = mv->card[4]; 
            for(i = 0; i < 16; i++) {
                if(c1 == xabc[i]) cx1 = i;
                if(c2 == xabc[i]) cx2 = i;
            }
            mx->track[cline] = cx1 * 16 + cx2;
            // printf("mx->track[%d] = %d\n",cline,mx->track[cline]);
            if(mx->track[cline] > MXMB) {   /* 97 V_1.26 */
                if(!mv->pass) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                    printf("Error! Track over than %d\n",MXMB);
                }
                fclose(fpg);
                return(1);
            }
            mx->mmvcp[cline] = mv->card[6];
            i = 8;
            for(k = 0; k < 36; k++) {
                mx->partf[cline][k] = mv->card[i];
                /* printf("mx->partf[%d][%d]=%c\n",
                       cline,k,mx->partf[cline][k]); */
                i++;
            }
            while(mv->card[i] == ' ') i++;
            for(k = 0; k < 16; k++) {
                if(mv->card[i] == 's') mx->channel[cline] = k;
                if(mv->card[i] == 'S') mx->channel[cline] = k;
                i++;
            }
            while(mv->card[i] == ' ') i++;
            j = 0;
            while(isdigit(mv->card[i++])) {
                chw[j++] = mv->card[i-1];
                if(j >= 7) break;
            }
            chw[j] = '\0';
            sscanf(chw,"%d",&mx->port[cline]);
            while(mv->card[i] == ' ') i++;
            j = 0;
            while(isdigit(mv->card[i++])) {
                chw[j++] = mv->card[i-1];
                if(j >= 7) break;
            }
            chw[j] = '\0';
            sscanf(chw,"%d",&mx->member[cline]);
            cline++;
            if(cline > MXMB) { 
                if(!mv->pass) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                    printf("Error! cline over than MXMB %d\n",MXMB);
                }
                fclose(fpg);
                return(1);
            }
        }
        if(c == '_') break;
        if(c == 'f') break;
        if(c == ESC) break;
    }
    fclose(fpg);
    return(cline);
}

int mebcopy_meb(char *ofile,char *sfile)
//char *ofile;
//char *sfile;
{
    FILE *fpg,*fpo;
    //char card[82];
    char card[128];

    /*  Comment out 2007.3.25
    fpg = fopen_h(sfile,"rb");
    */
    if((fpg = fopen(sfile,"rb")) == NULL) return(-1);
    fpo = fopen_h(ofile,"wb");
    while(fgets(card,128,fpg)) if(card[78] != '!') fputs(card,fpo);
    fclose(fpg);
    fclose(fpo);
    return(0);
}

int rename_meb(char *sfile,char *dfile)   
//char *sfile;
//char *dfile;
{
    int  m;
                
    m = 0;
    while(sfile[m]) {
        if(sfile[m] == '.') break;
        m++;
    }
    m += 3;
    sfile[m] = 'g';
    if(!rename(sfile,dfile)) printf("Rename %s to %s\n",sfile,dfile);
    sfile[m] = 'c';
    if(!rename(sfile,dfile)) printf("Rename %s to %s\n",sfile,dfile);
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

/* generate rest line */
int mebrgen_meb(int *genloop)
//int  *genloop;
{
    *genloop = 1;
    if(mb->time == 2) {
        switch (mb->ryhm) {
            case  1 : mv->card[0] = 'R';
                      mv->card[1] = 'h';
                      mv->card[2] = '\0';
                      break;
            case  2 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      break;
            case  3 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '.';
                      mv->card[3] = '\0';
                      break;
            case  4 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 2;
                      break;
            case  6 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 3;
                      break;
            case  8 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 4;
                      break;
            case 10 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 5;
                      break;
            case 12 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 6;
                      break;
            default : break;
        }
    }
    if(mb->time == 4) {
        switch (mb->ryhm) {
            case  1 : mv->card[0] = 'R';
                      mv->card[1] = '\0';
                      break;
            case  2 : mv->card[0] = 'R';
                      mv->card[1] = 'h';
                      mv->card[2] = '\0';
                      break;
            case  3 : mv->card[0] = 'R';
                      mv->card[1] = 'h';
                      mv->card[2] = '.';
                      mv->card[3] = '\0';
                      break;
            case  4 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      break;
            /*
            case  5 : mv->card[0] = 'R';
                      mv->card[1] = '\0';
                      *genloop = 5;
                      break;
            */
            case  6 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '.';
                      mv->card[3] = '\0';
                      break;
            case  8 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 2;
                      break;
            case 12 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 3;
                      break;
            case 16 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 4;
                      break;
            default : break;
        }
    }
    if(mb->time == 8) {
        switch (mb->ryhm) {
            case  1 : mv->card[0] = 'r';
                      mv->card[1] = '\0';
                      break;
            case  2 : mv->card[0] = 'R';
                      mv->card[1] = '\0';
                      break;
            case  3 : mv->card[0] = 'R';
                      mv->card[1] = '.';
                      mv->card[2] = '\0';
                      break;
            case  4 : mv->card[0] = 'R';
                      mv->card[1] = 'h';
                      mv->card[2] = '\0';
                      break;
            /*
            case  5 : mv->card[0] = 'r';
                      mv->card[1] = '\0';
                      *genloop = 5;
                      break;
            */
            case  6 : mv->card[0] = 'R';
                      mv->card[1] = 'h';
                      mv->card[2] = '.';
                      mv->card[3] = '\0';
                      break;
            /*
            case  7 : mv->card[0] = 'r';
                      mv->card[1] = '\0';
                      *genloop = 7;
                      break;
            */
            case  8 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      break;
            case 12 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '.';
                      mv->card[3] = '\0';
                      break;
            case 16 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 2;
                      break;
            case 24 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 3;
                      break;
            case 32 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 4;
                      break;
            default : break;
        }
    }
    if(mb->time == 16) {
        switch (mb->ryhm) {
            case  1 : mv->card[0] = 'r';
                      mv->card[1] = 'r';
                      mv->card[2] = '\0';
                      break;
            case  2 : mv->card[0] = 'r';
                      mv->card[1] = '\0';
                      break;
            case  3 : mv->card[0] = 'r';
                      mv->card[1] = '.';
                      mv->card[2] = '\0';
                      break;
            case  4 : mv->card[0] = 'R';
                      mv->card[1] = '\0';
                      break;
            /*
            case  5 : mv->card[0] = 'R';
                      mv->card[1] = '\0';
                      *genloop = 5;
                      break;
            */
            case  6 : mv->card[0] = 'R';
                      mv->card[1] = '.';
                      mv->card[2] = '\0';
                      break;
            case  8 : mv->card[0] = 'R';
                      mv->card[1] = 'h';
                      mv->card[2] = '\0';
                      break;
            case 12 : mv->card[0] = 'R';
                      mv->card[1] = 'h';
                      mv->card[2] = '.';
                      mv->card[3] = '\0';
                      break;
            case 16 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      break;
            case 24 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '.';
                      mv->card[3] = '\0';
                      break;
            case 32 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 2;
                      break;
            case 64 : mv->card[0] = 'R';
                      mv->card[1] = 'w';
                      mv->card[2] = '\0';
                      *genloop = 3;
                      break;
            default : break;
        }
    }
    /* printf("%s mb->time=%d mb->ryhm=%d\n",mv->card,mb->time,mb->ryhm); */
    return(0);
}

/* system exclusive check sum 
   ex.
   <00 30 22 RS 7f> RS(rs) : Roland type sum
*/
int sexclsum_meb()
{
    int  i,j,k;
    char w[4];
    int  x;
    int  sum;
    int  l;

    i = 1;
    j = 1;
    k = 0;
    sum = 0;
    while(mv->card[i]) {
        if(mv->card[i] == '>') break;
        if(mv->card[i] == 'r') break;
        if(mv->card[i] == 'R') break;
        if(mv->card[i] != ' ') {
            w[k] = mv->card[i];
            if((k = j % 2) == 0 && j > 0) {
                w[2] = '\0';
                sscanf(w,"%x",&x);
                sum = sum + x;
                /* printf("sum=%d x=%d \n",sum,x); */
            }
            j++;
        }
        i++;
    }
    if((mv->card[i] == 'r' || mv->card[i] == 'R') && 
        (mv->card[i+1] == 's' || mv->card[i+1] == 'S')) {
        j = sum & 0x007f;
        k = ~j & 0x007f;
        k++;
        l = sum + k;
        if(mv->debug) {
            printf("Debug! sum : %d hex(%04x)\n",sum,sum);
            printf("Debug! chksum : %d hex(%04x)\n",k,k);
            printf("Debug! sum + chksum : %d + %d = %d  hex(%04x)\n",
                sum,k,l,l);
        }
        sprintf(w,"%02x",k);
        mv->card[i] = w[0];
        mv->card[i+1] = w[1];
    }
    return(0);
}

FILE *fopen_h(char *file, char *mode)
//char *file;
//char *mode;
{
    FILE *fp;

    if((fp = fopen(file, mode)) == NULL) {
        printf("Error! fopen_h() Can't open file %s mode %s\n",
            file, mode);
        exit(0);
    }
    return(fp);
}
