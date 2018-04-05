
/**
 * 2-D data generation and visualization
 */

let viewer = new PointViewer('canvas', 600, 600);
let point = new PointData();

/**
 * Generate some data randomly
 */
let d1 = point.randomMultiplePoints(150, 50,  100, 20, 30);
let d2 = point.randomMultiplePoints(300, 40,  350, 30, 30);
let d3 = point.randomMultiplePoints(500, 20,  300, 50, 30);

/**
 * Draw
 */
viewer.draw(d1);
viewer.draw(d2);
viewer.draw(d3);


/**
 * Create centroids
 */
let c1 = point.randomSinglePoint(150, 20,  100, 20).toCentroid('red');
let c2 = point.randomSinglePoint(300, 20,  350, 20).toCentroid('lime');
let c3 = point.randomSinglePoint(500, 20,  300, 20).toCentroid('#ff0');

/**
 * Draw
 */
viewer.draw(c1);
viewer.draw(c2);
viewer.draw(c3);