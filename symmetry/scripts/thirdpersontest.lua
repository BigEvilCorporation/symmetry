-- SYMMETRY ENGINE
-- Third person camera test script

-----------------------------------------
-- TWEAKABLES

-- Distance from host object to camera
CameraDistance = 50.0

-- Crate movement speed
MoveSpeed = 0.05
StrafeSpeed = 0.05

-----------------------------------------

-- Previous camera pitch
OldPitch = 0.0

function OnLoad(ScriptName)

	-- Load a crate
	loadmodel("crate", "crate.sym.mesh")

	-- Build octree
	processscene()

	-- Draw grid
	drawgrid(true)

	-- Stop default input script
	stopscript("_input")

	-- Run this script
	runscript(ScriptName)

end 


function Tick(DeltaTime)

	-- Check keys, move/strafe crate
	if iskeydown("w") == 1 then move("crate", -DeltaTime * MoveSpeed) end
	if iskeydown("s") == 1 then move("crate", DeltaTime * MoveSpeed) end
	if iskeydown("a") == 1 then strafe("crate", -DeltaTime * StrafeSpeed) end
	if iskeydown("d") == 1 then strafe("crate", DeltaTime * StrafeSpeed) end

	-- Get mouse relative position
	MouseX, MouseY = getmouserelative()

	-- Get mouse resolution
	ResX, ResY = getmouseresolution()

	-- Yaw crate using (inverted) mouse X
	yaw("crate", -(MouseX * ResX))

	-- Get crate position and rotation
	PosX, PosY, PosZ = getposition("crate")
	RotX, RotY, RotZ = getrotation("crate")

	-- Get camera rotation
	CamX, CamY, CamZ = getcamerarot()

	-- Save pitch
	OldPitch = OldPitch + (MouseY * ResY)

	-- Move camera to crate's (inverted) position and rotation
	setcamerapos(-PosX, -PosY, -PosZ)
	setcamerarot(-RotX, -RotY, -RotZ)

	-- Pitch camera using (inverted) mouse Y
	pitchcamera(-OldPitch)

	-- Move camera back to distance
	movecamera(-CameraDistance)

end
	