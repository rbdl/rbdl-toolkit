rod_length = 1

meshes = {
  rod1 = {
	color = { 1, 0, 0},
	mesh_center = {0, 0, -rod_length/2},
	dimensions = { 0.2, 0.2, rod_length},
	src = "unit_cube.obj"
  },
  rod2 = {
	color = { 0, 1, 0},
	mesh_center = {0, 0, -rod_length/2},
	dimensions = { 0.1, 0.1, rod_length},
	src = "unit_cube.obj"
  }
}

model = {
  configuration = {
	axis_right = { -1, 0, 0 },
	axis_front = { 0, -1, 0 },
	axis_up =    { 0, 0, -1 },
  },

  frames = {
	{
	  name = "segment1",
	  parent = "ROOT",
	  visuals = { meshes.rod1 },
	  joint = {{ 0, 1, 0, 0, 0, 0 }},
	  joint_frame = {
		E = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}
	  }
	},
	{
	  name = "segment2",
	  parent = "segment1",
	  visuals = { meshes.rod2 },
	  joint = {{ 0, 1, 0, 0, 0, 0 }},
	  joint_frame = {
		r = { 0., 0., -rod_length },
		E = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}
	  }
	}
  }
}

return model

