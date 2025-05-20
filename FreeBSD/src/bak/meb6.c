
// ----------------------
//  AMuPLC (meb6.c)
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
extern MEBVAR *mb;
extern MEBSRG *ls;
extern MEBEDT *ge;
extern char pwsmp[];
extern char pwmid[];
char smpbar[] = "pno  msg  hh:mm:ss/ff.ff  evno  ainf";
char midbar[] = "0____.____1____.____2 Exp  tp  velo ";
char mdlbar[] = "  -1______0______1______2______*______4______5______6______7";
char lowbar[] = "  _____-1______0______1______2______*______4______5______6__";
char higbar[] = "  ____1______2______*______4______5______6______7______8____";
char topbar[] = " p.v. mod cn:cdt pcg tp*______4______5______6______7______8 ";
char gcfbar[] = " p.v. mod cn:cdt pcg tp2______*______4______5______6______7 ";
char synbar[] = " p.v. mod cn:cdt pcg tp1______2______*______4______5______6 ";
char altbar[] = " p.v. mod cn:cdt pcg tp0______1______2______*______4______5 ";
char basbar[] = " p.v. mod cn:cdt pcg tp1______0______1______2______*______4 ";
char ubsbar[] = " p.v. mod cn:cdt pcg tp2_____-1______0______1______2______* ";
char rtmbar[] = "  tp 3____.____4____.____5____.____6____.____7____.____8____";
char dodbar[] = " p.v. mod cn:cdt pcg tp 4____.____5____.____6____.____7____.";

char obartyp;

