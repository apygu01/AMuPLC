
// ----------------------
//  AMuPLC (meb5.c)
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
extern MIVMXM *mx;
extern MEBVAR *mb;
extern MEBEDT *ge;
extern MEBANS *ma;
extern MEBCPS *cp;
extern char prsynpmd[];
extern char prsyndat[];
char mcline[36];
//float fnumdin_meb();
extern char *prtsym;
//extern FILE *fopen_h();

int mksym_miv(struct mivksn *mk)
//struct mivksn *mk;
{
    int  i,j;

    /* Major */
    mk->keysin[0][0] = "C";
    mk->keysin[0][1] = "G";
    mk->keysin[0][2] = "D";
    mk->keysin[0][3] = "A";
    mk->keysin[0][4] = "E";
    mk->keysin[0][5] = "H";
    mk->keysin[0][6] = "Fis";
    mk->keysin[0][7] = "Cis";
    mk->keysin[0][8] = "F";
    mk->keysin[0][9] = "B";
    mk->keysin[0][10] = "Es";
    mk->keysin[0][11] = "As";
    mk->keysin[0][12] = "Des";
    mk->keysin[0][13] = "Ges";
    mk->keysin[0][14] = "Ces";
    /* Minor */
    mk->keysin[1][0] = "A";
    mk->keysin[1][1] = "E";
    mk->keysin[1][2] = "H";
    mk->keysin[1][3] = "Fis";
    mk->keysin[1][4] = "Cis";
    mk->keysin[1][5] = "Gis";
    mk->keysin[1][6] = "Dis";
    mk->keysin[1][7] = "Ais";
    mk->keysin[1][8] = "D";
    mk->keysin[1][9] = "G";
    mk->keysin[1][10] = "C";
    mk->keysin[1][11] = "F";
    mk->keysin[1][12] = "B";
    mk->keysin[1][13] = "Es";
    mk->keysin[1][14] = "As";
    if(mv->debug == 2) {
        for(i = 0; i < 2; i++) {
            for(j = 0; j < 15 ; j++) 
            printf("mk->keysin[%d][%d] = %s\n",i,j,mk->keysin[i][j]);
        }
    }
    return(0);
}

/* check main file xxx_x00.xxx 
        00 : mv->mfflg = 1;
*/
int chkmfile_miv(char *fname)
//char *fname;
{
    char c1,c2;
    char *pc;
    int  i,j;

    i = 0;
    pc = fname;
    while(*pc) {
        i++;
        pc++;
    }
    j = 0;
    pc = fname;
    while(*pc != '_') {
        pc++;
        j++;
        if( j >= i) break;
    }
    pc = pc + 2;
    c1 = *pc;
    pc++;
    c2 = *pc;
    if(c1 == '0' && c2 == '0') return(1); /* xxx_x00.xxx file(main) */
    return(0);
}

/* {%xx} %xx _/>> ni taishite xx wo setei suru.
   {xxxxxxx xx} no syori
*/
int beauexp_meb()
{
    int  i,j;
    int  rtn;
    int  sp;
    char wk[4];
    char cw[82];

    if(mv->card[1] == '%') {
        cw[0] = mv->card[2];
        cw[1] = mv->card[3];
        cw[2] = '\0';
        sscanf(cw,"%d",&mb->bexp);
        if(mv->card[4] != '}')
        if(!mv->pass) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error! %05d \"%s\"\n", mv->line, mv->card);
        }
        return(1);
    }
    if(mv->card[1] == 'm' && mv->card[2] == '.' && mv->card[3] == 'm') {
        exptmp_meb();         /* {m.m =/ = xx } */
        mb->bexpsf = 0;
        return(0);
    }
    if(mv->card[1] == '/') {
        beautemp_meb();
        mb->bexpsf = 0;     /* beautify expressin symbol serch off */
        return(0);
    }
    sp = 0;
    if(mv->card[0] == '>') sp = 1;
    for(i = 0; i < 80; i++) {
        cw[i] = tolower(mv->card[sp]);
        if(mv->card[sp] == '}') break;
        sp++;
    }
    cw[++i] = '\0';
    rtn = beauexp_sub1(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub2(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub3(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub4(cw);
    if(!rtn) return(0);
    rtn = beauexp_sub5(cw);
    if(!rtn) return(0);
    if(mv->card[1] == 'G' || mv->card[1] == 'g') {
        rtn = strck_meb(); /* Global */
        if(!rtn) return(0);
    }
    if(mv->card[1] == 'S' || mv->card[1] == 's') {
        rtn = strck_meb(); /* Global */
        if(!rtn) return(0);
    }
    if(mv->card[1] == '#') {
        rtn = strck_meb();
        return(rtn);
    }
    return(rtn);
}

int beauexp_sub1(char *cw)
//char *cw;
{
    if(!strcmp(cw,"{generate o note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate o/. note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 2;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate o/ note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 4;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/ note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 4;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/) note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 8;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/)) note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 16;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/))) note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 32;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/)))) note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 64;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/))))) note lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 128;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate rest lines}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 1000;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate global chord lines}")) {
        mb->gchdf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate global chord select lines}")) {
        mb->gchdsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 4;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate o note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 1;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate o/. note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 2;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate o/ note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 3;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/ note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 4;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/) note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 8;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/)) note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 16;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/))) note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 32;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/)))) note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 64;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate _/))))) note lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 128;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate rest lines all}")) {
        mb->mnflg1 = 1;
        mb->gentyp = 1000;
        mb->amnflg1 = 1;
        beaucsym_meb();
        return(0);
    }
    return(1);
}

