
// ----------------------
//  AMuPLC (meb3.c)
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
char exsym[] = "P_Exp";

extern MIVARG *mv;
extern MEBVAR *mb;
extern MEBSRG *ls;
extern MEBEDT *ge;
extern MEBCPS *cp;
extern char scard[MSTC][82];
extern int scdn;

/* main case '-' line */
int mainbar_meb()
{
    mb->shiftv = 0;
    if(mv->step == 1) sequchg_meb();
    barsync_meb();
    mb->bartyp = mv->card[1];
    bsyntyp_meb();
    if(mb->midi) excmidbr_meb();
    if(mb->cueing) excsmpbr_meb();
    if(mv->card[1] == 'c') {
        if(mv->card[12] == '0') mb->smpmid = 1; /* s */
        if(mv->card[12] == 'p') mb->smpmid = 2; /* q */
    }
    else mb->smpmid = 3;   /* % */
    barcheck_meb();  /* check bar type & exchange */
    if(mb->binstf == mb->sequ) barinst_meb();
    beaupos_meb();
    if(mb->delbarv > 0) {
        mb->delbarf = 1;
        mb->delbarv--;
    }
    else mb->delbarf = 0;
    return(0);
}

/* part case '-' */
int pcasebar_meb()
{
    char *ps;
    int  i;

    mb->shiftv = 0;
    sequchg_meb();
    mb->bartyp = mv->card[1];
    bsyntyp_meb();
    mb->bartyp = mv->card[1];
    if(mb->expnl != 1) barsync_meb();
    setbyps_meb();
    barcheck_meb();  /* check bar type & exchange */
    if(mb->binstf == mb->sequ) barinst_meb();
    beaupos_meb();
    if(mb->delbarv > 0) {
        mb->delbarf = 1;
        mb->delbarv--;
    }
    else mb->delbarf = 0;
    ps = exsym;
    for(i = 74; i < 79; i++) {
        mv->card[i] = *ps; 
        ps++;
    }
    return(0);
}

/* part case '%' */
int pcaseper_meb(FILE *fpi)
//FILE *fpi;
{
    int  i;

    if(mv->debug == 1) printf("Debug1! Pass+++ pcaseper_() step = %d\n",
        mv->step);
    /*
    if(mb->clrntl) {
        mb->prflg = 0;
        return(0);
    }
    if(mb->clrntsf) {
        clrnotes_meb();
        return(0);
    }
    */
    if(mb->clrontf) clronts_meb();
    noteshft_meb();
    /* beaumod_meb();     */
    beaudpset_meb();   /* Add 1996.7.3 */
    beaudymc_meb();
    /* observe other notes */
    /* notesync_meb(); */
    mb->renp = 1;
    renget_meb();
    if(mb->renp >= MSTC) {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
                printf("Error! %05d renpu max is %d\n",
                mv->line,MSTC);
        }
        return(0);
    }
    if(mb->renp > 1) {
        rensync_meb(fpi);
        strcpy(mv->card,scard[0]);
    }
    else for(i = 0; i < mb->rept; i++) {
        beautrns_meb();
        if( mb->bsyntyp == 'v') beausynt_meb();
        if(mb->bartyp == 'r') beaurtmv_meb();
        /*
        if(mb->observf) {
            if(mb->bartyp != 'r') {
                beauobsv_meb();
                beauobst_meb();
            }
        }
        */
        notesync_meb();
    }
    mb->mnflg1 = 2;
    if(mb->durtev) duration_meb();
    if(mb->notecf) notechg_meb();
    ed_velocity();
    /* if(mb->chdobf) chdNote_meb(); */
    beaumod_meb();
    return(0);
}

/* #modify %xx add duration rates */
int duration_meb()
{
    char wk[4];
    int  durv;

    wk[0] = mv->card[1];
    wk[1] = mv->card[2];
    wk[3] = '\0';
    sscanf(wk,"%d",&durv);
    durv += mb->durtev;    /* add duration rate */
    /* if(durv > 99) durv = 99; */
    if(durv > 99) durv = 0; /* %00 = 99.99 */
    if(durv <= 0) durv = 1;
    sprintf(wk,"%02d",durv);
    mv->card[1] = wk[0];
    mv->card[2] = wk[1];
    return(0);
}

/* Conducter 000 track 's' line velocity_vase */
int edc_velocity()
{
    char wk[8];
    float f;

    if(mv->card[0] != 's') return(0);
    /* cueing format ? */
    if(mv->card[44] == '.' || mv->card[46] == '.') return(0); 
    if(mb->veltef) {
        sprintf(wk,"%4.1f",mb->veltev);
        mv->card[43] = wk[0];
        mv->card[44] = wk[1];
        mv->card[45] = wk[2];
        mv->card[46] = wk[3];
    }
    if(ge->def_vb_f && mb->veltef) {
        sprintf(wk,"%4.1f",ge->def_vb_v);
        mv->card[43] = wk[0];
        mv->card[44] = wk[1];
        mv->card[45] = wk[2];
        mv->card[46] = wk[3];
    }
    if(ge->asn_vb_f) {
        sprintf(wk,"%4.1f",ge->asn_vb_v);
        mv->card[43] = wk[0];
        mv->card[44] = wk[1];
        mv->card[45] = wk[2];
        mv->card[46] = wk[3];
    }
    if(ge->mod_vb_f) {
        sprintf(wk,"%4.1f",ge->mod_vb_v);
        mv->card[43] = wk[0];
        mv->card[44] = wk[1];
        mv->card[45] = wk[2];
        mv->card[46] = wk[3];
    }
    if(ge->set_vb_f) {
        sprintf(wk,"%4.1f",ge->set_vb_v);
        mv->card[43] = wk[0];
        mv->card[44] = wk[1];
        mv->card[45] = wk[2];
        mv->card[46] = wk[3];
    }
    if(ge->asc_vb_f) {
        if(isdigit(mv->card[43])) wk[0] = mv->card[43];
	else wk[0] = ' ';
        if(isdigit(mv->card[44])) wk[1] = mv->card[44];
	else wk[1] = ' ';
	wk[2] = '.';
        if(isdigit(mv->card[46])) wk[3] = mv->card[46];
	else wk[3] = ' ';
	wk[4] = '\0';
        /* sscanf(wk,"%4.1f",&f); */
        sscanf(wk,"%f",&f);
        printf("wk = %s ,f = %f\n",wk,f);
        if(ge->asc_vb_f == 2) f = f + ge->asc_vb_v; /* #add */
        if(ge->asc_vb_f == 3) f = f - ge->asc_vb_v; /* #sub */
	if(ge->asc_vb_f == 4) f = mb->veltev;
        if(ge->asc_vb_f == 5) f = mb->veltev;
        sprintf(wk,"%4.1f",f);
        mv->card[43] = wk[0];
        mv->card[44] = wk[1];
        mv->card[45] = wk[2];
        mv->card[46] = wk[3];
    }
    return(0);
}

