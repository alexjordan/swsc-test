#!/usr/bin/zsh

xmlstarlet sel -T -t -f -o ": " -v '//results/result/cycles' -n **/*.ait.xml
grep "d.splits" **/*.xml
grep "Loop.*\(ensure\|reserve\)" **/*.ait.txt
for i in **/*.gdl; do
	echo -n "$i (res): "
	grep "call _sc_reserve" $i | wc -l
	echo -n "$i (ens): "
	grep "call _sc_ensure" $i | wc -l
done
