#!/bin/bash
if [ ! -e bin ];then
  mkdir bin
  cd bin
  emcmake cmake ../src
  cd ..
fi 

cd bin
emmake make


