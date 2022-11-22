/**
 * @file RunTest.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "DynBitFurtherTest.hpp"
#include "DynBitsetFileIOTest.hpp"
#include "DynamicBitsetTest.hpp"

#include <functional>
#include <vector>

namespace Test {

static const std::vector<std::function<void()>> tests {
    DynamicBitsetTest,
    DynBitFurtherTest,
    DynBitsetFileIOTest,
};

void RunTest() {
    for (auto&& func : tests) {
        func();
    }
}

} // namespace Test