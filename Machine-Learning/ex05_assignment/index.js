
/**
 * 2-D data generation and visualization
 * Class assignment basd on the Eucredient distance
 */

let viewer = new PointViewer('canvas', 600, 600);
let point = new PointData();

let numPoints = 10;
let d1 = point.randomMultiplePoints(150, 50,  200, 50, numPoints);
let d2 = point.randomMultiplePoints(300, 50,  350, 50, numPoints);
let d3 = point.randomMultiplePoints(500, 50,  300, 50, numPoints);

let c1 = point.randomSinglePoint(200, 50,  200, 50).toCentroid('red');
let c2 = point.randomSinglePoint(300, 50,  350, 50).toCentroid('lime');
let c3 = point.randomSinglePoint(500, 50,  300, 50).toCentroid('#ff0');


/**
 * Mearge all data points and centroids
 */
let datapoints = d1.concat(d2).concat(d3);
let centroids  = [c1, c2, c3];
viewer.draw(datapoints);
viewer.draw(centroids);


/**
 * Example 1 Slow motion
 */
function doSlowAssignment() {
    let colors = ['rgba(255,0,0,0.5)', 'rgba(0,255,0,0.5)', 'rgba(255,255,0,0.5)'];
    let j = 0;
    let dist = [];
    let timer = setInterval( () => {
        viewer.draw(datapoints, true);   //!! redraw
        viewer.draw(centroids);

        //!! get a point
        let p = datapoints[j];

        //!! clear distant array
        dist = [];

        //!! calculate distance
        for(let i=0; i<centroids.length; i++) {
            let c = centroids[i];  
            viewer.drawEucredient(p, c, colors[i]);
            dist.push(p.distanceTo(c)); 
        }

        //!! search minimum distance
        let distMin = dist[0];
        let distIdx = 0;
        for(let i=0; i<dist.length; i++) {
            if(dist[i] < distMin) {
                distMin = dist[i];
                distIdx = i;
            }
        }

        //!! assign to the class
        p.color = colors[distIdx]; // centroids[distIdx].color;
        p.group = distIdx;

        if(++j >= datapoints.length) {
            clearInterval(timer);
            viewer.draw(datapoints, true); //!! redraw
            viewer.draw(centroids);
            console.log('Done!');
        }
    },200);
} 

// doSlowAssignment();




/**
 * Example 2 Assignment
 */
function doFastAssignment() {
    let colors = ['rgba(255,0,0,0.5)', 'rgba(0,255,0,0.5)', 'rgba(255,255,0,0.5)'];
    let j = 0;
    let dist = [];
    for(let j=0; j<datapoints.length; j++) {
        //!! get a point
        let p = datapoints[j];

        //!! clear distant array
        dist = [];

        //!! calculate distance
        for(let i=0; i<centroids.length; i++) {
            let c = centroids[i];  
            dist.push(p.distanceTo(c)); 
        }

        //!! search minimum distance
        let distMin = dist[0];
        let distIdx = 0;
        for(let i=0; i<dist.length; i++) {
            if(dist[i] < distMin) {
                distMin = dist[i];
                distIdx = i;
            }
        }

        //!! assign to the class
        p.color = colors[distIdx]; // centroids[distIdx].color;
        p.group = distIdx;
    }

    viewer.draw(datapoints, true);   //!! redraw
    viewer.draw(centroids);
} 
setTimeout(() =>{
    doFastAssignment();
}, 1000);