int ed_velocity_base()
{
    char wk[8];
    float f;

    if(mb->veltef) {
        sprintf(wk,"%4.1f",mb->veltev);
        mv->card[69] = wk[0];
        mv->card[70] = wk[1];
        mv->card[71] = wk[2];
        mv->card[72] = wk[3];
    }
    if(ge->def_vb_f && mb->veltef) {
        sprintf(wk,"%4.1f",ge->def_vb_v);
        mv->card[69] = wk[0];
        mv->card[70] = wk[1];
        mv->card[71] = wk[2];
        mv->card[72] = wk[3];
    }
    if(ge->asn_vb_f) {
        sprintf(wk,"%4.1f",ge->asn_vb_v);
        mv->card[69] = wk[0];
        mv->card[70] = wk[1];
        mv->card[71] = wk[2];
        mv->card[72] = wk[3];
    }
    if(ge->mod_vb_f) {
        sprintf(wk,"%4.1f",ge->mod_vb_v);
        mv->card[69] = wk[0];
        mv->card[70] = wk[1];
        mv->card[71] = wk[2];
        mv->card[72] = wk[3];
    }
    if(ge->set_vb_f) {
        sprintf(wk,"%4.1f",ge->set_vb_v);
        mv->card[69] = wk[0];
        mv->card[70] = wk[1];
        mv->card[71] = wk[2];
        mv->card[72] = wk[3];
    }
    if(ge->asc_vb_f) {
        if(isdigit(mv->card[69])) wk[0] = mv->card[69];
	else wk[0] = ' ';
        if(isdigit(mv->card[70])) wk[1] = mv->card[70];
	else wk[1] = ' ';
	wk[2] = '.';
        if(isdigit(mv->card[72])) wk[3] = mv->card[72];
	else wk[3] = ' ';
	wk[4] = '\0';
        /* sscanf(wk,"%4.1f",&f); */
        sscanf(wk,"%f",&f);
        /* printf("wk = %s ,f = %f\n",wk,f); */
        if(ge->asc_vb_f == 2) f = f + ge->asc_vb_v; /* #add */
        if(ge->asc_vb_f == 3) f = f - ge->asc_vb_v; /* #sub */
	if(ge->asc_vb_f == 4) f = 0.0;
        if(ge->asc_vb_f == 5) f = mb->veltev;
        sprintf(wk,"%4.1f",f);
        mv->card[69] = wk[0];
        mv->card[70] = wk[1];
        mv->card[71] = wk[2];
        mv->card[72] = wk[3];
    }
    return(0);
}

int notechg_meb()
{
    return(0);
}

/* part case slash */
int pcasesh_meb()
{
    int  ren;

    ren = beaubar_meb();
    mb->renp = ren;
    if(mb->clrntsf) {
        clrnotes_meb();
        /* return(ren); */
    }
    if(mb->clrontf) clronts_meb();
    noteshft_meb();
    beaudpset_meb();
    beaudymc_meb();
    beautrns_meb();
    if(mb->bsyntyp == 'v') beausynt_meb();
    if(mb->bartyp == 'r') beaurtmv_meb();
    /*
    if(mb->observf) {
        if(mb->bartyp != 'r') {
            beauobsv_meb();
            beauobst_meb();
        }
    }
    */
    notesync_meb();
    return(ren);
}

/* part case rest */
int pcaserst_meb()
{
    if(mb->clrntsf) {
        clrnotes_meb();
        return(0);
    }
    if(mb->clrontf) clronts_meb();
    noteshft_meb();
    beauadd_meb();
    beaudymc_meb();
    beautrns_meb();
    if( mb->bsyntyp == 'v') beausynt_meb();
    if(mb->bartyp == 'r') beaurtmv_meb();
    if(mb->clrstl) mb->prflg = 0;
    /*
    if(mb->observf) {
        if(mb->bartyp != 'r') {
            beauobsv_meb();
            beauobst_meb();
        }
    }
    */
    restsync_meb();
    return(0);
}

/* part case '{' */
int pcasebeu_meb(struct mivksn *mk,struct mivsym ms[],char obsf[STLN])
//struct mivksn *mk;
//struct mivsym ms[];
//char obsf[STLN];
{
    int  len;

    len = strlen(mv->card);
    if(len > 79) mv->card[79] = '\0';
    /* printf("mb->lbdflg=%d mb->bsbgnp=%d\n",mb->lbdflg,mb->bsbgnp); */
    /* printf("%s\n",mv->card); */
    /* if(mb->lbdflg && mb->bsbgnp) if(mv->card[77] != '}') mb->prflg = 0; */
    if(mb->lbdflg && mb->bsbgnp) mb->prflg = 0;
    if(mb->xltosf && (mv->card[1] != '/') && (mv->card[75] != '/'))
        mb->prflg = 1;
    if(mv->card[1] == '!') return(0); /* Chord */
    if(mv->card[1] == '$') {
        /* printf("%s\n",mv->card); */
        if(mv->card[2] == ':' || mv->card[3] == ':' ||
            mv->card[4] == ':') {
            return(0);
        }
        /*
        keychk_meb(mk);
        */
        keysgn_meb(mk);
        return(0);
    }
    beauexp_meb();
    if(mb->bexpsf) beauexpr_meb(ms);
    if(mb->observf && ls->fmxopend == 0) {
        if((ls->fmx = fopen(obsf,"rb")) == NULL) {
            //printf("pcasebeau_() observe file \"%s\" not found\n",obsf);
            mb->observf = 0;
        }
        else ls->fmxopend = 1;
    }
    return(0);
}

int keychk_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    int  i;
    int  rtn;
    int  tabfl;

    key_meb();
    mb->adsyn = 0;
    return(0);
}

/* part case '(' */
int pcaselbu_meb(struct mivksn *mk,struct mivsym ms[])
//struct mivksn *mk;
//struct mivsym ms[];
{
    int  i;

    /* beauspace_meb(mv->card); */
    if(mb->pbdflg && mb->bsbgnp) {
        if(mv->card[77] != '}') mb->prflg = 0;
    }
    if(mv->card[1] == '$') {
        mv->card[0] = '{';
        for(i = 0; i < 79; i++) {
            if(mv->card[i] == ')') {
                mv->card[i] = '}';
                break;
            }
        }
        keysgn_meb(mk);
        mv->card[0] = '(';
        for(i = 0; i < 79; i++) {
            if(mv->card[i] == '}') {
                mv->card[i] = ')';
                break;
            }
        }
        return(0);
    }
    sbrtolbr_meb();
    beauexp_meb();
    if(mb->bexpsf) beauexpr_meb(ms);
    lbrtosbr_meb();
    return(0);
}

