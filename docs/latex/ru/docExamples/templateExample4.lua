function getTowns(scenarioSize)
    local zoneTowns = { }
    
    if scenarioSize == 48 then
        zoneTowns[1] = { tier = 2 }
    elseif scenarioSize == 72 then
        zoneTowns[1] = { tier = 3 }
        zoneTowns[2] = { tier = 5 }
    end
    
    return zoneTowns
end

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
                    
                    mines = getMines(races[1]),
                    towns = getTowns(scenarioSize)
                },
                {
                    id = 1,
                    type = Zone.PlayerStart,
                    race = races[2],
                    size = scenarioSize / 2,
                    
                    mines = getMines(races[2]),
                    towns = getTowns(scenarioSize)
                }
            },
            
            connections = {
                { from = 0, to = 1 }
            }
        }
    end,
}