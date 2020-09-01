// weak_sectors.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//
#include <cstdint>
#ifdef _MSC_VER
#include "stdafx.h"
#endif
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "memory.h"
#include "encoder_tables"
#include "ecc.h"
#ifdef _MSC_VER
#include "windows.h"
#endif

int scramble_L2(unsigned char *inout)
{
  unsigned char *r = inout + 12;
  unsigned char *s = yellowbook_scrambler;
  unsigned int i;
  unsigned int *f = (unsigned int *)inout;

  for (i = (L2_RAW + L2_Q + L2_P +16)/sizeof(unsigned char); i; i--) {
    *r++ ^= *s++;
  }

  /* generate F1 frames */
  for (i = (2352/sizeof(unsigned int)); i; i--) {
    *f++ = ((*f & 0xff00ff00UL) >> 8) | ((*f & 0x00ff00ffUL) << 8);
  }

  return 0;
}

int descramble_L2(unsigned char *inout)
{
  unsigned char *r = inout + 12;
  unsigned char *s = yellowbook_scrambler;
  unsigned int i;
  unsigned int *f = (unsigned int *)inout;

  for (i = (2352/sizeof(unsigned int)); i; i--) {
    *f++ = ((*f & 0xff00ff00UL) >> 8) | ((*f & 0x00ff00ffUL) << 8);
  }

  for (i = (L2_RAW + L2_Q + L2_P +16)/sizeof(unsigned char); i; i--) {
    *r++ ^= *s++;
  }

  /* generate F1 frames */

  return 0;
}
static char merging_bits[4][3]=
{
	{ 0,0,0 },{ 0,0,1 },{ 0,1,0 },{ 1,0,0 }
};

#define IVP_NO10ZEROS		0x01

