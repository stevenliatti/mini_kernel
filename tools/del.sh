#!/bin/bash 

make fs_create
./fs_create doge_fs 512 fs.img 5120
make fs_add
./fs_add txt_files1/raed.txt fs.img 
./fs_add txt_files1/hello.txt fs.img
./fs_add txt_files1/s.txt fs.img 
./fs_add txt_files1/a.txt fs.img 
make fs_del
./fs_del hello.txt fs.img
./fs_del s.txt fs.img
