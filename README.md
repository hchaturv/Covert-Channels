__Objective__

To implement Covert Channels between two coresident VMS

__Methods__

__Approach 1__ Using host networking interface
	a.	 Both VMs profile the normal load of the host network interface
	
	b. 	After a predecided amount of time one of the VM would start overloading the host NIC
	
	c. 	The second VM would profile the NIC 
	
	d. 	Based on displacement of the load from the normal load a 0 or a 1 would be assumend by the second VM
	

Assumptions:
	1. 	This theory assumes that both the VMs boot up at the same time
	
	2. 	Profiling is being done by using ping response time. I will later pass on a paper that proves that ping
	responses can be used to compute with high accuracy the cpu load.
	
	3. 	I know of a tool that can be used for the profiling part - Weka, it will be great if you guys can read about it a little so that we can discuss it later.
	

__Approach 2__ Using host file de-duplicator
	a. 	The host would have a de-duplicator that would basically, de-duplicate the common files. Duh! The files that are common to both VMs would be stored as one with each VM having a reference to that file.
	
	b. 	After some time, one VM would take a pre-decided file and make changes to only one chunk of it, such that the remaining chunk is still duplicate.
	
	c. 	The second VM would then try to write at predecided chunks.
	
	d. 	Based on the access time for each chunk, (which can be same as before or less) the second VM would discern whether its a one or a zero.
	

Premise:
	1. This approach is based on the fact that a de-duplicator would break the file in chunks, the duplicate chunks would still be on the common disk implying more access time where as the chunk which is not duplicate any more would reside on the local memory of individual VMs implying lesser access times.


_We had, after last discussion with Professor Coffman, decided that we would be going ahead with the first approach as, according to him, the second one might take considerable amount of time._
