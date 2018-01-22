#!/bin/bash
if [ -z "$BUILD_DATE_ID" ]; then
  export BUILD_DATE_ID=$(date)
fi

bin/macos/tundra2 macosx-clang-release
