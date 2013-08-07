// Helpers for tests
#ifndef _HI_TEST_H_
#define _HI_TEST_H_

#ifdef HI_DEBUG
  #undef HI_DEBUG
#endif
#define HI_DEBUG 1
#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <hi/common.h>
#include <unistd.h> // _exit

#if HI_TEST_SUIT_RUNNING
  #define print(...) ((void)0)
#else
  #define print(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#endif

// ------------------------------
#ifdef __cplusplus
#include <iostream>

namespace hi {

template <typename A, typename B>
inline void HI_UNUSED _assert_eq(
    A value1, const char* name1,
    B value2, const char* name2,
    const char* source_name, int source_line)
{
  if (!(value1 == value2)) {
    std::cerr << "\nAssertion failure at "
              << source_name << ":" << source_line << "\n"
              << "  " << name1 << " == " << name2 << "\n"
              << "  " << value1 << " == " << value2
              << std::endl;
    _exit(30);
  }
}

template <typename A, typename B>
inline void HI_UNUSED _assert_not_eq(
    A value1, const char* name1,
    B value2, const char* name2,
    const char* source_name, int source_line)
{
  if (!(value1 != value2)) {
    std::cerr << "\nAssertion failure at "
              << source_name << ":" << source_line << "\n"
              << "  " << name1 << " != " << name2 << "\n"
              << "  " << value1 << " != " << value2
              << std::endl;
    _exit(30);
  }
}

inline void HI_UNUSED _assert_eq_cstr(
    const char* cstr1, const char* name1,
    const char* cstr2, const char* name2,
    const char* source_name, int source_line)
{
  if (std::strcmp(cstr1, cstr2) != 0) {
    std::cerr << "\nAssertion failure at "
              << source_name << ":" << source_line << "\n"
              << "  " << name1 << " == " << name2 << "\n"
              << "  \"" << cstr1 << "\" == \"" << cstr2 << "\""
              << std::endl;
    _exit(30);
  }
}

#define assert_eq(a, b) ::hi::_assert_eq((a), #a, (b), #b, HI_FILENAME, __LINE__)
#define assert_not_eq(a, b) ::hi::_assert_not_eq((a), #a, (b), #b, HI_FILENAME, __LINE__)
#define assert_eq_cstr(a, b) ::hi::_assert_eq_cstr((a), #a, (b), #b, HI_FILENAME, __LINE__)
#define assert_true(a) ::hi::_assert_eq((a), #a, true, "true", HI_FILENAME, __LINE__)
#define assert_false(a) ::hi::_assert_eq((a), #a, false, "false", HI_FILENAME, __LINE__)
#define assert_null(a) ::hi::_assert_eq((a), #a, nullptr, "NULL", HI_FILENAME, __LINE__)

} // namespace
#endif // __cplusplus


#endif  // _HI_TEST_H_
