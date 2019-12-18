-- SYMMETRY ENGINE
-- Hover vehicle

-- TWEAKABLES ------------------------------------------------------

-- Hovercraft mesh file
Mesh = "ketset.sym.mesh"

-- Speed
Speed = 80.0

-- Steer speed
SteerAmount = 80.0

-- Hovercraft mass
Mass = 50.0

-- Hover height
HoverHeight = 5.0

-- Hover damping
Damping = 0.1

-- Craft size
Size = SYM_VECTOR3(20, 10, 40)

-- Distance from craft to camera
CameraDistance = 50.0
---------------------------------------------------------------------

-- GLOBALS

-- PI / 180
Pi_Over180 = 0.0174532925

-- Hover thruster placement
Thrusters = { 	SYM_VECTOR3( Size.x * 0.6, Size.y * 0.1,  Size.z * 0.6),
				SYM_VECTOR3(-Size.x * 0.6, Size.y * 0.1,  Size.z * 0.6),
				SYM_VECTOR3( Size.x * 0.6, Size.y * 0.1,  0.0),
				SYM_VECTOR3(-Size.x * 0.6, Size.y * 0.1,  0.0),
				SYM_VECTOR3( Size.x * 0.6, Size.y * 0.1, -Size.z * 0.6),
				SYM_VECTOR3(-Size.x * 0.6, Size.y * 0.1, -Size.z * 0.6)}

Gravity = 18.16
GravityForce = SYM_VECTOR3(0, -Mass * Gravity, 0)
ThrusterForce = (Mass * Gravity / 6) / HoverHeight
MaxHoverHeight = 1.25 * HoverHeight + HoverHeight
HoverDistance = 0.0
Steering = 0.0

Loading = 1

function OnLoad(ScriptName)

	-- TODO: Generate geometry name for multiple vehicles

	-- Load loading screen
	loadsprite("Loading", "hcraft_loading.png")

	-- Get screen resolution
	ScreenX, ScreenY = getscreenresolution()

	-- Get sprite size
	SizeX, SizeY = getspritesize("Loading")

	-- Set sprite position
	setspritepos("Loading", (ScreenX / 2) - (SizeX / 2), (ScreenY / 2) - (SizeY / 2), 0)

	-- Run this script
	runscript(ScriptName)

	-- Force a render
	forcerender()

	-- Load scene file
	loadscene("hoverplayground.sym.scene")

	-- Load skybox
	loadskyboxtextures("skyboxes//reef//reef_north.png", "skyboxes//reef//reef_south.png", "skyboxes//reef//reef_east.png", "skyboxes//reef//reef_west.png", "skyboxes//reef//reef_up.png", "skyboxes//reef//reef_down.png")

	-- Turn sky on
	drawsky(true)

	-- Load hovercraft mesh
	loadmodel("HCraft", Mesh, 0)

	-- Set position
	--setposition("HCraft", SYM_VECTOR3(0, 5, 0))

	-- Set mass
	setmass("HCraft", Mass)

	-- Turn off auto freeze, so the physics engine doesn't put the object to 'sleep' if it stops moving
	setautofreeze("HCraft", 0)

	-- Process scene
	processscene()

	-- Load font
	loadfont("arial12", "arial.ttf", 12)

	-- Set font colour
	settextcolour("arial12", 0, 255, 0)

	-- Stop default input script
	stopscript("_input")

	-- Run physics
	runphysics()

	-- Finished loading
	Loading = 0

	drawdebuginfo(true)

end

function OnRenderOverlay()

	-- If loading, render loading screen
	if Loading == 1
	then
		rendersprite("Loading")
	end

end

function OnApplyForceAndTorque(GeomName)

	if GeomName == "HCraft"
	then
				
		-- Get matrix
		Matrix = getmatrix("HCraft")
	
		-- Get hovercraft omega
		Omega = getomega("HCraft")

		-- Get force direction in global space
		ThrusterDir = Matrix:UnrotateVector(SYM_VECTOR3(0, 1, 0))

		TotalForce = GravityForce
		TotalTorque = SYM_VECTOR3(0, 0, 0)
	
		-- Loop through thrusters
		for i = 1,6
		do
			-- Get thruster position in global space
			--io.write(Thrusters[i].x.."  "..Thrusters[i].y.."  "..Thrusters[i].z.."\n")
			ThrusterPos = Matrix:TransformVector(Thrusters[i])
	
			-- Get thruster ineffective point
			ThrusterDeadPos = ThrusterPos - (ThrusterDir * MaxHoverHeight)
	
			-- Raycast from ThrusterPos to ThrusterDeadPos to get distance from ground (if any)
			HoverDistance = 0.0
			raycastto("ThrusterRay", ThrusterPos, ThrusterDeadPos)

			-- Get thruster effective distance
			if HoverDistance == 0
			then
				Distance = 0.0
			else
				Distance = MaxHoverHeight * (1.0 - HoverDistance)
			end

			-- Get magnitude of thruster force
			ForceMag = ThrusterForce * Distance

			-- Get thruster relative position
			ThrusterRelativePos = Matrix:RotateVector(Thrusters[i])

			-- Get thruster velocity
			ThrusterVelocity = Omega:Cross(ThrusterRelativePos)

			-- Get thruster damp force
			ThrusterDampForce = -ThrusterVelocity:Dot(ThrusterDir) * Mass * Damping

			-- Calculate force
			ThisForce = ThrusterDir * (ForceMag + ThrusterDampForce)

			-- Calculate torque
			ThisTorque = ThrusterRelativePos:Cross(ThisForce)

			TotalForce = TotalForce + ThisForce
			TotalTorque = TotalTorque + ThisTorque
		end

		-- Get forward vector
		Forward = SYM_VECTOR3(-Matrix:get(2, 0), -Matrix:get(2, 1), -Matrix:get(2, 2))

		-- Scale by current speed, add to total force
		TotalForce = TotalForce + (Forward * CurrentSpeed * Mass)

		-- Add steering torque
		TotalTorque = TotalTorque + SYM_VECTOR3(0, Steering * Mass, 0)

		-- Set hovercraft force and torque
		setforce("HCraft", TotalForce)
		settorque("HCraft", TotalTorque)

		-- Cleanup
		Matrix:delete()
		Omega:delete()

	else
		setforce(GeomName, GravityForce)
	end

end

function OnRayHit(RayName, GeomName, Distance, NormX, NormY, NormZ)

	if RayName == "ThrusterRay"
	then
		HoverDistance = Distance
	end

end

function OnTick(DeltaTime)

	-- Check for escape key, quit
	if iskeypressedascii(27) == 1 then shutdown() end

	-- Movement keys
	if iskeydown("w") == 1 then CurrentSpeed = Speed
	elseif iskeydown("s") == 1 then CurrentSpeed = -Speed
	else CurrentSpeed = 0.0 end

	-- Steering keys
	if iskeydown("a") == 1 then Steering = SteerAmount
	elseif iskeydown("d") == 1 then Steering = -SteerAmount
	else Steering = 0.0 end

	-- Get mouse relative position
	MouseX, MouseY = getmouserelative()

	-- Get mouse resolution
	ResX, ResY = getmouseresolution()

	-- Get craft matrix
	Matrix = getmatrix("HCraft")
	
	-- Position camera at craft's position
	setcamerapos(Matrix:TransformVector(SYM_VECTOR3(0, 0, 0)))

	-- Pitch camera using (inverted) mouse Y
	pitchcamera(-MouseY * ResY)
	yawcamera(-MouseX * ResX)

	-- Move camera back to distance
	movecamera(-CameraDistance)

	-- Cleanup
	Matrix:delete()

end
