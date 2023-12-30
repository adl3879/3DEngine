-- load default values for game
function love.load()
	print("zelda game starting.......")
	window_width = love.graphics.getWidth()
	window_height = love.graphics.getHeight()

	-- windfield
	local wf = require "lib.windfield"
	world = wf.newWorld()
	world:addCollisionClass("attackable")
	
	-- ui
	local UI = require "ui"
	ui = UI:new()

	GameObjectInstance = require "object"
	require "player"
	Level = require "level"
	Camera = require "lib.camera"
	cam = Camera()
	sti = require "lib.sti"

	-- particle system
	ParticleEffectInstance = require "particle"
	
	-- collider
	require "physics.collider"

	Level:new()
end

-- update fn is called at every frame
function love.update(dt)
	GameObjectInstance:update(dt)
	Level:update(dt)
	world:update(dt)

	-- camera
	if cam.x < window_width / 2 then cam.x = window_width / 2 end
	if cam.y < window_height / 2 then cam.y = window_height / 2 end

	ParticleEffectInstance:update(dt)
end

function love.draw()
	cam:attach()
		Level:render()
		GameObjectInstance:draw()
		ParticleEffectInstance:draw()
		-- world:draw()
		-- for i, collider in pairs(colliders) do collider:draw() end
	cam:detach()

	ui:render()
end

-- center
function love.graphics.center(sprite, rect)
	local px = rect.x + (rect.w / 2 - sprite:getWidth() / 2)
	local py = rect.y + (rect.h / 2 - sprite:getHeight() / 2)
	
	return px, py
end

function love.keypressed(key)
	if key == "escape" then love.event.quit() end
end

-- wave value
function wave_value()
	local value = math.sin(love.timer.getTime() * 30 )
	if value > 0 then return 1 else return 0 end
end

function import_folder(path)
	local sprites = {}
	local fs = love.filesystem
	local files_table = fs.getDirectoryItems(path)
	
	for i, v in ipairs(files_table) do
		local file = path.."/"..v
		if fs.getInfo(file).type == "file" then
			local image = love.graphics.newImage(file)
			table.insert(sprites, image)
		end
	end

	return sprites
end


