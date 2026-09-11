#include <cstdlib>
#define main original_main
#include "inertia_test.cpp"
#undef main
int main() {
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
  return original_main();
}
