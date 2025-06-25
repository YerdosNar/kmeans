import java.util.*;

public class Main {
	public static void main(String... args) {
		var sc = new Scanner(System.in);
		var ran = new Random();
		System.out.print("Input K: ");
		int k = sc.nextInt();
		System.out.print("Input X max coordinate: ");
		int x = sc.nextInt();
		System.out.print("Input Y max coordinate: ");
		int y = sc.nextInt();
		System.out.print("Input max iterations: ");
		int iterations = sc.nextInt();

		var points = new ArrayList<Point>();
		for(int i = 1; i <= x; i++) {
			for(int j = 1; j <= y; j++) {
				points.add(new Point(i, j));
			}
		}
//		for(int i = 0; i < 10000; i++) {
//			int a = ran.nextInt(x);
//			int b = ran.nextInt(y);
//			points.add(new Point(Math.max(a, b), Math.min(a, b)));
//		}

		long startTime = System.nanoTime();
		var kmeans = new KMeans(k, points);
		kmeans.run(iterations);
		var clusters = kmeans.getClusters();
		var centroids = kmeans.getCentroids();

		KMeansVisualizer.run(kmeans.getPoints(), kmeans.getCentroids(), kmeans.getClusters());
		long endTime = System.nanoTime();
		System.out.println("Elapsed time: " + (endTime - startTime) / 1000000 + " ms");
		int idx = 0;
		for(var centroid : centroids) {
			System.out.print("Cluster " + idx + "th coordinates: x = " + centroid.x + ", y = " + centroid.y + "\n");
			System.out.println("And has: " + clusters.get(idx).size());
			idx++;
		}
		sc.close();
	}
}
