#include "test.h"
#include <hi/hi.h>
#include <hi/rusage.h>
#include <uv.h>

using namespace hi;

const size_t N = 4000;
int Y = 0;
int* X = &Y;

hi::rusage::sample rsample;

void reset() {
  main_queue().async([&]{ rsample(); });
}

void HI_NO_INLINE do_work() {
  asm("");
  *X = *X + 1;
}

void check_and_summarize(const char* label) {
  #if !HI_TEST_SUIT_RUNNING
  double avg_user = ((double)rsample.delta_user_usec() * 1000.0) / N;
  double avg_real = ((double)rsample.delta_real_usec() * 1000.0) / N;
  print(
    "%s:\n"
    "  Per invocation:\n"
    "    Average real time:     % 15.0f ns\n"
    "    Average user CPU time: % 15.0f ns\n"
    "    Syscall overhead:      % 15.1f %%"
    "",
    label,
    avg_real,
    avg_user,
    (1.0 - (avg_user / avg_real)) * 100.0
  );
  rsample.delta_dump("  All invocations:", "    ");
  #endif
}

void enqueue_chain(size_t index) {
  main_queue().async([&,index](){
    do_work();
    if (index != N-1) {
      enqueue_chain(index + 1);
    } else {
      check_and_summarize("Chained");
    }
  });
}

int main(int argc, char** argv) {
  print("N = %zu", N);
  // First up is batched queueing
  reset();
  for (size_t i = 0; i != N; ++i) {
    main_queue().async([&,i](){
      do_work();
    });
  }
  main_queue().async([]{
    check_and_summarize("Batch");

    // Next up is chained queueing
    reset();
    enqueue_chain(0);
  });

  return main_loop();
}
