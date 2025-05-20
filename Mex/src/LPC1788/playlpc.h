// ls->mtclk modx comment 2021.4.22
// add Play End Loop 2020.8.23
// UPutch3 -> Port0 , UPutch4 -> Port1 2019.4.12
// playlpc.h 2019.4.5 Shutdown OK
// 2018.12.29 lpc2388 -> lpc1788
// 2018.8.8 test NG bak
// 2018.5.13 MTC Off Rel_V1.0 Size_15492
// playlpc.h 2017.11.20 test
/* System  Cont Reg */
#define SYS_SCS		((volatile unsigned int *)(0x400FC1A0))

/* Clock Pwoer Cont. Reg. */
#define PLL0CON		((volatile unsigned int *)(0x400FC080))
#define PLL0CFG		((volatile unsigned int *)(0x400FC084))
#define PLL0STAT	((volatile unsigned int *)(0x400FC088))
#define PLL0FEED	((volatile unsigned int *)(0x400FC08C))

#define CLK_CCLKSEL     ((volatile unsigned int *)(0x400Fc108))
#define CLK_USBCLKSEL   ((volatile unsigned int *)(0x400Fc108))
#define CLK_CLKSRCSEL	((volatile unsigned int *)(0x400FC10C))
#define CLK_PCLKSEL 	((volatile unsigned int *)(0x400FC1A8))  // 1/4 Reset

/* Power Cont. Reg  */
#define POW_PCONP	((volatile unsigned int *)(0x400FC0C4))

// IO Cont. Reg  UART
#define IOCON_P0_0      ((volatile unsigned int *)(0x4002C000)) //UART3 TXD
#define IOCON_P0_1      ((volatile unsigned int *)(0x4002C004)) //UART3 RXD
#define IOCON_P0_2      ((volatile unsigned int *)(0x4002C008)) //UART0 TXD
#define IOCON_P0_3      ((volatile unsigned int *)(0x4002C00C)) //UART0 RXD
#define IOCON_P0_10     ((volatile unsigned int *)(0x4002C028)) //UART2 TXD
#define IOCON_P0_15     ((volatile unsigned int *)(0x4002C03C)) //UART1 TXD
#define IOCON_P0_16     ((volatile unsigned int *)(0x4002C040)) //UART1 RXD
#define IOCON_P1_18     ((volatile unsigned int *)(0x4002C0C8)) //LED Display
#define IOCON_P0_22     ((volatile unsigned int *)(0x4002C058)) //UART4 TXD
#define IOCON_P0_25     ((volatile unsigned int *)(0x4002C064)) //UART3 TXD

// GPIO IOCON
#define IOCON_P0_4      ((volatile unsigned int *)(0x4002C010)) // Gstop=1
#define IOCON_P0_5      ((volatile unsigned int *)(0x4002C014)) // Pose
#define IOCON_P0_6      ((volatile unsigned int *)(0x4002C018)) // Gstop=4
#define IOCON_P0_7      ((volatile unsigned int *)(0x4002C01C)) // Stop
#define IOCON_P0_8      ((volatile unsigned int *)(0x4002C020))
#define IOCON_P0_9      ((volatile unsigned int *)(0x4002C024))
#define IOCON_P0_14     ((volatile unsigned int *)(0x4002C038))
#define IOCON_P1_18     ((volatile unsigned int *)(0x4002C0C8)) // LED2

/* GPIO Port0 Reg */
#define FGPIO_FIO0DIR   ((volatile unsigned int *)(0x20098000)) //Direction 
#define FGPIO_FIO0MASK  ((volatile unsigned int *)(0x20098010)) //Mask
#define FGPIO_FIO0PIN   ((volatile unsigned int *)(0x20098014)) //Pin Value
#define FGPIO_FIO0SET   ((volatile unsigned int *)(0x20098018)) //OutPut Set
#define FGPIO_FIO0CLR   ((volatile unsigned int *)(0x2009801C)) //OutPut Clear

/* GPIO Port1 Reg */
#define FGPIO_FIO1DIR   ((volatile unsigned int *)(0x20098020)) // Port1
#define FGPIO_FIO1MASK  ((volatile unsigned int *)(0x20098030))
#define FGPIO_FIO1PIN   ((volatile unsigned int *)(0x20098034))
#define FGPIO_FIO1SET   ((volatile unsigned int *)(0x20098038))
#define FGPIO_FIO1CLR   ((volatile unsigned int *)(0x2009803C))

/* GPIO Port2 Reg */
#define FGPIO_FIO2DIR   ((volatile unsigned int *)(0x20098040)) // Port2
#define FGPIO_FIO2MASK  ((volatile unsigned int *)(0x20098050))
#define FGPIO_FIO2PIN   ((volatile unsigned int *)(0x20098054))
#define FGPIO_FIO2SET   ((volatile unsigned int *)(0x20098058))
#define FGPIO_FIO2CLR   ((volatile unsigned int *)(0x2009805C))

