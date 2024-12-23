#include "mach.h"
#include "rvmbits.h"
#include <stdint.h>
#include <stdlib.h>

/* Return perm err if the process has no
   authority to execute the vm call. */
#define check_perm(d) do { \
    if (!has_perm((d)))    \
      return E_PERM;       \
  } while (0)

excp vmcall(void) {
switch (reg[R0] & 0xffff) {
#define defcall(n) case (n):


defcall(VM_EXIT) {
  benchmark_tag();
  dump_benchmark();
  /* Allocated resources are automatically reclaimed by the OS,
     so using exit() directly is safe for the VM. However, the
     user program is responsible for cleaning up critical
     resources that need special handling (i.e. open dbs that
     has no recovery mechs). As such, relying solely on VM_EXIT
     without proper user-managed resource cleanup might not
     always be safe for the user. */
  exit(reg[R1]);
}


default: return E_INVC; }
}
