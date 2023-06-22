OnCreate = function()
    print "creating......"
end

OnUpdate = function(dt)
    if Input.IsKeyPressed("B") then
        print "B pressed"
        printMessage("Hello from Lua")
    end
end

OnKeyPressed = function(key)
    if key == "A" then
        print "A pressed"
    end
end

OnMouseButtonPressed = function(button)
    if button == "Left" then
        print "Left button pressed"
    end
end

OnMouseMoved = function(x, y, xOffset, yOffset)
    print("Mouse moved", x, y)
end
