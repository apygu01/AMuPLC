
// ----------------------
//  AMuPLC (mlk1.c)
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

#include "mlk.h"
extern MLKSRG *ls;
extern MLKVAR *lv;
extern MLKFRS *lr;
extern MLKCDE *lc;
/* data check */
int datachk_mlk()
{
    switch (lc->c[0]) {
        case 0xff : ffdecode_mlk();
                    break;
        case 0xf0 : f0decode_mlk();
                    break;
        case 0xf1 : f1decode_mlk();
                    break;
        case 0xb1 : b1decode_mlk();
                    break;
        case 0xf2 : f2decode_mlk();
                    break;
        case 0xf3 : f3decode_mlk();
                    break;
        case 0xf4 : f4decode_mlk();
                    break;
        case 0xf5 : f5decode_mlk();
                    break;
        case 0xf7 : f7decode_mlk();
                    break;
        case 0xf8 : f8decode_mlk();
                    break;
        case 0xf9 : f9decode_mlk();
                    break;
        case 0xfa : fadecode_mlk();
                    break;
        case 0xfb : fbdecode_mlk();
                    break;
        case 0xfc : fcdecode_mlk();
                    break;
        case 0xfd : fddecode_mlk();
                    break;
        case 0xfe : fedecode_mlk();
                    break;
        case 0xdd : dddecode_mlk();
                    break;
        case 0xe1 : e1decode_mlk();
                    break;
        case 0xe2 : e2decode_mlk();
                    break;
        case 0xe3 : e3decode_mlk();
                    break;
        case 0xe4 : e4decode_mlk();
                    break;
        default   : break;
    }
    return(0);
}

/*   FF code ....................................
Header       : FFvwxxyy
 v(gate)     : gate number
 w(chan)     : channel number
 xx(trak)    : track (0-35)
 yy(memb)    : member
*/
int ffdecode_mlk()
{
    int  i;
    int  pfg;

    lv->gate = (lc->s[0] & 0x00f0) >> 4;
    lv->chan = lc->s[0] & 0x000f;
    lv->trak = (int)lc->c[2];
    lv->memb = (int)lc->c[3];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf(
        "FF(%02x%02x%02x%02x) Header / gate=%d chan=%d trak=%d memb=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],
        lv->gate,lv->chan,lv->trak,lv->memb);
    }
    return(0);
}

/*   F0 code ....................................
File control : F0xxyyyy
xx(f0typ)    : file type(staff type)
yyyy(f0num)  : file number(000 - z99)  max = z * 100 + 99 = z99
 ex. Activ conductor staff    xx = C 000 yyyy =    0 (xxx_000.meg)
     exsist conductor stff    xx = c 200 yyyy =  200 (xxx_200.meg)
     exsist midlle staff      xx = m 101 yyyy =  101 ( xxx : file name )
     Active synthesizer staff xx = S A02 yyyy =  1002
*/
int f0decode_mlk()
{
    int  i;
    int  pfg;

    lv->f0typ = lc->c[1] & 0xff;
    lv->f0num = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F0(%02x%02x%02x%02x) File control / f0typ='%c' f0num=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->f0typ,lv->f0num);
    }
    return(0);
}

/*   F1 code ....................................
SMPTE time code : F1xxxxxx
 xxxxxx(smpte)  : time code (hour * minites * seconds * frame)
 max = 24h * 60m * 60s * 30f = 2592000 + 108000 + 1800 + 30 = 2701830
 max = 108000 * hour + 1800 * minites + 30 * seconds + frame
*/
int f1decode_mlk()
{
    union {
        short wi[2];
        long wl;
    } w;
    int  i;
    int  pfg;
    int  ffl;
    long smptf;

    ffl = lc->c[1] & 0x80;
    if(ffl) {
        if(ls->comptyp == 0) {
            w.wi[1] = lc->s[0] & 0x000f;
            w.wi[0] = lc->s[1];
        }
        if(ls->comptyp == 1) {
            w.wi[0] = lc->s[0] & 0x000f;
            w.wi[1] = lc->s[1];
        }
        if(ls->comptyp == 2) {
            w.wi[1] = lc->s[0] & 0x000f;
            w.wi[0] = lc->s[1];
        }
        smptf = w.wl;
        lv->smpte = lv->smpte + smptf; /* add flactional flame */
        lv->tctyp = lc->s[0] & 0x0070;
        lv->tctyp = lv->tctyp >> 4;
        if(lv->tctyp == 0) lv->sflame = 24;
        if(lv->tctyp == 1) lv->sflame = 25;
        if(lv->tctyp == 2) lv->sflame = 30;
        if(lv->tctyp == 3) lv->sflame = 30;
    }
    else {
        if(ls->comptyp == 0) {
            w.wi[1] = lc->s[0] & 0x00ff;
            w.wi[0] = lc->s[1];
        }
        if(ls->comptyp == 1) {
            w.wi[0] = lc->s[0] & 0x00ff;
            w.wi[1] = lc->s[1];
        }
        if(ls->comptyp == 2) {
            w.wi[1] = lc->s[0] & 0x00ff;
            w.wi[0] = lc->s[1];
        }
        lv->smpte = w.wl * 100;
    }
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F1(%02x%02x%02x%02x) SMPTE time code / smpte=%ld sflame=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->smpte,lv->sflame);
    }
    return(0);
}

