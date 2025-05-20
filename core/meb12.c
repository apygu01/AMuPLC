
// ----------------------
//  AMuPLC (meb12.c)
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
extern MEBCPS *cp;
extern MEBCSL *cs;
extern MEBANS *ma;
extern char mfile[STLN];
/*
  -m0000002 _-1______0______1______2______*______4______5______6______7
*/
char *bfc =
 "-m0000002 ABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABCDEFGABC.";

/*
  -s0000005 p.v. mod cn:cdt pcg tp1______2______*______4______5______6 velo
*/
char *bsc =
 "-s0000005 ......................CDEFGABCDEFGABCDEFGABCDEFGABCDEFGABC.";

char *qch = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz&$#@";
char jrule[MAXRL][82];  /* Major Rule */
char irule[MAXRL][82];  /* minor Rule */
struct modkey {
    char key[18];
    char cn[7][3];      
};
struct modkey maj[12] = {
    "$ C_Major",         "C","D","E","F","G","A","B",
    "$&&&&& D&_Major",   "D&","E&","F","G&","A&","B&","C",
    "$## D_Major",       "D","E","F#","G","A","B","C#",
    "$&&& E&_Major",     "E&","F","G","A&","B&","C","D",
    "$#### E_Major",     "E","F#","G#","A","B","C#","D#",
    "$& F_Major",        "F","G","A","B&","C","D","E",
    "$&&&&&& G&_Major",  "G&","A&","B&","C&","D&","E&","F",
    "$# G_Major",        "G","A","B","C","D","E","F#",
    "$&&&& A&_Major",    "A&","B&","C","D&","E&","F","G",
    "$### A_Major",      "A","B","C#","D","E","F#","G#",
    "$&& B&_Major",      "B&","C","D","E&","F","G","A",
    "$##### B_Major",    "B","C#","D#","E","F#","G#","A#"
};
struct modkey min[12] = {
    "$ A_minor",         "A","B","C","D","E","F","G",
    "$&&&&& B&_minor",   "B&","C","D&","E&","F","G&","A&",
    "$## B_minor",       "B","C#","D","E","F#","G","A",
    "$&&& C_minor",      "C","D","E&","F","G","A&","B&",
    "$#### C#_minor",    "C#","D#","E","F#","G#","A","B",
    "$& D_minor",        "D","E","F","G","A","B&","C",
    "$&&&&&& E&_minor",  "E&","F","G&","A&","B&","C&","D&",
    "$# E_minor",        "E","F#","G","A","B","C","D",
    "$&&&& F_minor",     "F","G","A&","B&","C","D&","E&",
    "$### F#_minor",     "F#","G#","A","B","C#","D","E",
    "$&& G_minor",       "G","A","B&","C","D","E&","F",
    "$##### G#_minor",   "G#","A#","B","C#","D#","E","F#"
};
char Scale[MAXLVR];
char Scalex[MAXLVR];  /* _... -> +abc */
int  Scaf;  /* Scale Active flag */
char Ocps[80];  /* store cp->ocp */
char Orfn[80];   /* Open rule file name */

/* ! line */
int chord_cps()
{
    int  i,j,k;
    int  brf; /* Bracket ? */
    int  qf;
    int  len;
    int  len2;
    char skey[MAXLVL];
    char temp[82];
    int  adf;
    char adst[240];
    int  hlp;
    int  cldf;
    int  pf;
    char clds[12];

    if(mv->debug == 6) printf("chord_() cp->achgf = %d cp->chgf = %d\n",
        cp->achgf,cp->chgf);
    if(mv->debug == 6) printf("%s\n",mv->card);
    /* printf("mb->bartyp=%c\n",mb->bartyp); */
    if(mb->bartyp == 'r' || mb->bartyp == 'd') return(0);
    cp->mf = 0;  /* Multi Chord flag off */
    cp->bf = 0;
    cp->pol = 0;
    Scaf = 0;
    Scale[0] = '\0';
    adf = 0;
    if(mv->card[0] == '{') {
        i = 1;
        while(1) {
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == '}') break;
            i++;
            if(i > 78) break;
        }
        i++;
        j = 0;
        adst[0] = '\0';
        while(1) {
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == 'o' && mv->card[i+1] == 'n') adf = 1;
            if(mv->card[i] == ' ' && mv->card[i+1] == '~') adf = 2; /* 2005.2 */
            adst[j] = mv->card[i];
            i++;
            if(i > 79) break;
            j++;
        }
        adst[j] = '\0';
        if(mv->debug == 6) printf("{..} on xxx adf = %d adst = %s\n",
            adf,adst);
    }
    brf = 0; /* Remove { } ( ) */
    if(mv->card[0] == '{' || mv->card[0] == '(') {
        if(cp->chgf) chprgres_meb();
        if(mv->card[2] == '}' || mv->card[2] == ')') return(0);
        strcpy(temp,mv->card);
        i = 0;
        if(mv->card[0] == '{') brf = 1;
        if(mv->card[0] == '(') brf = 2;
        while(1) {
            if(temp[i+1] == '}') break;
            if(brf == 2) if(temp[i+1] == ')') break;
            /* if(temp[i+1] == ')') break; */
            if(temp[i+1] == '\0') break;
            if(temp[i+1] == '\r') break;
            if(temp[i+1] == '\n') break;
            mv->card[i] = temp[i+1];
            i++;
            if(i > 78) break;
        }
        mv->card[i] = '\0';
        /* printf("%s\n",mv->card); */
    }
    qf = 0;
    len = strlen(qch);
    i = 0;
    while(1) { /* question char ? */
        if(qch[i] == '\0') break;
        if(mv->card[1] == qch[i]) qf = 1;
        i++;
        if(i > len) break;
    }
    /* printf("len = %d qf = %d\n",len,qf); */
    if(!qf) return(1);
    i = 1;
    j = 0;
    k = 0;
    cldf = 0;
    pf = 0;
    while(mv->card[i] != ' ') {
        /* Add 2007.2.3 */
        if(mv->card[i] == '~') {
            cldf = 1;
            k = i;
            if(mv->mfflg == 0) break;
        }
        if(mv->card[i] == '\0') break;
        if(mv->card[i] == '\r') break;
        if(mv->card[i] == '\n') break;
        if(mv->card[i] == '*') {
            cp->mf = 1;  /* Multi Chord */
            break;
        }
        skey[j] = mv->card[i];
        i++;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    skey[j] = '\0';
    if(cldf) {
        j = 0;
        while(1) {
            if(mv->card[k] == ' ') break;
            if(mv->card[k] == '}') break;
            if(mv->card[k] == '\0') break;
            if(mv->card[k] == '\r') break;
            if(mv->card[k] == '\n') break;
            clds[j] = mv->card[k];
            k++;
            j++;
            if(j > 12) break;
        }
        clds[j] = '\0';
        /*
        printf("%s\n",mv->card);
        printf("clds = %s\n",clds);
        */
    }
    if(cp->mf) {
        j = 0;
        while(1) {
            if(mv->card[i] == ' ') break;
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == '\r') break;
            if(mv->card[i] == '\n') break;
            cp->star[j] = mv->card[i];
            i++;
            if(i > 80) break;
            j++;    
            if(j > 8) break;
        }
        cp->star[j] = '\0';
    }
    mv->card[i] = '\0';
    if(mv->debug == 6) printf("cp->mf = %d skey = %s cp->star = %s\n",
        cp->mf,skey,cp->star);
    hlp = hlponly_cps(skey);
    if(!hlp) {
        if(cp->chgf) chgserch_cps(skey);
        else cpserch_cps(skey);
        if(ma->nsf) setavns_meb(skey); /* set available note scale */
        /* printf("%s\n",mv->card); */
        if(mv->debug == 6) printf("Scaf = %d Scale = %s\n",Scaf,Scale);
        if(Scaf == 0 && Scale[0] != '\0') {
            strcat(mv->card," ,");
            strcat(mv->card,Scale);
            /* printf("%s Add Scale\n",mv->card); */
        }
    }
    if(brf) { /* Restore { } ( ) */
        strcpy(temp,mv->card);
        if(brf == 1) mv->card[0] = '{';
        if(brf == 2) mv->card[0] = '(';
        i = 1;
        while(1) {
            mv->card[i] = temp[i-1];
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == '\r') break;
            if(mv->card[i] == '\n') break;
            i++;
            /* if(i > 77) break; */
            if(i > 66) break;
        }
        if(brf == 1) mv->card[i] = '}';
        if(brf == 2) mv->card[i] = ')';
        mv->card[++i] = '\0';
        /* printf("%s\n",mv->card); */
    }
    if(adf) {
        len = strlen(mv->card);
        len2 = strlen(adst);
        if(len + len2 > 79) {
            j = 78 - len2;
            if(brf == 1) mv->card[j] = '}';
            if(brf == 2) mv->card[j] = ')';
            mv->card[++j] = '\0';
        }
        strcat(mv->card,adst);
    }
    /* printf("mb->chdobf = %d\n",mb->chdobf); */
    if(mb->chdobf) chdobsev_meb();
    /* printf("skey = %s\n",skey); */
    pf = 0;
    if(mv->mfflg == 0) {
        for(i = 0; i < 82; i++) {
            if(mv->card[i] == ' ') pf++;
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == '\r') break;
            if(mv->card[i] == '\n') break;
        }
    }
    if(cldf && pf > 1) {
        i = 0;
        j = 0;
        while(1) {
            temp[j] = mv->card[i];
            if(mv->card[i] == ' ') break;
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == '\r') break;
            if(mv->card[i] == '\n') break;
            i++;
            j++;
            if(i > 82) break;
        }
        temp[j] = '\0';
        strcpy(adst,temp);
        strcat(adst,clds);
        /*
        printf("temp = %s\n",temp);
        printf("adst = %s\n",adst);
        */
        j = 0;
        for(; i < 82; i++) {
            if(mv->card[i] != ' ') break;
        }
        while(1) {
            temp[j] = mv->card[i];
            if(mv->card[i] == ' ') break;
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == '\r') break;
            if(mv->card[i] == '\n') break;
            i++;
            j++;
            if(i > 82) break;
        }
        temp[j] = '\0';
        strcat(adst," ");
        strcat(adst,temp);
        strcat(adst,clds);
        /*
        printf("temp = %s\n",temp);
        printf("adst = %s\n",adst);
        */
        for(; i < 82; i++) {
            if(mv->card[i] != ' ') break;
        }
        j = 0;
        while(1) {
            temp[j] = mv->card[i];
            /* ignore -> next_chord */
            if(mv->card[i] == '>' && mv->card[i-1] == '-') {
                temp[j-1] = '}';
                break;
            }
            if(mv->card[i] == '\0') break;
            if(mv->card[i] == '\r') break;
            if(mv->card[i] == '\n') break;
            i++;
            j++;
            if(i > 82) break;
        }
        temp[j] = '\0';
        strcat(adst," ");
        strcat(adst,temp);
        /*
        printf("temp = %s\n",temp);
        printf("adst = %s\n",adst);
        */
        strcpy(mv->card,adst);
    }
    return(0);
}

