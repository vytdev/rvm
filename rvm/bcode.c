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
  return memcmp(buf, RMAGIC, 4) == 0;
}


/* This is where the start of the actual descriptor-based bytecode
   parser. */

typedef struct RvmSec {
  rvm_shdr  sechdr;
  char      *payload;
  uvar      pos;
  bool      is_used;
} RvmSec;

typedef struct Rvm {
  u64     size;
  rvmhdr  hdr;
  /* Section infos. */
  RvmSec  *scl;
  uvar    scl_len;
  uvar    scl_alloc;
  /* String table. */
  char    *strtab;
  uvar    str_size;
  uvar    str_alloc;
} Rvm;


Rvm *rvm_open(void) {
  Rvm *rd = (Rvm*)malloc(sizeof(Rvm));
  if (!rd)
    return NULL;
  /* Allocate the section list. */
  RvmSec *scl = (RvmSec*)malloc(sizeof(RvmSec) * 8);
  if (!scl) {
    free(rd);
    return NULL;
  }
  rd->scl       = scl;
  rd->scl_len   = 0;
  rd->scl_alloc = 8;
  for (uvar i = 0; i < 8; i++) {
    rd->scl[i].is_used = false;
    rd->scl[i].payload = NULL;
    rd->scl[i].sechdr.name = 0;
  }
  /* Allocate the string table. */
  char *strtab = (char*)malloc(64);
  if (!strtab) {
    free(rd);
    free(scl);
    return NULL;
  }
  memset(strtab, 0, 64); /* Clear the table. */
  rd->strtab    = strtab;
  rd->str_size  = 1;              /* Empty str at ndx=0 */
  rd->str_alloc = 64;
  /* Other attributes. */
  rd->size   = 64;
  memset(&rd->hdr, 0, sizeof(rvmhdr));
  return rd;
}


void rvm_close(Rvm *rd) {
  if (!rd)
    return;
  if (rd->scl) {
    for (uvar i = 0; i < rd->scl_len; i++)
      free(rd->scl->payload);
    free(rd->scl);
  }
  if (rd->strtab)
    free(rd->strtab);
  free(rd);
}


static bool resize_strtab(Rvm *rd, uvar new_sz) {
  if (new_sz <= rd->str_alloc)
    return false;
  char *new_buf = (char*)realloc(rd->strtab, new_sz);
  if (!new_buf)
    return false;
  memset(new_buf + rd->str_alloc, 0, new_sz - rd->str_alloc);
  rd->strtab = new_buf;
  rd->str_alloc = new_sz;
  return true;
}


uint32_t rvm_addstr(Rvm *rd, char *str) {
  if (!rd || !str)
    return MAX_U32;
  uvar len = strlen(str);
  if (len == 0)
    return 0;
  /* Try to re-allocate the string table, if necessary. */
  uvar newsz = rd->str_size + len + 1;
  if (newsz >= rd->str_alloc) {
    uvar new_alloc = rd->str_alloc;
    while (new_alloc <= newsz)
      new_alloc *= 2;
    if (!resize_strtab(rd, new_alloc))
      return MAX_U32;
  }
  /* Return the created str index. */
  uvar currsz = rd->str_size;
  memcpy(rd->strtab + currsz, str, len);
  rd->str_size = newsz;
  return currsz;
}


uint32_t rvm_addstr_nodup(Rvm *rd, char *str) {
  if (!rd || !str)
    return MAX_U32;
  /* Check if the string is already allocated. */
  uvar i = 0;
  while (i < rd->str_size) {
    if (strcmp(&rd->strtab[i], str) == 0)
      return i;
    i += strlen(&rd->strtab[i]) + 1;
  }
  /* Allocate a new string instead. */
  return rvm_addstr(rd, str);
}


rvmhdr rvm_gethdr(Rvm *rd) {
  if (!rd)
    return (rvmhdr){0};
  return rd->hdr;
}


void rvm_setabi(Rvm *rd, uint16_t abi_ver) {
  if (!rd)
    return;
  rd->hdr.abi_ver = abi_ver;
}


void rvm_settype(Rvm *rd, uint8_t type) {
  if (!rd)
    return;
  rd->hdr.type = type;
}


void rvm_setflags(Rvm *rd, uint8_t flags) {
  if (!rd)
    return;
  rd->hdr.flags = flags;
}


/* Returns true if the given strndx is valid. */
static inline bool validate_strndx(char *strtab, uvar sz, uint32_t strndx) {
  if (strndx >= sz)
    return false;
  for (uint32_t i = strndx; i < sz; i++)
    if (strtab[i] == '\0')
      return true;
  return false;
}


bool rvm_load(Rvm *rd, char *img, uvar size) {
  if (!rd || !img || size < 64)
    return false;
  /* Check the magic number. */
  if (!check_magic(img))
    return false;
  rvmhdr hdr = parse_rvmhdr(img);
  rd->hdr = hdr;
  /* Check the strtab. */
  if (hdr.stroff >= size)
    return false;
  #define check_str(n) do { \
      if (!validate_strndx(img + hdr.stroff, size - hdr.stroff, (n))) \
        return false; \
    } while (0)
  #define getstr(n) \
    (img + hdr.stroff + (n))
  /* Load the sections. */
  if (hdr.shoff >= size || hdr.shoff + hdr.shnum * 32 > size)
    return false;
  for (uvar i = 0; i < hdr.shnum; i++) {
    rvm_shdr curr = parse_rvm_shdr(img + hdr.shoff + i * 32);
    if (curr.offset >= size || curr.offset + curr.size > size)
      return false;
    check_str(curr.name);
    RvmSec *sec = rvm_makesec(rd, getstr(curr.name), curr.size);
    if (!sec)
      return false;
    if (!rvm_write(sec, img + curr.offset, curr.size))
      return false;
    rvm_pseek(sec, 0, RSEEK_SET);
    rvm_setentcnt(sec, curr.entcnt);
  }
  #undef check_str
  #undef getstr
  return true;
}


