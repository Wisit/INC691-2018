
/**
 * 2-D data generation and visualization
 * Update/move centroids basd on Eucredient distances
 */

function KMeans() {

    let animimator = null;
    const canvasWidth  = JsUtils.getWindowWidth();
    const canvasHeight = JsUtils.getWindowHeight();

    let viewer = new PointViewer('canvas', canvasWidth, canvasHeight);
    let point = new PointData();


    const colors5 = ['#f00', '#0f0', '#ff0', '#f0f', '#0ff'];


    function randInt(min, max) {
        return Math.floor(min + Math.random() * (max-min));
    }


    //!! Centroids Array
    let centroids  = [];

    //!! Data points array
    let datapoints = [];

    //!! Initial centroids based on global mean value
    function initCentroids() {
        centroids = [];
        let sx = 0;
        let sy = 0;
        for(let i=0; i<datapoints.length; i++) {
            sx += datapoints[i].x;
            sy += datapoints[i].y;
        }
        sx /= datapoints.length;    // uX
        sy /= datapoints.length;    // uY

        //!! Create centroids, 3 - 5 centoids
        const nc = 3 + Math.floor(Math.random()*3);
        for(let i=0; i<nc; i++) {
            centroids.push(point.randomSinglePoint(sx, 5,  sy, 5).toCentroid( colors5[i] ));
        }
    }

    
    /**
     * Example 1 Update centoids (Slow motion)
     */
    let cenDist = 0;
    let numIterations = 0;
    const MAX_ITERATIONS = 50;
    const TERMINATION_TH = 10



    this.addPoints = function(muX, sigmaX, muY, sigmaY, points) {
        //console.log(muX, sigmaX, muY, sigmaY, points);
        let pts = point.randomMultiplePoints(muX, sigmaX, muY, sigmaY, points); 
        datapoints = datapoints.concat(pts);
        viewer.draw(datapoints, true);
        //viewer.draw(centroids);
        //doKMeans();
    }

    this.clearPoints = function() {
        cancelAnimationFrame(animimator); 
        datapoints = [];  
        centroids = [];
        viewer.draw(datapoints, true); 
        //viewer.draw(centroids);
    }

    this.start = function() {
        if(datapoints.length <= 0) {
            let pts = point.randomMultiplePoints(canvasWidth/2, canvasWidth/8, canvasHeight/2, canvasHeight/8, Math.floor(100+Math.random()*100)); 
            datapoints = datapoints.concat(pts);  
        }

        //!! Create centroids based on Mu
        initCentroids();


        //!!
        for(let i=0; i<datapoints.length; i++) {
            delete datapoints[i].group; 
            datapoints[i].color = null; 
        }

        
        let cenDist = 0;
        let numIterations = 0;

        viewer.draw(datapoints, true);
        viewer.draw(centroids);
        doKMeans();   
    }



    
    function doKMeans() {
        let colors = [
            'rgba(255,0,0,0.5)', 
            'rgba(0,255,0,0.5)',
            'rgba(255,255,0,0.5)',
            'rgba(2550,0,255,0.5)',
            'rgba(0,255,255,0.5)',
        ];

        let pIdx = 0;   //!! point index
        let dist = [];  //!! distance between points to a centroid

        
        //!! Remove members from the centroids and assign to group i
        for(let i=0; i<centroids.length; i++) {
            centroids[i].members = [];  // remove all memberas
            centroids[i].group = i;     // assign to group i
        }



        function printInfo() {
            let ypos = 20;
            let xpos = 10;
            function nextY(){ ypos+=20; return ypos;}
            viewer.print('Dr.Santi Nuratch', xpos, nextY(), 'rgba(255,255,255,0.5)', 14);
            viewer.print('Machine Learning Algorithm', xpos, nextY(), 'rgba(255,255,10,0.5)', 14);
            viewer.print('Embedded Computing and Control Laboratory', xpos, nextY(), 'rgba(50,255,85,0.5)', 14);
            viewer.print('ECC-Lab | INC-KMUTT', xpos, nextY(), 'rgba(255,150,50,0.5)', 14);
            viewer.print('02 April 2018', xpos, nextY(), 'rgba(155,155,255,0.5)', 14);
            // viewer.print('---------------------------------------------------', 10, 120, 'rgba(155,155,155,0.5)', 14);

            nextY(); // add empty line
            for(let i=0; i<centroids.length; i++) {
                viewer.print('Class-' + i + ': ' + centroids[i].members.length, xpos, nextY(),  centroids[i].color);
            }
            viewer.print('Iteration: ' + (numIterations+1), xpos, nextY(), 'rgba(255,255,255,0.5)'); 
            
            let err = (cenDist==0)?TERMINATION_TH/2 + Math.random() : cenDist;
            viewer.print('Error[' + (numIterations+1) +']: ' + err.toFixed(2), xpos, nextY(), 'rgba(255,255,255,0.5)'); 
        }



        function doCentroidAdaptation() {
        //let timer = setInterval( () => {
            viewer.draw(datapoints, true);  //!! clear screen and draw points
            viewer.draw(centroids);         //!! draw centroids

            //!! get a point
            let p = datapoints[pIdx];

            //!! clear distant array
            dist = [];

            //!! calculate euclidean distance and draw connected line (a point to all centroids)
            for(let i=0; i<centroids.length; i++) {
                let c = centroids[i];  
                viewer.drawEucredient(p, c, colors[i], 2);
                dist.push( p.distanceTo(c) ); 
                
            }

            //!! search minimum distance between a point and centroids
            let distMin = dist[0];
            let classIdx = 0;
            for(let i=0; i<dist.length; i++) {
                if(dist[i] < distMin) {
                    distMin = dist[i];
                    classIdx = i;
                }
            }

           
            //!! assign to the class and give it color
            p.color = colors[classIdx];
            p.group = classIdx;


            //!! Check if it is alreader added
            let alreadyAdded = false;
            for(let i=0; i < centroids[classIdx].members.length; i++) {
                if(p === centroids[classIdx].members[i]) {
                    alreadyAdded = true;
                    break;
                }
            }
            if(alreadyAdded === false) {

                //!! add a point to the member of the centroid
                centroids[classIdx].members.push(p);

                //!! search the point(s) that belonged to another group
                let remIdx = [];
                for(let i=0; i<centroids.length; i++) {
                    let c = centroids[i];
                    for(let j=0; j < centroids[i].members.length; j++) {
                        let p = centroids[i].members[j];
                        if(p.group != c.group) {
                            //!! the point in grpup-i, index-j will be removed
                            remIdx.push({'g':i, 'idx':j});
                        }
                    }
                }
                //!! remove the point(s) that belonged to another group
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

            cenDist += Math.abs(centroids[classIdx].x - sx/centroids[classIdx].members.length);
            cenDist += Math.abs(centroids[classIdx].y - sy/centroids[classIdx].members.length);

            centroids[classIdx].x = sx/centroids[classIdx].members.length;
            centroids[classIdx].y = sy/centroids[classIdx].members.length;

            

            //!! Print system information
            printInfo();
            
          
            
            //!!
            if(++pIdx >= datapoints.length) {
                pIdx = 0;
                viewer.draw(datapoints, true); //!! redraw
                viewer.draw(centroids);
                numIterations++;

                printInfo();

                if(numIterations >= MAX_ITERATIONS || cenDist < TERMINATION_TH) {
                    cancelAnimationFrame(animimator);  
                    console.log('Done!, No error');  
                    return; 
                }
                console.log('Iterations: ' + numIterations);
                cenDist = 0;
            }

            animimator = window.requestAnimationFrame(doCentroidAdaptation);
        }

        doCentroidAdaptation();
    } 

    
}

