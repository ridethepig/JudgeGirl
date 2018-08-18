//
// Created by unix on 18-8-15.
//

#ifndef SANDBOX_RUNNER_CONFIGUREPARSER_H
#define SANDBOX_RUNNER_CONFIGUREPARSER_H

#include "common.h"

typedef struct DATA_POINT_INFO {
    int ML; // given by MB
    int TL; // given by ms
    int idx;
} data_point_info_t;

enum Language {
    ansic = 1,
    cpp,
    pascal
};

typedef struct PROBLEM_INFO {
    std::string name;
    std::string src_path;
    std::string src_name;
    std::string problem_id;
    int lang;
    int data_count;
    std::string cflags;
    std::vector<data_point_info_t> data_info;
} problem_info_t;

typedef struct GLOBAL_CONFIG{
    std::string judge_sandbox_path;
    std::string judge_prob_root_path;
    std::string judge_prob_config_file;
    std::string problem_path;
    std::string source_file_dir_path;
} global_config_t;

const std::string config_path = "./sandbox.conf";

void read_config_local(global_config_t & global_config);
void read_config_problem(std::string problem_id, problem_info_t & problem_info, const global_config_t &);

#endif //SANDBOX_RUNNER_CONFIGUREPARSER_H
