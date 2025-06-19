package linearAlgebra.kmeans;

public class Point {
	public double x, y;

	public Point(double x, double y) {
		this.x = x;
		this.y = y;
	}
	
	public double distance(Point p) {
		double xCoor = this.x - p.x;
		double yCoor = this.y - p.y;
		return Math.sqrt(xCoor * xCoor + yCoor * yCoor);
	}
}
