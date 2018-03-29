'use strict'                    // use strict mode  

/*
 * Example 1 -- switch, case, break
 */
var x = 5;                      // Change value of x to another and check the result
console.log('x:', x);
switch(x)
{
    case 0:
        console.log('case 0');
    break;

    case 5:
        console.log('case 5');
    break;

    case 9:
        console.log('case 9');
    break;
}


/*
 * Example 2 -- switch, case, default, break
 */
var y = 'bat';                  // Change value of y to another and check the result
console.log('y:', y);
switch(y)
{
    case 'cat':
        console.log('case cat');
    break;

    case 'dog':
        console.log('case dog');
    break;

    case 'pig':
        console.log('case pig');
    break;

    default:
        console.log('Unknown animal');
    break;
}

/*
 * Example 3 -- switch, case, WITHOUT break
 */
var k = 3;                     // Change value of k to another and check the result
switch(k)
{
    case 1:
        console.log('k = 1');
    case 2:
        console.log('k = 2');
    case 3:
        console.log('k = 3');
    case 4:
        console.log('k = 4');
    case 5:
        console.log('k = 5');
}