/* GPIO Port3 Reg */
#define FGPIO_FIO3DIR   ((volatile unsigned int *)(0x20098060)) // Port3
#define FGPIO_FIO3MASK  ((volatile unsigned int *)(0x20098070))
#define FGPIO_FIO3PIN   ((volatile unsigned int *)(0x20098074))
#define FGPIO_FIO3SET   ((volatile unsigned int *)(0x20098078))
#define FGPIO_FIO3CLR   ((volatile unsigned int *)(0x2009807C))

/* GPIO Port4 Reg */
#define FGPIO_FIO4DIR   ((volatile unsigned int *)(0x20098080)) // Port4
#define FGPIO_FIO4MASK  ((volatile unsigned int *)(0x20098090))
#define FGPIO_FIO4PIN   ((volatile unsigned int *)(0x20098094))
#define FGPIO_FIO4SET   ((volatile unsigned int *)(0x20098098))
#define FGPIO_FIO4CLR   ((volatile unsigned int *)(0x2009809C))

/* GPIO Port5 Reg */
#define FGPIO_FIO5DIR   ((volatile unsigned int *)(0x200980A0)) // Port5
#define FGPIO_FIO5MASK  ((volatile unsigned int *)(0x200980B0))
#define FGPIO_FIO5PIN   ((volatile unsigned int *)(0x200980B4))
#define FGPIO_FIO5SET   ((volatile unsigned int *)(0x200980B8))
#define FGPIO_FIO5CLR   ((volatile unsigned int *)(0x200980BC))

/* GPIO Interrupt Register0 Base 0x40028000 Manual P139
   STATUS GPIO Overall inturrupt Status
   SATTR  GPIO Inturrupt status for Rising edge
   STATF  GPIO Inturrupu status for Falling edge
   CLR    GPIO Inturrupt Clear
   ENR    GPIO Inturrupt Enable for Rising edge
   ENF    GPIO Inturrupt Enable for Falling edge
*/
#define GPIO_IO0IntStat  ((volatile unsigned int *)(0x40028080))
#define GPIO_IO0IntStatR ((volatile unsigned int *)(0x40028084))
#define GPIO_IO0IntStatF ((volatile unsigned int *)(0x40028088))
#define GPIO_IO0IntClr   ((volatile unsigned int *)(0x4002808C))
#define GPIO_IO0IntEnR   ((volatile unsigned int *)(0x40028090))
#define GPIO_IO0IntEnF   ((volatile unsigned int *)(0x40028094))

// NVIC ISER0 Inturrupu Set-Enable Register 0
#define ISER0           ((volatile unsigned int *)(0xE000E100))
#define ISER0_TIMER0_INT_BIT  (0x00000002) // ISER0 Timer0 bit1
#define ISER0_UART1_INT_BIT   (0x00000040) // ISER0 UART1 bit6
#define ISER0_EINT0_INT_BIT   (0x00040000) // ISER0 EINT0 bit18 (RESET)
#define ISER0_EINT1_INT_BIT   (0x00040000) // ISER0 EINT1 bit19
#define ISER0_EINT2_INT_BIT   (0x00040000) // ISER0 EINT2 bit20
#define ISER0_EINT3_INT_BIT   (0x00200000) // ISER0 EINT3 bit21 (GPIO)
#define ISER0_INT_BITS        (0x00240042) // Timer0 UART1 EINT0 EINT3 Enable

// IABR0 Interrupt Active Bit Register 0
#define IABR0           ((volatile unsigned int *)(0xE000E300))

/* play1.c __irq
   status = *IABR0;
   status = *GPIO_IO0IntStatF; // GPIO port0 falling Edge int.
   status 0x10 Gstop=1 Po_4
   status 0x20 Gstop=2 Po_5 pose
   status 0x40 Gstop=4 Po_6
   status 0x80 Gstop=8 Po_7 stop
*/

/*
 Timer0 Register  0x40004000
 Timer1 Register  0x40008000
 Timer2 Register  0x40090000
 Timer3 Register  0x40094000

      offset
 IR   0x000  // Interrupt Register
 TCR  0x004  // Timer Control Register
 TC   0x008  // Timer Counter
 PR   0x00C  // Prescale Register
 PC   0x010  // Prescale Counter
 MCR  0x014  // Match Controle Register
 MR0  0x018  // Match Register0
 MR1  0x01C  // Match Register1
 MR2  0x020  // Match Register2
 MR3  0x024  // Match Register3
 CCR  0x028  // Capture Control Register
 CR0  0x02C  // Caputre Register0
 CR1  0x030  // Caputre Register1
 EMR  0x03C  // External Match Register
 CTCR 0x070  // Count Control Register
*/
// Timer0 Register
#define TIMER0_InterruptRegister    ((volatile unsigned int *)(0x40004000))
#define TIMER0_TimerControlRegister ((volatile unsigned int *)(0x40004004))
#define TIMER0_TimerCounter         ((volatile unsigned int *)(0x40004008))
#define TIMER0_PrescaleRegister     ((volatile unsigned int *)(0x4000400C))
#define TIMER0_PrescaleCounter      ((volatile unsigned int *)(0x40004010))
#define TIMER0_MatchControlRegister ((volatile unsigned int *)(0x40004014))
#define TIMER0_MatchRegister0       ((volatile unsigned int *)(0x40004018))