/* Set Chord observe note line */
int chdobsev_meb()
{
    int  i;
    int  cf;
    int  pf;
    int  rt;
    int  rtn;
    char tk[MAXLVR];
    char sk[MAXLVR];
    char Cn[8];  /* Chord name */

    /* printf("%s\n",mv->card); */
    if(cp->pol) {
        rtn = Polyobs_meb();
        return(rtn);
    }
    cp->caf = 0;
    cf = getchord_meb(1,Cn); /* cf : C D E F G A B */
    if(mv->debug == 6) printf("chdobsev_meb() Cn = %s cf = %d\n",Cn,cf);
    if(cf == -2) return(1); /* error chord */
    if(cp->bf) {
        i = 0;
        while(1) {
            if(Cn[i] == '\0') break;
            if(Cn[i] == '/') {
                Cn[i] = '\0';
                break;
            }
            i++;
            if(i > 8) break;
        }
    }
    strcpy(tk,"$5:");
    strcat(tk,Cn);
    pf = cpget_cps(tk,sk);
    if(mv->debug == 6) printf("pf = %d tk = %s sk = %s\n",pf,tk,sk);
    if(pf) return(2); /* no chord */
    rtn = getRoot_meb(1,&rt); /* rt : Root Column position */
    if(rtn == 3 || rtn == 4 || rtn == 7) modRoot_meb(&rt);
    if(mv->debug == 6) printf("rtn = %d rt = %d\n",rtn,rt);
    chkival_meb(rtn,rt,sk);
    cp->caf = 1;
    return(0);
}

/* get chord name */
int getchord_meb(int acf,char Cn[8])
//int  acf; /* 1:chord 2:Available flag */
//char Cn[8];
{
    int  i,j;
    int  cf; /* Chord flag */
    char c;
    char wk[82];

    strcpy(wk,mv->card);
    beauspace_meb(wk);
    if(mv->debug == 6) printf("getchord() wk = %s acf = %d\n",wk,acf); 
    Cn[0] = '\0';
    if(wk[0] == '!')  i = 1;
    if(wk[1] == '!')  i = 2;
    c = wk[i];
    if(c == 'I' || c == 'V' || c == '&' || c == '#') {
        i++;
        while(wk[i] != ' ') {
            if(wk[i] == '\0') break;
            i++;
            if(i > 20) break;
        }
        i++;
        /* printf("wk[i] = %c i = %d\n",wk[i],i); */
        cf = -2;
        if(wk[i] == 'C') cf = 0;
        if(wk[i] == 'D') cf = 2;
        if(wk[i] == 'E') cf = 4;
        if(wk[i] == 'F') cf = 5;
        if(wk[i] == 'G') cf = 7;
        if(wk[i] == 'A') cf = 9;
        if(wk[i] == 'B') cf = 11;
        if(cf > -2) {
            if(wk[i + 1] == '#') {
                cf = cf + 1;
                i++;
            }
            if(wk[i + 1] == '&') {
                cf = cf - 1;
                i++;
            }
            if(wk[i + 1] == '*') {
                cf = cf + 2;
                i++;
            }
            if(wk[i + 1] == '@') {
                cf = cf - 2;
                i++;
            }
        }
    }
    else {
        /* printf("wk[i] = %c i = %d\n",wk[i],i); */
        cf = -2;
        if(wk[i] == 'C') cf = 0;
        if(wk[i] == 'D') cf = 2;
        if(wk[i] == 'E') cf = 4;
        if(wk[i] == 'F') cf = 5; 
        if(wk[i] == 'G') cf = 7;
        if(wk[i] == 'A') cf = 9;
        if(wk[i] == 'B') cf = 11; 
        if(cf > -2) {
            if(wk[i + 1] == '#') {
                cf = cf + 1;
                i++;
            }
            if(wk[i + 1] == '&') {
                cf = cf - 1;
                i++;
            }
            if(wk[i + 1] == '*') {
                cf = cf + 2;
                i++;
            }
            if(wk[i + 1] == '@') {
                cf = cf - 2;
                i++;
            }
        }
    }
    if(cf > -2) {
        if(acf == 1) cp->crp = cf + ((cp->cbp + 2) * 12); /* chord */
        /* if(acf == 2) ma->arp = cf + ((ma->abp + 2) * 12);  Avail */
        if(acf == 2) ma->arp = cf + ((ma->abp + 2) * 12);  /* Avail */
        i++;
        j = 0;
        while(wk[i] != ' ') {
            if(wk[i] == '/') break;
            if(wk[i] == '\0') break;
            if(wk[i] == '}') break;
            Cn[j] = wk[i];
            i++;
            if(i > 79) break;
            j++;
            if(j > 7) break;
        }
        if(j == 0) {
            Cn[0] = 'M'; /* Major */
            j++;
        }
        Cn[j] = '\0';
        if(mv->debug == 6) {
            if(acf == 1) printf("Cn = %s cp->crp = %d cp->cbp = %d\n",
                Cn,cp->crp,cp->cbp);
            if(acf == 2) printf("Cn = %s ma->arp = %d ma->abp = %d\n",
                Cn,ma->arp,ma->abp);
        }
    }
    return(cf);
}

/* check interval from Root */
int chkival_meb(int sf,int rt,char Note[MAXLVR])
//int  sf; /* # & flag */
//int  rt; /* Root position */
//char Note[MAXLVR];
{
    int  i;
    int  len;
    int  iv;
    char rch;

    len = strlen(Note);
    iv = 0; 
    for(i = 3; i < 69; i++) cp->cnl[i] = ' ';
    cp->cnl[0] = '!';
    cp->cnl[1] = 'x';
    cp->cnl[2] = 'x';
    cp->cnl[69] = '\0';
    strcat(cp->cnl,"xx.x ._...");
    if(mv->debug == 6) printf("chkival() sf = %d rt = %d Note = %s len = %d\n",
        sf,rt,Note,len);
    rch = Note[0];
    for( i = 0; i < len; i++) {
        if(Note[i] != ' ') {
            iv = getival_meb(i,Note);
            setival2_meb(sf,rt,iv,Note[i],rch);
            setival3_meb(rt,iv,Note[i]);
        }
    }
    /* printf("%s\n",cp->cnl); */
    return(0);
}

/* Note set by interval */
int setival2_meb(int sf,int rt,int iv,char Nc,char rch)
//int  sf; /* 1:& 2:# 3:&No 4:#No flag 5:&Root 6:#Root */
//int  rt; /* Root column */
//int  iv; /* interval from Root */
//char Nc; /* Note char */
//char rch; /* Root char */
{
    int  i,j;
    int  k1,k2;
    int  dv;
    int  p1,p2,pd;
    /* int  r1,r2,rd; */
    int  tf;
    int  pf;
    int  ft;
    char wk[82];

    if(mv->debug == 6) {
        printf("cp->cvc = %s\n",cp->cvc);
        printf("Nc = %c sf = %d rt = %d iv = %d\n",Nc,sf,rt,iv);
    }
    if(rt < 11 || rt > 68) return(1); /* rt error */
    if(cp->bf) { /* on base */
        pf = 0;
        ft = 32;
        if(mb->bartyp == 'l') ft = 10;
        if(mb->bartyp == 'm') ft = 10;
        if(mb->bartyp == 'h') ft = 10;
        if(mb->bartyp == 't') ft = 32;
        if(mb->bartyp == 's') ft = 32;
        if(mb->bartyp == 'g') ft = 32;
        if(mb->bartyp == 'a') ft = 32;
        if(mb->bartyp == 'b') ft = 32;
        if(ft == 10) strcpy(wk,bfc);
        if(ft == 32) strcpy(wk,bsc);
        /* printf("%s <--wk\n",wk); */
        for(i = rt - 1; i > 11; i--) {
            if(cp->bc[0] == wk[i]) {
                pf = 1;
                break;
            }
        }
        p1 = getkeyp_meb(i,1);
        p2 = getkeyp_meb(i,3);
        pd = p2 - p1;
        /* check key  pd  1:#  -1:&  */
        if(pf) {
            cp->cnl[i] = '.';
            if(cp->bc[1] == '#') cp->cnl[i] = '>';
            if(cp->bc[1] == '&') cp->cnl[i] = '<';
            if(pd == 1) { /* # key */
                if(cp->cnl[i] == '.') cp->cnl[i] = '<';
                if(cp->cnl[i] == '>') cp->cnl[i] = '.';
            }
            if(pd == -1) { /* & key */
                if(cp->cnl[i] == '.') cp->cnl[i] = '>';
                if(cp->cnl[i] == '<') cp->cnl[i] = '.';
            }
        }
        if(cp->cnl[i] == '.') cp->cnl[i] = cp->cvc[0];
        if(cp->cnl[i] == '>') if(cp->cvc[0] == '.') cp->cnl[i] = '+';
        if(cp->cnl[i] == '<') if(cp->cvc[0] == '.') cp->cnl[i] = '-';
        if(mv->debug == 6) printf("%s\n",cp->cnl);
    }
    k1 = getkeyp_meb(rt,3);
    p1 = getkeyp_meb(rt,1);
    p2 = getkeyp_meb(rt,3);
    pd = p2 - p1;
    /* Root key Position pd  1:#  -1:&  */
    if(pd == 0) {
        /* cp->cnl[rt] = '.'; */
        cp->cnl[rt] = rch; 
        if(sf == 1 || sf == 5) { /* Root& */
            cp->cnl[rt] = '<';
            k1 = k1 - 1;
        }
        if(sf == 2 || sf == 6) { /* Root# */
            cp->cnl[rt] = '>';
            k1 = k1 + 1;
        }
    }
    if(pd == 1) {  /* # key */
        cp->cnl[rt] = '<';
        k1 = k1 - 1;
        if(sf == 0 || sf == 1 || sf == 5) {
            cp->cnl[rt] = '.';
            k1 = getkeyp_meb(rt,3); /* Root key position */
        }
    }
    if(pd == -1) { /* & key */
        cp->cnl[rt] = '>';
        k1 = k1 + 1;
        if(sf == 0 || sf == 2 || sf == 6) {
            cp->cnl[rt] = '.';
            k1 = getkeyp_meb(rt,3); /* Root key position */
        }
    }
    if(mv->debug == 6 && iv == 0) {
        printf("Nc= %c Root pd= %d sf= %d cp->cnl[%d]= %c\n",
            Nc,pd,sf,rt,cp->cnl[rt]);
    }
    if(iv != 0) { /* Not Root */
        tf = 0;
        for(i = rt; i < 69; i++) {
            k2 = getkeyp_meb(i,3);
            dv = k2 - k1;
            /* printf("dv = %d k1 = %d k2 = %d\n",dv,k1,k2); */
            if(dv == iv) {
                if(tf == 0) {
                    tf = 1;
                    cp->cnl[i] = '.'; 
                    if(mv->debug == 6) {
                       printf("Nc= %c dv,iv= %d cp->cnl[%d]= %c\n",
                       Nc,dv,i,cp->cnl[i]);
                    }
                }
                /* printf("cp->cnl[%d] = %c\n",i,cp->cnl[i]); */
            }
            else {
                if(Nc == '#' || Nc == '.') {
                    if(tf == 0) {
                        if((dv - 1) == iv) { /* iv over */
                            tf = 1;
                            cp->cnl[i] = '<';  /* . */
                            if(mv->debug == 6) {
                             printf("Nc= %c dv-1,iv= %d cp->cnl[%d]= %c\n",
                             Nc,dv - 1,i,cp->cnl[i]);
                            }
                        }
                        if((dv + 1) == iv) { /* iv less */
                            tf = 1;
                            cp->cnl[i] = '>';
                            if(mv->debug == 6) {
                             printf("Nc= %c dv+1,iv= %d cp->cnl[%d]= %c\n",
                             Nc,dv + 1,i,cp->cnl[i]);
                            }
                        }
                    }
                }
                if(Nc == '&') {
                    if((dv + 1) == iv) { /* iv less */
                        if(tf == 0) {
                            tf = 1;
                            cp->cnl[i] = '>';
                            if(mv->debug == 6) {
                             printf("Nc= %c dv+1,iv= %d cp->cnl[%d]= %c\n",
                             Nc,dv + 1,i,cp->cnl[i]);
                            }
                        }
                    }
                    if((dv - 1) == iv) { /* iv over */       
                        if(tf == 0) {
                            tf = 1;
                            cp->cnl[i] = '<';
                            if(mv->debug == 6) {
                             printf("Nc= %c dv-1,iv= %d cp->cnl[%d]= %c\n",
                             Nc,dv - 1,i,cp->cnl[i]);
                            }
                        }
                    }
                }
            }
        }
    }
    if(mv->debug == 6) printf("%s\n",cp->cnl);
    return(0);
}