/*   B1 code ....................................
SMPTE full time code : B1xxxxxx
 xxxxxx(smpte)  : time code (hour * minites * seconds * frame)
 max = 24h * 60m * 60s * 30f = 2592000 + 108000 + 1800 + 30 = 2701830
 max = 108000 * hour + 1800 * minites + 30 * seconds + frame
*/
int b1decode_mlk()
{
    union {
        short wi[2];
        long wl;
    } w;
    int  i;
    int  pfg;
    int  ffl;
    long smptf;

    ffl = lc->c[1] & 0x80;
    if(ffl) {
        if(ls->comptyp == 0) {
            w.wi[1] = lc->s[0] & 0x000f;
            w.wi[0] = lc->s[1];
        }
        if(ls->comptyp == 1) {
            w.wi[0] = lc->s[0] & 0x000f;
            w.wi[1] = lc->s[1];
        }
        if(ls->comptyp == 2) {
            w.wi[1] = lc->s[0] & 0x000f;
            w.wi[0] = lc->s[1];
        }
        smptf = w.wl;
        lv->smpte = lv->smpte + smptf;  /* add flactional flame */
        lv->tctyp = lc->s[0] & 0x0070;
        lv->tctyp = lv->tctyp >> 4;
        if(lv->tctyp == 0) lv->sflame = 24;
        if(lv->tctyp == 1) lv->sflame = 25;
        if(lv->tctyp == 2) lv->sflame = 30;
        if(lv->tctyp == 3) lv->sflame = 30;
    }
    else {
        if(ls->comptyp == 0) {
            w.wi[1] = lc->s[0] & 0x00ff;
            w.wi[0] = lc->s[1];
        }
        if(ls->comptyp == 1) {
            w.wi[0] = lc->s[0] & 0x00ff;
            w.wi[1] = lc->s[1];
        }
        if(ls->comptyp == 2) {
            w.wi[1] = lc->s[0] & 0x00ff;
            w.wi[0] = lc->s[1];
        }
        lv->smpte = w.wl * 100;
    }
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("B1(%02x%02x%02x%02x) SMPTE time code / smpte=%ld sflame=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->smpte,lv->sflame);
    }
    return(0);
}

/*   F2 code ....................................
Master velocity(dynamic) : F2xxyyyy
xx          : reserve
yyyy(mvelo) : velocity(dynamic)
 ex. 68.2% = 6820
*/
int f2decode_mlk()
{
    int  i;
    int  pfg;

    lv->mvelo = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F2(%02x%02x%02x%02x) Master velocity / mvelo=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->mvelo);
    }
    return(0);
}

/* F3 decode ....................................
File control : F3xxyyyy
xx         : channel
yyyy       : gate(port) number(000 - 999)
 This is used by after F0 code
*/
int f3decode_mlk()
{
    int  channel;
    int  gate;
    int  i;
    int  pfg;

    channel = lc->i[0];
    gate = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F3(%02x%02x%02x%02x) File control / f3channel=%d f3gate=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],channel,gate);
    }
    return(0);
}

/* F4 decode ....................................
SMPTE time lag : F4yyyyyy
yyyyyy     : Smpte time lag (flame * 100)
*/
int f4decode_mlk()
{
    int  i;
    int  pfg;

    lv->stlag = lc->l;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F4(%02x%02x%02x%02x) Smpte time lag / lag=%ld\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->stlag);
    }
    return(0);
}

