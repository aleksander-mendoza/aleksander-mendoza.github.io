#!/bin/bash
if [ ! -e bin ];then
  mkdir bin
  cd bin
  cmake ../src
  cd ..
fi 

cd bin
cmake --build .


