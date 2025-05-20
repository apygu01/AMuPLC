#include "stdio.h"
main(argc,argv)
int  argc;
char *argv[];
{
    long lsum;
    long csum;
    long cnum;
    long lnum;
    FILE *fopen(),*fpi;
    int  c;
    int  prf;

    lsum = 0;
    csum = 0;
    cnum = 0;
    lnum = 0;
    prf = 0;
    if(!strcmp(argv[1],"-s")) prf = 1;
    fpi = fopen("mex.h","rb");
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mex.h char count = %ld line count = %ld\n",cnum,lnum);

    fpi = fopen("mexm.c","rb");
    cnum = 0;
    lnum = 0;
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mexm.c char count = %ld line count = %ld\n",cnum,lnum);

    fpi = fopen("mex1.c","rb");
    cnum = 0;
    lnum = 0;
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mex1.c char count = %ld line count = %ld\n",cnum,lnum);

    fpi = fopen("mex2.c","rb");
    cnum = 0;
    lnum = 0;
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mex2.c char count = %ld line count = %ld\n",cnum,lnum);

    fpi = fopen("mex3.c","rb");
    cnum = 0;
    lnum = 0;
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mex3.c char count = %ld line count = %ld\n",cnum,lnum);

    fpi = fopen("mex4.c","rb");
    cnum = 0;
    lnum = 0;
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mex4.c char count = %ld line count = %ld\n",cnum,lnum);

    fpi = fopen("mex5.c","rb");
    cnum = 0;
    lnum = 0;
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mex5.c char count = %ld line count = %ld\n",cnum,lnum);

    fpi = fopen("mexw.c","rb");
    cnum = 0;
    lnum = 0;
    while((c = fgetc(fpi)) != EOF) {
        csum++;
        cnum++;
        if(c == 0x0d) {
            lsum++;
            lnum++;
        }
        if(prf) putchar(c);
    }
    fclose(fpi);
    printf("mexw.c char count = %ld line count = %ld\n",cnum,lnum);
    printf("mex total char count = %ld line count = %ld\n",csum,lsum);
    return(0);
}