/* modify 'O' -> '/w' mv->card line */
int modcard_meb()
{
    char card[82];
    int  i;

    strcpy(card,mv->card);
    mv->card[0] = '/';
    if(card[0] == 'O') mv->card[1] = 'w';
    if(card[0] == 'o') {
        mv->card[1] = 'h';
        if(card[1] == '/') return(0);
    }
    if(card[0] == '|') {
        mv->card[1] = 'd';
        if(card[1] == '/') return(0);
    }
    for(i = 1; i < 11; i++) {
        mv->card[i+1] = card[i];
        if(card[i] == '\0') break;
        if(card[i] == ' ') break;
    }
    return(0);
}

int rensync_meb(FILE *fpi)
//FILE *fpi;
{
    int  i,k;
    int  rencnt;
    int  renp;
    int  splt;
    long nval;

    strcpy(scard[0],mv->card);
    strcpy(scard[1],mv->next);
    strcpy(scard[2],mv->next2);
    for(i = 0; i < 82; i++) {
        if(scard[1][i] == '\n') {
            scard[1][i] = '\0';
            break;
        }
    }
    rencnt = 1;
    scdn = 2;
    if(scard[1][4] == '[') rencnt++;
    if(scard[2][4] == '[') rencnt++;
    if(rencnt == 2) {
        scdn++;
        if(mv->step == 1) fgets(scard[scdn],82,fpi);
        else mgets_meb(scard[scdn]);
        mv->line++;
        for(i = 0; i < 82; i++) {
            if(scard[scdn][i] == '\n') {
                scard[scdn][i] = '\0';
                break;
            }
        }
    }
    if(rencnt >= 3) {
        scdn++;
        while(1) {
            if(scdn > MSTC) {
                if(!mv->pass) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                        printf("Error! renp count greater than %d scdn=%d\n",
                        MSTC,scdn);
                }
                return(-1);
            }
            if(mv->step == 1) fgets(scard[scdn],82,fpi);
            else mgets_meb(scard[scdn]);
            for(i = 0; i < 82; i++) {
                if(scard[scdn][i] == '\n') {
                    scard[scdn][i] = '\0';
                    break;
                }
            }
            if(scard[scdn][0] == '-') break; /* Next Bar */ 
            if(scard[scdn][4] == '[') rencnt++;
            /* printf("rencnt = %d\n",rencnt); */
            if(rencnt >= mb->renp) break;
            if(scard[scdn][0] != '{') {
                scdn++;
                mv->line++;
            }
        }
        for(k = 0; k < 1; k++) {
            scdn++;
            if(scdn > MSTC) {
                if(!mv->pass) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                        printf("Error! renp count greater than %d scdn=%d\n",
                        MSTC,scdn);
                }
                return(-1);
            }
            if(mv->step == 1) fgets(scard[scdn],82,fpi);
            else mgets_meb(scard[scdn]);
            mv->line++;
            for(i = 0; i < 82; i++) {
                if(scard[scdn][i] == '\n') {
                    scard[scdn][i] = '\0';
                    break;
                }
            }
        }
    }
    if(mv->debug == 1) {
        printf("Debug1! mb->rept=%d  mb->renp=%d rencnt=%d scdn=%d\n",
        mb->rept,mb->renp,rencnt,scdn);
        for(i = 0; i < scdn - 1 ; i++) {
            printf("%s\n",scard[i]);
        }
    }
    if(mb->rept <= 0) mb->rept = 1;
    for(k = 0; k < mb->rept; k++) {
        for(i = 0; i < scdn - 1 ; i++) {
            strcpy(mv->card,scard[i]);
            strcpy(mv->next,scard[i+1]);
            strcpy(mv->next2,scard[i+2]);
            if( mv->card[0] == '%' ||
                mv->card[0] == 'c' ||
                mv->card[0] == 's') {
                if(mv->mfflg) {
                    if(mb->cueing) excsmpnl_meb();
                    if(mb->midi) excmidnl_meb();
		    edc_transpose();
		    edc_velocity();
                }
                else {
                    if(i != 0) {
                        if(mb->expnl == 7) { /* crear P_Exp symbols Add 2002 */
                            mv->card[74] = '.';
                            mv->card[75] = '_';
                            mv->card[76] = '.';
                            mv->card[77] = '.';
                            mv->card[78] = '.';
                        }
			noteshft_meb();
			beaumod_meb();
                        beaudpset_meb();   /* Add 1996.7.3 */
                        beaudymc_meb();   /* Add 1996.7.3 */
                        ed_velocity();   /* Add 1997.12 */
		    }
                    if(mb->bsyntyp == 'v') beausynt_meb();
                    if(mb->bartyp == 'r') beaurtmv_meb();
                    /*
                    if(mb->observf) {
                        if(mb->bartyp != 'r') {
                            beauobsv_meb();
                            beauobst_meb();
                        }
                    }
                    */
                    beautrns_meb();
                    if(mb->dexpcl) cldexp_meb(); /* Reset Dynamic Expand */
                    if(mb->dexpn) setdexs_meb();
                }
                if(mb->clrontf) clronts_meb();
                notesync_meb();
            }
            if( mv->card[0] == '+') {
                beauadd_meb();
                noteshft_meb();
                bedymst_meb();
                beaudymc_meb();
                beautrns_meb();
                if( mb->bsyntyp == 'v') beausynt_meb();
                if(mb->bartyp == 'r') beaurtmv_meb();
            }
            if(mv->card[75] == ' ' && mv->card[76] == '.' &&
               mv->card[77] == '.' && mv->card[78] == '.') mv->card[75] = '_';
            strcpy(scard[i],mv->card);
            strcpy(scard[i+1],mv->next);
            strcpy(scard[i+2],mv->next2);
            if(i == 0) {
                nval = mb->nval;
                splt = mb->splt;
                renp = mb->renp;
            }
            mb->nval = nval;
            mb->splt = splt;
            mb->renp = renp;
        }
    }
    return(0);
}

int renget_meb()
{
    int  i,j;
    int  renp;
    int  rept;
    char wk[4];

    i = 4;
    renp = 0;
    rept = 0;
    mb->rept = 1;
    while(mv->card[i]) {
        if(mv->card[i] == '[') renp = 1;
        if(mv->card[i] == '*') rept = 1;
        i++;
        if(i > 9) break;
    }
    if(renp) {
        i = 0;
        while(mv->card[i] != '[') i++;
        j = 0;
        i++;
        while(isdigit(mv->card[i])) wk[j++] = mv->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mb->renp);
    }
    if(rept) {
        i = 0;
        while(mv->card[i] != '*') i++;
        j = 0;
        i++;
        while(isdigit(mv->card[i])) wk[j++] = mv->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mb->rept);
    }
    return(0);
}

