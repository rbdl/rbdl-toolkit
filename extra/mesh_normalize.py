#!/bin/python3
# A small python program that will normalize all meshes that are give to it via the cmd
# To use it you need to build and install openmesh (https://www.openmesh.org)

import openmesh as om
import numpy as np
import sys

def main():
	if len(sys.argv) < 2:
		print("Usage: %s meshfile [meshfile] ..." % sys.argv[0])
		return
	for meshfile in sys.argv[1:]:
		mesh = om.read_trimesh(meshfile)

		vertexes = mesh.points()
		bbox_x_min = min(vertexes[:,0])
		bbox_y_min = min(vertexes[:,1])
		bbox_z_min = min(vertexes[:,2])

		bbox_x_max = max(vertexes[:,0])
		bbox_y_max = max(vertexes[:,1])
		bbox_z_max = max(vertexes[:,2])

		diff_x  = bbox_x_max - bbox_x_min
		diff_y  = bbox_y_max - bbox_y_min
		diff_z  = bbox_z_max - bbox_z_min

		center = np.array([(bbox_x_max+bbox_x_min)*0.5, (bbox_y_max+bbox_y_min)*0.5, (bbox_z_max+bbox_z_min)*0.5])
		print(center)

		vertexes /= np.array([diff_x, diff_y, diff_z])
		om.write_mesh(meshfile, mesh)

if __name__ == "__main__":
	main()
