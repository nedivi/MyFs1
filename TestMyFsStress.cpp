#include "gtest/gtest.h"
#include "MyFs.h"


#define DO_STRESS_TEST
//#define DEBUG_STRESS_TEST



// See explanation on "TEST(...)" below
void testStressDir(std::string seperator, int NumCharToDelete, const int MAX_DIR)
{
  CMyFs MyFs;
  static const int NUM_LOOPS = 1000;
  static const std::string DIR_NAME = "A";
  for (int loopCnt = 0; loopCnt < NUM_LOOPS; ++loopCnt)
  {
    std::string strDir = DIR_NAME;
    for (int dirCnt = 0; dirCnt < MAX_DIR; ++dirCnt)
    {
      EXPECT_EQ(MyFs.AddDir(strDir), FsNs::EFSReturnCodeId::Ok);
#ifdef DEBUG_STRESS_TEST
      if (dirCnt % (MAX_DIR / 10) == 0)
      {
        std::cout << "Dir=" << strDir << std::endl;
        std::cout << "dirCnt=" << dirCnt << std::endl;
      }
#endif
      strDir = strDir + seperator + DIR_NAME;
    }
    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumDirs(), MAX_DIR);
    EXPECT_EQ(MyFs.GetSumLinks(), 0);
    EXPECT_EQ(MyFs.GetSumFiles(), 0);

    for (int dirCnt = 0; dirCnt < MAX_DIR; ++dirCnt)
    {
      for (int numCh = 0; numCh < NumCharToDelete; ++numCh)
      {
        strDir.pop_back();
      }
      //    std::cout << "Dir=" << strDir << std::endl;
      EXPECT_EQ(MyFs.DeleteDir(strDir), FsNs::EFSReturnCodeId::Ok);
#ifdef DEBUG_STRESS_TEST
      if (dirCnt % (MAX_DIR / 10) == 0)
      {
        std::cout << "Dir=" << strDir << std::endl;
        std::cout << "dirCnt=" << dirCnt << std::endl;
      }
#endif
    }
    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumDirs(), 0);
    EXPECT_EQ(MyFs.GetSumLinks(), 0);
    EXPECT_EQ(MyFs.GetSumFiles(), 0);
  }
}

#ifdef DO_STRESS_TEST
// Create & Delete multi times the directories "A", "A/A", "A/A/A" ...
TEST(TestMyFs, StressMultiSubDirs)
{
  const int MAX_DIR = 50;
  const std::string DIR_SEPERATOR = "/";
  testStressDir(DIR_SEPERATOR, (int)DIR_SEPERATOR.length() + 1, MAX_DIR);
}

// Create & Delete multi times the directories "A", "AA", "AAA" ...
TEST(TestMyFs, StressFlatDirs)
{
  const int MAX_DIR = 200;
  const std::string EMPTY_SEPERATOR = "";
  testStressDir(EMPTY_SEPERATOR, (int)EMPTY_SEPERATOR.length() + 1, MAX_DIR);
}


// Create & Delete multi times the files "FILE.A", "FILE.AA", "FILE.AAA" ...
TEST(TestMyFs, StressFiles)
{
  CMyFs MyFs;
  static const int NUM_LOOPS = 1000;
  static const int MAX_FILE = 200;
  static const std::string FILE_EXT = "A";
  std::string strFile = "FILE." + FILE_EXT;

  for (int loopCnt = 0; loopCnt < NUM_LOOPS; ++loopCnt)
  {
    for (int fileCnt = 0; fileCnt < MAX_FILE; ++fileCnt)
    {
      EXPECT_EQ(MyFs.AddFile(strFile), FsNs::EFSReturnCodeId::Ok);
      if (fileCnt % (MAX_FILE / 10) == 0)
      {
        // std::cout << "strFile=" << strFile << std::endl;
        // std::cout << "fileCnt=" << fileCnt << std::endl;
      }
      strFile += FILE_EXT;
    }
    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumFiles(), MAX_FILE);
    EXPECT_EQ(MyFs.GetSumDirs(), 0);
    EXPECT_EQ(MyFs.GetSumLinks(), 0);

    for (int fileCnt = 0; fileCnt < MAX_FILE; ++fileCnt)
    {
      strFile.pop_back();
      EXPECT_EQ(MyFs.DeleteFile(strFile), FsNs::EFSReturnCodeId::Ok);
      if (fileCnt % (MAX_FILE / 10) == 0)
      {
        // std::cout << "strFile=" << strFile << std::endl;
        // std::cout << "fileCnt=" << fileCnt << std::endl;
      }
    }
    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumFiles(), 0);
    EXPECT_EQ(MyFs.GetSumDirs(), 0);
    EXPECT_EQ(MyFs.GetSumLinks(), 0);
  }
}




// Create & Delete multi times the links "link1" --> "A.dat",  "link11" --> "A.dat" , "link111" --> "A.dat"...
TEST(TestMyFs, StressLinks)
{
  CMyFs MyFs;
  static const int NUM_LOOPS = 1000;
  static const int MAX_LINK = 200;
  static const std::string LINK_EXT = "1";
  static const std::string LINKED_FILE = "A.dat";
  std::string strLink = std::string("link") + LINK_EXT;

  for (int loopCnt = 0; loopCnt < NUM_LOOPS; ++loopCnt)
  {
    for (int fileCnt = 0; fileCnt < MAX_LINK; ++fileCnt)
    {
      EXPECT_EQ(MyFs.AddLink("", strLink, LINKED_FILE), FsNs::EFSReturnCodeId::Ok);
#ifdef DEBUG_STRESS_TEST
      if (fileCnt % (MAX_LINK / 10) == 0)
      {
        std::cout << "strLink=" << strLink << std::endl;
        std::cout << "fileCnt=" << fileCnt << std::endl;
      }
#endif
      strLink += LINK_EXT;
    }
    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumFiles(), 0);
    EXPECT_EQ(MyFs.GetSumDirs(), 0);
    EXPECT_EQ(MyFs.GetSumLinks(), MAX_LINK);

    for (int linkCnt = 0; linkCnt < MAX_LINK; ++linkCnt)
    {
      strLink.pop_back();
      EXPECT_EQ(MyFs.DeleteLink(strLink), FsNs::EFSReturnCodeId::Ok);
#ifdef DEBUG_STRESS_TEST
      if (linkCnt % (MAX_LINK / 10) == 0)
      {
        std::cout << "strLink=" << strLink << std::endl;
        std::cout << "linkCnt=" << linkCnt << std::endl;
      }
#endif
    }
    MyFs.CollectStatisticsAllFS();
    EXPECT_EQ(MyFs.GetSumFiles(), 0);
    EXPECT_EQ(MyFs.GetSumDirs(), 0);
    EXPECT_EQ(MyFs.GetSumLinks(), 0);
  }
}

#endif