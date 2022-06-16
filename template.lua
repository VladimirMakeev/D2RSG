-- Disciples 2 random map template

template = {
	name = "Test template",
	minSize = 48,
	maxSize = 72,
	
	-- Races available on this template
	races = {Race.Human, Race.Undead},

	zones = {
		{
			-- Empire starting zone
			name = "Empire start",
			type = Zone.PlayerStart,
			size = 15,
			monsters = Monsters.Normal,

			terrains = {Terrain.Neutral, Terrain.Human},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {Resource.Gold, Resource.LifeMana},
		},
		{
			-- Undead Hordes starting zone
			name = "Undead start",
			type = Zone.PlayerStart,
			size = 15,
			monsters = Monsters.Normal,

			terrains = {Terrain.Neutral, Terrain.Undead},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {Resource.Gold, Resource.DeathMana},
		},
		{
			-- Central zone
			name = "Junction",
			type = Zone.PlayerStart,
			size = 18,
			monsters = Monsters.Strong,

			terrains = {Terrain.Neutral, Terrain.Undead},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {Resource.Gold, Resource.InfernalMana, Resource.RunicMana, Resource.GroveMana},
		},
	},

	connections = {
		-- We should get 'Empire start' <-> 'Junction' <-> 'Undead start'
		{a = "Empire start", b = "Junction", guard = 1000},
		{a = "Undead start", b = "Junction", guard = 1000},
	},
}

