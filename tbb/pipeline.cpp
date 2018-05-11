#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_allocator.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <iostream>
#include "tbb/compat/thread"

using namespace std;

#define NUM_PIPELINE_STAGE 100000
static unsigned long pr = 0;

class MyInputFilter : public tbb::filter{
public:
	MyInputFilter();
private:
	unsigned long c; 
	void* operator() (void*);

};

MyInputFilter::MyInputFilter() :
	filter(serial_in_order),
	c(0)
{

}

void* MyInputFilter::operator()(void*) {

	c++;

	if (c > 1)
		return NULL;

	return &c;
}


class MyTransformFilter: public tbb::filter {
public:
    MyTransformFilter();
    void* operator()( void* item );
private:
    unsigned long out;
};

MyTransformFilter::MyTransformFilter() : 
    tbb::filter(serial_in_order) 
{}


class MyOutputFilter: public tbb::filter {
    unsigned long output;
public:
    MyOutputFilter();
    void* operator()( void* item );
};


MyOutputFilter::MyOutputFilter() : 
    tbb::filter(serial_in_order),
    output(0)
{
}


void* MyOutputFilter::operator()( void* item ) {
    output = *static_cast<unsigned long*>(item);
    
    //cout <<  (unsigned) pthread_self() << endl;

    //cout<<"Output is: " << output << endl;

    pr = output;
//    cout << "Output is: " << output << endl;
    
    return NULL;
}

void* MyTransformFilter::operator()( void* item ) {
	unsigned long in = *static_cast<unsigned long*>(item);
	out = in + 1;

	return &out;
}


int run_pipeline(int nthreads)
{
	//create the pipeline
	tbb::pipeline pipeline;

	MyInputFilter input_filter;
	pipeline.add_filter(input_filter);

	MyTransformFilter transform_filter[NUM_PIPELINE_STAGE];

        //Spawn NUM_PIPELINE_STAGE of pipeline stages
	for (unsigned i=0; i<NUM_PIPELINE_STAGE; i++)
	{
		pipeline.add_filter(transform_filter[i]);
	}


	MyOutputFilter output_filter;	
	pipeline.add_filter(output_filter);

	tbb::tick_count t0 = tbb::tick_count::now();
	pipeline.run(nthreads);
	tbb::tick_count t1 = tbb::tick_count::now();

	cout << "A parallel version of " << NUM_PIPELINE_STAGE << " pipeline stages took " << (t1-t0).seconds() * 1000000.0 << " ns to run" << endl;
	cout << "Output = " << pr << endl;

	return 1;

}

//Squential version
int run_seq()
{
	unsigned long r = 1;
	
	tbb::tick_count t0 = tbb::tick_count::now();
	for (unsigned i=0; i<NUM_PIPELINE_STAGE; i++)
	{
		r = r + 1;
	}
	
	tbb::tick_count t1 = tbb::tick_count::now();
	
	
	cout << "A sequential version stages took " << (t1-t0).seconds() * 1000000.0 << " ns to run" << endl;
	cout << "Output = " << r << endl;
	
}


int main (int argc, char* argv[]) {
		int n = tbb::task_scheduler_init::default_num_threads();
                cout << "TBB default number of threads: " << n << endl;

                if(!run_pipeline (n))
                {
                    return 1;
                }

		run_seq();

		return 0;
}
