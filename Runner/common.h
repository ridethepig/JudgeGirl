//
// Created by unix on 18-8-14.
//

#ifndef SANDBOX_RUNNER_COMMON_H
#define SANDBOX_RUNNER_COMMON_H

#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

const int ERR_FILE_NOT_EXIST = 250;
const int ERR_DIR_NOT_EXIST = 249;
const int ERR_NO_INPUT = 248;
const int ERR_CONFIG_SYNTAX = 247;
const int ERR_SRC_SYNTAX = 246;
const int FEATURE_UNSUPPORTED = 245;
const int ERR_UNKNOWN = 244;
const int ERR_COMPILATION = 243;
const int ERR_SANDBOX = 242;
const int ERR_ENV = 241;
inline std::string path_cat(std::string path, const std::string &file) {
    if (path[path.length() - 1] != '/') path += '/';
    return path + file;
}

inline std::string path_cat_path(std::string path, const std::string &path2) {
    if (path[path.length() - 1] != '/') path += '/';
    return path + path2;
}

inline void remove_all(std::string & str, const std::string del) {
    size_t _begin_ = 0;
    _begin_ = str.find(del, _begin_);
    while (_begin_ != -1) {
        str.replace(_begin_, 1, "");
        _begin_ = str.find(del, _begin_);
    }
}
inline void remove_space(std::string & str) {
    remove_all(str, "\n");
    remove_all(str, "\t");
    remove_all(str, " ");
}
inline bool check_dir(std::string & Path) {
    std::string path = Path;
    size_t len = path.length();
    if(path[len-1] != '/' && path[len-1] != '\\')
        path.append("/");
    path.append("000.tmp");
    std::ofstream outf;
    outf.open(path);
    bool flag = outf;
    if(outf)
    {
        outf.close();
        remove(path.c_str());
    }
    return flag;
}

inline bool check_file(std::string &path) {
    std::fstream f_check;
    f_check.open(path);
    if (!f_check) {
        return false;
    }
    f_check.close();
    return true;
}

inline int conver_string_int(const std::string & str) {
    int ret;
    std::stringstream ss(str);
    ss >> ret;
    return ret;
}

#endif //SANDBOX_RUNNER_COMMON_H
