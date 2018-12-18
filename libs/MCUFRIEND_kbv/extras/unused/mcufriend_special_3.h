//################################# SSD1289 SHIELD #######################
#define SSD1289_JUMPERS 2       //Uno Shield with VERY different pin-out to Mcufriend
//#define USE_SSD1289_SHIELD_UNO 
//#define USE_SSD1289_SHIELD_MEGA 
//#define USE_SSD1289_SHIELD_DUE 

#elif defined(__AVR_ATmega328P__) && defined(USE_SSD1289_SHIELD_UNO)    //on UNO
#warning using SSD1289 Shield for mega328
#define RD_PORT PORTC
#define RD_PIN  3
#define WR_PORT PORTC
#define WR_PIN  2
#define CD_PORT PORTC
#define CD_PIN  1
#define CS_PORT PORTC
#define CS_PIN  0
#define RESET_PORT PORTB
#define RESET_PIN  1      //actually SD_CS

// SSD1289 shield has LCD_D0 on RXD0.   Fine for write-only
// For any Read operations,  put jumper from D0 to D8, Switch #2 to OFF.
// If using Serial,  jumper D1 to A5, Switch #1 to OFF
#if SSD1289_JUMPERS == 0
#warning no jumpers.  Switch #1=ON, #2=ON
#define BMASK    0x00       //0x00 for output, 0x01 for Read + Serial
#define CMASK    0x00       //0x20 for Read + Serial 
#define DMASK    (~BMASK)
#define write8(x)     { PORTD = x; WR_STROBE; }
#define read_8()      ( PIND )
#elif SSD1289_JUMPERS == 1
#warning jumper D0 to D8.  Switch #1=ON, #2=OFF
#define BMASK    0x01       //0x00 for output, 0x01 for Read + Serial
#define CMASK    0x00       //0x20 for Read + Serial
#define DMASK    (~BMASK)
#define write8(x)     { PORTD = (PORTD & ~DMASK) | (x & DMASK); PORTB = (PORTB & ~BMASK) | (x & BMASK); WR_STROBE; }
#define read_8()      ( (PIND & DMASK)|(PINB & BMASK) )
#elif SSD1289_JUMPERS == 2
#warning jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#define BMASK    (1<<0)       //0x00 for output, 0x01 for Read + Serial
#define CMASK    (1<<5)       //0x20 for Read + Serial
#define DMASK    (0xFC)
#define write8(x)     { PORTC = (PORTC & ~CMASK) | ((x<<4) & CMASK);\
                        PORTD = (PORTD & ~DMASK) | (x & DMASK);\
						PORTB = (PORTB & ~BMASK) | (x & BMASK); WR_STROBE; }
#define read_8()      ( ((PINC & CMASK)>>4)|(PIND & DMASK)|(PINB & BMASK) )
#endif
#define setWriteDir() { DDRC |=  CMASK; DDRD |=  DMASK; DDRB |=  BMASK; }
#define setReadDir()  { DDRC &= ~CMASK; DDRD &= ~DMASK; DDRB &= ~BMASK; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATmega2560__) && defined(USE_SSD1289_SHIELD_MEGA)   //on MEGA2560
#warning using SSD1289 Shield for mega2560
#define RD_PORT PORTF
#define RD_PIN  3      //A3
#define WR_PORT PORTF
#define WR_PIN  2      //A2
#define CD_PORT PORTF
#define CD_PIN  1      //A1
#define CS_PORT PORTF
#define CS_PIN  0      //A0
#define RESET_PORT PORTH
#define RESET_PIN  6   //D9 DS_CS,  D10=T_CS, D9=SD_CS, D8=n.c.

// only for SSD1289 data bus on D2..D9  UNTESTED
#if (SSD1289_JUMPERS == 0)    //Switch #1=ON, #2=ON
#warning no jumpers Switch #1=ON, #2=ON
#define EMASK         0x3B
#define FMASK         0x00
#define HMASK         0x18
#define GMASK         0x20
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTE &= ~EMASK; \
	PORTE |= (((x) & (1<<0)) << 0); \
	PORTE |= (((x) & (1<<1)) << 0); \
	PORTE |= (((x) & (3<<2)) << 2); \
	PORTG |= (((x) & (1<<4)) << 1); \
	PORTE |= (((x) & (1<<5)) >> 2); \
	PORTH |= (((x) & (3<<6)) >> 3); \
}

#define read_8()      ( ((PINE & (1<<0)) >> 0)\
                      | ((PINE & (1<<1)) >> 0)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#elif (SSD1289_JUMPERS == 1)    //jumper D0 to D8.  Switch #1=ON, #2=OFF
