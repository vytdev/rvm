#include "mach.h"
#include "rvmbits.h"
#include <stdint.h>
#include <stdlib.h>

/* Return perm err if the process has no
   authority to execute the vm call. */
#define check_perm(d) do { \
    if (!has_perm((d)))    \
      return S_PERM;       \
  } while (0)

statcd vmcall(uint16_t ndx) {
switch (ndx) {
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
  exit(reg[R0]);
}


default: return S_INVC; }
}