int setival3_meb(int rt,int iv,char Nc)
//int  rt; /* Root column */
//int  iv; /* interval from Root */
//char Nc; /* Note char */
{
    int  i,j;
    int  k1,k2;
    int  ft;
    int  dv;

    for(i = rt; i < 69; i++) {
        if(cp->cnl[i] == '<') {
            k1 = getkeyp_meb(i-1,3);
            k2 = getkeyp_meb(i,3);
            /* printf("k1 = %d k2 = %d\n",k1,k2); */
            dv = k2 - k1;
            if(dv == 1) {
                cp->cnl[i] = ' '; 
                cp->cnl[i - 1] = '.';
            }
        }
        if(cp->cnl[i] == '>') {
            k1 = getkeyp_meb(i,3);      
            k2 = getkeyp_meb(i+1,3);
            /* printf("k1 = %d k2 = %d\n",k1,k2); */
            dv = k2 - k1;
            if(dv == 1) {
                cp->cnl[i] = ' ';
                cp->cnl[i + 1] = '.';
            }
        }
    }
    ft = 32;
    if(mb->bartyp == 'l') ft = 10;
    if(mb->bartyp == 'm') ft = 10;
    if(mb->bartyp == 'h') ft = 10;
    if(mb->bartyp == 't') ft = 32;
    if(mb->bartyp == 's') ft = 32;
    if(mb->bartyp == 'g') ft = 32;
    if(mb->bartyp == 'a') ft = 32;
    if(mb->bartyp == 'b') ft = 32;
    j = 0;
    /* for(i = rt - 1; i < 69; i++) { */
    for(i = ft; i < 69; i++) {
        if(cp->cnl[i] == '.') cp->cnl[i] = cp->cvc[j];
        if(cp->cnl[i] == '>') if(cp->cvc[j] == '.') cp->cnl[i] = '+';
        if(cp->cnl[i] == '<') if(cp->cvc[j] == '.') cp->cnl[i] = '-';
        if(cp->cnl[i] != ' ') j++;
        if(j > 50) break;
    }
    /* printf("%s\n",cp->cnl); */
    return(0);
}

/* get Root column of Note line */
int getRoot_meb(int acf,int *rt)
//int  acf; /* 1:chord 2:Available flag */
//int  *rt;
{
    int  i,j,k;
    int  keyp;
    int  p1,p2,pd;
    int  rtn;

    /*
    printf("cp->crp = %d mb->bartyp = %c mb->ksin = %d\n",
        cp->crp,mb->bartyp,mb->ksin);
    */
    k = 5;  /* mb->bartyp == 'm' */
    j = 10;
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
        if(acf == 1) {
            if(keyp == cp->crp) {
                *rt = i;
                /* printf("keyp = %d i = %d\n",keyp,i); */
                if(mv->debug == 6)
                    printf("rtn= 0 Root ok keyp= %d rt= %d\n",keyp,*rt);
                return(0); /* return Root Column */
            }
            if(keyp > cp->crp) break;
        }
        if(acf == 2) {
            /* printf("keyp = %d ma->arp = %d\n",keyp,ma->arp); */
            if(keyp == ma->arp) {
                *rt = i;
                /* printf("keyp = %d i = %d\n",keyp,i); */
                if(mv->debug == 6)
                    printf("rtn= 0 Root ok keyp= %d rt= %d\n",keyp,*rt);
                return(0); /* return Root Column */
            }
            if(keyp > ma->arp) break;
        }
    }
    p1 = getkeyp_meb(i,1);
    p2 = getkeyp_meb(i,3);
    pd = p2 - p1;
    /* & # Root */
    /* #Idim Gdim check */
    if(mv->card[1] == '&' || mv->card[2] == '&') {
        j = 3;
        while(1) {
            if(mv->card[j] == '&') {
                *rt = i;
                if(mv->debug == 6)
                    printf("rtn= 1 Root& keyp= %d rt=  %d\n",keyp,*rt);
                return(1);
            }
            if(mv->card[j] == ',') break;
            if(mv->card[j] == '\0') break;
            if(mv->card[j] == '\r') break;
            if(mv->card[j] == '\n') break;
            j++;
            if(j > 20) break;
        }
        *rt = i;
        if(pd == -1) *rt = i - 1; 
        if(mv->debug == 6)
             printf("rtn= 3 &Root pd= %d keyp= %d rt=  %d\n",pd,keyp,*rt);
        return(3); /* & keyp No match */
    }
    if(mv->card[1] == '#' || mv->card[2] == '#') {
        j = 3;
        while(1) {
            if(mv->card[j] == '#') {
                *rt = i - 1;
                if(mv->debug == 6)
                    printf("rtn= 2 Root# keyp= %d rt= %d\n",keyp,*rt);
                return(2);
            }
            if(mv->card[j] == ',') break;
            if(mv->card[j] == '\0') break;
            if(mv->card[j] == '\r') break;
            if(mv->card[j] == '\n') break;
            j++;
            if(j > 20) break;
        }
        *rt = i;
        if(pd == -1) *rt = i - 1; 
        if(mv->debug == 6)
            printf("rtn= 4 #Root pd= %d keyp= %d rt= %d\n",pd,keyp,*rt);
        return(4); /* # keyp No match */
    }
    j = 3;
    while(1) {
        if(mv->card[j] == '&') {
            *rt = i;
            if(mv->debug == 6)
                printf("rtn= 5 Root& keyp= %d rt= %d\n",keyp,*rt);
            return(5);
        }
        if(mv->card[j] == '#') {
            *rt = i - 1;
            if(mv->debug == 6)
                printf("rtn= 6 Root# keyp= %d rt= %d\n",keyp,*rt);
            return(6);
        }
        if(mv->card[j] == ',') break;
        if(mv->card[j] == '\0') break;
        if(mv->card[j] == '\r') break;
        if(mv->card[j] == '\n') break;
        j++;
        if(j > 20) break;
    }
    *rt = i;
    if(pd == -1) *rt = i - 1; 
    if(mv->debug == 6)
        printf("rtn 7 no Root pd= %d keyp= %d rt= %d\n",pd,keyp,*rt);
    return(7); /* # & Root */
}

/* modify Root position */
int modRoot_meb(int *rt)
//int  *rt;
{
    int  i;
    int  p1,p2,pd1,pd2;

    i = *rt;
    p1 = getkeyp_meb(i-1,1);
    p2 = getkeyp_meb(i,1);
    pd1 = p2 - p1;
    i = *rt - 1;
    p1 = getkeyp_meb(i,1);
    p2 = getkeyp_meb(i,3);
    pd2 = p2 - p1;
    if(pd1 == 1) {
        if(pd2 == -1) *rt = *rt - 1;
    }
    return(0);
}

/* get keyp Note line */
int getkeyp_meb(int p,int nbc)
//int  p;
//int  nbc; /* 1:natural key , 2:base key , 3:current key */
{
    int  i,j,k;
    int  keyp;

    /*
    printf("cp->crp = %d mb->bartyp = %c mb->ksin = %d\n",
        cp->crp,mb->bartyp,mb->ksin);
    */
    k = 5;  /* mb->bartyp == 'm' */
    j = 10;
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
        if(nbc == 1) keyp = mv->nktb[k];
        if(nbc == 2) keyp = mv->bktb[k];
        if(nbc == 3) keyp = mv->ktb[k];
        if(i == p) {
            /* printf("keyp = %d i = %d\n",keyp,i); */
            return(keyp); /* return key positon */
        }
    }
    return(-1); /* Error */
}

/* $x: Set Level */
int setlvl_cps()
{
    int  i,j;
    char lvc;
    int  nolv;
    char skey[MAXLVL];
    int  grtn;

    /* printf("%s\n",mv->card); */
    nolv = 0;
    i = 3;
    if(mv->card[0] == '{' || mv->card[0] == '(') {
        i = 4;
        if(mv->card[2] == ':') nolv = 1; 
    }
    else if(mv->card[1] == ':') nolv = 1;
    if(nolv) {
        setnolv_cps();
        return(0);
    }
    lvc = '\0';
    if(i == 4) {
        lvc = mv->card[2];
    }
    else {
        lvc = mv->card[1];
    }
    j = 0;
    while(1) {
        if(mv->card[i] == '\0') break;
        if(mv->card[i] == '\r') break;
        if(mv->card[i] == '\n') break;
        if(mv->card[i] == ' ') break;
        if(mv->card[i] == '}') break;
        if(mv->card[i] == ')') break;
        skey[j] = tolower(mv->card[i]);
        j++;
        if(j >= MAXLVL) break;
        i++;
        if(i > 80) break;
    }
    skey[j] = '\0';
    if(mv->debug == 6) printf("setlvl_cps() lvc = %c skey = %s\n",lvc,skey);
    switch(lvc) {
        case  'R' : /* Rule file name */
                  grtn = getRcpn_cps();
                  opencp_meb();
                  break;
        case  '0' : /* Level 0 */
                  opencp_meb();
                  setlv0_cps(skey);
                  break;
        case  '1' : /* Level 1 */
                  opencp_meb();
                  setlv1_cps(skey);
                  break;
        case  '2' :  /* Level 2 */
                  opencp_meb();
                  setlv2_cps(skey);
                  break;
        case  '3' :  /* Level 3 */
                  opencp_meb();
                  setlv3_cps(skey);
                  break;
        case  '4' :  /* Level 4 */
                  opencp_meb();
                  setlv4_cps(skey);
                  break;
        case  '5' :  /* Level 5 */
                  opencp_meb();
                  setlv5_cps(skey);
                  break;
        case  '6' :  /* Level 6 */
                  opencp_meb();
                  setlv6_cps(skey);
                  break;
        case  'A' :  /* Level A */
                  opencp_meb();
                  setlvA_cps(skey);
                  break;
        default : break;
    }
    return(0);
}

