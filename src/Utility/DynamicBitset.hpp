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
    /**
     * @note
        @e relation_between @b back_bit_index @p and @b num_of_bit
            back_bit_index = num_of_bit % box_size - 1
     */
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

    /* Necessary Info during `Deserialization` */
    struct Info {
        u32 NumOfBit   = 0; // >=0
        u32 NumOfBox   = 1; // >=1
        u32 CurrBoxIdx = 0; // >=0
    };

private:
    std::vector<u32> Data       = { all_false };
    u32              NumOfBit   = 0; // >=0
    u32              NumOfBox   = 1; // >=1
    u32              CurrBoxIdx = 0; // >=0

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

public:
    template <typename functor>
    void for_each_bit(functor func) const {
        // 1. first deal with [0, NumOfBox - 2]
        for (u32 BoxIdx = 0; BoxIdx < NumOfBox - 1; ++BoxIdx) {
            u32 Box        = Data[BoxIdx];
            u32 LeftOptNum = box_size;
            while (LeftOptNum--) {
                u32 Bit = Box & 1;
                func(Bit);
                Box >>= 1;
            }
        }
        // 2. then deal with [NumOfBox - 1] (the last one)
        u32 LastBox    = Data[NumOfBox - 1];
        u32 LeftOptNum = NumOfBit % box_size;
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
        check_if_add_box(); // may add box
        /* ---*--- core ---*--- */
        u32& CurrBox       = Data[CurrBoxIdx];
        u32  NewBackBitIdx = NumOfBit % box_size;
        if (in) {
            u32 to_push = 1;
            for (u32 opt_time = 0; opt_time < NewBackBitIdx; ++opt_time) {
                to_push <<= 1;
            }
            // 00000000 1 00000000 like
            CurrBox |= to_push;
        } else {
            u32 to_push = 1;
            for (u32 opt_time = 0; opt_time < NewBackBitIdx; ++opt_time) {
                to_push <<= 1;
            }
            to_push = ~to_push; // 11111111 0 11111111 like
            CurrBox &= to_push;
        }
        /* ---*--- end of core ---*--- */
        update_NumOfBit(); // update NumOfBit
    }
    void pop_back() {
        /**
         * @brief This function could make sure that:
                Each unused bit will be `0`
                Each popped bit will be reset to `0` later
         */

        /* ---*--- core ---*--- */
        u32& CurrBox    = Data[CurrBoxIdx];
        u32  CurrBitIdx = NumOfBit % box_size - 1;
        u32  to_push    = 1;
        for (u32 opt_time = 0; opt_time < CurrBitIdx; ++opt_time) {
            to_push <<= 1;
        }
        to_push = ~to_push; // 011111111 like
        CurrBox &= to_push;
        /* ---*--- end of core ---*--- */

        update_NumOfBit_sub();
        check_if_sub_box();
    }
    [[nodiscard]] bool back() const {
        u32 CurrBox    = Data[CurrBoxIdx];
        u32 bit_mask   = 1;
        u32 CurrBitIdx = NumOfBit % box_size - 1;
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
        /* ---*--- core ---*--- */
        for_each_bit([&](const u32& Bit) {
            if (Bit) {
                ret.push_back('1');
            } else {
                ret.push_back('0');
            }
        });
        /* ---*--- end of core ---*--- */
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

    void sync_necessary_info(const Info& info) {
        NumOfBit   = info.NumOfBit;
        NumOfBox   = info.NumOfBox;
        CurrBoxIdx = info.CurrBoxIdx;
    }
    [[nodiscard]] Info get_necessary_info() const {
        return Info {
            .NumOfBit   = NumOfBit,
            .NumOfBox   = NumOfBox,
            .CurrBoxIdx = CurrBoxIdx,
        };
    }

    friend bool operator==(
        const DynamicBitset& lhs,
        const DynamicBitset& rhs
    ) {
        /**
         * @brief The discarded code is used when:
                DynamicBitset @b cannot_guarantee the @p protocol:
                    @b unused/popped_bit @e equals @b `0`
                But that will be less efficient
         *
         */
        // std::string lhs_serial = lhs.convert_to_CharStream();
        // std::string rhs_serial = rhs.convert_to_CharStream();
        // return lhs_serial == rhs_serial;

        /**
         * @brief The current code is used when:
                DynamicBitset @b can_guarantee the @p protocol:
                    @b unused/popped_bit @e equals @b `0`
                And it's the most efficient one
         *
         */
        if (&rhs == &lhs) {
            // self cmp
            return true;
        }
        bool Data     = lhs.Data == rhs.Data;
        bool NumOfBit = lhs.NumOfBit == rhs.NumOfBit;
        return Data && NumOfBit;
    }

    friend std::ostream& operator<<(std::ostream& os, const DynamicBitset& obj) {
        obj.for_each_bit([](const u32& bit) {
            std::cout << bit;
        });
        return os;
    }
    friend std::fstream& operator<<(std::fstream& os, const DynamicBitset& obj) {
        Info header_info = obj.get_necessary_info();
        os.write(reinterpret_cast<char*>(&header_info), sizeof(header_info));
        obj.for_each_box([&](u32 box) {
            os.write(reinterpret_cast<char*>(&box), sizeof(box));
        });
        return os;
    }
    friend void operator>>(std::fstream& is, DynamicBitset& obj) {
        // 1. reset all status
        obj.Data.clear();
        obj.NumOfBit   = 0;
        obj.NumOfBox   = 0;
        obj.CurrBoxIdx = 0;

        // 2. get header_info
        Info received_info;
        is.read(
            reinterpret_cast<char*>(&received_info),
            sizeof(received_info)
        );
        obj.sync_necessary_info(received_info);

        // 3. directly read
        u32 Box = 0;
        while (is.read(
            reinterpret_cast<char*>(&Box),
            sizeof(Box)
        )) {
            obj.Data.push_back(Box);
        }
    }
};

} // namespace Utility