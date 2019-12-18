--SYMMETRY ENGINE
--TERRAIN COLLISION TEST

function OnLoad()

	--Load skybox textures
	loadskyboxtextures("skyboxes\\reef\\reef_north.png", "skyboxes\\reef\\reef_south.png", "skyboxes\\reef\\reef_east.png", "skyboxes\\reef\\reef_west.png", "skyboxes\\reef\\reef_up.png", "skyboxes\\reef\\reef_down.png")
	
	--Load the heightmap
	loadterrainheightmap("terr1.png")
	
	--Load the texture
	loadterraintexture("terr1texture.png")
	
	--Show the terrain
	drawterrain(true)
	
	--Block number
	Count = 0

	--Loop through rows
	for i = 1,5
	do
		--Loop through columns
		for j = 1,5
		do
		
			--Load a block
			loadmodel("blocklarge"..Count, "block.s3dm")
			
			--Move the block
			movemodel("blocklarge"..Count, 800 + (i * 3.5), 1300, 900 + (j * 3.5))
			
			--Set the block's mass
			setmass("blocklarge"..Count, 0.4)
			
			--Increment block number
			Count = Count + 1
			
		end
	end
	
	--Set camera position
	setcamerapos(695, 800, 695)

end