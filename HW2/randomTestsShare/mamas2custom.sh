#!/bin/bash
make

echo "Running test..."
for ((i=0;i<200;i++)); do
	if [ -f test$i.in ]; then
		./hw2 "test$i.in" > "trace$i.temp"
		result=`diff "test$i.out" "trace$i.temp"`
		if [ -n "$result" ]; then
			echo "Test $i: failed"
		else 
			echo "Test $i: passed"
		fi
	fi
done
rm -f *.temp
echo "end of test..."