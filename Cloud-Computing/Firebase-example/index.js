
const ref = firebase.database().ref();

//!!
//!! Ex1
//!!
// ref.child("Text").set("Hello World!!");


//!!
//!! Ex2
//!!
// for(let i=0; i<10; i++) {
//     ref.child("data"+i).set(Math.random()*100);  
// }


//!!
//!! Ex3
//!!
// const N = 10;
// let count = 0;
// const timer = setInterval(() => {
//     ref.child("real-time").set(Math.random()*100);
//     if(count++ >= 5) {
//         clearInterval(timer);
//     }
// }, 2000);



//!!
//!! Ex4 push (random key)
//!! 
// ref.push().set('message'); 
// ref.push().set('message'); 


//!!
//!! Ex5 receive data
//!! 
// const key = 'real-time';
// const refData = ref.child(key);
// refData.on('value', (snapshot) => {
//     console.log(snapshot.val());
// });



//!!
//!! Ex6 read data
//!! 
// const targetKey = 'Users';
// const refUsers = ref.child(targetKey);
// refUsers.on('child_added', (snap) => {
//     let name = snap.child('name').val();
//     let email = snap.child('email').val();
//     console.log('name:', name, ', ', 'email:', email);
    
// });


//!!
//!! Ex7 write array
//!! 
// let array_data = ['hello', 'world'];
// ref.child("array-data").set(array_data);


//!!
//!! Ex8 read array
//!! 
// const refArray = ref.child('array-data');

//!! Ex8.1
// refArray.on('value', (snap) => {
//     console.log('Ex8.1:', snap.val());
// });
//!! Ex8.2
// refArray.on('child_added', (snap) => {
//     console.log('Ex8.2:', snap.val());
// });


//!!
//!! Ex8 write an object
//!! 
// const box = {size:12, color:'red'};
// ref.child("obj-data").set(box);

//!!
//!! Ex9 write an array of objects
//!! 
// const boxes = [
//     {size:34, color:'green'}, 
//     {size:13, color:'yellow'},
//     {size:65, color:'red'},
// ];
// ref.child("Boxes").set(boxes);

//!!
//!! Ex10 write other complex data
//!!
// const Users = [
//     {
//         Name: 'user1',
//         Email: 'u1@gmail.com',
//         Scores: [23, 64, 46, 22]
//     },
//     {
//         Name: 'user2',
//         Email: 'u2@gmail.com',
//         Scores: [12, 89, 24, 53]
//     },
//     {
//         Name: 'user3',
//         Email: 'u3@gmail.com',
//         Scores: [45, 84, 24, 96]
//     },
// ];
// ref.child("Users").set(Users);