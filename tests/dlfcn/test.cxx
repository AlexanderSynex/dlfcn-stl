#include "dlfcn/dynamic_library.hpp"

#include <gtest/gtest.h>

#include <gnu/lib-names.h>
#include <stdexcept>

using namespace snx;

TEST(dlfcn, loading) {
  EXPECT_TRUE(dlfcn::DynamicLibrary(LIBM_SO));
  EXPECT_FALSE(dlfcn::DynamicLibrary("libfalse"));
}

TEST(dlfcn, extracting) {
  auto validLibrary = dlfcn::DynamicLibrary(LIBM_SO)
                      << dlfcn::MODIFIERS::GLOBAL;
  auto errorLibrary = dlfcn::DynamicLibrary("libfalse");

  // dlfcn::DynamicLibrary(LIBM_SO).extract<double(double)>("asd");

  EXPECT_NO_THROW(validLibrary.extract<double(double)>("cos"));
  EXPECT_THROW(validLibrary.extract<double(double)>("cosqqqq"),
               std::invalid_argument);
  EXPECT_THROW(errorLibrary.extract<int(int)>("asd"), std::invalid_argument);

  EXPECT_FLOAT_EQ(validLibrary.extract<double(double)>("cos")(0), 1);

  /// @todo Добавить тесты для извлечения переменных
}
