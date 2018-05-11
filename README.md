Here we provide two simple examples showing what will happen when the compiler fails to optimise across pipeline boundaries using Intel TBB/ICC and Google Golang. 

In the examples, we create a pipeline of 100K stages. Each pipeline stage just adds one to its input. We compare the parallel version against a sequential version that just uses a simple for loop to carry out the same task. If the compiler is aware of the parallel patterns and can optimise across pattern boundaries, it could generate an optimised version to compute a single statement of 1 + 100000 = 100001. 

However, the current implementation of Intel TBB/ICC and Golang fail to do so, leading to a 
slowdown of 3,000x over the sequential version as shown in the following diagram. 


![Alt text](/result.png?raw=true "Result Diagram")



Results were obtained by running the examples on a server with a 40-core Intel Xeon E5-2650 CPU @ 2.30 GHz, 64 of RAM, running CentOS 7 with Linux kernel 3.10. We used Intel ICC v18.02 and TBB v4.4 (compiled with -O3), and go v1.1.0.
