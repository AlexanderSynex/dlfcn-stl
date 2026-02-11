// Copyright 2025 <Alexander Malikov>

#pragma once

#include <dlfcn.h>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace snx {
namespace dlfcn {

enum class MODIFIERS : decltype(RTLD_LAZY) {
  // clang-format off
  LAZY     = RTLD_LAZY,
  NOW      = RTLD_NOW,
  GLOBAL   = RTLD_GLOBAL,
  LOCAL    = RTLD_LOCAL,
  /// @todo Добавить проверки версии GLIBC
  NODELETE = RTLD_NODELETE,  // since glibc 2.2
  NOLOAD   = RTLD_NOLOAD,    // since glibc 2.2
  DEEPBIND = RTLD_DEEPBIND,  // since glibc 2.3.4
  // clang-format on
};

auto operator|(MODIFIERS m1, MODIFIERS m2)
    -> std::underlying_type<MODIFIERS>::type {
  return static_cast<std::underlying_type<MODIFIERS>::type>(m1) |
         static_cast<std::underlying_type<MODIFIERS>::type>(m2);
}

class DynamicLibrary {
  struct Deleter {
    void operator()(void *ptr) const { dlclose(ptr); }
  };

  using LibraryHandler = std::unique_ptr<void, Deleter>;

  template <typename Func> using Callable = std::function<Func>;

 public:
  explicit DynamicLibrary(std::string_view path)
      : DynamicLibrary(path, static_cast<std::underlying_type<MODIFIERS>::type>(
                                 MODIFIERS::LAZY)) {}

  virtual ~DynamicLibrary() = default;

  bool loaded() const { return not(lib == nullptr); }
  operator bool() const { return loaded(); }

  template <typename Func>
  std::enable_if_t<std::is_function_v<Func>, Callable<Func>>
  extract(std::string_view name) & {
    if (not lib) {
      throw std::invalid_argument("Library was not properly loaded");
    }
    return Callable<Func>{
        reinterpret_cast<Func *>(dlsym(lib.get(), name.data()))};
  }

 private:
  DynamicLibrary(std::string_view path, int modifiers)
      : lib{LibraryHandler(dlopen(path.data(), modifiers))} {}

  LibraryHandler lib = nullptr;
};

}  // namespace dlfcn
}  // namespace snx
