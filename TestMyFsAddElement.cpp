#include "gtest/gtest.h"
#include "MyFs.h"

TEST(TestMyFs, AddDir)
{
  CMyFs MyFs;

  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::ErrCannotAddDirAlreadyExist);
  EXPECT_EQ(MyFs.AddDir("A#$%"), FsNs::EFSReturnCodeId::ErrDirNameIsInvalid);
  EXPECT_EQ(MyFs.AddDir("A/B"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddDir("A/B"), FsNs::EFSReturnCodeId::ErrCannotAddDirAlreadyExist);
  EXPECT_EQ(MyFs.AddDir("A/B.dat"), FsNs::EFSReturnCodeId::ErrDirNameIsInvalid);
}

TEST(TestMyFs, AddFile)
{
  CMyFs MyFs;

  EXPECT_EQ(MyFs.AddFile("A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddFile("A."), FsNs::EFSReturnCodeId::ErrFileNameIsInvalid);
  EXPECT_EQ(MyFs.AddFile("A"), FsNs::EFSReturnCodeId::ErrFileNameIsInvalid);
  EXPECT_EQ(MyFs.AddFile("A.dat"), FsNs::EFSReturnCodeId::ErrCannotAddFileAlreadyExist);
  EXPECT_EQ(MyFs.AddFile("A#$%"), FsNs::EFSReturnCodeId::ErrFileNameIsInvalid);
  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddDir("A/B"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddFile("A/B.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddFile("A/B.dat"), FsNs::EFSReturnCodeId::ErrCannotAddFileAlreadyExist);
  EXPECT_EQ(MyFs.AddFile("A/BCD"), FsNs::EFSReturnCodeId::ErrFileNameIsInvalid);
}