/*   F5 code ....................................
Code data     : F5xyyyyy
  x(kind)     : kind (note or rest)
  yyyyy(cval) : code value %60 _/ = 100000 * 60 / 100 :  %
 ex. %60 _/.[3  wwwww(cval) = (150000 * 60 / 100) / 3
                            = 90000 / 3
                            = 30000
                 x(kind) = 1
                 (rest = 0, note = 1, tril = 2, turn = 3, mordent = 4)
                 (double mordent = 5, double tril = 6)
*/
int f5decode_mlk()
{
    union {
        short wi[2];
        long wl;
    } w;
    int  i;
    int  pfg;

    if(ls->comptyp == 0) {
        lv->kind = lc->c[1] & 0xf0;
        lv->kind = lv->kind >> 4;
        w.wi[1] = lc->i[0] & 0x000f;
        w.wi[0] = lc->i[1];
    }
    if(ls->comptyp == 1) {
        lv->kind = lc->c[1] & 0xf0;
        lv->kind = lv->kind >> 4;
        w.wi[0] = lc->i[0] & 0x000f;
        w.wi[1] = lc->i[1];
    }
    if(ls->comptyp == 2) {
        lv->kind = lc->c[1] & 0xf0;
        lv->kind = lv->kind >> 4;
        w.wi[1] = lc->i[0] & 0x000f;
        w.wi[0] = lc->i[1];
    }
    lv->cval = w.wl;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(!pfg) return(0);
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F5(%02x%02x%02x%02x) Code data / kind=%d cval=%ld\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->kind,lv->cval);
    }
    return(0);
}

/*   F6 code ....................................
Midi control : F6vvwwxx
 vv(mist)    : midi status data  vv=00(cont),vv=01(1ch),vv=02(fin)
 ww(mifd)    : midi first data
 xx(misd)    : midi second data
 ex. <01,33,54,4a,2e> F6000133 f600544a f6012e00
*/
int f6decode_mlk()
{
    int  i;
    int  pfg;

    lv->mist = (int)lc->c[1];
    lv->mifd = (int)lc->c[2];
    lv->misd = (int)lc->c[3];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F6(%02x%02x%02x%02x) MIDI control / mist=%d mifd=%d misd=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->mist,lv->mifd,lv->misd);
    }
    return(0);
}

/*   F7 code ....................................
Instruments  : F7vvxxxx
 vv(trns)    : transpose
 xxxx(inst)  : instrument no.
 ex. [vn]   3    13   1   -3 , chanel 13 inst = 1
                mv->trns = -3 (mr->trns &= 0x00ff)
*/
int f7decode_mlk()
{
    int  i;
    int  pfg;

    lv->trns = (int)lc->c[1];
    lv->inst = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F7(%02x%02x%02x%02x) Instruments/ trns=%d inst=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->trns,lv->inst);
    }
    return(0);
}

/*   F8 code ....................................
Key and time : F8wxyyzz
 w(kscl)     : moll or dur  (none=0,dur=1,moll=2,major=3,minor=4)
 x(ksin)     : key signeture
 yy(ritm)    : rithm
 zz(time)    : time base
 ex. $###3/4 x_moll
         w(kscl) = 1
         x(ksin) = 3
         yy(ritm) = 3
         zz(time) = 4
*/
int f8decode_mlk()
{
    int  i;
    int  pfg;

    lv->kscl = lc->c[1] & 0xf0;
    lv->kscl = lv->kscl >> 4;
    lv->ksin = lc->c[1] & 0x0f;
    lv->ritm = lc->c[2] & 0xff;
    lv->time = lc->c[3] & 0xff;
    if(lv->time == 0) lv->gcval = 0;
    else lv->gcval = (400000/lv->time) * lv->ritm;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf(
        "F8(%02x%02x%02x%02x) Key & time / kscl=%d ksin=%d ritm=%d time=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],
        lv->kscl,lv->ksin,lv->ritm,lv->time);
        /* printf("gcval=%ld\n",lv->gcval); */
    }
    return(0);
}

