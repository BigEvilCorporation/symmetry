-- SYMMETRY ENGINE
-- Hover vehicle

-- TWEAKABLES ------------------------------------------------------

-- Hovercraft mesh file
Mesh = "gunship.sym.mesh"

-- Acceleration
Acceleration = 0.01

-- Top speed
TopSpeed = 50.0

-- Hovercraft mass
Mass = 2.0

-- Hover height
HoverHeight = 10.0

-- Hover damping
Damping = 0.25

-- Hover thruster placement
Thrusters = { 	SYM_VECTOR3:new(-10,  0,  10),	-- Front Left
		SYM_VECTOR3:new( 10,  0,  10),	-- Front Right
		SYM_VECTOR3:new(-10,  0,   0),	-- Mid Left
		SYM_VECTOR3:new( 10,  0,   0),	-- Mid Right
		SYM_VECTOR3:new(-10,  0, -10),	-- Rear Left
		SYM_VECTOR3:new( 10,  0, -10)}	-- Rear Right

---------------------------------------------------------------------

-- GLOBALS
Gravity = 9.8
ThrusterForce = (Mass * Gravity / table.getn(Thrusters)) / HoverHeight
MaxHoverHeight = 1.25 * HoverHeight + HoverHeight
HoverDistance = 0.0
CurrentSpeed = 0.0
Distances = {}

function OnLoad(ScriptName)

	-- TODO: Generate geometry name for multiple vehicles

	-- Crate floor
	addplane("Floor", 500, 500, 0, 0, 0)

	-- Set material
	setmaterial("Floor", "floor.sym.material")

	-- Load hovercraft mesh
	loadmodel("HCraft", Mesh)

	-- Set position
	setposition("HCraft", SYM_VECTOR3:new(0, 5, 0))

	-- Set mass
	setmass("HCraft", Mass)

	-- Process scene
	processscene()

	-- Load font
	loadfont("arial12", "arial.ttf", 12)

	-- Set font colour
	settextcolour("arial12", 0, 255, 0)

	-- Run this script by default
	runscript(ScriptName)

	-- Run physics
	--runphysics()

end

function oOnRenderOverlay()

	-- Get matrix
	Matrix = getmatrix("HCraft")

	-- Get force direction in global space
	ThrusterDir = Matrix:UnrotateVector(SYM_VECTOR3:new(0, 1, 0))

	-- Loop through thrusters
	for i = 1,table.getn(Thrusters)
	do
		-- Get thruster position in global space
		ThrusterPos = Matrix:TransformVector(Thrusters[i])

		-- Get thruster ineffective point
		ThrusterDeadPos = ThrusterPos - (ThrusterDir * MaxHoverHeight)

		-- Raycast from ThrusterPos to ThrusterDeadPos to get distance from ground (if any)
		raycastto("ThrusterRay", ThrusterPos, ThrusterDeadPos)

		-- Get thruster effective distance
		Distance = MaxHoverHeight * (1.0 - HoverDistance)

		rendertext("arial12", "Thruster "..i.." X: "..ThrusterPos.x.." Y: "..ThrusterPos.y.." Z: "..ThrusterPos.z.." Distance: "..Distance, 10, 20 * i)
	end
end

function OnApplyForceAndTorque(GeomName)

	if GeomName == "HCraft"
	then
		-- Get matrix
		Matrix = getmatrix("HCraft")
	
		-- Get force direction in global space
		ThrusterDir = Matrix:RotateVector(SYM_VECTOR3:new(0, 1, 0))
	
		-- Get hovercraft velocity and omega
		Velocity = getvelocity("HCraft")
		Omega = getomega("HCraft")
	
		-- Loop through thrusters
		for i = 1,table.getn(Thrusters)
		do
			-- Get thruster position in global space
			ThrusterPos = Matrix:TransformVector(Thrusters[i])
	
			-- Get thruster ineffective point
			ThrusterDeadPos = ThrusterPos - (ThrusterDir * MaxHoverHeight)
	
			-- Raycast from ThrusterPos to ThrusterDeadPos to get distance from ground (if any)
			HoverDistance = 0.0
			raycastto("ThrusterRay"..i, ThrusterPos, ThrusterDeadPos)

			-- Get thruster effective distance
			Distance = HoverDistance --MaxHoverHeight * (1.0 - HoverDistance)

			-- Get magnitude of thruster force
			Force = 40 * Distance
	
			-- Get thruster relative position
			ThrusterRelativePos = Matrix:RotateVector(Thrusters[i])
	
			-- Get thruster velocity
			ThrusterVelocity = (Velocity + Omega):Cross(ThrusterRelativePos)
	
			-- Get thruster damp force
			ThrusterDampForce = -(ThrusterVelocity:Dot(ThrusterDir)) * Mass * Damping
	
			-- Calculate force
			TotalForce = SYM_VECTOR3:new(0, -Mass * Gravity, 0) + ThrusterDir * (Force + ThrusterDampForce)
			
			-- Calculate torque
			TotalTorque = ThrusterRelativePos:Cross(TotalForce)
		end

		-- Set hovercraft force and torque
		setforce("HCraft", TotalForce)
		settorque("HCraft", TotalTorque)

	end

end

function OnRayHit(RayName, GeomName, Distance, NormX, NormY, NormZ)

	--if RayName == "ThrusterRay"
	--then
		io.write("Ray hit "..RayName.." distance "..Distance.."\n")
		HoverDistance = Distance
	--end

end

function OnTick(DeltaTime)

	-- Movement keys
	if iskeydown("t") == 1 then CurrentSpeed = CurrentSpeed + (Acceleration * DeltaTime) end
	if iskeydown("g") == 1 then CurrentSpeed = CurrentSpeed - (Acceleration * DeltaTime) end

	if iskeypressed("i") == 1 then impulse("HCraft", SYM_VECTOR3:new(0, 10, 0), getposition("HCraft")) end

	-- Regulate speed
	if CurrentSpeed > TopSpeed then CurrentSpeed = TopSpeed end
	if CurrentSpeed < -TopSpeed/4 then CurrentSpeed = -TopSpeed/4 end

	-- Set speed
	setvelocity("HCraft", SYM_VECTOR3:new(0, 0, CurrentSpeed))

	--SDLK_UP	= 273
	--SDLK_DOWN	= 274
	--SDLK_RIGHT	= 275
	--SDLK_LEFT	= 276

end

function OnPostRender()

	-- Get matrix
	Matrix = getmatrix("HCraft")

	-- Get force direction in global space
	ThrusterDir = Matrix:UnrotateVector(SYM_VECTOR3:new(0, 1, 0))

	gl_colour3f(SYM_VECTOR3:new(1.0, 0.0, 0.0))
	gl_beginlines()

	-- Loop through thrusters
	for i = 1,table.getn(Thrusters)
	do
		-- Get thruster position in global space
		ThrusterPos = Matrix:TransformVector(Thrusters[i])

		-- Get thruster ineffective point
		ThrusterDeadPos = ThrusterPos - (ThrusterDir * MaxHoverHeight)

		
		gl_vertex3f(ThrusterPos)
		gl_vertex3f(ThrusterDeadPos)
		
	end

	gl_end()
	gl_colour3f(SYM_VECTOR3:new(1.0, 1.0, 1.0))

end
