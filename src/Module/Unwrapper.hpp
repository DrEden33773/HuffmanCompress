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
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "../Utility/DynamicBitset.hpp"
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
    void unwrap_from_CharStream() {
        Compressed_file.open(Compressed_path, std::fstream::binary | std::fstream::in);
        if (!Compressed_file.is_open()) {
            throw std::runtime_error("Cannot open `code.dat`");
        }

        Unwrapped_file.open(Unwrapped_path, std::fstream::out | std::fstream::trunc);
        if (!Unwrapped_file.is_open()) {
            throw std::runtime_error("Cannot open `recode.txt`");
        }

        char temp = 0;
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
        // 1. receive the whole BitStream
        Compressed_file.open(Compressed_path, std::fstream::binary | std::fstream::in);
        if (!Compressed_file.is_open()) {
            throw std::runtime_error("Cannot open `code.dat`");
        }
        Utility::DynamicBitset BitStream_Receiver;
        Compressed_file >> BitStream_Receiver;
        Compressed_file.close();

        // 2. get the whole CharStream
        std::string CharStream_Receiver = BitStream_Receiver.convert_to_CharStream();

        // 3. parse the whole CharStream
        Unwrapped_file.open(Unwrapped_path, std::fstream::out | std::fstream::trunc);
        if (!Unwrapped_file.is_open()) {
            throw std::runtime_error("Cannot open `recode.txt`");
        }
        for (const char& curr : CharStream_Receiver) {
            CharStream.push_back(curr);
            CharStreamParser();
        }
        Unwrapped_file.close();
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
        char unwrapped          = 0;
        char src                = 0;
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

    void show_src_size() {
        std::cout << "Source File's size(byte) => ";
        std::cout << std::filesystem::file_size(SrcFile_path);
        std::cout << std::endl;
        std::cout << std::endl;
    }
    void show_expected_compressed_size() {
        size_t expected_byte_size = 0;
        size_t expected_bits      = 0;

        std::fstream BitMapFile;
        BitMapFile.open(
            Utility::FileManager::Filename::HuffmanCode,
            std::fstream::in
        );
        if (!BitMapFile.is_open()) {
            throw std::runtime_error("Cannot open SrcFile!");
        }

        std::string tag;
        size_t      freq = 0;
        std::string code;
        while (BitMapFile >> tag >> freq >> code) {
            size_t size_of_code = code.size();
            expected_bits += freq * size_of_code;
        }

        BitMapFile.close();

        expected_byte_size = (expected_bits / 8);

        std::cout << "Expected Compressed File's size(byte) => ";
        std::cout << expected_byte_size;
        std::cout << std::endl;
        std::cout << std::endl;
    }
    void show_compressed_size() {
        std::cout << "Compressed File's size(byte) => ";
        std::cout << std::filesystem::file_size(Compressed_path);
        std::cout << std::endl;
        std::cout << std::endl;
    }

public:
    static bool Unwrap(const CharStream_BitCodeMap_Type& in) {
        Unwrapper Unwrapping_Process;

        Unwrapping_Process.get_CharStreamBitCodeMap(in);
        // Unwrapping_Process.unwrap_from_CharStream();
        Unwrapping_Process.unwrap_from_bitstream();

        assert(Unwrapping_Process.if_result_is_right());

        std::cout << "UnwrappedFile == SrcFile, Success!" << std::endl;
        std::cout << std::endl;

        std::cout << "Here's the path of `Unwrapped`" << std::endl;
        std::cout << ">>> " << std::filesystem::absolute(Unwrapping_Process.Unwrapped_path);
        std::cout << std::endl;
        std::cout << std::endl;

        Unwrapping_Process.show_src_size();
        Unwrapping_Process.show_expected_compressed_size();
        Unwrapping_Process.show_compressed_size();

        return true;
    }
};

} // namespace Module