/* get Rule file name from {$R:fn} */
int getRcpn_cps()
{
    int  i,j;
    char wk[82];

    /* printf("%s\n",mv->card); */
    j = 3;
    if(mv->card[0] == '{') j = 4;
    for(i = 0 ; j < 82; i++,j++) {
        if(mv->card[j] == '\0') break;
        if(mv->card[j] == '\r') break;
        if(mv->card[j] == '\n') break;
        if(mv->card[j] == ' ') break;
        if(mv->card[j] == '}') break; 
        if(mv->card[j] == ')') break;
        wk[i] = mv->card[j];
    }
    wk[i] = '\0';
    cp->ocpf++;
    strcpy(cp->ocp,wk);
    if(mv->debug == 6) printf("cp->ocp = %s cp->ocpf = %d\n",cp->ocp,cp->ocpf);
    return(0);
}

/* $:T-D ,($:D-T) {$:T-SD} no level */
int setnolv_cps()
{
    int  i,j;     
    char skey[MAXLVL];

    j = 0;
    i = 2;
    if(mv->card[0] == '{') i = 3;  
    if(mv->card[0] == '(') i = 3;
    while(1) {
        if(mv->card[i] == '\0') break;
        if(mv->card[i] == '\r') break;
        if(mv->card[i] == '\n') break;
        if(mv->card[i] == ' ') break;
        if(mv->card[i] == '}') break;
        if(mv->card[i] == ')') break;
        skey[j] = tolower(mv->card[i]);
        j++;
        if(j >= MAXLVL) break;
        i++;
        if(i > 80) break;
    }
    skey[j] = '\0';
    /* printf("setnolv skey = %s\n",skey); */
    setlv6_cps(skey);
    setlv5_cps(skey);
    setlv4_cps(skey);
    setlv3_cps(skey);
    setlv2_cps(skey);
    setlv1_cps(skey);
    setlv0_cps(skey);
    return(0);
}

/* ! $ Call from chord_cps() */
int cpserch_cps(char skey[MAXLVL])
//char skey[MAXLVL];
{
    int  i,j,k;
    int  sf;
    int  bf;
    int  mf;
    int  rtn;
    char fn[STLN];
    char line[82];
    char work[82];
    char tkey[MAXLVL];
    char wkey[MAXLVL];
    char bc[16];
    char bk[16];

    i = 0;
    bf = 0;
    while(1) {
        if(skey[i] == '/') {
            bf = 1;
            break;
        }
        if(skey[i] == '\0') break;
        wkey[i] = skey[i];
        i++;
        if(i >= MAXLVL) break;
    }
    wkey[i] = '\0';
    if(bf) {
        cp->bf = 1;
        j = 0;
        i++;
        while(1) {
            if(skey[i] == ' ') break;
            cp->bg[j] = skey[i];
            i++;
            if(i >= MAXLVL) break;
            j++;
            if(j >= 7) break;
        }
        j++;
        cp->bg[j] = '\0';
    }
    else {
        cp->bf = 0;
        cp->bg[0] = '\0';
    }
    if(mv->debug == 6) printf("cpserch() skey = %s cp->bf = %d cp->bg = %s\n",
        skey,cp->bf,cp->bg);
    i = 0;
    k = strlen(wkey);
    mf = 0;
    sf = 0;
    while(1) {
        if(wkey[i] == '\0') break;
        if(wkey[i] == 'm') mf = 1;
        if(wkey[i] == 'M') mf = 2;
        if(wkey[i] == '7') mf = 3;
        if(wkey[i] == '6') mf = 3;
        if(wkey[i] == 'd') mf = 3;
        if(wkey[i] == 'I') mf = 3;
        if(wkey[i] == 'V') mf = 3;
        if(wkey[i] == 'C') sf = 1;
        if(wkey[i] == 'D') sf = 1;
        if(wkey[i] == 'E') sf = 1;
        if(wkey[i] == 'F') sf = 1;
        if(wkey[i] == 'G') sf = 1;
        if(wkey[i] == 'A') sf = 1;
        if(wkey[i] == 'B') sf = 1;
        i++;
        if(i > k) break;
    }
    if(!mf && sf) {
        wkey[k] = 'M';
        wkey[k+1] = '\0';
    }
    if(mv->debug == 6) printf("skey =%s wkey = %s cp->bf = %d cp->bg = %s\n",
        skey,wkey,cp->bf,cp->bg);
    sf = bgchk_meb(bc,bk); /* Base Chord check */
    cp->pol = 0;
    if(sf) {
        cp->pol = 1;
        sf = Poly_key(skey,wkey,bc,bk);
        return(sf);
    }
    sf = 0;
    k = 0;
    rtn = 0;
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]); 
        if(cp->key == 2) strcpy(line,irule[k]); 
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        if(line[0] == '!') {
            /* printf("%s",line); */
            i = 1;
            j = 0;
            while(line[i] != ' ') {
                tkey[j] = line[i];
                i++;
                j++;
                if(j >= MAXLVL) break;
            }
            tkey[j] = '\0';
            if(!strcmp(tkey,wkey)) {
                sf = 1;
                if(mv->debug == 6) printf("Debug6! %s",line);
                i = 1;
                while(1) {
                    if(line[i] == '\0') break;
                    if(line[i] == '\r') break;
                    if(line[i] == '\n') break;
                    if(line[i] == '\"') line[i] = ' '; 
                    i++;
                    if(i > 80) break;
                }
                rtn = cpserch1_cps(line); /* !SKEY = $TergetStr */
                if(mv->debug == 6) printf("cpserch_(1) rtn = %d\n",rtn);
                if(rtn) sf = 0;
            } 
        }
    }
    if(!sf) {
        cpserch3_cps(skey,wkey); 
        if(skey[0] == 'I') sf = 2;
        if(skey[0] == 'V') sf = 3;
        if(skey[1] == 'I') sf = 2;
        if(skey[1] == 'V') sf = 3;
        if(mv->debug == 6) printf("sf = %d rtn = %d\n",sf,rtn);
        if(sf || rtn) {
            sf = Special_key(line,skey,wkey);
        }
        if(sf) {
            i = 0;
            while(1) {
                if(mv->card[i] == ' ') break;
                if(mv->card[i] == ',') break;
                if(mv->card[i] == '\r') break;
                if(mv->card[i] == '\n') break;
                if(mv->card[i] == '\0') break;
                i++;
                if(i > 80) break;
            }
            mv->card[i] = '\0';
            mv->wrncnt++;
            mv->msglvl = 2;
            if(mv->msglvl >= mv->prtlvl)
            printf("Worning! %s is not found in Rule file\n",skey);
        }
    }
    return(0);
}

int cpget_cps(char tk[MAXLVR],char rtey[MAXLVR])
//char tk[MAXLVR];
//char rtey[MAXLVR];
{
    int  i,j,k;
    int  lv;
    char sk[MAXLVL];

    lv = 0;
    if(mv->debug == 6) printf("Debug6! cpget_cps() tk=%s\n",tk);
    i = 3;
    if(tk[2] != ':') {
        lv = 1;
        i = 1;
    }
    else lv = 0x0f & tk[1];
    if(lv < 0 || lv > 9) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! cpget rule file %s\n",tk);
        return(-1);
    }
    j = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == '~') break;  /* -> VIm/VII~2 */
        sk[j] = tk[i];
        i++;
        if(i > MAXLVL) break;
        j++;
    }
    sk[j] = '\0';
    if(mv->debug == 6) printf("cpget lv = %d sk = %s cp->n[%d] = %d\n",
        lv,sk,lv,cp->n[lv]);
    rtey[0] = '\0';
    for(k = 0; k < cp->n[lv]; k++) {
        if(!strcmp(cp->l[lv][k],sk)) {
            strcat(rtey,cp->r[lv][k]);
            if(mv->debug == 6) printf("cpget rtey = %s\n",rtey);
            return(0);
        }
    }
    return(1);
}

/* $m:I_Major/Minor Modulation */
int modget_cps(char tk[MAXLVR],char mk[MAXLVR],char rk[MAXLVR])
//char tk[MAXLVR];
//char mk[MAXLVR];
//char rk[MAXLVR]; /* return key */
{
    int  i,j,k;
    int  mod_kn;      /* modulate key number */
    int  mnf;         /* Major Minor flag */
    int  pf;
    char wk[18];
    char ck[4];
    char c_key[4];    /* current key name */
    char w_key[4];    /* current key temp name */

    if(mv->debug == 6) printf("Debug6! modget_cps() tk=%s mk=%s\n",tk,mk);
    rk[0] = '\0';
    i = 0;
    mnf = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == 'j') mnf = 1; /* Major */
        if(tk[i] == 'i') mnf = 2; /* Minor */
        i++;
        if(i > MAXLVR) break;
    }
    if(!mnf) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! modget_(1) rule file %s\n",tk);
        return(1);
    }
    c_key[0] = mk[0];
    c_key[1] = mk[1];
    c_key[2] = '\0';
    /* get mod_kn */
    i = 0;
    pf = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == ':') {
            pf = 1;
            break;
        }
        i++;
        if(i > MAXLVR) break;
    }
    if(!pf) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! modget_(2) rule file %s\n",tk);
        return(1);
    }
    i++;
    j = 0;
    pf = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == '_') {
            pf = 1;
            break;
        }
        wk[j] = tk[i];
        i++;
        if(i > MAXLVR) break;
        j++;
        if(j > 18) {
            pf = 0; /* Error */
            break;
        }
    }
    wk[j] = '\0';
    if(!pf) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! modget_(3) rule file %s\n",tk);
        return(1);
    }
    if(mv->debug == 6) printf("Debug6! c_key = %s wk = %s\n",c_key,wk);
    i = 0;
    j= 0;
    while(1) {
        if(wk[i] != 'I' && wk[i] != 'V' && wk[i] != '&' &&
            wk[i] != '#') break;
        if(wk[i] == 'I' || wk[i] == 'V') {
            ck[j] = wk[i];
            j++;
            if(j > 3) break;
        }
        i++;
        if(i > 6) break;
    }
    ck[j] = '\0';
    if(mv->debug == 6) printf("Debug6! ck = %s\n",ck);
    mod_kn = 0;
    if(!strcmp(ck,"I")) mod_kn = 0;
    if(!strcmp(ck,"II")) mod_kn = 1;
    if(!strcmp(ck,"III")) mod_kn = 2;
    if(!strcmp(ck,"IV")) mod_kn = 3;
    if(!strcmp(ck,"V")) mod_kn = 4;
    if(!strcmp(ck,"VI")) mod_kn = 5;
    if(!strcmp(ck,"VII")) mod_kn = 6;
    /* get mod_key */
    /* c_key = mktb[c_kn][0]; */
    pf = 0;
    if(mnf == 1) {
        for(i = 0; i < 12; i++) {
            if(mv->debug == 6) {
                printf("Debug6! maj[%d].cn[%d] = %s ,c_key = %s\n",
                i,mod_kn,maj[i].cn[mod_kn],c_key);
            }
            if(!strcmp(maj[i].cn[mod_kn],c_key)) {
                strcpy(rk,maj[i].key);
                pf = 1;
            }
        }
    }
    if(mnf == 2) {
        for(i = 0; i < 12; i++) {
            if(mv->debug == 6) {
                printf("Debug6! min[%d].cn[%d] = %s ,c_key = %s\n",
                i,mod_kn,min[i].cn[mod_kn],c_key);
            }
            if(!strcmp(min[i].cn[mod_kn],c_key)) {
                strcpy(rk,min[i].key);
                pf = 1;
            }
        }
    }
    if(pf) {
        strcat(rk," ");
        strcat(rk,wk);
    }
    else {
        if(!strcmp(c_key,"D&")) strcpy(w_key,"C#");
        if(!strcmp(c_key,"E&")) strcpy(w_key,"D#");
        if(!strcmp(c_key,"G&")) strcpy(w_key,"F#");
        if(!strcmp(c_key,"A&")) strcpy(w_key,"G#");
        if(!strcmp(c_key,"B&")) strcpy(w_key,"A#");
        pf = getmodk_meb(mnf,w_key,mod_kn,rk);
        if(pf) {
            strcat(rk," ");
            strcat(rk,wk);
        }
        else {
            if(!strcmp(c_key,"C#")) strcpy(w_key,"D&");
            if(!strcmp(c_key,"D#")) strcpy(w_key,"E&");
            if(!strcmp(c_key,"F#")) strcpy(w_key,"G&");
            if(!strcmp(c_key,"G#")) strcpy(w_key,"A&");
            if(!strcmp(c_key,"A#")) strcpy(w_key,"B&");
            pf = getmodk_meb(mnf,w_key,mod_kn,rk);
            if(pf) {
                strcat(rk," ");
                strcat(rk,wk);
            }
            else {
                strcat(rk,",No ");
                strcat(rk,wk);
            }
        }
    }
    if(mv->debug == 6) {
        printf("cp->key = %d mnf = %d\n",cp->key,mnf);
        printf("c_key= %s mod_kn= %d rk = %s\n",c_key,mod_kn,rk);
    }
    return(0);
}

