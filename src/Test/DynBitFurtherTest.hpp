/**
 * @file DynBitFurtherTest.hpp
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

void DynBitFurtherTest() {
    Utility::DynamicBitset A;
    Utility::DynamicBitset B;

    // build A and B
    {
        for (int i = 0; i < 18; ++i) {
            A.push_back(true);
        }
        for (int i = 0; i < 18; ++i) {
            A.push_back(false);
        }
        for (int i = 0; i < 4; ++i) {
            A.pop_back();
        }
        for (int i = 0; i < 4; ++i) {
            A.push_back(true);
        }

        for (int i = 0; i < 18; ++i) {
            B.push_back(true);
        }
        for (int i = 0; i < 14; ++i) {
            B.push_back(false);
        }
        for (int i = 0; i < 4; ++i) {
            B.push_back(true);
        }
        std::cout << "A => " << A << std::endl;
        std::cout << std::endl;
        std::cout << "B => " << B << std::endl;
        std::cout << std::endl;
        std::cout << "A.back => " << A.back() << std::endl;
        std::cout << std::endl;
        std::cout << "B.back => " << B.back() << std::endl;
        std::cout << std::endl;

        assert(A == B);

        std::cout << "A == B, assert succeed!" << std::endl;
        std::cout << std::endl;
    }

    // specifically test `Utility::DynamicBitset::pop_back()`
    {
        // pop_back() costs too much time!

        // for (int i = 0; i < 7; ++i) {
        //     A.pop_back();
        //     B.pop_back();
        // }
        // std::cout << "A and B execute `pop_back()` 7 times" << std::endl;
        // std::cout << std::endl;

        // std::cout << "A => " << A << std::endl;
        // std::cout << std::endl;
        // std::cout << "B => " << B << std::endl;
        // std::cout << std::endl;
        // std::cout << "A.back => " << A.back() << std::endl;
        // std::cout << std::endl;
        // std::cout << "B.back => " << B.back() << std::endl;
        // std::cout << std::endl;

        // assert(A == B);

        // std::cout << "A == B, assert succeed!" << std::endl;
        // std::cout << std::endl;
    }
}

} // namespace Test