#!/bin/bash
make
echo "Running deterministic tests"
for ((i=1;i<14;i++)); do
	if [ -f ${i}def.txt ]; then
		./CacheSim "${i}def.txt" "${i}input.txt" "${i}temp.temp"
		result=`diff "${i}temp.temp" "${i}expectedoutput.txt"`
		if [ -n "$result" ]; then
			echo -e "Test $i: \e[31mfailed\e[0m"
			echo -n -e "\e[31mTest $i description: \e[0m"
			cat "${i}description.txt"
		else
			echo -e "Test $i: \e[1;32mpassed\e[0m"
		fi
	fi
done
rm *.temp
echo "end of deterministic tests"

echo "Running random tests, let's get frenzy! :)"
for ((i=0;i<200;i++)); do
	if [ -f def${i}.in ]; then
		./CacheSim "def$i.in" "input$i.in" "temp$i.temp"
		result=`diff "temp$i.temp" "out$i.out"`
		if [ -n "$result" ]; then
			echo -e "Test $i: \e[31mfailed\e[0m"
			cat "def$i.in"
		else 
			echo -e "Test $i: \e[1;32mpassed\e[0m"
		fi
	fi
done
#rm *.temp
echo "end of random tests :("
