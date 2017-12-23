#!/bin/bash

for i in txt_files/* ; do
	./fs_add "$i" fs.img
done
