#include "MyFs.h"
#include <iostream>

int main()
{
  CMyFs MyFs;
  MyFs.SetDisplayRc(true);
  //  std::string MyDir1("A/B/C");
  MyFs.AddDir("A");
  MyFs.AddDir("A");
  MyFs.AddDir("A.dat");
  MyFs.AddFile("A.dat");
  MyFs.AddLink("", "ZZZ", "A.dat");
  MyFs.AddLink("", "ZZZ", "B.dat");
  MyFs.AddLink("", "ZZZ.dat", "B.dat");
  MyFs.AddFile("A.");
  MyFs.AddFile("AX_C");
  MyFs.AddFile("Z");
  MyFs.AddDir("A/B");
  MyFs.AddDir("A/B/C");
  MyFs.AddLink("A/B", "YYY", "C");
  MyFs.AddLink("A/B", "YYY", "Q");
  MyFs.AddLink("A/B", "QQQ", "Q");
  MyFs.AddDir("A/B/D.dat");
  MyFs.AddFile("A/B/D.dat");
  MyFs.DisplayAllFS();
  MyFs.DisplaySumOfAllFS();

  MyFs.AddDir("A/C/D");
  MyFs.AddDir("A/C/E");
  MyFs.AddDir("A/C/F");
  MyFs.AddDir("A/C/G");
  MyFs.DisplayAllFS();
  MyFs.DisplaySumOfAllFS();

  MyFs.AddFile("A/C/D/Y.dat");
  MyFs.AddFile("A/C/D/Z.dat");
  MyFs.AddFile("A/C/D/S.dat");
  MyFs.AddDir("A/C");
  MyFs.AddFile("A/C/D/Y.dat");
  MyFs.AddDir("A/C/D");
  MyFs.DisplayAllFS();

  MyFs.AddFile("A/C/D/Y.dat");
  MyFs.AddFile("A/C/D/Z.dat");
  MyFs.AddFile("A/C/D/S.dat");
  MyFs.DisplayAllFS();

  MyFs.DeleteFile("A/C/D/Y.dat");
  MyFs.DeleteFile("A/C/D/Z.dat");
  MyFs.DeleteFile("A/C/D/S.dat");
  MyFs.DeleteDir("A/C");
  MyFs.DisplayAllFS();

  MyFs.DeleteLink("A/B/YYY");
  MyFs.DeleteLink("A/B/QQQ");
  MyFs.DeleteLink("A/B/ZZZ");

  MyFs.DeleteFile("A/C/D/Y.dat");
  MyFs.DeleteFile("D.dat");
  MyFs.DeleteFile("A.dat");
  MyFs.DisplayAllFS();

  exit(0);
}