int beauexp_sub2(char *cw)
//char *cw;
{
    if(!strcmp(cw,"{modify dumper signs}")) {
        mv->dflg = 'm';
        mb->bexpsf = 0;     /* beautify expressin symbol serch off */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{change dumper signs}")) {
        mv->dflg = 'c';
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete dumper signs}")) {
        mv->dflg = 'r';
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete note lines}")) {
        mb->clrntl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete rest lines}")) {
        mb->clrstl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete symbol lines}")) {
        mb->clrsml = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete plus lines}")) {
        mb->clrpls = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete comment lines}")) {
        mb->clrcom = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete chord lines}")) {
        mb->clrchd = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete chord check lines}")) {
        mb->delchk = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete global chord select lines}")) {
        mb->gchdlf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete expand lines}")) {
        mb->delexp = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete dumper signs}")) {
        mb->clrdpl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete note lines all}")) {
        mb->clrntl = 1;
        mb->aclrntl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete rest lines all}")) {
        mb->clrstl = 1;
        mb->aclrstl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete symbol lines all}")) {
        mb->clrsml = 1;
        mb->aclrsml = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete plus lines all}")) {
        mb->clrpls = 1;
        mb->aclrpls = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete comment lines all}")) {
        mb->clrcom = 1;
        mb->aclrcom = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete chord check lines all}")) {
        mb->delchk = 1;
        mb->adelchk = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete chord lines all}")) {
        mb->clrchd = 1;
        mb->aclrchd = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete expand lines all}")) {
        mb->delexp = 1;
        mb->adelexp = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete dumper signs all}")) {
        mb->clrdpl = 1;
        mb->aclrdpl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{renumber bar lines}")) {
        mb->renumf = 1;
        mb->xrenumf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{renumber bar lines all}")) {
        mb->renumf = 1;
        mb->arenumf = 1;
        beaucsym_meb();
        return(0);
    }
    /* add 2011.11.9 */
    if(!strcmp(cw,"{renumber bar lines continuously all}")) {
        mb->renumf = 1;
        mb->arenumf = 1;
        mb->crenumf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete dumper signs}")) {
        mv->dflg = 'd';
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete large brackets lines}")) {
        mb->lbdflg = 1;       /* beautify symbol delete on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete parenthesis lines}")) {
        mb->pbdflg = 1;       /* beautify symbol delete on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete square lines}")) {
        mb->sbdflg = 1;       /* beautify symbol delete on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete large brackets lines all}")) {
        mb->lbdflg = 1;       /* beautify symbol delete on */
        mb->albdflg = 1;      /* beautify symbol delete all on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete parenthesis lines all}")) {
        mb->pbdflg = 1;       /* beautify symbol delete on */
        mb->apbdflg = 1;      /* beautify symbol delete all on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{delete square lines all}")) {
        mb->sbdflg = 1;       /* beautify symbol delete on */
        mb->asbdflg = 1;      /* beautify symbol delete all on */
        beaucsym_meb();
    }
    return(1);
}

int beauexp_sub3(char *cw)
//char *cw;
{
    if(!strcmp(cw,"{exchange parenthesis to large brackets}")) {
        mb->xstolf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange parenthesis to large brackets all}")) {
        mb->xstolf = 1;
        mb->axstolf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange large brackets to parenthesis}")) {
        mb->xltosf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange large brackets to parenthesis all}")) {
        mb->xltosf = 1;
        mb->axltosf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange global chord select to progression lines}")) {
        mb->gchdxf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange global chord select to progression lines all}")) {
        mb->gchdxf = 1;
        mb->agchdxf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange global chord progression to select lines}")) {
        mb->gchdxf = 2;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange global chord progression to select lines all}")) {
        mb->gchdxf = 2;
        mb->agchdxf = 2;
        beaucsym_meb();
        return(0);
    }
    /* printf("Passcw=%s step=%d\n",cw,mv->step); */
    if(!strcmp(cw,"{observe chord_name voicing}")) {
        cp->scnvf = mv->step;
        mb->bexpsf = 0;
        if(mv->step == 4) beaucsym_meb();
        return(0);
    }
    /*
    if(!strcmp(cw,"{observe chord_name voicing all}")) {
        cp->scnvf = mv->step;
        cp->ascnvf = mv->step;
        mb->bexpsf = 0;
        if(mv->step == 3) beaucsym_meb();
        return(0);
    }
    */
    if(!strcmp(cw,"{change chord progression lines}")) {
        cp->chgf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{change chord progression lines all}")) {
        cp->chgf = 1;
        cp->achgf = 1;
        beaucsym_meb();
        return(0);
    }
    return(1);
}

int beauexp_sub4(char *cw)
//char *cw;
{
    if(!strcmp(cw,"{exchange beautify values}")) {
        mb->bexchf = 1;      /* beautify exchange on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset beautify values}")) {
        mb->bexchf = 1;       /* beautify exchange on */
        mb->gaterf = 1;       /* gate_rate */
        mb->pitchf = 1;       /* pitchvend */
        mb->modulf = 1;       /* modulation */
        mb->cntlnf = 1;       /* countrole number */
        mb->cntldt = 1;       /* countrole data   */
        mb->transvf = 1;      /* transpose */
        mb->veltef = 1;       /* velocity_base value */
        mb->clvelf = 1;       /* cler velocity flag */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange beautify values all}")) {
        mb->bexchf = 1;       /* beautify exchange on */
        mb->abexchf = 1;      /* beautify exchange all on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset beautify values all}")) {
        mb->bexchf = 1;       /* beautify exchange on */
        mb->abexchf = 1;      /* beautify exchange all on */
        mb->gaterf = 1;       /* gate_rate */
        mb->agaterf = 1;      /* gate_rate */
        mb->pitchf = 1;       /* pitchvend */
        mb->apitchf = 1;      /* all pitchvend */
        mb->modulf = 1;       /* modulation */
        mb->amodulf = 1;      /* all modulation */
        mb->cntlnf = 1;       /* countrole number */
        mb->acntlnf = 1;      /* all countrole number */
        mb->cntldf = 1;       /* countrole data   */
        mb->acntldf = 1;      /* all countrole data   */
        mb->transvf = 1;      /* transpose */
        mb->atransvf = 1;     /* all transpose */
        mb->veltef = 1;       /* velocity_base value */
        mb->aveltef = 1;      /* all velocity_base value */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset gate_rate values}")) {
        mb->gaterf = 1;       /* reset gate_rate on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset gate_rate values all}")) {
        mb->gaterf = 1;       /* reset gate_rate on */
        mb->agaterf = 1;      /* all reset gate_rate  on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset pitch_vend values}")) {
        mb->pitchf = 1;       /* beautify pitch_vend on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset pitch_vend values all}")) {
        mb->pitchf = 1;       /* beautify pitch_vend on */
        mb->apitchf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset modulation values}")) {
        mb->modulf = 1;       /* beautify modulation on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset modulation values all}")) {
        mb->modulf = 1;       /* beautify modulation on */
        mb->amodulf = 1;
        beaucsym_meb();
        return(0);
    }
    /* support spell miss 2011.6 */
    if(!strcmp(cw,"{reset moduration values}")) {
        mb->modulf = 1; 
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset moduration values all}")) {
        mb->modulf = 1; 
        mb->amodulf = 1;
        beaucsym_meb();
        return(0);
    }
    /* -------- */
    if(!strcmp(cw,"{reset control_number values}")) {
        mb->cntlnf = 1;       /* beautify control_number on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset control_number values all}")) {
        mb->cntlnf = 1;       /* beautify control_number on */
        mb->acntlnf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset control_data values}")) {
        mb->cntldf = 1;       /* beautify control_data on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset control_data values all}")) {
        mb->cntldf = 1;       /* beautify control_data on */
        mb->acntldf = 1;
        beaucsym_meb();
        return(0);
    }
    /* support spell miss */
    if(!strcmp(cw,"{reset moduration values}")) {
        mb->modulf = 1;       /* beautify modulation on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset moduration values all}")) {
        mb->modulf = 1;       /* beautify modulation on */
        mb->amodulf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset controle_number values}")) {
        mb->cntlnf = 1;       /* beautify control_number on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset controle_number values all}")) {
        mb->cntlnf = 1;       /* beautify control_number on */
        mb->acntlnf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset controle_data values}")) {
        mb->cntldf = 1;       /* beautify control_data on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset controle_data values all}")) {
        mb->cntldf = 1;       /* beautify control_data on */
        mb->acntldf = 1;
        beaucsym_meb();
        return(0);
    }
    /* ---- */
    if(!strcmp(cw,"{reset program_change values}")) {
        mb->prgchgf = 1;       /* beautify program_change_data on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset program_change values all}")) {
        mb->prgchgf = 1;       /* beautify program_change_data on */
        mb->aprgchgf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset transpose values}")) {
        mb->transvf = 1;       /* beautify transpose values on */
        mb->tpv = 0;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset transpose values all}")) {
        mb->transvf = 1;       /* beautify transpose values on */
        mb->atransvf = 1;
        mb->tpv = 0;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset velocity_base values}")) {
        mb->veltef = 1;        /* beautify velocity_base value on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset velocity_base values all}")) {
        mb->veltef = 1;        /* beautify velocity_base value on */
        mb->aveltef = 1;       /* beautify velocity_base value on */
        beaucsym_meb();
        return(0);
    }
    return(1);
}

