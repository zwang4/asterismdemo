package main

import "fmt"
import "time"

//Source of the pipeline
func gen() <-chan int {
    max := 1 
    out := make(chan int)
    go func() {
        for n:=0; n<max; n++ {
            out <- n
        }
        close(out)
    }()
    return out
}


func add(in <-chan int) <-chan int {
    out := make(chan int)
    go func() {
        for n := range in {
            out <- n + 1
        }
        close(out)
    }()
    return out
}

func main() {

     max := 100000

    /*Pipeline version start*/
     start := time.Now()
     c := gen()
     r := add(c)
 
     //create max number of pipeline stages
     for n:=0; n<max; n++ {
	r = add(r)
     }

     t := time.Now();
     pelapsed := t.Sub(start);
     
     pr := 0
     for n := range r {
        pr = n
     }
    
     fmt.Printf("A pipeline of %d stages took %v to run (result=%d)\n", max, pelapsed, pr)

     /*Pipeline versione ends*/

     /*Sequential version starts*/
     start = time.Now()
     ss := 1
     for i:=0; i<max; i++ {
	 ss = ss + 1;
     }

     t = time.Now();
     selapsed := t.Sub(start);
     fmt.Printf("A sequential version using a for loop took %v to run (result=%d)\n", selapsed, ss)

     /*Sequential version ends*/

     //fmt.Print("The parallel version is %f times slower than the sequential counterpart", pelapsed/selapsed)
     
}