int notesync_meb()
{
    int  rtn;

    rtn = nsync2_meb();
    if(rtn <= 0) return(0);
    if(!mv->pass) {
        mv->wrncnt++;
        mv->msglvl = 2;
        if(mv->msglvl >= mv->prtlvl)
        if(rtn == 1) {
            printf("Worning! %05d _/))))) is max.\n", mv->line);
            printf("%s\n",mv->card);
        }
        if(rtn == 2) {
            printf("Worning! %05d _/))))). is limit over\n",mv->line);
            printf("%s\n",mv->card);
        }
        if(rtn == 3) {
            printf("Worning! %05d _/)))).. is limit over\n",mv->line);
            printf("%s\n",mv->card);
        }
        if(rtn == 4) printf("Worning! %05d ./>>> is max.\n",mv->line);
        if(rtn == 5) printf("Worning! %05d ./>>>. limit over\n",mv->line);
        if(rtn == 6) printf("Worning! %05d ./>>.. limit over\n",mv->line);
        if(rtn == 7) printf("Worning! %05d note line\n",mv->line);
    }
    return(rtn);
}

int nsync2_meb()
{
    int  i,j,k;
    int  l,s;
    long ll;
    int  eeod;
    int  pasf;
    int  pgate;
    long nval;
    int  accia;
    long nacia;
    char wk[10];
    int  hosei;
    int  schfg;       /* smpte line change on off flag */

    if(mv->mfflg) beaumset_meb();
    if(mv->next[0] == '-') strcpy(mv->temp,mv->next2);
    else strcpy(mv->temp,mv->next);
    pasf = 0;
    i = 0;
    l = 0;
    hosei = 0;
    accia = 0;
    nacia = 0;
    wk[0] = mv->card[1];
    wk[1] = mv->card[2];
    wk[2] = '\0';
    sscanf(wk,"%d",&pgate);
    if(mv->card[4] == '_') {
        i = 6;
        mb->nval = 100000;
        while(mv->card[i] == ')') {
            i++;
            l++;
            if(l > 5) return(1);
            mb->nval = mb->nval / 2;
        }
        if(mv->card[i] == '.') {
            if(l == 5) return(2);
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                if(l == 4) return(3);
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    /* Modify 1996.3.24 */
    /* printf("i=%d\n",i);  Debug */
    if(mv->card[4] == 'O') {
        i = 5;
        mb->nval = 400000;
        if(mv->card[i] == '.') {
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == 'o') {
        i = 6;
        mb->nval = 200000;
        if(mv->card[i] == '.') {
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == '|') {
        i = 7;
        mb->nval = 800000;
        if(mv->card[i] == '.') {
            ll = mb->nval/2;
            mb->nval = ll/2 + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
    }
    if(mv->card[4] == '.') {
        i = 6;
        /* mb->nval = 50000; */
        mb->nval = 100000;
        while(mv->card[i] == ')' || mv->card[i] == '>') {
            if(mv->card[i] == '>') accia = 1;
            if(mv->card[i] == ')') nacia = 1;
            i++;
            l++;
            if(l > 3) return(4);
            mb->nval = mb->nval / 2;
        }
        if(mv->card[i] == '.') {
            if(l == 3) return(5);
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                if(l == 2) return(6);
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
        pasf = 1;
        if(nacia) nacia = mb->nval;
        if(accia) mb->acci += mb->nval;
    }
    if(mv->card[4] == '[') pasf = 1;
    else {
        mb->splt = 1;
        mb->renp = 1;
    }
    if(!pasf) return(7);
    l = 4;
    while(mv->card[l] != ' ') l++;
    j = 0;
    while(isdigit(mv->card[i])) wk[j++] = mv->card[i++];
    wk[j] = '\0';
    if(j && i <= l) sscanf(wk,"%d",&mb->splt);
    if(mv->card[i] == '[') {
        j = 0;
        i++;
        while(isdigit(mv->card[i])) wk[j++] = mv->card[i++];
        wk[j] = '\0';
        if(j) sscanf(wk,"%d",&mb->renp);
    }
    ll = mb->nval;
    if(mv->card[4] != '[') {
        k = mb->splt * mb->renp;
        mb->nval = ll / k;
        hosei = ll - (mb->nval * k);
    }
    /*
    wk[0] = mv->card[69];
    wk[1] = mv->card[70];
    wk[2] = mv->card[71];
    wk[3] = mv->card[72];
    wk[4] = '\0';
    sscanf(wk,"%f",&mv->velo);
    */
    eeod = 0;
    if(mv->temp[0] == ':') eeod = 1;
    if(mv->debug == 1) {
        printf("Debug1! nval=%ld splt=%d renp=%d ",
        ll,mb->splt,mb->renp);
        printf("rept=%d hosei=%d\n",mb->rept,hosei);
    }
    if(mv->step == 1 || mv->step == 3) {
        for(s = 0; s < mb->splt; s++) {
            nval = mb->nval;
            if(mb->sync > 0xffffff || mb->sync < 0) return(-1);
            mb->kind = 1;  /* note */
            if(!accia) {
                nval -= mb->acci;
                mb->acci = 0;
            }
            if(s == 0) schfg = 1;
            else schfg = 0;
            if(eeod) {
                mb->cval = nval*pgate/100;
                if(mv->mfflg == 1) smpteset_meb(schfg);
            }
            else {
                mb->cval = nval;
                if(mv->mfflg == 1) smpteset_meb(schfg);
                mb->cval = nval*pgate/100;
            }
            mb->psyn = mb->sync;
            strcpy(mv->temp,mv->card);
            mb->kind = 1;  /* note */
            mb->psyn = mb->sync - mb->cval;
            if(mv->synck) printf("Synchk! sync=%ld note val=%ld\n",
                mb->sync,nval);
            mb->sync = mb->sync - nval;
            mb->adsyn = mb->adsyn + nval;
            if(eeod) {
                mb->cval = nval - nval*pgate/100;
                if(mv->synck) printf("Synchk! sync=%ld anote val=%ld\n",
                    mb->sync,mb->cval);
                strcpy(mv->befr,mv->card);
                if(mv->next[0] == '-') strcpy(mv->card,mv->next2);
                else strcpy(mv->card,mv->next);
                if(mv->mfflg == 1) {
                    smpteset_meb(1);
                    if(mb->cueing) excsmpnl_meb();
                    if(mb->midi) excmidnl_meb();
                }
                if(mv->next[0] == '-') strcpy(mv->next2,mv->card);
                else strcpy(mv->next,mv->card);
                strcpy(mv->card,mv->befr);
            }
        }
    }
    else {
        for(s = 0; s < mb->splt; s++) {
            nval = mb->nval;
            if(mb->sync > 0xffffff || mb->sync < 0) return(-1);
            mb->kind = 1;  /* note */
            if(!accia) {
                nval -= mb->acci;
                mb->acci = 0;
            }
            if(eeod) {
                mb->cval = nval*pgate/100;
            }
            else {
                mb->cval = nval;
                mb->cval = nval*pgate/100;
            }
            mb->psyn = mb->sync;
            mb->kind = 1;  /* note */
            mb->psyn = mb->sync - mb->cval;
            if(mv->synck) printf("Synchk! sync=%ld note val=%ld\n",
                mb->sync,nval);
            mb->sync = mb->sync - nval;
            mb->adsyn = mb->adsyn + nval;
            if(eeod) {
                mb->cval = nval - nval*pgate/100;
                if(mv->synck) printf("Synchk! sync=%ld anote val=%ld\n",
                    mb->sync,mb->cval);
            }
        }
    }
    mb->sync -= hosei;
    mb->adsyn += hosei;
    if(nacia) mb->sync = mb->sync + nacia;
    return(0);
}

int restsync_meb()
{
    int  i,j;
    int  dgf;
    int  eol;
    int  rept;
    long l;
    char wk[4];
    char card[82];
    int  swf;

    dgf = 0;
    rept = 1;
    mb->kind = 0;   /* rest */
    swf = 0;
    strcpy(card,mv->card);
    for(i = 0; i < 12; i++) {
        if(card[i] == '+' || card[i] == '-') {
            card[i] = ' ';
            swf = 1;
            break;
        }
    }
    if(swf) {
        i++;
        while(isdigit(card[i])) {
            card[i] = ' ';
            i++;
        }  
        card[i] = ' ';
    }
    /* printf("%s\n",card); */
    if(card[0] == 'R') {
        mb->cval = 100000;
        if(card[1] == '.') mb->cval = 150000;
        if(card[2] == '.') mb->cval = 175000;
        if(card[1] == 'w' || card[1] == 'W') {
            mb->cval = 400000;
            if(card[2] == '.') mb->cval = 600000;
            if(card[3] == '.') mb->cval = 700000;
            if(card[4] == '.') mb->cval = 750000;
        }
        if(card[1] == 'h' || card[1] == 'H') {
            mb->cval = 200000;
            if(card[2] == '.') mb->cval = 300000;
            if(card[3] == '.') mb->cval = 350000;
            if(card[4] == '.') mb->cval = 375000;
        }
    }
    if(card[0] == 'r') {
        mb->cval = 50000;
        i = 1;
        while(card[i] == 'r') {
            mb->cval = mb->cval/2;
            i++;
        }
        if(card[i] == '.') mb->cval += mb->cval/2;
        if(card[i+1] == '.') mb->cval += mb->cval/2;
    }
    eol = 0;
    while(card[eol] != ' ') eol++;
    i = 0;
    while(card[i] != ' ') i++;
    if(eol > i) eol = i;
    for(j = 0; j < i; j++) if(isdigit(card[j])) dgf = 1;
    if(dgf) {
        i = 0;
        while(!isdigit(card[i])) i++;
        j = 0;
        while(isdigit(card[i])) {
        wk[j] = card[i];
            i++;
            j++;
        }
        wk[j] = '\0';
        sscanf(wk,"%d",&rept);
        l = mb->cval * rept;
        if(l > 1677215) {
            if(!mv->pass) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl)
                printf("Error! %05d rest repeart over %d\n",
                mv->line,rept);
            }
        }
        else mb->cval = l;
    }
    if(mv->synck) {
        printf("Synchk! sync=%ld rest val=%ld\n",
            mb->sync,mb->cval);
    }
    mb->sync -= mb->cval;
    mb->adsyn += mb->cval;
    if(mv->mfflg == 1) smpteset_meb(1);
    return(0);
}

int beaudymc_meb()
{
    char card[8];

    sprintf(card,"%4.1f",mv->velo);
    if(mv->card[72] == 'x' || 
        mv->card[72] == 'X' || mv->card[72] == ' ') {
        if(card[2] == '.') {
            mv->card[69] = card[0];
            mv->card[70] = card[1];
            mv->card[71] = card[2];
            mv->card[72] = card[3];
        }
        if(card[1] == '.') {
            mv->card[69] = ' ';
            mv->card[70] = card[0];
            mv->card[71] = card[1];
            mv->card[72] = card[2];
        }
    }
    ed_velocity_base();
    return(0);
}

int beaupos_meb()
{
    int  i;

    for(i = 74; i < 79; i++) {
        switch(mv->card[i]) {
            case '<' : mv->plps = i;
                       break;
            case '>' : mv->prps = i;
                       break;
            case '^' : mv->mdps = i;
                       break;
            case 'M' : mv->mdps = i;
                       break;
            case 'D' : mv->dpos = i;
                       break;
            case 'S' : mv->spos = i;
                       break;
            default  : break;
        }
    }
    return(0);
}

int beaurep_meg(char card[82])
//char card[82];
{
    int  i,j;
    int  rept;
    char wk[4];

    rept = 1;
    i = 1;
    j = 0;
    while(isdigit(mv->card[i])) wk[j++] = mv->card[i++];
    wk[j] = '\0';
    if(j) sscanf(wk,"%d",&rept);
    strcpy(mv->card,card);
    return(rept);
}

/* global key {$&&3/4} */
int beaugkey_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    char wk[8];
    char c;
    int  i,j;
    int  pas;
    int  err;
    int  ryhm;
    int  time;
    char wscl[8];
    char keysgn[12];
    int  ky;
    int  rf;

    if(mv->card[2] == ':' || mv->card[3] == ':') {
        setlvl_cps();
        return(0);
    }
    i = 0;
    err = 0;
    pas = 0;
    while(mv->card[i]) {
        if(mv->card[i] == '}') break;
        if(mv->card[i] == 'T' && mv->card[i-1] == ' ') break;
        i++;
    }
    while(mv->card[i] != 'r' && mv->card[i] != 'l') {
        i--;
        if(i < 0) break;
        pas = 1;
    }
    wk[0] = '\0';
    if(pas) {
        i = i - 2;
        for(j = 0; j < 3; j++) wk[j] = mv->card[i++];
        wk[3] = '\0';
    }
    if(!strcmp(wk,"dur")) mv->mkscl = 1;
    if(!strcmp(wk,"oll")) mv->mkscl = 2;
    if(!strcmp(wk,"jor")) mv->mkscl = 3;
    if(!strcmp(wk,"nor")) mv->mkscl = 4;
    i = 2;
    mb->ksin = 0;
    if( mv->card[2] == '#') {
        while(mv->card[i] == '#') {
            i++;
            mb->ksin++;
        }
    }
    if(mv->card[2] == '&') {
        mb->ksin = 7;
        while(mv->card[i] == '&') {
            i++;
            mb->ksin++;
        }
    }
    if(mb->ksin < 0 || mb->ksin > 14) {
        err = 1;
        mb->ksin = 0;
    }
    c = mv->card[i];
    rf = 0;
    if(isalnum(c)) {
        if(c == 'c' || c == 'C') {
            ryhm = 4;
            time = 4;
        }
        else {
            if(!isdigit(c)) err = 2;
            j = 0;
            ryhm = 0;
            while(mv->card[i]) {
                if(mv->card[i] == '/') break;
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) err = 2;
            }
            if(j == 0 || j > 3) err = 3;
            wk[j] = '\0';
            sscanf(wk,"%d",&ryhm);
            if(ryhm > 255) err = 3;
            j = 0;
            i++;
            while(isdigit(mv->card[i])) {
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) err = 4;
            }
            if(j == 0 || j > 3) err = 5;
            wk[j] = '\0';
            sscanf(wk,"%d",&time);
            if(time > 255) err = 6;
        }
        if(!err) rf = 1;
    }
    wscl[0] = '\0';
    switch(mv->mkscl) {
        case  1 : strcat_h(wscl,"_dur");
                  cp->key = 1;
                  ky = 0;
                  break;
        case  2 : strcat_h(wscl,"_moll");
                  cp->key = 2;
                  ky = 1;
                  break;
        case  3 : strcat_h(wscl,"_Major");
                  cp->key = 1;
                  ky = 0;    
                  break;
        case  4 : strcat_h(wscl,"_minor"); 
                  cp->key = 2;
                  ky = 1;
                  break;
        default : strcat_h(wscl,"_Major ?");
                  cp->key = 0; 
                  ky = 0; 
                  break;
    }
    strcpy(keysgn,mk->keysin[ky][mb->ksin]);
    strcat(keysgn,wscl);
    modkey_meb(keysgn);
    if(mv->card[i] == '}') mv->card[i] = '\0';
    if(mv->card[i] == ')') mv->card[i] = '\0';
    if(mv->card[i] == ' ') mv->card[i] = '\0';
    mv->card[i+1] = '\0';
    strcat(mv->card," ");
    /*
    strcat(mv->card,mk->keysin[ky][mb->ksin]);
    strcat(mv->card,wscl);
    */
    strcat(mv->card,keysgn);
    strcat(mv->card,"}");
    if(err && mv->pass == 0) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! %05d key signature\n",mv->line);
    }
    key_meb();
    setkey_cps();
    mb->adsyn = 0;
    return(rf);
}

