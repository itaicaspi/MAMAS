#!/bin/bash

make

echo "Running test..."
for ((i=1;i<8;i++)); do
	./hw2 "trace$i.txt" > "trace$i.temp"
	result=`diff "out$i" "trace$i.temp"`
	if [ -n "$result" ]; then
		echo "Test $i: failed"
	else 
		echo "Test $i: passed"
	fi
done
rm -f *.temp
echo "end of test..."