int beauexp_sub5(char *cw)
//char *cw;
{
    int  i;

    if(!strcmp(cw,"{generate beautify symbols}")) {
        mb->bstflg = 1;      /* cond's beauttify copy flag on */
        /* mb->lbdflg = 1;       beautify symbol delete on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{generate beautify symbols all}")) {
        mb->abstflg = 1;     /* cond's beauttify copy all flag on */
        /* mb->albdflg = 1;      beautify symbol delete all on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange beautify symbols all}")) {
        mb->abstflg = 1;     /* cond's beauttify copy all flag on */
        mb->albdflg = 1;      /* beautify symbol delete all on */
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange midi format to cueing format}")) {
        mb->midi = 0;
        mb->cueing = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{exchange cueing format to midi format}")) {
        mb->midi = 1;
        mb->cueing = 0;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset tempo values}")) {
        mb->rstmpf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset tempo values all}")) {
        mb->rstmpf = 1;
        mb->arstmpf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset smpte values}")) {
        mb->rssmptf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset smpte values all}")) {
        mb->rssmptf = 1;
        mb->arssmptf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset dynamic values}")) {
        mb->rsdynaf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset dynamic values all}")) {
        mb->rsdynaf = 1;
        mb->arsdynaf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{observe other notes}")) {
        mb->observf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{observe other notes all}")) {
        mb->observf = 1;
        mb->aobservf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{observe chord notes}")) {
        mb->chdobf = 1;
        ma->nsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset chord notes}")) { /* for old .meg source */
        mb->chdobf = 3;  // change 2017.09.29
        ma->nsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{change chord notes}")) {
        mb->chdobf = 3;  // change 2017.09.29
        ma->nsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset chord & available notes}")) {
        mb->chdobf = 2;
        ma->nsf = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{observe chord notes all}")) {
        mb->chdobf = 1;
        mb->achdobf = 1;
        ma->nsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset chord & availeble notes all}")) {
        mb->chdobf = 2;
        mb->achdobf = 2;
        ma->nsf = 3;
        ma->ansf = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{observe available note scale}")) {
        ma->nsf = 2;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{observe available note scale all}")) {
        ma->nsf = 2;
        ma->ansf = 2;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{show expanded note lines}")) {
        mb->expnl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{show expanded note lines all}")) {
        mb->expnl = 1;
        mb->aexpnl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{hide expanded note lines}")) {
        mb->expnl = 2;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{hide expanded note lines all}")) {
        mb->expnl = 2;
        mb->aexpnl = 2;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{hide chord progression lines}")) {
        mb->hcpl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{hide chord progression lines all}")) {
        mb->hcpl = 1;
        mb->ahcpl = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset expand symbols}")) {
        mb->expnl = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset expand symbols all}")) {
        mb->expnl = 3;
        mb->aexpnl = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset exp symbols}")) {
        mb->expnl = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset exp symbols all}")) {
        mb->expnl = 3;
        mb->aexpnl = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{clear p_exp symbols}")) {
        mb->expnl = 7;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{clear p_exp symbols all}")) {
        mb->expnl = 7;
        mb->aexpnl = 7;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset dynamic expand symbols}")) {
        mb->dexpcl = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reset dynamic expand symbols all}")) {
        mb->dexpcl = 3;
        mb->adexpcl = 3;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{decode dynamic expand note lines}")) {
        mb->decxp = 1;
        i = 35;
        while(1) {
            if(mv->card[i] == '=') {
                i++;
                while(1) {
                    if(mv->card[i] != ' ') {
                        mb->rbtch = mv->card[i];
                        i = 47;
                        break;
                    }
                    i++;
                    if(i > 47) break;
                }
            }
            i++;
            if(i > 47) break;
        }
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{save expanded note lines}")) {
        mb->expnl = 4;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{save expanded note lines all}")) {
        mb->expnl = 4;
        mb->aexpnl = 4;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{include expanded note lines}")) {
        mb->expnl = 5;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{include expanded note lines all}")) {
        mb->expnl = 5;
        mb->aexpnl = 5;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reload expand files}")) {
        mb->expnl = 6;
        mb->expff = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{reload expand files all}")) {
        mb->expnl = 6;
        mb->aexpnl = 6;
        mb->expff = 1;
        mb->aexpff = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{store expand files}")) {
        /* fn_exp.mph -> fn.mph */
        mb->expstf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{store expand files all}")) {
        /* fn_exp.mph -> fn.mph */
        mb->expstf = 1;
        mb->aexpstf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{save & store expand files}")) {
        /* fn_exp.mph -> fn.mph */
        mb->expnl = 4;
        mb->expstf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{save & store expand files all}")) {
        /* fn_exp.mph -> fn.mph */
        mb->expnl = 4;
        mb->aexpnl = 4;
        mb->expstf = 1;
        mb->aexpstf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{register expand files}")) {
        /* fn_exp.mph -> meb.mph */
        mb->exprsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{register expand files all}")) {
        /* fn_exp.mph -> meb.mph */
        mb->exprsf = 1;
        mb->aexprsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{save & register expand files}")) {
        /* fn_exp.mph -> meb.mph */
        mb->expnl = 4;
        mb->exprsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{save & register expand files all}")) {
        /* fn_exp.mph -> meb.mph */
        mb->expnl = 4;
        mb->aexpnl = 4;
        mb->exprsf = 1;
        mb->aexprsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{clear notes}")) {
        mb->clrntsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{clear notes all}")) {
        mb->clrntsf = 1;
        mb->aclrntsf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{clear observed notes}")) {
        mb->clrontf = 1;
        mb->aclrontf = 1;
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(cw,"{clear observed notes all}")) {
        mb->clrontf = 1;
        mb->aclrontf = 1;
        beaucsym_meb();
        return(0);
    }
    return(1);
}

