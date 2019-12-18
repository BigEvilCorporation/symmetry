-- SYMMETRY.LUA

-- Startup script for Symmetry Engine

function OnLoad(ScriptName)
	
	consoleprint("Symmetry Engine **DEVELOPMENT** v0.4")
	consoleprint("Copyright 2008 Matt Phillips - http://www.hysteriagaming.com")
	
	-- Load font
	--loadfont("arial24", "arial.ttf", 24)

	-- Load default input script
	--loadscript("_input", "default_input.lua")
	
	-- Load console background texture
	loadconsoletexture("console//console01.png")
	
	-- Setup environment
	--drawgrid(true)
	--drawcollision(true)
	
	-- Start intro video
	--loadvideo("robotica2.mp4")

end

function OnRun()

	-- Start playing intro video
	--playvideo()
	
end
	
