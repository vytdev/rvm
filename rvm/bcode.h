#ifndef RVM_BCODE_H_
#define RVM_BCODE_H_
#include "rvmbits.h"
#include "config.h"
#include <stdint.h>
#include <stdbool.h>


/* Parse rvmhdr from a buffer. (consumes 64 bytes) */
rvmhdr parse_rvmhdr(char *buf);

/* Parse rvm_shdr from a buffer. (consumes 32 bytes) */
rvm_shdr parse_rvm_shdr(char *buf);

/* Parse rvm_symb from a buffer. (consumes 16 bytes) */
rvm_symb parse_rvm_symb(char *buf);

/* Parse rvm_reloc from a buffer. (consumes 24 bytes) */
rvm_reloc parse_rvm_reloc(char *buf);

/* Dump rvmhdr into a buffer. (uses 64 bytes) */
void write_rvmhdr(char *buf, rvmhdr hdr);

/* Dump rvm_shdr into a buffer. (uses 32 bytes) */
void write_rvm_shdr(char *buf, rvm_shdr shdr);

/* Dump rvm_symb into a buffer. (uses 16 bytes) */
void write_rvm_symb(char *buf, rvm_symb sym);

/* Dump rvm_reloc into a buffer. (uses 24 bytes) */
void write_rvm_reloc(char *buf, rvm_reloc rel);

/* Check if the magic number matches RVM's. (uses 4 bytes) */
bool check_magic(char *buf);


/* Opaque handle for bytecode deserialisation. */
typedef struct ByteCode ByteCode;

/* Opens a bytecode descriptor. */
ByteCode *bc_open(void *buf, uvar len);

/* Closes a bytecode descriptor. */
void bc_close(ByteCode *bd);

/* Returns the cached image header. */
rvmhdr bc_gethdr(ByteCode *bd);

/* Returns a ptr to the strtab entry of strndx. Can be NULL. */
char *bc_getstr(ByteCode *bd, uint32_t strndx);

/* Returns a ptr to the shdr of the given section. Can be NULL. */
void *bc_getsect(ByteCode *bd, char *name);

/* Returns a ptr to the payload of the given shdr. Can be NULL. */
void *bc_getpload(ByteCode *bd, rvm_shdr shdr);

#endif // RVM_BCODE_H_
