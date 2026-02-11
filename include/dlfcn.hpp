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

  using SymbolHandler = std::unique_ptr<void, Deleter>;

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
    return Callable<Func>{
        reinterpret_cast<Func *>(extractRaw(name, "Function").release())};
  }

  template <typename Type>
  std::enable_if_t<not std::is_function_v<Type>, Type>
  extract(std::string_view name) & {
    return reinterpret_cast<Type &>(extractRaw(name, "Variable"));
  }

 private:
  SymbolHandler extractRaw(std::string_view name, std::string_view symbolName) {
    if (not lib) {
      throw std::invalid_argument("Library was not properly loaded");
    }
    auto func = dlsym(lib.get(), name.data());

    auto quoted = [](std::string_view str) {
      using namespace std::string_literals;
      return "\""s + std::string{str} + '"';
    };

    using namespace std::string_literals;
    if (not func) {
      throw std::invalid_argument(std::string{symbolName} + " with name "s +
                                  quoted(name) + " was not imported");
    }

    return SymbolHandler{dlsym(lib.get(), name.data())};
  }

  DynamicLibrary(std::string_view path, int modifiers)
      : lib{SymbolHandler(dlopen(path.data(), modifiers))} {}

  SymbolHandler lib = nullptr;
};

}  // namespace dlfcn
}  // namespace snx
