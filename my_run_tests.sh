#!/bin/bash

WORKSPACE=$(git rev-parse --show-toplevel)
cd $WORKSPACE/build

cd utils/utils/test
# ./utils_moduletests --help
./utils_moduletests --gtest_filter=*Semaphore_test*
