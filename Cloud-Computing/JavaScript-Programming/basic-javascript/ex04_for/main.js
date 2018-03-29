'use strict'                              // use strict mode

/*
 * Example 1 -- for-loop with count-up
 */

for(var i=0; i<10; i++) {                 // Starts from 0, up to 10, increases by 1
    console.log('i: ', i);
}

/*
 * Example 2 -- for-loop with count-down
 */
for(var j=9; j>=0; j--) {                 // Starts from 9, down to 0
    console.log('j: ', j*2);
}

/*
 * Example 3 -- for-loop with count-up by two
 */
for(var k=0; k<5; k+=2) {                // Starts from 0, up to 4, increases by 2
    console.log('k: ', k);
}

/*
 * Example 4 -- for-loop with break
 */
for(var n=0; ; n+=2) {                   // No condition checking in for()
    console.log('n: ', n*2);
    if(n*2 > 50) {
        break;                           // Exit from this loop when n*2 > 50   
    }
}

/*
 * Example 5 -- for-loop with continue and break
 */
for(var m=0; ; ) {                       // No condition checking and no variable changing
    console.log('m: ', m);
    m++;                                 // Increases value of m by 1
    if(m < 10) {
        continue;                        // Do next loop, don't run lines below
    }
    break;                               // Exit from this loop
}

/*
 * Example 6 -- for-loop, the wile(true) alternative
 */
var counter = 0;
for(;;) {                                // Works like while(true)
    console.log('counter: ', counter++);
    if(counter >= 5) {
        break;                           // Exit from this loop
    }
    continue;                            // Not required in this example
}