int beaucsym_meb()
{
    int  i,j;
    char cw[82];

    mb->bexpsf = 0;
    if(mv->card[0] == '>') return(0);
    if(mv->mfflg && mv->step < 1) return(0);
    /* if(mv->mfflg && mv->step <= 1) return(0); ? */
    if(mv->card[0] == '#' && mv->step != 4) return(0); /* 2011.6.27 */
    /*
    strcpy(cw,mv->card);
    mv->card[0] = ' ';
    mv->card[1] = '\0';
    strcat(mv->card,cw);
    if(strlen(mv->card) > 79) mv->card[79] = '\0';
    */
    strcpy(cw,mv->card);
    mv->card[0] = ' ';
    for(i = 1,j = 0; i < 82; i++,j++) {
        mv->card[i] = cw[j];
        if(cw[j] == '\0') break;
    }
    return(0);
}

int beautemp_meb()
{
    int  i,j;
    int  k;
    int  eq;
    int  wfg;
    char c;
    char wk[80];

    k = 0;
    eq = 0;
    wfg = 0;
    for(i = 1; i < 80 ; i++) {
        if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
        if(mv->card[i] != ' ') {
            wk[k] = mv->card[i];
            /* if(wk[k] == '}') wk[k] = ')'; */
            k++;
        }
    }
    mv->card[0] = '\0';
    /* strcat(mv->card,"[m.m. "); */
    strcat(mv->card,"{m.m. ");
    mv->card[6] = ' ';
    i = 0;
    while(wk[i] != '/') {
        i++;
        if(i > 10) {
            if(!mv->pass) {
                mv->errcnt++;
                mv->msglvl = 3;
                if(mv->msglvl >= mv->prtlvl)
                printf("Error! %05d beautify tempo\n",mv->line);
            }
            return(1);
        }
    }
    i = 0;
    j = 7;
    while(!isdigit(wk[i])) {
        if(wk[i] == '=') {
            mv->card[j++] = ' ';
            eq = 1;
        }
        mv->card[j] = wk[i];
        i++;
        j++;
    }
    if(!eq) {
        mv->card[j++] = ' ';
        mv->card[j++] = '=';
    }
    mv->card[j++] = ' ';
    while(isdigit(wk[i])) {
        mv->card[j] = wk[i];
        i++;
        j++;
    }
    /* mv->card[j++] = ')'; */
    mv->card[j++] = '}';
    for(i = 8 ; i < j ; i++) {
        if(mv->card[i] == '/') mv->card[i] = ')';
        if(mv->card[i] == 'w') mv->card[i] = ' ';
        if(mv->card[i] == 'h') mv->card[i] = ' ';
        if(mv->card[i] == 'd') mv->card[i] = ' ';
    }
    c = wk[1];
    mv->card[6] = '_';
    if(c == 'h') {
        mv->card[6] = 'o';
        mv->card[7] = '/';
    }
    if(c == 'w') {
        mv->card[6] = 'O';
        mv->card[7] = ' ';
    }
    if(c == 'd') {
        mv->card[6] = '|';
        mv->card[7] = 'O';
        mv->card[8] = '|';
    }
    k = 0;
    for(i = 0; i < j; i++) {
        if(mv->card[i] != ' ') {
            wk[k++] = mv->card[i];
            wfg = 1;
        }
        else {
            if(wfg) wk[k++] = ' ';
            wfg = 0;
        }
    }
    j = 0;
    for(i = 0 ; i < k ; i++) {
        if(wk[i] == ' ' && wk[i+1] == '.') {
            i++;
            mv->card[j++] = wk[i++];
            mv->card[j++] = wk[i];
        }
        else mv->card[j++] = wk[i];
    }
    mv->card[j] = '\0';
    exptmp_meb();
    return(0);
}

