//
// Created by unix on 18-8-15.
//

#include <sys/procfs.h>
#include "sandboxer.h"
#include <unistd.h>

std::string CE_info;

void read_src_info(std::string & src_name, problem_info_t & problem_info, const global_config_t & global_config) {
    auto root_path = global_config.source_file_dir_path;
    auto src_path = src_name;
    remove_all(src_path, " ");
    //src_path = path_cat(path_cat_path(root_path, global_config.problem_path), src_path);
    src_path = path_cat(root_path, src_path);
    std::fstream f_src;
    f_src.open(src_path);
    if (!f_src) {
        std::cerr << "Err: Source File not Found" << std::endl;
        exit(ERR_FILE_NOT_EXIST);
    }
    f_src.close();
    auto dot_pos = src_name.rfind('.');
    if (dot_pos == -1) {
        std::cerr << "Err: Source File Name is Invalid" << std::endl;
        exit(ERR_SRC_SYNTAX);
    }
    problem_info.src_path = src_name.substr(0, dot_pos);
    problem_info.src_name = problem_info.src_path;
    auto lang = src_name.substr(dot_pos + 1);
    remove_all(lang, " ");
    for (auto itr : lang) itr = (char)tolower(itr);
    if (lang == "cpp") {
        problem_info.lang = cpp;
    }
    else if (lang == "c") {
        problem_info.lang = ansic;
    }
    else if (lang == "pas") {
        problem_info.lang = pascal;
    }
    else {
        std::cerr << "Err: Unknown Language" << std::endl;
        exit(ERR_SRC_SYNTAX);
    }
    src_name = src_path;
}

int compile_and_link(const std::string& src_path, problem_info_t & problem_info, const global_config_t & global_config) {
    std::string complie_command;
    CE_info = "";
    switch (problem_info.lang) {
        case cpp: {
            complie_command = "g++";
            break;
        }
        case ansic: {
            complie_command = "gcc -fno-asm";
            break;
        }
        case pascal: {
            complie_command = "fpc";
            exit(FEATURE_UNSUPPORTED);
        }
        default:{
            std::cerr << "Unknown language type. Maybe parser failed." << std::endl;
            exit(ERR_SRC_SYNTAX);
        }
    }
    std::string output_filename = problem_info.src_path;
    //output_filename = path_cat(path_cat_path(global_config.judge_prob_root_path, global_config.problem_path),output_filename);
    output_filename = path_cat(global_config.source_file_dir_path, output_filename);
    problem_info.src_path = output_filename;
    complie_command += " " + src_path + " -o " + output_filename;
    complie_command += " " + default_cflags + " " + problem_info.cflags;
    complie_command += " 2>&1";
    //system(complie_command.c_str());
    std::cout << complie_command << std::endl;
    FILE* shell_stream;
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    shell_stream = popen(complie_command.c_str(), "r");
    if (shell_stream == nullptr) {
        std::cerr << "Err: Unknown System Command Operation Error" << std::endl;
        exit(ERR_COMPILATION);
    }
    int output_cnt = fread(buffer, sizeof(char), sizeof(buffer), shell_stream);
    if (output_cnt >= MAX_BUFFER_SIZE) {
        CE_info = "Insufficient Compilation Error. Your Program is Full of Bugs.";
    }
    else {
        CE_info = buffer;
    }
    pclose(shell_stream);
    if (!check_file(output_filename)) {
        return Compilation_Error;
    }
    return Compilation_Succeeded;
}

void clean_tmp(problem_info_t & problem_info) {
    std::string rm_command = "rm " + problem_info.src_name + " " + problem_info.name + ".out";
    system(rm_command.c_str());
    std::cout << rm_command << std::endl;
}

