# minialign
Minimalistic aligner which uses Minimap for input mapping locations and Edlib (Myers' bit-vector impl.) for alignment.  

## Build  
```  
make modules && make tools & make  
```  

## Example  
```  
./wrap-minialign.sh test-data/lambda/NC_001416.fa test-data/lambda/reads.fastq out.sam  
```  

## Usage  
Generate a mapping PAF/MHAP file using Minimap, GraphMap or MHAP:  
```  
minimap test-data/lambda/NC_001416.fa test-data/lambda/reads.fastq > test-data/lambda/overlaps.paf  
```  

Align the mappings using Minialign:  
```  
bin/minialign test-data/lambda/NC_001416.fa test-data/lambda/reads.fastq test-data/lambda/overlaps.paf > test-data/lambda/alignments.sam  
```  
If your mappings are in the MHAP format, you can specify this using the ```--mhap``` option.  

To use hard-clipping for reporting alignments instead of soft:  
```  
bin/minialign --hard-clip test-data/lambda/NC_001416.fa test-data/lambda/reads.fastq test-data/lambda/overlaps.paf > test-data/lambda/alignments.sam  
```  
