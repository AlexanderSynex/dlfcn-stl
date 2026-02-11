// Copyright 2025 <Alexander Malikov>

#pragma once

#include <dlfcn.h>
#include <memory>
#include <string_view>

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

class DynamicLibrary {
  struct Deleter {
    void operator()(void *ptr) const { dlclose(ptr); }
  };

  using LibraryHandler = std::unique_ptr<void, Deleter>;

 public:
  explicit DynamicLibrary(std::string_view path)
      : DynamicLibrary(path, MODIFIERS::LAZY) {}

  virtual ~DynamicLibrary() = default;

  bool loaded() const { return not(lib == nullptr); }
  operator bool() const { return loaded(); }

 private:
  DynamicLibrary(std::string_view path, MODIFIERS modifiers)
      : lib{LibraryHandler(dlopen(path.data(), static_cast<int>(modifiers)))} {}

  LibraryHandler lib = nullptr;
};
}  // namespace dlfcn
}  // namespace snx
