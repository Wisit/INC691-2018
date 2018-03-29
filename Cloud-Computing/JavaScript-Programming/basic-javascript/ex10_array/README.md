## ex10_array
This example demonstrates how to use the ***Array*** in different ways. The ***Array*** object is a global object that is used in the construction of arrays; which are high-level, list-like objects.

```javascript
'use strict'     // use strict mode

/*
 * Example 1 -- Create an array
 */
let numArr = [1, 3, 5, 7];
console.log('Length of numArr: ', numArr.length);
console.log('Data in numArr:', numArr);

let strArr = ['ant', 'bat', 'cat', 'dog', 'pig'];
console.log('Length of strArr: ', strArr.length);
console.log('Data in strArr:', strArr);


/*
 * Example 2 -- Access (index into) an array item
 */
console.log('The first element of numArr:', numArr[0]);
console.log('The last element of strArr:', strArr[strArr.length - 1]);


/*
 * Example 3 -- Loop over an array
 */
for(let i=0; i<numArr.length; i++) {
    console.log('numArr[' + i + '] = ' + numArr[i]);
}

for(let i=0; i<strArr.length; i++) {
    console.log('strArr[' + i + '] = ' + strArr[i]);
}

numArr.forEach(function(item, index, array){
    console.log(item, index, array);
});

strArr.forEach(function(item, index, array){
    console.log(item, index, array);
});


/*
 * Example 4 -- push() -- Add an item to the end of an array
 */
console.log('Before push:', numArr);
numArr.push(9);     // push() is used to add an item to the end of an array
console.log('After push: ', numArr);


/*
 * Example 5 -- pop() -- Remove an item from the end of an array
 */
console.log('Before pop:', strArr);
strArr.pop();       // pop() is used to remove an item to the end of an array
console.log('After pop: ', strArr);


/*
 * Example 6 -- unshift() -- Add an item to the front of an array
 */
console.log('Before unshift:', numArr);
numArr.unshift(4);  // unshift() used to add an item to the front of an array
console.log('After unshift: ', numArr);


/*
 * Example 7 -- shift() -- Remove an item from the front of an array
 */
console.log('Before shift:', strArr);
strArr.shift();     // shift() is used to remove an item from the front of an array
console.log('After shift: ', strArr);


/*
 * Example 8 -- indexOf() -- Find the index of an item in the array
 */
console.log('Index of number 7 in numArr is', numArr.indexOf(7));
console.log('Index of number 0 in numArr is', numArr.indexOf(0));  // Returns -1 means Not Found

console.log('Index of "dog" in strArr is', strArr.indexOf('dog'));
console.log('Index of "fox" in strArr is', strArr.indexOf('fox')); // Returns -1 means Not Found


/*
 * Example 9 -- splice() -- Remove an item by index position from an array
 */
console.log('Before splice:', numArr);
numArr.splice(3, 2);          // remove items at index3 and index4
                              // The first argument (3) is the index
                              // The second argument (2) is number of items to be removed
console.log('After splice: ', numArr);

console.log('Before splice:', strArr);
const remIdx = strArr.indexOf("dog") // find/search the position (index) of the "dog"
strArr.splice(remIdx, 1);            // remove the "dog" from the strArr
console.log('After splice: ', strArr);

```
