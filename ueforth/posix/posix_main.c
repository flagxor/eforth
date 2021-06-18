#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/errno.h>

#include "common/opcodes.h"
#include "common/calling.h"
#include "common/calls.h"

#define HEAP_SIZE (10 * 1024 * 1024)
#define STACK_SIZE (64 * 1024)

#define PLATFORM_OPCODE_LIST \
  Y(errno, DUP; tos = (cell_t) errno) \
  Y(DLSYM, tos = (cell_t) dlsym(a1 ? a1 : RTLD_DEFAULT, a0); --sp) \
  CALLING_OPCODE_LIST \

#include "common/core.h"
#include "common/interp.h"

#include "gen/posix_boot.h"

int main(int argc, char *argv[]) {
  void *heap = mmap(
      (void *) 0x8000000, HEAP_SIZE,
      PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  forth_init(argc, argv, heap, boot, sizeof(boot));
  for (;;) { g_sys.rp = forth_run(g_sys.rp); }
  return 1;
}
