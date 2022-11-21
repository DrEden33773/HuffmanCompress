/**
 * @file DynamicBitset.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace Utility {

class DynamicBitset {
private:
    std::vector<size_t> Data;
    int                 NumOfBit       = 0;
    int                 NumOfContainer = 0;

public:
    std::fstream& operator<<(std::fstream& receiver) {
        return receiver;
    }
};

} // namespace Utility