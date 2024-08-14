#!/bin/bash

# 找到并删除所有名为 'build' 的文件夹，同时打印路径
find . -type d -name "build" -print -exec rm -rf {} +

# 找到并删除所有名为 '.vscode' 的文件夹，同时打印路径
find . -type d -name ".vscode" -print -exec rm -rf {} +

echo "删除完成。"
