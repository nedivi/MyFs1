#ifndef _MYFS_H_
#define _MYFS_H_

#include <string>
#include <map>
#include <vector>

enum class EFSType { FILE, DIR, LINK };  // File system type
//typedef  std::string TFILE_NAME;
//typedef std::pair<std::string, EFileType> TFsAttr;
class CFileSystemDir;

//typedef std::map<std::string, CFileSystemDir*>  TMapFS;
typedef std::map<std::string, std::shared_ptr<CFileSystemDir> > TMapFS;
typedef TMapFS::iterator  TMapFSIter;

// Files System Data
class CFileSystemDir {
public:
  TMapFS m_MapDirName;  // Using map to achieve Log(N) complexity
  TMapFS m_MapFileName;              // Map for file name

public:
  // Constructor
  CFileSystemDir() {}


};

class CMyFs {
private:
  std::shared_ptr<CFileSystemDir> m_FileSystemData;

  void SplitFullPathToVector(std::string& FileNameFullPath, std::vector<std::string> &MyVec);
  std::shared_ptr<CFileSystemDir> DeleteElementInFS(std::vector<std::string>& VecFSNames);
  std::shared_ptr<CFileSystemDir> GetDirInFs(std::vector<std::string>& VecFSNames);
  bool SearchInDirs(std::vector<std::string>& VecFSNames, std::string& FsToBeAdded, std::shared_ptr<CFileSystemDir>& FileSystemDataPtr);
  void DeleteDirRecursive(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr);
  bool DeleteDir(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted);
  bool DeleteFile(std::shared_ptr<CFileSystemDir>& FileSystemDataPtr, std::string& FsToBeDeleted);
  bool DeleteElementInFs(std::vector<std::string>& VecFSNames);
  bool AddElementToFs(std::vector<std::string>& VecFSNames, EFSType FSType);
  bool ValidFileName(std::string& FileName);
  bool ValidDirName(std::string& FileName);

  bool ValidDirsName(std::vector<std::string>& VecFSNames);
  bool ValidFSName(std::vector<std::string>& VecFSNames, EFSType FSType);
public:
  CMyFs();
  bool AddDir(std::string FileNameFullPath);
  bool AddFile(std::string FileNameFullPath);
  bool DeleteElement(std::string FileNameFullPath);
};


#endif