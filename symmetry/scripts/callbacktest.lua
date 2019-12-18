-- SYMMETRY ENGINE
-- Physics callbacks test

function OnLoad(ScriptName)

	--Load scene
	loadscene("physicstest.sym.scene")

	--Set the gunship's mass
	setmass("gunship", 5.0)
	
	--Set the gunship's initial velocity
	setvelocity("gunship", 0, 0, 160)

end


function OnRun()

	-- Run physics
	runphysics()

end


function Tick(DeltaTime)

	-- Check if 'r' key is pressed and released
	if iskeypressed("r") == 1
	then 
		-- Get camera position
		CamPosX, CamPosY, CamPosZ = getcamerapos()

		-- Get camera rotation
		CamRotX, CamRotY, CamRotZ = getcameradir()

		-- Do a raycast
		raycast("TestRay", CamPosX, CamPosY, CamPosZ, CamRotX, CamRotY, CamRotZ, 500.0)
	end

end


function CollisionCallback(GeomName)

	-- Print some info to console
	--consoleprint("Collision with: "..GeomName)

end


function RaycastCallback(RayName, GeomName, Distance, NormX, NormY, NormZ)

	-- Print some info to console
	consoleprint("Ray '"..RayName.."' hit geometry '"..GeomName.."' at ray point "..Distance.." with normal X: "..NormX.." Y: "..NormY.." Z: "..NormZ)

	if RayName == "TestRay"
	then
			impulse(GeomName, -NormX * 100, -NormY * 100, -NormZ * 100, CamPosX, CamPosY, CamPosZ)
	end

end
