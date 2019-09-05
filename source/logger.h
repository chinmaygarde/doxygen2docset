// This source file is part of doxygen2docset.
// Licensed under the MIT License. See LICENSE.md file for details.

#pragma once

#include <iostream>

#include "macros.h"

namespace d2d {

template <class Type>
class AutoLogger {
 public:
  AutoLogger(Type& logger) : logger_(logger) {}

  ~AutoLogger() {
    logger_ << std::endl;
    logger_.flush();
  }

  template <class T>
  AutoLogger& operator<<(const T& object) {
    logger_ << object;
    return *this;
  }

 private:
  Type& logger_;

  D2D_DISALLOW_COPY_AND_ASSIGN(AutoLogger);
};

using AutoOStreamLogger = AutoLogger<std::ostream>;

#define D2D_ERROR ::d2d::AutoOStreamLogger(std::cerr)
#define D2D_LOG ::d2d::AutoOStreamLogger(std::cout)

}  // namespace d2d
