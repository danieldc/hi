#ifndef _HI_H_
#define _HI_H_

#include <hi/common.h>

namespace hi {

struct error;
struct queue;
struct channel;
struct tls_context;
struct data_; typedef ::std::shared_ptr<data_> data;
template <typename T> using fun = ::std::function<T>;

// Call a function exactly and only once per once_flag object
struct once_flag; template<class Function, typename... Args>
void once(once_flag&, Function&&, Args&&...);

// The main queue
queue& main_queue();
int   main_loop();  // Enter the main queue run loop
bool  main_next();  // Run events in the main queue. Blocks if the queue is empty. Returns true if
                    // there are more events waiting to be processed.
bool  main_next_nowait(); // Does not block in the case there are no queued events

// Execute a function in some background thread
void async(fun<void()>);

// Suspend the calling queue for `seconds` time. Returns true if interrupted.
bool sleep(double seconds);

// Serial processing queue
struct queue {
  queue(const std::string& label);
  // static queue current();
  queue& resume() const;
  queue& async(fun<void()>) const;
  bool is_current() const; // true if this is the calling queue
  const std::string& label() const;
  queue() : self(nullptr) {};
  HI_REF_MIXIN(queue)
};

struct semaphore {
  semaphore(unsigned int value = 0);
  
  // Decrements the value of semaphore variable by 1. If the value becomes negative, the queue
  // executing wait() is blocked, i.e., added to the semaphore's waiting queue.
  void wait() const;

  // Increments the value of semaphore variable by 1. After the increment, if the pre-increment
  // value was negative (meaning there are queues waiting), it transfers a blocked queue from the
  // semaphore's waiting queue to the ready queue.
  void signal() const;

  HI_REF_MIXIN(semaphore)
};

struct channel {
  static channel connect(const std::string& endpoint, fun<void(error,channel)>);
  static channel connect(const std::string& endpoint, tls_context, fun<void(error,channel)>);
  static channel connect(queue, const std::string& endpoint, fun<void(error,channel)>);
  static channel connect(queue, const std::string& endpoint, tls_context, fun<void(error,channel)>);
  std::string endpoint_name() const;
  tls_context tls() const; // == nullptr unless TLS-filtered
  void close(fun<void()> = nullptr) const;
  void read(size_t max_size, fun<bool(error,data)>) const;
  void write(const char* buf, size_t len, fun<void(error)> = nullptr) const; // buf copy
  void write(char* buf, size_t len, size_t capacity, fun<void(error)>) const; // user buf mgmt.
  channel(); // == nullptr
  HI_REF_MIXIN(channel)
};

struct tls_context {
  tls_context();
  void load_ca_cert_file(const char* path);
  HI_REF_MIXIN(tls_context)
};

struct error {
  error(const std::string& msg, int code = 0);
  error(std::string&& msg, int code = 0);
  error() : self(nullptr) {} // == nullptr
  int code() const;
  const std::string& message() const;
  HI_REF_MIXIN(error)
};

data create_data(char* bytes, size_t size, size_t capacity); // takes ownership over `bytes`

struct data_ {
  const char* bytes() const { return _bytes; }
  char* bytes() { return _bytes; }
  size_t size() const { return _size; }
  void set_size(size_t z) { assert(z <= _capacity); _size = z; }
  size_t capacity() const { return _capacity; }
  ~data_();
  data_(char* b, size_t z, size_t c) : _bytes(b), _size(z), _capacity(c) {}
  char*  _bytes;
  size_t _size;
  size_t _capacity;
};

inline std::ostream& operator<< (std::ostream& os, const error& e) {
  return (e == nullptr) ? (os << "(null)") : (os << e.message() << " #" << e.code());
}

// ------------------------------------------------------------------------------------------------

struct once_flag { volatile long s = 0; };
template<class Function, typename... Args>
inline void once(once_flag& pred, Function&& f, Args&&... args) {
  if (hi_atomic_cas(&pred.s, 0, 1) == 0) { f(args...); }
}

} // namespace

#endif // _HI_H_
