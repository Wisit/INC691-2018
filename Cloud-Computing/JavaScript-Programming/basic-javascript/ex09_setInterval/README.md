## ex09_setInterval
This example demonstrates how to use the ***setInterval*** function in different ways. Using ***callback*** function, ***anonymous*** function and ***arrow*** function are also explained.

```javascript
'use strict'                  // use strict mode

/*
 * Example 1 -- setInterval with a callback function
 */
let counter1 = 0;
function callback1() {
    console.log('Ex1: counter1:', ++counter1);
}
console.log('Ex1: setInterval() is started, the counter1 will be incresed every 2 seconds');
const t1 = setInterval(callback1, 2000);

/*
 * Example 2 -- setInterval with an anonymous function
 */
let counter2 = 0;
console.log('Ex2: setInterval() is started, the counter2 will be incresed every 1 seconds');
const t2 = setInterval(function(){
    console.log('Ex2: counter2:', ++counter2);
}, 1000);

/*
 * Example 3 -- setInterval with an arrow function
 */
let counter3 = 0;
console.log('Ex2: setInterval() is started, the counter2 will be incresed every 3 seconds');
const t3 = setInterval( () => {
    console.log('Ex3: counter3:', ++counter3);
}, 3000);


/**
 * This Timeout will clear/stop all timers (t1, t2 and t3) after 10 seconds
 * We can hit CTRL+C to terminate before it get 10 seconds
 */
setTimeout(() => {
    clearTimeout(t1);
    clearTimeout(t2);
    clearTimeout(t3);
    console.log('Goodbye, all Intervals are cleared/stopped');
}, 10000);

```
