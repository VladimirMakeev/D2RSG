{
    exchangeRates = [[
function getExchangeRates(visitor)
    return {
        {
            Resource.Gold,
            {
                { Resource.LifeMana, 10, 1 },
                { Resource.DeathMana, 10, 1 },
                { Resource.RunicMana, 10, 1 },
                { Resource.InfernalMana, 10, 1 },
                { Resource.GroveMana, 10, 1 }
            }
        },
    }
end
    ]],

    stock = {
        { resource = Resource.Gold, infinite = true },
        { resource = Resource.LifeMana, infinite = true },
        { resource = Resource.InfernalMana, infinite = true },
        { resource = Resource.DeathMana, infinite = true },
        { resource = Resource.RunicMana, infinite = true },
    }
}