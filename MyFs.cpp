#include "MyFs.h"
#include <filesystem>
#include <iostream>
#include <regex>

namespace fs = std::filesystem;

CMyFs::CMyFs()
{
  m_FileSystemData = std::make_shared<CFileSystemDir>();
}

bool CMyFs::ValidFileName(std::string& FileName)
{
  static std::regex  regex("^[a-zA-Z]+\\.[a-zA-Z]+");
  return std::regex_match(FileName, regex);
}

bool CMyFs::ValidDirName(std::string& DirName)
{
  static std::regex  regex("^[a-zA-Z]+");
  return std::regex_match(DirName, regex);

}

bool CMyFs::ValidDirsName(std::vector<std::string>& VecStrings)
{
  bool Ok = false;
  for (auto &it : VecStrings)
  {
    Ok = ValidDirName(it);
    if (Ok == false)
    {
      break;
    }
  }
  return Ok;
}

bool CMyFs::ValidFSName(std::vector<std::string>& VecFSNames, EFSType FSType)
{
  bool Ok = false;
  int cnt = 0;
  size_t lastElementCnt = VecFSNames.size() - 1;
  for (auto &it : VecFSNames)
  {
    if (cnt < lastElementCnt)
    {
      Ok = ValidDirName(it);
    }
    else
    {
      // Link and file have the same validity
      Ok = ((FSType == EFSType::DIR) ? ValidDirName(it) : ValidFileName(it));
    }
    if (Ok == false)
    {
      break;
    }
    ++cnt;
  }
  return Ok;
}

void CMyFs::SplitFullPathToVector(std::string& FileNameFullPath, std::vector<std::string> &VecFSNames)
{
  std::filesystem::path path(FileNameFullPath);
  for (auto &p : path)
  {
    std::string MyString(p.string());
    VecFSNames.push_back(MyString);
    std::cout << "MyString=" << MyString << std::endl;
  }

}


std::shared_ptr<CFileSystemDir> CMyFs::GetDirInFs(std::vector<std::string>& VecFSNames)
{
  std::shared_ptr<CFileSystemDir> FileSystemDataPtr = m_FileSystemData;
  bool FoundMatchDir = true;
  for (auto &MyDirIter : VecFSNames)
  {
    TMapFSIter CfsIter = FileSystemDataPtr->m_MapDirName.find(MyDirIter);
    FoundMatchDir = (CfsIter != FileSystemDataPtr->m_MapDirName.end());

    if (FoundMatchDir)
    {
      FileSystemDataPtr = CfsIter->second;  // Find matching dir
    }
    else
    {
      FoundMatchDir = false;
      break;
    }
  }
  return FoundMatchDir ? FileSystemDataPtr : nullptr;  // If find match than return pointer to matching location in FS
}


bool CMyFs::SearchInDirs(std::vector<std::string>& VecFSNames, std::string& FsToBeAdded, std::shared_ptr<CFileSystemDir>& FileSystemDataPtr)
{
  bool FoundMatchFs = false;
  FsToBeAdded = VecFSNames.back();  // The dir name to be added
  VecFSNames.pop_back();   // Take out the dir name to be added 
  FileSystemDataPtr = GetDirInFs(VecFSNames);
  /*
  if (FileSystemDataPtr)  // Legal path to add directory;
  {
    // Now look if there is a file or directory that match this new FS name
    FoundMatchFs = (FileSystemDataPtr->m_MapDirName.find(FsToBeAdded) != FileSystemDataPtr->m_MapDirName.end());
    if (FoundMatchFs == false)
    {
      FoundMatchFs = (FileSystemDataPtr->m_MapFileName.find(FsToBeAdded) != FileSystemDataPtr->m_MapFileName.end());
    }
  }
  */
  return FoundMatchFs;
}

void CMyFs::DeleteDirRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr)
{
  if (FileSystemDataPtr->m_MapDirName.empty() == false)  // More subdirectories
  {
    for (auto &it : FileSystemDataPtr->m_MapDirName)
    {
      DeleteDirRecursive(it.second);
    }
  }
  FileSystemDataPtr->m_MapDirName.clear();
  FileSystemDataPtr->m_MapFileName.clear();
}

bool CMyFs::DeleteDir(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted)
{
  bool FoundMatchDir = (FileSystemDataPtr->m_MapDirName.find(FsToBeDeleted) != FileSystemDataPtr->m_MapDirName.end());
  DeleteDirRecursive(FileSystemDataPtr);

  return FoundMatchDir;
}

bool CMyFs::DeleteFile(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted)
{
  bool FoundMatchFile = (FileSystemDataPtr->m_MapFileName.find(FsToBeDeleted) != FileSystemDataPtr->m_MapFileName.end());
  if (FoundMatchFile)
  {
    FileSystemDataPtr->m_MapFileName.erase(FsToBeDeleted);
  }
  if (FileSystemDataPtr->m_MapFileName.size() <= 0)
  {
    FileSystemDataPtr->m_MapFileName.clear();
  }
  return FoundMatchFile;
}


bool CMyFs::DeleteElementInFs(std::vector<std::string>& VecFSNames)
{
  bool Ok = false;
  if (VecFSNames.size() >= 0)
  {
    std::string FsToBeDeleted;
    std::shared_ptr<CFileSystemDir> FileSystemDataPtr;
    bool FoundMatchFs = SearchInDirs(VecFSNames, FsToBeDeleted, FileSystemDataPtr);
    if (FileSystemDataPtr)
    {
      bool Deleted = DeleteDir(FileSystemDataPtr, FsToBeDeleted);
      if (Deleted = false)
      {
        bool Deleted = DeleteFile(FileSystemDataPtr, FsToBeDeleted);
      }
    }
  }
  return Ok;
}

