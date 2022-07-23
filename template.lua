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

			stacks = {
				-- Total stacks value
				value = { min = 1500, max = 2500 },
				-- Stacks count
				count = 10,
			},

			merchants = {
				{
					-- Types of items allowed for this merchant, optional
					itemTypes = {Item.PotionHeal, Item.PotionBoost},
					-- Total value of merchant items, excluding items that must be generated, optional 
					cash = { min = 2000, max = 4000},
					-- Items that must be generated, optional
					items = {
						{
							id = 'g000ig0006',
							min = 1,
							max = 5
						},
						{id = 'g000ig0001', min = 3, max = 3},
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

			stacks = {
				-- Total stacks value
				value = { min = 1500, max = 2500 },
				-- Stacks count
				count = 10,
			},

			merchants = {
				{
					-- Types of items allowed for this merchant, optional
					itemTypes = {Item.Talisman, Item.Orb, Item.PotionBoost},
					-- Total value of merchant items, excluding items that must be generated, optional 
					cash = { min = 3200, max = 6000},
					-- Items that must be generated, optional
					items = {
						{
							id = 'g000ig0006',
							min = 1,
							max = 5
						},
						{id = 'g000ig0001', min = 3, max = 3},
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

			-- Neutral stacks that does not guard anything
			-- For xp only
			stacks = {
				-- Total stacks value
				value = { min = 3500, max = 7500 },
				-- Stacks count
				count = 15,
			},

			-- Bags with treasures, potions, spells or other items
			-- Loosely guarded
			bags = {
				-- Total value of items in bags
				value = { min = 20000, max = 30000 },
				-- Bags count
				count = 20,
			},

			-- Merchants
			-- Merchant content (tradable items) depends on cash and itemTypes.
			-- Cash is spread equally between all types specified.
			-- You can also explicitly set items that will be awailable,
			-- regardless of cash and itemTypes 
			merchants = {
				{
					-- Types of items allowed for this merchant, optional
					itemTypes = {Item.TravelItem, Item.Weapon, Item.Armor, Item.Wand, Item.Scroll},
					-- Total value of merchant items, excluding items that must be generated, optional 
					cash = { min = 20000, max = 50000 },
					-- Items that must be generated, optional
					items = {
						{
							id = 'g000ig0004',
							min = 1,
							max = 2
						},
					},
				},
			},

			-- Mages
			mages = {
				{
					-- Types of spells allowed for this mage, optional
					spellTypes = {Spell.Heal, Spell.RestoreMove},
					-- Total value of mage spells, excluding spells that must be generated, optional
					cash = { min = 200, max = 1000 },
					-- Spells that must be generated, optional
					spells = { 'g000ss0002', 'g000ss0004' },
				},
				{
					-- Types of spells allowed for this mage, optional
					spellTypes = {Spell.Attack, Spell.Boost, Spell.Lower},
					-- Total value of mage spells, excluding spells that must be generated, optional
					cash = { min = 2000, max = 5000 },
				},
			},

			-- Mercenaries
			mercenaries = {
				{
					-- Subraces of units allowed for hire, optional
					subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
					-- Total value of units, excluding units that must be generated, optional
					cash = { min = 200, max = 1000 },
					-- Units that must be generated, optional
					units = {
						{ id = 'g000uu0007', level = 2, unique = false },
						{ id = 'g000uu0088', level = 20, unique = true },
					},
					
				},
			},

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
					itemId = 'g000ig9119', -- talisman of life
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

			stacks = {
				-- Total stacks value
				value = { min = 2500, max = 3500 },
				-- Stacks count
				count = 12,
			},

			bags = {
				-- Total value of items in bags
				value = { min = 10000, max = 15000 },
				-- Bags count
				count = 10,
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
				tier5 = 1
			},

			stacks = {
				-- Total stacks value
				value = { min = 2500, max = 3500 },
				-- Stacks count
				count = 12,
			},

			bags = {
				-- Total value of items in bags
				value = { min = 10000, max = 15000 },
				-- Bags count
				count = 10,
			},
		},
	},

	connections = {
		{a = 0, b = 3, guard = 550},
		{a = 1, b = 4, guard = 550},
		{a = 2, b = 3, guard = 1000},
		{a = 2, b = 4, guard = 1000},
	},
}