/* UART */
#define UART_BaseAdr0	0x4000C000	/* UART0 Base Address */
#define UART_BaseAdr1	0x40010000	/* UART1 Base Address */
#define UART_BaseAdr2	0x40098000	/* UART2 Base Address */
#define UART_BaseAdr3	0x4009C000	/* UART3 Base Address */
#define UART_BaseAdr4	0x400A4000	/* UART4 Base Address */

// UART0 
#define COM_BaseAdr0    0x4000C000      /* UART0 Base Address */
#define UART0_INT_BIT   0x00000080  // UART0 Int bit
#define COM_RBR0                ((volatile unsigned char *)(COM_BaseAdr0+0x00))
#define COM_THR0                ((volatile unsigned char *)(COM_BaseAdr0+0x00))
#define COM_DLL0                ((volatile unsigned char *)(COM_BaseAdr0+0x00))
#define COM_DLM0                ((volatile unsigned char *)(COM_BaseAdr0+0x04))
#define COM_IER0                ((volatile unsigned char *)(COM_BaseAdr0+0x04))
#define COM_IIR0                ((volatile unsigned char *)(COM_BaseAdr0+0x08))
#define COM_FCR0                ((volatile unsigned char *)(COM_BaseAdr0+0x08))
#define COM_LCR0                ((volatile unsigned char *)(COM_BaseAdr0+0x0C))
#define COM_LSR0                ((volatile unsigned char *)(COM_BaseAdr0+0x14))
#define COM_SCR0                ((volatile unsigned char *)(COM_BaseAdr0+0x1C))
#define COM_FDR0                ((volatile unsigned char *)(COM_BaseAdr0+0x28))

/* UART1 */
#define COM_BaseAdr1	0x40010000	/* UART1 Base Address */
#define UART1_INT_BIT   0x00000080  // UART1 Int bit
#define COM_RBR1		((volatile unsigned char *)(COM_BaseAdr1+0x00))
#define COM_THR1		((volatile unsigned char *)(COM_BaseAdr1+0x00))
#define COM_DLL1		((volatile unsigned char *)(COM_BaseAdr1+0x00))
#define COM_DLM1		((volatile unsigned char *)(COM_BaseAdr1+0x04))
#define COM_IER1		((volatile unsigned char *)(COM_BaseAdr1+0x04))
#define COM_IIR1		((volatile unsigned char *)(COM_BaseAdr1+0x08))
#define COM_FCR1		((volatile unsigned char *)(COM_BaseAdr1+0x08))
#define COM_LCR1		((volatile unsigned char *)(COM_BaseAdr1+0x0C))
#define COM_LSR1		((volatile unsigned char *)(COM_BaseAdr1+0x14))
#define COM_SCR1		((volatile unsigned char *)(COM_BaseAdr1+0x1C))
#define COM_FDR1		((volatile unsigned char *)(COM_BaseAdr1+0x28))

/* UART2 */
#define COM_BaseAdr2	0x40098000	/* UART2 Base Address */
#define COM_RBR2		((volatile unsigned char *)(COM_BaseAdr2+0x00))
#define COM_THR2		((volatile unsigned char *)(COM_BaseAdr2+0x00))
#define COM_DLL2		((volatile unsigned char *)(COM_BaseAdr2+0x00))
#define COM_DLM2		((volatile unsigned char *)(COM_BaseAdr2+0x04))
#define COM_IER2		((volatile unsigned char *)(COM_BaseAdr2+0x04))
#define COM_IIR2		((volatile unsigned char *)(COM_BaseAdr2+0x08))
#define COM_FCR2		((volatile unsigned char *)(COM_BaseAdr2+0x08))
#define COM_LCR2		((volatile unsigned char *)(COM_BaseAdr2+0x0C))
#define COM_LSR2		((volatile unsigned char *)(COM_BaseAdr2+0x14))
#define COM_SCR2		((volatile unsigned char *)(COM_BaseAdr2+0x1C))
#define COM_FDR2		((volatile unsigned char *)(COM_BaseAdr2+0x28))

