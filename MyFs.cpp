#include "MyFs.h"
#include <filesystem>
#include <iostream>
#include <regex>

namespace fs = std::filesystem;
using namespace FsNs;

#define  DISPLAY_DEBUG 1

CMyFs::CMyFs()
{
  m_FileSystemData = std::make_shared<CFileSystemDir>();
}

bool CMyFs::ValidFileName(std::string& FileName)
{
  static std::regex  regex("^[a-zA-Z]+\\.[a-zA-Z]+");
  return std::regex_match(FileName, regex);
}

// Assuming that Link-name & dir-name have the same syntax
bool CMyFs::ValidLinkName(std::string& FileName)
{
  return ValidDirName(FileName);
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
      switch (FSType)
      {
      case EFSType::DIR:
        Ok = ValidDirName(it);
        break;
      case EFSType::FILE:
        Ok = ValidFileName(it);
        break;
      case EFSType::LINK:
        Ok = ValidLinkName(it);
        break;
      default:
        Ok = false;
        break;

      }
      if (Ok == false)
      {
        break;
      }
      ++cnt;
    }
  }
  return Ok;
}

void CMyFs::DisplayReturnCode()
{
  std::string ReturnCodeStr("Return code not found");
  if (MapReturnCodeId2Str.find(m_FSReturnCodeId) != MapReturnCodeId2Str.end())
  {
    ReturnCodeStr = MapReturnCodeId2Str[m_FSReturnCodeId];
  }
  std::cout << "Result: " << ReturnCodeStr << std::endl;
}

void CMyFs::SplitFullPathToVector(std::string& FileNameFullPath, std::vector<std::string> &VecFSNames)
{
  std::filesystem::path path(FileNameFullPath);
  for (auto &p : path)
  {
    std::string MyString(p.string());
    VecFSNames.push_back(MyString);
#ifdef DISPLAY_DEBUG
    std::cout << "MyString=" << MyString << std::endl;
#endif
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

void CMyFs::DisplayFS(std::string FSName, int FSLevel, EFSType FSType)
{
  for (int i = 0; i < FSLevel; ++i)
  {
    std::cout << "\t";
  }
  std::cout << FSName << "\t\t\t" << m_MapFSType2FString[FSType] << std::endl;

}

void CMyFs::DisplayAllFSRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, int FSlevel)
{
  for (auto &it : FileSystemDataPtr->m_MapDirName)
  {
    DisplayFS(it.first, FSlevel, EFSType::DIR);
    DisplayAllFSRecursive(it.second, FSlevel + 1);
  }

  for (auto &it : FileSystemDataPtr->m_MapFileName)
  {
    DisplayFS(it.first, FSlevel, EFSType::FILE);
  }

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
  if (FoundMatchDir)
  {
    DeleteDirRecursive(FileSystemDataPtr->m_MapDirName[FsToBeDeleted]);
    FileSystemDataPtr->m_MapDirName.erase(FsToBeDeleted);
  }

  return FoundMatchDir;
}

bool CMyFs::DeleteFile(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted)
{
  bool FoundMatchFile = (FileSystemDataPtr->m_MapFileName.find(FsToBeDeleted) != FileSystemDataPtr->m_MapFileName.end());
  if (FoundMatchFile)
  {
    FileSystemDataPtr->m_MapFileName.erase(FsToBeDeleted);
  }
  if (FileSystemDataPtr->m_MapFileName.size() <= 0)  //  No more elements, clear map
  {
    FileSystemDataPtr->m_MapFileName.clear();
  }
  return FoundMatchFile;
}


bool CMyFs::DeleteElementInFs(std::vector<std::string>& VecFSNames)
{
  bool ElementDeleted = false;
  if (VecFSNames.size() >= 0)
  {
    std::string FsToBeDeleted;
    std::shared_ptr<CFileSystemDir> FileSystemDataPtr;
    bool FoundMatchFs = SearchInDirs(VecFSNames, FsToBeDeleted, FileSystemDataPtr);
    if (FileSystemDataPtr)
    {
      bool Deleted = DeleteDir(FileSystemDataPtr, FsToBeDeleted);
      if (Deleted == false)
      {
        bool Deleted = DeleteFile(FileSystemDataPtr, FsToBeDeleted);
      }
    }
  }
  return ElementDeleted;
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
        FileSystemDataPtr->m_MapDirName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
        break;
      case EFSType::FILE:
        FileSystemDataPtr->m_MapFileName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
        break;
      case EFSType::LINK:
        FileSystemDataPtr->m_MapFileName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
        break;
      }
      Ok = true;
    }
  }
  return Ok;
}



