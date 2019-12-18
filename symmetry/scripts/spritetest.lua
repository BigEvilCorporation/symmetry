-- SYMMETRY ENGINE
-- Mouse cursor test

function OnLoad(ScriptName)

	-- Load sprite
	loadsprite("MouseCursor", "mousecursor.png")

	-- Set sprite transparency (0 - 1)
	setspritetransparency("MouseCursor", 0.5)

end


function RenderOverlay()

	-- Get mouse absolute coords
	MouseX, MouseY = getmouseabsolute()

	-- Get screen resolution
	ScreenX, ScreenY = getscreenresolution()

	-- If coords are outside the screen resolution, warp pointer
	if MouseX > ScreenX then warpmouse(ScreenX, MouseY) end
	if MouseY > ScreenY then warpmouse(MouseX, ScreenY) end

	-- Set sprite position (inverted Y)
	setspritepos("MouseCursor", MouseX, ScreenY - MouseY, 0)

	-- Render sprite
	rendersprite("MouseCursor")

end
