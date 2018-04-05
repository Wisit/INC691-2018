
/**
 * 2-D data generation and visualization
 * Eucredient distance
 */

let viewer = new PointViewer('canvas', 600, 600);
let point = new PointData();

let numPoints = 10;

/**
 * Generate some data randomly
 */
let d1 = point.randomMultiplePoints(150, 50,  200, 50, numPoints);
let d2 = point.randomMultiplePoints(300, 50,  350, 50, numPoints);
let d3 = point.randomMultiplePoints(500, 50,  300, 50, numPoints);

/**
 * Draw
 */
viewer.draw(d1);
viewer.draw(d2);
viewer.draw(d3);


/**
 * Create 3 centroids
 */
let c1 = point.randomSinglePoint(200, 50,  200, 50).toCentroid('red');
let c2 = point.randomSinglePoint(300, 50,  350, 50).toCentroid('lime');
let c3 = point.randomSinglePoint(500, 50,  300, 50).toCentroid('#ff0');

/**
 * Draw
 */
viewer.draw(c1);
viewer.draw(c2);
viewer.draw(c3);



/**
 * Example 1
 */
viewer.drawEucredient(d2[0], c1, c1.color, true);
viewer.drawEucredient(d3[5], c2, c2.color, true);
viewer.drawEucredient(d2[7], c3, c3.color, true);



/**
 * Example 2 -- d1 points to a centroid c1
 */
// for(let i=0; i<d1.length; i++) {
//     viewer.drawEucredient(d1[i], c1, c1.color);   
//     console.log(i, d1[i].distanceTo(c1));
// }

/**
 * Example 3 -- d1 points to a centroid c1 (slow motion)
 */
// var i = 0;
// var timer = setInterval(() => {
//     viewer.drawEucredient(d1[i], c1, c1.color); 
//     console.log(i, d1[i].distanceTo(c1));
//     i++;  
//     if(i >= d1.length) {
//         console.log('Done!');
//         clearInterval(timer);
//     }  
// }, 200);


/**
 * Example 3 -- d1 points to all centroids (slow motion)
 */
// let i = 0;
// let timer1 = setInterval(() => {
//     viewer.drawEucredient(d1[i], c1, c1.color);
//     viewer.drawEucredient(d1[i], c2, c2.color);
//     viewer.drawEucredient(d1[i], c3, c3.color); 
//     console.log(i, d1[i].distanceTo(c1), d1[i].distanceTo(c2), d1[i].distanceTo(c3));
//     i++;  
//     if(i >= d1.length) {
//         console.log('Done!');
//         clearInterval(timer1);
//     }  
// }, 200);


/**
 * Example 4 -- connect them all, points to centroids (slow motion)
 */
// let allPoints = d1.concat(d2).concat(d3);
// let j = 0;
// var timer2 = setInterval(() => {
//     viewer.drawEucredient(allPoints[j], c1, c1.color);
//     viewer.drawEucredient(allPoints[j], c2, c2.color);
//     viewer.drawEucredient(allPoints[j], c3, c3.color); 
//     console.log(j, allPoints[j].distanceTo(c1), allPoints[j].distanceTo(c2), allPoints[j].distanceTo(c3));
//     j++;  
//     if(j >= allPoints.length) {
//         console.log('Done!');
//         clearInterval(timer2);
//     }  
// }, 200);


/**
 * Example 5 -- connect them all, centroids to points
 */
// function connected_them_all() {
//     let allPoints = d1.concat(d2).concat(d3);
//     let allCentroids = [c1, c2, c3];
//     //!! centroids
//     for(let i=0; i<allCentroids.length; i++) {
//         let c = allCentroids[i];
//         // points
//         for(let j=0; j<allPoints.length; j++) {
//             let p = allPoints[j];
//             // calculate distance
//             viewer.drawEucredient(p, c, c.color);
//             console.log(i, j, p.distanceTo(c));
//         } 
//     }

// } 
// connected_them_all();


/**
 * Example 6 -- connect them all, centroids to points (slow motion)
 */
// function connected_them_all() {
//     let allPoints = d1.concat(d2).concat(d3);
//     let allCentroids = [c1, c2, c3];
//     let i = 0;
//     let j = 0;
//     //!! centroids
//     let timer = setInterval( () => {
//         //!! centroid
//         let c = allCentroids[i];
//         //!! points
//         let p = allPoints[j];
//         //!! calculate distance
//         viewer.drawEucredient(p, c, c.color);
//         console.log(i, j, p.distanceTo(c));
//         //!! next point
//         if(++j >= allPoints.length) {
//             j = 0;
//             //!! next centroid
//             if(++i >=  allCentroids.length) {
//                 clearInterval(timer);   
//             }
//         }
//     },60);
// } 
// connected_them_all();

/**
 * Example 7 -- connect them all, points to centroids (slow motion)
 */
// function connected_them_all() {
//     let allPoints = d1.concat(d2).concat(d3);
//     let allCentroids = [c1, c2, c3];
//     let i = 0;
//     let j = 0;
//     //!! centroids
//     let timer = setInterval( () => {
//         //!! centroid
//         let c = allCentroids[i];
//         //!! points
//         let p = allPoints[j];
//         //!! calculate distance
//         viewer.drawEucredient(p, c, c.color);
//         console.log(i, j, p.distanceTo(c));
//         //!! next point
//         if(++i >= allCentroids.length) {
//             i = 0;
//             //!! next centroid
//             if(++j >=  allPoints.length) {
//                 clearInterval(timer);   
//             }
//         }
//     },60);
// } 
// connected_them_all();