bool CMyFs::AddDir(std::string DirNameFullPath)
{
  SetReturnCodeOk();
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(DirNameFullPath, VecFSNames);
  bool Ok = ValidFSName(VecFSNames, EFSType::DIR);
  if (Ok)
  {
    Ok = AddElementToFs(VecFSNames, EFSType::DIR);
    if (Ok == false)
    {
      SetReturnCode(EFSReturnCodeId::ErrCannotAddDir);
    }
  }
  else
  {
    SetReturnCode(EFSReturnCodeId::ErrDirNameIsInvalid);
  }
#ifdef DISPLAY_DEBUG
  std::cout << "Ok=" << Ok << std::endl;
#endif
  DisplayReturnCode();
  return Ok;
}

bool CMyFs::AddFile(std::string FileNameFullPath)
{
  SetReturnCodeOk();
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(FileNameFullPath, VecFSNames);
  bool Ok = ValidFSName(VecFSNames, EFSType::FILE);
  if (Ok)
  {
    Ok = AddElementToFs(VecFSNames, EFSType::FILE);
    if (Ok == false)
    {
      SetReturnCode(EFSReturnCodeId::ErrCannotAddFile);
    }
  }
  else
  {
    SetReturnCode(EFSReturnCodeId::ErrFileNameIsInvalid);
  }
#ifdef DISPLAY_DEBUG
  std::cout << "Ok=" << Ok << std::endl;
#endif
  DisplayReturnCode();

  return Ok;
}

bool CMyFs::AddLink(std::string LinkName, std::string LinkFileOrDirNameFullPath)
{
  SetReturnCodeOk();
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(LinkFileOrDirNameFullPath, VecFSNames);
  bool Ok = ValidFSName(VecFSNames, EFSType::FILE);
  if (Ok)
  {
    Ok = AddElementToFs(VecFSNames, EFSType::FILE);
    if (Ok == false)
    {
      SetReturnCode(EFSReturnCodeId::ErrCannotAddFile);
    }
  }
  else
  {
    SetReturnCode(EFSReturnCodeId::ErrFileNameIsInvalid);
  }
#ifdef DISPLAY_DEBUG
  std::cout << "Ok=" << Ok << std::endl;
#endif
  DisplayReturnCode();

  return Ok;
}

void CMyFs::DisplayAllFS()
{
  DisplayAllFSRecursive(m_FileSystemData, 0);
}


bool CMyFs::DeleteElement(std::string FileNameFullPath)
{
  SetReturnCodeOk();
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(FileNameFullPath, VecFSNames);

  bool elementDeleted = DeleteElementInFs(VecFSNames);
  if (elementDeleted == false)
  {
    SetReturnCode(EFSReturnCodeId::ErrCannotDelete);
  }


  DisplayReturnCode();
  return elementDeleted;
}

int main()
{
  CMyFs MyFs;
  //  std::string MyDir1("A/B/C");
  MyFs.AddDir("A");
  MyFs.AddDir("A");
  MyFs.AddDir("A.dat");
  MyFs.AddFile("A.dat");
  MyFs.AddFile("A.");
  MyFs.AddFile("AX_C");
  MyFs.AddFile("Z");
  MyFs.AddDir("A/B");
  MyFs.AddDir("A/B/C");
  MyFs.AddDir("A/B/D.dat");
  MyFs.AddFile("A/B/D.dat");
  MyFs.DisplayAllFS();

  MyFs.AddDir("A/C/D");
  MyFs.AddDir("A/C/E");
  MyFs.AddDir("A/C/F");
  MyFs.AddDir("A/C/G");
  MyFs.DisplayAllFS();

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

  MyFs.DeleteElement("A/C/D/Y.dat");
  MyFs.DeleteElement("A/C/D/Z.dat");
  MyFs.DeleteElement("A/C/D/S.dat");
  MyFs.DeleteElement("A/C");
  MyFs.DisplayAllFS();

  MyFs.DeleteElement("A/C/D/Y.dat");
  MyFs.DeleteElement("A.dat");
  MyFs.DeleteElement("A.dat");
  MyFs.DisplayAllFS();



  exit(0);
}
