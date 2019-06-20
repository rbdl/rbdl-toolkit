#!/bin/python3
# A small python program that will center meshes that are given as arguments

import numpy as np
import sys
import re

def main():

	if len(sys.argv) < 2:
		print("Usage: %s meshfile [meshfile] ..." % sys.argv[0])
		return

	for meshfile in sys.argv[1:]:

		mesho_reg = re.compile("^o .*")
		with open(meshfile) as f:
			cnt = 0
			for line in f:
				if not mesho_reg.match(line) is None:
					cnt += 1
			if cnt > 1:
				print("File %s contains multiple meshes, which this script does not support ... Skipping!" % meshfile)
				continue

		prefix = []
		postfix = []
		vertex_section_found = False
		vertex_reg = re.compile("^v .*")
		vertexes = []

		# read vertex data and save the rest
		with open(meshfile) as mf:
			for line in mf:
				result = vertex_reg.match(line)
				if result is None:
					if not vertex_section_found:
						prefix.append(line)
					else:
						postfix.append(line)
				else:
					if not vertex_section_found:
						vertex_section_found = True
					linesplit = line.split(" ")
					vertex = np.array(linesplit[1:])
					vertex = np.asfarray(vertex, float)
					vertexes.append(vertex)

		vertexes = np.array(vertexes)
		print(meshfile, vertexes.shape)
		
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
		vertexes -= center

		# write transformed mesh file
		with open(meshfile, "w") as of:
			for line in prefix:
				of.write(line)

			for vertex in vertexes:
				of.write("v %f %f %f\n" % (vertex[0], vertex[1], vertex[2]))

			for line in postfix:
				of.write(line)


if __name__ == "__main__":
	main()
