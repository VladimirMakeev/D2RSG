-- Disciples 2 random map template


--[[

TODO:

- Вынос описания зон в функцию что примет список рас, выбранных игроком (в UI, либо командная строка)

- Глобальные настройки:
  - Настройки запрещенных юнитов
  - Настройки запрещенных предметов
  - Возможность ставить дальников в передний ряд.

- Настройки зон:
  - Зеркальные зоны, с идентичным наполнением (результатами генерации)
  - Открытые/закрытые границы зон

- Типы доступности рудников:
  - Рандомные с охраной/без
  - Точно охраняемые
  - На острове, нельзя захватить просто так (только жезл)

- Генерация:
  - Луа функция-фильтр для юнитов, предметов, заклинаний. Например отсеять юнитов по типу/источнику атаки.

- Генерация лута:
  - Порог минимального и максимального велью одной вещи
  - Запрет дубликатов вещей в одном отряде/сундуке

]]--

local Value = function(minValue, maxValue)
	local v = maxValue or minValue
	return { min = minValue, max = v }
end

local startingStacks = {
	-- Total stacks value, excluding predefined stacks
	value = Value(1200 * 2),
	-- Stacks count
	count = 20,
	-- Stacks loot, split evenly between them, optional
	loot = {
		-- Item types available, optional
		itemTypes = {Item.PotionHeal, Item.PotionBoost},
		-- Total value items
		value = { min = 250, max = 250 },
		-- Items that must be generated, optional
		items = {
			{ id = 'g000ig0006', min = 1, max = 1 },
		},
	},

	-- Stacks that must be generated, optional
	predefined = {
		{
			-- Subraces available, optional
			subraceTypes = {Subrace.NeutralWolf, Subrace.NeutralBarbarian, Subrace.Dwarf, Subrace.Heretic, Subrace.Elf, Subrace.NeutralElf},
			-- Total value of stack units
			value = { min = 200, max = 250 },
			-- Stack items, optional
			-- Description is the same as for stacks.loot
			loot = {
				items = {
					{ id = 'g000ig0001', min = 5, max = 5 },
				}
			},
		},
	},
}

local startingMerchants = {
	{
		itemTypes = {Item.PotionHeal, Item.PotionBoost},
		value = { min = 2000, max = 4000 },
		items = {
			{ id = 'g000ig0006', min = 1, max = 5 },
			{ id = 'g000ig0001', min = 3, max = 3 },
		},

		guard = {
			subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
			value = { min = 150, max = 225 },
			loot = {
				itemTypes = {Item.Talisman, Item.Orb},
				value = { min = 500, max = 600 },
			},
		},
	},
}


