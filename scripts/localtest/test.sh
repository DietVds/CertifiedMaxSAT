#!/bin/bash
let TIMEOUT_SOLVER=60*60
let TIMEOUT_SOLVER_PL=1*TIMEOUT_SOLVER
let TIMEOUT_VERIPB=10*$TIMEOUT_SOLVER_PL

let MEMOUT_SOLVER=8192
let MEMOUT_SOLVER_PL=MEMOUT_SOLVER
let MEMOUT_VERIPB=1*$MEMOUT_SOLVER_PL

file_results="results.txt"
TMPDIR=tmpdir
instances="instances_to_test"

echo "filename,runtime_without_prooflogging,runtime_with_prooflogging,runtime_verification, verification_succeeded" > ./$(echo file_results)
echo "instances, filename, res_runtime_without_prooflogging, res_mem_without_prooflogging, res_runtime_with_prooflogging, res_mem_with_prooflogging, res_runtime_verification, res_mem_verification, res_verification_succeeded" > ./$(echo file_results)

for filename in $(ls mse21_complete_unwt/easy_instances/)
do
	gunzip ./instances_to_test/$filename
done

for filename in $(ls "$instances")
do
	echo "start $filename"
	
	res_runtime_without_prooflogging="NA"
	res_mem_without_prooflogging="NA"
	res_runtime_with_prooflogging="NA"
	res_mem_with_prooflogging="NA"
	res_runtime_verification="NA"
	res_mem_verification="NA"
	res_verification_succeeded="NA"

	# run
	runlim -r $TIMEOUT_SOLVER -s $MEMOUT_SOLVER -o $TMPDIR/$filename.txt ./qmaxsat_without_prooflogging ./instances_to_test/$filename 
	
	# extract time
	res_runtime_without_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	# extract space
	res_mem_without_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	# status
	status=$(cat $TMPDIR/$filename.txt | grep 'status:' | awk '{print $3}');

	echo "$filename without prooflogging:"
	echo "runtime: $res_runtime_without_prooflogging "
	echo "mem: $res_mem_without_prooflogging"

	if [[ "$status" == "ok" ]]
	then
		
		rm maxsat_proof.pbp
		# run
	    	runlim -r $TIMEOUT_SOLVER_PL -s $MEMOUT_SOLVER_PL -o $TMPDIR/$filename.txt ./../../qmaxsat -log_duration_totalizer ./$instances/$filename 

	    	# extract time
	    	res_runtime_with_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'real:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	    	# extract space
	    	res_mem_with_prooflogging=$(cat $TMPDIR/$filename.txt | grep 'space:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');

	    	# status
	    	status=$(cat $TMPDIR/$filename.txt | grep 'status:' | awk '{print $3}');
	    	
	    	#duration genCardinalDefinitions
	    	res_time_genCardinalDefinitions=$(cat duration_totalizer.txt | grep 'genCardinalDefinitions:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');
	    	
	    	#duration genCardinals
	    	res_time_genCardinals=$(cat duration_totalizer.txt | grep 'genCardinals:' | grep -Eo '[+-]?[0-9]+([.][0-9]+)?');
	    		    	
	    	echo "$filename with prooflogging:"
		echo "runtime: $res_runtime_with_prooflogging "
		echo "mem: $res_mem_with_prooflogging"
		echo "duration genCardinalDefinitions: $res_time_genCardinalDefinitions"
		echo "duration genCardinals: $res_time_genCardinals"
		
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
	echo "$instances, $filename, $res_runtime_without_prooflogging, $res_mem_without_prooflogging, $res_runtime_with_prooflogging, $res_time_genCardinalDefinitions, $res_time_genCardinals, $res_mem_with_prooflogging, $res_runtime_verification, $res_mem_verification, $res_verification_succeeded" >> ./$(echo file_results)
done

