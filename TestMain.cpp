#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
  // change the default log write directory to a temp directory, since unit tests are running
  // from within visual studio executable

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}