template = {
	name = "Test template",
	minSize = 48,
	maxSize = 120,
	-- Races available on this template
	races = {Race.Human, Race.Undead},
	-- Percentage of road tiles covered with actual roads
	roads = 65,
	-- Starting players gold
	startingGold = 500,
	forest = 35,

	zones = {
		{
			-- Empire starting zone
			id = 0,
			type = Zone.PlayerStart,
			size = 15,

			mines = {
				gold = 1,
				lifeMana = 1,
			},

			merchants = startingMerchants,			
			stacks = startingStacks,
		},
		{
			-- Undead Hordes starting zone
			id = 1,
			type = Zone.PlayerStart,
			size = 15,

			mines = {
				gold = 1,
				deathMana = 1,
			},
			
			merchants = startingMerchants,			
			stacks = startingStacks,
		},
		{
			-- Central zone
			id = 2,
			type = Zone.Treasure,
			size = 38,

			mines = {
				gold = 3,
				infernalMana = 2,
				runicMana = 2,
				groveMana = 1,
			},

			towns = {
				{
					tier = 5,
					garrison = {
						subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
						value = { min = 1000, max = 1500 },
						-- Items in garrison, optional
						loot = {
							-- Item types available, optional
							itemTypes = {Item.Weapon, Item.Orb},
							-- Total value of garrison items
							value = { min = 1200, max = 1300 },
							-- Items that must be generated, optional
							items = {
								{ id = 'g000ig0006', min = 1, max = 5},
							},
						},
					},
					stack = {
						subraceTypes = {Subrace.Dwarf, Subrace.NeutralHuman, Subrace.NeutralGreenSkin, Subrace.NeutralBarbarian},
						value = { min = 100, max = 1500 },

						loot = {
							-- Item types available, optional
							itemTypes = {Item.PotionHeal, Item.PotionBoost},
							-- Total value of garrison items
							value = { min = 200, max = 300 },
						},
					},
				},
				{
					tier = 3,
					garrison = {
						subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
						value = { min = 200, max = 300 },
					},
				},
				{
					tier = 2,
					garrison = {
						subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
						value = { min = 100, max = 150 },
					},
				},
				{
					tier = 1,
					garrison = {
						subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
						value = { min = 0, max = 0 },
					},
					stack = {
						subraceTypes = {Subrace.Dwarf},
						value = { min = 50, max = 75 },
					},
				},
			},

			mages = {
				{
					spellTypes = {Spell.Heal, Spell.RestoreMove},
					value = { min = 200, max = 1000 },
					-- Spells that must be generated, optional
					spells = { 'g000ss0002', 'g000ss0004' },

					guard = {
						subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
						value = { min = 250, max = 275 },
					},
				},
				{
					spellTypes = {Spell.Attack, Spell.Boost, Spell.Lower},
					value = { min = 2000, max = 5000 },

					guard = {
						--subraceTypes = {Subrace.Human, Subrace.NeutralHuman, Subrace.NeutralDragon, Subrace.NeutralBarbarian},
						value = { min = 250, max = 875 },
						
						loot = {
							-- Predefined item that must be generated, optional
							-- Only first specified entry is used.
							-- If specified, value and itemTypes are ignored
							items = {
								{ id = 'g000ig0006', min = 1, max = 1},
							},
						}
					},
				},
			},

			mercenaries = {
				{
					-- Subraces of units allowed to hire, optional
					subraceTypes = {Subrace.Human, Subrace.NeutralHuman},
					-- Total value of units, excluding predefined units, optional
					value = { min = 200, max = 250 },
					-- Units that must be generated, optional
					units = {
						{ id = 'g000uu0007', level = 2, unique = false },
						{ id = 'g000uu0088', level = 20, unique = true },
					},
			
					-- Stack guarding the mercenary camp, optional
					guard = {
						-- Subraces available, optional
						subraceTypes = {Subrace.Human, Subrace.NeutralHuman, Subrace.NeutralWolf},
						-- Total value of stack units
						value = { min = 200, max = 250 },
						-- Stack items, optional
						-- Description is the same as for garrison.loot
						loot = {},
					},
				},
				{
					-- Total value of units, excluding predefined units, optional
					value = { min = 400, max = 550 },
			
					-- Stack guarding the mercenary camp, optional
					guard = {
						-- Subraces available, optional
						subraceTypes = {Subrace.Human, Subrace.NeutralHuman, Subrace.Neutral},
						-- Total value of stack units
						value = { min = 600, max = 750 },
						-- Stack items, optional
						-- Description is the same as for garrison.loot
						loot = {},
					},
				},
			},
			
			ruins = {
				{
					-- Reward in gold, optional
					gold = { min = 200, max = 250 },
					-- Reward item, optional
					loot = {
						-- Predefined item that must be generated, optional
						-- Only first specified entry is used.
						-- If specified, value and itemTypes are ignored
						items = {
							{ id = 'g000ig0006', min = 1, max = 1},
						},
					},
			
					-- Group of units inside ruin
					-- Loot is ignored here
					guard = {
						-- Subraces available, optional
						subraceTypes = {Subrace.NeutralElf, Subrace.NeutralWolf, Subrace.NeutralHuman},
						-- Total value of units in ruin
						value = { min = 200, max = 250 },
					},
				},
				{
					-- Reward in gold, optional
					gold = { min = 500, max = 550 },
					-- Reward item, optional
					loot = {
						-- Item types available, optional
						--itemTypes = {Item.Weapon, Item.Talisman},
						-- Value of item
						-- optional if item is specified
						value = { min = 2000, max = 3000 },
					},
			
					-- Group of units inside ruin
					-- Loot is ignored here
					guard = {
						-- Subraces available, optional
						subraceTypes = {Subrace.Heretic, Subrace.NeutralHuman},
						-- Total value of units in ruin
						value = { min = 400, max = 450 },
					},
				},
			},
		},
		{
			-- For testing
			id = 3,
			type = Zone.Treasure,
			size = 20,

			mines = {
				gold = 2,
				infernalMana = 0,
				runicMana = 1,
				groveMana = 0,
			},
			
			stacks = {
				value = Value(8500),
				-- Stacks count
				count = 35,
				-- Stacks loot, split evenly between them, optional
				loot = {
					-- Total value items
					value = Value(2500),
				},
			},
			
			mages = {
				{
					-- Total value of mage spells, excluding spells that must be generated, optional
					value = Value(5000),

					-- Stack guarding the mage, optional
					guard = {
						-- Total value of stack units
						value = Value(1000),
					},
				},
				{
					-- Total value of mage spells, excluding spells that must be generated, optional
					value = Value(5000),

					-- Stack guarding the mage, optional
					guard = {
						-- Total value of stack units
						value = Value(900),
					},
				},
			},
		},
		{
			-- For testing
			id = 4,
			type = Zone.Treasure,
			size = 20,

			mines = {
				gold = 2,
				infernalMana = 1,
				runicMana = 0,
				groveMana = 0,
			},
			
			stacks = {
				value = Value(8500),
				-- Stacks count
				count = 35,
				-- Stacks loot, split evenly between them, optional
				loot = {
					-- Total value items
					value = Value(2500),
				},
			},
			
			mages = {
				{
					-- Total value of mage spells, excluding spells that must be generated, optional
					value = Value(5000),

					-- Stack guarding the mage, optional
					guard = {
						-- Total value of stack units
						value = Value(1000),
					},
				},
				{
					-- Total value of mage spells, excluding spells that must be generated, optional
					value = Value(5000),

					-- Stack guarding the mage, optional
					guard = {
						-- Total value of stack units
						value = Value(900),
					},
				},
			},
		},
	},

	connections = {
		{
			from = 0,
			to = 3,
			guard = {
				-- Subraces available, optional
				subraceTypes = {Subrace.Dwarf, Subrace.Heretic, Subrace.Undead, Subrace.Elf, Subrace.NeutralGreenSkin, Subrace.NeutralBarbarian, Subrace.NeutralDragon},
				-- Total value of units
				value = { min = 550, max = 550 },
				
				loot = {
					-- Item types available, optional
					itemTypes = {Item.Orb, Item.Talisman, Item.PotionBoost, Item.PotionHeal},
					-- Value of item
					-- optional if item is specified
					value = { min = 2000, max = 3000 },
				},
			},
		},
		{
			from = 1,
			to = 4,
			guard = {
				-- Subraces available, optional
				subraceTypes = {Subrace.Dwarf, Subrace.Heretic, Subrace.Human, Subrace.Elf, Subrace.NeutralGreenSkin, Subrace.NeutralBarbarian, Subrace.NeutralDragon},
				-- Total value of units
				value = { min = 550, max = 550 },
				
				loot = {
					-- Item types available, optional
					itemTypes = {Item.Orb, Item.Talisman, Item.PotionBoost, Item.PotionHeal},
					-- Value of item
					-- optional if item is specified
					value = { min = 2000, max = 3000 },
				},
			},
		},
		{
			from = 2,
			to = 3,
			guard = {
				-- Total value of units
				value = { min = 1000, max = 1000 },
				
				loot = {
					-- Item types available, optional
					itemTypes = {Item.Orb, Item.Talisman, Item.PotionBoost, Item.PotionHeal},
					-- Value of item
					-- optional if item is specified
					value = { min = 2000, max = 3000 },
				},
			},
		},
		{
			from = 2,
			to = 4,
			guard = {
				-- Total value of units
				value = { min = 1000, max = 1000 },
				
				loot = {
					-- Item types available, optional
					itemTypes = {Item.Orb, Item.Talisman, Item.PotionBoost, Item.PotionHeal},
					-- Value of item
					-- optional if item is specified
					value = { min = 2000, max = 3000 },
				},
			},
		},
	},
}