int beaukey_meb(struct mivksn *mk)
//struct mivksn *mk;
{
    char wk[8];
    char c;
    int  i,j;
    int  pas;
    int  err;
    int  ryhm;
    int  time;
    char wscl[8];
    char keysgn[12];
    int  ky;
    int  rf;

    i = 0;
    err = 0;
    pas = 0;
    while(mv->card[i]) i++;
    while(mv->card[i] != 'r' && mv->card[i] != 'l') {
        i--;
        if(i < 0) break;
        pas = 1;
    }
    wk[0] = '\0';
    if(pas) {
        i = i - 2;
        for(j = 0; j < 3; j++) wk[j] = mv->card[i++];
        wk[3] = '\0';
    }
    if(!strcmp(wk,"dur")) mv->mkscl = 1;
    if(!strcmp(wk,"oll")) mv->mkscl = 2;
    if(!strcmp(wk,"jor")) mv->mkscl = 3;
    if(!strcmp(wk,"nor")) mv->mkscl = 4;
    i = 1;
    mb->ksin = 0;
    if( mv->card[1] == '#') {
        mb->trktyp = 0;         /* transpose key type # */
        while(mv->card[i] == '#') {
            i++;
            mb->ksin++;
        }
    }
    if(mv->card[1] == '&') {
        mb->trktyp = 1;
        mb->ksin = 7;
        while(mv->card[i] == '&') {
            i++;
            mb->ksin++;
        }
    }
    if(mb->ksin < 0 || mb->ksin > 14) {
        err = 1;
        mb->ksin = 0;
    }
    c = mv->card[i];
    rf = 0;
    if(isalnum(c)) {
        if(c == 'c' || c == 'C') {
            ryhm = 4;
            time = 4;
        }
        else {
            if(!isdigit(c)) err = 2;
            j = 0;
            ryhm = 0;
            while(mv->card[i]) {
                if(mv->card[i] == '/') break;
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) err = 2;
            }
            if(j == 0 || j > 3) err = 3;
            wk[j] = '\0';
            sscanf(wk,"%d",&ryhm);
            if(ryhm > 255) err = 3;
            j = 0;
            i++;
            while(isdigit(mv->card[i])) {
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) err = 4;
            }
            if(j == 0 || j > 3) err = 5;
            wk[j] = '\0';
            sscanf(wk,"%d",&time);
            if(time > 255) err = 6;
        }
        if(!err) rf = 1;
    }
    wscl[0] = '\0';
    switch(mv->mkscl) {
        case  1 : strcat_h(wscl,"_dur");
                  cp->key = 1;
                  ky = 0;
                  break;
        case  2 : strcat_h(wscl,"_moll");
                  cp->key = 2;
                  ky = 1;
                  break;
        case  3 : strcat_h(wscl,"_Major");
                  cp->key = 1;
                  ky = 0;    
                  break;
        case  4 : strcat_h(wscl,"_minor"); 
                  cp->key = 2;
                  ky = 1;
                  break;
        default : strcat_h(wscl,"_Major ?");
                  cp->key = 0; 
                  ky = 0; 
                  break;
    }
    strcpy(keysgn,mk->keysin[ky][mb->ksin]);
    strcat(keysgn,wscl);
    modkey_meb(keysgn);
    mv->card[i+1] = '\0';
    strcat(mv->card," ");
    /*
    strcat(mv->card,mk->keysin[ky][mb->ksin]);
    strcat(mv->card,wscl);
    */
    strcat(mv->card,keysgn);
    if(err && mv->pass == 0) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! %05d key signature\n",mv->line);
    }
    return(rf);
}