int getmodk_meb(int mnf,char c_key[4],int mod_kn,char rk[MAXLVR])
//int  mnf; /* Major/minor flag */
//char c_key[4];
//int  mod_kn;
//char rk[MAXLVR];
{
    int  i;
    int  pf;

    pf = 0;
    if(mnf == 1) {
        for(i = 0; i < 12; i++) {
            if(mv->debug == 6) {
                printf("Debug6! maj[%d].cn[%d] = %s ,c_key = %s\n",
                i,mod_kn,maj[i].cn[mod_kn],c_key);
            }
            if(!strcmp(maj[i].cn[mod_kn],c_key)) {
                strcpy(rk,maj[i].key);
                pf = 1;
            }
        }
    }
    if(mnf == 2) {
        for(i = 0; i < 12; i++) {
            if(mv->debug == 6) {
                printf("Debug6! min[%d].cn[%d] = %s ,c_key = %s\n",
                i,mod_kn,min[i].cn[mod_kn],c_key);
            }
            if(!strcmp(min[i].cn[mod_kn],c_key)) {
                strcpy(rk,min[i].key);
                pf = 1;
            }
        }
    }
    return(pf);
}

int cpserch1_cps(char line[82])
//char line[82]; 
{
    int  i,j,k;
    int  pf;
    int  lv;
    int  rtn;
    char sk[MAXLVL];
    char wk[82];

    i = 0;
    pf = 0;
    if(mv->debug == 6) printf("Debug6! cpserch1_cps() %s",line);
    beauspace_meb(line);
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '=') {
            pf = 1;
            break;
        }
        i++;
        if(i > 80) break;
    }
    if(!pf) {
        strcpy(mv->card,line);
        /*
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
        printf("Error! chserch1(1) rule file %s\n",line);
        */
        return(1);
    }
    pf = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '$') {
            pf++;
            break;
        }
        i++;
        if(i > 80) break;
    }
    if(!pf) { /* $ Char not found */
        strcpy(mv->card,line);
        /*
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! cpserch1(2) rule file %s\n",line);
        return(1);
        */
        return(2);
    }
    lv = 0x0f & line[++i];
    if(lv < 0 || lv > 9) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! rule file %s\n",line);
        return(3);
    }
    if(line[++i] != ':') {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl)
            printf("Error! cpserch1(3) rule file %s\n",line);
        return(4);
    }
    j = 0;
    i++;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        sk[j] = line[i];
        i++;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    sk[j] = '\0';
    if(lv == 2 && cp->mf) strcat(sk,cp->star); 
    if(mv->debug == 6) printf("cpserch1(4) lv = %d sk = %s cp->n[%d] = %d\n",
        lv,sk,lv,cp->n[lv]);
    rtn = 5;
    for(k = 0; k < cp->n[lv]; k++) {
        if(!strcmp(cp->l[lv][k],sk)) {
            wk[0] = '\0';
            strcat(wk,"!");
            strcat_cps(wk,cp->r[lv][k]);
            /* printf("wk = %s\n",wk); */
            i = 0;
            pf = 0;
            while(1) {
                if(wk[i] == '\0') break;  
                if(wk[i] == '\r') break;
                if(wk[i] == '\n') break;
                if(wk[i] == '$') pf++;
                i++;
                if(i > 80) break;
            }
            if(pf) rtn = cpserch2_cps(sk,wk);
            strcpy(mv->card,wk);
            break;
        }
    }
    return(rtn);
}

int cpserch2_cps(char cn[MAXLVL],char wk[82])
//char cn[MAXLVL];
//char wk[82];
{
    int  i,j,k,n,kk;
    int  pf;
    int  af;
    int  len;
    char line[82];
    char rest[82];
    char sk[MAXLVR];
    char tk[MAXLVR];
    char rk[MAXLVR];
    int  cf;
    int  chdf;
    int  oaf; /* on chord rtn */
    char cd[16];
    char onc[MAXLVR];
    char mk[MAXLVR];
    int  modf;
    char wa[MAXLVR];
    char wr[MAXLVR];
    char c_key[8];
    int  onchk;

    strcpy(line,wk);
    beauspace_meb(line);
    if(mv->debug == 6) printf("cpserch2_(1) wk = %s\n",wk);
    /* if(cp->chgf) setnpcd_meb(wk); */
    setnpcd_meb(wk);
    i = 0;
    pf = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == '$') {
            pf++;
        }
        i++;
        if(i > 80) break;
    }
    if(pf <= 0) return(6);
    af = 1;
    if(cp->bf) {
        strcpy(tk,"$1:");
        strcat(tk,cp->bg);
        af = cpget_cps(tk,onc);
        if(mv->debug == 6) printf("cpserch2_(2) af = %d cp->bg = %s onc = %s\n",
            af,cp->bg,onc);
        if(af) return(af);
        strcpy(cp->bc,onc);
    }
    oaf = 0;
    if(!af) oaf = 1;
    /* Arg */
    i = 0;
    onchk = 0;
    while(1) {
        wk[i] = line[i];
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        if(line[i] == '/') onchk++;
        i++;
        if(i > 80) break;
    }
    wk[i] = '\0';
    if(oaf && onchk == 0) {
        strcat(wk,"/");
        strcat(wk,cp->bg);
    }
    mk[0] = '\0';
    i = 0;
    for(k = 0; k < pf; k++) {
        af = 0;
        while(1) {
            if(line[i] == '\0') break;
            if(line[i] == '\r') break;
            if(line[i] == '\n') break;
            if(line[i] == '$') {
                af = 1;
                break;
            }
            i++;
            if(i > 80) break;
        }
        if(!af) break;
        j = 0;
        while(1) {
            if(line[i] == '\0') break;
            if(line[i] == '\r') break;
            if(line[i] == '\n') break;
            if(line[i] == ' ') break;
            if(line[i] == '-') break;
            /* if(line[i] == '_') break; */
            if(line[i] == '.') break;
            if(line[i] == ',') break;
            if(line[i] == '[') break;
            if(line[i] == ']') break;
            tk[j] = line[i];
            i++;
            if(i > 80) break;
            j++;
            if(j >= MAXLVR) break;
        }
        tk[j] = '\0';
        modf = 0;
        if(tk[0] == '$') {
            if(tk[1] == 'm' && tk[2] == ':') { /* modkey */
                af = modget_cps(tk,mk,rk);
                modf = 1;
                sk[0] = '\0';
                strcat(sk,"=> ");
                strcat(sk,rk);
            }
            else {
                cf = tkcheck_meb(tk,cd);
                af = cpget_cps(tk,sk);
                strcpy(mk,sk); /* save c_key */
                if(strlen(sk) < 8) strcpy(c_key,sk); /* save c_key */
            }
            if(mv->debug == 6) printf("cpserch2_(3) af = %d tk = %s sk = %s\n",
                af,tk,sk);
            if(!af) {
                if(tk[1] == '4' && cp->bf) {
                    j = strlen(sk);
                    n = 6;
                    chdf = 0;
                    while(1) {
                        if(sk[n] == '\0') break;
                        if(sk[n-1] == 'c' && sk[n] == 'h') {
                            chdf = 1;
                            break;
                        }
                        n++;
                        if(n > j) break;
                    }
                    if(chdf) {
                        sk[n-1] = '\0';
                        strcat(wk," ");
                        strcat(wk,sk);
                        strcat(wk,"on ");
                        strcat(wk,onc);
                        strcat(wk," chord");
                    }
                    else {
                        strcat(wk,sk);
                    }
                    if(mv->debug == 6)
                        printf("cpserch2 (on) sk = %s onc = %s chdf = %d\n",
                            sk,onc,chdf);
                }
                else {
                    if(sk[0] != 'R') strcat_cps(wk," "); /* Root */
                    strcat_cps(wk,sk);
                    if(cf == 1 && modf == 0) strcat(wk,cd);
                    /* printf("wk = %s cd = %s\n",wk,cd); */
                    /*
                    if(tk[2] != ':') {
                        if(oaf) {
                            strcat(wk,"/");
                            strcat(wk,onc);
                        }
                        strcat(wk," ,");
                        strcat(wk,sk);
                    }
                    if(tk[2] != ':') {
                        strcat(wk," ,");
                    }
                    */
                    /* printf("wk = %s\n",wk); */
                }
            }
            else strcat(wk," ..");
            /* printf("wk = %s\n",wk); */
        }
    }
    /* rest line ! */
    if(ma->nsf) {
        /* printf("cn = %s\n",cn); */
        wr[0] = '\0';
        for(kk = 0; kk < ma->n; kk++) {
            if(!strcmp(ma->l[kk],cn)) {
                strcpy(wa,ma->r[kk]);
                strcpy(Scale,ma->r[kk]);
                Scaf = 1;
                strcat(wk," ,");
                strcat(wk,c_key); 
                k = 0;
                while(1) {
                    if(wa[k] == '\0') break;
                    if(wa[k] == '_') break;
                    k++;
                    if(k > MAXLVR) break;
                }
                j = 0;
                while(1) {
                    if(wa[k] == '\0') break;
                    wr[j] = wa[k];
                    k++;
                    if(k > MAXLVR) break;
                    j++;
                }
                wr[j] = '\0';
                strcat(wk,wr);
                if(mv->debug == 6) {
                printf("Scale = %s wr = %s c_key = %s\n",Scale,wr,c_key);
                printf("kk = %d wk = %s\n",kk,wk);
                }
                break;
            }
        }
    }
    len = strlen(line);
    if(i < len) {
        j = 0;
        while(1) {
            if(line[i] == '\0') break;
            if(line[i] == '\r') break;
            if(line[i] == '\n') break;
            rest[j] = line[i];
            j++;
            i++;
            if(i >= len) break;
        }
        rest[j] = '\0';
        strcat_cps(wk,rest);
    }
    return(0);
}

