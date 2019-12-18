-- SYMMETRY ENGINE
-- TERRAIN TEST SCRIPT

function OnLoad()
	--Load the heightmap
	loadterrainheightmap("terr1.png", 1, 1, 2, 2)
	
	--Set the material
	setterrainmaterial("terrain.sym.material")
	
	--Show the terrain
	drawterrain(true)
end