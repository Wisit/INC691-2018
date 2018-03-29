## ex08_setTimeout
This example demonstrates how to use the ***setTimeout*** function in different ways. Using ***callback*** function, ***anonymous*** function and ***arrow*** function are also explained.

```javascript
'use strict'                  // use strict mode

/*
 * Example 1 -- setTimeout with a callback function
 */
function callback1() {
    console.log('Ex1: Timeout!!');
}
console.log('Ex1: setTimeout() is started, please wait 5 seconds');
setTimeout(callback1, 5000);

/*
 * Example 2 -- setTimeout with an anonymous function
 */
console.log('Ex2: setTimeout() is started, please wait 6 seconds');
setTimeout(function(){
    console.log('Ex2: Timeout!!');    
}, 6000);

/*
 * Example 3 -- setTimeout with an arrow function
 */
console.log('Ex3: setTimeout() is started, please wait 7 seconds');
setTimeout( () => {
    console.log('Ex3: Timeout!!');    
}, 7000);


```