/* cleare notes */
int clrnotes_meb()
{
    int  i;

    switch (mb->bartyp) {
        case  'l' : /* low bar */
                    for(i = 11 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'm' : /* middle */
                    for(i = 11 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'h' : /* high */
                    for(i = 11 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  't' : /* top-clef */
                    for(i = 32 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  's' : /* synth */
                    for(i = 32 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'g' : /* gclef */
                    for(i = 32 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'a' : /* alt */
                    for(i = 32 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'b' : /* bass */
                    for(i = 32 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'u' : /* under-bass */
                    for(i = 32 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'r' : /* rhythm */
                    for(i = 14 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        case  'd' : /* dod   */
                    for(i = 32 ; i < 69; i++) mv->card[i] = ' ';
                    break;
        default   : break;
    }
    return(0);
}

/* cleare observed notes */
int clronts_meb()
{
    int  i;

    switch (mb->bartyp) {
        case  'l' : /* low bar */
                    for(i = 11 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'm' : /* middle */
                    for(i = 11 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'h' : /* high */
                    for(i = 11 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  't' : /* top-synth */
                    for(i = 32 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  's' : /* synth */
                    for(i = 32 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'g' : /* gclef */
                    for(i = 32 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'a' : /* alt */
                    for(i = 32 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'b' : /* bass */
                    for(i = 32 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'u' : /* under-bass */
                    for(i = 32 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'r' : /* rhythm */
                    for(i = 14 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        case  'd' : /* dod   */
                    for(i = 32 ; i < 69; i++) {
                        if(mv->card[i] == '.') mv->card[i] = ' ';
                        if(mv->card[i] == '+') mv->card[i] = ' ';
                        if(mv->card[i] == '-') mv->card[i] = ' ';
                        if(mv->card[i] == 'x') mv->card[i] = ' ';
                        if(mv->card[i] == 'o') mv->card[i] = ' ';
                    }
                    break;
        default   : break;
    }
    return(0);
}

/* note line shift due to bar type change */
int noteshft_meb()
{
    int  i;
    char wk[82];
    int  osp;    /* (original) shift start point */
    int  oep;    /* (original) shift end point */
    int  tsp;    /* (terget) shift start point */
    int  tep;    /* (terget) shift end point */

    if(mb->bartyp == 'r') {
        strcpy(wk,mv->card);
        if(wk[21] == ':' && (wk[24] != ' ') && (wk[13] != ' ')) {
            mv->card[11] = 'x';
            mv->card[12] = 'x';
            for(i = 13; i < 32; i++) mv->card[i] = ' ';
        }
    }
    /* printf("mb->shiftv = %d mb->bartyp = %c\n",mb->shiftv,mb->bartyp); */
    if(!mb->shiftv) return(0);
    if(mb->bartyp == 'r') {
        for(i = 13; i < 32; i++) mv->card[i] = ' ';
        return(0);
    }
    if(mb->bartyp == 'd') {
        for(i = 11; i < 32; i++) mv->card[i] = ' ';
        return(0);
    }
    strcpy(wk,mv->card);
    /* printf("mb->shiftv=%d obartyp=%c\n",mb->shiftv,obartyp); */
    if(obartyp == 'p') { /* tp */
        if( mb->bartyp == 'l' ||
            mb->bartyp == 'm' ||
            mb->bartyp == 'h') {
               for(i = 11; i < 68 ; i++) mv->card[i] = ' ';
        }
        return(0);
    }
    osp = 11;
    oep = 68;
    tsp = 11;
    tep = 68;
    if(obartyp == 'v') osp = 32;
    if( mb->bartyp == 's' ||
        mb->bartyp == 't' ||
        mb->bartyp == 'g' ||
        mb->bartyp == 'u' ||
        mb->bartyp == 'a' ||
        mb->bartyp == 'b' ) {
            osp = 32; /* add 2003 */
            tsp = 32;
            tep = 68;
    }
    lnshift_meb(osp,oep,tsp,tep,mb->shiftv);
    if( mb->bartyp == 'l' ||
        mb->bartyp == 'm' ||
        mb->bartyp == 'h') {
           if(obartyp == 'v') for(i = 11; i < 32 ; i++) mv->card[i] = ' ';
    }
    return(0);
}

/* card line shift */
int lnshift_meb(int osp,int oep,int tsp,int tep,int sfv)
//int osp;
//int oep;
//int tsp;
//int tep;
//int sfv;
{
    char wk[82];
    int  i;

    /*
    printf("osp=%d oep=%d tsp=%d tep=%d sfv=%d\n",osp,oep,tsp,tep,sfv);
    printf("%s\n",mv->card); 
    */
    strcpy(wk,mv->card);
    if(tsp == 32) for(i = 11; i < 32 - sfv; i++) wk[i] = ' ';
    for(i = osp; i <= oep; i++) {
        if((i + sfv) >= tsp && (i + sfv) < tep) mv->card[i+sfv] = wk[i];
    }
    if(sfv > 0) for(i = osp ; i < osp + sfv; i++) mv->card[i] = ' ';
    if(sfv < 0) for(i = (tep + sfv) ; i < tep; i++) mv->card[i] = ' ';
    /* if(tsp == 32) for(i = 11; i < 32; i++) mv->card[i] = ' '; out 2003 */
    return(0);
}

/*  check bar type & exchange
         0----+----1----+----2----+----3----+----4----+----5----+----
-m0100001  -1______0______1______2______*______4______5______6______7
-l0100002  _____-1______0______1______2______*______4______5______6__
-h0100003  ____1______2______*______4______5______6______7______8____
-s0100004 p.v. mod cn:cdt pcg tp1______2______*______4______5______6 
-r0100001  tp 3____.____4____.____5____.____6____.____7____.____8____
-d0100001 p.v. mod cn:cdt pcg tp 4____.____5____.____6____.____7____.
*/
int barcheck_meb()
{
    int  i;
    int  oldp;    /* old '*' psition */
    int  newp;    /* new '*' psition */

    obartyp = mv->card[12]; /* v p - ? */
    if(mb->bartyp == 'm') {
        /* if(mv->card[40] != '*') */
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = mdlbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
        return(0);
    }
    if(mb->bartyp == 'l') {
        /* if(mv->card[45] != '*') */
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = lowbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
        return(0);
    }
    if(mb->bartyp == 'h') {
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = higbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
        return(0);
    }
    if(mb->bartyp == 's') {
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = synbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
    }
    if(mb->bartyp == 't') {
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = topbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
    }
    if(mb->bartyp == 'g') {
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = gcfbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
    }
    if(mb->bartyp == 'a') {
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = altbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
    }
    if(mb->bartyp == 'b') {
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = basbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
    }
    if(mb->bartyp == 'u') {
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') oldp = i;
        for(i = 0; i < 60; i++) mv->card[i+9] = ubsbar[i];
        for(i = 11; i < 68; i++) if(mv->card[i] == '*') newp = i;
        mb->shiftv = newp - oldp;
    }
    if(mb->bartyp == 'r') {
        if(mv->card[15] == 'm' && mv->card[16] == 'o' &&
           mv->card[17] == 'd' ) mb->shiftv = 1; /* mod ? */
        for(i = 0; i < 60; i++) mv->card[i+9] = rtmbar[i];
    }
    if(mb->bartyp == 'd') {
        if(mv->card[11] == 't' && mv->card[12] == 'p' &&
           mv->card[13] == ' ' ) mb->shiftv = 1; /* tp  ? */
        for(i = 0; i < 60; i++) mv->card[i+9] = dodbar[i];
    }
    return(0);
}

/* exchange to cueing bar
-c0100001   pno  msg  hh:mm:ss/ff.ff  evno  ainf  tempo    dymc  hh:mm:ss/ff.ff
-c0100002   0____.____1____.____2____. tp  velo   tempo    dymc  hh:mm:ss/ff.ff
*/
int excsmpbr_meb()
{
    int  i,j;

    if(mv->card[12] == 'm' && mv->card[45] == 'n') return(0);
    for(i = 0,j = 12; j < 48; i++,j++) mv->card[j] = smpbar[i];
    return(0);
}

/* exchange smpte to midi bar */
int excmidbr_meb()
{
    int  i,j;

    if(mv->card[12] == '0' && mv->card[45] == 'l') return(0);
    for(i = 0,j = 12; j < 48; i++,j++) mv->card[j] = midbar[i];
    return(0);
}

/* exchange to midi note line
%68 _/                           ._... xx  xx.x   xxx.x  xxx.x%  xx:xx:xx/xx..
s68 _/      ..............  xx:xx:xx/xx.xx xxxxx  xxx.x  xxx.x%  xx:xx:xx/xx..
*/
int excmidnl_meb()
{
    int  i,j;

    for(i = 12; i < 33; i++) mv->card[i] = ' ';
    for(i = 33,j = 0; i < 48; i++,j++) mv->card[i] = pwmid[j];
    beaumset_meb();
    if(mv->card[0] == 'c') mv->card[0] = 's';
    return(0);
}

/* exchange to cueing note line */
int excsmpnl_meb()
{
    int  i,j;

    for(i = 12,j = 0; i < 48; i++,j++) mv->card[i] = pwsmp[j];
    if(mv->card[0] == 's') mv->card[0] = 'c';
    return(0);
}

int sequchg_meb()
{
    char wk[8];

    sprintf(wk,"%02d",mb->sequ);
    mv->card[2] = wk[0];
    mv->card[3] = wk[1];
    /*
    if(mv->card[0] == '{' && mv->card[1] == '/' && mv->card[77] == '}')
        mb->xbnum = 0;
    */
    mb->xbnum++;
    sprintf(wk,"%05d",mb->xbnum);
    mv->card[4] = wk[0];
    mv->card[5] = wk[1];
    mv->card[6] = wk[2];
    mv->card[7] = wk[3];
    mv->card[8] = wk[4];
    /* printf("mb->sequ = %d  mb->xbnum = %d\n",mb->sequ,mb->xbnum); */
    return(0);
}

/* + line velociti set */
int bedymst_meb()
{
     char work[8];

     work[0] = mv->card[69];
     work[1] = mv->card[70];
     work[2] = mv->card[71];
     work[3] = mv->card[72];
     work[4] = '\0';
     sscanf(work,"%f",&mv->velo);
     return(0);
}

/* beautify smpte init val set */
int beauspvl_meb()
{
    char wk[8];
    float tempo;

    /* P_Exp expand */
    /*
    if(mv->card[34] == '+' || mv->card[34] == '-') {
    }
    */
    /* transpose */
    if(mv->card[40] != 'x') {
        wk[0] = mv->card[39];
        wk[1] = mv->card[40];
        wk[2] = '\0';
        sscanf(wk,"%d",&mb->tpv);
    }
    /* velociti */
    if(mv->card[47] != 'x') {
        wk[0] = mv->card[43];
        wk[1] = mv->card[44];
        wk[2] = mv->card[45];
        wk[3] = mv->card[46];
        wk[4] = mv->card[47];
        wk[5] = '\0';
        sscanf(wk,"%f",&mv->velo);
    }
    /* tempo */
    if(mv->card[54] != 'x') {
        wk[0] = mv->card[49];
        wk[1] = mv->card[50];
        wk[2] = mv->card[51];
        wk[3] = mv->card[52];
        wk[4] = mv->card[53];
        wk[5] = mv->card[54];
        wk[6] = '\0';
        sscanf(wk,"%f",&tempo);
        mv->tempo = (tempo + 0.01) * 10;
    }
    /* mdymc */
    if(mv->card[61] != 'x') {
        wk[0] = mv->card[57];
        wk[1] = mv->card[58];
        wk[2] = mv->card[59];
        wk[3] = mv->card[60];
        wk[4] = mv->card[61];
        wk[5] = '\0';
        sscanf(wk,"%f",&mv->mdymc);
    }
    /* SMPTE time code set */
    if(mv->card[78] != 'x') {
        wk[0] = mv->card[74];
        wk[1] = mv->card[75];
        wk[2] = mv->card[76];
        wk[3] = mv->card[77];
        wk[4] = mv->card[78];
        wk[5] = '\0';
        sscanf(wk,"%f",&mb->smptef);
        wk[0] = mv->card[71];
        wk[1] = mv->card[72];
        wk[2] = '\0';
        sscanf(wk,"%d",&mb->smptes);
        wk[0] = mv->card[68];
        wk[1] = mv->card[69];
        wk[2] = '\0';
        sscanf(wk,"%d",&mb->smptem);
        wk[0] = mv->card[65];
        wk[1] = mv->card[66];
        wk[2] = '\0';
        sscanf(wk,"%d",&mb->smpteh);
    }
    return(0);
}

/* beautify synthesizer pitch vend & modulation & control change
-s0100001 p.v. mod cn:cdt pcg tp
%68 _/)   xxxx xxx xx:xxx ... xx
*/
int beausynt_meb()
{
    char card[8];
    int  rtn;

    if(mv->debug) {
        printf("beausynt_meb() mb->gaterf=%d ",mb->gaterf);
        printf("mb->pitchf=%d ",mb->pitchf);
        printf("mb->modulf=%d ",mb->modulf);
        printf("mb->cntlnf=%d ",mb->cntlnf);
        printf("mb->cntldf=%d\n",mb->cntldf);
    }
    if(mb->gaterf && mv->card[0] == '%') {
        if(ge->def_gr_f > 10) {
            sprintf(card,"%02d",ge->def_bexp);
            mv->card[1] = card[0];
            mv->card[2] = card[1];
        }
	else {
            rtn = def_gr_get(); /* get ge->def_bexp */
            /* cmment out 2007.9.24 */
            if(rtn == 0) {
                sprintf(card,"%02d",ge->def_bexp);
                mv->card[1] = card[0];
                mv->card[2] = card[1];
            }
            /* add 2007.9.24 */
            sprintf(card,"%02d",ge->def_bexp);
            mv->card[1] = card[0];
            mv->card[2] = card[1];
	}
	if(ge->def_bexp == 0) {
            gate_rate_get(); /* get mb->bexp */
            sprintf(card,"%02d",mb->bexp);
            mv->card[1] = card[0];
            mv->card[2] = card[1];
	}
    }
    if(mb->pitchf || mv->card[13] == ' ' ||
        mv->card[13] == 'x' || mv->card[13] == 'X') {
        sprintf(card,"%5d",mb->pitchv);
        if(!isdigit(mv->card[9])) {
            if(card[0] != ' ') mv->card[9] = card[0];
            if(card[1] != ' ') mv->card[10] = card[1];
            if(card[2] != ' ') mv->card[11] = card[2];
            if(card[3] != ' ') mv->card[12] = card[3];
            if(card[4] != ' ') mv->card[13] = card[4];
        }
        else mv->card[13] = '0';
        if(mb->pitchf) {  /* add 2007.9.24 */
            mv->card[9] = ' ';
            mv->card[10] = ' ';
            mv->card[11] = ' ';
            mv->card[12] = ' ';
            mv->card[13] = '0';
        }
    }
    if(mb->modulf || mv->card[17] == ' ' || 
        mv->card[17] == 'x' || mv->card[17] == 'X') {
        sprintf(card,"%3d",mb->modul);
        mv->card[15] = card[0];
        mv->card[16] = card[1];
        mv->card[17] = card[2];
    }
    if(mb->cntlnf || mv->card[20] == ' ' || 
        mv->card[20] == 'x' || mv->card[20] == 'X') {
        sprintf(card,"%2d",mb->cntlno);
        mv->card[19] = card[0];
        mv->card[20] = card[1];
    }
    if(mb->cntldf || mv->card[24] == ' ' || 
        mv->card[24] == 'x' || mv->card[24] == 'X') {
        sprintf(card,"%3d",mb->cntldt);
        mv->card[22] = card[0];
        mv->card[23] = card[1];
        mv->card[24] = card[2];
    }
    if(mb->prgchgf) {
        mv->card[26] = '.';
        mv->card[27] = '.';
        mv->card[28] = '.';
    }
    /*
    if(mb->trsnf >= 0) {
        sprintf(card,"%3d",mb->trnsv);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    */
    if(mv->card[31] == ' ' ||
        mv->card[31] == 'x' || mv->card[31] == 'X') {
        sprintf(card,"%3d",mb->tpv);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    ed_pitch_vend();
    ed_modulation();
    ed_transpose();
    /* ed_velocity(); */
    return(0);
}

int ed_pitch_vend()
{
    int  pv;
    char card[8];

    if(ge->def_pv_f && mb->pitchf) {
        sprintf(card,"%5d",ge->def_pv_v);
	/*
        if(card[0] != ' ') mv->card[9] = card[0];
        if(card[1] != ' ') mv->card[10] = card[1];
        if(card[2] != ' ') mv->card[11] = card[2];
        if(card[3] != ' ') mv->card[12] = card[3];
	*/
        if(!isdigit(mv->card[9])) {
            mv->card[9]  = card[0];
            mv->card[10] = card[1];
            mv->card[11] = card[2];
            mv->card[12] = card[3];
            mv->card[13] = card[4];
        }
        else {
            mv->card[12] = ' ';
            mv->card[13] = '0';
        }
    }
    if(ge->asn_pv_f) {
        sprintf(card,"%5d",ge->asn_pv_v);
        if(!isdigit(mv->card[9])) {
            mv->card[9]  = card[0];
            mv->card[10] = card[1];
            mv->card[11] = card[2];
            mv->card[12] = card[3];
            mv->card[13] = card[4];
        }
        else {
            mv->card[12] = ' ';
            mv->card[13] = '0';
        }
    }
    if(ge->mod_pv_f) {
        sprintf(card,"%5d",ge->mod_pv_v);
        if(!isdigit(mv->card[9])) {
            mv->card[9]  = card[0];
            mv->card[10] = card[1];
            mv->card[11] = card[2];
            mv->card[12] = card[3];
            mv->card[13] = card[4];
        }
        else {
            mv->card[12] = ' ';
            mv->card[13] = '0';
        }
    }
    if(ge->set_pv_f) {
        sprintf(card,"%5d",ge->set_pv_v);
        if(!isdigit(mv->card[9])) {
            mv->card[9]  = card[0];
            mv->card[10] = card[1];
            mv->card[11] = card[2];
            mv->card[12] = card[3];
            mv->card[13] = card[4];
        }
        else {
            mv->card[12] = ' ';
            mv->card[13] = '0';
        }
    }
    if(ge->asc_pv_f) {
	card[0] = mv->card[9];
	card[1] = mv->card[10];
	card[2] = mv->card[11];
	card[3] = mv->card[12];
	card[4] = mv->card[13];
	card[5] = '\0';
        sscanf(card,"%5d",&pv);
        if(ge->asc_pv_f == 2) { /* #add */
            if(pv >= 0) pv = pv + ge->asc_pv_v; /* add */
	    else pv = pv - ge->asc_pv_v; 
	}
        if(ge->asc_pv_f == 3) { /* #sub */
	    if(pv >= 0) pv = pv - ge->asc_pv_v; /* sub */
            else pv = pv + ge->asc_pv_v;
        }
	if(ge->asc_pv_f == 4) pv = 0;
        if(ge->asc_pv_f == 5) pv = mb->pitchv;
        sprintf(card,"%5d",pv);
        mv->card[9]  = card[0];
        mv->card[10] = card[1];
        mv->card[11] = card[2];
        mv->card[12] = card[3];
        mv->card[13] = card[4];
    }
    return(0);
}

int ed_modulation()
{
    int  md;
    char card[8];

    if(mb->modulf || mv->card[17] == ' ' || 
        mv->card[17] == 'x' || mv->card[17] == 'X') {
        sprintf(card,"%3d",mb->modul);
        mv->card[15] = card[0];
        mv->card[16] = card[1];
        mv->card[17] = card[2];
    }

    if(ge->def_md_f && mb->modulf) {
        sprintf(card,"%3d",ge->def_md_v);
        mv->card[15] = card[0];
        mv->card[16] = card[1];
        mv->card[17] = card[2];
    }
    if(ge->asn_md_f) {
        sprintf(card,"%3d",ge->asn_md_v);
        mv->card[15] = card[0];
        mv->card[16] = card[1];
        mv->card[17] = card[2];
    }
    if(ge->mod_md_f) {
        sprintf(card,"%3d",ge->mod_md_v);
        mv->card[15] = card[0];
        mv->card[16] = card[1];
        mv->card[17] = card[2];
    }
    if(ge->set_md_f) {
        sprintf(card,"%3d",ge->set_md_v);
        mv->card[15] = card[0];
        mv->card[16] = card[1];
        mv->card[17] = card[2];
    }
    if(ge->asc_md_f) {
	card[0] = mv->card[15];
	card[1] = mv->card[16];
	card[2] = mv->card[17];
	card[3] = '\0';
        sscanf(card,"%3d",&md);
        if(ge->asc_md_f == 2) { /* #add */
            md = md + ge->asc_md_v;
	}
        if(ge->asc_md_f == 3) { /* #sub */
            md = md - ge->asc_md_v;
        }
	if(ge->asc_md_f == 4) md = 0;
        if(ge->asc_md_f == 5) md = mb->modul;
        sprintf(card,"%3d",md);
        mv->card[15] = card[0];
        mv->card[16] = card[1];
        mv->card[17] = card[2];
    }
    return(0);
}

/* Conducter 000 track 's' line transpose */
int edc_transpose()
{
    int  tp;
    char card[8];

    if(mv->card[0] != 's') return(0);
    /* cueing format ? */
    if(mv->card[39] == 'x' || mv->card[40] == 'x') return(0);
    if(ge->def_tp_f && mb->transvf) {
        sprintf(card,"%3d",ge->def_tp_v);
        mv->card[38] = card[0];
        mv->card[39] = card[1];
        mv->card[40] = card[2];
    }
    if(ge->asn_tp_f) {
        sprintf(card,"%3d",ge->asn_tp_v);
        mv->card[38] = card[0];
        mv->card[39] = card[1];
        mv->card[40] = card[2];
    }
    if(ge->mod_tp_f) {
        sprintf(card,"%3d",ge->mod_tp_v);
        mv->card[38] = card[0];
        mv->card[39] = card[1];
        mv->card[40] = card[2];
    }
    if(ge->set_tp_f) {
        sprintf(card,"%3d",ge->set_tp_v);
        mv->card[38] = card[0];
        mv->card[39] = card[1];
        mv->card[40] = card[2];
    }
    if(ge->asc_tp_f) {
        card[0] = mv->card[38];
        card[1] = mv->card[39];
        card[2] = mv->card[40];
	card[3] = '\0';
        sscanf(card,"%3d",&tp);
        if(ge->asc_tp_f == 2) { /* #add */
            tp = tp + ge->asc_tp_v;
	}
        if(ge->asc_tp_f == 3) { /* #sub */
            tp = tp - ge->asc_tp_v;
        }
	if(ge->asc_tp_f == 4) tp = 0;
        if(ge->asc_tp_f == 5) tp = mb->tpv;
        sprintf(card,"%3d",tp);
        mv->card[38] = card[0];
        mv->card[39] = card[1];
        mv->card[40] = card[2];
    }
    return(0);
}

int ed_transpose()
{
    int  tp;
    char card[8];

    if(mv->card[31] == ' ' ||
        mv->card[31] == 'x' || mv->card[31] == 'X') {
        sprintf(card,"%3d",mb->tpv);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    if(mv->card[21] == ':' && mb->transvf) {
        sprintf(card,"%3d",ge->def_tp_v);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    if(ge->def_tp_f && mb->transvf) {
        sprintf(card,"%3d",ge->def_tp_v);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    if(ge->asn_tp_f) {
        sprintf(card,"%3d",ge->asn_tp_v);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    if(ge->mod_tp_f) {
        sprintf(card,"%3d",ge->mod_tp_v);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    if(ge->set_tp_f) {
        sprintf(card,"%3d",ge->set_tp_v);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    if(ge->asc_tp_f) {
	card[0] = mv->card[29];
	card[1] = mv->card[30];
	card[2] = mv->card[31];
	card[3] = '\0';
        sscanf(card,"%3d",&tp);
        if(ge->asc_tp_f == 2) { /* #add */
            tp = tp + ge->asc_tp_v;
	}
        if(ge->asc_tp_f == 3) { /* #sub */
            tp = tp - ge->asc_tp_v;
        }
	if(ge->asc_tp_f == 4) tp = 0;
        if(ge->asc_tp_f == 5) tp = mb->gtrnsv;
        sprintf(card,"%3d",tp);
        mv->card[29] = card[0];
        mv->card[30] = card[1];
        mv->card[31] = card[2];
    }
    return(0);
}

int ed_velocity()
{
    char card[4];
    int  pos;
    int  val;
    int  rtn;
    int  pf;
    int  vel;

    if(ge->def_vl_f && mb->clvelf) {
        pf = 0;
        /* get note postion & value */
        while((rtn = get_npv_meb(pf,&pos,&val))) {
            pf = rtn;
            /* printf("pos = %d  val = %d\n",pos,val); */
            sprintf(card,"%2d",ge->def_vl_v);
            mv->card[pos] = card[1];
        }
    }
    if(ge->asn_vl_f) {
        pf = 0;
        while((rtn = get_npv_meb(pf,&pos,&val))) {
            pf = rtn;
            /* printf("pos = %d  val = %d\n",pos,val); */
            sprintf(card,"%2d",ge->asn_vl_v);
            mv->card[pos] = card[1];
        }
    }
    if(ge->mod_vl_f) {
        pf = 0;
        while((rtn = get_npv_meb(pf,&pos,&val))) {
            pf = rtn;
            /* printf("pos = %d  val = %d\n",pos,val); */
            sprintf(card,"%2d",ge->mod_vl_v);
            mv->card[pos] = card[1];
        }
    }
    if(ge->set_vl_f) {
        pf = 0;
        while((rtn = get_npv_meb(pf,&pos,&val))) {
            pf = rtn;
            /* printf("pos = %d  val = %d\n",pos,val); */
            sprintf(card,"%2d",ge->set_vl_v);
            mv->card[pos] = card[1];
        }
    }
    if(ge->asc_vl_f) {
        pf = 0;
        while((rtn = get_npv_meb(pf,&pos,&val))) {
            pf = rtn;
            /* printf("pos = %d  val = %d\n",pos,val); */
            if(ge->asc_vl_f == 2) { /* #add */
                vel = val + ge->asc_vl_v;
            }
            if(ge->asc_vl_f == 3) { /* #sub */
                vel = val - ge->asc_vl_v;
            }
	    if(ge->asc_vl_f == 4) vel = 0;
            if(ge->asc_vl_f == 5) vel = mb->velv; /* default */
            if(vel > 9) vel = 9;
            if(vel < 0) vel = 0;
            sprintf(card,"%2d",vel);
            mv->card[pos] = card[1];
        }
    }
    return(0);
}

/* get note postion & value 
0123456789+123456789+123456789+123456789+123456789+123456789+123456789+12345678
hml       _-1______0______1______2______*______4______5______6______7
-s0000001 p.v. mod cn:cdt pcg tp1______2______*______4______5______6 velo P_Exp
r          tp 3____.____4____.____5____.____6____.____7____.____8____
*/
int get_npv_meb(int pf,int *pos,int *val)
//int  pf;  /* pass flag */
//int  *pos;
//int  *val;
{
    int  i;
    char c;
    static int sp;
    static int ep;

    if(!pf) {
        /* printf("mb->bartyp = %c\n",mb->bartyp); */
        switch(mb->bartyp) {
            case  'm' : /* mdl */
                        sp = 11;
                        ep = 69;
                        break;
            case  'l' : /* low */
                        sp = 11;
                        ep = 69;
                        break;
            case  'h' : /* high */
                        sp = 11;
                        ep = 69;
                        break;
            case  't' : /* top */
                        sp = 32;
                        ep = 69;
                        break;
            case  'g' : /* g-clef */
                        sp = 32;
                        ep = 69;
                        break;
            case  's' : /* shynth */
                        sp = 32;
                        ep = 69;
                        break;
            case  'a' : /* alt-clef */
                        sp = 32;
                        ep = 69;
                        break;
            case  'b' : /* bass-clef */
                        sp = 32;
                        ep = 69;
                        break;
            case  'u' : /* under_bass */
                        sp = 32;
                        ep = 69;
                        break;
            case  'r' : /* rhythm*/
                        sp = 13;
                        ep = 69;
                        break;
            case  'd' : /* dod */
                        sp = 32;
                        ep = 69;
                        break;
            default   : /* none */
                        printf("Error bartyp %c  at get_npv_meb()\n",
                            mb->bartyp);
                        return(0);
                        break;
        }
    }
    for(i = sp; i < ep; i++) {
        c = mv->card[i];
        if(c != ' ') {
            if(isdigit(c)) {
                *val = c & 0x0f;
                /* printf("c = %c *val = %d\n",c,*val); */
                *pos = i;
                sp = i + 1;
                return(1);
            }
        }
    }
    return(0);
}

/* rhythm note line
-r0100001  tp 
%_/        xx 
*/
int beaurtmv_meb()
{
     int  tp;
     char card[8];

     if(mb->transvf || mv->card[12] == ' ' || 
         mv->card[12] == 'x' || mv->card[12] == 'X') {
         sprintf(card,"%3d",mb->tpv);
         /* mv->card[10] = card[0]; */
         mv->card[11] = card[1];
         mv->card[12] = card[2];
     }
    if(ge->def_tp_f && mb->transvf) {
        sprintf(card,"%3d",ge->def_tp_v);
        /* mv->card[10] = card[0]; */
        mv->card[11] = card[1]; 
        mv->card[12] = card[2];
    }   
    if(ge->asn_tp_f) {
        sprintf(card,"%3d",ge->asn_tp_v); 
        /* mv->card[10] = card[0]; */
        mv->card[11] = card[1];
        mv->card[12] = card[2]; 
    }       
    if(ge->mod_tp_f) {
        sprintf(card,"%3d",ge->mod_tp_v);
        /* mv->card[10] = card[0]; */
        mv->card[11] = card[1];
        mv->card[12] = card[2];
    }   
    if(ge->set_tp_f) { 
        sprintf(card,"%3d",ge->set_tp_v);
        /* mv->card[10] = card[0]; */
        mv->card[11] = card[1];
        mv->card[12] = card[2];
    }
    if(ge->asc_tp_f) {
        card[0] = mv->card[10];
        card[1] = mv->card[11];
        card[2] = mv->card[12];
        card[3] = '\0';
        sscanf(card,"%3d",&tp);
        if(ge->asc_tp_f == 2) { /* #add */
            tp = tp + ge->asc_tp_v;
        }
        if(ge->asc_tp_f == 3) { /* #sub */
            tp = tp - ge->asc_tp_v;
        }
        if(ge->asc_tp_f == 4) tp = 0;
        if(ge->asc_tp_f == 5) tp = mb->gtrnsv;
        sprintf(card,"%3d",tp); 
        mv->card[10] = card[0];
        mv->card[11] = card[1];
        mv->card[12] = card[2];
    }   
    return(0);
}

/*
    make multi file
        ex0_x   ->  ex0_x00.mlk return(99)  prtn = 0
        ex0_xx  ->  ex0_0xx.mlk return(0)   prtn = 36
        ex0_xxx ->  ex0_xxx.mlk return(0)   prtn = 0
        ex0/Y;X ->  ex0_002.mlk retnrn(Y)   prtn = X (fnmb = 0,fnmb = -1)
        ex0/Y;X ->  ex0_002.mlk retnrn(Y-1) prtn = X (fnmb = 1)
    modify 1989.9.26 v_1.45  _axx. : hex suport
*/
int mkmultif_h(char *infn,char *outf,char *ftyp,int fnmb,int *prtn,int *yfile,int *xfile)
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
        if(*xmf == '/') vflg = 1;   /* 1st dimensional vectoe _xx or _x */
        if(*xmf == '_') vflg = 2;   /* 1st dimensional vectoe _xx or _x */
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
    sscanf(xw,"%x",prtn);  /* %d v_1.45 */
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
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
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
        xw[j] = '\0';
        /* printf("i = %d  fnum = %s xw = %s\n",i,fnum,xw); */
        if(i == 1) {
            for(j = 0; j < 36; j++,ps++) if(fnum[0] == *ps) break;
            n = j;
        }
        else sscanf(fnum,"%d",&n);
        *yfile = n;
        sscanf(xw,"%x",xfile);  /* %d v_1.45 */
        strcpy(outf,w);
        strcat_h(outf,"_");
        fnum[1] = '\0';
        strcat_h(outf,fnum);
        sprintf(fnum,"%02x",fnmb+1);  /* %03d v_1.45 */
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
    sscanf(w,"%x",xfile);  /* %d v_1.45 */
    return(0);
}

int filemake_h(int n,char inf[],char fext[],char dfile[],int *yflp,int *xflp)
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

    /*
    printf("Check! filemake_() n=%d inf=%s fext=%s dfile=%s\n",
        n,inf,fext,dfile); 
    */
    trk = n/1000;  /* n/100 v_1.45 */
    memb = n - trk * 1000;  /* n - trk * 100 v_1.45 */
    *yflp = trk;
    *xflp = memb;
    pc = "0123456789abcdefghijklmnopqrstuvwxyz";
    for(i = 0; i < trk ; i++) pc++;
    wfnum[0] = *pc;
    wfnum[1] = '\0';
    sprintf(wk,"%02x",memb);  /* %02d v_1.45 */
    strcat(wfnum,wk);
    dfile[0] = '\0';
    strcat(dfile,inf);
    strcat(dfile,"_");
    strcat(dfile,wfnum);
    strcat(dfile,fext);
    // printf("Check! memb=%d wk=%s wfnum=%s dfile=%s\n",memb,wk,wfnum,dfile);
    return(0);
}

/* SMPTE time data set */
int smpteset_meb(int schfg)
//int  schfg;
{
    float gtime;
    float tval;
    float flame;
    int  whour;
    int  wmin;
    int  wsec;
    char wk[12];

    /* if(mb->smpmid != 1 && mb->smpmid != 2) return(0);  not smpte */
    /* SMPTE time code set */
    if(schfg) {
        if(mb->rssmptf || mv->card[78] == 'x' || mv->card[78] == 'X') {
            sprintf(wk,"%05.2f",mb->smptef);
            mv->card[74] = wk[0];
            mv->card[75] = wk[1];
            mv->card[76] = wk[2];
            mv->card[77] = wk[3];
            mv->card[78] = wk[4];
            sprintf(wk,"%02d",mb->smptes);
            mv->card[71] = wk[0];
            mv->card[72] = wk[1];
            sprintf(wk,"%02d",mb->smptem);
            mv->card[68] = wk[0];
            mv->card[69] = wk[1];
            sprintf(wk,"%02d",mb->smpteh);
            mv->card[65] = wk[0];
            mv->card[66] = wk[1];
            /*
            printf("Check1 flame=%5.2f sec=%d min=%d hour=%d\n",
                mb->smptef,mb->smptes,mb->smptem,mb->smpteh);
            if(schfg == 3) return(0);
            */
        }
        if(mb->setsmpf) {
            smpteget_meb();
            flame = mb->gsmptef + mb->smpstf;
            if(flame < 0) flame = 0;
            wsec = flame / mb->sflame;
            flame = flame - (wsec * mb->sflame);  /* SMPTE flames */
            wsec = mb->gsmptes + wsec + mb->smpsts;
            if(wsec < 0) wsec = 0;
            wmin = wsec / 60;
            wsec = wsec - (wmin * 60);      /* SMPTE seconds */
            wmin = mb->gsmptem + wmin + mb->smpstm;
            if(wmin < 0) wmin = 0;
            whour = wmin / 60;
            wmin = wmin - (whour * 60);     /* SMPTE minites */
            /* SMPTE hours */
            whour = mb->gsmpteh + whour + mb->smpsth;
            if(whour < 0) whour = 0;
            whour %= 24;
            sprintf(wk,"%05.2f",flame);
            mv->card[74] = wk[0];
            mv->card[75] = wk[1];
            mv->card[76] = wk[2];
            mv->card[77] = wk[3];
            mv->card[78] = wk[4];
            sprintf(wk,"%02d",wsec);
            mv->card[71] = wk[0];
            mv->card[72] = wk[1];
            sprintf(wk,"%02d",wmin);
            mv->card[68] = wk[0];
            mv->card[69] = wk[1];
            sprintf(wk,"%02d",whour);
            mv->card[65] = wk[0];
            mv->card[66] = wk[1];
            /*
            printf("Check2 flame=%5.2f sec=%d min=%d hour=%d\n",
                mb->smptef,mb->smptes,mb->smptem,mb->smpteh);
            */
            return(0);
        }
    }
    if(mv->card[50] == '=') {
        gtime = 1;  /* ======= line */
        return(0);
    }
    else {
        wk[0] = mv->card[49];
        wk[1] = mv->card[50];
        wk[2] = mv->card[51];
        wk[3] = mv->card[52];
        wk[4] = mv->card[53];
        wk[5] = mv->card[54];
        wk[6] = '\0';
        sscanf(wk,"%f",&gtime);
        tval = mb->cval;
        tval = tval / 100000.0;
        gtime = gtime / tval;
        if(gtime <= 0.00001 || gtime > 100000) {
            if(!mv->pass && (schfg != 3)) {
               printf("Error! tempo data ");
               printf("tval=%f ",tval);
               printf("gtime=%f mb->cval=%ld\n",gtime,mb->cval);
            }
            gtime = 60;
        }
    }
    flame = 60 * mb->sflame / gtime;
    /*
    printf("flame=%f mb->sflame=%d gate time=%f\n",flame,mb->sflame,gtime);
    */
    mb->cval = 0;
    /* flame = mb->smptef + flame + 0.0001; 2011.9.1 Comment Out */
    flame = mb->smptef + flame;
    wsec = flame / mb->sflame;
    mb->smptef = flame - (wsec * mb->sflame);  /* SMPTE flames */
    wsec = mb->smptes + wsec;
    wmin = wsec / 60;
    mb->smptes = wsec - (wmin * 60);      /* SMPTE seconds */
    wmin = mb->smptem + wmin;
    whour = wmin / 60;
    mb->smptem = wmin - (whour * 60);     /* SMPTE minites */
    mb->smpteh = mb->smpteh + whour;      /* SMPTE hours */
    /*
    printf("Check3 flame=%5.2f sec=%d min=%d hour=%d\n",
        mb->smptef,mb->smptes,mb->smptem,mb->smpteh);
    */
    return(0);
}

/* get SMPTE #defined time code
#set smpteadd xx:xx:xx/xx.xx 
#set smptesub xx:xx:xx/xx.xx 
*/
int smptaget_meb()
{
    int  i,j;
    char wk[16];
    char wc[8];

    i = 0;
    while(mv->card[i] != '.') {
        i++;
        if(i >= 40) {
            if(!mv->pass) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl)
                printf("Error! %06d #set smpte (smptaget)\n",mv->line);
            }
            return(-1);
        }
    }
    if(mv->card[i-3] != '/') {
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! %06d #set smpte (smptaget)\n",mv->line);
        }
        return(-1);
    }
    i = i + 2;
    j = 13;
    while(mv->card[i] != ' ') {
        wk[j] = mv->card[i];
        j--;
        if(j < 0) break;
        i--;
    }
    wk[14] = '\0';
    wc[0] = wk[0];
    wc[1] = wk[1];
    wc[2] = 0x00;
    sscanf(wc,"%d",&mb->smpsth);
    wc[0] = wk[3];
    wc[1] = wk[4];
    wc[2] = 0x00;
    sscanf(wc,"%d",&mb->smpstm);
    wc[0] = wk[6];
    wc[1] = wk[7];
    wc[2] = 0x00;
    sscanf(wc,"%d",&mb->smpsts);
    wc[0] = wk[9];
    wc[1] = wk[10];
    wc[2] = wk[11];
    wc[3] = wk[12];
    wc[4] = wk[13];
    wc[5] = 0x00;
    sscanf(wc,"%f",&mb->smpstf);
    /* printf("wk = %s wc = %s val = %f\n",wk,wc,mb->smpstf); */
    return(0);
}

/* get SMPTE time code */
int smpteget_meb()
{
    char wc[8];

    wc[0] = mv->card[65];
    wc[1] = mv->card[66];
    wc[2] = 0x00;
    sscanf(wc,"%d",&mb->gsmpteh);
    wc[0] = mv->card[68];
    wc[1] = mv->card[69];
    wc[2] = 0x00;
    sscanf(wc,"%d",&mb->gsmptem);
    wc[0] = mv->card[71];
    wc[1] = mv->card[72];
    wc[2] = 0x00;
    sscanf(wc,"%d",&mb->gsmptes);
    wc[0] = mv->card[74];
    wc[1] = mv->card[75];
    wc[2] = mv->card[76];
    wc[3] = mv->card[77];
    wc[4] = mv->card[78];
    wc[5] = 0x00;
    sscanf(wc,"%f",&mb->gsmptef);
    /* printf("wc = %s val = %f\n",wc,mb->gsmptef); */
    return(0);
}

/* smpte start val set
Smpte start time = xx:xx:xx/xx.xx
Smpte lag time = xx:xx:xx/xx.xx
Smpte flame type = x
*/
int smptsval_meb()
{
    char wk[6];

    if(mb->smptef < 0 || mb->smptef > 30.0) return(-1);
    if(mb->smptes < 0 || mb->smptes > 60) return(-1);
    if(mb->smptem < 0 || mb->smptem > 60) return(-1);
    if(mb->smpteh < 0 || mb->smpteh > 60) return(-1);
    if(mv->card[6] == 's') {
        if(mb->arssmptf || mv->card[32] == 'x' || mv->card[32] == 'X') {
            sprintf(wk,"%05.2f",mb->smptef);
            mv->card[28] = wk[0];
            mv->card[29] = wk[1];
            mv->card[30] = wk[2];
            mv->card[31] = wk[3];
            mv->card[32] = wk[4];
            sprintf(wk,"%02d",mb->smptes);
            mv->card[25] = wk[0];
            mv->card[26] = wk[1];
            sprintf(wk,"%02d",mb->smptem);
            mv->card[22] = wk[0];
            mv->card[23] = wk[1];
            sprintf(wk,"%02d",mb->smpteh);
            mv->card[19] = wk[0];
            mv->card[20] = wk[1];
        }
    }
    if(mv->card[6] == 'l') {
        if(mv->card[30] == 'x' || mv->card[30] == 'X') {
            sprintf(wk,"%05.2f",mb->smplagf);
            mv->card[26] = wk[0];
            mv->card[27] = wk[1];
            mv->card[28] = wk[2];
            mv->card[29] = wk[3];
            mv->card[30] = wk[4];
            sprintf(wk,"%02d",mb->smplags);
            mv->card[23] = wk[0];
            mv->card[24] = wk[1];
            sprintf(wk,"%02d",mb->smplagm);
            mv->card[20] = wk[0];
            mv->card[21] = wk[1];
            sprintf(wk,"%02d",mb->smplagh);
            mv->card[17] = wk[0];
            mv->card[18] = wk[1];
        }
    }
    return(0);
}

/* get smpte start val
Smpte start time = xx:xx:xx/xx.xx
Smpte lag time = xx:xx:xx/xx.xx
*/
int smptgval_meb()
{
    char wk[6];
    float flame;
    int  whour;
    int  wmin;
    int  wsec;

    if(mv->card[6] == 's' && (mv->card[32] != 'x')) {
        wk[0] = mv->card[28];
        wk[1] = mv->card[29];
        wk[2] = mv->card[30];
        wk[3] = mv->card[31];
        wk[4] = mv->card[32];
        wk[5] = 0x00;
        sscanf(wk,"%f",&mb->smptef);
        wk[0] = mv->card[25];
        wk[1] = mv->card[26];
        wk[2] = 0x00;
        sscanf(wk,"%d",&mb->smptes);
        wk[0] = mv->card[22];
        wk[1] = mv->card[23];
        wk[2] = 0x00;
        sscanf(wk,"%d",&mb->smptem);
        wk[0] = mv->card[19];
        wk[1] = mv->card[20];
        wk[2] = 0x00;
        sscanf(wk,"%d",&mb->smpteh);
    }
    if(mv->card[6] == 'l' && (mv->card[30] != 'x')) {
        wk[0] = mv->card[26];
        wk[1] = mv->card[27];
        wk[2] = mv->card[28];
        wk[3] = mv->card[29];
        wk[4] = mv->card[30];
        wk[5] = 0x00;
        sscanf(wk,"%f",&mb->smplagf);
        wk[0] = mv->card[23];
        wk[1] = mv->card[24];
        wk[2] = 0x00;
        sscanf(wk,"%d",&mb->smplags);
        wk[0] = mv->card[20];
        wk[1] = mv->card[21];
        wk[2] = 0x00;
        sscanf(wk,"%d",&mb->smplagm);
        wk[0] = mv->card[17];
        wk[1] = mv->card[18];
        wk[2] = 0x00;
        sscanf(wk,"%d",&mb->smplagh);
        flame = mb->smptef + mb->smplagf;
        wsec = flame / mb->sflame;
        mb->smptef = flame - (wsec * mb->sflame);  /* SMPTE flames */
        wsec = mb->smptes + wsec + mb->smplags;
        wmin = wsec / 60;
        mb->smptes = wsec - (wmin * 60);      /* SMPTE seconds */
        wmin = mb->smptem + wmin + mb->smplagm;
        whour = wmin / 60;
        mb->smptem = wmin - (whour * 60);     /* SMPTE minites */
        mb->smpteh = mb->smpteh + whour + mb->smplagh;  /* SMPTE hours */
        mb->smpteh %= 24;
    }
    /* flame type */
    if(mv->card[6] == 'f' && (mv->card[30] != 'x')) {
        mv->tctyp = numdinmb_meb(mv->card);
        if(mv->tctyp == 0) mb->sflame = 24;
        if(mv->tctyp == 1) mb->sflame = 25;
        if(mv->tctyp == 2) mb->sflame = 30;
        if(mv->tctyp == 3) mb->sflame = 30;
        /* printf("tctyp=%d sflame=%d\n",mv->tctyp,mb->sflame); */
    }
    return(0);
}

/* Observe other notes */
int beauobsv_meb()
{
    union MCODE u;
    unsigned char iword[4];
    int c;
    int j,k;

    if(mv->debug == 5)
        printf("beauobsv_() mb->sequ=%d mb->bnum=%d mb->sync=%ld openf = %d\n",
        mb->sequ,mb->bnum,mb->sync,ls->fmxopend);
    if(!ls->fmxopend) return(1);
    k = 0;
    while(1) {
        /* if(ls->sequ == mb->sequ && ls->bnum <= mb->bnum) break; */
        if(ls->sequ == mb->sequ && ls->bnum > mb->bnum) break;
        if((c = fgetc(ls->fmx)) == EOF) {
            mb->observf = 0;
            ls->fmxopend = 0;
            //fclose(ls->fmx);
            return(9);
        }
        j = k % 4;
        iword[j] = c;
        k++;
        if(j == 3) {
            compchk_mlk(&u,iword);
            k = 0;
            switch (u.wc[1]) {
                case 0xfb : fbdecode_mlk(&u);
                            break;
                case 0xfc : fcdecode_mlk(&u);
                            if(ls->bnum == mb->bnum && ls->sync < mb->sync)
                                return(0);
                            break;
                case 0xfd : fddecode_mlk(&u);
                            break;
                /*
                case 0xdd : dddecode_mlk(&u);
                            break;
                */
                default   : break;
            }
        }
    }
    return(0);
}

/* beautify observe set */
int beauobst_meb()
{
    int  i,j,k;
    int  lastp;

    lastp = 69;
    j = 11;
    k = 15;  /* mb->bartyp == 'm' */
    if(mb->bartyp == 'l') k = 11;
    if(mb->bartyp == 'h') k = 26;
    if( mb->bartyp == 's' ||
        mb->bartyp == 't' ||
        mb->bartyp == 'g' ||
        mb->bartyp == 'a' ||
        mb->bartyp == 'u' ||
        mb->bartyp == 'b') {
        k = 53;
        j = 32;
    }
    k = k - j;
    for(i = j; i < lastp; i++) {
        /*
        if(mv->card[i] == '.') mv->card[i] = ' ';
        if(mv->card[i] == '+') mv->card[i] = ' ';
        if(mv->card[i] == '-') mv->card[i] = ' ';
        if(mv->card[i] == 'x') mv->card[i] = ' ';
        */
        if(mv->card[i] == ' ') {
            ls->keyp = mv->ktb[k];
            if(ls->obstb[ls->keyp] > 0) {
                mv->card[i] = '.';
                if(mv->debug == 5) {
                    printf("Debug5! ls->keyp=%d ls->trnsv=%d\n",
                        ls->keyp,ls->trnsv);
                }
            }
        }
        if(mv->card[i] == ' ') {
            ls->keyp = mv->ktb[k];
            if(ls->obstb[ls->keyp + 1] > 0) {
                mv->card[i] = '+';
                if(mv->debug == 5) {
                    printf("Debug5! ls->keyp=%d ls->trnsv=%d\n",
                        ls->keyp,ls->trnsv);
                }
            }
        }
        if(mv->card[i] == ' ') {
            ls->keyp = mv->ktb[k];
            if(ls->obstb[ls->keyp - 1] > 0) {
                mv->card[i] = '-';
                if(mv->debug == 5) {
                    printf("Debug5! ls->keyp=%d ls->trnsv=%d\n",
                        ls->keyp,ls->trnsv);
                }
            }
        }
        k++;
    }
    return(0);
}

int compchk_mlk(union MCODE *um,unsigned char iword[4])
//union MCODE *um;
//unsigned char iword[4];
{
    if(ls->comptyp == 0) {
        um->wc[0] = iword[1];
        um->wc[1] = iword[0];
        um->wc[2] = iword[3];
        um->wc[3] = iword[2];
    }
    if(ls->comptyp == 1) {
        um->wc[0] = iword[0];
        um->wc[1] = iword[1];
        um->wc[2] = iword[2];
        um->wc[3] = iword[3];
    }
    return(0);
}

/*   FB code ....................................
Bar          : FBxxyyyy
 xx(sequ)    : movement number
 yyyy(bnum)  : bar number
 ex. -m0100017-_____________..
         xx(sequ) = 1
         yyyy(bnum) = 17
*/
int fbdecode_mlk(union MCODE *u)
//union MCODE *u;
{
    ls->sequ = u->wc[0] & 0xff;
    ls->bnum = u->wi[1];
    ls->meas = ls->sequ * 10000 + ls->bnum;
    if(mv->debug == 5) {
        printf("FB(%04x%04x) Bar data / sequ=%d bnum=%d meas=%ld\n",
           u->wi[0],u->wi[1],ls->sequ,ls->bnum,ls->meas);
    }
    return(0);
}

/*   FC code ....................................
Synchronize  : FCxxxxxx
 xxxxxx(sync): synchronize
 ex. $4/4 = 400000 start value
*/
int fcdecode_mlk(union MCODE *u)
//union MCODE *u;
{
    union {
        short wi[2];
        long wl;
    } w;

    w.wi[0] = u->wi[1];
    w.wi[1] = u->wi[0] & 0x00ff;
    ls->sync = w.wl;
    if(mv->debug == 5) {
        printf("FC(%04x%04x) Synchronize / sync=%ld\n",
            u->wi[0],u->wi[1],ls->sync);
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
int fddecode_mlk(union MCODE *u)
//union MCODE *u;
{
    ls->fdtyp = u->wc[0] & 0xf0;
    ls->fdtyp = ls->fdtyp >> 4;
    ls->fdch = u->wc[0] & 0x0f;
    switch (ls->fdtyp) {
        case  8 : ls->keyp = u->wc[3] & 0xff;
                  ls->voff = u->wc[2] & 0xff;
                  ls->obstb[ls->keyp]--;
                  if(mv->debug == 5) {
                      printf("FD(%04x%04x) Note off / fdtyp=%d fdch=%d",
                          u->wi[0],u->wi[1],ls->fdtyp,ls->fdch);
                      printf(" keyp=%d voff=%d obstb[%d]=%d\n",
                          ls->keyp,ls->voff,ls->keyp,ls->obstb[ls->keyp]);
                  }
                  break;
        case  9 : ls->keyp = u->wc[3] & 0xff;
                  ls->veon = u->wc[2] & 0xff;
                  if(ls->veon) ls->obstb[ls->keyp]++;
                  else ls->obstb[ls->keyp]--;
                  if(mv->debug == 5) {
                      printf("FD(%04x%04x) Note on / fdtyp=%d fdch=%d",
                          u->wi[0],u->wi[1],ls->fdtyp,ls->fdch);
                      printf(" keyp=%d veon=%d obstb[%d]=%d\n",
                          ls->keyp,ls->veon,ls->keyp,ls->obstb[ls->keyp]);
                  }
                  break;
        default : break;
    }
    return(0);
}

/*   DD code ....................................
File          : DDxxyyzz
 xx(port)     : port
 yy(chan)     : channel
 zz(memb)     : member
*/
dddecode_mlk(union MCODE *u)
//union MCODE *u;
{
    ls->port = u->wc[0] & 0xff;
    ls->trak = u->wc[3] & 0xff;
    ls->memb = u->wc[2] & 0xff;
    if(mv->debug == 5) {
        printf("DD(%04x%04x) File / port=%d chan=%d memb=%d\n",
            u->wi[0],u->wi[1],ls->port,ls->trak,ls->memb);
    }
    return(0);
}
