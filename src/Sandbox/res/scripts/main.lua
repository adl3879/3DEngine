OnCreate = function()
    entity.setTransform("Rotation", -90, 0, 0)
end

OnUpdate = function(dt)
    local p = entity.getTransform().Translation
    local speed = 2 * dt

    if Input.IsKeyPressed("W") then
        entity.setTransform("Translation", p.x, p.y + speed, p.z)
    elseif Input.IsKeyPressed("S") then
        entity.setTransform("Translation", p.x, p.y - speed, p.z)
    elseif Input.IsKeyPressed("A") then
        entity.setTransform("Translation", p.x - speed, p.y, p.z)
    elseif Input.IsKeyPressed("D") then
        entity.setTransform("Translation", p.x + speed, p.y, p.z)
    elseif Input.IsKeyPressed("Q") then
        entity.setTransform("Translation", p.x, p.y, p.z - speed)
    elseif Input.IsKeyPressed("E") then
        entity.setTransform("Translation", p.x, p.y, p.z + speed)
    end
end

OnKeyPressed = function(key)
end
