#!/bin/bash

for i in txt_files/* ; do
	./fs_add "$i" fs.img
done

./fs_del AARON.txt fs.img
# ./fs_del s.txt fs.img