--SYMMETRY ENGINE
--Default input handler script

-- Tweakables
MoveSpeed = 0.1
StrafeSpeed = 0.07

-- Screen resolution
ScreenX = 0
ScreenY = 0

function OnLoad(ScriptName)

	-- Get screen resolution
	ScreenX, ScreenY = getscreenresolution()

	-- Run this script by default
	runscript(ScriptName)

end


function OnTick(DeltaTime)

	-- Check for escape key, quit
	if iskeypressedascii(27) == 1 then shutdown() end

	-- Check keys, move/strafe camera
	if iskeydown("w") == 1 then movecamera(DeltaTime * MoveSpeed) end
	if iskeydown("s") == 1 then movecamera(-DeltaTime * MoveSpeed) end
	if iskeydown("a") == 1 then strafecamera(-DeltaTime * StrafeSpeed) end
	if iskeydown("d") == 1 then strafecamera(DeltaTime * StrafeSpeed) end

	-- Get mouse relative position
	MouseX, MouseY = getmouserelative()

	-- Get mouse resolution
	ResX, ResY = getmouseresolution()

	-- Pitch and Yaw camera
	yawcamera(-MouseX * ResX)
	pitchcamera(-MouseY * ResY)

	-- Warp mouse to centre
	--warpmouse(ScreenX * 0.5, ScreenY * 0.5)

end