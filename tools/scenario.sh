#!/bin/bash

for i in txt_files1/* ; do
	./fs_add "$i" fs.img
done

./fs_del hello.txt fs.img
# ./fs_del AARON.txt fs.img