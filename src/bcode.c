#include "bcode.h"
#include "rvmbits.h"
#include "config.h"
#include "codec.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


rvmhdr parse_rvmhdr(char *buf) {
  rvmhdr hdr;
  hdr.ident[RHM_0] = buf[0];
  hdr.ident[RHM_1] = buf[1];
  hdr.ident[RHM_2] = buf[2];
  hdr.ident[RHM_3] = buf[3];
  hdr.abi_ver  = read16(buf+4);
  hdr.type     = read8(buf+6);
  hdr.flags    = read8(buf+7);
  hdr.entryp   = read64(buf+8);
  hdr.shoff    = read64(buf+16);
  hdr.shnum    = read64(buf+24);
  hdr.stroff   = read64(buf+32);
  return hdr;
}

rvm_shdr parse_rvm_shdr(char *buf) {
  rvm_shdr shdr;
  shdr.name    = read32(buf+0);
  shdr.offset  = read64(buf+8);
  shdr.size    = read64(buf+16);
  shdr.entcnt  = read64(buf+24);
  return shdr;
}

rvm_symb parse_rvm_symb(char *buf) {
  rvm_symb sym;
  sym.name     = read32(buf+0);
  sym.type     = read8(buf+4);
  sym.bind     = read8(buf+5);
  sym.value    = read64(buf+8);
  return sym;
}

rvm_reloc parse_rvm_reloc(char *buf) {
  rvm_reloc rel;
  rel.offset   = read64(buf+0);
  rel.addend   = read64(buf+8);
  rel.name     = read32(buf+16);
  rel.nsrc     = read8(buf+20);
  rel.stype    = read8(buf+21);
  return rel;
}

void write_rvmhdr(char *buf, rvmhdr hdr) {
  buf[0] = hdr.ident[RHM_0];
  buf[1] = hdr.ident[RHM_1];
  buf[2] = hdr.ident[RHM_2];
  buf[3] = hdr.ident[RHM_3];
  write16(buf+4,  hdr.abi_ver);
  write8(buf+6,   hdr.type);
  write8(buf+7,   hdr.flags);
  write64(buf+8,  hdr.entryp);
  write64(buf+16, hdr.shoff);
  write64(buf+24, hdr.shnum);
  write64(buf+32, hdr.stroff);
}

void write_rvm_shdr(char *buf, rvm_shdr shdr) {
  write32(buf+0,  shdr.name);
  write64(buf+8,  shdr.offset);
  write64(buf+16, shdr.size);
  write64(buf+24, shdr.entcnt);
}

void write_rvm_symb(char *buf, rvm_symb sym) {
  write32(buf+0,  sym.name);
  write8(buf+4,   sym.type);
  write8(buf+5,   sym.bind);
  write64(buf+8,  sym.value);
}

void write_rvm_reloc(char *buf, rvm_reloc rel) {
  write64(buf+0,  rel.offset);
  write64(buf+8,  rel.addend);
  write32(buf+16, rel.name);
  write8(buf+20,  rel.nsrc);
  write8(buf+21,  rel.stype);
}

bool check_magic(char *buf) {
  return memcmp(buf, RMAGIC, RHM_NI) == 0;
}



typedef struct ByteCode {
  char   *buf;
  uvar   len;
  rvmhdr hdr;
} ByteCode;


ByteCode *bc_open(void *buf, uvar len) {
  if (!buf || len < 64)
    return NULL;
  /* Some validations. */
  if (!check_magic(buf))
    return NULL;
  rvmhdr hdr = parse_rvmhdr(buf);
  if (hdr.shoff >= len || hdr.shoff + hdr.shnum * 32 > len)
    return NULL; /* shtab */
  if (hdr.stroff >= len)
    return NULL; /* strtab */
  /* Setup the ByteCode struct. */
  ByteCode *bd = (ByteCode*)malloc(sizeof(ByteCode));
  if (!bd)
    return NULL;
  bd->buf = buf;
  bd->len = len;
  bd->hdr = hdr;
  return bd;
}


void bc_close(ByteCode *bd) {
  if (bd)
    free(bd);
}


rvmhdr bc_gethdr(ByteCode *bd) {
  if (!bd)
    return (rvmhdr){0};
  return bd->hdr;
}


char *bc_getstr(ByteCode *bd, uint32_t strndx) {
  if (!bd)
    return NULL;
  for (uint64_t i = bd->hdr.stroff + strndx; i < bd->len; i++)
    if (bd->buf[i] == '\0')
      return bd->buf + bd->hdr.stroff + strndx;
  return NULL;
}


void *bc_getsect(ByteCode *bd, char *name) {
  char *shtab = bd->buf + bd->hdr.shoff;
  for (uint64_t i = 0; i < bd->hdr.shnum; i++) {
    char *sect = bc_getstr(bd, read32(shtab));
    if (!sect)
      continue;
    if (strcmp(sect, name) == 0)
      return shtab;
    shtab += 32;
  }
  return NULL;
}


void *bc_getpload(ByteCode *bd, rvm_shdr shdr) {
  if (!bd)
    return NULL;
  if (shdr.offset >= bd->len || shdr.size > bd->len || shdr.offset + shdr.size > bd->len)
    return NULL;
  return bd->buf + shdr.offset;
}