/*   F9 code ....................................
Expression   : F9xxyyyy
 xx(egrp)    : expression group
 yyyy(expr)  : expression number
 ex. [amore]   xx = 1 yyyy = ????,   exp. 234  xx = 77 yyyy = 234
*/
int f9decode_mlk()
{
    int  i;
    int  pfg;

    lv->egrp = lc->c[1] & 0xff;
    lv->expr = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("F9(%02x%02x%02x%02x) Expression / egrp=%d expr=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->egrp,lv->expr);
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
int fadecode_mlk()
{
    int  i;
    int  pfg;

    lv->tsym = lc->i[0];
    lv->temp = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                /* if(u->wc[1] == ls->dckcd[i]) pfg = 1;
                printf("## %02x %02x\n",lc->c[0],ls->dckcd[i]); */
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("FA(%02x%02x%02x%02x) Tempo / tsym=%d temp=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->tsym,lv->temp);
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
int fbdecode_mlk()
{
    int  i;
    int  pfg;

    lv->movn = lc->i[0];
    lv->bnum = lc->i[1];
    lv->meas = lv->movn * 100000 + lv->bnum;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf(
        "FB(%02x%02x%02x%02x) Bar data / movn=%d bnum=%d meas=%ld\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],
        lv->movn,lv->bnum,lv->meas);
    }
    return(0);
}

mfbdecode_mlk()
{
    int  i;
    int  pfg;

    lv->mmovn = lc->i[0];
    lv->mbnum = lc->i[1];
    lv->mmeas = lv->mmovn * 100000 + lv->mbnum;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf(
        "FB(%02x%02x%02x%02x) Bar data / mmovn=%d mbnum=%d mmeas=%ld\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],
        lv->mmovn,lv->mbnum,lv->mmeas);
    }
    return(0);
}

/*   FC code ....................................
Synchronize  : FCxxxxxx
 xxxxxx(sync): synchronize
 ex. $4/4 = 400000 start value
*/
fcdecode_mlk()
{
    int  i;
    int  pfg;

    lv->sync = lc->l;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("FC(%02x%02x%02x%02x) Synchronize / sync=%ld\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->sync);
    }
    return(0);
}
mfcdecode_mlk()
{
    int  i;
    int  pfg;

    lv->msync = lc->l;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("FC(%02x%02x%02x%02x) Synchronize / msync=%ld\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->msync);
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

Pol.key.p.    : FDAxyyzz
 x(chan)      : channel no.
 yy(keyp)     : key position
 zz(kprs)     : key pressure

Cont.change   : FDBxyyzz
 x(chan)      : channel no.
 yy(cntn)     : control no.
 zz(cntv)     : control value

Prog.change   : FDCxyyzz
 x(chan)      : channel no.
 yy(prgn)     : program number
 zz(prgv)     : program change val (??)

Chan.pres.    : FDDxyyzz (after tatch)
 x(chan)      : channel no.
 yy(ptyp)     : pressure type(vib. etc.)
 zz(prsv)     : pressure value
 ex. 4<  3>  2v

Pitch, vend.   : FDExyyzz
 x(chan)      : channel no.
 yy(pvls)     : vvvvvvv LSB val.
 zz(pvms)     : uuuuuuuu MSB val.
*/
fddecode_mlk()
{
    int  i;
    int  pfg;

    lv->fdtyp = lc->c[1] & 0xf0;
    lv->fdtyp = lv->fdtyp >> 4;
    lv->fdch = lc->c[1] & 0x0f;
    switch (lv->fdtyp) {
        case  8 : lv->keyp = lc->c[2] & 0xff;
                  lv->voff = lc->c[3] & 0xff;
                  if(ls->dchk || ls->trace) {
                      if(ls->dckn) {
                          pfg = 0;
                          for(i = 0; i < ls->dckn; i++) {
                             if(lc->c[0] == ls->dckcd[i]) pfg = 1;
                          }
                          if(!pfg) break;
                      }
                      if(ls->trace == 2) printf("             ");
                      if(ls->trace == 3) printf("                      ");
                      printf("FD(%02x%02x%02x%02x) Note off / fdtyp=%d fdch=%d",
                      lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->fdtyp,lv->fdch);
                      printf(" keyp=%d voff=%d\n",lv->keyp,lv->voff);
                  }
                  break;
        case  9 : lv->keyp = lc->c[2] & 0xff;
                  lv->veon = lc->c[3] & 0xff;
                  if(ls->dchk || ls->trace) {
                      if(ls->dckn) {
                          pfg = 0;
                          for(i = 0; i < ls->dckn; i++) {
                             if(lc->c[0] == ls->dckcd[i]) pfg = 1;
                          }
                          if(!pfg) break;
                      }
                      if(ls->trace == 2) printf("             ");
                      if(ls->trace == 3) printf("                      ");
                      printf("FD(%02x%02x%02x%02x) Note on / fdtyp=%d fdch=%d",
                      lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->fdtyp,lv->fdch);
                      printf(" keyp=%d veon=%d\n",lv->keyp,lv->veon);
                  }
                  break;
        case 10 : lv->keyp = lc->c[2] & 0xff;
                  lv->kprs = lc->c[3] & 0xff;
                  if(ls->dchk || ls->trace) {
                      if(ls->dckn) {
                          pfg = 0;
                          for(i = 0; i < ls->dckn; i++) {
                             if(lc->c[0] == ls->dckcd[i]) pfg = 1;
                          }
                          if(!pfg) break;
                      }
                      if(ls->trace == 2) printf("             ");
                      if(ls->trace == 3) printf("                      ");
                      printf("FD(%02x%02x%02x%02x) Poliphonic key pressure /",
                      lc->c[0],lc->c[1],lc->c[2],lc->c[3]);
                      printf(" fdtyp=%d fdch=%d keyp=%d veon=%d\n",
                          lv->fdtyp,lv->fdch,lv->keyp,lv->kprs);
                  }
                  break;
        case 11 : lv->cntn = lc->c[2] & 0xff;
                  lv->cntv = lc->c[3] & 0xff;
                  if(ls->dchk || ls->trace) {
                      if(ls->dckn) {
                          pfg = 0;
                          for(i = 0; i < ls->dckn; i++) {
                             if(lc->c[0] == ls->dckcd[i]) pfg = 1;
                          }
                          if(!pfg) break;
                      }
                      if(ls->trace == 2) printf("             ");
                      if(ls->trace == 3) printf("                      ");
                      printf("FD(%02x%02x%02x%02x) Cont. change /",
                      lc->c[0],lc->c[1],lc->c[2],lc->c[3]);
                      printf(" fdtyp=%d fdch=%d cntn=%d cntv=%d\n",
                          lv->fdtyp,lv->fdch,lv->cntn,lv->cntv);
                  }
                  break;
        case 12 : lv->prgn = lc->c[2] & 0xff;
                  lv->prgv = lc->c[3] & 0xff;
                  if(ls->dchk || ls->trace) {
                      if(ls->dckn) {
                          pfg = 0;
                          for(i = 0; i < ls->dckn; i++) {
                             if(lc->c[0] == ls->dckcd[i]) pfg = 1;
                          }
                          if(!pfg) break;
                      }
                      if(ls->trace == 2) printf("             ");
                      if(ls->trace == 3) printf("                      ");
                      printf("FD(%02x%02x%02x%02x) Prog. change /",
                      lc->c[0],lc->c[1],lc->c[2],lc->c[3]);
                      printf(" fdtyp=%d fdch=%d prgn=%d prgv=%d\n",
                          lv->fdtyp,lv->fdch,lv->prgn,lv->prgv);
                  }
                  break;
        case 13 : lv->ptyp = lc->c[2] & 0xff;
                  lv->prsv = lc->c[3] & 0xff;
                  if(ls->dchk || ls->trace) {
                      if(ls->dckn) {
                          pfg = 0;
                          for(i = 0; i < ls->dckn; i++) {
                             if(lc->c[0] == ls->dckcd[i]) pfg = 1;
                          }
                          if(!pfg) break;
                      }
                      if(ls->trace == 2) printf("             ");
                      if(ls->trace == 3) printf("                      ");
                      printf("FD(%02x%02x%02x%02x) Channel pressure /",
                      lc->c[0],lc->c[1],lc->c[2],lc->c[3]);
                      printf(" fdtyp=%d fdch=%d ptyp=%d prsv=%d\n",
                          lv->fdtyp,lv->fdch,lv->ptyp,lv->prsv);
                  }
                  break;
        case 14 : lv->pvls = lc->c[2] & 0xff;
                  lv->pvms = lc->c[3] & 0xff;
                  if(ls->dchk || ls->trace) {
                      if(ls->dckn) {
                          pfg = 0;
                          for(i = 0; i < ls->dckn; i++) {
                             if(lc->c[0] == ls->dckcd[i]) pfg = 1;
                          }
                          if(!pfg) break;
                      }
                      if(ls->trace == 2) printf("             ");
                      if(ls->trace == 3) printf("                      ");
                      printf("FD(%02x%02x%02x%02x) Pitch vend /",
                      lc->c[0],lc->c[1],lc->c[2],lc->c[3]);
                      printf(" fdtyp=%d fdch=%d pvls=%d pvms=%d\n",
                          lv->fdtyp,lv->fdch,lv->pvls,lv->pvms);
                  }
                  break;
        default : break;
    }
    return(0);
}

/*   FE code ....................................
Final         : FEvvvvvv
 vvvvvv(tcnt) : total output code count
*/
fedecode_mlk()
{
    int  i;
    int  pfg;

    lv->tcnt = lc->l;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("FE(%02x%02x%02x%02x) Final / tcnt=%ld\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->tcnt);
    }
    return(0);
}

/*   DD code ....................................
File          : DDxxyyzz
 xx(port)     : port
 yy(chan)     : channel
 zz(memb)     : member
*/
int dddecode_mlk()
{
    int  i;
    int  pfg;

    lv->port = lc->c[1] & 0xff;
    lv->trak = lc->c[2] & 0xff;
    lv->memb = lc->c[3] & 0xff;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("DD(%02x%02x%02x%02x) File / port=%d chan=%d memb=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->port,lv->trak,lv->memb);
    }
    return(0);
}

/*
Phrase data 1   : E1xyzzzz
 x(ftype)      : reserve(phrase type)
 y(fchan)      : channel
 zzzz(fsval)   : phrase start value
*/
int e1decode_mlk()
{
    int  i;
    int  pfg;
    int  p;
    int  f;

    p = lv->port;                      /* port */
    if(p >= MAXPT) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
        printf("Error! port number(%d) greater than MAXPT(%d)\n",p,MAXPT);
        p = 0;
    }
    lr->ftype[p] = 0;
    lr->ftype[p] = (lc->c[1] & 0xf0) >> 4;
    f = lr->ftype[p];                     /* flame type */
    lr->fchan[p][f] = lc->c[1] & 0x0f;
    lr->fsval[p][f] = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("E1(%02x%02x%02x%02x) Phrase E1 / ftype[%d]=%d",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],p,lr->ftype[p]);
        printf(" fchan[%d][%d]=%d fsval[%d][%d]=%d\n",
            p,f,lr->fchan[p][f],p,f,lr->fsval[p][f]);
    }
    return(0);
}

/*
Phrase data 2   : E2xxyyyy
 xx(fctno)     : phrase control number
 yyyy(feval)   : phrase end value
*/
int e2decode_mlk()
{
    int  i;
    int  pfg;
    int  p;
    int  f;

    p = lv->port;                      /* port */
    if(p >= MAXPT) p = 0;              /* error */
    f = lr->ftype[p];
    lr->fctno[p][f] = lc->c[1] & 0xff;
    lr->feval[p][f] = lc->i[1];
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("E2(%02x%02x%02x%02x) Phrase E2 / ftype[%d]=%d",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],p,lr->ftype[p]);
        printf(" fctno[%d][%d]=%d feval[%d][%d]=%d\n",
            p,f,lr->fctno[p][f],p,f,lr->feval[p][f]);
    }
    return(0);
}

