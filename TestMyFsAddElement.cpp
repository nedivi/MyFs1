#include "gtest/gtest.h"
#include "MyFs.h"

TEST(TestMyFs, AddDir)
{
  CMyFs MyFs;
  //  std::string MyDir1("A/B/C");
  MyFs.AddDir("A");
  EXPECT_EQ(MyFs.GetReturnCode(), FsNs::EFSReturnCodeId::Ok);

  MyFs.AddDir("A");
  EXPECT_EQ(MyFs.GetReturnCode(), FsNs::EFSReturnCodeId::Ok);


}

