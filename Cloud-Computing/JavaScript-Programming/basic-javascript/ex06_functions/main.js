'use strict'                  // use strict mode

/*
 * Example 1 -- No return, no input arguments
 */
function func1() {
    console.log('No return, no input argument function');
}
func1();                      // Calls the function func1

/*
 * Example 2 -- Has return, no input arguments
 */
function func2() {
    console.log('Has return, no input arguments functon');
    return 'Hello World!';    // It can return be a number or an object
}
var f2_ret = func2();         // Calls the function func2 and receives its return value
console.log('The func2 returns:', f2_ret); 

/*
 * Example 3 -- No return, has one input argument
 */
function func3(a) {
    console.log('No return, has one input argument');
    console.log('The value of input argument is', a);
}
func3(12);                    // Calls the function func3 and passes a number 12 to it
func3('Hello');               // Calls the function func3 and passes a string 'Hello' to it

/*
 * Example 4 -- No return, has two input arguments
 */
function func4(a, b) {
    console.log('No return, has two input arguments');
    console.log('The values of input arguments are', a, b);
}
func4(23, 33);                   // Calls the function func4 and passes numbers 23 and 33 to it
func4('Hello', 11);              // Calls the function func4 and passes a string 'Hello' and a number 11 to it


/*
 * Example 5 -- Has return, has two input arguments
 */
function func5(a, b) {
    console.log('Has return, has two input arguments');
    console.log('The values of input arguments are', a, b);
    return a + b;                 // Returns a + b (numerical operation or string concatination)
}
console.log( func5(5, 8) );       // Calls the function func5 and passes numbers 5 and 8 to it
console.log( func5(3, 'hello') ); // Calls the function func5 and passes a number 3 and a string 'hello' to it

/*
 * Example 6 -- Return an object, and required an object as input argument
 */
function func6(io) {            // The io (the input object used as input argument)
    var ro = {};                // Declares an empty object ro (used as returned object)
    ro.x = io.a + 5;            // Updates value of ro.x
    ro.y = io.b - 3;            // Updates value of ro.y
    return ro;                  // Returns the object, the ro
}
var obj = func6({a:10, b:20});  // Calls the func6 and passes an object (containing a and b) to it
console.log('obj.x:', obj.x);   // Prints the returned value, the obj.x
console.log('obj.y:', obj.y);   // Prints the returned value, the obj.y

/*
 * Note:
 * Object is very very useful in JavaScript and other languages
 * Don't worry, we will see in depth details later
 */
