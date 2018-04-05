
/**
 * 1-D data generation and visualization
 */

const data   = new Data();
const viewer = new DataViewer('canvas', 800, 400);

const rand1 = data.randomMultipleNumbers(+10, 5,  100);
const rand2 = data.randomMultipleNumbers(+50, 10, 100);
const rand3 = data.randomMultipleNumbers(-10, 2,  100);
const rand4 = data.randomMultipleNumbers(-50, 30, 100);

viewer.line(rand1, '#f00', 1);
viewer.line(rand2, '#0f0', 1);
viewer.line(rand3, '#0ff', 1);
viewer.line(rand4, '#f0f', 1);

/**
 * ref: https://www.mathsisfun.com/data/
 */