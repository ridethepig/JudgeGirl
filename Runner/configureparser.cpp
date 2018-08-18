//
// Created by unix on 18-8-15.
//

#include "configureparser.h"

void read_config_local(global_config_t & global_config) {
    std::fstream f_config;

    std::string judge_src_dir_path;
    std::string judge_prob_root_path;
    std::string judge_prob_config_file;

    f_config.open(config_path);
    if (f_config.bad()) {
        std::cerr << "No Configure File Found." << std::endl;
        exit(ERR_FILE_NOT_EXIST);
    }


    getline(f_config, judge_src_dir_path);
    remove_all(judge_src_dir_path, "\n");
    remove_all(judge_src_dir_path, " ");

    getline(f_config, judge_prob_root_path);
    remove_all(judge_prob_root_path, "\n");
    remove_all(judge_prob_root_path, " ");

    getline(f_config, judge_prob_config_file);
    remove_all(judge_prob_config_file, "\n");
    remove_all(judge_prob_config_file, " ");

    if (!check_dir(judge_src_dir_path)) {
        std::cerr << "Source Directory not Exist." << std::endl;
        exit(ERR_DIR_NOT_EXIST);
    }
    if (!check_dir(judge_prob_root_path)) {
        std::cerr << "Problem Directory not Exist." << std::endl;
        exit(ERR_DIR_NOT_EXIST);
    }
    f_config.close();
    global_config.judge_prob_root_path = judge_prob_root_path;
    global_config.judge_prob_config_file = judge_prob_config_file;
    global_config.judge_sandbox_path = "./sandbox";
    global_config.source_file_dir_path = judge_src_dir_path;
}

void read_config_problem(std::string problem_id, problem_info_t & problem_info, const global_config_t & global_config) {
    std::fstream f_prob_config;
    problem_info.problem_id = problem_id;
    auto judge_prob_config_file = global_config.judge_prob_config_file;
    problem_id = path_cat(global_config.judge_prob_root_path, problem_id);
    if (!check_dir(problem_id)) {
        std::cerr << "Problem Id not Found" << std::endl;
        exit(ERR_DIR_NOT_EXIST);
    }
    judge_prob_config_file = path_cat(problem_id, judge_prob_config_file);
    f_prob_config.open(judge_prob_config_file);
    if (!f_prob_config || f_prob_config.bad()) {
        std::cerr << "Problem Info not Found" << std::endl;
        exit(ERR_FILE_NOT_EXIST);
    }
    std::string config_line;
    while (getline(f_prob_config, config_line)) {
        auto equ_pos = config_line.find('=');
        remove_all(config_line, "\n");
        remove_all(config_line, "\t");
        if (equ_pos == -1) {
            f_prob_config.close();
            std::cerr << "Problem Config File Syntax Err: No symbol \'=\' found" << std::endl;
            exit(ERR_CONFIG_SYNTAX);
        }
        std::string config_key = config_line.substr(0, equ_pos);
        remove_all(config_key, " ");
        if (config_key == "name") {
            problem_info.name = config_line.substr(equ_pos + 1);
        }
        else if (config_key == "cflags") {
            problem_info.cflags = config_line.substr(equ_pos + 1);
        }
        else if (config_key == "count") {
            std::stringstream sst(config_line.substr(equ_pos + 1));
            sst >> problem_info.data_count;
            if (problem_info.data_count == 0) {
                f_prob_config.close();
                std::cerr << "Err when Reading Count." << std::endl;
                exit(ERR_CONFIG_SYNTAX);
            }
        }
        else if (config_key == "index") {
            int dataidx = conver_string_int(config_line.substr(equ_pos + 1));
            if (dataidx == 0) {
                f_prob_config.close();
                std::cerr << "Err when Reading Data Number." << std::endl;
                exit(ERR_CONFIG_SYNTAX);
            }

            getline(f_prob_config, config_line);
            remove_space(config_line);
            equ_pos = config_line.find('=');
            if (equ_pos == -1) {
                f_prob_config.close();
                std::cerr << "Problem Config File Syntax Err: No symbol \'=\' found" << std::endl;
                exit(ERR_CONFIG_SYNTAX);
            }
            int memory_lmt = conver_string_int(config_line.substr(equ_pos + 1));
            if (memory_lmt == 0) {
                f_prob_config.close();
                std::cerr << "Err when Reading Memory Limit."
                        << "Data point:"
                        << dataidx << std::endl;
                exit(ERR_CONFIG_SYNTAX);
            }

            getline(f_prob_config, config_line);
            remove_space(config_line);
            equ_pos = config_line.find('=');
            if (equ_pos == -1) {
                f_prob_config.close();
                std::cerr << "Problem Config File Syntax Err: No symbol \'=\' found" << std::endl;
                exit(ERR_CONFIG_SYNTAX);
            }
            int time_lmt = conver_string_int(config_line.substr(equ_pos + 1));
            if (time_lmt == 0) {
                f_prob_config.close();
                std::cerr << "Err when Reading Time Limit."
                          << "Data point:"
                          << dataidx << std::endl;
                exit(ERR_CONFIG_SYNTAX);
            }
            problem_info.data_info.push_back({memory_lmt, time_lmt, dataidx});
        }
        else {
            std::cerr << "Fatal : Unknown Symbol Read: " << config_line <<std::endl;
            f_prob_config.close();
            exit(ERR_CONFIG_SYNTAX);
        }
    }
    f_prob_config.close();
    if (problem_info.data_count == 0) {
        std::cerr << "Fatal : DataCount is 0. Maybe You Have Written an Empty Config" << config_line <<std::endl;
        exit(ERR_CONFIG_SYNTAX);
    }
}