#include "types.h"
#include "stat.h"
#include "user.h"

/*	Author: 	Joseph Harris
 *	Date: 		9/5/2018
 *	Contents:	Uses the Euclidean Algorithm to find the greatest common divisor
 *			of all arguments passed to it.
 * 			This program is a utility for xv6.
*/


int gcd(int a, int b)			//this is a modulus-based algorithm
{
  while(a%b!=0) {	
    int tmp = b;
    b = a%b;
    a = tmp;
  }

  return b;
}

int
main(int argc, char *argv[])
{
  int ret;

  if(argc < 3){
    printf(2, "Usage: gcd num1 num2...\n");
    exit();
  }

  int a = atoi(argv[1]);
  int b = atoi(argv[2]);

  if(a<b) ret = gcd(b,a);			//initially get gcd of first two arguments and store the result in ret
  else ret = gcd(a,b);			//gcd(smaller int, bigger int);
  
  int i=3;
  int next;
  while(argv[i]) {				//n stores the result of each gcd() call.
    next = atoi(argv[i++]);			//if there are more than 2 args, loop through argv's
    if(ret < next) ret = gcd(ret,next);		//gcd of the result and the next command-line arg
    else ret = gcd(next,ret);
  }
  printf(1, "gcd(");
  i = 1;
  while(argv[i+1]) {
    printf(1,"%s, ", argv[i++]);
  }
  printf(1, "%s) = %d\n", argv[i], ret);

  exit();
}
