--SYMMETRY ENGINE
--OCTREE TEST

function OnLoad()

	--Block number
	Count = 0

	--Loop through rows
	for i = 0,10
	do
		--Loop through columns
		for j = 0,10
		do

			--Loop through depth
			for k = 0,2
			do
		
				--Load a crate
				loadmodel("crate"..Count, "crate.sym.mesh")
				
				--Move the crate
				setposition("crate"..Count, i * 50, j * 50, k * 30)
				
				--Increment crate number
				Count = Count + 1

			end
			
		end
	end

	--Build octree, set physics world size
	processscene()

end 
