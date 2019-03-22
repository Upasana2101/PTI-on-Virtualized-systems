for i in 512K 1M 10M 100M 
do
	fio fio-seq-RW.fio --size=$i --output=$1_$i
done

