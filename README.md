# minialign
Minimalistic aligner which uses Minimap for input mapping locations and Edlib (Myers' bit-vector impl.) for alignment.  
  
## Build  
```  
make modules  
make  
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
