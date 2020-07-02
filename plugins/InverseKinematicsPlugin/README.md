Inverse Kinematics Plugin
=========================

This is a relatively simple computation plugin. It finds models that have Markers and a corresponding Motion Capture, it will then calculate the inverse
kinematics to find the joint dofs for every mocap farme calculating a new animation for the model.

# Settings

IK.lambda : lambda to use for the inverese kinematics
IK.max_steps : maximum amount of steps to perform when running IK
IK.tolerance : tolerance for error on the calculated marker positions
