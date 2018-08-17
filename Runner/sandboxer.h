//
// Created by unix on 18-8-15.
//

#ifndef SANDBOX_RUNNER_SANDBOXER_H
#define SANDBOX_RUNNER_SANDBOXER_H

#include "common.h"
#include "configureparser.h"
#include <cstring>

const int Compilation_Succeeded = 0;
const int Compilation_Error = 1;

enum JudgeResult_E {
    Accepted= 1,
    WrongAnswer,
    RuntimeError,
    CompilationError,
    MemoryLimitExceeded,
    TimeLimitExceeded,
    UnknownError
};

typedef struct DATA_POINT_RES{
    int time;
    int memory;
    JudgeResult_E result;
} data_point_res_t;

typedef struct JUDGE_RESULT{
    int score;
    std::vector<data_point_res_t> detial_info;
} judge_result_t;

const std::string default_cflags = "-static";
void read_src_info(std::string &, problem_info_t &, const global_config_t &);
int compile_and_link(const std::string &, problem_info_t &, const global_config_t &);
int judge(problem_info_t &, const global_config_t &, judge_result_t &);
void gen_judge_res(JudgeResult_E, const judge_result_t&, const problem_info_t&);
void clean_tmp(problem_info_t&);
#endif //SANDBOX_RUNNER_SANDBOXER_H
