--HUDTEST.LUA

--SYMMETRY ENGINE

--Test script for implementing a HUD

function OnLoad()
	loadfont("arial24", "arial.ttf", 24)
	settextcolour("arial24", 0, 255, 0)
end

function RenderOverlay()
	rendertext("arial24", "Health: ", 40, 40)
end
