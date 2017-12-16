#!/bin/bash 

make clean_img
make
./fs_create doge_fs 512 fs.img 5120

./fs_add txt_files1/raed.txt fs.img # 1 block
./fs_add txt_files1/hello.txt fs.img # 2 blocks
./fs_add txt_files1/s.txt fs.img # 3 blocks
./fs_add txt_files1/a.txt fs.img # 1 blocks

echo "-------------- del --------------"

./fs_del hello.txt fs.img # 2 blocks
./fs_del s.txt fs.img # 3 blocks