int key_meb()
{
    int  rtn;

    rtn = key2_meb();
    if(rtn <= 0) return(0);
    if(!mv->pass) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        if(rtn == 1) printf("Error! %05d key signature\n",mv->line);
        if(rtn == 2) printf("Error! %05d rhythm data error\n",mv->line);
        if(rtn == 3) printf("Error! %05d rhythm greater than 255\n",mv->line);
        if(rtn == 4) printf("Error! %05d rhythm(time) data error\n",mv->line);
        if(rtn == 5) printf("Error! %05d ryhm(time) greater than 255\n",
            mv->line);
    }
    return(rtn);
}

int key2_meb()
{
    char wk[8];
    char c;
    int  i,j;
    int  err;

    if(mv->debug == 16) {  // 2025.4.3
        printf("///meb3.c///ksin=%d %05d key signature\n",mb->ksin,mv->line);
        printf("///mv->step=%d\n",mv->step);
        printf("///%s\n",mv->card);
    }
    keyinimb_meb();
    err = 0;
    i = 1;
    if(mv->card[0] == '{') i = 2;
    if(mv->card[i] == '#') {
        while(mv->card[i] == '#') i++;
    }
    if(mv->card[i] == '&') {
        while(mv->card[i] == '&') i++;
    }
    if(mb->ksin < 0 || mb->ksin > 14) return(1);
    c = mv->card[i];
    if(isalnum(c)) {
        if(c == 'c' || c == 'C') {
            mb->ryhm = 4;
            mb->time = 4;
        }
        else {
            if(!isdigit(c)) err = 1;
            j = 0;
            mb->ryhm = 0;
            while(mv->card[i]) {
                if(mv->card[i] == '/') break;
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) err =2;
            }
            if(j == 0 || j > 3) err = 3;
            if(err) return(2);
            wk[j] = '\0';
            sscanf(wk,"%d",&mb->ryhm);
            if(mb->ryhm > 255) return(3);
            j = 0;
            i++;
            while(isdigit(mv->card[i])) {
                wk[j] = mv->card[i];
                i++;
                j++;
                if(j > 8) return(4);
            }
            if(j == 0 || j > 3) return(4);
            wk[j] = '\0';
            sscanf(wk,"%d",&mb->time);
            if(mb->time > 255) return(5);
        }
        if(!err) mb->sync = (400000/mb->time)*mb->ryhm;
    }
    keysetmb_meb();
    return(0);
}

