@echo off

set PATH=C:\Program Files\Git\bin;C:\Program Files (x86)\Git\bin;Cache\cmake\bin;%PATH%

cmake -DO=Build.cmake;Source/Engine/Build.cmake -DOWNLOAD_DIR=Cache -P Source/Engine/CMake/SelfUpdate/SelfUpdate.cmake
cmake -P Build.cmake
