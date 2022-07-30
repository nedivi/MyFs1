#include "gtest/gtest.h"
#include "MyFs.h"

//#define DEBUG_MISC_TEST

// See explanation on "TEST(...)" below
//void testMisc1(std::string seperator, int NumCharToDelete, const int MAX_DIR)
TEST(TestMyFs, TestMiscAddDeleteDirsFilesLinks)
{
  CMyFs MyFs;
  //  MyFs.SetDisplayRc(true);

  static const int NUM_LOOPS = 1000;
  static const std::string DIR_NAME = "DIR";
  static const std::string FILE_NAME = "FILE.DAT";
  static const std::string LNK_NAME = "LNK";
  static const std::string LINKED_FILE = "A.DAT";

  static const std::string EXT = "A";
  std::string strDir = DIR_NAME;
  std::string strFile = FILE_NAME;
  std::string strLink = LNK_NAME;

  for (int loopCnt = 0; loopCnt < NUM_LOOPS; ++loopCnt)
  {
    EXPECT_EQ(MyFs.AddDir(strDir), FsNs::EFSReturnCodeId::Ok);
    EXPECT_EQ(MyFs.AddFile(strFile), FsNs::EFSReturnCodeId::Ok);
    EXPECT_EQ(MyFs.AddLink("", strLink, LINKED_FILE), FsNs::EFSReturnCodeId::Ok);
#ifdef DEBUG_MISC_TEST
    if (loopCnt % (NUM_LOOPS / 10) == 0)
    {
      std::cout << "Dir=" << strDir << "  " << "File=" << strFile << "  " << "Link=" << strLink << "  " << std::endl;
      std::cout << "loopCnt=" << loopCnt << std::endl;
    }
#endif
    strDir += EXT;
    strFile += EXT;
    strLink += EXT;
    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumDirs(), loopCnt + 1);
    EXPECT_EQ(MyFs.GetSumLinks(), loopCnt + 1);
    EXPECT_EQ(MyFs.GetSumFiles(), loopCnt + 1);
  }

  for (int loopCnt = NUM_LOOPS; loopCnt > 0; --loopCnt)
  {
    strDir.pop_back();
    strFile.pop_back();
    strLink.pop_back();

    EXPECT_EQ(MyFs.DeleteDir(strDir), FsNs::EFSReturnCodeId::Ok);
    EXPECT_EQ(MyFs.DeleteFile(strFile), FsNs::EFSReturnCodeId::Ok);
    EXPECT_EQ(MyFs.DeleteLink(strLink), FsNs::EFSReturnCodeId::Ok);
#ifdef DEBUG_MISC_TEST
    if (loopCnt % (NUM_LOOPS / 10) == 0)
    {
      std::cout << "Dir=" << strDir << "  " << "File=" << strFile << "  " << "Link=" << strLink << "  " << std::endl;
      std::cout << "loopCnt=" << loopCnt << std::endl;
    }
#endif

    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumDirs(), loopCnt - 1);
    EXPECT_EQ(MyFs.GetSumLinks(), loopCnt - 1);
    EXPECT_EQ(MyFs.GetSumFiles(), loopCnt - 1);
  }

  /*
  for (int dirCnt = 0; dirCnt < MAX_DIR; ++dirCnt)
  {
    for (int numCh = 0; numCh < NumCharToDelete; ++numCh)
    {
      strDir.pop_back();
    }
    //    std::cout << "Dir=" << strDir << std::endl;
    EXPECT_EQ(MyFs.DeleteDir(strDir), FsNs::EFSReturnCodeId::Ok);
    if (dirCnt % (MAX_DIR / 10) == 0)
    {
      //        std::cout << "Dir=" << strDir << std::endl;
      //        std::cout << "dirCnt=" << dirCnt << std::endl;
    }
  }
  MyFs.CollectStatisticsAllFS();
  EXPECT_EQ(MyFs.GetSumDirs(), 0);
  EXPECT_EQ(MyFs.GetSumLinks(), 0);
  EXPECT_EQ(MyFs.GetSumFiles(), 0);
}
*/
}

