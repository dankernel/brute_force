#!/bin/bash

WORK=1
find -size +40M -not -path "./" | grep -q "\." && WORK=0

if [ $WORK = 1 ]; then
  echo -e '\E[34m'"\033[1m[O  K] 40M\033[0m"

  if [ ! -d .git/ ]
  then
    git init
  fi

  git add .
  MSG=auto'-'`date +%Y%m%d'-'%H%M%S`
  git commit -m "$MSG"
  git push

  work="[O  K] ".$(pwd)
  echo -e '\E[34m'"\033[1m$work\033[0m"
else
  echo -e '\E[34m'"\033[1m[FAIL]40M\033[0m"
fi


