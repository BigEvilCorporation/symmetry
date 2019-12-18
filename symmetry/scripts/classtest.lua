-- SYMMETRY ENGINE
-- Lua classes test

V1 = SYM_VECTOR3:new(0, 0, 0)

function OnLoad(ScriptName)

	loadscene("physicstest.sym.scene")

	V1 = getposition("gunship")

	consoleprint("Gunship: X: "..V1.x.." Y: "..V1.y.." Z: "..V1.z)

end

function Tick(DeltaTime)

	V1 = V1 + SYM_VECTOR3:new(0.01 * DeltaTime, 0, 0)
	setposition("gunship", V1)

end