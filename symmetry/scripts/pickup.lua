--PICKUP.LUA

--Test script for implementing a pickup

function OnLoad()
	loadmodel("pickup", "pickup1.s3dm")
	movemodel("pickup", 500, 0, 0)
end

function PreRender()
	rotatemodel("pickup", 0, 0.1, 0)
end

function OnDestroy()
	deletemodel("pickup")
end