{
    id = 0,
    type = Zone.PlayerStart,
    race = races[1],
    size = 20,
    
    capital = {
        garrison = {
            subraceTypes = { Subrace.Human },
            value = { min = 150, max = 175 },
            loot = {
                itemTypes = { Item.PotionHeal, Item.PotionRevive },
                value = { min = 750, max = 800 }
            }
        },

        spells = {
            'g000ss0021',
            'g000ss0004'
        },
    }
}