/*======================================================================*
 *
 *            SECTION MANAGEMENT
 *
 *======================================================================*/

RvmSec *rvm_getsec(Rvm *rd, char *name) {
  if (!rd || !name)
    return NULL;
  for (uvar i = 0; i < rd->scl_alloc; i++) {
    if (rd->scl[i].is_used && strcmp(
        &rd->strtab[rd->scl[i].sechdr.name],
        name) == 0)
      return &rd->scl[i];
  }
  return NULL;
}


static inline bool resize_double_scl(Rvm *rd) {
  uvar new_alloc = rd->scl_alloc * 2;
  RvmSec *new_arr = (RvmSec*)realloc(rd->scl, sizeof(RvmSec) * new_alloc);
  if (!new_arr)
    return false;
  for (uvar i = rd->scl_alloc; i < new_alloc; i++) {
    new_arr[i].is_used = false;
    new_arr[i].payload = NULL;
    new_arr[i].sechdr.name = 0;
  }
  rd->scl = new_arr;
  rd->scl_alloc = new_alloc;
  return true;
}


RvmSec *rvm_makesec(Rvm *rd, char *name, uvar size) {
  if (!rd || !name)
    return NULL;
  /* Check if the section does already exist. */
  if (rvm_getsec(rd, name))
    return NULL;
  /* Re-allocate the section list if needed. */
  if (rd->scl_len >= rd->scl_alloc)
    if (!resize_double_scl(rd))
      return NULL;
  RvmSec *target = NULL;
  for (uvar i = 0; i < rd->scl_alloc; i++) {
    if (rd->scl[i].is_used)
      continue;
    target = &rd->scl[i];
    break;
  }
  /* Setup the section payload. */
  RvmSec sec;
  sec.is_used = true;
  sec.payload = NULL;
  sec.pos = 0;
  if (size > 0) {
    sec.payload = (char*)malloc(size);
    if (!sec.payload)
      return NULL;
  }
  /* Setup the section header. */
  uint32_t strndx = rvm_addstr(rd, name);
  if (strndx == MAX_U32) {
    if (sec.payload)
      free(sec.payload);
    return NULL;
  }
  sec.sechdr.name   = strndx;
  sec.sechdr.offset = 0;
  sec.sechdr.size   = size;
  sec.sechdr.entcnt = 0;
  *target = sec;
  return target;
}


void rvm_rmsec(Rvm *rd, char *name) {
  if (!rd || !name)
    return;
  for (uvar i = 0; i < rd->scl_alloc; i++) {
    if (strcmp(
        &rd->strtab[rd->scl[i].sechdr.name],
        name) != 0)
      continue;
    rd->scl[i].is_used = false;
    rd->scl_len--;
    break;
  }
}


rvm_shdr rvm_getshdr(RvmSec *sec) {
  if (!sec)
    return (rvm_shdr){0};
  return sec->sechdr;
}


void rvm_setentcnt(RvmSec *sec, uint64_t entcnt) {
  if (!sec)
    return;
  sec->sechdr.entcnt = entcnt;
}


void rvm_pseek(RvmSec *sec, ivar offst, int seek) {
  if (!sec)
    return;
  uvar newpos = 0;
  if (seek == RSEEK_SET)
    newpos = offst;
  else if (seek == RSEEK_CURR)
    newpos = sec->pos + offst;
  else if (seek == RSEEK_END)
    newpos = sec->sechdr.size - offst;
  if (newpos > sec->sechdr.size)
    return;
  sec->pos = newpos;
}


uvar rvm_ptell(RvmSec *sec) {
  return sec->pos;
}


uvar rvm_getsize(RvmSec *sec) {
  return sec->sechdr.size;
}


bool rvm_resize(RvmSec *sec, uvar newsz) {
  if (!sec)
    return false;
  if (sec->sechdr.size == newsz)
    return true;
  /* The payload is de-allocated. */
  if (newsz == 0) {
    if (sec->payload) {
      free(sec->payload);
      sec->payload = NULL;
      sec->pos = 0;
      sec->sechdr.size = 0;
    }
    return true;
  }
  /* Try to re-allocate the payload. */
  char *newbuf = NULL;
  if (!sec->payload)
    newbuf = (char*)malloc(newsz);
  else
    newbuf = (char*)realloc(sec->payload, newsz);
  if (!newbuf)
    return false;
  sec->payload = newbuf;
  sec->sechdr.size = newsz;
  /* Update the pos. */
  if (sec->pos > newsz)
    sec->pos = newsz;
  return true;
}


bool rvm_read(RvmSec *sec, void *buf, uvar len) {
  if (!sec || !buf)
    return false;
  if (len == 0)
    return true;
  uvar start = sec->pos;
  uvar end = start + len;
  uvar size = sec->sechdr.size;
  if (!sec->payload || start >= size || end > size)
    return false;
  char *arr = buf;
  for (uvar i = 0; i < len; i++)
    arr[i] = sec->payload[start + i];
  sec->pos = end;
  return true;
}


bool rvm_write(RvmSec *sec, void *buf, uvar len) {
  if (!sec || !buf)
    return false;
  if (len == 0)
    return true;
  uvar start = sec->pos;
  uvar end = start + len;
  uvar size = sec->sechdr.size;
  if (!sec->payload || start >= size || end > size)
    return false;
  char *arr = buf;
  for (uvar i = 0; i < len; i++)
    sec->payload[start + i] = arr[i];
  sec->pos = end;
  return true;
}
