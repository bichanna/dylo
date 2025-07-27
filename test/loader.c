#include "../dylo.h"
#include <stdio.h>
#include <stdint.h>

int main(void) {
  Dylo_Handle handle = dylo_open("libtest.dylib");
  if (!handle) {
    printf("%s\n", dylo_error());
    return -1;
  }

  int32_t (*add_int)(int32_t, int32_t) = dylo_symbol(handle, "add_int");
  if (!add_int) {
    printf("%s\n", dylo_error());
    return -1;
  }

  printf("result: %i\n", add_int(1, 1));

  if (dylo_close(handle) != 0) {
    return -1;
  }

  return 0;
}
