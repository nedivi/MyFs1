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

CMyFs::~CMyFs()
{
  DeleteDirRecursive(m_FileSystemData);
}

// File name validation , Currently alfa-beta with 2 sections (like "doc.txt")
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

// Directory name validation , Currently alfa-beta with 1 sections (like "doc")
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
  //  size_t lastElementCnt = (FSType == EFSType::LINK) ? VecFSNames.size() : VecFSNames.size() - 1;
  size_t lastElementCnt = VecFSNames.size() - 1;

  for (auto &it : VecFSNames)
  {
    if (cnt < lastElementCnt)
    {
      Ok = ValidDirName(it);
    }
    else
    {
      switch (FSType)
      {
      case EFSType::DIR:
      case EFSType::LINK:
        Ok = ValidDirName(it);
        break;
      case EFSType::FILE:
        Ok = ValidFileName(it);
        break;
      default:
        Ok = false;
        break;

      }
      if (Ok == false)
      {
        break;
      }
    }
    ++cnt;
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
    TMapDirIter CfsIter = FileSystemDataPtr->m_MapDirName.find(MyDirIter);
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
  VecFSNames.pop_back();            // Take out the dir name to be added 
  FileSystemDataPtr = GetDirInFs(VecFSNames);
  return FoundMatchFs;
}

// Display FS In hierarchy mode 
void CMyFs::DisplayFS(std::string FSName, int FSLevel, EFSType FSType)
{
  for (int i = 0; i < FSLevel; ++i)
  {
    std::cout << "\t";
  }
  std::cout << FSName << "\t" << m_MapFSType2FString[FSType] << std::endl;
}

void CMyFs::DisplayLink(std::string LinkName, std::string FStoBeLinked, std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, int FSLevel)
{
  bool FoundMatchFs = (FileSystemDataPtr->m_MapDirName.find(FStoBeLinked) != FileSystemDataPtr->m_MapDirName.end());
  if (FoundMatchFs == false)
  {
    FoundMatchFs = (FileSystemDataPtr->m_SetFileName.find(FStoBeLinked) != FileSystemDataPtr->m_SetFileName.end());
  }
  std::string StrExistLink = FoundMatchFs ? LINK_EXIST : LINK_NOT_EXIST;
  std::string LinkNameToDisplay = StrExistLink + LINK_SIMBOL + LinkName;
  DisplayFS(LinkNameToDisplay, FSLevel, EFSType::LINK);

}

