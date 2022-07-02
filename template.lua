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

			neutralTowns = {
				tier1 = 0
			},

			merchants = {
				{
					-- Types of items allowed for this merchant, optional
					itemTypes = {Item.PotionHeal, Item.PotionBoost},
					-- Total value of merchant items, excluding items that must be generated, optional 
					cash = { min = 200, max = 1000},
					-- Items that must be generated, optional
					items = {
						{
							id = 'g000ig0006',
							min = 1,
							max = 5
						},
					},
				},
			},

			ruins = {
				{ cash = {min = 50, max = 500}},
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

			neutralTowns = { tier1 = 0 },
			ruins = {
				{ cash = {min = 50, max = 500}},
			},

			merchants = {
				{
					-- Types of items allowed for this merchant, optional
					itemTypes = {Item.PotionHeal, Item.PotionBoost},
					-- Total value of merchant items, excluding items that must be generated, optional 
					cash = { min = 200, max = 1000},
					-- Items that must be generated, optional
					items = {
						{
							id = 'g000ig0006',
							min = 1,
							max = 5
						},
					},
				},
			},
		},
		{
			-- Central zone
			id = 2,
			type = Zone.Treasure,
			size = 38,
			monsters = Monsters.Strong,

			terrains = {Terrain.Neutral, Terrain.Undead},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {
				gold = 3,
				infernalMana = 2,
				runicMana = 2,
				groveMana = 1,
			},

			neutralTowns = {
				tier3 = 2,
				tier4 = 1
			},

			-- Merchants
			-- Merchant content (tradable items) depends on cash and itemTypes.
			-- Cash is spread equally between all types specified.
			-- You can also explicitly set items that will be awailable,
			-- regardless of cash and itemTypes 
			merchants = {
				{
					-- Types of items allowed for this merchant, optional
					itemTypes = {Item.PotionHeal, Item.PotionBoost},
					-- Total value of merchant items, excluding items that must be generated, optional 
					cash = { min = 200, max = 1000},
					-- Items that must be generated, optional
					items = {
						{
							id = 'g000ig0004',
							min = 10,
							max = 50
						},
					},
				},
			},

			-- Mages
			-- Mercenaries
			-- Trainers

			-- List of ruins in the zone
			ruins = {
				{
					-- Reward in gold, optional
					cash = { min = 200, max = 1000},
					-- Reward item value, optional
					item = { min = 100, max = 500},
					-- You can specity item id that must be generated, optional
					-- In this case, 'item' field is ignored
					--itemId = 'item id',
				},
				{
					cash = { min = 10, max = 100},
				},
				{
					cash = { min = 100, max = 2000},
				},
				{
					cash = { min = 500, max = 1000},
				},
				{
					cash = { min = 100, max = 300},
				},
				{
					-- Test retarded cases
					cash = { min = 500, max = 200},
				},
			},
		},
		{
			-- For testing
			id = 3,
			type = Zone.Treasure,
			size = 20,

			monsters = Monsters.Strong,

			terrains = {Terrain.Neutral, Terrain.Undead},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {
				gold = 2,
				infernalMana = 0,
				runicMana = 1,
				groveMana = 0,
			},

			neutralTowns = {
				tier1 = 1,
				tier2 = 1
			},
		},
		{
			-- For testing
			id = 4,
			type = Zone.Treasure,
			size = 20,

			monsters = Monsters.Strong,

			terrains = {Terrain.Neutral, Terrain.Undead},
			grounds = {Ground.Plain, Ground.Forest},
			mines = {
				gold = 2,
				infernalMana = 1,
				runicMana = 0,
				groveMana = 0,
			},

			neutralTowns = {
				tier1 = 1,
				tier2 = 1
			},
		},
	},

	connections = {
		{a = 0, b = 3, guard = 1000},
		{a = 1, b = 4, guard = 1000},
		{a = 2, b = 3, guard = 10},
		{a = 2, b = 4, guard = 20},
	},
}