bool CMyFs::AddElementToFs(std::vector<std::string>& VecFSNames, EFSType FSType)
{
  bool Ok = false;
  if (VecFSNames.size() >= 0)
  {
    std::string FsToBeAdded;
    std::shared_ptr<CFileSystemDir> FileSystemDataPtr;
    bool FoundMatchFs = false;
    SearchInDirs(VecFSNames, FsToBeAdded, FileSystemDataPtr);
    if (FileSystemDataPtr)  // Legal path to add directory;
    {
      // Now look if there is a file or directory that match this new FS name
      FoundMatchFs = (FileSystemDataPtr->m_MapDirName.find(FsToBeAdded) != FileSystemDataPtr->m_MapDirName.end());
      if (FoundMatchFs == false)
      {
        FoundMatchFs = (FileSystemDataPtr->m_MapFileName.find(FsToBeAdded) != FileSystemDataPtr->m_MapFileName.end());
      }
    }
    if ((FoundMatchFs == false) && (FileSystemDataPtr))
    {
      switch (FSType)
      {
      case EFSType::DIR:
        //          FileSystemDataPtr->m_MapDirName[FsToBeAdded] = new CFileSystemDir;
        FileSystemDataPtr->m_MapDirName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
        break;
      case EFSType::FILE:
        //         FileSystemDataPtr->m_MapFileName[FsToBeAdded] = new CFileSystemDir;
        FileSystemDataPtr->m_MapFileName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
        break;
      case EFSType::LINK:
        //          FileSystemDataPtr->m_MapFileName[FsToBeAdded] = new CFileSystemDir;
        FileSystemDataPtr->m_MapFileName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
        break;
      }
      Ok = true;
    }
    /*
    std::string FsToBeAdded = VecFSNames.back();  // The dir name to be added
    VecFSNames.pop_back();   // Take out the dir name to be added
    std::shared_ptr<CFileSystemDir> FileSystemDataPtr = GetDirInFs(VecFSNames);
    if (FileSystemDataPtr)  // Legal path to add directory;
    {
      // Now look if there is a file or directory that match this new FS name
      bool FoundMatchFs = (FileSystemDataPtr->m_MapDirName.find(FsToBeAdded) != FileSystemDataPtr->m_MapDirName.end());
      if (FoundMatchFs == false)
      {
        FoundMatchFs = (FileSystemDataPtr->m_MapFileName.find(FsToBeAdded) != FileSystemDataPtr->m_MapFileName.end());
      }
      if (FoundMatchFs == false)
      {
        switch (FSType)
        {
        case EFSType::DIR:
          //          FileSystemDataPtr->m_MapDirName[FsToBeAdded] = new CFileSystemDir;
          FileSystemDataPtr->m_MapDirName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
          break;
        case EFSType::FILE:
          //         FileSystemDataPtr->m_MapFileName[FsToBeAdded] = new CFileSystemDir;
          FileSystemDataPtr->m_MapFileName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
          break;
        case EFSType::LINK:
          //          FileSystemDataPtr->m_MapFileName[FsToBeAdded] = new CFileSystemDir;
          FileSystemDataPtr->m_MapFileName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
          break;
        }
        Ok = true;
      }
    }
    */
  }
  return Ok;
}



bool CMyFs::AddDir(std::string FileNameFullPath)
{
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(FileNameFullPath, VecFSNames);
  bool Ok = ValidFSName(VecFSNames, EFSType::DIR);
  if (Ok)
  {
    Ok = AddElementToFs(VecFSNames, EFSType::DIR);
  }
  std::cout << "Ok=" << Ok << std::endl;

  return Ok;
}

bool CMyFs::AddFile(std::string FileNameFullPath)
{
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(FileNameFullPath, VecFSNames);
  bool Ok = ValidFSName(VecFSNames, EFSType::FILE);
  if (Ok)
  {
    Ok = AddElementToFs(VecFSNames, EFSType::FILE);
  }
  std::cout << "Ok=" << Ok << std::endl;

  return Ok;
}

bool CMyFs::DeleteElement(std::string FileNameFullPath)
{
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(FileNameFullPath, VecFSNames);
  bool Ok = true;

  std::string FsToBeDeleted = VecFSNames.back();  // The dir name to be added
  VecFSNames.pop_back();   // Take out the dir name to be added 

  std::shared_ptr<CFileSystemDir> FileSystemDataPtr = GetDirInFs(VecFSNames);
  if (FileSystemDataPtr)  // Legal path to add directory;
  {
    // Now look if there is a file or directory that match this new FS name
    bool FoundMatchFs = (FileSystemDataPtr->m_MapDirName.find(FsToBeDeleted) != FileSystemDataPtr->m_MapDirName.end());
  }
  return Ok;
}

int main()
{
  CMyFs MyFs;
  //  std::string MyDir1("A/B/C");
  MyFs.AddDir("A");
  MyFs.AddDir("A");
  MyFs.AddDir("A.dat");
  MyFs.AddDir("A/B");
  MyFs.AddDir("A/B/C");
  MyFs.AddDir("A/B/D.dat");
  MyFs.AddFile("A/B/D.dat");
  MyFs.AddDir("A/C/D");
  MyFs.AddDir("A/C");
  MyFs.AddFile("A/C/D/Y.dat");
  MyFs.AddDir("A/C/D");
  MyFs.AddFile("A/C/D/Y.dat");

  MyFs.DeleteElement("A/C");
  MyFs.AddDir("A.dat");
  MyFs.AddDir("A.dat");



  exit(0);
}
