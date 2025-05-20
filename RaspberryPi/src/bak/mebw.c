
// ----------------------
//  AMuPLC (mebw.c)
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

/* mebw.c */
#include "meb.h"
extern MIVARG *mv;
extern MEBVAR *mb;
extern MEBCPS *cp;
extern MEBCSL *cs;
extern MEBANS *ma;
extern char jrule[MAXRL][82];  /* Major Rule */
extern char irule[MAXRL][82];  /* minor Rule */
char cdName[24];
char cdNval[MAXLVM];

int selcnv_meb()
{
    int  i,j,k;
    int  len;
    int  pf;

    if(mv->debug == 15) {
        printf("selcnv_meb() mv->step=%d cp->scnvf=%d mb->bartyp = %c\n",
            mv->step,cp->scnvf,mb->bartyp);
        printf("cdNval  = \"%s\"\n",cdNval);
        /* printf("%s\n",mv->card); */
    }
    k = 5;  /* mb->bartyp == 'm' */
    j = 17;
    if(mb->bartyp == 'l') k = 0;
    if(mb->bartyp == 'h') k = 16;
    if(mb->bartyp == 't') {
        k = 27;
        j = 31;
    }
    if(mb->bartyp == 'g') {
        k = 20;
        j = 31;
    }
    if(mb->bartyp == 's') {
        k = 13;
        j = 31;
    }
    if(mb->bartyp == 'a') {
        k = 6;
        j = 31;
    }
    if(mb->bartyp == 'b') {
        k = -1;
        j = 31;
    }
    if(mb->bartyp == 'u') {
        k =  -8;
        j = 31;
    }
    /* clear chord & avalable notes */
    for(i = j; i < 69; i++) {
        if(mv->card[i] == '.') mv->card[i] = ' ';
        if(mv->card[i] == '+') mv->card[i] = ' ';
        if(mv->card[i] == '-') mv->card[i] = ' ';
        if(mv->card[i] == 'x') mv->card[i] = ' ';
        if(mv->card[i] == 'o') mv->card[i] = ' ';
    }
    for(i = j; i < 67; i++,k++) {
        if(k > MAXLVM) break;
        if(k > 0) {
            if(cdNval[k] == '\0') break;
            if(mv->card[i] == ' ') mv->card[i] = cdNval[k];
        }
    }
    /* select chord_name_voicing_velocity set ma->nvc */
    if(ma->nvf) {
        len = strlen(ma->nvc);
        if(mv->debug == 15) printf("ma->nvc = %s len = %d\n",ma->nvc,len);
        k = 0;
        for(i = j; i < 69; i++) {
            pf = 0;
            if(mv->card[i] == '.') {
                mv->card[i] = ma->nvc[k];
                pf = 1;
            }
            if(mv->card[i] == '+') {
                if(ma->nvc[k] != '.') mv->card[i] = '>';
                pf = 1;
            }
            if(mv->card[i] == '-') {
                if(ma->nvc[k] != '.') mv->card[i] = '<';
                pf = 1;
            }
            if(pf) {
                k++;
                if(k > len) break;
            }
        }
    }
    if(mv->debug == 15) printf("%s\n",mv->card);
    return(0);
}

int gCname_meb()
{
    int  i,j;

    if(mv->debug == 15) printf("%s\n",mv->card);
    for(i = 2; i < 20; i++) {
        if(mv->card[i] == ' ') break;
        if(mv->card[i] == '}') break;
        if(mv->card[i] == '\0') break;
    }
    while(1) {
        if(mv->card[i] != ' ') break;
        i++;
        if(mv->card[i] == '}') break;
        if(mv->card[i] == '\0') break;
    }
    j = 0;
    while(1) {
        if(mv->card[i] == ' ') break;
        cdName[j] = mv->card[i];
        i++;
        j++;
        if(j > 22) break;
        if(mv->card[i] == '}') break;
        if(mv->card[i] == '\0') break;
    }
    cdName[j] = '\0';
    cdNval[0] = '\0';
    ma->scvf = 0;
    for(i = 0; i < ma->n; i++) {
        if(!strcmp(cdName,ma->l[i])) {
            strcpy(cdNval,ma->m[i]);
            ma->scvf = 1;
            break;
        }
    }
    if(mv->debug == 15) {
        printf("gCname_meb() cdName = %s\n",cdName);
        printf("cdNval  = \"%s\"\n",cdNval);
    }
    return(0);
}


/* Observe Chord_Name voicing select $N: */
int setlvN_cnv()
{
    int  i,k;
    int  af;
    int  sf;
    int  ef;
    int  ac;
    int  rtn;     
    char line[82];
    
    af = 0;
    ac = 0;
    k = 0;
    if(mv->debug == 15) {
        printf("Debug15! setlvN_cnv() ma->skey=%s ",ma->skey);
        if(cp->key == 1) printf("cp->jky=%d\n",cp->jky);
        if(cp->key == 2) printf("cp->iky=%d\n",cp->iky);
    }
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]);
        if(cp->key == 2) strcpy(line,irule[k]);
        k++;  
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        ef = 0;
        sf = 0;
        if(line[0] == '$' && line[1] == 'N' && line[2] == ':') sf = 1;  
        if(sf) {  
            /* printf("%s",line);  */
            i = 3;
            while(1) {
                if(line[i] == '=') {
                    ef = 1;
                    break;
                }
                if(line[i] == '\0') break;
                if(line[i] == '\r') break;
                if(line[i] == '\n') break;
                i++;
                if(i > 80) break;
            }
            if(!ef) {
                af = linechk_cps(line,ma->skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) ma->n = 0;
                    setvalN_cnv(line);   /* set chord_name valuse */
                    ac++;
                }
            }
        }
    }
    return(0);
}

