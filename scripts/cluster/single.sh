#!/bin/bash

instances=INSTANCES
filename=FILENAME
extension="${filename##*.}"
filename="${filename%.*}"

TIMEOUT_SOLVER=TIME_L
TIMEOUT_SOLVER_PL=$TIMEOUT_SOLVER
TIMEOUT_VERIPB=$(echo 10*$TIMEOUT_SOLVER / 1 | bc)

MEMOUT_SOLVER=MEM_L
MEMOUT_SOLVER_PL=$MEMOUT_SOLVER
MEMOUT_VERIPB=$(echo 10*$MEMOUT_SOLVER / 1 | bc)

res_runtime_without_prooflogging="NA"
res_mem_without_prooflogging="NA"
res_runtime_with_prooflogging="NA"
res_mem_with_prooflogging="NA"
res_runtime_verification="NA"
res_mem_verification="NA"
res_verification_succeeded="NA"
res_time_genCardinals_without_PL="NA"
res_time_genCardinals="NA"
res_time_genCardinalDefinitions="NA"
res_proofsize="NA"


## VANILLA

# run
./runlim -r $TIMEOUT_SOLVER -s $MEMOUT_SOLVER -o $TMPDIR/${filename}.txt ./qmaxsat -log_duration_totalizer=$TMPDIR/${filename}_totalizer.txt $instances/${filename}.${extension}
cat $TMPDIR/${filename}_totalizer.txt

# extract time
res_runtime_without_prooflogging=$(cat $TMPDIR/${filename}.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

# extract space
res_mem_without_prooflogging=$(cat $TMPDIR/${filename}.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

#extract time for genCardinals
res_time_genCardinals_without_PL=$(cat $TMPDIR/${filename}_totalizer.txt | grep 'genCardinals:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

# status
status=$(cat $TMPDIR/${filename}.txt | grep 'status:' | awk '{print $3}');

echo "$filename without prooflogging:"
echo "runtime: $res_runtime_without_prooflogging "
echo "mem: $res_mem_without_prooflogging"
echo "runtime genCardinals: $res_time_genCardinals_without_PL"

if [[ "$status" == "ok" ]]
then

    ## PROOFLOGGED
    
    # run
    ./runlim -r $TIMEOUT_SOLVER_PL -s $MEMOUT_SOLVER_PL -o $TMPDIR/${filename}.txt ./qmaxsat_prooflogging -log_duration_totalizer=$TMPDIR/${filename}_totalizer.txt -proof-file=$VSC_SCRATCH/${filename}_proof.pbp $instances/${filename}.${extension} 

    # extract time
    res_runtime_with_prooflogging=$(cat $TMPDIR/${filename}.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

    # extract space
    res_mem_with_prooflogging=$(cat $TMPDIR/${filename}.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

    #extract time for genCardinals
    res_time_genCardinals=$(cat $TMPDIR/${filename}_totalizer.txt | grep 'genCardinals:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');
   	
    if [[ "$res_time_genCardinals" == "" ]]
    then
    	res_time_genCardinals="NA"
    fi

    #extract time for genCardinalDefinitions
    res_time_genCardinalDefinitions=$(cat $TMPDIR/${filename}_totalizer.txt | grep 'genCardinalDefinitions:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');
    	
    if [[ "$res_time_genCardinalDefinitions" == "" ]]
    then
        res_time_genCardinalDefinitions="NA"
    fi
	    	
    # extract proof size
    res_proofsize=$(stat --printf="%s" $VSC_SCRATCH/${filename}_proof.pbp)
    
    if [[ "$res_proofsize" == "" ]]
    then
        res_proofsize="NA"
    fi

    # status
    status=$(cat $TMPDIR/${filename}.txt | grep 'status:' | awk '{print $3}');

    echo "$filename with prooflogging:"
    echo "runtime: $res_runtime_with_prooflogging "
    echo "mem: $res_mem_with_prooflogging"
    echo "duration genCardinalDefinitions: $res_time_genCardinalDefinitions"
    echo "duration genCardinals: $res_time_genCardinals"
    echo "proofsize: $res_proofsize"
	
    if [[ "$status" == "ok" ]]
    then	

        ## VERIFICATION

        # run
        ./runlim -r $TIMEOUT_VERIPB -s $MEMOUT_VERIPB -o $TMPDIR/${filename}.txt ./veripb --wcnf $instances/${filename}.${extension} $VSC_SCRATCH/${filename}_proof.pbp > $TMPDIR/${filename}_verification.txt

        # extract time
        res_runtime_verification=$(cat $TMPDIR/${filename}.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

        # extract space
        res_mem_verification=$(cat $TMPDIR/${filename}.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

  	    echo "$filename verification:"
	    echo "runtime: $res_runtime_verification "
    	echo "mem: $res_mem_verification"

    	if grep -q "succeeded" $TMPDIR/${filename}_verification.txt; then
    		res_verification_succeeded=1
    	else
    		res_verification_succeeded=0	
    	fi
    fi
fi

rm $VSC_SCRATCH/${filename}_proof.pbp
echo "$filename, $res_runtime_without_prooflogging, $res_time_genCardinals_without_PL, $res_mem_without_prooflogging, $res_runtime_with_prooflogging, $res_proofsize, $res_time_genCardinals, $res_time_genCardinalDefinitions, $res_mem_with_prooflogging, $res_runtime_verification, $res_mem_verification, $res_verification_succeeded" >> ./results/"$filename"_result.csv