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



/* RVM stream descriptor. */
typedef struct Rvm    Rvm;
typedef struct RvmSec RvmSec;

/* Create a new RVM descriptor. */
Rvm *rvm_open(void);

/* Close an RVM descriptor. */
void rvm_close(Rvm *rd);

/* Adds a new string into the string table. Returns MAX_U32 when fails. */
uint32_t rvm_addstr(Rvm *rd, char *str);

/* Like rvm_addstr(), but avoids duplicates. */
uint32_t rvm_addstr_nodup(Rvm *rd, char *str);

/* Get the bytecode header. */
rvmhdr rvm_gethdr(Rvm *rd);

/* Set the ABI version for the bytecode. */
void rvm_setabi(Rvm *rd, uint16_t abi_ver);

/* Set the type of the bytecode. */
void rvm_settype(Rvm *rd, uint8_t type);

/* Set the type flags of the flags. */
void rvm_setflags(Rvm *rd, uint8_t flags);

/* Load an in-memory bytecode image stream. */
bool rvm_load(Rvm *rd, char *img, uvar size);



/* For rvm_pseek(). */
#define RSEEK_SET  0
#define RSEEK_CURR 1
#define RSEEK_END  2

/* Get a section by name. */
RvmSec *rvm_getsec(Rvm *rd, char *name);

/* Create a new section. */
RvmSec *rvm_makesec(Rvm *rd, char *name, uvar size);

/* Removes a section. */
void rvm_rmsec(Rvm *rd, char *name);

/* Get a section's header. */
rvm_shdr rvm_getshdr(RvmSec *sec);

/* Set the entry count of the section header. */
void rvm_setentcnt(RvmSec *sec, uint64_t entcnt);

/* Seek on the section payload. */
void rvm_pseek(RvmSec *sec, ivar offst, int seek);

/* Returns the current pos in the payload. */
uvar rvm_ptell(RvmSec *sec);

/* Returns the size of the payload. */
uvar rvm_getsize(RvmSec *sec);

/* Resize the payload. */
bool rvm_resize(RvmSec *sec, uvar newsz);

/* Read from a section payload. Returns false if it tries to read out of bounds. */
bool rvm_read(RvmSec *sec, void *buf, uvar len);

/* Write to a section payload. Returns false if it tries to write out of bounds. */
bool rvm_write(RvmSec *sec, void *buf, uvar len);

#endif // RVM_BCODE_H_
