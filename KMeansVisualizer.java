package linearAlgebra.kmeans;

import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.util.List;

public class KMeansVisualizer extends JFrame {
	private List<Point> points;
	private List<Point> centroids;
	private List<List<Point>> clusters;
	private int scaling;
	private int padding = 50;
	private Color[] colors;

	public KMeansVisualizer(List<Point> points, List<Point> centroids, List<List<Point>> clusters) {
		this.points = points;
		this.centroids = centroids;
		this.clusters = clusters;

		int maxX = (int) points.stream().mapToDouble(p -> p.x).max().orElse(800);
		int maxY = (int) points.stream().mapToDouble(p -> p.y).max().orElse(800);
		scaling = Math.min(700 / maxX, 700 / maxY);

		setTitle("KMeans Visualization");
		setSize((maxX * scaling) + padding, (maxY * scaling) + padding);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		add(new GraphPanel());
		setLocationRelativeTo(null);

		generateColors();
	}

	private void generateColors() {
		var ran = new Random();
		colors = new Color[clusters.size()];
		for(int i = 0; i < clusters.size(); i++) {
			colors[i] = new Color(ran.nextInt(255), ran.nextInt(255), ran.nextInt(255));
		}
	}

	private class GraphPanel extends JPanel {
		@Override
		protected void paintComponent(Graphics g) {
			super.paintComponent(g);
			Graphics2D g2d = (Graphics2D) g;
			g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

			g2d.setColor(Color.BLACK);
			for(Point centroid:centroids) {
				int x = (int) (centroid.x * scaling);
				int y = (int) (centroid.y * scaling);
				g2d.fillOval(x-5, y-5, 8, 8);
			}

			g2d.setColor(Color.GRAY);
			for(int i = 0; i < clusters.size(); i++) {
				for(int j = 0; j < clusters.get(i).size(); j++) {
					int x1 = (int) (centroids.get(i).x * scaling);
					int y1 = (int) (centroids.get(i).y * scaling);
					int x2 = (int) (clusters.get(i).get(j).x * scaling);
					int y2 = (int) (clusters.get(i).get(j).y * scaling);
					g2d.drawLine(x1, y1, x2, y2);
				}
			}

			for(int i = 0; i < clusters.size(); i++) {
				g2d.setColor(colors[i]);
				for(Point p:clusters.get(i)) {
					int x = (int) (p.x * scaling);
					int y = (int) (p.y * scaling);
					g2d.fillOval(x, y, 5, 5);
				}
			}
		}
	}
	public static void run(List<Point> points, List<Point> centroids, List<List<Point>> clusters) {
		SwingUtilities.invokeLater(() ->  {
			KMeansVisualizer vis = new KMeansVisualizer(points, centroids, clusters);
			vis.setVisible(true);
		});
	}
}
