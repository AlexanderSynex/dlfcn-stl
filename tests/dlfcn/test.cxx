#include "dlfcn.hpp"
#include <gtest/gtest.h>

#include <gnu/lib-names.h>

using namespace snx;

TEST(dlfcn, loading) {
  EXPECT_TRUE(dlfcn::DynamicLibrary(LIBM_SO));
  EXPECT_FALSE(dlfcn::DynamicLibrary("libfalse"));
}
