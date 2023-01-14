function getMines(race)
    local zoneMines = { }
    
    zoneMines.gold = 2
    
	if race == Race.Human then
		zoneMines.lifeMana = 2
	elseif race == Race.Dwarf then
		zoneMines.runicMana = 2
	elseif race == Race.Undead then
		zoneMines.deathMana = 2
	elseif race == Race.Heretic then
		zoneMines.infernalMana = 2
	elseif race == Race.Elf then
		zoneMines.groveMana = 2
	end
    
    return zoneMines
end

template = {
	name = 'Example',
	description = 'Example template description',
	minSize = 48,
	maxSize = 72,
	maxPlayers = 2,
    forest = 65,
    roads = 40,
    startingGold = 850,
	
	getContents = function(races, scenarioSize)
        return {
            zones = {
                {
                    id = 0,
                    type = Zone.PlayerStart,
                    race = races[1],
                    size = scenarioSize / 2,
                    
                    mines = getMines(races[1])
                },
                {
                    id = 1,
                    type = Zone.PlayerStart,
                    race = races[2],
                    size = scenarioSize / 2,
                    
                    mines = getMines(races[2])
                }
            },
            
            connections = {
                { from = 0, to = 1 }
            }
        }
    end,
}