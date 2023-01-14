template = {
    name = 'Template name',
    description = 'Template description',
    minSize = 48,
    maxSize = 72,
    maxPlayers = 1,
    startingGold = 250,
    roads = 65,
    forest = 45,

    getContents = function(races, scenarioSize)
        return {
            zones = {
                {
                    id = 0,
                    type = Zone.PlayerStart,
                    race = races[1],
                    size = 25,
                }
            },
            
            connections = {},
        }
    end,
}