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
    static constexpr u32 all_false = 0;

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
    void check_if_sub_box() {
        bool if_add_box = NumOfBit % box_size == 0 && NumOfBit;
        if (!if_add_box) {
            return;
        }
        Data.pop_back();
        --NumOfBox;
        --CurrBoxIdx;
    }
    void update_NumOfBit() {
        ++NumOfBit;
    }
    void update_NumOfBit_sub() {
        --NumOfBit;
    }
    void update_CurrBitIdx() {
        u32 RawIdx = (NumOfBit - 1);
        CurrBitIdx = RawIdx % box_size;
    }

public:
    template <typename functor>
    void for_each_bit(functor func) const {
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
    void for_each_box(functor func) const {
        for (u32 BoxIdx = 0; BoxIdx < NumOfBox; ++BoxIdx) {
            u32 Box = Data[BoxIdx];
            func(Box);
        }
    }

    void push_back(const bool& in) {
        check_if_add_box();  // may add box
        update_NumOfBit();   // add bit, then update NumOfBit first (must be done first)
        update_CurrBitIdx(); // use updated NumOfBit to get CurrBitIdx
        // ---*--- core ---*---
        u32& CurrBox = Data[CurrBoxIdx];
        if (in) {
            u32 to_push = 1;
            for (u32 opt_time = 0; opt_time < CurrBitIdx; ++opt_time) {
                to_push <<= 1;
            }
            // 100000000 like
            CurrBox |= to_push;
        } else {
            u32 to_push = 1;
            for (u32 opt_time = 0; opt_time < CurrBitIdx; ++opt_time) {
                to_push <<= 1;
            }
            to_push = ~to_push; // 01111111 like
            CurrBox &= to_push;
        }
        // ---*--- end of core ---*---
    }
    void pop_back() {
        // you have to set highest bit to `0`

        update_NumOfBit_sub();
        update_CurrBitIdx();
        check_if_sub_box(); // may sub box
    }
    [[nodiscard]] bool back() const {
        u32 CurrBox  = Data[CurrBoxIdx];
        u32 bit_mask = 1;
        for (u32 opt_time = 0; opt_time < CurrBitIdx; ++opt_time) {
            bit_mask <<= 1;
        }
        u32 received = bit_mask & CurrBox;
        for (u32 opt_time = 0; opt_time < CurrBitIdx; ++opt_time) {
            received >>= 1;
        }
        u32 res = received;
        if (res) {
            return true;
        }
        return false;
    }
    [[nodiscard]] std::string convert_to_CharStream() const {
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
    DynamicBitset(const std::initializer_list<u32>& init) {
        for (const u32& bit : init) {
            if (bit) {
                push_back(true);
            } else {
                push_back(false);
            }
        }
    }
    DynamicBitset(const std::initializer_list<bool>& init) {
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

    friend std::ostream& operator<<(std::ostream& os, const DynamicBitset& obj) {
        obj.for_each_bit([](const u32& bit) {
            std::cout << bit;
        });
        return os;
    }
    friend std::fstream& operator<<(std::fstream& os, const DynamicBitset& obj) {
        obj.for_each_box([&](const u32& box) {
            os.write((char*)(&box), sizeof(box));
        });
        return os;
    }

    [[nodiscard]] u32 get_NumOfBit() const {
        return NumOfBit;
    }
    [[nodiscard]] u32 get_NumOfBox() const {
        return NumOfBox;
    }
    [[nodiscard]] u32 get_CurrBoxIdx() const {
        return CurrBoxIdx;
    }
    [[nodiscard]] u32 get_CurrBitIdx() const {
        return CurrBitIdx;
    }
    void set_NumOfBit(const u32& new_v) {
        NumOfBit = new_v;
    }
    void set_NumOfBox(const u32& new_v) {
        NumOfBox = new_v;
    }
    void set_CurrBoxIdx(const u32& new_v) {
        CurrBoxIdx = new_v;
    }
    void set_CurrBitIdx(const u32& new_v) {
        CurrBitIdx = new_v;
    }
};

} // namespace Utility