int exptmp_meb()
{
    int  rtn;

    rtn = exptmp2_meb();
    if(rtn <= 0) return(0);
    if(!mv->pass) {
        mv->wrncnt++;
        mv->msglvl = 2;
        if(mv->msglvl >= mv->prtlvl)
        if(rtn == 1) {
            printf("Worning! %05d _/))))) is max.\n",mv->line);
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
        if(rtn == 4) printf("Worning! %05d tempo \'=\' symbol not found\n",
            mv->line);
    }
    return(rtn);
}

int exptmp2_meb()
{
    int  i,j;
    int  k,l;
    long ll;
    float tempo;
    char wk[10];
    long nvt[22];

    nvt[0] = 800000;    /* 0 : |O| */
    nvt[1] = 700000;    /* 1 : O.. */
    nvt[2] = 600000;    /* 2 : O.  */
    nvt[3] = 400000;    /* 3 : whole */
    nvt[4] = 350000;    /* 4 : o/.. */
    nvt[5] = 300000;    /* 5 : o/.  */
    nvt[6] = 200000;    /* half */
    nvt[7] = 175000;    /* _/.. */
    nvt[8] = 150000;    /* _/.  */
    nvt[9] = 100000;    /* 4th */
    nvt[10] = 87500;    /* _/).. */
    nvt[11] = 75000;    /* _/).  */
    nvt[12] = 50000;    /* 8th */
    nvt[13] = 43750;    /* _/)).. */
    nvt[14] = 37500;    /* _/)). */
    nvt[15] = 25000;    /* 16th */
    nvt[16] = 21875;    /* _/))).. */
    nvt[17] = 18750;    /* _/))). */
    nvt[18] = 12500;    /* 32th */
    nvt[19] =  9375;    /* _/)))). */
    nvt[20] =  6250;    /* 64th */
    nvt[21] =  3125;    /* 128th */
    i = 0;
    k = 0;
    l = 0;
    mb->nval = 0;
    while(mv->card[k]) k++;
    while(mv->card[i] != ' ') i++;
    i++;
    if(mv->card[i] == '_') {
        i = i + 2;
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
    }
    if(mv->card[i] == 'O') {
        i++;
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
    }
    if(mv->card[i] == 'o') {
        mb->nval = 200000;
        i = i + 2;
        if(mv->card[i] == '.') {
            ll = mb->nval / 2;
            mb->nval = ll + mb->nval;
            i++;
            if(mv->card[i] == '.') {
                mb->nval = ll/2 + mb->nval;
                i++;
            }
        }
    }
    if(mv->card[i] == '|') {
        i = i + 3;
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
    }
    while(mv->card[i] != '=') {
        i++;
        if(i > k) return(4);
    }
    while(!isdigit(mv->card[i])) i++;
    j = 0;
    while(isdigit(mv->card[i]) || mv->card[i] == '.') {
        wk[j] = mv->card[i];
        i++;
        j++;
    }
    wk[j] = '\0';
    sscanf(wk,"%f",&tempo);
    for(j = 0; j < 21; j++) if(mb->nval == nvt[j]) break;
    mv->tempo = tempo * (float)nvt[j] / 10000.0;      /* *10 */
    /* printf("mv->tempo = %f\n",mv->tempo); */
    return(0);
}

/* bar no pedal no bubun ni ..... wo settei suru. */
int beauadd_meb()
{
    int  i;
    char *ps;

    for(i = 0; i < 80 ; i++) {
        if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
    }
    if(mb->bsyntyp == 'v') {
        if((mv->card[21] != ':') && (mv->card[28] != '.')) {
            ps = prsynpmd;
            for(i = 10; i < 32; i++) {
                mv->card[i] = *ps;
                ps++;
            }
        }
    }
    mv->card[75] = '_';  /* ' ' -> '_' 98.1 */
    if(mv->card[70] == ' ') {
        ps = prsyndat;
        for(i = 69; i < 80; i++) {
            mv->card[i] = *ps;
            ps++;
        }
    }
    if(mv->dflg == 'm')
        if(mv->card[mv->dpos] == '.') mv->card[mv->dpos] = mv->dped;
    if(mv->dflg == 'c') mv->card[mv->dpos] = mv->dped;
    if(mv->dped == '@') mv->dped = ':';
    if(mv->dped == '*') mv->dped = '.';
    if(mv->dflg == 'r') mv->card[mv->dpos] = '.';
    mv->card[79] = '\0';
    return(0);
}
/* %xx nitaishite {%xx} no sitei ga areba henkou suru. */
int beaumod_meb()
{
    int  i;
    int  asc;
    int  rtn;
    char bwk[4];
    char *ps;
    int  passf;

    if(mv->debug == 13) printf("Debug13! beaumod_() step=%d mb->gatrf=%d ",
        mv->step,mb->gaterf);
    for(i = 0; i < 79 ; i++) {
        if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
    }
    passf = 0;
    if(mv->card[1] == ' ' || 
        mv->card[2] == 'x' || mv->card[2] == 'X') {
        gate_rate_get(); /* get mb->bexp */
        sprintf(bwk,"%02d",mb->bexp);
        mv->card[1] = bwk[0];
        mv->card[2] = bwk[1];
        passf = 1;
        if(mv->debug == 13)
            printf("mb->bexp=%d\n",mb->bexp);
    }
    if(ge->asn_gr_f) {  /* #assign */
        if(ge->asn_gr_f > 10) {
            sprintf(bwk,"%02d",ge->asn_bexp);
            mv->card[1] = bwk[0];
            mv->card[2] = bwk[1];
        }
	else {
            rtn = asn_gr_get(); /* get ge->asn_bexp */
            if(rtn == 0) {
                sprintf(bwk,"%02d",ge->asn_bexp);
                mv->card[1] = bwk[0];
                mv->card[2] = bwk[1];
                if(mv->debug == 13) printf("++++Pass ");
            }
	}
        passf = 1;
        if(mv->debug == 13)
            printf("rtn=%d ge->asn_gr_f=%d ge->asn_bexp=%d\n",
            rtn,ge->asn_gr_f,ge->asn_bexp);
    }
    if(ge->mod_gr_f) {
        if(ge->mod_gr_f > 10) {
            sprintf(bwk,"%02d",ge->mod_bexp);
            mv->card[1] = bwk[0];
            mv->card[2] = bwk[1];
        }
	else {
            rtn = mod_gr_get(); /* get ge->mod_bexp */
            if(rtn == 0) {
                sprintf(bwk,"%02d",ge->mod_bexp);
                mv->card[1] = bwk[0];
                mv->card[2] = bwk[1];
                if(mv->debug == 13) printf("++++Pass ");
            }
	}
        passf = 1;
        if(mv->debug == 13)
            printf("rtn=%d ge->mod_gr_f=%d ge->mod_bexp=%d\n",
            rtn,ge->mod_gr_f,ge->mod_bexp);
    }
    if(ge->set_gr_f) {
        if(ge->set_gr_f > 10) {
            sprintf(bwk,"%02d",ge->set_bexp);
            mv->card[1] = bwk[0];
            mv->card[2] = bwk[1];
        }
	else {
            rtn = set_gr_get(); /* get ge->set_bexp */
            if(rtn == 0) {
                sprintf(bwk,"%02d",ge->set_bexp);
                mv->card[1] = bwk[0];
                mv->card[2] = bwk[1];
                if(mv->debug == 13) printf("++++Pass ");
            }
	}
        passf = 1;
        if(mv->debug == 13)
            printf("rtn = %d ge->set_gr_f=%d ge->set_bexp=%d\n",
            rtn,ge->set_gr_f,ge->set_bexp);
    }
    if(ge->asc_gr_f) {
        if(ge->asc_gr_f > 10) {
            bwk[0] = mv->card[1];
            bwk[1] = mv->card[2];
            bwk[2] = '\0';
            sscanf(bwk,"%02d",&asc);
	    if(asc == 0) asc = 100;
            /* printf("asc = %02d ge->asc_bexp = %d\n",asc,ge->asc_bexp); */
            if(ge->asc_gr_f == 12) asc = asc + ge->asc_bexp;
            if(ge->asc_gr_f == 13) asc = asc - ge->asc_bexp;
            if(ge->asc_gr_f == 14) asc = ge->asc_bexp;
            if(ge->asc_gr_f == 15) asc = ge->asc_bexp;
	    if(asc <= 0) asc = 1;
	    if(asc > 99) asc = 0;  /* 100 -> %00 */
            sprintf(bwk,"%02d",asc);
            mv->card[1] = bwk[0];
            mv->card[2] = bwk[1];
            passf = 1;
            if(mv->debug == 13) printf("ge->asc_gr_f=%d asc=%d\n",
                ge->asc_gr_f,asc);
        }
	else {
            if(ge->asc_gr_f == 2 || ge->asc_gr_f == 3) { /* reset/crear */
                rtn = asc_gr_get(); /* get ge->asc_bexp */
                if(rtn == 0) {
                    bwk[0] = mv->card[1];
                    bwk[1] = mv->card[2];
                    bwk[2] = '\0';
                    sscanf(bwk,"%02d",&asc);
	            if(asc == 0) asc = 100;
                    /*
                    printf("asc = %02d ge->asc_bexp = %d\n",asc,ge->asc_bexp);
                    */
                    if(ge->asc_gr_f == 2) asc = asc + ge->asc_bexp;
                    if(ge->asc_gr_f == 3) asc = asc - ge->asc_bexp;
		    if(asc <= 0) asc = 1;
	            if(asc > 99) asc = 0;  /* 100 -> %00 */
                    sprintf(bwk,"%02d",asc);
                    mv->card[1] = bwk[0];
                    mv->card[2] = bwk[1];
                    if(mv->debug == 13) printf("++++Pass1 ");
                }
            }
            if(ge->asc_gr_f == 4 || ge->asc_gr_f == 5) { /* reset/crear */
                rtn = def_gr_get(); /* get ge->def_bexp */
                if(rtn == 0) {
                    sprintf(bwk,"%02d",ge->def_bexp);
                    mv->card[1] = bwk[0];
                    mv->card[2] = bwk[1];
                    if(mv->debug == 13) printf("++++Pass2 ");
                }
            }
            passf = 1;
            if(mv->debug == 13)
                printf("rtn=%d ge->asc_gr_f=%d ge->def_bexp=%d\n",
                rtn,ge->asc_gr_f,ge->def_bexp);
	}
    }
    /* if(ge->def_gr_f && mb->gaterf) {  #define */
       if(passf == 0 && mb->gaterf) { /* #define */ 
        if(ge->def_gr_f > 10) {
            sprintf(bwk,"%02d",ge->def_bexp);
            mv->card[1] = bwk[0];
            mv->card[2] = bwk[1];
        }
	else {
            rtn = def_gr_get(); /* get ge->def_bexp */
            if(rtn == 0) {
                sprintf(bwk,"%02d",ge->def_bexp);
                mv->card[1] = bwk[0];
                mv->card[2] = bwk[1];
                if(mv->debug == 13) printf("++++Pass ");
            }
	}
        passf = 1;
        if(mv->debug == 13)
            printf("rtn=%d ge->def_gr_f=%d ge->def_bexp=%d\n",
            rtn,ge->def_gr_f,ge->def_bexp);
    } 
    if(mv->debug == 13) {
        if(passf == 0) printf("\n");
        printf("%s\n",mv->card);
    }
/*
    if(mb->bsyntyp == 'v') {
        if((mv->card[21] != ':') && (mv->card[28] != '.')) {
            ps = prsynpmd;
            for(i = 10; i < 32; i++) {
                mv->card[i] = *ps;
                ps++;
            }
        }
    }
    if(mv->dflg == 'm')
        if(mv->card[mv->dpos] == '.') mv->card[mv->dpos] = mv->dped;
    if(mv->dflg == 'c') mv->card[mv->dpos] = mv->dped;
    if(mv->dped == '@') mv->dped = ':';
    if(mv->dped == '*') mv->dped = '.';
    if(mv->dflg == 'r') mv->card[mv->dpos] = '.';
    mv->card[79] = '\0';
    */
    return(0);
}

int beaupula_meb(char *inf)
//char *inf;
{
    FILE *fic;
    int  i,j;
    char wkc[4];
    char xfile[STLN];
    int  l;
    char mc;
    char wcard[82];

    if(mv->card[0] == 'C' && mv->card[4] == ' ') {
        for(i = 8 ; i < 44; i++) mv->card[i] = '.';
    }
    else {
        strcpy(xfile,inf);
        for(i = 8,j = 0; i < 44 ; i++,j++)
            mv->card[i] = mx->partf[mb->cline][j];
        mv->card[6] = '.';
        i = 8;
        for(j = 0 ; j < 36; j++) {
            mc = mv->card[i];
            if(mb->cline == 0) mcline[j] = mc;
            if(mc == '/') mv->card[i] = '.';
            if(mc == '&' || mc == '*' || mc == '@'
                || mc == '?' || mc == '+') {
		/*
                printf("*(prtsym+j) = %c mb->cline = %d\n",
                    *(prtsym+j),mb->cline);
                */
                l = 0;
                while(xfile[l] != '.' && xfile[l] != '_') l++;
                l++;
                sprintf(wkc,"%02x",mx->track[mb->cline]);
                xfile[l+1] = wkc[0];
                xfile[l+2] = wkc[1];
                xfile[l] = *(prtsym+j);
                /* printf("xfile = %s\n",xfile); */
                if((fic = fopen(xfile,"rb")) != NULL) {
                    while(fgets(wcard, 82, fic)) {
                        if(wcard[2] == '$') break;
                        if(wcard[2] == '_') break;
                        if(wcard[1] == '/' && wcard[78] == '|') break;
                    }
                    /* 
                    if(!isalpha(mv->card[i])) mv->card[i] = tolower(wcard[4]);
                    else mv->card[i] = wcard[4];
                    */ 
                    mv->card[i] = wcard[4];
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

int beauback_meb(char *outf)
//char *outf;
{
    int  i;
    char inf[STLN];
    char *pf;
    FILE *fpi,*fpo;
    
    i = 0;
    pf = outf;
    while(*pf != '.') {
        inf[i] = *pf;
        i++;
        pf++;
    }
    inf[i] = '\0';
    strcat_h(inf,".meb");
    fpo = fopen_h(outf,"wb");
    fpi = fopen_h(inf,"rb");
    if(mv->wflag && (mv->trace || mv->source)) printf("\n");
    while(fgets(mv->card, 82, fpi)) {  /* beauback */
        mv->line++;
        fputs(mv->card,fpo);
        if(mv->wflag && (mv->trace || mv->source))
            printf("%s", mv->card);
        if(mv->card[0] == '$' && mv->card[2] == '$') break; /* EOF */
    }
    if(mv->wflag) 
       printf("Beautify back \"%s\" from \"%s\", line = %d\n",
       outf, inf, mv->line);
    fclose(fpi);
    fclose(fpo);
    return(0);
}

int strck_meb()
{
    int  rtn;

    rtn = strck2_meb();
    if(rtn <= 0) return(0);
    if(!mv->pass) {
        if(rtn) {
            mv->errcnt++;
            mv->msglvl = 3;
            if(mv->msglvl >= mv->prtlvl)
            printf("Error: %05d %s\n",mv->line,mv->card);
            return(rtn);
        }
    }
    return(rtn);
}

int strck2_meb()
{
    int  i,j,k,n;
    char work[80];
    char card[82];
    int  rtn;
    char scrf[100];

    n = strlen(mv->card);
    /* printf("strck2() n = %d\n",n); */
    strcpy(card,mv->card);  /* {#xxx} */
    if(card[0] == '{' || card[0] ==  '(') {
	j = n;
        for(i = n; i > 0; i--) {
            card[i-1] = mv->card[i];
            if(card[i] == '}' ) {
		card[i] = ' ';
		j = i;
	    }
            /*
            if(card[i] == ')' ) {
		card[i] = ' ';
		j = i;
	    }
            */
	    if(tolower(card[i]) == ' ' &&
               tolower(card[i+1]) == 'o' && card[i+2] == 'n') {
                if(mv->mfflg) return(0); /* ignore conducter {..} on xxx */
	    }
        }
        card[j] = '\0';
        /* printf("%s\n",card); */
    }
    j = 0;
    while(card[j] != ' ') {
        work[j] = tolower(card[j]);
        j++;
    }
    work[j] = '\0';
    if(!strcmp(work,"global_transpose")) {
        mb->GTPf = 1;
        mb->GTPv = numdinmb_meb(card);
        if(mb->GTPv < -90 || mb->GTPv > 90) {
            printf("Error! Global_Transpose data %d\n",mb->GTPv);
        }
        beaucsym_meb(); /* line shift */ 
        return(0);
    }
    if(!strcmp(work,"global_key_transposition") ||
       !strcmp(work,"global_transposition")) {
        mb->GKTPf = 1;
        mb->GKTPv = numdinmb_meb(card);
        if(mb->GKTPv < -90 || mb->GKTPv > 90) {
            printf("Error! Global_Transposition data %d\n",mb->GKTPv);
        }
        beaucsym_meb(); /* line shift */ 
        return(0);
    }
    if(!strcmp(work,"seuence_transpose")) {
        mb->STPv = numdinmb_meb(card);
        if(mb->STPv < -90 || mb->STPv > 90) {
            printf("Error! Sequence_Transpose data %d\n",mb->STPv);
        }   
        beaucsym_meb(); /* line shift */
        return(0);
    }          
    if(!strcmp(work,"sequence_key_transposition")  ||
       !strcmp(work,"sequence_transposition")) { 
        mb->SKTPv = numdinmb_meb(card);
        if(mb->SKTPv < -90 || mb->SKTPv > 90) {
            printf("Error! Sequence_Transposition data %d\n",mb->SKTPv);
        }
        beaucsym_meb(); /* line shift */
        return(0);
    }   
    if(!strcmp(work,"#define") || !strcmp(work,"#def")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(1);
        }
        i = 0;
        while(card[j] != ' ') {
            work[i++] = tolower(card[j]);
            /* work[i++] = card[j]; */
            j++;
            if(j > n) return(1);
        }
        work[i] = '\0';
        rtn = define_meb(card,work);
        return(rtn);
    }
    if(!strcmp(work,"#assign") || !strcmp(work,"#asn")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(1);
        }
        i = 0;
        while(card[j] != ' ') {
            work[i++] = tolower(card[j]);
            /* work[i++] = card[j]; */
            j++;
            if(j > n) return(1);
        }
        work[i] = '\0';
        rtn = assign_meb(card,work);
        return(rtn);
    }
    if(!strcmp(work,"#set")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(1);
        }
        i = 0;
        while(card[j] != ' ') {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) return(1);
        }
        work[i] = '\0';
        rtn = set_meb(card,work);
        if(!strcmp(work,"renumber_start_value")) {
            //SetRenumStartVal_meb();
            mb->xbnum = numdinmb_meb(card);
            //if(mv->mfflg && mb->sequ == 0) mb->ybnum = mb->xbnum; // 2024.9.4
            if(mv->mfflg) mb->ybnum = mb->xbnum; // 2024.9.4
            mb->xbnum--;
            if(mv->debug) {
                printf("renumber_start_value xbnum=%d ybnum=%d\n",
                mb->xbnum,mb->ybnum);
            }
            SetRenumStartVal_meb();  // 2024.9.4
            return(0);
        }
        if(!strcmp(work,"smpte_add_value")) {
            if(!smptaget_meb()) {
                mb->setsmpf = 1;
                beaucsym_meb();
            }
            else mb->setsmpf = 0;
            return(0);
        }
        if(!strcmp(work,"smpte_sub_value")) {
            if(!smptaget_meb()) {
                mb->setsmpf = 1;
                beaucsym_meb();
                mb->smpstf = -mb->smpstf;
                mb->smpsts = -mb->smpsts;
                mb->smpstm = -mb->smpstm;
                mb->smpsth = -mb->smpsth;
            }
            else mb->setsmpf = 0;
            return(0);
        }
        return(rtn);
    }
    if(!strcmp(work,"#unset")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(1);
        }
        i = 0;
        while(card[j] != ' ') {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) return(1);
        }
        work[i] = '\0';
	unset_meb(card,work);
    }
    if(!strcmp(work,"#modify_start")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(1);
        }
        i = 0;
        while(card[j] != ' ') {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) return(1);
        }
        work[i] = '\0';
        rtn = modify_start_meb(card,work);
        return(rtn);
    }
    if(!strcmp(work,"#modify_end")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(9);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        rtn = modify_end_meb(card,work);
        return(rtn);
    }
    if(!strcmp(work,"#include")) {
        mb->inclf = 1;
        i = 5;
        while(card[i] != ' ') i++;
        while(card[i] == ' ') i++;
        for(j = 0; j < 70 ; j++,i++) {
            mv->incf[j] = card[i];
            if(card[i] == ' ') break;
            if(card[i] == '\0') break;
            if(card[i] == 0x0d) break;
            if(card[i] == 0x0a) break;
        }
        mv->incf[j] = '\0';
        if(mv->incf[0] == 'T' || mv->incf[0] == 't') {
            if(mv->incf[4] =='/') {
                strcpy(scrf,mv->incf);
                j = strlen(scrf);
                i = 5;
                k = 0;
                while(1) {
                    mv->incf[k] = scrf[i];
                    k++;
                    i++;
                    if(i > j) break;
                }
                mv->incf[k] = '\0';
                sprintf(scrf,"%s",TACTDIR);
                //sprintf(scrf,"%s\0",TACTDIR);
                strcat(scrf,mv->incf);
                strcpy(mv->incf,scrf);
            }
        }
        if(mv->debug) printf("mv->incf = %s\n",mv->incf);
        beaucsym_meb();
        return(0);
    }
    if(!strcmp(work,"#delete")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(10);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        if(!strcmp(work,"bar")) {
            mb->delbarv = numdinmb_meb(card);
            if(mv->step != 1) beaucsym_meb();
            return(0);
        }
        if(!strcmp(work,"bars")) {
            mb->delbarv = numdinmb_meb(card);
            if(mv->step != 1) beaucsym_meb();
            return(0);
        }
        return(12);
    }
    if(!strcmp(work,"#add")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        rtn = add_sub_clear(card,work,2); /* add*/
        if(!strcmp(work,"bar")) {
            mb->addbarv = numdinmb_meb(card);
            if(mv->step != 1) beaucsym_meb();
            return(0);
        }
        if(!strcmp(work,"bars")) {
            mb->addbarv = numdinmb_meb(card);
            if(mv->step != 1) beaucsym_meb();
            return(0);
        }
        return(rtn);
    }
    if(!strcmp(work,"#sub")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        rtn = add_sub_clear(card,work,3); /* sub */
        return(rtn);
    }
    if(!strcmp(work,"#clear")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        rtn = add_sub_clear(card,work,4); /* clear */
        return(rtn);
    }
    if(!strcmp(work,"#reset")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        rtn = add_sub_clear(card,work,5); /* reset */
        return(rtn);
    }
    if(!strcmp(work,"#stop")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }   
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }   
        work[i] = '\0';
        if(!strcmp(work,"chord_progression")) {
            cp->chgf = 0;
            /* off next chord */
            for(k = 0; k < 10; k++) cp->npcd[k][0] = '\0';
            return(0);
        }
        return(15);
    } 
    if(!strcmp(work,"#pose")) { 
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        if(!strcmp(work,"chord_progression")) {
            cp->chgf = 0;
            return(0);
        }
        return(15);
    }
    if(!strcmp(work,"#start")) {
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        if(!strcmp(work,"chord_progression")) {
            cp->chgf = 1;
            /* off next chord */
            for(k = 0; k < 10; k++) cp->npcd[k][0] = '\0';
            return(0);
        }   
        return(15);
    }   
    if(!strcmp(work,"#resume")) { 
        while(card[j] == ' ') {
            j++;
            if(j > n) return(11);
        }
        i = 0;
        while(card[j] != ' ' || isalpha(card[j])) {
            work[i++] = tolower(card[j]);
            j++;
            if(j > n) break;
        }
        work[i] = '\0';
        if(!strcmp(work,"chord_progression")) {
            cp->chgf = 1;
            return(0);
        }
        return(15);
    }
    return(12);
}

