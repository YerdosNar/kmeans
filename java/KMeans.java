import java.util.*;

public class KMeans {
	private int k;
	private List<Point> points;
	private List<Point> centroids;
	private List<List<Point>> clusters;

	public KMeans(int k, List<Point> ps) {
		this.k = k;
		this.points = ps;
		this.centroids = new ArrayList<>();
		this.clusters = new ArrayList<>();
		initCentroids();
	}

	private void initCentroids() {
		// Step 1: Randomly choose the first centroid from the points list.
		centroids.add(points.get(new Random().nextInt(points.size())));

		// Step 2: Select each remaining centroid
		for (int i = 1; i < k; i++) {
			List<Double> distances = new ArrayList<>();

			// Calculate the squared distance from each point to its nearest centroid
			for (Point p : points) {
				double minDist = Double.POSITIVE_INFINITY;
				for (Point centroid : centroids) {
					double distance = p.distance(centroid);
					minDist = Math.min(minDist, distance * distance);
				}
				distances.add(minDist);
			}

			// Choose the next centroid with a weighted probability based on distance
			double totalDistance = distances.stream().mapToDouble(Double::doubleValue).sum();
			double r = new Random().nextDouble() * totalDistance;

			double cumulativeDistance = 0;
			for (int j = 0; j < points.size(); j++) {
				cumulativeDistance += distances.get(j);
				if (cumulativeDistance >= r) {
					centroids.add(points.get(j));
					break;
				}
			}
		}
	}

	private int assignPointToClusters(Point p) {
		double minDis = Double.POSITIVE_INFINITY;
		int closestCluster = 0;
		for(int i = 0; i < k; i++) {
			double dis = p.distance(centroids.get(i));
			if(dis < minDis) {
				minDis = dis;
				closestCluster = i;
			}
		}
		return closestCluster;
	}

	private void updateCentroids() {
		for(int i = 0; i < k; i++) {
			double sumX = 0;
			double sumY = 0;
			for(Point p: clusters.get(i)) {
				sumX += p.x;
				sumY += p.y;
			}
			centroids.set(i, new Point(sumX / clusters.get(i).size(), sumY / clusters.get(i).size()));
		}
	}

	public void run(int maxIterations) {
		int iterations = 0;
		double threshold = 0.0001;

		while(iterations < maxIterations) {
			clusters.clear();
			for(int i = 0; i < k; i++) {
				clusters.add(new ArrayList<>());
			}

			for(Point p:points) {
				int cluster = assignPointToClusters(p);
				clusters.get(cluster).add(p);
			}

			var oldCentroids = new ArrayList<>(centroids);
			updateCentroids();

			for(int i = 0; i < k; i++) {
				if(oldCentroids.get(i).distance(centroids.get(i)) > threshold) {
					break;
				}
			}
			iterations++;
		}
	}

	public List<Point> getCentroids() {
		return centroids;
	}

	public List<List<Point>> getClusters() {
		return clusters;
	}

	public List<Point> getPoints() {
		return points;
	}
}
