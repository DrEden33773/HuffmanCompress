/**
 * @file Scanner.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "../Utility/FileManager.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Module {

class Scanner {
private:
    const std::filesystem::path& opt_file_path = Utility::FileManager::Filename::SourceFile;

    std::filesystem::path source_file_path;

    std::unordered_map<char, int> FreqMap;

    bool if_need_to_move_src = true;

public:
    void locate_source_file() {
        bool if_opt_source_exist = std::filesystem::exists(opt_file_path);
        if (if_opt_source_exist) {
            bool if_opt_source_empty = std::filesystem::is_empty(opt_file_path);
            if (!if_opt_source_empty) {
                // should not ReLocate the src
                if_need_to_move_src = false;
                return;
            }
        }
        while (true) {
            std::cout << "Please input the `Path` of source file " << std::endl;
            std::cout << R"( E.G. => C:/Path/Name.txt | C:\\Path\\Name.txt )" << std::endl;
            std::cout << ">>> ";

            std::cin >> source_file_path;

            if (!std::filesystem::exists(source_file_path)) {
                std::cout << "Input path DO NOT EXIST! Please try again... " << std::endl;
                std::cout << std::endl;
                source_file_path.clear();
            } else {
                std::cout << std::endl;
                break;
            }
        }
    }
    void copy_the_source_file() {
        if (!if_need_to_move_src) {
            return;
        }
        Utility::FileManager::copy_source_file_from(source_file_path);
        std::cout << "Successfully get source file. It had been moved to: " << std::endl;
        std::cout << "\t" << Utility::FileManager::Filename::SourceFile << std::endl;
        std::cout << std::endl;
    }
    void scan_opt_file() {
        std::fstream opt_file;

        opt_file.open(opt_file_path, std::fstream::in);
        if (!opt_file.is_open()) {
            throw std::runtime_error("Failed to open `source.txt`!");
        }
        char curr_char;
        while (opt_file.get(curr_char)) {
            ++FreqMap[curr_char];
        }
        opt_file.close();
    }

public:
    static auto Scan() {
        Scanner Scanning_Process;

        Scanning_Process.locate_source_file();
        Scanning_Process.copy_the_source_file();
        Scanning_Process.scan_opt_file();

        std::cout << "Scanning process is over! " << std::endl;
        std::cout << std::endl;

        return Scanning_Process.FreqMap;
    }
};

} // namespace Module