int judge(problem_info_t & problem_info, const global_config_t & global_config, judge_result_t& judge_result) {
    std::string copy_command = "mv " + problem_info.src_path + " " + global_config.judge_sandbox_path;
    std::cout << copy_command << std::endl;
    if (system(copy_command.c_str()) != 0) {
        std::cerr << "Error May Have Occurred" << std::endl;
    }
    chdir("./sandbox");
    for (auto itr : problem_info.data_info) {
        std::string input_filename = problem_info.name + std::to_string(itr.idx) + ".in";
        input_filename = path_cat(path_cat_path(global_config.judge_prob_root_path, global_config.problem_path),input_filename);
        std::string output_filename = problem_info.name + ".out";
        std::string ans_filename = input_filename; ans_filename = ans_filename.replace(ans_filename.length() - 3, 4, ".out");
        std::string judge_command = "./caretaker ";
        remove_space(input_filename); remove_space(ans_filename);
        //judge_command += path_cat(global_config.judge_sandbox_path, problem_info.src_name);
        judge_command += problem_info.src_name;
        judge_command += " -t " + std::to_string(itr.TL) + " -m " + std::to_string(itr.ML * 1024);
        judge_command += " < " + input_filename + " > " + output_filename;
        auto status = system(judge_command.c_str());
        std::cout << judge_command << std::endl;
        if (status == -1) {
            std::cerr << "Unknown Err with Judge Command" << std::endl;
            clean_tmp(problem_info);
            exit(ERR_SANDBOX);
        }
        int tmp = 0;
        if (WIFEXITED(status)) {
            if ((tmp = WEXITSTATUS(status) != 0) && tmp != 1) {
                std::cerr << "Unknown Err with Sandbox Program 1:" << tmp << std::endl;
                clean_tmp(problem_info);
                exit(ERR_SANDBOX);
            }
        }
        else {
            std::cerr << "Unknown Err with Sandbox Program 2" << std::endl;
            clean_tmp(problem_info);
            exit(ERR_SANDBOX);
        }
        std::fstream f_result1; f_result1.open("res.in");
        if (!f_result1 || f_result1.bad()) {
            std::cerr << "Err: SandBox No Output" << std::endl;
            clean_tmp(problem_info);
            exit(ERR_FILE_NOT_EXIST);
        }
        std::string result1; getline(f_result1, result1);
        remove_space(result1);
        if (result1 == "MLE") {
            judge_result.detial_info.push_back({-1, -1, MemoryLimitExceeded});
        }
        else if (result1 == "RTE") {
            judge_result.detial_info.push_back({-1, -1, RuntimeError});
        }
        else if (result1 == "TLE") {
            judge_result.detial_info.push_back({-1, -1, TimeLimitExceeded});
        }
        else if (result1 == "UKE") {
            judge_result.detial_info.push_back({-1, -1, UnknownError});
        }
        else if (result1 == "RSC") {
            int memory_usage, time_usage, ret_val;
            f_result1 >> ret_val >> time_usage >> memory_usage;
            std::string diff_command = "diff " + output_filename + " " + ans_filename + " -a -b";
            std::cout << diff_command << std::endl;
            auto status = system(diff_command.c_str());
            if (status == -1) {
                std::cerr << "Unknown Err with Judge Command" << std::endl;
                clean_tmp(problem_info);
                exit(ERR_SANDBOX);
            }
            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) == 0) {
                    judge_result.detial_info.push_back({time_usage, memory_usage, Accepted});
                }
                else {
                    judge_result.detial_info.push_back({time_usage, memory_usage, WrongAnswer});
                }
            }
            else {
                std::cerr << "Unknown Err with Sandbox Program" << std::endl;
                clean_tmp(problem_info);
                exit(ERR_SANDBOX);
            }
        }
        else {
            std::cerr << "Err: Runtime Result Unknown: " << result1 << std::endl;
        }
        remove("res.in");
    }
    clean_tmp(problem_info);
    chdir("../");
}

void gen_judge_res(JudgeResult_E result_e, const judge_result_t& judge_result, const problem_info_t& problem_info) {
    std::ofstream of_res;
    chdir("./sandbox");
    of_res.open(problem_info.src_name + "-result.json");
    if (!of_res) {
        std::cerr << "Err : Result File Generate Failed.Cannot Create File" << std::endl;
        exit(ERR_UNKNOWN);
    }
    std::string output = "{ \"ProblemNumber\": \"" + problem_info.problem_id + "\",";
    if (result_e == CompilationError) {
        output += "\"Status\" : \"CE\",";
        output += "\"Score\" : 0}";
        of_res << output;
        of_res.close();
        of_res.open(problem_info.src_name+"_compile_error_info.txt");
        of_res << CE_info;
        of_res.close();
        return;
    }
    output += "\"DataPoints\":[";
    bool ac_flag = true;
    int ac_point = 0;
    int cnt = 0;
    for (auto itr : judge_result.detial_info) {
        cnt ++;
        output += "{\"DataNumber\":" + std::to_string(cnt) + ", \"Status\":";
        switch (itr.result) {
            case Accepted: {
                ac_point ++;
                output += "\"AC\",";
                break;
            }
            case WrongAnswer: {
                ac_flag = false;
                output += "\"WA\",";
                break;
            }
            case MemoryLimitExceeded: {
                ac_flag = false;
                output += "\"MLE\",";
                break;
            }
            case TimeLimitExceeded: {
                output += "\"TLE\",";
                ac_flag = false;
                break;
            }
            case RuntimeError:{
                ac_flag = false;
                output += "\"RE\",";
                break;
            }
            case UnknownError:{
                ac_flag = false;
                output += "\"UKE\",";
                break;
            }
        }
        output += "\"Memory\":" + std::to_string(itr.memory) + ",\"Time\":" + std::to_string(itr.time) + "},";
    }
    output.erase(output.length()-1, 1);
    output += "],";
    if (ac_flag) {
        output += "\"Status\":\"AC\",";
    }
    else {
        output += "\"Status\":\"NAC\",";
    }
    int score = (double)ac_point / (double)problem_info.data_count * 100.0;
    output += "\"Score\":" + std::to_string(score) + "}";
    of_res << output;
    of_res.close();
    chdir("../");
}