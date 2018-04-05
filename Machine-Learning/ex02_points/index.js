
/**
 * 2-D data generation and visualization
 */

let viewer = new PointViewer('canvas', 600, 600);
let point = new PointData();

let data_g1 = point.randomMultiplePoints(200, 50,  500, 20, 30);
let data_g2 = point.randomMultiplePoints(300, 60,  400, 10, 30);
let data_g3 = point.randomMultiplePoints(500, 10,  200, 80, 30);
let data_g4 = point.randomMultiplePoints(200, 40, 150, 50, 30);
let data_g5 = point.randomMultiplePoints(300, 50, 250, 30, 30);
let data_g6 = point.randomMultiplePoints(500, 20, 500, 20, 30);

/**
 * Draw
 */
viewer.draw(data_g1);
viewer.draw(data_g2);
viewer.draw(data_g3);

/**
 * change their color
 */
data_g4.map( d =>d.color='red' );
data_g5.map( d =>d.color='lime');
data_g6.map( d =>d.color='#ff0');

/**
 * Draw
 */
viewer.draw(data_g4);
viewer.draw(data_g5);
viewer.draw(data_g6);

