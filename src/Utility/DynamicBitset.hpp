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
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace Utility {

class DynamicBitset {
public:
    /* Type Aliases */
    using i16 = short;
    using u16 = unsigned short;
    using i32 = int;
    using u32 = unsigned int;
    using i64 = long long;
    using u64 = unsigned long long;

    /* Bit Masks */
    static constexpr u32 all_true  = 0xffffffff;
    static constexpr u32 all_false = 0x0000000;
    static constexpr u32 low_true  = 0x0000001;
    static constexpr u32 low_false = 0x0000001;

    /* Conversion Info */
    static constexpr u32 u32_bytes = sizeof(u32);
    static constexpr u32 u32_bits  = u32_bytes * 8;
    static constexpr u32 box_size  = u32_bits;

private:
    std::vector<u32> Data       = { all_false };
    u32              NumOfBit   = 0; // >=0
    u32              NumOfBox   = 1; // >=1
    u32              CurrBoxIdx = 0; // >=0
    u32              CurrBitIdx = 0; // [0, box_size) => NumOfBit % box_size

private:
    void check_if_add_box() {
        bool if_add_box = NumOfBit % box_size == 0 && NumOfBit;
        if (!if_add_box) {
            return;
        }
        Data.push_back(all_false);
        ++NumOfBox;
        ++CurrBoxIdx;
    }
    void update_NumOfBit() {
        ++NumOfBit;
    }
    void update_CurrBitIdx() {
        CurrBitIdx = NumOfBit % box_size - 1;
    }

public:
    template <typename functor>
    void for_each_bit(functor func) {
        // 1. first deal with [0, NumOfBox - 2]
        for (u32 BoxIdx = 0; BoxIdx < NumOfBox - 1; ++BoxIdx) {
            u32 Box = Data[BoxIdx];
            while (Box) {
                u32 Bit = Box & 1;
                func(Bit);
                Box >>= 1;
            }
        }
        // 2. then deal with [NumOfBox - 1] (the last one)
        u32 LastBox    = Data[NumOfBox - 1];
        u32 LeftOptNum = CurrBitIdx + 1;
        while (LeftOptNum--) {
            u32 Bit = LastBox & 1;
            func(Bit);
            LastBox >>= 1;
        }
    }
    template <typename functor>
    void for_each_box(functor func) {
        for (u32 BoxIdx = 0; BoxIdx < NumOfBox; ++BoxIdx) {
            u32 Box = Data[BoxIdx];
            func(Box);
        }
    }

    void push_back(const bool& in) {
        check_if_add_box(); // may add box
        update_NumOfBit();
        update_CurrBitIdx();
        // ---*--- core ---*---
        u32& CurrBox = Data[CurrBoxIdx];
        if (in) {
            u32 to_add = low_true;
            for (u32 opt_time = 0; opt_time < CurrBitIdx; ++opt_time) {
                to_add <<= 1;
            }
            CurrBox += to_add;
        }
        // ---*--- end of core ---*---
    }
    std::string convert_to_CharStream() {
        std::string ret;
        // ---*--- core ---*---
        for_each_bit([&](const u32& Bit) {
            if (Bit) {
                ret.push_back('1');
            } else {
                ret.push_back('0');
            }
        });
        // ---*--- end of core ---*---
        return ret;
    }
    void convert_from_CharStream(const std::string& CharStream) {
        for (const char& curr : CharStream) {
            if (curr == '0') {
                push_back(false);
            } else {
                push_back(true);
            }
        }
    }

public:
    DynamicBitset() = default;
    explicit DynamicBitset(const std::string& CharStream) {
        convert_from_CharStream(CharStream);
    }
    DynamicBitset(const std::initializer_list<u32> init) {
        for (const u32& bit : init) {
            if (bit) {
                push_back(true);
            } else {
                push_back(false);
            }
        }
    }
    DynamicBitset(const std::initializer_list<bool> init) {
        for (const bool& bit : init) {
            push_back(bit);
        }
    }

    friend bool operator==(
        const DynamicBitset& lhs,
        const DynamicBitset& rhs
    ) {
        bool if_Data     = lhs.Data == rhs.Data;
        bool if_NumOfBit = lhs.NumOfBit == rhs.NumOfBit;
        bool if_NumOfBox = lhs.NumOfBox == rhs.NumOfBox;
        return if_Data && if_NumOfBit && if_NumOfBox;
    }
};

} // namespace Utility