/*
Phrase data 3   : E3xxxxxx
 xxxxxx(fnval) : phrase note value
*/
int e3decode_mlk()
{
    int  i;
    int  pfg;
    int  p;
    int  f;

    p = lv->port;                      /* port */
    if(p >= MAXPT) p = 0;              /* error */
    f = lr->ftype[p];
    lr->fnval[p][f] = lc->l;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("E3(%02x%02x%02x%02x) Phrase E3 / ftype[%d]=%d",
            lc->c[0],lc->c[1],lc->c[2],lc->c[3],p,lr->ftype[p]);
        printf(" fnval[%d][%d]=%ld\n",p,f,lr->fnval[p][f]);
    }
    return(0);
}

/*
Phrase data 4   : E4xxyyyy
  xx(fxtyp)   : Phrase extention type
	   0  : No
	   1  : Interval
	   2  : Number_of_control_data
  yyyy(fxval) : Phrase extention value
*/
int e4decode_mlk()
{
    int  i;
    int  pfg;
    int  p;
    int  f;

    p = lv->port;                      /* port */
    if(p >= MAXPT) p = 0;              /* error */
    f = lr->ftype[p];
    lr->fxtyp[p][f] = lc->c[1] & 0xff;
    lr->fxval[p][f] = lc->i[1] & 0x3ff;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("E4(%02x%02x%02x%02x) Phrase E4 / fxtyp[%d]=%d",
            lc->c[0],lc->c[1],lc->c[2],lc->c[3],p,lr->fxtyp[p][f]);
        printf(" fxval[%d][%d]=%d\n",p,f,lr->fxval[p][f]);
    }
    return(0);
}

