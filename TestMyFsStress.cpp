#include "gtest/gtest.h"
#include "MyFs.h"

// See explanation on "TEST(...)" below
void testStressDir(std::string seperator, int NumCharToDelete)
{
  CMyFs MyFs;
  static const int MAX_DIR = 100;
  static const int NUM_LOOPS = 1000;
  static const std::string DIR_FILE = "A";
  for (int loopCnt = 0; loopCnt < NUM_LOOPS; ++loopCnt)
  {
    std::string strDir = DIR_FILE;
    for (int dirCnt = 0; dirCnt < MAX_DIR; ++dirCnt)
    {
      EXPECT_EQ(MyFs.AddDir(strDir), FsNs::EFSReturnCodeId::Ok);
      if (dirCnt % (MAX_DIR / 10) == 0)
      {
        //        std::cout << "Dir=" << strDir << std::endl;
        //        std::cout << "dirCnt=" << dirCnt << std::endl;
      }
      strDir = strDir + seperator + DIR_FILE;
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
}

// Create & Delete multi times the directories "A", "A/A", "A/A/A" ...
TEST(TestMyFs, StressMultiSubDirs)
{
  testStressDir("/", 2);
}

// Create & Delete multi times the directories "A", "AA", "AAA" ...
TEST(TestMyFs, StressFlatDirs)
{
  testStressDir("", 1);
}


// Create & Delete multi times the files "FILE.A", "FILE.AA", "FILE.AAA" ...
TEST(TestMyFs, StressFiles)
{
  CMyFs MyFs;
  static const int NUM_LOOPS = 1000;
  static const int MAX_FILE = 100;
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



