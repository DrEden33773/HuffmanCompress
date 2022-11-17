/**
 * @file TaskManager.hpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "Module/Compressor.hpp"
#include "Module/Scanner.hpp"
#include "Module/Unwrapper.hpp"
#include "Utility/FileManager.hpp"

#include <functional>

namespace Task {

template <class Arg, class F>
auto operator|(Arg&& arg, F&& f) -> decltype(f(std::forward<Arg>(arg))) {
    return f(std::forward<Arg>(arg));
}

auto run_all_task() {
    Utility::FileManager::path_init();
    auto FreqMap    = Module::Scanner::Scan();
    auto BitCodeMap = Module::Compressor::Compress(FreqMap);
    auto IfSuccess  = Module::Unwrapper::Unwrap(BitCodeMap);
    return IfSuccess;
}

} // namespace Task