/*
Execution code : E6vvwwxx
 vv(e6cn)  : execution control no. vv=00(fin),vv=01(1ch,fin),vv=02(2ch)
 ww(e6d1)  :
 xx(e6d2)  :
 vv : 0  Title message fin
      1  title message one (ww : data  xx : 00)
      2  title message two (ww : 1st data, xx : 2nd data);
      3  #define performer_sync_port xx (ww = 0)
      4  #define esu_sync_port xx (ww = 0)
*/
int e6decode_mlk()
{
    int  i;
    int  pfg;

    lv->e6cn = lc->c[1] & 0xff;
    lv->e6d1 = lc->c[2] & 0xff;
    lv->e6d2 = lc->c[3] & 0xff;
    if(ls->dchk || ls->trace) {
        if(ls->dckn) {
            pfg = 0;
            for(i = 0; i < ls->dckn; i++) {
                if(lc->c[0] == ls->dckcd[i]) pfg = 1;
            }
            if(!pfg) return(0);
        }
        if(ls->trace == 2) printf("             ");
        if(ls->trace == 3) printf("                      ");
        printf("E6(%02x%02x%02x%02x) Execution code / e6cn=%d e6d1=%d e6d2=%d\n",
        lc->c[0],lc->c[1],lc->c[2],lc->c[3],lv->e6cn,lv->e6d1,lv->e6d2);
    }
    return(0);
}

