-- SYMMETRY ENGINE
-- Rotation (pitch, yaw, roll) test

function OnLoad(ScriptName)

	addbox("PitchCrate", 10, 10, 10, 0, 0, 0)
	addbox("YawCrate", 10, 10, 10, 20, 0, 0)
	addbox("RollCrate", 10, 10, 10, 40, 0, 0)

	processscene()

end

function Tick(DeltaTime)

	pitch("PitchCrate", 0.1 * DeltaTime)
	strafe("PitchCrate", 0.01 * DeltaTime)

	yaw("YawCrate", 0.1 * DeltaTime)
	fly("YawCrate", 0.01 * DeltaTime)

	roll("RollCrate", 0.1 * DeltaTime)
	move("RollCrate", 0.01 * DeltaTime)

end