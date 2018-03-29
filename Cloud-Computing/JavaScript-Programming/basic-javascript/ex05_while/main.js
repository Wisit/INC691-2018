'use strict'                              // use strict mode

/*
 * Example 1 -- while(true), the infinite loop with berak
 */
while(true) {
   var x = Math.random()*10;             // Generates a number randomly 
   console.log('x:', x);                 // Prints the generated number
   if(x > 9) {                           // Check if x is greater  than 9
       break;                            // Exits from this while loop
   }
}

/*
 * Example 2 -- while(true), the infinite loop with continue and break
 */
while(true) {
    var x = Math.random()*10;             // Generates a number randomly 
    console.log('x:', x);                 // Prints the generated number
    if(x < 9) {                           // Check if x is less than 9
        continue;                         // Continue to next turn, don't move to lines below
    }
    break;                                // Exits from this while loop
 }

/*
 * Example 3 -- while loop with condition checking
 */
var num = 20;                             // Declare variable num with initial value 20
while(num > 10) {                         // Do the loop, if the num is greater than 10
    console.log('num:', num);             // Prints the value of num
    num = num - 1;                        // Decreases value of num by 1
}


/*
 * Example 4 -- do, while
 */
var q = 17;                              // Declare variable q with initial value 17
console.log('initial value of q:', q);
do{
    console.log('q:', q);                // Lines in the do{} are always executed in the first iteration
    q += 2;                              // Increases value of q by 2
}while(q < 15)                           // Do next loop/iteration if the q is less than 15

/*
 * Example 4 -- do, while with multiple conditions checking
 */
var r = 'rat';                           // Declare variable r with initial value 'rat'
var cnt = 0;                             // Declare variable cnt with initial value 0
console.log('initial values:', r, cnt);
do{
    console.log('values:', r, cnt);      // Lines in the do{} are always executed in the first iteration
    cnt++;                               // Increases value of cnt by 1
    if(cnt > 10) {
        r = 'dog';
    }
}while( (cnt < 5) || (r !== 'dog') );    // Do next loop/iteration if one or more condition is true
