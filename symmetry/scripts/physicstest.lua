-- SYMMETRY ENGINE
-- PHYSICS TEST

function OnLoad()

	--Create the floor
	addplane("floor", 100, 100, -50, 0, -50)
	
	--Load floor material
	setmaterial("floor", "floor.sym.material")
	
	--Block number
	Count = 0

	--Loop through rows
	for i = 0,3
	do
		--Loop through columns
		for j = 0,3
		do
		
			--Load a crate
			loadmodel("crate"..Count, "crate.sym.mesh")
			
			--Move the crate
			setposition("crate"..Count, i * 10, j * 10, 0)
			
			--Set the crate's mass
			setmass("crate"..Count, 2.0)
			
			--Increment crate number
			Count = Count + 1
			
		end
	end
	
	--Load the gunship
	loadmodel("gunship", "gunship.sym.mesh")
	
	--Move the gunship behind the tower
	setposition("gunship", 15, 25, -50)
	
	--Rotate the gunship for a better impact
	setrotation("gunship", 0, 45, 0)
	
	--Set the gunship's mass
	setmass("gunship", 5.0)
	
	--Set the gunship's initial velocity
	setvelocity("gunship", 0, 0, 80)

	--Build the octree and set physics world size
	processscene()

end