static char EFM_table[256][14]=
{
	/* 0x00: */ { 0,1,0,0,1,0,0,0,1,0,0,0,0,0 },
	/* 0x01: */ { 1,0,0,0,0,1,0,0,0,0,0,0,0,0 },
	/* 0x02: */ { 1,0,0,1,0,0,0,0,1,0,0,0,0,0 },
	/* 0x03: */ { 1,0,0,0,1,0,0,0,1,0,0,0,0,0 },
	/* 0x04: */ { 0,1,0,0,0,1,0,0,0,0,0,0,0,0 },
	/* 0x05: */ { 0,0,0,0,0,1,0,0,0,1,0,0,0,0 },
	/* 0x06: */ { 0,0,0,1,0,0,0,0,1,0,0,0,0,0 },
	/* 0x07: */ { 0,0,1,0,0,1,0,0,0,0,0,0,0,0 },
	/* 0x08: */ { 0,1,0,0,1,0,0,1,0,0,0,0,0,0 },
	/* 0x09: */ { 1,0,0,0,0,0,0,1,0,0,0,0,0,0 },
	/* 0x0A: */ { 1,0,0,1,0,0,0,1,0,0,0,0,0,0 },
	/* 0x0B: */ { 1,0,0,0,1,0,0,1,0,0,0,0,0,0 },
	/* 0x0C: */ { 0,1,0,0,0,0,0,1,0,0,0,0,0,0 },
	/* 0x0D: */ { 0,0,0,0,0,0,0,1,0,0,0,0,0,0 },
	/* 0x0E: */ { 0,0,0,1,0,0,0,1,0,0,0,0,0,0 },
	/* 0x0F: */ { 0,0,1,0,0,0,0,1,0,0,0,0,0,0 },
	/* 0x10: */ { 1,0,0,0,0,0,0,0,1,0,0,0,0,0 },
	/* 0x11: */ { 1,0,0,0,0,0,1,0,0,0,0,0,0,0 },
	/* 0x12: */ { 1,0,0,1,0,0,1,0,0,0,0,0,0,0 },
	/* 0x13: */ { 0,0,1,0,0,0,0,0,1,0,0,0,0,0 },
	/* 0x14: */ { 0,1,0,0,0,0,1,0,0,0,0,0,0,0 },
	/* 0x15: */ { 0,0,0,0,0,0,1,0,0,0,0,0,0,0 },
	/* 0x16: */ { 0,0,0,1,0,0,1,0,0,0,0,0,0,0 },
	/* 0x17: */ { 0,0,1,0,0,0,1,0,0,0,0,0,0,0 },
	/* 0x18: */ { 0,1,0,0,1,0,0,0,0,1,0,0,0,0 },
	/* 0x19: */ { 1,0,0,0,0,0,0,0,0,1,0,0,0,0 },
	/* 0x1A: */ { 1,0,0,1,0,0,0,0,0,1,0,0,0,0 },
	/* 0x1B: */ { 1,0,0,0,1,0,0,0,0,1,0,0,0,0 },
	/* 0x1C: */ { 0,1,0,0,0,0,0,0,0,1,0,0,0,0 },
	/* 0x1D: */ { 0,0,0,0,1,0,0,0,0,1,0,0,0,0 },
	/* 0x1E: */ { 0,0,0,1,0,0,0,0,0,1,0,0,0,0 },
	/* 0x1F: */ { 0,0,1,0,0,0,0,0,0,1,0,0,0,0 },
	/* 0x20: */ { 0,0,0,0,0,0,0,0,1,0,0,0,0,0 },
	/* 0x21: */ { 1,0,0,0,0,1,0,0,0,0,1,0,0,0 },
	/* 0x22: */ { 0,0,0,0,1,0,0,0,1,0,0,0,0,0 },
	/* 0x23: */ { 0,0,1,0,0,1,0,0,1,0,0,0,0,0 },
	/* 0x24: */ { 0,1,0,0,0,1,0,0,0,0,1,0,0,0 },
	/* 0x25: */ { 0,0,0,0,0,1,0,0,0,0,1,0,0,0 },
	/* 0x26: */ { 0,1,0,0,0,0,0,0,1,0,0,0,0,0 },
	/* 0x27: */ { 0,0,1,0,0,1,0,0,0,0,1,0,0,0 },
	/* 0x28: */ { 0,1,0,0,1,0,0,1,0,0,1,0,0,0 },
    /* 0x29: */ { 1,0,0,0,0,0,0,1,0,0,1,0,0,0 },
	/* 0x2A: */ { 1,0,0,1,0,0,0,1,0,0,1,0,0,0 },
	/* 0x2B: */ { 1,0,0,0,1,0,0,1,0,0,1,0,0,0 },
	/* 0x2C: */ { 0,1,0,0,0,0,0,1,0,0,1,0,0,0 },
	/* 0x2D: */ { 0,0,0,0,0,0,0,1,0,0,1,0,0,0 },
	/* 0x2E: */ { 0,0,0,1,0,0,0,1,0,0,1,0,0,0 },
	/* 0x2F: */ { 0,0,1,0,0,0,0,1,0,0,1,0,0,0 },
	/* 0x30: */ { 0,0,0,0,0,1,0,0,0,0,0,0,0,0 },
	/* 0x31: */ { 1,0,0,0,0,0,1,0,0,0,1,0,0,0 },
	/* 0x32: */ { 1,0,0,1,0,0,1,0,0,0,1,0,0,0 },
	/* 0x33: */ { 1,0,0,0,0,1,0,0,0,1,0,0,0,0 },
	/* 0x34: */ { 0,1,0,0,0,0,1,0,0,0,1,0,0,0 },
	/* 0x35: */ { 0,0,0,0,0,0,1,0,0,0,1,0,0,0 },
	/* 0x36: */ { 0,0,0,1,0,0,1,0,0,0,1,0,0,0 },
	/* 0x37: */ { 0,0,1,0,0,0,1,0,0,0,1,0,0,0 },
	/* 0x38: */ { 0,1,0,0,1,0,0,0,0,0,1,0,0,0 },
	/* 0x39: */ { 1,0,0,0,0,0,0,0,0,0,1,0,0,0 },
	/* 0x3A: */ { 1,0,0,1,0,0,0,0,0,0,1,0,0,0 },
	/* 0x3B: */ { 1,0,0,0,1,0,0,0,0,0,1,0,0,0 },
	/* 0x3C: */ { 0,1,0,0,0,0,0,0,0,0,1,0,0,0 },
	/* 0x3D: */ { 0,0,0,0,1,0,0,0,0,0,1,0,0,0 },
	/* 0x3E: */ { 0,0,0,1,0,0,0,0,0,0,1,0,0,0 },
	/* 0x3F: */ { 0,0,1,0,0,0,0,0,0,0,1,0,0,0 },
	/* 0x40: */ { 0,1,0,0,1,0,0,0,1,0,0,1,0,0 },
	/* 0x41: */ { 1,0,0,0,0,1,0,0,1,0,0,1,0,0 },
	/* 0x42: */ { 1,0,0,1,0,0,0,0,1,0,0,1,0,0 },
	/* 0x43: */ { 1,0,0,0,1,0,0,0,1,0,0,1,0,0 },
	/* 0x44: */ { 0,1,0,0,0,1,0,0,1,0,0,1,0,0 },
	/* 0x45: */ { 0,0,0,0,0,0,0,0,1,0,0,1,0,0 },
	/* 0x46: */ { 0,0,0,1,0,0,0,0,1,0,0,1,0,0 },
	/* 0x47: */ { 0,0,1,0,0,1,0,0,1,0,0,1,0,0 },
	/* 0x48: */ { 0,1,0,0,1,0,0,1,0,0,0,1,0,0 },
	/* 0x49: */ { 1,0,0,0,0,0,0,1,0,0,0,1,0,0 },
	/* 0x4A: */ { 1,0,0,1,0,0,0,1,0,0,0,1,0,0 },
	/* 0x4B: */ { 1,0,0,0,1,0,0,1,0,0,0,1,0,0 },
	/* 0x4C: */ { 0,1,0,0,0,0,0,1,0,0,0,1,0,0 },
	/* 0x4D: */ { 0,0,0,0,0,0,0,1,0,0,0,1,0,0 },
	/* 0x4E: */ { 0,0,0,1,0,0,0,1,0,0,0,1,0,0 },
	/* 0x4F: */ { 0,0,1,0,0,0,0,1,0,0,0,1,0,0 },
	/* 0x50: */ { 1,0,0,0,0,0,0,0,1,0,0,1,0,0 },
	/* 0x51: */ { 1,0,0,0,0,0,1,0,0,0,0,1,0,0 },
	/* 0x52: */ { 1,0,0,1,0,0,1,0,0,0,0,1,0,0 },
	/* 0x53: */ { 0,0,1,0,0,0,0,0,1,0,0,1,0,0 },
	/* 0x54: */ { 0,1,0,0,0,0,1,0,0,0,0,1,0,0 },
	/* 0x55: */ { 0,0,0,0,0,0,1,0,0,0,0,1,0,0 },
	/* 0x56: */ { 0,0,0,1,0,0,1,0,0,0,0,1,0,0 },
	/* 0x57: */ { 0,0,1,0,0,0,1,0,0,0,0,1,0,0 },
	/* 0x58: */ { 0,1,0,0,1,0,0,0,0,0,0,1,0,0 },
	/* 0x59: */ { 1,0,0,0,0,0,0,0,0,0,0,1,0,0 },
	/* 0x5A: */ { 1,0,0,1,0,0,0,0,0,0,0,1,0,0 },
	/* 0x5B: */ { 1,0,0,0,1,0,0,0,0,0,0,1,0,0 },
	/* 0x5C: */ { 0,1,0,0,0,0,0,0,0,0,0,1,0,0 },
	/* 0x5D: */ { 0,0,0,0,1,0,0,0,0,0,0,1,0,0 },
	/* 0x5E: */ { 0,0,0,1,0,0,0,0,0,0,0,1,0,0 },
	/* 0x5F: */ { 0,0,1,0,0,0,0,0,0,0,0,1,0,0 },
	/* 0x60: */ { 0,1,0,0,1,0,0,0,1,0,0,0,1,0 },
	/* 0x61: */ { 1,0,0,0,0,1,0,0,1,0,0,0,1,0 },
	/* 0x62: */ { 1,0,0,1,0,0,0,0,1,0,0,0,1,0 },
    /* 0x63: */ { 1,0,0,0,1,0,0,0,1,0,0,0,1,0 },
    /* 0x64: */ { 0,1,0,0,0,1,0,0,1,0,0,0,1,0 },
    /* 0x65: */ { 0,0,0,0,0,0,0,0,1,0,0,0,1,0 },
    /* 0x66: */ { 0,1,0,0,0,0,0,0,1,0,0,1,0,0 },
    /* 0x67: */ { 0,0,1,0,0,1,0,0,1,0,0,0,1,0 },
    /* 0x68: */ { 0,1,0,0,1,0,0,1,0,0,0,0,1,0 },
    /* 0x69: */ { 1,0,0,0,0,0,0,1,0,0,0,0,1,0 },
    /* 0x6A: */ { 1,0,0,1,0,0,0,1,0,0,0,0,1,0 },
	/* 0x6B: */ { 1,0,0,0,1,0,0,1,0,0,0,0,1,0 },
    /* 0x6C: */ { 0,1,0,0,0,0,0,1,0,0,0,0,1,0 },
    /* 0x6D: */ { 0,0,0,0,0,0,0,1,0,0,0,0,1,0 },
    /* 0x6E: */ { 0,0,0,1,0,0,0,1,0,0,0,0,1,0 },
    /* 0x6F: */ { 0,0,1,0,0,0,0,1,0,0,0,0,1,0 },
    /* 0x70: */ { 1,0,0,0,0,0,0,0,1,0,0,0,1,0 },
    /* 0x71: */ { 1,0,0,0,0,0,1,0,0,0,0,0,1,0 },
    /* 0x72: */ { 1,0,0,1,0,0,1,0,0,0,0,0,1,0 },
    /* 0x73: */ { 0,0,1,0,0,0,0,0,1,0,0,0,1,0 },
    /* 0x74: */ { 0,1,0,0,0,0,1,0,0,0,0,0,1,0 },
    /* 0x75: */ { 0,0,0,0,0,0,1,0,0,0,0,0,1,0 },
    /* 0x76: */ { 0,0,0,1,0,0,1,0,0,0,0,0,1,0 },
    /* 0x77: */ { 0,0,1,0,0,0,1,0,0,0,0,0,1,0 },
    /* 0x78: */ { 0,1,0,0,1,0,0,0,0,0,0,0,1,0 },
    /* 0x79: */ { 0,0,0,0,1,0,0,1,0,0,1,0,0,0 },
    /* 0x7A: */ { 1,0,0,1,0,0,0,0,0,0,0,0,1,0 },
    /* 0x7B: */ { 1,0,0,0,1,0,0,0,0,0,0,0,1,0 },
    /* 0x7C: */ { 0,1,0,0,0,0,0,0,0,0,0,0,1,0 },
    /* 0x7D: */ { 0,0,0,0,1,0,0,0,0,0,0,0,1,0 },
    /* 0x7E: */ { 0,0,0,1,0,0,0,0,0,0,0,0,1,0 },
    /* 0x7F: */ { 0,0,1,0,0,0,0,0,0,0,0,0,1,0 },
	/* 0x80: */ { 0,1,0,0,1,0,0,0,1,0,0,0,0,1 },
	/* 0x81: */ { 1,0,0,0,0,1,0,0,1,0,0,0,0,1 },
	/* 0x82: */ { 1,0,0,1,0,0,0,0,1,0,0,0,0,1 },
	/* 0x83: */ { 1,0,0,0,1,0,0,0,1,0,0,0,0,1 },
	/* 0x84: */ { 0,1,0,0,0,1,0,0,1,0,0,0,0,1 },
	/* 0x85: */ { 0,0,0,0,0,0,0,0,1,0,0,0,0,1 },
	/* 0x86: */ { 0,0,0,1,0,0,0,0,1,0,0,0,0,1 },
	/* 0x87: */ { 0,0,1,0,0,1,0,0,1,0,0,0,0,1 },
	/* 0x88: */ { 0,1,0,0,1,0,0,1,0,0,0,0,0,1 },
	/* 0x89: */ { 1,0,0,0,0,0,0,1,0,0,0,0,0,1 },
	/* 0x8A: */ { 1,0,0,1,0,0,0,1,0,0,0,0,0,1 },
	/* 0x8B: */ { 1,0,0,0,1,0,0,1,0,0,0,0,0,1 },
	/* 0x8C: */ { 0,1,0,0,0,0,0,1,0,0,0,0,0,1 },
	/* 0x8D: */ { 0,0,0,0,0,0,0,1,0,0,0,0,0,1 },
	/* 0x8E: */ { 0,0,0,1,0,0,0,1,0,0,0,0,0,1 },
	/* 0x8F: */ { 0,0,1,0,0,0,0,1,0,0,0,0,0,1 },
	/* 0x90: */ { 1,0,0,0,0,0,0,0,1,0,0,0,0,1 },
	/* 0x91: */ { 1,0,0,0,0,0,1,0,0,0,0,0,0,1 },
	/* 0x92: */ { 1,0,0,1,0,0,1,0,0,0,0,0,0,1 },
	/* 0x93: */ { 0,0,1,0,0,0,0,0,1,0,0,0,0,1 },
	/* 0x94: */ { 0,1,0,0,0,0,1,0,0,0,0,0,0,1 },
    /* 0x95: */ { 0,0,0,0,0,0,1,0,0,0,0,0,0,1 },
    /* 0x96: */ { 0,0,0,1,0,0,1,0,0,0,0,0,0,1 },
    /* 0x97: */ { 0,0,1,0,0,0,1,0,0,0,0,0,0,1 },
    /* 0x98: */ { 0,1,0,0,1,0,0,0,0,0,0,0,0,1 },
    /* 0x99: */ { 1,0,0,0,0,0,1,0,0,1,0,0,0,0 },
    /* 0x9A: */ { 1,0,0,1,0,0,0,0,0,0,0,0,0,1 },
    /* 0x9B: */ { 1,0,0,0,1,0,0,0,0,0,0,0,0,1 },
    /* 0x9C: */ { 0,1,0,0,0,0,1,0,0,1,0,0,0,0 },
    /* 0x9D: */ { 0,0,0,0,1,0,0,0,0,0,0,0,0,1 },
    /* 0x9E: */ { 0,0,0,1,0,0,0,0,0,0,0,0,0,1 },
    /* 0x9F: */ { 0,0,1,0,0,0,1,0,0,1,0,0,0,0 },
    /* 0xA0: */ { 0,0,0,0,1,0,0,0,1,0,0,0,0,1 },
    /* 0xA1: */ { 1,0,0,0,0,1,0,0,0,0,1,0,0,1 },
    /* 0xA2: */ { 0,1,0,0,0,1,0,0,0,1,0,0,0,0 },
    /* 0xA3: */ { 0,0,0,0,0,1,0,0,1,0,0,0,0,1 },
    /* 0xA4: */ { 0,1,0,0,0,1,0,0,0,0,1,0,0,1 },
    /* 0xA5: */ { 0,0,0,0,0,1,0,0,0,0,1,0,0,1 },
    /* 0xA6: */ { 0,1,0,0,0,0,0,0,1,0,0,0,0,1 },
    /* 0xA7: */ { 0,0,1,0,0,1,0,0,0,0,1,0,0,1 },
    /* 0xA8: */ { 0,1,0,0,1,0,0,1,0,0,1,0,0,1 },
    /* 0xA9: */ { 1,0,0,0,0,0,0,1,0,0,1,0,0,1 },
    /* 0xAA: */ { 1,0,0,1,0,0,0,1,0,0,1,0,0,1 },
    /* 0xAB: */ { 1,0,0,0,1,0,0,1,0,0,1,0,0,1 },
    /* 0xAC: */ { 0,1,0,0,0,0,0,1,0,0,1,0,0,1 },
    /* 0xAD: */ { 0,0,0,0,0,0,0,1,0,0,1,0,0,1 },
    /* 0xAE: */ { 0,0,0,1,0,0,0,1,0,0,1,0,0,1 },
    /* 0xAF: */ { 0,0,1,0,0,0,0,1,0,0,1,0,0,1 },
    /* 0xB0: */ { 0,0,0,0,0,1,0,0,1,0,0,0,0,0 },
    /* 0xB1: */ { 1,0,0,0,0,0,1,0,0,0,1,0,0,1 },
    /* 0xB2: */ { 1,0,0,1,0,0,1,0,0,0,1,0,0,1 },
    /* 0xB3: */ { 0,0,1,0,0,1,0,0,0,1,0,0,0,0 },
    /* 0xB4: */ { 0,1,0,0,0,0,1,0,0,0,1,0,0,1 },
    /* 0xB5: */ { 0,0,0,0,0,0,1,0,0,0,1,0,0,1 },
    /* 0xB6: */ { 0,0,0,1,0,0,1,0,0,0,1,0,0,1 },
    /* 0xB7: */ { 0,0,1,0,0,0,1,0,0,0,1,0,0,1 },
    /* 0xB8: */ { 0,1,0,0,1,0,0,0,0,0,1,0,0,1 },
    /* 0xB9: */ { 1,0,0,0,0,0,0,0,0,0,1,0,0,1 },
    /* 0xBA: */ { 1,0,0,1,0,0,0,0,0,0,1,0,0,1 },
    /* 0xBB: */ { 1,0,0,0,1,0,0,0,0,0,1,0,0,1 },
    /* 0xBC: */ { 0,1,0,0,0,0,0,0,0,0,1,0,0,1 },
    /* 0xBD: */ { 0,0,0,0,1,0,0,0,0,0,1,0,0,1 },
    /* 0xBE: */ { 0,0,0,1,0,0,0,0,0,0,1,0,0,1 },
    /* 0xBF: */ { 0,0,1,0,0,0,0,0,0,0,1,0,0,1 },
    /* 0xC0: */ { 0,1,0,0,0,1,0,0,1,0,0,0,0,0 },
    /* 0xC1: */ { 1,0,0,0,0,1,0,0,0,1,0,0,0,1 },
    /* 0xC2: */ { 1,0,0,1,0,0,1,0,0,1,0,0,0,0 },
    /* 0xC3: */ { 0,0,0,0,1,0,0,0,1,0,0,1,0,0 },
    /* 0xC4: */ { 0,1,0,0,0,1,0,0,0,1,0,0,0,1 },
    /* 0xC5: */ { 0,0,0,0,0,1,0,0,0,1,0,0,0,1 },
    /* 0xC6: */ { 0,0,0,1,0,0,1,0,0,1,0,0,0,0 },
    /* 0xC7: */ { 0,0,1,0,0,1,0,0,0,1,0,0,0,1 },
    /* 0xC8: */ { 0,0,0,0,1,0,0,1,0,0,0,0,0,1 },
    /* 0xC9: */ { 1,0,0,0,0,1,0,0,0,0,0,0,0,1 },
    /* 0xCA: */ { 0,0,0,0,1,0,0,1,0,0,0,1,0,0 },
    /* 0xCB: */ { 0,0,0,0,1,0,0,1,0,0,0,0,0,0 },
    /* 0xCC: */ { 0,1,0,0,0,1,0,0,0,0,0,0,0,1 },
    /* 0xCD: */ { 0,0,0,0,0,1,0,0,0,0,0,0,0,1 },
    /* 0xCE: */ { 0,0,0,0,0,0,1,0,0,1,0,0,0,0 },
    /* 0xCF: */ { 0,0,1,0,0,1,0,0,0,0,0,0,0,1 },
    /* 0xD0: */ { 0,0,0,0,0,1,0,0,1,0,0,1,0,0 },
    /* 0xD1: */ { 1,0,0,0,0,0,1,0,0,1,0,0,0,1 },
    /* 0xD2: */ { 1,0,0,1,0,0,1,0,0,1,0,0,0,1 },
    /* 0xD3: */ { 1,0,0,0,0,1,0,0,1,0,0,0,0,0 },
    /* 0xD4: */ { 0,1,0,0,0,0,1,0,0,1,0,0,0,1 },
    /* 0xD5: */ { 0,0,0,0,0,0,1,0,0,1,0,0,0,1 },
    /* 0xD6: */ { 0,0,0,1,0,0,1,0,0,1,0,0,0,1 },
    /* 0xD7: */ { 0,0,1,0,0,0,1,0,0,1,0,0,0,1 },
    /* 0xD8: */ { 0,1,0,0,1,0,0,0,0,1,0,0,0,1 },
    /* 0xD9: */ { 1,0,0,0,0,0,0,0,0,1,0,0,0,1 },
    /* 0xDA: */ { 1,0,0,1,0,0,0,0,0,1,0,0,0,1 },
    /* 0xDB: */ { 1,0,0,0,1,0,0,0,0,1,0,0,0,1 },
    /* 0xDC: */ { 0,1,0,0,0,0,0,0,0,1,0,0,0,1 },
    /* 0xDD: */ { 0,0,0,0,1,0,0,0,0,1,0,0,0,1 },
    /* 0xDE: */ { 0,0,0,1,0,0,0,0,0,1,0,0,0,1 },
    /* 0xDF: */ { 0,0,1,0,0,0,0,0,0,1,0,0,0,1 },
    /* 0xE0: */ { 0,1,0,0,0,1,0,0,0,0,0,0,1,0 },
    /* 0xE1: */ { 0,0,0,0,0,1,0,0,0,0,0,0,1,0 },
    /* 0xE2: */ { 1,0,0,0,0,1,0,0,0,1,0,0,1,0 },
    /* 0xE3: */ { 0,0,1,0,0,1,0,0,0,0,0,0,1,0 },
    /* 0xE4: */ { 0,1,0,0,0,1,0,0,0,1,0,0,1,0 },
    /* 0xE5: */ { 0,0,0,0,0,1,0,0,0,1,0,0,1,0 },
    /* 0xE6: */ { 0,1,0,0,0,0,0,0,1,0,0,0,1,0 },
    /* 0xE7: */ { 0,0,1,0,0,1,0,0,0,1,0,0,1,0 },
    /* 0xE8: */ { 1,0,0,0,0,1,0,0,0,0,0,0,1,0 },
    /* 0xE9: */ { 1,0,0,0,0,1,0,0,0,0,0,1,0,0 },
    /* 0xEA: */ { 0,0,0,0,1,0,0,1,0,0,1,0,0,1 },
    /* 0xEB: */ { 0,0,0,0,1,0,0,1,0,0,0,0,1,0 },
    /* 0xEC: */ { 0,1,0,0,0,1,0,0,0,0,0,1,0,0 },
    /* 0xED: */ { 0,0,0,0,0,1,0,0,0,0,0,1,0,0 },
    /* 0xEE: */ { 0,0,0,1,0,0,0,0,1,0,0,0,1,0 },
    /* 0xEF: */ { 0,0,1,0,0,1,0,0,0,0,0,1,0,0 },
    /* 0xF0: */ { 0,0,0,0,0,1,0,0,1,0,0,0,1,0 },
    /* 0xF1: */ { 1,0,0,0,0,0,1,0,0,1,0,0,1,0 },
    /* 0xF2: */ { 1,0,0,1,0,0,1,0,0,1,0,0,1,0 },
    /* 0xF3: */ { 0,0,0,0,1,0,0,0,1,0,0,0,1,0 },
    /* 0xF4: */ { 0,1,0,0,0,0,1,0,0,1,0,0,1,0 },
    /* 0xF5: */ { 0,0,0,0,0,0,1,0,0,1,0,0,1,0 },
    /* 0xF6: */ { 0,0,0,1,0,0,1,0,0,1,0,0,1,0 },
    /* 0xF7: */ { 0,0,1,0,0,0,1,0,0,1,0,0,1,0 },
    /* 0xF8: */ { 0,1,0,0,1,0,0,0,0,1,0,0,1,0 },
    /* 0xF9: */ { 1,0,0,0,0,0,0,0,0,1,0,0,1,0 },
    /* 0xFA: */ { 1,0,0,1,0,0,0,0,0,1,0,0,1,0 },
    /* 0xFB: */ { 1,0,0,0,1,0,0,0,0,1,0,0,1,0 },
    /* 0xFC: */ { 0,1,0,0,0,0,0,0,0,1,0,0,1,0 },
    /* 0xFD: */ { 0,0,0,0,1,0,0,0,0,1,0,0,1,0 },
    /* 0xFE: */ { 0,0,0,1,0,0,0,0,0,1,0,0,1,0 },
	/* 0xFF: */ { 0,0,1,0,0,0,0,0,0,1,0,0,1,0 }
};