#warning jumper D0 to D8.  Switch #1=ON, #2=OFF
#define EMASK         0x3A
#define FMASK         0x00
#define HMASK         0x38
#define GMASK         0x20
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTE &= ~EMASK; \
	PORTH |= (((x) & (1<<0)) << 5); \
	PORTE |= (((x) & (1<<1)) << 0); \
	PORTE |= (((x) & (3<<2)) << 2); \
	PORTG |= (((x) & (1<<4)) << 1); \
	PORTE |= (((x) & (1<<5)) >> 2); \
	PORTH |= (((x) & (3<<6)) >> 3); \
}

#define read_8()      ( ((PINH & (1<<5)) >> 5)\
                      | ((PINE & (1<<1)) >> 0)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#elif (SSD1289_JUMPERS == 2)    //jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#warning jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#define FMASK         0x20
#define EMASK         0x38
#define HMASK         0x38
#define GMASK         0x20
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTF &= ~FMASK; PORTE &= ~EMASK; \
	PORTH |= (((x) & (1<<0)) << 5); \
	PORTF |= (((x) & (1<<1)) << 4); \
	PORTE |= (((x) & (3<<2)) << 2); \
	PORTG |= (((x) & (1<<4)) << 1); \
	PORTE |= (((x) & (1<<5)) >> 2); \
	PORTH |= (((x) & (3<<6)) >> 3); \
}

#define read_8()      ( ((PINH & (1<<5)) >> 5)\
                      | ((PINF & (1<<5)) >> 4)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#endif
#define setWriteDir() { DDRH |=  HMASK; DDRG |=  GMASK; DDRF |=  FMASK; DDRE |=  EMASK;  }
#define setReadDir()  { DDRH &= ~HMASK; DDRG &= ~GMASK; DDRF &= ~FMASK; DDRE &= ~EMASK;  }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__SAM3X8E__) && defined(USE_SSD1289_SHIELD_DUE)   // on DUE
#warning USE_SSD1289_SHIELD_DUE
// configure macros for the control pins
#define RD_PORT PIOA
#define RD_PIN  22     //A3
#define WR_PORT PIOA
#define WR_PIN  23     //A2
#define CD_PORT PIOA
#define CD_PIN  24     //A1
#define CS_PORT PIOA
#define CS_PIN  16     //A0
#define RESET_PORT PIOC
#define RESET_PIN  21  //D9 Touch CS
// configure macros for data bus
// only for SSD1289 data bus on D2..D9  UNTESTED
#if SSD1289_JUMPERS == 0
#warning no jumpers Switch #1=ON, #2=ON
#define AMASK         (3<<8)
#define BMASK         (1<<25)
#define CMASK         (0xBC << 21)
#define write_8(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; \
	PIOA->PIO_SODR = (((x) & (1<<0)) << 8); \
	PIOA->PIO_SODR = (((x) & (1<<1)) << 8); \
	PIOB->PIO_SODR = (((x) & (1<<2)) << 23); \
	PIOC->PIO_SODR = (((x) & (1<<3)) << 25); \
	PIOC->PIO_SODR = (((x) & (1<<4)) << 22); \
	PIOC->PIO_SODR = (((x) & (1<<5)) << 20); \
	PIOC->PIO_SODR = (((x) & (1<<6)) << 18); \
	PIOC->PIO_SODR = (((x) & (1<<7)) << 16); \
}

#define read_8()      ( ((PIOA->PIO_PDSR & (1<<8)) >> 8)\
                      | ((PIOA->PIO_PDSR & (1<<9)) >> 8)\
                      | ((PIOB->PIO_PDSR & (1<<25)) >> 23)\
                      | ((PIOC->PIO_PDSR & (1<<28)) >> 25)\
                      | ((PIOC->PIO_PDSR & (1<<26)) >> 22)\
                      | ((PIOC->PIO_PDSR & (1<<25)) >> 20)\
                      | ((PIOC->PIO_PDSR & (1<<24)) >> 18)\
                      | ((PIOC->PIO_PDSR & (1<<23)) >> 16)\
                      )
#elif SSD1289_JUMPERS == 1
#warning jumper D0 to D8.  Switch #1=ON, #2=OFF
#define AMASK         (1<<9)
#define BMASK         (1<<25)
#define CMASK         (0xBE << 21)
#define write_8(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; \
	PIOC->PIO_SODR = (((x) & (1<<0)) << 22); \
	PIOA->PIO_SODR = (((x) & (1<<1)) << 8); \
	PIOB->PIO_SODR = (((x) & (1<<2)) << 23); \
	PIOC->PIO_SODR = (((x) & (1<<3)) << 25); \
	PIOC->PIO_SODR = (((x) & (1<<4)) << 22); \
	PIOC->PIO_SODR = (((x) & (1<<5)) << 20); \
	PIOC->PIO_SODR = (((x) & (1<<6)) << 18); \
	PIOC->PIO_SODR = (((x) & (1<<7)) << 16); \
}

