#ifndef  CROSS_COMMON_H_
#define  CROSS_COMMON_H_
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <time.h>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <cassert>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <time.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
namespace batch
{
inline void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
};
inline std::pair<std::string, std::string> SplitPath(std::string &path)
{
    ReplaceStringInPlace(path,"\\","/");
    std::string::size_type last_sep = path.find_last_of("/");
    if (last_sep != std::string::npos) {
        return std::make_pair(std::string(path.begin(), path.begin() + last_sep),
                              std::string(path.begin() + last_sep + 1, path.end()));
    }
    return std::make_pair(".", path);
};
inline std::string GetCurrentDir()
{
  char result[ FILENAME_MAX ];
  int i;
  #ifdef _WIN32
    int cnt = GetModuleFileName( NULL, result, FILENAME_MAX );
  #else
    ssize_t cnt = readlink( "/proc/self/exe", result, FILENAME_MAX );
  #endif
  for (i = cnt; i >=0; --i) {
      if (result[i] == '/') {
          result[i+1] = '\0';
          break;
      }
  }
  return std::string( result );
};

///分割字符串
///@param s  字符串
///@param delim 分隔符
///@param elems  分割完的字符串
///@return 分割完的字符串
inline std::vector<std::string>& split(const std::string& s,
                                       char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;

    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
};
///分割字符串
///@param s  字符串
///@param delim 分隔符
///@return 分割完的字符串
inline std::vector<std::string> split(const std::string& s,
                                      char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
};
///组合字符串
///@param elements 字符串数组
///@param separator 分隔符
///@return 合并的字符串
inline std::string join(const std::vector<std::string>&
                        elements, const char* const separator)
{
    switch(elements.size()) {
    case 0:
        return "";

    case 1:
        return elements[0];

    default:
        std::ostringstream os;
        std::copy(elements.begin(), elements.end() - 1,
                  std::ostream_iterator<std::string>(os, separator));
        os << *elements.rbegin();
        return os.str();
    }
};

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
inline const std::string currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
};
//from https://bitbucket.org/szx/glob LICENSE MIT
#ifdef _WIN32
class Glob
{
public:
    Glob(const std::string &pattern):
        ok_(false),
        find_handle_(INVALID_HANDLE_VALUE)
    {
        find_handle_ = FindFirstFile(pattern.c_str(), &find_data_);
        ok_ = find_handle_ != INVALID_HANDLE_VALUE;
    };
    ~Glob()
    {
        if (find_handle_ != INVALID_HANDLE_VALUE) {
            FindClose(find_handle_);
        }
    };

    inline std::string GetFileName() const
    {
        assert(ok_);
        return find_data_.cFileName;
    };

    inline operator bool() const
    {
        return ok_;
    };

    inline bool Next()
    {
        ok_ = FindNextFile(find_handle_, &find_data_) != 0;
        return ok_;
    };

private:
    Glob(const Glob &);
    void operator=(const Glob &);

private:
    bool ok_;
    HANDLE find_handle_;
    WIN32_FIND_DATA find_data_;
};
#else
class Glob
{
public:
    Glob(const std::string &pattern)
        : dir_(0),
          dir_entry_(0)
    {
        std::pair<std::string, std::string> dir_and_mask = SplitPath(pattern);

        dir_ = opendir(dir_and_mask.first.c_str());
        pattern_ = dir_and_mask.second;

        if (dir_ != 0) {
            Next();
        }
    };
    ~Glob()
    {
        if (dir_ != 0) {
            closedir(dir_);
        }
    };

    inline std::string GetFileName() const
    {
        assert(dir_entry_ != 0);
        return dir_entry_->d_name;
    };

    inline operator bool() const
    {
        return dir_entry_ != 0;
    };

    inline bool Next()
    {
        while ((dir_entry_ = readdir(dir_)) != 0) {
            if (!fnmatch(pattern_.c_str(), dir_entry_->d_name,
                         FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD)) {
                return true;
            }
        }
        return false;
    };

private:
    Glob(const Glob &);
    inline void operator=(const Glob &);

private:
    std::string pattern_;
    DIR *dir_;
    struct dirent *dir_entry_;
};
#endif

} // namespace
#endif
