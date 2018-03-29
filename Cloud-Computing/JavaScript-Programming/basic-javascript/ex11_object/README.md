## ex11_object
This example demonstrates how to use the JavaScript ***Object*** in different ways. The JavaScript is designed on a simple object-based paradigm. An object is a collection of properties, and a property is an association between a *name* (or *key*) and a *value*. A property's value can be a function, in which case the property is known as a method. 

```javascript
'use strict'     // use strict mode

/*
 * Example 1 -- Create an object
 */
let car = new Object(); // create an empty object
console.log('car', car);

/*
 * Example 2 -- Define object's propeties
 */
car.color = 'red';
console.log('car', car);

car.year = 1982;
console.log('car', car);

car['model'] = 'Mustang'; // using bracket notation
console.log('car', car);

/*
 * Example 3 -- Access object's propeties by name
 */
console.log('Color:', car.color);
console.log('Year:', car['year']);
car.model = 'C320';  // change property value
console.log(car);

/*
 * Example 4 -- Access object's propeties using hasOwnProperty()
 */
for(let prop in car) {
    if(car.hasOwnProperty(prop)) {
        console.log(prop, car[prop]);
    }
}


/*
 * Example 5 -- Deleting property using delete()
 */
console.log('Before delete:', car);
delete car.year;  // delete/remove the year property from the car object
console.log('After delete: ', car);


/*
 * Example 6 -- Creating a new object using object initializer
 */
let paper1 =  {width: 100, height: 50, color:'green'};
console.log(paper1);

// An object can be a property of another object
let paper2 = {color: 'yellow', dimensions: {width: 30, height: 50} };
console.log(paper2);
console.log("paper2's width:", paper2.dimensions.width);

/*
 * Example 7 -- Creating a new object using a constructor function
 */
function Pen(length, radius, color) {
    // Notice the use of this to assign values to the object's properties 
    // based on the values passed to the function
    this.length = length;
    this.radius = radius;
    this.color  = color;
}
// Create a pen
let pen = new Pen(15, 1, 'red');
console.log(pen);

/*
 * Example 8 -- Comparing objects
 */
let penA = new Pen(15, 1, 'green');
let penB = new Pen(15, 1, 'green');
console.log(penA == penB);  // false
console.log(penA === penB); // false

penA = penB; // assign penB object reference to penA

// here penA and penB are pointing to same object
console.log(penA == penB);  // true
console.log(penA === penB); // true

// changing the property of penA, the property of the penB is also changed
// because they are pointing to same object
penA.color = 'red';
console.log(penA);
console.log(penA);


/*
 * Example 9 -- Method of an object
 */
function Person(name, age, parent) {
    // Properties
    this.name   = name;
    this.age    = age;
    this.parent = parent;
    // Method
    this.info = function() {
        console.log('Name: ', this.name);
        console.log('Age: ', this.age);
        console.log('Parent Name: ', this.parent ? this.parent.name : "None");
    }
}

// Create objects
let somchai = new Person('Somchai', 60, null);
let sompong = new Person('Sompong', 30, somchai);
let somsri  = new Person('Somsri',  10, sompong);
// call the method, the info()
somchai.info();
sompong.info();
somsri.info();


/*
 * Example 10 -- Defining getters and setters
 */

let circle = {
    r: 10,
    get Area() {
        return Math.PI * this.r * this.r;
    },
    set Radius(new_r) {
        this.r = new_r;
    }
    
};

console.log('r', circle.r);           // check property r
console.log('area', circle.Area);     // get the Area property

circle.Radius = 5;                    // set the Radius property to a new value
console.log(circle.r);                // check property r
console.log(circle.Area);             // get the Area property

```