/* / de hajimaru meg data nitaishite %xx _/.. wo tuke 
   //////(3(7 nadode 7 character wo koetabaai worning wo dasu,
   sarani ++++++++++ no bubun wo .......... tosuru.
*/
int beaubar_meb()
{
    int  i;
    int  j;
    int  k;
    int  pas;
    int  ren;
    char c;
    char bwk[4];
    char card[82];

    j = 0;
    i = 0;
    k = 0;
    pas = 0;
    ren = 0;
    for(i = 0; i < 80 ; i++) {
        if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
    }
    while(mv->card[j] != ' ') j++;
    for(i = 0; i < j ; i++) {
        if(mv->card[i] == '[') {
            pas = 1;
            i++;
        }
        if(pas) {
            if(mv->card[i] == '*') break;
            if(!isdigit(mv->card[i])) {
                pas = 0;
                if(!mv->pass) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl)
                    printf("Error! %05d bar renp data.\n",mv->line);
                }
                break;
            }
            bwk[k] = mv->card[i];
            k++;
        }
    }
    if(pas) {
        bwk[k] = '\0';
        sscanf(bwk,"%d",&ren);
        if(ren > 255) {
            if(!mv->pass) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl)
                printf("Error! %05d bar renp greater than 255 len=%d\n",
                mv->line,ren);
            }
            ren = 0;
        }
    }
    j = 0;
    c = mv->card[1];
    if(c == '/' || c == ' ' || c == '.' || c == '*' || c == '[') {
        while(mv->card[j] != ' ') j++;
        if(j > 12) j = 12;
        /* for(i = j ; i >= 0 ; i--) mv->card[i+5] = mv->card[i]; */
        strcpy(card,mv->card);
        for(i = 0; i < j; i++) mv->card[i+5] = card[i];
        for(i = 6 ; i <= j+4 ; i++) {
            if(mv->card[i] == '/') mv->card[i] = ')';
        }
        mv->card[4] = '_';
    }
    else {
        j = 0;
        if(c == 'h') {
            while(mv->card[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) mv->card[i+4] = mv->card[i];
            mv->card[4] = 'o';
            mv->card[5] = '/';
        }
        if(c == 'w') {
            while(mv->card[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) mv->card[i+3] = mv->card[i];
            mv->card[3] = ' ';
            mv->card[4] = 'O';
        }
        if(c == 'd') {
            while(mv->card[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) mv->card[i+3] = mv->card[i];
            mv->card[3] = ' ';
            mv->card[4] = '|';
            mv->card[5] = 'O';
            mv->card[6] = '|';
        }
        if(c == '[' || isdigit(c)) {
            j = 0;
            while(mv->card[j] != ' ') j++;
            for(i = j ; i >= 0 ; i--) mv->card[i+5] = mv->card[i];
            mv->card[4] = '_';
        }
    }
    mv->card[0] = '%';
    mv->card[1] = 'x';
    mv->card[2] = 'x';
    mv->card[3] = ' ';
    return(ren);
}

/* '.'  ->   %xx ./> : acciaccatura */
int beauabar_meb()
{
    int  i;
    int  j;
    int  k;
    char c;
    int  pas;
    char bwk[4];
    int  ren;

    i = 0;
    for(i = 0; i < 80 ; i++) {
        if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
    }
    ren = 0;
    pas = 0;
    for(i = 0; i < 12 ; i++) {
        if(mv->card[i] == '[') {
            pas = 1;
            break;
        }
    }
    if(pas) {
        i++;
        k = 0;
        /* printf("mv->card[i] = %c\n",mv->card[i]); */
        for(;i < 12; i++) {
            if(!isdigit(mv->card[i])) {
                break;
            }
            bwk[k] = mv->card[i];
            k++;
        }
    }
    if(pas) {
        bwk[k] = '\0';
        sscanf(bwk,"%d",&ren);
        if(ren > 255) {
            if(!mv->pass) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl)
                printf("Error! %05d bar renp greater than 255 len=%d\n",
                mv->line,ren);
            }
            ren = 0;
        }
    }
    /* printf("bwk=%s ren=%d\n",bwk,ren); */
    if(ren) {
        i = 0;
        pas = 0;
        while(mv->card[i] != ' ') {
            if(mv->card[i] == '[') pas = 1;
            if(pas) mv->card[i] = ' ';
            i++;
        }
    }
    j = 0;
    c = mv->card[1];
    if(c == '.' || c == ' ') {
        while(mv->card[j] != ' ') j++;
        for(i = j ; i >= 0 ; i--) mv->card[i+5] = mv->card[i];
        for(i = 6 ; i <= j+4 ; i++) {
            if(mv->card[i] == '.') mv->card[i] = '>';
        }
        mv->card[i] = '>';
        mv->card[4] = '.';
        mv->card[5] = '/';
    }
    if(ren) {
        i = 4;
        while(mv->card[i] != ' ') i++;
        mv->card[i++] = '[';
        k = 0;
        while(bwk[k] != '\0') {
            mv->card[i] = bwk[k];
            i++;
            k++;
        }
    }
    if(j > 3 && mv->pass == 0) {
        mv->wrncnt++;
        mv->msglvl = 2;
        if(mv->msglvl >= mv->prtlvl)
        printf(" Worning! %05d '.' over 3 characters\n",mv->line);
    }
    mv->card[0] = '%';
    mv->card[1] = 'x';
    mv->card[2] = 'x';
    mv->card[3] = ' ';
    return(ren);
}

