MyArray = { CVector3(10, 0, 10) }

function OnLoad(ScriptName)

	loadmodel("Craft", "ketset.sym.mesh")
	addplane("Floor", 100, 100, 0, 0, 0)
	setmass("Craft", 5.0)
	setposition("Craft", CVector3(0, 15, 0))
	processscene()
	runscript(ScriptName)
	runphysics()
	
end

function oOnCollision(GeomName1, GeomName2)

	io.write(MyArray[1].x.."  "..MyArray[1].y.."  "..MyArray[1].z.."\n")

end

function OnApplyForceAndTorque(GeomName)

	io.write(MyArray[1].x.."  "..MyArray[1].y.."  "..MyArray[1].z.."\n")
	
end

function oOnRenderOverlay()

	io.write(MyArray[1].x.."  "..MyArray[1].y.."  "..MyArray[1].z.."\n")
	
end

function oOnTick(DeltaTime)

	io.write(MyArray[1].x.."  "..MyArray[1].y.."  "..MyArray[1].z.."\n")

end