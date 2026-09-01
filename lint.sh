#!/bin/sh
set -e

echo "Running clang-tidy..."
clang-tidy -p build src/*.c --checks="-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling"

echo "Running cppcheck..."
cppcheck --enable=warning,style,performance --inline-suppr -I include src/