void CMyFs::DisplayAllFSRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, int FSlevel)
{
  for (auto &it : FileSystemDataPtr->m_MapDirName)
  {
    DisplayFS(it.first, FSlevel, EFSType::DIR);
    DisplayAllFSRecursive(it.second, FSlevel + 1);
  }

  for (auto &it : FileSystemDataPtr->m_SetFileName)
  {
    DisplayFS(it, FSlevel, EFSType::FILE);
  }

  for (auto &it : FileSystemDataPtr->m_MapLinkName)
  {
    DisplayLink(it.first, it.second, FileSystemDataPtr, FSlevel);
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
  FileSystemDataPtr->m_SetFileName.clear();
  FileSystemDataPtr->m_MapLinkName.clear();
}

bool CMyFs::DeleteDirInFS(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted)
{
  FsNs::TMapDir& MapDirName = FileSystemDataPtr->m_MapDirName;
  bool FoundMatchDir = (MapDirName.find(FsToBeDeleted) != MapDirName.end());
  if (FoundMatchDir)
  {
    DeleteDirRecursive(MapDirName[FsToBeDeleted]);
    MapDirName.erase(FsToBeDeleted);
  }
  if (MapDirName.size() <= 0)  //  No more elements, clear map
  {
    MapDirName.clear();
  }

  return FoundMatchDir;
}

bool CMyFs::DeleteFileInFS(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted)
{
  FsNs::TSetFile& SetFileName = FileSystemDataPtr->m_SetFileName;
  bool FoundMatchFile = (SetFileName.find(FsToBeDeleted) != SetFileName.end());
  if (FoundMatchFile)
  {
    SetFileName.erase(FsToBeDeleted);
  }
  if (SetFileName.size() <= 0)  //  No more elements, clear map
  {
    SetFileName.clear();
  }
  return FoundMatchFile;
}

bool CMyFs::DeleteLinkInFS(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted)
{
  FsNs::TMapLink& MapLinkName = FileSystemDataPtr->m_MapLinkName;
  bool FoundMatchFile = (MapLinkName.find(FsToBeDeleted) != MapLinkName.end());
  if (FoundMatchFile)
  {
    MapLinkName.erase(FsToBeDeleted);
  }
  if (MapLinkName.size() <= 0)  //  No more elements, clear map
  {
    MapLinkName.clear();
  }
  return FoundMatchFile;
}


bool CMyFs::DeleteElementInFs(std::vector<std::string>& VecFSNames, EFSType FSType)
{
  bool ElementDeleted = false;
  if (VecFSNames.size() >= 0)
  {
    std::string FsToBeDeleted;
    std::shared_ptr<CFileSystemDir> FileSystemDataPtr;
    bool FoundMatchFs = SearchInDirs(VecFSNames, FsToBeDeleted, FileSystemDataPtr);
    if (FileSystemDataPtr)
    {
      switch (FSType)
      {
      case EFSType::DIR:
        ElementDeleted = DeleteDirInFS(FileSystemDataPtr, FsToBeDeleted);
        break;
      case EFSType::FILE:
        ElementDeleted = DeleteFileInFS(FileSystemDataPtr, FsToBeDeleted);
        break;
      case EFSType::LINK:
        ElementDeleted = DeleteLinkInFS(FileSystemDataPtr, FsToBeDeleted);
        break;

      }
    }
  }
  return ElementDeleted;
}

bool CMyFs::AddLinkToFs(std::vector<std::string>& VecFSNames, std::string LinkName, std::string LinkedElement)
{
  bool Ok = false;
  if (VecFSNames.size() >= 0)
  {
    std::shared_ptr<CFileSystemDir> FileSystemDataPtr = GetDirInFs(VecFSNames);
    if (FileSystemDataPtr)  // Legal path to add element
    {
      // Check if link is already exist in this directory
      bool FoundMatchFs = (FileSystemDataPtr->m_MapLinkName.find(LinkName) != FileSystemDataPtr->m_MapLinkName.end());
      if (FoundMatchFs == false)
      {
        FileSystemDataPtr->m_MapLinkName[LinkName] = LinkedElement;
        Ok = true;
      }
    }
  }
  return Ok;
}

bool CMyFs::AddFileOrDirToFs(std::vector<std::string>& VecFSNames, EFSType FSType)
{
  bool Ok = false;
  if (VecFSNames.size() >= 0)
  {
    std::string FsToBeAdded;
    std::shared_ptr<CFileSystemDir> FileSystemDataPtr;
    bool FoundMatchFs = false;
    SearchInDirs(VecFSNames, FsToBeAdded, FileSystemDataPtr);
    if (FileSystemDataPtr)  // Legal path to add element
    {
      // Now look if there is a file or directory that match this new FS name
      FoundMatchFs = (FileSystemDataPtr->m_MapDirName.find(FsToBeAdded) != FileSystemDataPtr->m_MapDirName.end());
      if (FoundMatchFs == false)
      {
        FoundMatchFs = (FileSystemDataPtr->m_SetFileName.find(FsToBeAdded) != FileSystemDataPtr->m_SetFileName.end());
      }
    }
    if ((FoundMatchFs == false) && (FileSystemDataPtr))
    {
      switch (FSType)
      {
      case EFSType::DIR:
        Ok = true;
        FileSystemDataPtr->m_MapDirName[FsToBeAdded] = std::make_shared<CFileSystemDir>();
        break;
      case EFSType::FILE:
        Ok = true;
        FileSystemDataPtr->m_SetFileName.insert(FsToBeAdded);
        break;
      }
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
    Ok = AddFileOrDirToFs(VecFSNames, EFSType::DIR);
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
    Ok = AddFileOrDirToFs(VecFSNames, EFSType::FILE);
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

bool CMyFs::AddLink(std::string PathFileOfTheLink, std::string LinkName, std::string LinkedElement)
{
  SetReturnCodeOk();
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(PathFileOfTheLink, VecFSNames);
  bool Ok = true;
  if (VecFSNames.size() > 0)
  {
    Ok = ValidFSName(VecFSNames, EFSType::LINK);
  }
  if (Ok)
  {
    Ok = AddLinkToFs(VecFSNames, LinkName, LinkedElement);
    if (Ok == false)
    {
      SetReturnCode(EFSReturnCodeId::ErrCannotAddLink);
    }
  }
  else
  {
    SetReturnCode(EFSReturnCodeId::ErrLinkNameIsInvalid);
  }
#ifdef DISPLAY_DEBUG
  std::cout << "Ok=" << Ok << std::endl;
#endif
  DisplayReturnCode();

  return Ok;
}

bool CMyFs::DeleteDir(std::string FileNameFullPath)
{
  return DeleteElement(FileNameFullPath, EFSType::DIR);
}

bool CMyFs::DeleteFile(std::string FileNameFullPath)
{
  return DeleteElement(FileNameFullPath, EFSType::FILE);
}

bool CMyFs::DeleteLink(std::string FileNameFullPath)
{
  return DeleteElement(FileNameFullPath, EFSType::LINK);
}

void CMyFs::DisplayAllFS()
{
  DisplayAllFSRecursive(m_FileSystemData, 0);
}


bool CMyFs::DeleteElement(std::string FileNameFullPath, EFSType FSType)
{
  SetReturnCodeOk();
  std::vector<std::string> VecFSNames;
  SplitFullPathToVector(FileNameFullPath, VecFSNames);

  bool elementDeleted = DeleteElementInFs(VecFSNames, FSType);
  if (elementDeleted == false)
  {
    SetReturnCode(EFSReturnCodeId::ErrCannotDelete);
  }
  DisplayReturnCode();
  return elementDeleted;
}

// Statistics on FS data
CMyFs::FSStatistics::FSStatistics()
{
  ClearAll();
}

void CMyFs::FSStatistics::ClearAll()
{
  for (auto &it : m_MapStat)
  {
    it.clear();
  }
}

void CMyFs::FSStatistics::AddCount(FsNs::EFSType FSType, int Level, int count)
{

  TMapStatFSLevel2Count& MapStatFSLevel2Count = m_MapStat[(int)FSType];
  if (MapStatFSLevel2Count.find(Level) == MapStatFSLevel2Count.end())
  {
    MapStatFSLevel2Count[Level] = count;
  }
  else
  {
    MapStatFSLevel2Count[Level] = count;
  }
}

int CMyFs::FSStatistics::GetCount(FsNs::EFSType FSType, int Level)
{

  TMapStatFSLevel2Count& MapStatFSLevel2Count = m_MapStat[(int)FSType];
  bool foundKey = (MapStatFSLevel2Count.find(Level) != MapStatFSLevel2Count.end());
  return foundKey ? MapStatFSLevel2Count[Level] : 0;
}

int CMyFs::FSStatistics::GetCountAll(FsNs::EFSType FSType)
{

  TMapStatFSLevel2Count& MapStatFSLevel2Count = m_MapStat[(int)FSType];
  int count = 0;
  for (auto &it : MapStatFSLevel2Count)
  {
    count += it.second;
  }
  return count;
}