typedef struct
{
	int		dwForce_9;
	int		dwForce_10;
	int		dwForce_9_mask;
	int		dwForce_10_mask;

	int		iDSV;
} WEAKPATTERNDESCRIPTOR;

WEAKPATTERNDESCRIPTOR	weak_patterns[65536];



bool IsValidPattern(char* lpbIn,int dwLength,int dwFlags=0)
{
	int		dwZ=2;
	int	i;
//	char	debug[1000];

//	ZeroMemory(debug,1000);
//	for (i=0;i<(int)dwLength;debug[i++]=48+lpbIn[i]);

	for (i=0;i<(int)dwLength;i++)
	{
		if (lpbIn[i]==0)
		{
			dwZ++;
		}
		else
		{
			if (dwZ<2)
			{
				return false;
			}
			dwZ=0;
		}
		if (dwZ>10) return false;
		if (dwZ>9)
		{
			if ((i>=14)&&(i<27)&&(dwFlags&IVP_NO10ZEROS)) return false;
		}
	}
	return true;
}

bool GetWeakPatternDescriptor(int iPattern, WEAKPATTERNDESCRIPTOR *lpwpd)
{
	int		dwValids[4];
	int		i,j,k,l,m,dwVal,dwForce_9,dwForce_10,dwOnes,iDSV,iAdd,dwForce_9_mask,dwForce_10_mask;
	char	channel_bits[41];
	 
	dwVal=iPattern;
	for (m=0;m<4;m++)
	{
		dwValids[m]=0;
	};
	for (l=0;l<2;l++)
	{
		for (j=0;j<14;j++)
		{
			channel_bits[j+17*l]=EFM_table[dwVal%256][j];
			channel_bits[17*(1-l)+j]=EFM_table[dwVal/256][j];
		}
		for (j=0;j<4;j++)
		{
			for (k=0;k<3;k++)
			{
				channel_bits[14+k]=merging_bits[j][k];
			}
			for (m=0;m<2;m++)
			{
				if (IsValidPattern(channel_bits,31,m*IVP_NO10ZEROS))
				{
					dwValids[2*m+l]|=(1<<j);
				}
			}
		}
	}

// Bösartigkeit der Muster bestimmen
	dwForce_10=0; dwForce_9=0;
	dwForce_10_mask=0; dwForce_9_mask=0;
	if ((dwValids[0]==1)||(!(dwValids[0]&1)))
	{
		dwForce_10++;
		dwForce_10_mask|=1;
	}
	if ((dwValids[1]==1)||(!(dwValids[1]&1)))
	{
		dwForce_10++;
		dwForce_10_mask|=2;
	}
	if ((dwValids[2]==1)||(!(dwValids[2]&1)))
	{
		dwForce_9++;
		dwForce_9_mask|=1;
	}
	if ((dwValids[3]==1)||(!(dwValids[3]&1)))
	{
		dwForce_9++;
		dwForce_9_mask|=2;
	}
			
	dwOnes=0;
	iDSV=0;

	if (dwForce_9==2)
	{
				// F: x/2
		for (j=0;j<14;j++)
		{
			channel_bits[j]=EFM_table[dwVal%256][j];			
			channel_bits[j+17]=EFM_table[dwVal/256][j];
		}
		for (m=0;m<2;m++)
		{
			if (dwValids[2+m]==1)
			{
				for (j=0;j<3;j++)
				{
					channel_bits[j+14+17*m]=merging_bits[0][j];
				}
			}
			else
			{
				for (j=0;j<3;j++)
				{
					channel_bits[j+14+17*m]=merging_bits[2][j];
				}
			}
		}
		iAdd=1;
		for (j=0;j<34;j++)
		{
			dwOnes+=channel_bits[j];
			if (channel_bits[j]) iAdd*=-1;
			iDSV+=iAdd;
		}
	}
	if (dwOnes%2) iDSV=0;

	if (lpwpd)
	{
		lpwpd->dwForce_9=dwForce_9;
		lpwpd->dwForce_10=dwForce_10;
		lpwpd->dwForce_10_mask=dwForce_10_mask;
		lpwpd->dwForce_9_mask=dwForce_9_mask;
		lpwpd->iDSV=iDSV;
	}

	return true;
}

