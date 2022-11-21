/**
 * @file main.cpp
 * @author Eden (edwardwang33773@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "TaskManager.hpp"

#include "Test/RunTest.hpp"

int main(int argc, char** argv) {
    // bool IfSuccess = Task::run_all_task();
    Test::RunTest();
    return 0;
}
