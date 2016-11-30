#! /bin/sh

if [ "$#" -ne 3 ]; then
    echo "Wrong number of arguments."
    echo ""
    echo "Usage:"
    echo "    ./wrap-minialign.sh <ref.fa> <reads.fasta/fastq> <out.sam>"
    exit
fi

ref=$1
reads=$2
out=$3

cmd="tools/minimap/minimap $ref $reads"
echo $cmd
$cmd > ${out}.paf

echo ""

cmd="bin/minialign $ref $reads ${out}.paf"
echo $cmd
$cmd > ${out}
