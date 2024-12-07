#ifndef RVM_MACH_H_
#define RVM_MACH_H_
#include <stdint.h>
#include <stdbool.h>
#include "rvmbits.h"

extern char     *src;
extern uint64_t len;
extern uint64_t *data;
extern uint64_t datalen;
extern uint64_t reg[16];
extern uint64_t *stack;
extern uint32_t stack_len;
extern char     vmstate;
extern char     exitcode;

/* VM states */
#define V_INAC 0 /* Inactive */
#define V_PROV 1 /* Provisioning */
#define V_RUNN 2 /* Running */
#define V_SUSP 3 /* Suspended */
#define V_ERRR 4 /* Error */

/* Status codes */
typedef int statcd;
#define S_OK      0 /* Ok */
#define S_ERR     1 /* Internal error */
#define S_ILL     2 /* Illegal instruction */
#define S_INVC    3 /* Invalid VM call */
#define S_STOVF   4 /* Stack overflow */
#define S_STUND   5 /* Stack underflow */
#define S_OOB     6 /* Out of bounds access */

#define setf(f) (reg[RFL] |= (f))
#define clrf(f) (reg[RFL] &= ~(uint64_t)(f))
#define getf(f) (reg[RFL] & (f))

/* Check the magic number in src. */
bool checkmagic(char *src, uint64_t len);

/* Parse rvmhdr structure from src. */
bool parse_rvmhdr(char *src, uint64_t len, rvmhdr *out);

/* Return stat code strings. */
const char *statcd_msg(statcd n);

/* Dump the caller thread's registers into stderr. */
void dump_regs(void);

/* Load a program. The vm state must be in provisioning. */
bool vload(char *prog, uint64_t sz);

/* Initialise thread ctx. The vm must be in running state to use this. */
bool vth_init(uint32_t stlen);

/* Free thread ctx. Not allowed in provisioning state. */
bool vth_free(void);

/* Stack push and pop funcs. */
statcd vpush(uint64_t v);
statcd vpop(uint64_t *o);

/* Execute the next instruction. */
statcd vmexec(void);

/* VM Call. */
statcd vmcall(uint16_t ndx);

#endif // RVM_MACH_H_
