#include "gtest/gtest.h"
#include "MyFs.h"

//#define DISPLAY_RC

TEST(TestMyFs, DeleteDir)
{
  CMyFs MyFs;
#ifdef DISPLAY_RC
  MyFs.SetDisplayRc(true);
#endif

  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteDir("A"), FsNs::EFSReturnCodeId::ErrCannotDeleteDir);
  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddDir("A/B"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteDir("A/B"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteDir("A/B"), FsNs::EFSReturnCodeId::ErrCannotDeleteDir);

}


TEST(TestMyFs, DeleteFile)
{
  CMyFs MyFs;
#ifdef DISPLAY_RC
  MyFs.SetDisplayRc(true);
#endif

  EXPECT_EQ(MyFs.AddFile("A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteFile("A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteFile("A.dat"), FsNs::EFSReturnCodeId::ErrCannotDeleteFile);
  EXPECT_EQ(MyFs.DeleteFile("B.dat"), FsNs::EFSReturnCodeId::ErrCannotDeleteFile);
  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddFile("A/A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteFile("A/A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteFile("A/A.dat"), FsNs::EFSReturnCodeId::ErrCannotDeleteFile);
  EXPECT_EQ(MyFs.AddDir("A/B"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddDir("A/B/C"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddDir("A/B/C/D"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddFile("A/B/C/D/A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteDir("A/B"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteFile("A/B/C/D/A.dat"), FsNs::EFSReturnCodeId::ErrCannotDeleteFile);

}


TEST(TestMyFs, DeleteLink)
{
  CMyFs MyFs;
#ifdef DISPLAY_RC
  MyFs.SetDisplayRc(true);
#endif

  EXPECT_EQ(MyFs.AddLink("", "lnk", "A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteLink("lnk"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteLink("lnk"), FsNs::EFSReturnCodeId::ErrCannotDeleteLink);

  EXPECT_EQ(MyFs.AddDir("A"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.AddLink("A", "lnk", "A.dat"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteLink("A/lnk"), FsNs::EFSReturnCodeId::Ok);
  EXPECT_EQ(MyFs.DeleteLink("A/lnk"), FsNs::EFSReturnCodeId::ErrCannotDeleteLink);
  EXPECT_EQ(MyFs.DeleteLink("A/lnk2"), FsNs::EFSReturnCodeId::ErrCannotDeleteLink);
}