int smpflame_meb()
{
    int  whour;
    int  wmin;
    int  wsec;
    int  i,j;
    char wk[9];

    i = 0;
    while(mv->card[i] != 0x00) {
        i++;
        if(i >= 80) return(-1);
    }
    for(j = 0; j < 9; j++) wk[j] = ' ';
    j = 8;
    i++;
    while(mv->card[i] != ' ') {
        wk[j] = mv->card[i];
        j--;
        if(j < 0) break;
        i--;
    }
    sscanf(wk,"%f",&mb->smplagf);
    /* printf("wk = %s mb->smplagf = %f\n",wk,mb->smplagf); */
    wsec = mb->smplagf / mb->sflame;
    mb->smplagf = mb->smplagf - (wsec * mb->sflame);
    wmin = wsec / 60;
    mb->smplags = wsec - (wmin * 60);      /* SMPTE seconds */
    whour = wmin / 60;
    mb->smplagm = wmin - (whour * 60);     /* SMPTE minites */
    mb->smplagh = whour % 24;              /* SMPTE hours */
    return(0);
}

int beaumset_meb()
{
    int  i;
    char wk1[8];
    char wk2[8];

    i = 0;
    for(i = 0; i < 80 ; i++) {
        if(mv->card[i] <= 0x1f) mv->card[i] = ' ';
    }
    if(mv->card[0] == '%') {
        /* pcasesh_meb(); */
        if(mv->card[4] == '[') noteshft_meb();
        beaudpset_meb();
        beaudymc_meb();
        beautrns_meb();
        if(mb->bsyntyp == 'v') beausynt_meb();
        if(mb->bartyp == 'r') beaurtmv_meb();
        return(0);
    }
    if(mv->card[0] == 's' || mv->card[0] == 'c') {
        if(mb->bexchf || mv->card[2] == 'x' || 
            mv->card[2] == 'X' || mv->card[1] == ' ') {
	    /*
            sprintf(wk1,"%02d",mb->bexp);
            mv->card[1] = wk1[0];
            mv->card[2] = wk1[1];
	    */
	    beaumod_meb();
        }
    }
    if((mv->card[0] != 's') || (mv->card[0] != '|')) {
        if(mb->smpmid == 1) {
            if(mb->bexchf || mv->card[40] == 'x' || mb->transvf == 1 ||
                mv->card[40] == 'X' || mv->card[40] == ' ') {
                sprintf(wk1,"%02d",mb->tpv);
                mv->card[39] = wk1[0];
                mv->card[40] = wk1[1];
            }
            if(mb->bexchf || mv->card[46] == 'x' || 
                mv->card[46] == 'X' || mv->card[46] == ' ') {
                sprintf(wk1,"%5.1f",mv->velo);
                mv->card[42] = wk1[0];
                mv->card[43] = wk1[1];
                mv->card[44] = wk1[2];
                mv->card[45] = wk1[3];
                mv->card[46] = wk1[4];
            }
        }
    }
    mv->mtemp = mv->tempo;
    mv->mtemp = mv->mtemp/10.0;
    if(mb->rstmpf || mv->card[54] == 'x' || mv->card[54] == 'X') {
        sprintf(wk1,"%6.2f",mv->mtemp);
        mv->card[49] = wk1[0];
        mv->card[50] = wk1[1];
        mv->card[51] = wk1[2];
        mv->card[52] = wk1[3];
        mv->card[53] = wk1[4];
        mv->card[54] = wk1[5];
    }
    if(mb->rsdynaf || mv->card[61] == 'x' || mv->card[61] == 'X') {
        sprintf(wk2,"%5.1f",mv->mdymc);
        mv->card[57] = wk2[0];
        mv->card[58] = wk2[1];
        mv->card[59] = wk2[2];
        mv->card[60] = wk2[3];
        mv->card[61] = wk2[4];
    }
    mv->card[79] = '\0';
    return(0);
}

float fnumdin_meb(char *card)
//char *card;
{
    int  i,k;
    float f;
    char *pc;
    char cnum[10];

    k = 0;
    pc = card;
    while(*pc) pc++;
    pc--;
    while(*pc == ' ') pc--;
    for(i = 0; i < 10; i++) cnum[i] = ' ';
    cnum[9] = '\0';
    for(k = 8; k >= 0; k--) {
        if(*pc == ' ') break;
        cnum[k] = *pc;
        /* if(!isdigit(*pc)) return(-2);   -xxx vau */
        pc--;
    }
    sscanf(cnum,"%f",&f);
    return(f);
}

/* set mb->bsyntyp
   ex.
   -s0100001-   mb->bsyntyp = 'v';
*/
int bsyntyp_meb()
{
    if( mv->card[1] == 'd' ||
        mv->card[1] == 't' ||
        mv->card[1] == 's' ||
        mv->card[1] == 'g' ||
        mv->card[1] == 'a' ||
        mv->card[1] == 'u' ||
        mv->card[1] == 'b') mb->bsyntyp = 'v';
    else mb->bsyntyp = mv->card[1];
    return(0);
}