/* check $x:! or $!! */
int tkcheck_meb(char tk[MAXLVR],char cd[16])
//char tk[MAXLVR];
//char cd[16];
{
    int  i,j,k;
    int  af;

    i = 0;
    af = 0;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] == ':') return(0); 
        i++;
        if(i > MAXLVR) break;
    }
    i = 1;
    if(tk[1] == '#' || tk[1] == '&') i = 2;
    while(1) {
        if(tk[i] == '\0') break;
        if(tk[i] != 'I' && tk[i] != 'V') break;
        i++;
        if(i > MAXLVR) break;
    }
    j = i;
    k = 0;
    while(1) {
        if(tk[i] == '\0') break;
        cd[k] = tk[i];
        i++;
        if(i > MAXLVR) break;
        k++;
        if(k > 16) return(-1);
    }
    cd[k] = '\0';
    tk[j] = '\0';
    return(1);
}

int cpserch3_cps(char skey[MAXLVL],char wkey[MAXLVL])
//char skey[MAXLVL];
//char wkey[MAXLVL];
{
    int  i,j;
    int  k;
    char Cn[8];
    int  pf;
    char tk[MAXLVR];
    char sk[MAXLVR];
    char wk[82];
    char tkey[MAXLVL];

    k = strlen(wkey);
    if(mv->debug == 6) printf("cpserch3 () skey = %s wkey = %s k = %d\n",
        skey,wkey,k);
    i = 0;
    pf = 0;
    while(1) {
        if(wkey[i] == 'I') pf++;
        if(wkey[i] == 'V') pf++;
        i++;
        if(i > k) break;
    }
    if(pf) return(0);
    i = k;
    while(1) {
        if(wkey[i] == '&') break;
        if(wkey[i] == '#') break;
        if(wkey[i] == 'C') break;
        if(wkey[i] == 'D') break;
        if(wkey[i] == 'E') break;
        if(wkey[i] == 'F') break;
        if(wkey[i] == 'G') break;
        if(wkey[i] == 'A') break;
        if(wkey[i] == 'B') break;
        i--;
        if(i < 0) return(-1);
    }
    ++i;
    j = 0;
    while(1) {
        Cn[j] = wkey[i];
        i++;
        if(i > k) break;
        j++;
        if(j > 8) break;
    }
    Cn[j] = '\0';
    strcpy(tk,"$4:");
    strcat(tk,Cn);
    if(mv->debug == 6) printf("cpserch3 (1) wkey = %s k = %d Cn = %s\n",
        wkey,k,Cn);
    pf = cpget_cps(tk,sk);
    if(mv->debug == 6) printf("cpserch3 (2) tk = %s sk = %s pf = %d\n",
        tk,sk,pf);
    if(!pf) {
        strcpy(wk,"!");
        strcat(wk,skey);
        strcat(wk," ");
        i = 0;
        k = strlen(skey);
        while(1) {
            if(skey[i] == '/') break;
            if(skey[i] == '\0') break;
            tkey[i] = skey[i];
            i++;
            if(i > k) break;
        }
        tkey[i] = '\0';
        strcat(wk,",");
        strcat(wk,tkey);
        strcat(wk," ");
        if(cp->bf) {
            k = strlen(sk);
            i = 6;
            pf = 0;
            while(1) {
                if(sk[i] == '\0') break;
                if(sk[i-1] == 'c' && sk[i] == 'h') {
                    pf = 1;
                    break;
                }
                i++;
                if(i > k) break;
            }
            if(pf) {
                sk[i-1] = '\0';
                strcat(wk,sk);
                strcat(wk,"on ");
                strcat(wk,cp->bc);
                strcat(wk," chord");
            }
            else {
                strcat(wk,sk);
            }
            if(mv->debug == 6) printf("cpserch3 (3) sk = %s pf = %d\n",sk,pf);
        }
        else strcat(wk,sk);
    }
    strcpy(tk,"$3:");
    strcat(tk,Cn);
    pf = cpget_cps(tk,sk);
    if(mv->debug == 6) printf("cpserch3 (4) tk = %s sk = %s pf = %d\n",
        tk,sk,pf);
    if(!pf) strcat(wk,sk);
    strcpy(mv->card,wk);
    return(0);
}

int setkey_cps()
{
    int  i,j,k;
    int  af;
    char wk[MAXLVL];
    char skey[MAXLVL];

    if(mv->debug == 6) printf("%s\n",mv->card);
    i = 0;
    af = 0;
    while(mv->card[i] != '\0') {
        if(mv->card[i] == '_') {
            af = 1;
            break;
        }
        if(mv->card[i] == '\r') break;
        if(mv->card[i] == '\n') break;
        i++;
        if(i > 80) break;
    }
    if(!af) return(1); /* Major/Minor key not found */
    k = i;
    i++;
    j = 0;
    while(mv->card[i] != ' ') {
        if(mv->card[i] == '}') break;
        if(mv->card[i] == ')') break;
        wk[j] = tolower(mv->card[i]);
        if(mv->card[i] == '\r') break;
        if(mv->card[i] == '\n') break;
        i++;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    wk[j] = '\0';
    af = 0;
    if(!strcmp(wk,"major")) af = 1;
    if(!strcmp(wk,"dur")) af = 1;
    if(!strcmp(wk,"minor")) af = 2;
    if(!strcmp(wk,"moll")) af = 2;
    if(af) cp->key = af; /* Major/Minor */
    if(mv->debug == 6) printf("Level 0 skey = %s key = %d\n",wk,cp->key);
    opencp_meb();
    setlv0_cps(wk);
    setlv2_cps("default_chord_progression");
    setlv3_cps("default_root");
    setlv4_cps("default_name");
    setlv5_cps("default_note");
    setlv6_cps("default_chord_select");
    setlvA_cps("default_scale");
    if(cp->n[5] == 0) setlv5_cps("default_note");
    i = k;
    while(mv->card[i] != ' ') {
        i--;
        if(i <= 0) break;
    }
    i++;
    for( j = 0; i <= k; i++,j++) {
        if(j >= MAXLVL) break;
        skey[j] = tolower(mv->card[i]);
    }
    skey[j] = '\0';
    strcat(skey,wk);
    if(mv->debug == 6) printf("Level 1 skey = %s\n",skey);
    setlv1_cps(skey);
    /* setlvA_cps(skey); */
    return(0);
}

int setlv0_cps(char wk[MAXLVL])
//char wk[MAXLVL];
{
    int  i,k;
    int  af;
    int  ef;
    int  ac;
    char line[82];

    af = 0;
    ac = 0;
    k = 0;
    if(mv->debug == 6) printf("Debug6! setlv0_cps() sk=%s\n",wk);
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]); 
        if(cp->key == 2) strcpy(line,irule[k]); 
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        ef = 0;
        if(line[0] == '$' && line[1] == '0' && line[2] == ':') {
            /* printf("%s",line); */
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
                af = linechk_cps(line,wk);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) cp->n[0] = 0;
                    strcpy(cp->mk[0],wk); /* set level 0 marker */
                    setval_cps(line,0); /* set variable level 0 */
                    ac++;
                }
            }
        }
    }
    return(0);
}

int setlv1_cps(char skey[MAXLVL])
//char skey[MAXLVL];
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
    if(mv->debug == 6) {
        printf("setlv1_cps() skey=%s ",skey);
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
        if(line[0] == '$' && line[1] == '1' && line[2] == ':') sf = 1;
        if(line[0] == '$' && line[2] != ':') sf = 2;
        if(sf) {
            /* printf("%s",line); */
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
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) cp->n[1] = 0;
                    strcpy(cp->mk[1],skey); /* set level 1 marker */
                    setval_cps(line,1);   /* set variable level 1 */
                    ac++;
                }
            }
        }
    }
    return(0);
}

int setlv2_cps(char skey[MAXLVL])
//char skey[MAXLVL];
{
    int  i,k;
    int  af;
    int  sf;
    int  ef;
    int  ac;
    int  rtn;
    char line[82];

    if(mv->debug == 6) {
        printf("setlv2_cps() Orfn=%s skey=%s ",Orfn,skey);
        if(cp->key == 1) printf("cp->key= %d cp->jky=%d\n",cp->key,cp->jky);
        if(cp->key == 2) printf("cp->key= %d cp->iky=%d\n",cp->key,cp->iky);
    }
    if(mv->step == 3) {
        mv->msglvl = 0;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Select %s from rule file %s\n",skey,Orfn);
        }
    }
    af = 0;
    ac = 0;
    k = 0;
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]); 
        if(cp->key == 2) strcpy(line,irule[k]); 
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        ef = 0;
        sf = 0;
        if(line[0] == '$' && line[1] == '2' && line[2] == ':') sf = 1;
        if(sf) {
            /* printf("%s",line); */
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
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) cp->n[2] = 0;
                    strcpy(cp->mk[2],skey); /* set level 2 marker */
                    setval_cps(line,2);   /* set variable level 1 */
                    ac++;
                }
            }
        }
    }
    return(0);
}

int linechk_cps(char line[82],char skey[MAXLVL])
//char line[82];
//char skey[MAXLVL];
{
    int  i,j;
    int  n;
    char tkey[MAXLVL];

    i = 3;
    j = 0;
    /* 2005.2.5 
    tkey[0] = islower(line[3]);
    tkey[1] = islower(line[4]);
    tkey[2] = islower(line[5]);
    */
    tkey[0] = tolower(line[3]);
    tkey[1] = tolower(line[4]);
    tkey[2] = tolower(line[5]);
    tkey[3] = '\0';
    if(!strcmp(tkey,"end")) return(0);  /* END */
    beauspace_meb(line);
    n = strlen(line);
    /* 1 */
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf("linechk skey = %s tkey = %s",skey,tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 2 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 3 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 4 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 5 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 6 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 7 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 8 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    /* 9 */
    i++;
    j = 0;
    while(1) {
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        if(line[i] == ' ') break;
        tkey[j] = tolower(line[i]);
        i++;
        if(i >= n) break;
        if(i > 80) break;
        j++;
        if(j >= MAXLVL) break;
    }
    tkey[j] = '\0';
    if(mv->debug == 6) printf(" %s",tkey);
    if(!strcmp(tkey,skey)) {
        if(mv->debug == 6) printf("\n");
        return(1);  /* Match */
    }
    if(mv->debug == 6) printf("\n");
    return(0);
}
 
int setval_cps(char line[82],int lv)
//char line[82];
//int  lv;
{
    int  i,j;
    int  af;
    char sl[MAXLVL];
    char sr[MAXLVR];

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
        if(line[i] != '\'' && line[i] != '\"') sr[j++] = line[i];
        i++;
        if(i > 80) break;
        if(j >= MAXLVR) break;
    }
    sr[j] = '\0';
    /* printf("sl = %s sr = %s\n",sl,sr); */
    strcpy(cp->l[lv][cp->n[lv]],sl);
    strcpy(cp->r[lv][cp->n[lv]],sr);
    if(mv->debug == 6) {
        printf("lv=%d n=%d %s %s\n",
            lv,cp->n[lv],cp->l[lv][cp->n[lv]],cp->r[lv][cp->n[lv]]);
    }
    cp->n[lv]++;
    if(cp->n[lv] > MAXLVA) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Error! rule Level %d argument over than %d\n",
                lv,MAXLVA);
        }
        cp->n[lv]--;
    }
    return(0);
}

