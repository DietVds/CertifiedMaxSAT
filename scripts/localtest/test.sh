#!/bin/bash
let TIMEOUT_SOLVER=60*60
let TIMEOUT_SOLVER_PL=1*TIMEOUT_SOLVER
let TIMEOUT_VERIPB=10*$TIMEOUT_SOLVER_PL

let MEMOUT_SOLVER=8192
let MEMOUT_SOLVER_PL=MEMOUT_SOLVER
let MEMOUT_VERIPB=1*$MEMOUT_SOLVER_PL

file_results="results.csv"
TMPDIR=tmpdir
instances="instances_to_test"

echo "instances, filename, res_runtime_without_prooflogging, res_time_genCardinals_without_PL, res_mem_without_prooflogging, res_runtime_with_prooflogging, res_proofsize, res_time_genCardinals, res_time_genCardinalDefinitions,  res_mem_with_prooflogging, res_runtime_verification, res_mem_verification, res_verification_succeeded" > ./$(echo $file_results)
for filename in $(ls $instances)
do
	gunzip ./instances_to_test/$filename
done

for filename in $(ls $instances)
do
	echo "start $filename"
	
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
	
	rm duration_totalizer.txt
	
	# run
	runlim -r $TIMEOUT_SOLVER -s $MEMOUT_SOLVER -o $TMPDIR/$filename.txt ./../../qmaxsat_without_PL -log_duration_totalizer ./instances_to_test/$filename 
	
	# extract time
	res_runtime_without_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	# extract space
	res_mem_without_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	#extract time for genCardinals
	res_time_genCardinals_without_PL=$(cat duration_totalizer.txt | grep 'genCardinals:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');
	    		
	# status
	status=$(cat $TMPDIR/$filename.txt | grep 'status:' | awk '{print $3}');

	echo "$filename without prooflogging:"
	echo "runtime: $res_runtime_without_prooflogging "
	echo "mem: $res_mem_without_prooflogging"
	echo "runtime genCardinals: $res_time_genCardinals_without_PL"

	if [[ "$status" == "ok" ]]
	then
		
		rm maxsat_proof.pbp
		rm duration_totalizer.txt
		
		# run
	    	runlim -r $TIMEOUT_SOLVER_PL -s $MEMOUT_SOLVER_PL -o $TMPDIR/$filename.txt ./../../qmaxsat -log_duration_totalizer ./$instances/$filename 

	    	# extract time
	    	res_runtime_with_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	    	# extract space
	    	res_mem_with_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	    	# status
	    	status=$(cat $TMPDIR/$filename.txt | grep 'status:' | awk '{print $3}');
	    	
	    	#extract time for genCardinals
	    	res_time_genCardinals=$(cat duration_totalizer.txt | grep 'genCardinals:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');
	    	
	    	if [[ "$res_time_genCardinals" == "" ]]
	    	then
	    		res_time_genCardinals="NA"
	    	fi
	    	   		    	
	    	#extract time for genCardinalDefinitions
	    	res_time_genCardinalDefinitions=$(cat duration_totalizer.txt | grep 'genCardinalDefinitions:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');
	    	
	    	if [[ "$res_time_genCardinalDefinitions" == "" ]]
	    	then
	    		res_time_genCardinalDefinitions="NA"
	    	fi
	    	
	    	# extract proof size
	    	proofsize=$(stat --printf="%s" maxsat_proof.pbp)
	    
	    	if [[ "$proofsize" == "" ]]
		then
			proofsize="NA"
		fi
	    		    	
	    	echo "$filename with prooflogging:"
		echo "runtime: $res_runtime_with_prooflogging "
		echo "mem: $res_mem_with_prooflogging"
		echo "duration genCardinalDefinitions: $res_time_genCardinalDefinitions"
		echo "duration genCardinals: $res_time_genCardinals"
		echo "proofsize: $proofsize"
		
		if [[ "$status" == "ok" ]]
		then	
			
			# run
        		runlim -r $TIMEOUT_VERIPB -s $MEMOUT_VERIPB -o $TMPDIR/$filename.txt veripb --wcnf ./$instances/$filename ./maxsat_proof.pbp > $TMPDIR/res.txt

        		# extract time
        		res_runtime_verification=$(cat $TMPDIR/$filename.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

		       # extract space
       		res_mem_verification=$(cat $TMPDIR/$filename.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

		       # status
       		status=$(cat $TMPDIR/$filename.txt | grep 'status:' | awk '{print $3}');
    
    			echo "$filename verification:"
			echo "runtime: $res_runtime_verification "
			echo "mem: $res_mem_verification"
			
			if grep -q "succeeded" $TMPDIR/res.txt; then
				res_verification_succeeded=1
			else
				res_verification_succeeded=0	
			fi
		fi
	fi
	echo "$instances, $filename, $res_runtime_without_prooflogging, $res_time_genCardinals_without_PL, $res_mem_without_prooflogging, $res_runtime_with_prooflogging, $proofsize, $res_time_genCardinals, $res_time_genCardinalDefinitions,  $res_mem_with_prooflogging, $res_runtime_verification, $res_mem_verification, $res_verification_succeeded" >> ./$(echo $file_results)
done

