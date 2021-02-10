rod_length = 1

meshes = {
  rod1 = {
	color = { 1, 0, 0},
	mesh_center = {0, 0, -rod_length/2},
	dimensions = { 0.1, 0.1, rod_length},
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
  gravity = { 0, -9.81, 0 },

  configuration = {
	axis_front = { 1, 0, 0 },
	axis_up = { 0, 0, 1 },
	axis_right = { 0, 1, 0 },
  },

  frames = {
	{
	  name = "segment1",
	  parent = "ROOT",
	  visuals = { meshes.rod1 },
	  joint = {{ 0, 1, 0, 0, 0, 0 }},
	  joint_frame = {
		E = {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}}
	  }
	},
	{
	  name = "segment2",
	  parent = "segment1",
	  visuals = { meshes.rod2 },
	  joint = {{ 0, 1, 0, 0, 0, 0 }},
	  joint_frame = {
		r = { 0., 0., -rod_length },
		E = {{1, 0, 0}, {0, -1, 0}, {0, 0, -1}}
	  }
	}
  }
}

return model