/* $6: Chord select only */
int setval6_cps(char line[82])
//char line[82];
{
    int  i,j;
    int  af;
    char sl[MAXLVL];
    char sm[MAXLVR];
    char sr[MAXLVR];

    i = 3;
    j = 0;
    sl[0] = '\0';
    sm[0] = '\0';
    sr[0] = '\0';
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
    while(1) {
        if(line[i] == ' ') break;
        if(line[i] == '\0') break;
        if(line[i] == '\r') break;
        if(line[i] == '\n') break;
        sm[j] = line[i];
        i++;
        if(i > 80) break;
        j++;
        if(j >= MAXLVR) break;
    }
    sm[j] = '\0';
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
        if(line[i] != '\'' && line[i] != '\"') sr[j++] = line[i];
        i++;
        if(i > 80) break;
        if(j >= MAXLVR) break;
    }
    sr[j] = '\0';
    /* printf("sl = %s sm = %s\n",sl,sm); */
    strcpy(cs->l[cs->n],sl);
    strcpy(cs->m[cs->n],sm);
    strcpy(cs->r[cs->n],sr);
    if(mv->debug == 8) {
        printf("lv=6 n=%d   cs->l= %s     m= %s     r= \"%s\"\n",
            cs->n,cs->l[cs->n],cs->m[cs->n],cs->r[cs->n]);
    }
    cs->n++;
    if(cs->n > MAXLVA) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Error! rule Level 6 argument over than %d\n",MAXLVA);
        }
        cs->n--;
    }
    return(0);
}

int opencp_meb()
{
    int  i,j;
    int  nof;
    int  ofb; /* open file number */
    FILE *fcp;
    char wk[STLN];
    char ofn[STLN];
    char cfn[STLN];
    char cmf[STLN];
    char line[82];
    char tkey[MAXLVL];
    char scrf[100];

    /*
    if(cp->key == 1 && cp->jky) return(0);
    if(cp->key == 2 && cp->iky) return(0);
    if(cp->key != 1 && cp->key != 2) return(0); 
    */
    if(cp->ocpf) {
        /* if(!strcmp(Ocps,cp->ocp)) return(0); */
        if(cp->key != 1 && cp->key != 2) return(0);
        strcpy(Ocps,cp->ocp);
        if(cp->key == 1) {
            strcpy(ofn,cp->ocp);
            strcat(ofn,"cpj.dat");
            if(!strcmp(Orfn,ofn) && cp->jky) return(0);
        }
        if(cp->key == 2) {
            strcpy(ofn,cp->ocp);
            strcat(ofn,"cpi.dat");
            if(!strcmp(Orfn,ofn) && cp->iky) return(0);
        }
    }
    else {
        if(cp->key == 1 && cp->jky) return(0);
        if(cp->key == 2 && cp->iky) return(0);
        if(cp->key != 1 && cp->key != 2) return(0);
        if(cp->ocpf == 9) return(0);
    }
    i = 0;
    while(1) {
        if(mfile[i] == '_') break;
        if(mfile[i] == '\0') break;
        if(mfile[i] == '\r') break;
        if(mfile[i] == '\n') break;
        cfn[i] = mfile[i];
        i++;
    }
    cfn[i] = '\0';
    strcpy(cmf,cfn);
    if(cp->key == 1) {
        strcat(cfn,"cpj.dat");
        strcpy(cmf,"mebcpj.dat");
    }
    if(cp->key == 2) {
        strcat(cfn,"cpi.dat");
        strcpy(cmf,"mebcpi.dat");
    }
    nof = 1;
    ofb = 0;
    cp->iky = 0;
    cp->jky = 0;
    if(cp->ocpf) {
        nof = 0;
        if(mv->debug == 6) printf("ofn = %s ",ofn);
        /* strcpy(Orfn,ofn); */
        if((fcp = fopen(ofn,"r")) == NULL) { /* ofncpj.dat or ofncpi.dat */
            nof = 1;
            sprintf(scrf,"%s",TACTDIR);
            //sprintf(scrf,"%s\0",TACTDIR);
            strcat(scrf,ofn);
            /* strcpy(scrf,"/usr/local/etc/tact/"); */
            if(mv->debug == 6) printf("scrf = %s\n",scrf);
            if((fcp = fopen(scrf,"r")) == NULL) {
                if(!mv->wrnpf) {
                    mv->wrncnt++;
                    mv->msglvl = 2;
                    if(mv->msglvl >= mv->prtlvl) {
                        printf("Worning! Rule file %s not found\n",scrf);
                        mv->wrnpf = 1;
                    }
                }
                nof = 1;
            } 
            else {
                ofb = 2;
                cp->ocpf = 9;
            }
        } 
        else {
            ofb = 1;
            cp->ocpf = 9;
        }
    }
    if(mv->debug == 6) printf("cp->key = %d mfile = %s cfn = %s cmf = %s\n",
        cp->key,mfile,cfn,cmf);
    if(nof) {
        if((fcp = fopen(cfn,"r")) == NULL) {
            if((fcp = fopen(cmf,"r")) == NULL) { /* mebcpj.dat or mebcpi.dat */
                sprintf(scrf,"%s",TACTDIR);  
                //sprintf(scrf,"%s\0",TACTDIR);  
                strcat(scrf,cmf);
                /* strcpy(scrf,"/usr/local/etc/tact/"); */
                if(mv->debug == 6) printf("scrf = %s\n",scrf);
                if((fcp = fopen(scrf,"r")) == NULL) {
                    mv->errcnt++;
                    mv->msglvl = 3;
                    if(mv->msglvl >= mv->prtlvl) {
                        printf("Error! opencp_() Can't open %s mode r\n",scrf);
                    }
                    return(-1);
                }
            } else ofb = 4;
        } else ofb = 3;
    }
    switch (ofb) {
        case 1 : strcpy(wk,ofn);
                 break;
        case 2 : strcpy(wk,scrf);
                 break;
        case 3 : strcpy(wk,cfn);
                 break;
        case 4 : strcpy(wk,cmf);
                 break;
        defult : break;
    }
    strcpy(Orfn,wk);
    if(mv->step == 3) {
        if(mv->card[2] == 'R' || mv->card[1] == 'R') { /* {$R: || $R: ? */
            mv->msglvl = 0; /* -m0 option */
            if(mv->msglvl >= mv->prtlvl) {
                printf("Current Rule file is %s\n",wk);
            }
        }
    }
    if(mv->debug == 6) printf("Open file = %s cp->key = %d\n",wk,cp->key);
    if(cp->key == 1) {
        while(fgets(jrule[cp->jky], 82, fcp)) { /* Major */ 
            /* if(mv->debug == 6) printf("%s",jrule[cp->jky]); */
            cp->jky++;
            if(cp->jky > MAXRL) break;
        }
        if(mv->debug == 6) printf("opencp_meb() cp->jky=%d\n",cp->jky);
    }
    if(cp->key == 2) {
        while(fgets(irule[cp->iky], 82, fcp)) { /* minor */ 
            /* if(mv->debug == 6) printf("%s",irule[cp->iky]); */
            cp->iky++;
            if(cp->iky > MAXRL) break;
        }
        if(mv->debug == 6) printf("opencp_meb() cp->iky=%d\n",cp->iky);
    }
    fclose(fcp);
    return(0);
}

int setlv3_cps(char skey[MAXLVL])
//char skey[MAXLVL];
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
    if(mv->debug == 6) {
        printf("setlv3_cps() skey=%s ",skey);
        if(cp->key == 1) printf("cp->jky=%d\n",cp->jky);
        if(cp->key == 2) printf("cp->iky=%d\n",cp->iky);
    }
    k = 0;
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]); 
        if(cp->key == 2) strcpy(line,irule[k]); 
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        ef = 0;
        sf = 0;
        if(line[0] == '$' && line[1] == '3' && line[2] == ':') sf = 1;
        if(sf) {
            /* printf("%s",line); */
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
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) cp->n[3] = 0;
                    strcpy(cp->mk[3],skey); /* set level 3 marker */
                    setval_cps(line,3);   /* set variable level 3 */
                    ac++;
                }
            }
        }
    }
    return(0);
}

int setlv4_cps(char skey[MAXLVL])
//char skey[MAXLVL];
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
    if(mv->debug == 6) {
        printf("setlv4_cps() skey=%s ",skey);
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
        if(line[0] == '$' && line[1] == '4' && line[2] == ':') sf = 1;
        if(sf) {
            /* printf("%s",line); */
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
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) cp->n[4] = 0;
                    strcpy(cp->mk[4],skey); /* set level 4 marker */
                    setval_cps(line,4);   /* set variable level 4 */
                    ac++;
                }
            }
        }
    }
    return(0);
}

int setlv5_cps(char skey[MAXLVL])
//char skey[MAXLVL];
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
    if(mv->debug == 6) {
        printf("setlv5_cps() skey=%s ",skey);
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
        if(line[0] == '$' && line[1] == '5' && line[2] == ':') sf = 1;
        if(sf) {
            /* printf("%s",line); */
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
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(ac == 0) cp->n[5] = 0;
                    strcpy(cp->mk[5],skey); /* set level 5 marker */
                    setval_cps(line,5);   /* set variable level 5 */
                    ac++;
                }
            }
        }
    }
    return(0);
}

/* ? use chord select $6: */
int setlv6_cps(char skey[MAXLVL])
//char skey[MAXLVL];
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
    if(mv->debug == 8 || mv->debug == 6) {
        printf("Debug8! setlv6_cps() Orfn=%s skey=%s ",Orfn,skey);
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
        if(line[0] == '$' && line[1] == '6' && line[2] == ':') sf = 1;
        if(sf) {
            /* printf("%s",line); */
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
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    /* if(ac == 0) cp->n[5] = 0; */
                    if(ac == 0) cs->n = 0;
                    strcpy(cs->mk,skey);    /* set level 6 marker */
                    setval6_cps(line);      /* set variable level 6 */
                    ac++;
                }
            }
        }
    }
    return(0);
}

/* strcat limit len 80 */
int strcat_cps(char card[82],char wk[82])
//char card[82];
//char wk[82];
{
    int  tlen;
    char temp[180];

    temp[0] = '\0';
    strcat(temp,card);
    strcat(temp,wk);
    tlen = strlen(temp);
    /*
    printf("card = %s wk = %s\n",card,wk);
    printf("%s tlen = %d\n",temp,tlen);
    */
    if(tlen > 78) temp[78] = '\0';
    strcpy(card,temp);
    return(0);
}

/* get avil interval from Root in C */
int getival_meb(int kp,char Note[MAXLVR])
//int  kp;
//char Note[MAXLVR];
{  
    int  i,j,k;
    int  keyp;
    
    keyp = mv->nktb[kp];
    if(Note[kp] == '&') keyp = keyp - 1;
    if(Note[kp] == '@') keyp = keyp - 2;
    if(Note[kp] == '#') keyp = keyp + 1;
    if(Note[kp] == '*') keyp = keyp + 2;
    /* printf("keyp = %d kp = %d\n",keyp,kp); */
    return(keyp);
}  