#define read_8()      ( ((PIOC->PIO_PDSR & (1<<22)) >> 22)\
| ((PIOA->PIO_PDSR & (1<<9)) >> 8)\
| ((PIOB->PIO_PDSR & (1<<25)) >> 23)\
| ((PIOC->PIO_PDSR & (1<<28)) >> 25)\
| ((PIOC->PIO_PDSR & (1<<26)) >> 22)\
| ((PIOC->PIO_PDSR & (1<<25)) >> 20)\
| ((PIOC->PIO_PDSR & (1<<24)) >> 18)\
| ((PIOC->PIO_PDSR & (1<<23)) >> 16)\
)
#elif SSD1289_JUMPERS == 2
#warning jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#define AMASK         (1<<4)
#define BMASK         (1<<25)
#define CMASK         (0xBE << 21)
#define write_8(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; \
	                   PIOC->PIO_SODR = (((x) & (1<<0)) << 22); \
	                   PIOA->PIO_SODR = (((x) & (1<<1)) << 3); \
	                   PIOB->PIO_SODR = (((x) & (1<<2)) << 23); \
	                   PIOC->PIO_SODR = (((x) & (1<<3)) << 25); \
	                   PIOC->PIO_SODR = (((x) & (1<<4)) << 22); \
	                   PIOC->PIO_SODR = (((x) & (1<<5)) << 20); \
	                   PIOC->PIO_SODR = (((x) & (1<<6)) << 18); \
	                   PIOC->PIO_SODR = (((x) & (1<<7)) << 16); \
                     }

#define read_8()      ( ((PIOC->PIO_PDSR & (1<<22)) >> 22)\
                      | ((PIOA->PIO_PDSR & (1<<4)) >> 3)\
                      | ((PIOB->PIO_PDSR & (1<<25)) >> 23)\
                      | ((PIOC->PIO_PDSR & (1<<28)) >> 25)\
                      | ((PIOC->PIO_PDSR & (1<<26)) >> 22)\
                      | ((PIOC->PIO_PDSR & (1<<25)) >> 20)\
                      | ((PIOC->PIO_PDSR & (1<<24)) >> 18)\
                      | ((PIOC->PIO_PDSR & (1<<23)) >> 16)\
                      )
#endif
#define setWriteDir() { PIOA->PIO_OER = AMASK; PIOB->PIO_OER = BMASK; PIOC->PIO_OER = CMASK; }
#define setReadDir()  { \
	PMC->PMC_PCER0 = (1 << ID_PIOA)|(1 << ID_PIOB)|(1 << ID_PIOC);\
	PIOA->PIO_ODR = AMASK; PIOB->PIO_ODR = BMASK; PIOC->PIO_ODR = CMASK;\
}
#define write8(x)     { write_8(x); WR_ACTIVE; WR_STROBE; WR_IDLE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE4; dst = read_8(); RD_IDLE; RD_IDLE; RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }
// Shield Control macros.
#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

//##################################### USE_FRDM_K20 ###################################################
//#define USE_FRDM_K20
#elif defined(__MK20DX128__) && defined(USE_FRDM_K20) // Uno Shield on FRDM-K20
#warning Uno Shield on FRDM-K20
#define RD_PORT GPIOC
#define RD_PIN  0
#define WR_PORT GPIOC
#define WR_PIN  1
#define CD_PORT GPIOD
#define CD_PIN  6
#define CS_PORT GPIOD
#define CS_PIN  5
#define RESET_PORT GPIOB
#define RESET_PIN  1