/* ','  ->   %xx ./) : nachschlag */
int beaunbar_meb()
{
    int  i;
    int  j;
    int  k;
    char c;
    int  pas;
    char bwk[4];
    int  ren;

    i = 0;
    for(i = 0; i < 80 ; i++) {
        if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
    }
    pas = 0;
    ren = 0;
    for(i = 0; i < 12 ; i++) {
        if(mv->card[i] == '[') {
            pas = 1;
            break;
        }
    }
    if(pas) {
        i++;
        k = 0;
        /* printf("mv->card[i] = %c\n",mv->card[i]); */
        for(;i < 12; i++) {
            if(!isdigit(mv->card[i])) {
                break;
            }
            bwk[k] = mv->card[i];
            k++;
        }
    }
    if(pas) {
        bwk[k] = '\0';
        sscanf(bwk,"%d",&ren);
        if(ren > 255) {
            if(!mv->pass) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl)
                printf("Error! %05d bar renp greater than 255 len=%d\n",
                mv->line,ren);
            }
            ren = 0;
        }
    }
    /* printf("bwk=%s ren=%d\n",bwk,ren); */
    if(ren) {
        i = 0;
        pas = 0;
        while(mv->card[i] != ' ') {
            if(mv->card[i] == '[') pas = 1;
            if(pas) mv->card[i] = ' ';
            i++;
        }
    }
    j = 0;
    c = mv->card[1];
    if(c == ',' || c == ' ') {
        while(mv->card[j] != ' ') j++;
        for(i = j ; i >= 0 ; i--) mv->card[i+5] = mv->card[i];
        for(i = 6 ; i <= j+4 ; i++) {
            if(mv->card[i] == ',') mv->card[i] = ')';
        }
        mv->card[i] = ')';
        mv->card[4] = '.';
        mv->card[5] = '/';
    }
    if(ren) {
        i = 4;
        while(mv->card[i] != ' ') i++;
        mv->card[i++] = '[';
        k = 0;
        while(bwk[k] != '\0') {
            mv->card[i] = bwk[k];
            i++;
            k++;
        }
    }
    if(j > 3 && mv->pass == 0) {
        mv->wrncnt++;
        mv->msglvl = 2;
        if(mv->msglvl >= mv->prtlvl)
        printf(" Worning! %05d '.' over 3 characters\n",mv->line);
    }
    mv->card[0] = '%';
    mv->card[1] = 'x';
    mv->card[2] = 'x';
    mv->card[3] = ' ';
    return(ren);
}

/* Modify Chord Name */
int modkey_meb(char keysgn[])
//char keysgn[12];
{
    int  i;
    char wk[12];

    i = 0;
    while(1) {
        if(keysgn[i] == '\0') break;
        wk[i] = tolower(keysgn[i]);
        i++;
        if(i >= 12) break;
    }
    wk[i] = '\0';
    /* Major chord */
    if(!strcmp(wk,"b_major")) strcpy(keysgn,"B&_Major");
    if(!strcmp(wk,"h_major")) strcpy(keysgn,"B_Major");
    if(!strcmp(wk,"b_dur")) strcpy(keysgn,"H_dur");
    if(!strcmp(wk,"fis_major")) strcpy(keysgn,"F#_Major");
    if(!strcmp(wk,"f#_dur")) strcpy(keysgn,"Fis_dur");
    if(!strcmp(wk,"cis_major")) strcpy(keysgn,"C#_Major");
    if(!strcmp(wk,"c#_dur")) strcpy(keysgn,"Cis_dur");
    if(!strcmp(wk,"es_major")) strcpy(keysgn,"E&_Major");
    if(!strcmp(wk,"e&_dur")) strcpy(keysgn,"Es_dur");
    if(!strcmp(wk,"as_major")) strcpy(keysgn,"A&_Major");
    if(!strcmp(wk,"a&_dur")) strcpy(keysgn,"As_dur");
    if(!strcmp(wk,"des_major")) strcpy(keysgn,"D&_Major");
    if(!strcmp(wk,"d&_dur")) strcpy(keysgn,"Des_dur");
    if(!strcmp(wk,"ges_major")) strcpy(keysgn,"G&_Major");
    if(!strcmp(wk,"g&_dur")) strcpy(keysgn,"Ges_dur");
    if(!strcmp(wk,"ces_major")) strcpy(keysgn,"C&_Major");
    if(!strcmp(wk,"c&_dur")) strcpy(keysgn,"Ces_dur");
    /* Minor chord */
    if(!strcmp(wk,"h_minor")) strcpy(keysgn,"B_minor");
    /* if(!strcmp(wk,"b_moll")) strcpy(keysgn,"h_moll"); do not change */
    if(!strcmp(wk,"fis_minor")) strcpy(keysgn,"F#_minor");
    if(!strcmp(wk,"f#_moll")) strcpy(keysgn,"fis_moll");
    if(!strcmp(wk,"cis_minor")) strcpy(keysgn,"C#_minor");
    if(!strcmp(wk,"c#_moll")) strcpy(keysgn,"cis_moll");
    if(!strcmp(wk,"gis_minor")) strcpy(keysgn,"G#_minor");
    if(!strcmp(wk,"g#_moll")) strcpy(keysgn,"gis_moll");
    if(!strcmp(wk,"dis_minor")) strcpy(keysgn,"D#_minor");
    if(!strcmp(wk,"d#_moll")) strcpy(keysgn,"dis_moll");
    if(!strcmp(wk,"ais_minor")) strcpy(keysgn,"A#_minor");
    if(!strcmp(wk,"a#_moll")) strcpy(keysgn,"ais_moll");
    if(!strcmp(wk,"b_minor")) strcpy(keysgn,"B&_minor");
    if(!strcmp(wk,"b&_moll")) strcpy(keysgn,"b_moll");
    if(!strcmp(wk,"es_minor")) strcpy(keysgn,"E&_minor");
    if(!strcmp(wk,"e&_moll")) strcpy(keysgn,"es_moll");
    if(!strcmp(wk,"as_minor")) strcpy(keysgn,"A&_minor");
    if(!strcmp(wk,"a&_moll")) strcpy(keysgn,"as_moll");
    /* printf("keysgn = %s wk = %s\n",keysgn,wk); */
    return(0);
}

