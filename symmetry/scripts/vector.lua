-- SYMMETRY ENGINE
-- Lua class test

function OnLoad(ScriptName)

  io.write("Creating new vector\n")

  Vector1 = SYM_VECTOR3:new(0, 0, 0)
  
  io.write("Vector 1 = x: "..Vector1.x.." y: "..Vector1.y.." z: "..Vector1.z.."\n\n")

  io.write("Creating new vector\n")

  Vector2 = SYM_VECTOR3:new(2, 2, 2)

  io.write("Vector 2 = x: "..Vector2.x.." y: "..Vector2.y.." z: "..Vector2.z.."\n\n")

  io.write("Translating Vector 1 by Vector 2\n")

  Vector1 = Vector1 + Vector2

  io.write("Vector 1 = x: "..Vector1.x.." y: "..Vector1.y.." z: "..Vector1.z.."\n\n")

  io.write("Scaling Vector 1 by Vector 2\n")

  Vector1 = Vector1 * Vector2

  io.write("Vector 1 = x: "..Vector1.x.." y: "..Vector1.y.." z: "..Vector1.z.."\n\n")

  io.write("Subtracting Vector 2 from Vector 1\n")

  Vector1 = Vector1 - Vector2

  io.write("Vector 1 = x: "..Vector1.x.." y: "..Vector1.y.." z: "..Vector1.z.."\n\n")

end