--SYMMETRY ENGINE
--PHYSICS TEST

function OnLoad(ScriptName)

	--Load scene
	loadscene("physicstest.sym.scene")

	--Set the gunship's mass
	setmass("gunship", 5.0)
	
	--Set the gunship's initial velocity
	setvelocity("gunship", SYM_VECTOR3(0, 0, 160))

end