// configure macros for the data pins
#define AMASK ((1<<12)|(1<<5)|(1<<2)|(1<<1))
#define CMASK ((1<<8)|(1<<4)|(1<<3))
#define DMASK ((1<<4))
  #define write_8(d) { \
   GPIOA_PCOR = AMASK; GPIOC_PCOR = CMASK; GPIOD_PCOR = DMASK; \
   GPIOA_PSOR =      (((d) & (1<<0)) << 12) \
                  | (((d) & (1<<1)) << 1) \
                  |	(((d) & (1<<2)) << 3) \
                  | (((d) & (1<<5)) >> 4); \
   GPIOC_PSOR =      (((d) & (1<<4)) << 4) \
                  | (((d) & (3<<6)) >> 3); \
   GPIOD_PSOR =      (((d) & (1<<3)) << 1); \
  } 
  #define read_8() (          (((GPIOA_PDIR & (1<<5)) >> 3) \
                             | ((GPIOA_PDIR & (1<<1)) << 4) \
                             | ((GPIOA_PDIR & (1<<12)) >> 12) \
                             | ((GPIOA_PDIR & (1<<2))  >> 1) \
                             | ((GPIOC_PDIR & (1<<8))  >> 4) \
                             | ((GPIOC_PDIR & (3<<3))  << 3) \
                             | ((GPIOD_PDIR & (1<<4))  >> 1)))
  #define setWriteDir() {GPIOA_PDDR |=  AMASK;GPIOC_PDDR |=  CMASK;GPIOD_PDDR |=  DMASK; }
  #define setReadDir()  {GPIOA_PDDR &= ~AMASK;GPIOC_PDDR &= ~CMASK;GPIOD_PDDR &= ~DMASK; }

#define write8(x)     { write_8(x); WR_ACTIVE2; WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE4; RD_ACTIVE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PASTE(x, y)   x ## y

#define PIN_LOW(port, pin)    PASTE(port, _PCOR) =  (1<<(pin))
#define PIN_HIGH(port, pin)   PASTE(port, _PSOR) =  (1<<(pin))
#define PIN_OUTPUT(port, pin) PASTE(port, _PDDR) |= (1<<(pin))

//####################################### D21_XPRO ###########################################################
#elif defined(__SAMD21J18A__)   //regular UNO shield on D21_XPRO
#warning regular UNO shield on D21_XPRO
#include "samd21.h"
// configure macros for the control pins
#define RD_PORT PORT->Group[1]
#define RD_PIN  0
#define WR_PORT PORT->Group[1]
#define WR_PIN  1
#define CD_PORT PORT->Group[0]
#define CD_PIN  10
#define CS_PORT PORT->Group[0]
#define CS_PIN  11
#define RESET_PORT PORT->Group[0]
#define RESET_PIN  8
// configure macros for data bus
#define AMASK 0x00220000
#define BMASK 0x0000C0E4
#define write_8(d) { \
	PORT->Group[0].OUT.reg = (PORT->Group[0].OUT.reg & ~AMASK) \
	| (((d) & (1<<5)) << 16) \
	| (((d) & (1<<7)) << 10); \
	PORT->Group[1].OUT.reg = (PORT->Group[1].OUT.reg & ~BMASK) \
	| (((d) & (3<<0)) << 6) \
	| (((d) & (1<<2)) << 12) \
	| (((d) & (1<<3)) >> 1) \
	| (((d) & (1<<4)) << 1) \
	| (((d) & (1<<6)) << 9); \
}
#define read_8() (          (((PORT->Group[0].IN.reg & (1<<21)) >> 16) \
| ((PORT->Group[0].IN.reg & (1<<17)) >> 10) \
| ((PORT->Group[1].IN.reg & (3<<6)) >> 6) \
| ((PORT->Group[1].IN.reg & (1<<14)) >> 12) \
| ((PORT->Group[1].IN.reg & (1<<2))  << 1) \
| ((PORT->Group[1].IN.reg & (1<<5))  >> 1) \
| ((PORT->Group[1].IN.reg & (1<<15))  >> 9)))
#define setWriteDir() { \
	PORT->Group[0].DIRSET.reg = AMASK; \
	PORT->Group[1].DIRSET.reg = BMASK; \
	PORT->Group[0].WRCONFIG.reg = (AMASK>>16) | (0<<22) | (0<<28) | (1<<30) | (1<<31); \
	PORT->Group[1].WRCONFIG.reg = (BMASK & 0xFFFF) | (0<<22) | (0<<28) | (1<<30); \
}
#define setReadDir()  { \
	PORT->Group[0].DIRCLR.reg = AMASK; \
	PORT->Group[1].DIRCLR.reg = BMASK; \
	PORT->Group[0].WRCONFIG.reg = (AMASK>>16) | (1<<17) | (0<<28) | (1<<30) | (1<<31); \
	PORT->Group[1].WRCONFIG.reg = (BMASK & 0xFFFF) | (1<<17) | (0<<28) | (1<<30); \
}

#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }
// Shield Control macros.
#define PIN_LOW(port, pin)    (port).OUTCLR.reg = (1<<(pin))
#define PIN_HIGH(port, pin)   (port).OUTSET.reg = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port).DIR.reg |= (1<<(pin))

//###################################################################################