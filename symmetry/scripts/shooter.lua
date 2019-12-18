-- SYMMETRY ENGINE
-- Mini shooter game - engine test script

-- Variables
PrevLMB = 0
Loading = 1

function OnLoad(ScriptName)

	-- Load loading screen
	loadsprite("loading", "loading_fps.png")

	-- Get screen resolution
	ScreenX, ScreenY = getscreenresolution()

	-- Get sprite size
	SizeX, SizeY = getspritesize("loading")

	-- Set sprite position
	setspritepos("loading", (ScreenX / 2) - (SizeX / 2), (ScreenY / 2) - (SizeY / 2), 0)

	-- Force a render
	forcerender()

	-- Load scene
	loadscene("shooter.sym.scene")

	-- Load crosshair
	loadsprite("crosshair", "crosshair.png")

	-- Get crosshair size
	SizeX, SizeY = getspritesize("crosshair")

	-- Set crosshair position
	setspritepos("crosshair", (ScreenX / 2) - (SizeX / 2), (ScreenY / 2) - (SizeY / 2), 0)

	-- Set crosshair transparency (0 - 1)
	setspritetransparency("crosshair", 0.3)

	-- Load font
	loadfont("arial24", "arial.ttf", 24)

	-- Set font colour
	settextcolour("arial24", 0, 255, 0)

	-- Turn on debug info
	drawdebuginfo(true)

	-- No longer loading
	Loading = 0

	-- Run this script immediately
	runscript(ScriptName)

end


function OnRun()

	-- Run physics
	runphysics()

end


function Tick(DeltaTime)

	-- Check if left mouse button is down
	if islmbdown() == 1
	then 
		-- If mouse was not pressed at last Tick
		if PrevLMB == 0
		then
			-- Mark LMB as previously down
			PrevLMB = 1

			-- Get camera position
			CamPos = getcamerapos()
	
			-- Get camera forward vector
			CamDir = getcameradir()
	
			-- Do a raycast
			raycast("GunRay", CamPos, CamDir, 500.0)
		end

	else

		-- Mark LMB as previously up
		PrevLMB = 0

	end

end

function RenderOverlay()

	-- If loading, render loading screen
	if Loading == 1
	then

		rendersprite("Loading")

	else

		-- Render crosshair
		rendersprite("crosshair")
	
		-- Render text
		CamPos = getcamerapos()
		rendertext("arial24", "CamPos: X: "..CamPos.x.." Y: "..CamPos.y.." Z: "..CamPos.z, 40, 40)

	end

end


function CollisionCallback(GeomName)

end


function RaycastCallback(RayName, GeomName, Distance, NormX, NormY, NormZ)

	if RayName == "GunRay"
	then
		CamPos = getcamerapos()
		impulse(GeomName, SYM_VECTOR3:new(-NormX * 100, -NormY * 100, -NormZ * 100), CamPos)
	end

end
