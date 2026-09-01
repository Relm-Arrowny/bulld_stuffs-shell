#!/bin/sh
set -e

echo "Running clang-tidy..."
clang-tidy src/*.c -p build --checks="-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling"

echo "Running cppcheck..."
cppcheck --enable=warning,style,performance --inline-suppr src/