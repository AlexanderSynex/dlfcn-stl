#include "dlfcn/dynamic_library.hpp"

#include <stdexcept>

using namespace snx::dlfcn;

DynamicLibrary::SymbolHandler
DynamicLibrary::extractRaw(std::string_view name,
                           std::string_view symbolName) const {
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

  return SymbolHandler{func};
}
