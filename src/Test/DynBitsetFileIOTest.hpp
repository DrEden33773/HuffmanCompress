/**
 * @file DynBitsetFileIOTest.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "../Utility/DynamicBitset.hpp"
#include "../Utility/FileManager.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace Test {

void DynBitsetFileIOTest() {
    Utility::FileManager::path_init();

    Utility::DynamicBitset source { "1011101" };
    Utility::DynamicBitset receiver;

    std::fstream file;

    file.open(
        Utility::FileManager::Filename::TestDynBitset,
        std::fstream::binary | std::fstream::out
    );
    if (!file.is_open()) {
        throw std::runtime_error("cannot open the file");
    }
    file << source;
    file.close();

    file.open(
        Utility::FileManager::Filename::TestDynBitset,
        std::fstream::binary | std::fstream::in
    );
    if (!file.is_open()) {
        throw std::runtime_error("cannot open the file");
    }
    file >> receiver;
    file.close();

    assert(source == receiver);

    std::cout << "source == receiver, assert succeed!" << std::endl;
    std::cout << std::endl;
}

} // namespace Test