/* UART3 */
#define COM_BaseAdr3	0x4009C000	/* UART3 Base Address */
#define COM_RBR3		((volatile unsigned char *)(COM_BaseAdr3+0x00))
#define COM_THR3		((volatile unsigned char *)(COM_BaseAdr3+0x00))
#define COM_DLL3		((volatile unsigned char *)(COM_BaseAdr3+0x00))
#define COM_DLM3		((volatile unsigned char *)(COM_BaseAdr3+0x04))
#define COM_IER3		((volatile unsigned char *)(COM_BaseAdr3+0x04))
#define COM_IIR3		((volatile unsigned char *)(COM_BaseAdr3+0x08))
#define COM_FCR3		((volatile unsigned char *)(COM_BaseAdr3+0x08))
#define COM_LCR3		((volatile unsigned char *)(COM_BaseAdr3+0x0C))
#define COM_LSR3		((volatile unsigned char *)(COM_BaseAdr3+0x14))
#define COM_SCR3		((volatile unsigned char *)(COM_BaseAdr3+0x1C))
#define COM_FDR3		((volatile unsigned char *)(COM_BaseAdr3+0x28))

/* UART4 */
#define COM_BaseAdr4    0x400A4000      /* UART4 Base Address */
#define COM_RBR4                ((volatile unsigned char *)(COM_BaseAdr4+0x00))
#define COM_THR4                ((volatile unsigned char *)(COM_BaseAdr4+0x00))
#define COM_DLL4                ((volatile unsigned char *)(COM_BaseAdr4+0x00))
#define COM_DLM4                ((volatile unsigned char *)(COM_BaseAdr4+0x04))
#define COM_IER4                ((volatile unsigned char *)(COM_BaseAdr4+0x04))
#define COM_IIR4                ((volatile unsigned char *)(COM_BaseAdr4+0x08))
#define COM_FCR4                ((volatile unsigned char *)(COM_BaseAdr4+0x08))
#define COM_LCR4                ((volatile unsigned char *)(COM_BaseAdr4+0x0C))
#define COM_LSR4                ((volatile unsigned char *)(COM_BaseAdr4+0x14))
#define COM_SCR4                ((volatile unsigned char *)(COM_BaseAdr4+0x1C))
#define COM_FDR4                ((volatile unsigned char *)(COM_BaseAdr4+0x28))

#define MAXPT    6        // Max. port number

typedef struct {
    int  memb;         /* member */
    int  gate;         /* gate */
    int  chan;         /* channel */
    int  trak;         /* track */
    int  tsym;         /* tempo symbol no. */
    int  temp;         /* tempo */
    int  movn;         /* movement number */
    int  barn;         /* bar number */
    long sync;         /* sync value */
    int  notep;        /* note position */
    int  f0typ;        /* f0 code file type  */
    int  f0num;        /* f0 code file number */
    int  ssequ;        /* for start movement number select */
    long sbarn;        /* for start bar number select */
    int  esequ;        /* for end movement number select */
    long ebarn;        /* for end bar number select */
} MEXVAR;

union MCODE {
    unsigned char wc[4];
    unsigned short wi[2];
};

typedef struct {
    int  smpte;
    int  actpn;        /* active port number */
    int  mtcport;      /* midi time code send port */
    int  smptyp;       /* SMPTE flame type */
    int  smptef;       /* SMPTE flame number */
    //int  modx;         /* ls->mexres/120 */
    int  mtcnt;        /* MTC count */
    //int  mtclk;        /* MTC timer count */
    int  mtcaf;        /* MTC activ flag */
    int  mtchh;        /* MTC hh */
    int  mtcmm;        /* MTC mm */
    int  mtcss;        /* MTC ss */
    int  mtcff;        /* MTC ff */
    int  mexres;       /* mex flame resolution */
    int  subclk;       // timeclock flag
} MEXSRG;

// 19.2 Kbps org
#define BUFSIZE 10000    // recive buffer size
#define OFFLIM1  6000    // recive flow level1 (Play start)
#define OFFLIM2  5000    // level2
#define OFFLIM3  4000    // level3
#define ONLIMIT  3000    // xon limit

/*
// 19.2 K bps LPC1788 test
#define BUFSIZE  33000    // recive buffer size
#define OFFLIM1  28000    // recive flow level1
#define OFFLIM2  30000    // level2
#define OFFLIM3  32000    // level3
#define ONLIMIT   5000    // xon limit
*/

/*
// 19.2 K bps Default
#define BUFSIZE  18000    // recive buffer size
#define OFFLIM1  10000    // recive flow level1
#define OFFLIM2  12000    // level2
#define OFFLIM3  14000    // level3
#define ONLIMIT   4000    // xon limit
*/

// #define TRUE     1
// #define FALSE    0
#define XON      0x11
#define XOFF     0x13
#define ESC      0x1b     // Stop
#define EOB      0xff     // End of Buffer test

/******************************************************************************
**                            End Of File
******************************************************************************/

