#!/bin/bash

./fs_add txt_files2/hello.txt fs.img

./fs_add txt_files2/a.txt fs.img

./fs_del hello.txt fs.img

./fs_add txt_files2/r.txt fs.img

./fs_del a.txt fs.img

./fs_add txt_files2/s.txt fs.img

./fs_add txt_files2/a.txt fs.img

./fs_add txt_files2/b.txt fs.img

./fs_del a.txt fs.img
