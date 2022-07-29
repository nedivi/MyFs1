#ifndef _MYFS_H_
#define _MYFS_H_

#include <string>
#include <map>
#include <vector>


class CFileSystemDir;

namespace FsNs
{
  enum class EFSType { FILE, DIR, LINK };  // File system type
  //typedef  std::string TFILE_NAME;
  //typedef std::pair<std::string, EFileType> TFsAttr;

  typedef std::map<std::string, std::shared_ptr<CFileSystemDir> > TMapFS;
  typedef TMapFS::iterator  TMapFSIter;

  enum class EFSReturnCodeId {
    Ok,
    ErrFileNameIsInvalid,
    ErrCannotAddFile,
    ErrDirNameIsInvalid,
    ErrCannotAddDir,
    ErrCannotDelete
  };

  std::map< EFSReturnCodeId, std::string> MapReturnCodeId2Str = {
    {
      {EFSReturnCodeId::Ok,                       "Ok"},
      {EFSReturnCodeId::ErrFileNameIsInvalid,     "File name is invalid"},
      {EFSReturnCodeId::ErrCannotAddFile,         "Cannot add file, illegal path"},
      {EFSReturnCodeId::ErrDirNameIsInvalid,      "Directory name is invalid"},
      {EFSReturnCodeId::ErrCannotAddDir,          "Cannot add directory, illegal path"},
      {EFSReturnCodeId::ErrCannotDelete,          "Cannot delete element"},

    }
  };
}


// Files System Data
class CFileSystemDir {
public:
  FsNs::TMapFS m_MapDirName;               // Using map to achieve Log(N) complexity
  FsNs::TMapFS m_MapFileName;              // Map for file name

public:
  // Constructor
  CFileSystemDir() {}


};

class CMyFs {
private:
  // Variables
  std::map<FsNs::EFSType, std::string> m_MapFSType2FString =
  {
    {FsNs::EFSType::FILE, "file"},
    {FsNs::EFSType::DIR,  "dir"},
    {FsNs::EFSType::LINK, "link"}

  };

  std::shared_ptr<CFileSystemDir> m_FileSystemData;
  FsNs::EFSReturnCodeId m_FSReturnCodeId = FsNs::EFSReturnCodeId::Ok;

  // Functions
  void SetReturnCodeOk() { m_FSReturnCodeId = FsNs::EFSReturnCodeId::Ok; }
  void SetReturnCode(FsNs::EFSReturnCodeId FSReturnCodeId) { m_FSReturnCodeId = FSReturnCodeId; }
  void DisplayReturnCode();

  void SplitFullPathToVector(std::string& FileNameFullPath, std::vector<std::string> &MyVec);
  std::shared_ptr<CFileSystemDir> GetDirInFs(std::vector<std::string>& VecFSNames);
  bool SearchInDirs(std::vector<std::string>& VecFSNames, std::string& FsToBeAdded, std::shared_ptr<CFileSystemDir>& FileSystemDataPtr);
  void DisplayFS(std::string FSName, int FSLevel, FsNs::EFSType FSType);
  void DisplayAllFSRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, int FSlevel);
  void DeleteDirRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr);
  bool DeleteDir(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted);
  bool DeleteFile(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted);
  bool DeleteElementInFs(std::vector<std::string>& VecFSNames);
  bool AddElementToFs(std::vector<std::string>& VecFSNames, FsNs::EFSType FSType);
  bool ValidFileName(std::string& FileName);
  bool ValidLinkName(std::string& FileName);
  bool ValidDirName(std::string& FileName);

  bool ValidDirsName(std::vector<std::string>& VecFSNames);
  bool ValidFSName(std::vector<std::string>& VecFSNames, FsNs::EFSType FSType);
public:
  CMyFs();
  bool AddDir(std::string DirNameFullPath);
  bool AddFile(std::string FileNameFullPath);
  bool AddLink(std::string LinkName, std::string LinkFileOrDirNameFullPath);
  void DisplayAllFS();
  bool DeleteElement(std::string FileNameFullPath);
};


#endif