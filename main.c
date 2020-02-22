#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

#define BASE 6
#define NUM_THREADS 64

// typedef unsigned long int uint64_t;
// typedef signed long int int64_t;

//Fast convert characters to a unsigned 64bit integer
///@arg str: A null-terminated character string of letters between '0' and '9'
uint64_t fast_atoull( const char * str )
{
    uint64_t val = 0;
    //for every character in the string
    while( *str )
        //"shift" the value over and add the new character's value (*str++ is ugly but keeps this function super small)
        val = val*10 + (*str++ - '0');
    return val;
}

int main(int argc, char ** argv)
{
    //We tested thread numbers till we were convinced 64 was the optimal.
    omp_set_num_threads(NUM_THREADS);
    //for every argument passed to the program (except the first)
    for (int count=1; count<argc; ++count) {
        //convert that argument into a 64bit number.
        const int64_t num = fast_atoull(argv[count]);
        //given that all numbers are either prime or composite
        //a prime factor will be found between [1, n^0.5]
        //therefore we only need to look beween 1 and n^0.5.
        const int64_t sqrt_num = 1 + (int64_t) sqrtl(num);

        //this will hold the answer.
        int64_t answer = 0;

        //check "special cases" 2 and 3 as they don't follow the base 6 pattern
        if (num % 2 == 0)
            answer = 2;
        else if (num % 3 == 0)
            answer = 3;
        
        //if the answer was not 2 or 3.
        else {
            //find the lower prime factor between 5 and sqrt_num
            #pragma omp parallel for
            for (int64_t candidate = 5; candidate < sqrt_num; candidate += BASE) {
                //if the candidate is a prime factor of num, the prime sibling will be
                //  somewhere well above the sqrt of the number meaning that answer will only be set once.
                if (num % candidate == 0)
                    answer = candidate;
                //so we can make this check and assignment here and know its safe, because mathematically
                //  it can never happen again
                if (num % (candidate + 2) == 0)
                    answer = candidate + 2;
                //the only exception is when the number is n^2 and even then we only set answer once
            };
        }
        //could not find the answer which means big problems something went wrong.
        if (!answer)
            answer = 1;
        //finally print out the answer
        printf("%lld = %lld * %lld\n", num, answer, num / answer);
    }
    return 0;
}
