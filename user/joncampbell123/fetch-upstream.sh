#!/bin/bash
git remote add upstream https://github.com/open-watcom/open-watcom-v2.git || git remote set-url upstream https://github.com/open-watcom/open-watcom-v2.git || exit 1
git fetch --all || exit 1

