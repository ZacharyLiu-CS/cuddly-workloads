//
// logging.h
//
// Created by Zacharyliu-CS on 07/12/2023.
// Copyright (c) 2022 zhenliu <liuzhenm@mail.ustc.edu.cn>.
//

#pragma once
#include <gflags/gflags.h>
#include <gflags/gflags_declare.h>
#include <iostream>

DECLARE_bool(debug);

#define LOG(...)                                                               \
  do {                                                                         \
    if (FLAGS_debug)                                                           \
      LogWrapper(__FILE__, "line ", __LINE__, " : ", __VA_ARGS__);             \
  } while (0)

inline void LogWrapper() { std::cerr << std::endl; }

template <typename First, typename... Rest>
inline void LogWrapper(First &&first, Rest &&...rest) {
  std::cerr << std::forward<First>(first) << " ";
  LogWrapper(std::forward<Rest>(rest)...);
}