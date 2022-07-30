#include "gtest/gtest.h"
#include "MyFs.h"

//#define DISPLAY_RC

TEST(TestMyFs, AddDir)
{
  CMyFs MyFs;
#ifdef DISPLAY_RC
  MyFs.SetDisplayRc(true);
#endif

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
#ifdef DISPLAY_RC
  MyFs.SetDisplayRc(true);
#endif

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

TEST(TestMyFs, AddLink)
{
  CMyFs MyFs;
#ifdef DISPLAY_RC
  MyFs.SetDisplayRc(true);
#endif

  EXPECT_EQ(MyFs.AddLink("", "ZZZ", "A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddLink("", "ZZZ", "A.dat"), FsNs::EFSReturnCodeId::ErrCannotAddLinkAlreadyExist);
  EXPECT_EQ(MyFs.AddLink("A", "LLL", "A.dat"), FsNs::EFSReturnCodeId::ErrCannotAddLinkIllegalPath);
  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddLink("A", "LLL", "A.dat"), FsNs::EFSReturnCodeId::Ok);
}
