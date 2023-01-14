getContents = function(races, scenarioSize)
    local zone = { id = 0, type = Zone.PlayerStart, race = races[1], size = 25 }
    local scenarioZones = { zone }
    local zoneConnections = {}
    
    local contents = { zones = scenarioZones, connections = zoneConnections }
    return contents
end,