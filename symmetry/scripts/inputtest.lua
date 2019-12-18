--SYMMETRY ENGINE
--Scripted input test

function OnLoad(ScriptName)

end


function Tick(DeltaTime)

	if iskeydown("w") == 1
	then movecamera(DeltaTime * 0.1)
	end

end