/* Base Chord check */
int bgchk_meb(char bc[16],char bk[16])
//char bc[16];
//char bk[16];
{
    int  i,j;
    int  rt;

    i = 0;
    i = 0; 
    if(cp->bg[0] == '&' || cp->bg[0] == '#') {
        bc[0] = cp->bg[0];
        i = 1;
    }   
    while(1) {
        if(cp->bg[i] == '\0') break;  
        if(cp->bg[i] != 'I' && cp->bg[i] != 'V') break;
        bc[i] = cp->bg[i];
        i++; 
        if(i > 16) break;
    }   
    bc[i] = '\0';
    j = 0;
    while(1) {
        if(cp->bg[i] == '\0') break;  
        bk[j] = cp->bg[i];
        i++; 
        if(i > 16) break;
        j++;
    }
    bk[j] = '\0';
    rt = strlen(bk);
    if(mv->debug == 6) printf("bgchk_() cp->bg = %s bc = %s bk = %s rt = %d\n",
        cp->bg,bc,bk,rt);
    return(rt);
}

/* setlv A Available note scale */
int setlvA_cps(char skey[MAXLVL])
//char skey[MAXLVL];
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
    ma->n = 0;
    if(mv->debug == 6) {
        printf("setlvA_() skey = %s\n",skey);
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
        if(line[0] == '$' && line[1] == 'A' && line[2] == ':') sf = 1;
        if(sf) {
            /* printf("%s",line); */
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
                af = linechk_cps(line,skey);
                /* printf("af = %d\n",af); */
            }
            else {
                if(af) {
                    if(af == 0) ma->n = 0;
                    setavals_cps(line);    /* set avariable note scale */
                    ac++;
                }
            }
        }
    }
    return(0);
}

/* set avariable note scale value */
int setavals_cps(char line[82])
//char line[82];
{
    int  i,j,k;
    int  af;
    char sl[MAXLVL];
    char sm[MAXLVR];
    char sr[MAXLVR];
    char so[MAXLVR];
    
    /* printf("setavals_() line = %s",line); */
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
    if(mv->debug == 6) {
        printf("ma->n=%d %s %s %s\n",
            ma->n,ma->l[ma->n],ma->m[ma->n],ma->r[ma->n]);
    }   
    ma->n++; 
    if(ma->n > MAXLVA) {
        mv->errcnt++;
        mv->msglvl = 3;
        if(mv->msglvl >= mv->prtlvl) {
            printf("Error! rule Level %d argument over than %d\n",
            ma->n,MAXLVA);
        }
        ma->n--;
    }
    return(0);
}

/* Set next chord progression char to cp->npcd[][] */
int setnpcd_meb(char wk[82])
//char wk[82];
{
    int  i,j,k;
    int  pf;
    int  len;
    char wc[80];

    if(mv->debug == 6) printf("setnpcd_() wk = %s\n",wk);
    for(i = 0; i < 10; i++) cp->npcd[i][0] = '\0';
    i = 1;
    pf = 0;
    while(1) {
        if(wk[i] == '\0') break;
        if(wk[i] == '-' && wk[i+1] == '>') {
            pf = 1;
            break;
        }
        i++;
        if(i > 80) break;
    }
    if(pf == 0) return(1);
    len = strlen(wk);
    i = i + 2;
    for(j = 0; i < len; i++,j++) wc[j] = wk[i];  
    wc[j] = '\0';
    /* printf("wc = %s\n",wc); */
    i = 0;
    if(wc[0] == ' ') i = 1;
    j = 0;
    k = 0;
    while(1) {
        if(wc[i] == '\0') {
            cp->npcd[j][k] = '\0';
            if(mv->debug == 6)
                printf("cp->npcd[%d] = %s\n",j,cp->npcd[j]);
            break;
        }
        if(wc[i] == ',' || wc[i] == ' ') {
            cp->npcd[j][k] = '\0';
            if(mv->debug == 6)
                printf("cp->npcd[%d] = %s\n",j,cp->npcd[j]);
            k = 0;
            j++;
            if(j > 10) break; 
        }
        else {
            cp->npcd[j][k] = wc[i];
            k++;
            if(k > MAXLVL) break;
        }
        i++;
        if(i > 80) break;
    }
    cp->nnc = j;  /* Set number of next chord */
    if(mv->debug == 6) printf("setnpcd_() OK %s cp->nnc = %d\n",
        wc,cp->nnc);
    return(0);
}

/* Change chode progression */
int chprgres_meb()
{
    int  i;
    int  pf;
    int  len;

    if(mv->debug == 6) printf("chprgres_() cp->npc = %d\n",cp->npc);
    pf = 0;
    if(cp->npc) {
        for(i = cp->npc; i >= 0; i--) {
            if(cp->npcd[i][0] != '\0') {
                mv->card[2] = '\0';
                strcat(mv->card,cp->npcd[i]);
                /* printf("mv->card = %s %s\n",mv->card,cp->npcd[i]); */
                len = strlen(mv->card);
                mv->card[len] = '}';
                if(mv->card[0] == '{') mv->card[len] = '}';
                if(mv->card[0] == '(') mv->card[len] = ')';
                mv->card[len+1] = '\0';
                pf = 1;
                break;
            }
        }
    }
    else {
        if(cp->npcd[0][0] != '\0') {
            mv->card[2] = '\0';
            strcat(mv->card,cp->npcd[0]);
            len = strlen(mv->card);
            mv->card[len] = '}';
            if(mv->card[0] == '{') mv->card[len] = '}';
            if(mv->card[0] == '(') mv->card[len] = ')';
            mv->card[len+1] = '\0';
            pf = 1;
        } 
    }
    if(mv->debug == 6) printf("chprgres_() %s (check)\n",mv->card);
    return(0);
}

/* change chord progression serch */
int chgserch_cps(char skey[MAXLVL])
//char skey[MAXLVL];
{
    int  i,j,k;
    int  lv;
    int  pf;
    char wk[82];
    int  sf;
    char bc[16];
    char bk[16];
    char wkey[MAXLVL];
    int  rtn;

    lv = 2;
    rtn = 1;
    for(k = 0; k < cp->n[lv]; k++) {
        if(mv->debug == 6) printf("chgserch cp->l[%d][%d] = %s skey = %s\n",
            lv,k,cp->l[lv][k],skey);
        if(!strcmp(cp->l[lv][k],skey)) {
            wk[0] = '\0';
            strcat(wk,"!");
            strcat_cps(wk,cp->r[lv][k]);
            /* printf("wk = %s\n",wk); */
            i = 0;
            pf = 0;
            while(1) {
                if(wk[i] == '\0') break;
                if(wk[i] == '\r') break;
                if(wk[i] == '\n') break;
                if(wk[i] == '$') pf++;
                i++;  
                if(i > 80) break;
            }
            basechk_meb(skey,wkey);
            if(pf) rtn = cpserch2_cps(skey,wk);
            if(!rtn) strcpy(mv->card,wk);
            break;
        } 
    }
    sf = bgchk_meb(bc,bk); /* Base Chord check */
    cp->pol = 0;
    if(sf) {
        cp->pol = 1;
        basechk_meb(skey,wkey);
        sf = Poly_key(skey,wkey,bc,bk);
        return(sf);
    }
    if(rtn) {
        cpserch3_cps(skey,wkey);    
        if(skey[0] == 'I') sf = 2;
        if(skey[0] == 'V') sf = 3;
        if(skey[1] == 'I') sf = 2;
        if(skey[1] == 'V') sf = 3;
        if(mv->debug == 6) printf("sf = %d rtn = %d\n",sf,rtn);
        if(sf && rtn) {
            sf = Special_key(wk,skey,wkey);
        }
        if(sf) {
            i = 0;
            while(1) {
                if(mv->card[i] == ' ') break;
                if(mv->card[i] == ',') break;
                if(mv->card[i] == '\r') break;
                if(mv->card[i] == '\n') break;
                if(mv->card[i] == '\0') break;
                i++;
                if(i > 80) break;
            }
            mv->card[i] = '\0';
            mv->wrncnt++;
            mv->msglvl = 2;
            if(mv->msglvl >= mv->prtlvl)
            printf("Worning! %s is not found in Rule file\n",skey);
            return(-1);
        }
    }
    return(0);
}

int basechk_meb(char skey[MAXLVL],char wkey[MAXLVL])
//char skey[MAXLVL];
//char wkey[MAXLVL];
{
    int  i,j,k;
    int  sf;
    int  bf;
    int  mf;

    i = 0; 
    bf = 0;
    while(1) {
        if(skey[i] == '/') {
            bf = 1;
            break; 
        }
        if(skey[i] == '\0') break;  
        wkey[i] = skey[i]; 
        i++;
        if(i >= MAXLVL) break;
    }   
    wkey[i] = '\0';
    if(bf) {
        cp->bf = 1;
        j = 0;
        i++;
        while(1) {
            if(skey[i] == ' ') break;
            if(skey[i] == '~') break;
            cp->bg[j] = skey[i];
            i++;
            if(i >= MAXLVL) break;
            j++;
            if(j >= 7) break;
        }
        /* j++; */
        cp->bg[j] = '\0';
    }
    else {
        cp->bf = 0; 
        cp->bg[0] = '\0';
    }
    if(mv->debug == 6) printf("basechk() skey = %s cp->bf = %d cp->bg = %s\n",
        skey,cp->bf,cp->bg);
    return(0);
}

int hlponly_cps(char skey[MAXLVL])
//char skey[MAXLVL];
{
    int  i,j,k;
    char line[82];
    char wk[82];
    int sf;

    k = 0;
    if(mv->debug == 6) printf("Debug6! hlponly_cps() sk=%s\n",skey);
    while(1) {
        if(cp->key == 1) strcpy(line,jrule[k]);
        if(cp->key == 2) strcpy(line,irule[k]);
        k++;
        if(cp->key == 1 && k > cp->jky) break;
        if(cp->key == 2 && k > cp->iky) break;
        if(k > MAXRL) break;
        sf = 0;
        for(i = 1 ; i < 80; i++) {
            if(line[i] == '\0') break;
            if(line[i] == '=') sf = 1;
        }
        if(line[0] == '!' && sf == 0) {
            /* printf("sf = %d %s",sf,line); */
            i = 1;
            j = 0;
            while(1) {
                wk[j] = line[i];
                if(line[i] == ' ') break;
                i++;
                if(i > 80) break;
                j++;
            }
            wk[j] = '\0';
            /* printf("skey = %s wk = %s\n",skey,wk); */
            if(!strcmp(wk,skey)) {
                for(i = 0; i < 80; i++) {
                    if(line[i] == '\n') break;
                    if(line[i] == '\0') break;
                    if(line[i] == '\r') break;
                    mv->card[i] = line[i];
                }
                mv->card[i] ='\0';
                return(1); /* hlp on */
            }
        }
    }
    return(0);
}

int serial_meb(char card[82])
//char card[82];
{
    int  i;
    char work[20];

    if(card[3] == '$' && card[78] == '$') {
        /* snum = (int)Snum1 & (int)Snum2; */
        sprintf(work,"%s%s%s%s$$",Sname1,Sname2,Snum1,Snum2);
        for(i = 0; i < 20; i++) card[60+i] = work[i];
        /* printf(" %s\n",work); */
    }
    return(0);
}

