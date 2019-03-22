for i in {1..6}; do
	echo "$i"
	./run_fio_rw.sh $1_$i
done
