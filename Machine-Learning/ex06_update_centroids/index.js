
/**
 * 2-D data generation and visualization
 * Update/move centroids basd on mean value of the Eucredient distances
 */

let viewer = new PointViewer('canvas', 600, 600);
let point = new PointData();

let numPoints = 100;
let d1 = point.randomMultiplePoints(150, 50,  200, 60, numPoints);
let d2 = point.randomMultiplePoints(300, 40,  350, 80, numPoints);
let d3 = point.randomMultiplePoints(500, 50,  300, 70, numPoints);

let c1 = point.randomSinglePoint(350, 10,  350, 10).toCentroid('red');
let c2 = point.randomSinglePoint(350, 10,  350, 10).toCentroid('lime');
let c3 = point.randomSinglePoint(350, 10,  350, 10).toCentroid('#ff0');


/**
 * Mearge all data points and centroids
 */
let datapoints = d1.concat(d2).concat(d3);
let centroids  = [c1, c2, c3];
viewer.draw(datapoints);
viewer.draw(centroids);


/**
 * Example 1 Update centoids (Slow motion)
 */
let numIterations = 3;
function doSlowAssignment() {
    let colors = ['rgba(255,0,0,0.5)', 'rgba(0,255,0,0.5)', 'rgba(255,255,0,0.5)'];
    let j = 0;
    let dist = [];

    let animimator = null;

    for(let i=0; i<centroids.length; i++) {
        centroids[i].members = [];
        centroids[i].group = i;
    }

    function doCentroidAdaptation() {
    //let timer = setInterval( () => {
        viewer.draw(datapoints, true);   //!! redraw
        viewer.draw(centroids);

        //!! get a point
        let p = datapoints[j];

        //!! clear distant array
        dist = [];

        //!! calculate distance
        for(let i=0; i<centroids.length; i++) {
            let c = centroids[i];  
            viewer.drawEucredient(p, c, colors[i], 1.5);
            dist.push(p.distanceTo(c)); 
            
        }

        //!! search minimum distance
        let distMin = dist[0];
        let classIdx = 0;
        for(let i=0; i<dist.length; i++) {
            if(dist[i] < distMin) {
                distMin = dist[i];
                classIdx = i;
            }
        }

        //!! assign to the class
        p.color = colors[classIdx]; // centroids[classIdx].color;
        p.group = classIdx;


        //!!
        let alreadyAdded = false;
        for(let i=0; i < centroids[classIdx].members.length; i++) {
            if(p === centroids[classIdx].members[i]) {
                alreadyAdded = true;
                break;
            }
        }
        if(alreadyAdded === false) {

            //!! Add new member/child to current class
            centroids[classIdx].members.push(p);

            //!! search points that assigned in wrong class
            let remIdx = [];
            for(let i=0; i<centroids.length; i++) {
                let c = centroids[i];
                for(let j=0; j < centroids[i].members.length; j++) {
                    let p = centroids[i].members[j];
                    if(p.group != c.group) {
                        remIdx.push({'g':i, 'idx':j});
                        console.log('changed from', p.group, 'to', c.group);
                    }
                }
            }
            //!! remove th wrong points
            if(remIdx.length > 0) {
                for(let i=0; i<remIdx.length; i++) {
                    let g = remIdx[i].g;
                    let idx = remIdx[i].idx;
                    centroids[g].members.splice([idx],1);
                }
            }
        }

        

        //!! Update the current centroid
        let sx = 0;
        let sy = 0;
        for(let i=0; i < centroids[classIdx].members.length; i++) {
            sx +=  centroids[classIdx].members[i].x;
            sy +=  centroids[classIdx].members[i].y;
        }
        centroids[classIdx].x = sx/centroids[classIdx].members.length;
        centroids[classIdx].y = sy/centroids[classIdx].members.length;
        
        

        if(++j >= datapoints.length) {
            j = 0;
            console.log('Iterations', numIterations);
            numIterations--;
            viewer.draw(datapoints, true); //!! redraw
            viewer.draw(centroids);
            if(numIterations <= 0) {
                console.log('Done!');  
                cancelAnimationFrame(animimator);  
                return;    
            }
        }

        animimator = window.requestAnimationFrame(doCentroidAdaptation);
    }

    doCentroidAdaptation();
} 

doSlowAssignment();




/**
 * Example 2 Assignment
 */
// function doFastAssignment() {
//     let colors = ['rgba(255,0,0,0.5)', 'rgba(0,255,0,0.5)', 'rgba(255,255,0,0.5)'];
//     let j = 0;
//     let dist = [];
//     for(let j=0; j<datapoints.length; j++) {
//         //!! get a point
//         let p = datapoints[j];

//         //!! clear distant array
//         dist = [];

//         //!! calculate distance
//         for(let i=0; i<centroids.length; i++) {
//             let c = centroids[i];  
//             dist.push(p.distanceTo(c)); 
//         }

//         //!! search minimum distance
//         let distMin = dist[0];
//         let distIdx = 0;
//         for(let i=0; i<dist.length; i++) {
//             if(dist[i] < distMin) {
//                 distMin = dist[i];
//                 distIdx = i;
//             }
//         }

//         //!! assign to the class
//         p.color = colors[distIdx]; // centroids[distIdx].color;
//         p.group = distIdx;
//     }

//     viewer.draw(datapoints, true);   //!! redraw
//     viewer.draw(centroids);
// } 
// setTimeout(() =>{
//     doFastAssignment();
// }, 1000);



