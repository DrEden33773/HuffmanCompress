/**
 * @file Unwrapper.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
        `BitCode.bin` => BitCodeMap
        Use BitCodeMap to Unwrap the Compressed File
 * @version 0.1
 * @date 2022-11-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <utility>

#include "../Utility/FileManager.hpp"
#include "../Utility/HuffmanTree.hpp"

namespace Module {

class Unwrapper {
public:
    const std::filesystem::path& Compressed_path = Utility::FileManager::Filename::Compressed;
    const std::filesystem::path& Unwrapped_path  = Utility::FileManager::Filename::Unwrapped;
    const std::filesystem::path& SrcFile_path    = Utility::FileManager::Filename::SourceFile;

    using CharStream_Type               = Utility::HuffmanTree<char>::CharStreamBitCodeType;
    using CharStream_BitCodeMap_Type    = std::unordered_map<char, CharStream_Type>;
    using CharStream_RevBitCodeMap_Type = std::unordered_map<CharStream_Type, char>;

    std::fstream Unwrapped_file;
    std::fstream Compressed_file;
    std::fstream SrcFile;

private:
    // { char : CharStream_BitCode }
    CharStream_BitCodeMap_Type CharStream_BitCodeMap;
    // { CharStream_BitCode : char }
    CharStream_RevBitCodeMap_Type CharStream_RevBitCodeMap;
    // vector<char>
    CharStream_Type CharStream;

    void CharStreamParser() {
        if (CharStream_RevBitCodeMap.contains(CharStream)) {
            char curr = CharStream_RevBitCodeMap[CharStream];
            CharStream.clear();
            Unwrapped_file.put(curr);
        }
    }

public:
    void get_CharStreamBitCodeMap(const CharStream_BitCodeMap_Type& in) {
        CharStream_BitCodeMap = in;
        for (auto&& pair : CharStream_BitCodeMap) {
            CharStream_RevBitCodeMap.insert(
                std::make_pair(pair.second, pair.first)
            );
        }
    }
    void unwrap_from_char() {

        Compressed_file.open(Compressed_path, std::fstream::binary | std::fstream::in);
        if (!Compressed_file.is_open()) {
            throw std::runtime_error("Cannot open `code.dat`");
        }

        Unwrapped_file.open(Unwrapped_path, std::fstream::out | std::fstream::trunc);
        if (!Unwrapped_file.is_open()) {
            throw std::runtime_error("Cannot open `recode.txt`");
        }

        char temp;
        while (Compressed_file.read(
            reinterpret_cast<char*>(&temp),
            sizeof(temp)
        )) {
            CharStream.push_back(temp);
            CharStreamParser();
        }

        Compressed_file.close();
        Unwrapped_file.close();
    }
    void unwrap_from_bitstream() {
        // TODO(eden): SO F**KING HARD!
    }
    bool if_result_is_right() {
        bool res = true;
        Unwrapped_file.open(Unwrapped_path, std::fstream::in);
        if (!Unwrapped_file.is_open()) {
            throw std::runtime_error("Cannot open `recode.txt`");
        }
        SrcFile.open(Unwrapped_path, std::fstream::in);
        if (!SrcFile.is_open()) {
            throw std::runtime_error("Cannot open `recode.txt`");
        }
        char unwrapped;
        char src;
        int  unwrapped_char_num = 0;
        int  src_char_num       = 0;
        while (SrcFile.get(src)) {
            Unwrapped_file.get(unwrapped);
            ++src_char_num;
            ++unwrapped_char_num;
            if (unwrapped != src) {
                Unwrapped_file.close();
                SrcFile.close();
                return false;
            }
        }
        while (SrcFile.get(src)) {
            ++src_char_num;
        }
        while (Unwrapped_file.get(unwrapped)) {
            ++unwrapped_char_num;
        }
        if (src_char_num != unwrapped_char_num) {
            Unwrapped_file.close();
            SrcFile.close();
            return false;
        }
        Unwrapped_file.close();
        SrcFile.close();
        return true;
    }

public:
    static bool Unwrap(const CharStream_BitCodeMap_Type& in) {
        Unwrapper Unwrapping_Process;

        Unwrapping_Process.get_CharStreamBitCodeMap(in);
        Unwrapping_Process.unwrap_from_char();
        assert(Unwrapping_Process.if_result_is_right());

        std::cout << "UnwrappedFile == SrcFile, Success!" << std::endl;
        std::cout << std::endl;

        return true;
    }
};

} // namespace Module