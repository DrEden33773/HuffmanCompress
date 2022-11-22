/**
 * @file Compressor.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
        SourceFile => CompressedFile
        BitCodeMap => `BitCode.bin`
 * @version 0.1
 * @date 2022-11-13
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * @note
        1 byte = 8 bits
        sizeof(char) = 1 byte
        N states => UpperInt(N / 8) char
 *
 */

#include <stdint.h>

#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "../Utility/DynamicBitset.hpp"
#include "../Utility/FileManager.hpp"
#include "../Utility/HuffmanTree.hpp"

namespace Module {

class Compressor {
public:
    const std::filesystem::path& Compressed_path  = Utility::FileManager::Filename::Compressed;
    const std::filesystem::path& SourceFile_path  = Utility::FileManager::Filename::SourceFile;
    const std::filesystem::path& HuffmanCode_path = Utility::FileManager::Filename::HuffmanCode;

    using HuffmanTree               = Utility::HuffmanTree<char>;
    using CharStream_BitCodeType    = Utility::HuffmanTree<char>::CharStreamBitCodeType;
    using CharStream_BitCodeMapType = std::unordered_map<char, CharStream_BitCodeType>;
    using FreqMap                   = std::unordered_map<char, int>;

private:
    std::vector<std::pair<char, int>> CharFreqList;
    CharStream_BitCodeMapType         CharStream_BitCodeMap;

public:
    void get_CharFreqList(const FreqMap& input) {
        for (auto&& pair : input) {
            CharFreqList.push_back(std::make_pair(
                pair.first,
                pair.second
            ));
        }
    }
    void get_CharStream_BitCodeMap() {
        HuffmanTree the_tree(CharFreqList);
        CharStream_BitCodeMap = the_tree.get_bitcode_map();
    }
    void compress_to_CharStream() {
        std::fstream sourceFile;
        std::fstream compressed;

        sourceFile.open(SourceFile_path, std::fstream::in);
        if (!sourceFile.is_open()) {
            throw std::runtime_error("Cannot open `source.txt`!");
        }

        compressed.open(
            Compressed_path,
            std::fstream::binary | std::fstream::out | std::fstream::trunc
        );
        if (!compressed.is_open()) {
            throw std::runtime_error("Cannot open `code.dat`!");
        }

        char receiver;
        while (sourceFile.get(receiver)) {
            CharStream_BitCodeType BitCode = CharStream_BitCodeMap[receiver];
            for (char curr_bit : BitCode) {
                compressed.write(
                    reinterpret_cast<char*>(&curr_bit),
                    sizeof(curr_bit)
                );
            }
        }

        sourceFile.close();
        compressed.close();
    }
    void compress_to_BitStream() {
        std::fstream sourceFile;
        std::fstream compressed;
        std::string  CharStream;

        // 1. first get the CharStream
        sourceFile.open(SourceFile_path, std::fstream::in);
        if (!sourceFile.is_open()) {
            throw std::runtime_error("Cannot open `source.txt`!");
        }
        char receiver;
        while (sourceFile.get(receiver)) {
            CharStream += CharStream_BitCodeMap[receiver];
        }
        sourceFile.close();

        // 2. trans CharStream to BitStream
        Utility::DynamicBitset BitStream(CharStream);

        // 3. serialize BitStream
        compressed.open(
            Compressed_path,
            std::fstream::binary | std::fstream::out | std::fstream::trunc
        );
        if (!compressed.is_open()) {
            throw std::runtime_error("Cannot open `code.dat`!");
        }
        compressed << BitStream;
        compressed.close();
    }
    void serialize_CharStream_BitCodeMap() {
        std::fstream BitCodeMap_file;

        BitCodeMap_file.open(HuffmanCode_path, std::fstream::out);
        if (!BitCodeMap_file.is_open()) {
            throw std::runtime_error("Cannot open `Huffman.txt`!");
        }

        for (auto&& CharFreqPair : CharFreqList) {
            char curr_char = CharFreqPair.first;
            int  curr_freq = CharFreqPair.second;
            if (curr_char == '\t') {
                BitCodeMap_file << "__Tab"
                                << " ";
            } else if (curr_char == '\n') {
                BitCodeMap_file << "__NewLine"
                                << " ";
            } else if (curr_char == ' ') {
                BitCodeMap_file << "__Space"
                                << " ";
            } else if (curr_char == '\r') {
                BitCodeMap_file << "__WindowsNewLine"
                                << " ";
            } else {
                BitCodeMap_file << curr_char
                                << " ";
            }
            BitCodeMap_file << curr_freq << " ";
            CharStream_BitCodeType& curr_CharStream_BitCode = CharStream_BitCodeMap[curr_char];
            std::for_each(
                curr_CharStream_BitCode.begin(),
                curr_CharStream_BitCode.end(),
                [&BitCodeMap_file](const char& in) {
                    BitCodeMap_file.put(in);
                }
            );
            BitCodeMap_file << std::endl;
        }
    }

public:
    static auto Compress(const FreqMap& in) {
        Compressor Compressing_Process;

        Compressing_Process.get_CharFreqList(in);
        Compressing_Process.get_CharStream_BitCodeMap();

        // Compressing_Process.compress_to_CharStream();
        Compressing_Process.compress_to_BitStream();
        Compressing_Process.serialize_CharStream_BitCodeMap();

        std::cout << "Compressing process is over! " << std::endl;
        std::cout << std::endl;

        return Compressing_Process.CharStream_BitCodeMap;
    }
};

} // namespace Module