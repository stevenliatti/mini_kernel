#!/bin/bash

make fs_add

for i in txt_files/* ; do
	./fs_add "$i" fs.img
done