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
                    size = scenarioSize / 2
                },
                {
                    id = 1,
                    type = Zone.PlayerStart,
                    race = races[2],
                    size = scenarioSize / 2
                }
            },
            
            connections = {
                { from = 0, to = 1 }
            }
        }
    end,
}