/**
 * @file FileManager.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief To manage all filenames and paths
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Utility::FileManager {

namespace fs = std::filesystem;

namespace Path {
    /* root */
    static fs::path Root = "Files";

} // namespace Path

namespace Filename {
    /* Source.txt */
    static fs::path SourceFile = Path::Root / "source.txt";
    /* Huffman.txt */
    static fs::path HuffmanCode = Path::Root / "Huffman.txt";
    /* code.dat */
    static fs::path Compressed = Path::Root / "code.dat";
    /* recode.txt */
    static fs::path Unwrapped = Path::Root / "recode.txt";

} // namespace Filename

/* all_path in a vec */
static std::vector<fs::path> all_path {
    Path::Root,
};

static void build_all_path() {
    std::for_each(
        all_path.begin(),
        all_path.end(),
        [](const fs::path& curr) {
            if (!fs::exists(curr)) {
                try {
                    fs::create_directories(curr);
                } catch (fs::filesystem_error& e) {
                    std::cout << "Could not create directory, because: " << e.what() << std::endl;
                }
            }
        }
    );
}
static void check_all_path_existence() {
    std::for_each(
        all_path.begin(),
        all_path.end(),
        [](const fs::path& curr) {
            if (!fs::exists(curr)) {
                throw std::runtime_error("A FilePath is NOT exist!");
            }
        }
    );
}
static void path_init() {
    build_all_path();
    check_all_path_existence();
    std::cout << "Path_Init Succeeded!" << std::endl;
    std::cout << std::endl;
}
static void copy_source_file_from(const fs::path& from) {
    try {
        fs::copy_file(from, Filename::SourceFile);
    } catch (fs::filesystem_error& e) {
        std::cout << "Could not copy, because: " << e.what() << std::endl;
    }
}

} // namespace Utility::FileManager
