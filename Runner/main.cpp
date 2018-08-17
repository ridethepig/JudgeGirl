#include "common.h"
#include "configureparser.h"
#include "sandboxer.h"

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        std::cerr << "Too Few Args" << std::endl;
        return ERR_NO_INPUT;
    }
    std::string problem_id = argv[1];
    std::string src_name = argv[2];
    problem_info_t problem_info;
    global_config_t global_config;
    judge_result_t judge_result;
    global_config.problem_path = problem_id;
    read_config_local(global_config);
    read_config_problem(problem_id, problem_info, global_config);

    read_src_info(src_name, problem_info, global_config);
    auto compile_res = compile_and_link(src_name, problem_info, global_config);
    if (compile_res == Compilation_Error) {
        gen_judge_res(CompilationError, judge_result, problem_info);
        return 0;
    }

    judge(problem_info, global_config, judge_result);
    gen_judge_res(Accepted, judge_result, problem_info);
    return 0;
}