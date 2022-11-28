/**
 * @file DynamicBitsetTest.hpp
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
#include <cassert>
#include <iostream>
#include <string>

namespace Test {

void DynamicBitsetTest() {
    std::string src = "01011010110";

    Utility::DynamicBitset src_dyn_bitset(src);

    std::cout << "original_bitset => " << src_dyn_bitset << std::endl; // 01011010110
    std::cout << std::endl;

    std::string received = src_dyn_bitset.convert_to_CharStream(); // "01011010110"
    std::cout << "original_string => " << src << std::endl;
    std::cout << std::endl;
    std::cout << "received_string => " << received << std::endl;
    std::cout << std::endl;

    Utility::DynamicBitset check_helper(received); // 01011010110
    assert(check_helper == src_dyn_bitset);        // success

    std::cout << "new_bitset == original_bitset, assert succeed!" << std::endl;
    std::cout << std::endl;
}

} // namespace Test