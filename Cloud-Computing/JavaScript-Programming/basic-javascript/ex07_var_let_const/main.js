'use strict'                  // use strict mode

/*
 * Example 1 -- var can be redeclared
 */
/* 1.1 1st declartion */
 var x = 10;                 // Declare the variable var x and give it an initial varlue 10
 console.log('x:', x);

 /* 1.2 2nd declartion */
 var x = 11;                 // Re-Declare the variable var x AGAIN, and give it an initial varlue 12
 console.log('x:', x);

 /*
 * Example 2 -- let cannot be redeclared
 */
/* 2.1 1st declartion */
let y = 10;
console.log('y:', y);

/* 2.2 2nd declartion will cause the  error */
// let y = 11;              // SyntaxError: Identifier 'y' has already been declared


 /*
 * Example 3 -- const cannot be redeclared
 */
/* 3.1 1st declartion */
const z = 12;
console.log('z:', z);

/* 3.2 2nd declartion will cause the  error */
// const z = 13;           // SyntaxError: Identifier 'z' has already been declared

 /*
 * Example 4 -- var and let can be declared without initial value (undefined) but const cannot
 */
var   a;                   // OK
console.log(a);            // The result is undefined

let   b;                   // OK
console.log(b);            // The result is undefined

// const c;                // SyntaxError: Missing initializer in const declaration

/*
 * Example 5 -- var can be accessed outside its block
 */
for(var i=0; i<5; i++) {
    console.log('inside, i:', i);
}
console.log('outside, i:', i); // The variable var i can be accessed ouside the block 
                               // even though it is declared in the for-loop 

/*
 * Example 6 -- let cannot be accessed outside its block
 */
for(let j=0; j<5; j++) {
    console.log('inside, j:', j);
}
// console.log('outside, j:', j); // ReferenceError: j is not defined


/*
 * Example 7 -- Change (re-assign) value of variables can be applied on var and let only
 */
var   v1 = 1;
let   v2 = 2;
const v3 = 3;
console.log('initial values (v1, v2, v3):', v1, v2, v3);
v1 = 10;                         // ok
v2 = 20;                         // ok
// v3 = 30;                      // TypeError: Assignment to constant variable.
console.log('v1, v2:', v1, v2);

/*
 * Example 8 -- Three of them (var, let, const) cannot be accessed outside its function
 */
function func1()
{
    var   q = 10;
    let   r = 11;
    const s = 12;
}
// console.log('outside, q:', q);  // ReferenceError: q is not defined
// console.log('outside, r:', r);  // ReferenceError: r is not defined
// console.log('outside, s:', s);  // ReferenceError: s is not defined