/* set chord_name valuse */
int setvalN_cnv(char *line)
//char *line;
{
    int  i,j,k;
    int  af;
    char sl[MAXLVL];
    char sm[MAXLVR];
    char sr[MAXLVR];
    char so[MAXLVR];
         
    /* printf("setvalN_() line = %s",line); */
    i = 3;
    j = 0;
    if(line[0] == '$' && line[2] != ':') i = 1;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        sl[j] = line[i];
        i++;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    sl[j] = '\0';
    i = 3;
    af = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '=') {
            af = 1;
            break;
        }
        i++;
        if(i > 80) break;
    }
    if(!af) return(1);
    i++;
    while(line[i] == ' ') {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        i++;
        if(i > 80) break;
    }
    j = 0;
    af = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '\"' || line[i] == '\'')  af++;
        if(af != 1) if(line[i] == ' ') break;
        if(line[i] != '\'' && line[i] != '\"') sm[j++] = line[i];
        i++;
        if(i > 80) break;
        if(j >= MAXLVR) break;
    }
    sm[j] = '\0';
    k = strlen(line);
    if(k > i) {
        i++;
        while(1) {
            if(line[i] != ' ') break;
            i++;
            if(i > 79) break;
            if(i > k) break;
        }
        j = 0;
        while(1) {
            if(line[i] == ' ') break;
            if(line[i] == '\0') break;
            if(line[i] == '\r') break;
            if(line[i] == '\n') break;
            sr[j] = line[i];
            i++;
            if(i > 80) break;
            if(i > k) break;
            j++;
            if(j >= MAXLVR) break;
        }
        sr[j] = '\0';
        if(line[i] == ' ') i++;
        while(1) {
            if(line[i] != ' ') break;
            i++;
            if(i > 79) break;
            if(i > k) break;
        }
        j = 0;
        while(1) {
            if(line[i] == ' ') break;
            if(line[i] == '\0') break;
            if(line[i] == '\r') break;
            if(line[i] == '\n') break;
            so[j] = line[i];
            i++;
            if(i > 80) break;
            if(i > k) break;
            j++;
            if(j >= MAXLVR) break;
        }
        so[j] = '\0';
    }
    /* printf("sl = %s sm = %s sr = %s so = %s\n",sl,sm,sr,so); */
    strcpy(ma->l[ma->n],sl);
    strcpy(ma->m[ma->n],sm);
    strcpy(ma->r[ma->n],sr);
    strcpy(ma->o[ma->n],so);
    if(mv->debug == 15) {
        printf("ma->n = %d ma->l(Chord_Name) = %s\n",ma->n,ma->l[ma->n]);
        printf(" m0000006 _-1______0______1______2______*______4");
        printf("______5______6______7 \n");
        printf("ma->m   =  \"%s\"\n",ma->m[ma->n]);
    }  
    ma->n++;
    if(ma->n > MAXLVA) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Error! rule Level N argument over than %d\n",MAXLVA);
        }
        ma->n--;
    }
    return(0);
}

int setinst_cnv()
{
    int  i,j;

    i = 3;
    j = 0;
    ma->cvpf = 1;
    while(1) {
        if(mv->card[i] == ' ') break;
        if(mv->card[i] == '\0') break;
        ma->cdinst[j] = mv->card[i];
        i++;
        j++;
        if(j > MAXLVM) break;
    }
    ma->cdinst[j] = '\0';
    if(mv->debug == 15) printf("setinst_cnv() ma->cdinst = %s\n",ma->cdinst);
    return(0);
}

int getnkey_cnv()
{
    int  i,j;
    int  len;
    char inst[MAXLVM];

    /* printf("getnkey_cnv() ma->nkey = %s\n",ma->nkey); */
    if(mv->card[3] == ':') return(0);
    ma->nkey[0] = '\0';
    len = strlen(mv->card);
    for(i = len; i > 1; i--) {
        if(mv->card[i] == ' ') break;
    }
    i++;
    for(j = 0; i < len ;i++,j++) {
        if(mv->card[i] == '}') break;
        ma->nkey[j] = tolower(mv->card[i]);
    }
    ma->nkey[j] = '\0';
    if(mv->debug == 15) {
        printf("getnkey_cnv() %s len = %d ma->nkey = %s\n",
        mv->card,len,ma->nkey);
    }
    return(0);
}

int setskey_cnv()
{
    int  i,j;
    int  len;
    char inst[MAXLVM];

    /* printf("setskey_cnv() ma->nkey = %s\n",ma->nkey); */
    ma->n = 0; /* add 2007.2.12 */
    len = strlen(ma->cdinst);
    for(i = 0; i < len; i++) inst[i] = tolower(ma->cdinst[i]);
    inst[i] = '\0';  
    /* if(ma->cvpf) { */
    if(inst[0] != '+') {
        strcpy(ma->skey,inst);
    }
    else {
        strcpy(ma->skey,ma->nkey);
        strcat(ma->skey,inst);
    }
    if(mv->debug == 15) {
        printf("inst = %s\n",inst);
        printf("setskey_cnv() %s len = %d ma->skey = %s\n",
        mv->card,len,ma->skey);
        printf("ma->cdinst = %s\n",ma->cdinst);
    }
    return(0);
}

