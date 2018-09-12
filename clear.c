#include "types.h"
#include "stat.h"
#include "user.h"

/*	Author: 	Joseph Harris
 *	Date:		9/5/2018
 *	Content:	Single-line program that clears the screen using 
 * 			ANSI escape characters
*/

int
main(int argc, char *argv[])
{
  write(1, "\33[H\33[2J", 7);		//"\33[H\33[2J" is the ANSI escape sequence to clear the screen
					//and set the cursor back in the correct space.
					//"\33[2J" = ANSI sequence to clear the entire screen
					//"\33[H" = ANSI sequence to return the cursor to the "[H"ome position
  exit();
}