int dump_mlk(char dfile[STLN])
//char dfile[STLN];
{
    int  c1,c2,c3,c4;
    long l;
    long oct;
    int  mod;
    FILE *fpi;

    oct = 0;
    mod = 0;
    printf("\n\"%s\" dump start",dfile);
    l = 0;
    fpi = fopen_h(dfile,"rb");
    while(1) {
        c1 = fgetc(fpi);
        c2 = fgetc(fpi);
        c3 = fgetc(fpi);
        c4 = fgetc(fpi);
        if(!mod) printf("\n%07ld",oct);
        printf(" %02x%02x%02x%02x",c1,c2,c3,c4);
        l++;
        oct++;
        mod = oct % 8;
        if(c1 == 0xfe) break;
    }
    printf("\n\"%s\" input word count = %ld\n",dfile,l);
    fclose(fpi);
    return(0);
}

int filemake_h(unsigned int n,char inf[STLN],char fext[STLN],char dfile[STLN],int *yflp,int *xflp)
//unsigned int  n;
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
    int  member;

    trk = n/1000;  /* n/100 v_1.25 */
    member = n - trk * 1000;  /* n - trk * 100 v_1.25 */
    *yflp = trk;
    *xflp = member;
    pc = "0123456789abcdefghijklmnopqrstuvwxyz";
    for(i = 0; i < trk ; i++) pc++;
    wfnum[0] = *pc;
    wfnum[1] = '\0';
    sprintf(wk,"%02x",member);  /* %02d v_1.25 */
    strcat(wfnum,wk);
    dfile[0] = '\0';
    strcat(dfile,inf);
    strcat(dfile,"_");
    strcat(dfile,wfnum);
    strcat(dfile,fext);
    if(ls->debug) {
        printf("filemake_h() n=%d inf=%s fext=%s dfile=%s yflp=%d xflp=%d\n",
        n,inf,fext,dfile,*yflp,*xflp);
    }
    return(0);
}

