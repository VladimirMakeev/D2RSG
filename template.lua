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
			id = 0,
			type = Zone.PlayerStart,
			size = 15,
			monsters = Monsters.Normal,

			terrains = {Terrain.Neutral, Terrain.Human},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {
				gold = 1,
				lifeMana = 1,
			},
		},
		{
			-- Undead Hordes starting zone
			id = 1,
			type = Zone.PlayerStart,
			size = 15,
			monsters = Monsters.Normal,

			terrains = {Terrain.Neutral, Terrain.Undead},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {
				gold = 1,
				deathMana = 1,
			},
		},
		{
			-- Central zone
			id = 2,
			type = Zone.Junction,
			size = 18,
			monsters = Monsters.Strong,

			terrains = {Terrain.Neutral, Terrain.Undead},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {
				gold = 1,
				infernalMana = 1,
				runicMana = 1,
				groveMana = 1,
			},
		},
	},

	connections = {
		-- We should get 'Empire start' <-> 'Central' <-> 'Undead start'
		{a = 0, b = 2, guard = 1000},
		{a = 1, b = 2, guard = 1000},
	},
}

