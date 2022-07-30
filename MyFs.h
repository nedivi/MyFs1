
#include <string>
#include <map>
#include <vector>
#include <set>


class CFileSystemDir;

namespace FsNs
{
  enum class EFSType { FILE, DIR, LINK };  // File system type
  const int MAX_FS_TYPES = 3;  // 3 types of FS (File/Link/Dir)

  // Directory structure
  typedef std::map<std::string, std::shared_ptr<CFileSystemDir> > TMapDir;  // Map between the directory name to it's subdirectory
  typedef TMapDir::iterator  TMapDirIter;

  // File structure
  typedef std::set<std::string > TSetFile;
  typedef TSetFile::iterator     TSetFileIter;

  // Link structure
  typedef std::map<std::string, std::string> TMapLink;
  typedef TMapLink::iterator     TMapLinkIter;

  enum class EFSReturnCodeId {
    Ok,
    ErrFileNameIsInvalid,
    ErrCannotAddFile,
    ErrDirNameIsInvalid,
    ErrCannotAddDir,
    ErrLinkNameIsInvalid,
    ErrCannotAddLink,
    ErrCannotDelete
  };


}


#ifndef _MYFS_H_
#define _MYFS_H_


// Files System Data
class CFileSystemDir {
public:
  FsNs::TMapDir  m_MapDirName;               // Using map to achieve Log(N) complexity in Insert/Delete/Find directory
  FsNs::TSetFile m_SetFileName;              // Using set to achieve Log(N) complexity in Insert/Delete/Find file
  FsNs::TMapLink m_MapLinkName;              // Using map to achieve Log(N) complexity in Insert/Delete/Find link

public:
  // Constructor
  CFileSystemDir() {}


};

class CMyFs {
private:
  // Variables

  std::map< FsNs::EFSReturnCodeId, std::string> MapReturnCodeId2Str = {
    {
      {FsNs::EFSReturnCodeId::Ok,                       "Ok"},
      {FsNs::EFSReturnCodeId::ErrFileNameIsInvalid,     "File name is invalid"},
      {FsNs::EFSReturnCodeId::ErrCannotAddFile,         "Cannot add file, illegal path"},
      {FsNs::EFSReturnCodeId::ErrDirNameIsInvalid,      "Directory name is invalid"},
      {FsNs::EFSReturnCodeId::ErrCannotAddDir,          "Cannot add directory, illegal path"},
      {FsNs::EFSReturnCodeId::ErrLinkNameIsInvalid,     "Link name is invalid"},
      {FsNs::EFSReturnCodeId::ErrCannotAddLink,         "Cannot add link, illegal path"},
      {FsNs::EFSReturnCodeId::ErrCannotDelete,          "Cannot delete element"},

    }
  };

  std::map<FsNs::EFSType, std::string> m_MapFSType2FString =
  {
    {FsNs::EFSType::FILE, "file"},
    {FsNs::EFSType::DIR,  "dir"},
    {FsNs::EFSType::LINK, "link"}
  };
  const std::string LINK_SIMBOL = "@";
  const std::string LINK_NOT_EXIST = "!";
  const std::string LINK_EXIST = "";

  std::shared_ptr<CFileSystemDir> m_FileSystemData;
  FsNs::EFSReturnCodeId m_FSReturnCodeId = FsNs::EFSReturnCodeId::Ok;
  typedef std::map<int, int> TMapStatFSLevel2Count;

  class FSStatistics
  {
  private:
    TMapStatFSLevel2Count m_MapStat[FsNs::MAX_FS_TYPES];
    int m_MaxLevel = -1;  // Maximum level of directory
  public:
    FSStatistics();
    void ClearAll();
    void AddSum(FsNs::EFSType FSType, int Level, int count);
    int GetCount(FsNs::EFSType FSType, int Level);
    int GetCountAll(FsNs::EFSType FSType);
  };
  FSStatistics m_FSStatistics;

  // Functions
  void SetReturnCodeOk() { m_FSReturnCodeId = FsNs::EFSReturnCodeId::Ok; }
  void SetReturnCode(FsNs::EFSReturnCodeId FSReturnCodeId) { m_FSReturnCodeId = FSReturnCodeId; }
  void DisplayReturnCode();

  void SplitFullPathToVector(std::string& FileNameFullPath, std::vector<std::string> &MyVec);
  std::shared_ptr<CFileSystemDir> GetDirInFs(std::vector<std::string>& VecFSNames);
  bool SearchInDirs(std::vector<std::string>& VecFSNames, std::string& FsToBeAdded, std::shared_ptr<CFileSystemDir>& FileSystemDataPtr);
  void DisplayFS(std::string FSName, int FSLevel, FsNs::EFSType FSType);
  void DisplayLink(std::string LinkName, std::string FStoBeLinked, std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, int FSLevel);
  void StatAllFSRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, int FSlevel);
  void DisplayAllFSRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, int FSlevel);
  void DeleteDirRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr);
  bool DeleteDirInFS(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted);
  bool DeleteFileInFS(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted);
  bool DeleteLinkInFS(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted);
  bool DeleteElementInFs(std::vector<std::string>& VecFSNames, FsNs::EFSType FSType);
  bool AddLinkToFs(std::vector<std::string>& VecFSNames, std::string LinkName, std::string LinkedElement);
  bool AddFileOrDirToFs(std::vector<std::string>& VecFSNames, FsNs::EFSType FSType);
  bool ValidFileName(std::string& FileName);
  bool ValidLinkName(std::string& FileName);
  bool ValidDirName(std::string& FileName);

  bool ValidDirsName(std::vector<std::string>& VecFSNames);
  bool ValidFSName(std::vector<std::string>& VecFSNames, FsNs::EFSType FSType);
  bool DeleteElement(std::string FileNameFullPath, FsNs::EFSType FSType);

public:
  CMyFs();
  ~CMyFs();
  bool AddDir(std::string DirNameFullPath);
  bool AddFile(std::string FileNameFullPath);
  //  bool AddLink(std::string LinkName, std::string LinkFileOrDirNameFullPath);
  bool AddLink(std::string PathFileOfTheLink, std::string LinkName, std::string LinkedElement);
  bool DeleteDir(std::string FileNameFullPath);
  bool DeleteFile(std::string FileNameFullPath);
  bool DeleteLink(std::string FileNameFullPath);
  void DisplayAllFS();
  void CollectStatisticsAllFS();
  void DisplaySumOfAllFS();

};


#endif