FILE *fopen_h(char *file, char *mode)
//char *file;
//char *mode;
{
    FILE *fp;

    if((fp = fopen(file, mode)) == NULL) {
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
        printf("Can't open file %s mode %s\n",file, mode);
        exit(1);
    }
    return(fp);
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

int smptubit_mlk(union MCODE *u,FILE *fpo)
//union MCODE *u;
//FILE *fpo;
{
    unsigned char v;

    /* real time uinversal exclusive user bit message */
    if(ls->trace) printf("Trace! SMPTE user bit / f0 7f ");
    fputc(0xf0,fpo);
    fputc(0x7f,fpo);
    v = lv->chan & 0xff;
    fputc(v,fpo);
    fputc(0x01,fpo);
    fputc(0x02,fpo);
    if(ls->trace) printf("%02x 01 02 ",v);
    v = u->wc[1] & 0xf0;
    v = v >> 4;
    fputc(v,fpo);
    if(ls->trace) printf("%02x ",v);
    v = u->wc[1] & 0x0f;
    fputc(v,fpo);
    if(ls->trace) printf("%02x ",v);
    v = u->wc[0] & 0xf0;
    v = v >> 4;
    if(ls->trace) printf("%02x ",v);
    fputc(v,fpo);
    v = u->wc[0] & 0x0f;
    fputc(v,fpo);
    if(ls->trace) printf("%02x ",v);
    v = u->wc[3] & 0xf0;
    v = v >> 4;
    fputc(v,fpo);
    if(ls->trace) printf("%02x ",v);
    v = u->wc[3] & 0x0f;
    fputc(v,fpo);
    if(ls->trace) printf("%02x ",v);
    v = u->wc[2] & 0xf0;
    fputc(v,fpo);
    v = v >> 4;
    if(ls->trace) printf("%02x ",v);
    v = u->wc[2] & 0x0f;
    fputc(v,fpo);
    fputc(0x03,fpo);
    fputc(0xf7,fpo);
    lv->occnt = lv->occnt + 15;
    if(ls->trace) printf("%02x 03 f7\n",v);
    return(0);
}

/*
    make multi file
        ex0_x   ->  ex0_x00.mlk return(99)  prtn = 0
        ex0_xx  ->  ex0_0xx.mlk return(0)   prtn = 36
        ex0_xxx ->  ex0_xxx.mlk return(0)   prtn = 0
        ex0/Y;X ->  ex0_002.mlk retnrn(Y)   prtn = X (fnmb = 0,fnmb = -1)
        ex0/Y;X ->  ex0_002.mlk retnrn(Y-1) prtn = X (fnmb = 1)
    modify 1989.9.26 v_1.14  _axx. : hex suport
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
        if(*xmf == '(') break;      /* start to end bar & line select */
        if(*xmf == '/') vflg = 1;   /* 1 dimensional vector _xx or _x */
        if(*xmf == '_') vflg = 2;   /* 1 dimensional vector _xx or _x */
        if(*xmf == ';') xflg = 1;   /* 2 dimensional vector /Y;X */
        if(*xmf == ',') xflg = 1;   /* 2 dimensional vector /Y;X */
        if(*xmf == ',') *xmf = ';';
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
    sscanf(xw,"%x",prtn);  /* %d v_1.25 */
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
        ls->errcnt++;
        ls->msglvl = 3;
        if(ls->msglvl >= ls->prtlvl)
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
        sscanf(xw,"%x",xfile);  /* %d v_1.25 */
        strcpy(outf,w);
        strcat_h(outf,"_");
        fnum[1] = '\0';
        strcat_h(outf,fnum);
        sprintf(fnum,"%02x",fnmb+1);  /* %03d v_1.25 */
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
    sscanf(w,"%x",xfile);  /* %d v_1.25 */
    return(0);
}

int getdigit_h(char c,int *digit)
//char c;
//int  *digit;
{
    if(c >= '0' && c <= '9') *digit = c - '0';
    else if(c >= 'a' && c <= 'f') *digit = c - 'a' + 10;
    else if(c >= 'A' && c <= 'F') *digit = c - 'A' + 10;
    else return(-1);
    return(0);
}

int hlpread_mlk()
{
    int  c;
    FILE *fpi;
    char scrf[100];

    printf("\nMusic Edit File Linker V.%s-%s-%s\n",VERSION,OS,SUPPORT);
    if((fpi = fopen("mlkhlp.dat","r")) == NULL) {
        sprintf(scrf,"%s",TACTDIR);
        //sprintf(scrf,"%s\0",TACTDIR);
        strcat(scrf,"mlkhlp.dat");
        /* printf("scrf = %s\n",scrf); */
        fpi = fopen_h(scrf,"r");
    }
    while((c = fgetc(fpi)) != EOF) putchar(c);
    fclose(fpi);
    return(0);
}

