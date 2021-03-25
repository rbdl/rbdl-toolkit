#!/bin/python3
# A small python program that will center meshes that are given as arguments

import numpy as np
import sys
import re
from scipy.spatial.transform import Rotation as R

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

        rotation = R.from_rotvec(np.pi/2 * np.array([1, 0, 0]))

        transformed = rotation.apply(vertexes)
        print(transformed.shape)

        vertexes = transformed

        # write transformed mesh file
        with open(meshfile, "w") as of:
            for line in prefix:
                of.write(line)

            for vertex in vertexes:
                of.write("v %f %f %f\n" % (vertex[0], vertex[1], vertex[2]))
#
            for line in postfix:
                of.write(line)


if __name__ == "__main__":
    main()
