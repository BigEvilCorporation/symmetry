-- SYMMETRY ENGINE
-- PHYSICS TEST 2

function OnLoad()

	--Add brushes
	addplane("plane1", 60, 60, 30, 30, -30)
	addplane("plane2", 60, 60, -30, 90, -30)
	addplane("plane3", 60, 60, 30, 120, -30)
	addplane("plane4", 60, 60, -30, 180, -30)
	
	--Set brush materials
	setmaterial("plane1", "panel.sym.material")
	setmaterial("plane2", "panel.sym.material")
	setmaterial("plane3", "panel.sym.material")
	setmaterial("plane4", "panel.sym.material")
	
	--Rotate brushes
	setrotation("plane1", 0, 0, 45)
	setrotation("plane2", 0, 0, -45)
	setrotation("plane3", 0, 0, 45)
	setrotation("plane4", 0, 0, -45)
	
	--Add floor
	addplane("floor", 100, 100, 0, 0, 0)
	
	--Load floor material
	setmaterial("floor", "floor.sym.material")
	
	--Load crate
	loadmodel("crate", "crate.sym.mesh")
	
	--Set crate mass
	setmass("crate", 2.0)
	
	--Position crate
	setposition("crate", 0, 200, 0)

	--Build octree, set physics world size
	processscene()
	
end