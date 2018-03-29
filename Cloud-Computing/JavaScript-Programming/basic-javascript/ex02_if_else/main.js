'use strict'                  // use strict mode  

/*
 * Example 1 -- if
 */
var x = 10;                   // Change value of x to another and check the result
console.log('x:', x)
if(x > 5) {                   // The comparition can be ==, !=, >, >=, <, <=
    console.log('x > 5');
}


/*
 * Example 2 -- if, else
 */
 var y = 'hello';             // Change value of y to another and check the result
 console.log('y:', y);
 if(y == 'hello') {
     console.log(y, 'world');
 }
 else {
    console.log('hello there');  
 }

/*
 * Example 3 -- if, else if
 */
var z = 23;
console.log('z:', z);   
if(z > 10) {
    console.log('z > 10');   
}
else if(z > 15) {
    console.log('z > 15');   
}

/*
 * Example 4 -- if, else if, else
 */
var k = 19;
console.log('k:', k);   
if(k < 10) {
    console.log('k < 10');   
}
else if(k <= 15) {
    console.log('k <= 15');   
}
else {
    console.log('All conditions above are false!');
}

/*
 * Example 5 -- value and type checking
 */
var m = '10';                             // m is a string containing '10'
/* 5.1 Value comparision */
if(m == 10) {                             // Compare with number 10 (value only)
    console.log('Yes, string m == number 10');
}
else {
    console.log('No, string m != number 10');  
}

/* 5.2 Value and Type comparision, the === or !== can be used*/
if(m === 10) {                           // Compare with number 10 (value and type)
    console.log('Yes, string m === number 10');
}
else {
    console.log('No, suring m !== number 10');  
}


/*
 * Example 6 -- Multiple conditions checking
 */
var num1 = 32;
var str1 = 'cat';
/* 6.1 using && (AND, not Logical AND operator) */
if( (num1 > 30) && (str1 !== 'dog') ) {
    console.log('6.1: Yes both of them are true');
}

/* 6.2 using || (OR, not Logical OR operator) */
if( (num1 > 30) || (str1 !== 'dog') ) {
    console.log('5.2: Yes one or more is true');
}