int main(int argc, char* argv[])
{
	int		i,j,k,l,m,dwVal,dwForce_9,dwForce_10,dwOnes,iDSV,iAdd;
	int		iMinDSV=0;
	int		iMaxDSV=100;
	int		iForceMin=0;
	int		iForceMax=10;
	int		iForceSumMin=0;
	int		iForceSumMax=4;
	int		iDup=1;
	int		iGap=2;
	int		iCreate=0;
	char*	lpDest=NULL;
	char*	lpSource=NULL;
	FILE*	lpfOut;
	FILE*	lpfIn;
	int		dwValids[4];
	char	channel_bits[41];
	bool	bGoodPattern=true;
	int		iSector=0;
	union
	{
		char	sector[2448];
		uint16_t	sector_i[1224];
	};
	int		pattern_list[100];
	int		iUseList=0;
	int		iSectorSize=0;
	bool	bRegular;
	int		iForceGrade_9;
	int		iForceGrade_10;
	int		iOccurences[65536];
	bool	bOften;
	int		dwForce9,dwForce10;
	char	cWI;

	WEAKPATTERNDESCRIPTOR	wpd;

	if (argc==1)
	{
		printf ("\nNo parameters entered!\n");
		return 0;
	}
	printf("\n");

	for (i=2;i<=argc;i++)
	{
		if (!strncmp(argv[i-1],"-minDSV=",8))
		{
			iMinDSV=atoi(&(argv[i-1][8]));
		}
		if (!strncmp(argv[i-1],"-maxDSV=",8))
		{
			iMaxDSV=atoi(&(argv[i-1][8]));
		}
		if (!strncmp(argv[i-1],"-forcemin=",10))
		{
			iForceMin=atoi(&(argv[i-1][10]));
		}
		if (!strncmp(argv[i-1],"-forcemax=",10))
		{
			iForceMax=atoi(&(argv[i-1][10]));
		}
		if (!strncmp(argv[i-1],"-forceminsum=",13))
		{
			iForceSumMin=atoi(&(argv[i-1][13]));
		}
		if (!strncmp(argv[i-1],"-forcemaxsum=",13))
		{
			iForceSumMax=atoi(&(argv[i-1][13]));
		}
		if (!strncmp(argv[i-1],"-gap=",5))
		{
			iGap=atoi(&(argv[i-1][5]));
		}
		if (!strncmp(argv[i-1],"-create",7))
		{
			iCreate=1;
		}
		if (!strncmp(argv[i-1],"-dup=",5))
		{
			iDup=atoi(&(argv[i-1][5]));
		}
		if (!strncmp(argv[i-1],"-sector-size=",13))
		{
			iSectorSize=atoi(&(argv[i-1][13]));
		}
		if (!strncmp(argv[i-1],"-list=",6))
		{
			iUseList=1;
			printf("use know pattern list\n");
			if (!strncmp(&(argv[i-1][6]),"sd2old",6))
			{
				printf("create old SD2 patterns\n");
				pattern_list[0]=7;
				pattern_list[1]=0x5859;
				pattern_list[2]=0x5A5F;
				pattern_list[3]=0x7D65;
				pattern_list[4]=0x082A;
				pattern_list[5]=0x3916;
				pattern_list[6]=0x3F65;
				pattern_list[7]=0x2D9E;
			}
			if (!strncmp(&(argv[i-1][6]),"sd251+",6))
			{
				printf("create old SD2.51.021+ patterns\n");
				pattern_list[0]=9;
				pattern_list[1]=0x5859;
				pattern_list[2]=0x5A5F;
				pattern_list[3]=0x7D65;
				pattern_list[4]=0x082A;
				pattern_list[5]=0x3916;
				pattern_list[6]=0x3F65;
				pattern_list[7]=0x2D9E;
				pattern_list[8]=0x1212;
				pattern_list[9]=0x14B9;
			}
		}
		if (!strncmp(argv[i-1],"-out=",5))
		{
			lpDest=(char*)malloc(1+strlen(&(argv[i-1][5])));
			strcpy(lpDest,&(argv[i-1][5]));
		}
		if (!strncmp(argv[i-1],"-in=",4))
		{
			if (!strlen(&(argv[i-1][4])))
			{
				lpfIn=stdin;
				lpSource=(char*)malloc(256);
				fgets(lpSource,sizeof(lpSource),stdin);
				printf("source file:");
				printf(lpSource);
				printf("\n\n");
				lpfIn=fopen(lpSource,"rb");
			}
			else
			{
				lpSource=(char*)malloc(1+strlen(&(argv[i-1][4])));
				strcpy(lpSource,&(argv[i-1][4]));
				printf("source file: ");
				printf(lpSource);
				printf("\n\n");
				lpfIn=fopen(lpSource,"rb");
			}
		}
	}

	if (iCreate)
	{
		// create test file

		if (!iUseList)
		{
			printf("DSV for 12 bytes:\n");
			printf("  min: %d\n",iMinDSV);
			printf("  max: %d\n",iMaxDSV);
			printf("create test file: %s\n",(iCreate%2)?"yes":"no");
			printf("merging bits must be forced for\n");
			printf("  a maximum of  9 consecutive zeros: %d/2 bytes\n",(iForceMin%4));
			printf("  a maximum of 10 consecutive zeros: %d/2 bytes\n",(iForceMin>>2));
			printf("merging bits must be forced for not more than \n");
			printf("  a maximum of  9 consecutive zeros: %d/2 bytes\n",(iForceMax%4));
			printf("  a maximum of 10 consecutive zeros: %d/2 bytes\n",(iForceMax>>2));
			printf("Sum of force values:\n");
			printf("  min: %d   max: %d\n",iForceSumMin,iForceSumMax);
		}

		printf("Every appropriate weak sector is written %d times\n",iDup);
		printf("gap after weak sectors: %d sectors\n",iGap);
		if (lpDest) printf("output file: %s\n",lpDest);
		lpfOut=fopen(lpDest,"wb");
		printf("\nCreating list of weak sectors...\n");
		if (!iUseList)
		{	
			for (i=0;i<65536;i++)
			{
				bGoodPattern=true;
				if ((i/256)<=(i%256))
				{
					dwVal=i;
		
					GetWeakPatternDescriptor(dwVal,&wpd);

					dwForce_10=wpd.dwForce_10;
					dwForce_9=wpd.dwForce_9;
					iDSV=wpd.iDSV;
					if (dwForce_10<iForceMin>>2) bGoodPattern=false;
					if (dwForce_9<iForceMin%4) bGoodPattern=false;
					if (dwForce_10>iForceMax>>2) bGoodPattern=false;
					if (dwForce_9>iForceMax%4) bGoodPattern=false;

					if (dwForce_10+dwForce_9>iForceSumMax) bGoodPattern=false;
					if (dwForce_10+dwForce_9<iForceSumMin) bGoodPattern=false;

					if (abs(iDSV*6)<iMinDSV) bGoodPattern=false;
					if (abs(iDSV*6)>iMaxDSV) bGoodPattern=false;
	
					if (bGoodPattern)
					{
						printf ("%6d: %04X, %d / %d, DSV: %4d \n",iSector*(iDup+iGap),dwVal,dwForce_9,dwForce_10,6*iDSV);
						for (m=0;m<1024;m++)
						{
							sector[16+2*m]=dwVal%256;
							sector[16+2*m+1]=dwVal/256;
						}
						descramble_L2((unsigned char*)sector);
						for (m=0;m<iDup;m++)
						{
							fwrite(&(sector[16]),2048,1,lpfOut);
						}
						memset(&(sector[16]),0,2048);
		
						for (m=0;m<iGap;m++)
						{
							fwrite(&(sector[16]),2048,1,lpfOut);
						}

						iSector++;
					}
				}
			}
		}
		else
		{
			for (j=1;j<=pattern_list[0];j++)
			{
				dwVal=pattern_list[j];
				printf ("%6d: %04X\n",iSector*(iDup+iGap),dwVal);
	
				for (m=0;m<1024;m++)
				{
					sector[16+2*m]=dwVal%256;
					sector[16+2*m+1]=dwVal/256;
				}
				descramble_L2((unsigned char*)sector);
				for (m=0;m<iDup;m++)
				{
					fwrite(&(sector[16]),2048,1,lpfOut);
				}
				memset(&(sector[16]),0,2048);
	
				for (m=0;m<iGap;m++)
				{
					fwrite(&(sector[16]),2048,1,lpfOut);
				}
				iSector++;
			}

		}
		fclose(lpfOut);
    
		free(lpDest);
	}
	else
	{
		// reading a file

		for (i=0;i<65536;i++)
		{
			GetWeakPatternDescriptor(i,&(weak_patterns[i]));
		}
	
		iSector=0;
		while (fread(sector,iSectorSize,1,lpfIn))
		{
			if (iSector==826)
			{
				printf("");

			}
			if (iSectorSize==2048)
			{
				memmove(&(sector[16]),&(sector[0]),2048);
			}
			scramble_L2((unsigned char*)sector);

			memset(iOccurences,0,sizeof(iOccurences));
			
			// regular pattern?
			
			for (i=0;i<1024;i++)
			{
				l=sector_i[8+i];
				iOccurences[l%65536]++;
			}

			// most often occuring pattern

			do
			{
				bOften=false;

/*			The following ASM piece is 30% faster than this:

				k=0;
				for (i=0;i<65536;i++)
				{
					if (iOccurences[i]>iOccurences[k]) k=i;
				}
*/
			
// plain CPU:
				k=(int)&(iOccurences[0]);

				#ifdef _MSC_VER
				asm
				{
					pushad
					mov  esi, k
					mov  eax, 0
					mov  ecx, 65536
					mov  edx, [esi+4*eax]
				_vgl:	
					dec  ecx
					js   _ende
					mov  ebx, [esi+4*ecx]
					
					cmp  ebx, edx
					jbe  _vgl
					mov  eax, ecx
					mov  edx, [esi+4*eax]
					jmp  _vgl
				_ende:
					mov  k, eax
					popad
				}
				#else
				asm
				(
					"pushad;"
					"mov  esi, %0;"
					"mov  eax, 0;"
					"mov  ecx, 65536;"
					"mov  edx, [esi+4*eax];"
				"_vgl:"	
					"dec  ecx;"
					"js   _ende;"
					"mov  ebx, [esi+4*ecx];"
					
					"cmp  ebx, edx;"
					"jbe  _vgl;"
					"mov  eax, ecx;"
					"mov  edx, [esi+4*eax];"
					"jmp  _vgl;"
				"_ende:"
					"mov  %0, eax;"
					"popad" : "=a" (k)
				);
				#endif

				if (iOccurences[k]>30)
				{
					if (weak_patterns[k].dwForce_9+weak_patterns[k].dwForce_10>=3)
					{
						if (weak_patterns[k].iDSV>=5) cWI='w'; else cWI=' ';
					}
					else
						if (weak_patterns[k].dwForce_9+weak_patterns[k].dwForce_10>=2)
						{
							cWI='w';
						}
						else cWI=' ';

					printf("Sector %6d: Occurence of pattern %04X (%d/%d/%3d): %4d times => %c",iSector,k,
						weak_patterns[k].dwForce_10,weak_patterns[k].dwForce_9,weak_patterns[k].iDSV,
						iOccurences[k],cWI);
					printf("\n");

					bOften=true;
					iOccurences[k]=0;
				}
			} while (bOften);
	
			// intensiv
			dwForce_9=0;dwForce_10=0;
			for (i=0;i<2046;i++)
			{
				j=(uint8_t)sector[i]+256*(uint8_t)sector[i+1];
				wpd=(weak_patterns[j]);
				dwForce_9+=(wpd.dwForce_9_mask&1);
				dwForce_10+=(wpd.dwForce_10_mask&1);
				
			}

			if ((dwForce_9>950)||(dwForce_10>850))
			{
				printf("Sector %6d: F: %d / %d\n",iSector,dwForce_9,dwForce_10);

			}
		

			
			iSector++;

			if (!(iSector%512))
			{
				fprintf(stderr,"MBytes done: %4d\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",(iSector*iSectorSize)>>20);
			}
		}

		printf("MBytes done: %4d",(iSector*iSectorSize)>>20);
		fclose(lpfIn);
		
	}


	return 0;
}
