#pragma once

#include <array>
#include <cstdint>

namespace rsg {

class CMidgardID
{
public:
    enum class Category
    {
        Global,
        Campaign,
        Scenario,
        External,
        Invalid, /**< Should not be serialized. */
    };

    enum class Type
    {
        Empty = 0,         /**< Type of empty id 'g000000000' when converted from string.
                            * Can not be used to construct id from parts.
                            */
        ApplicationText,   /**< 'TA', entries of TApp.dbf and TAppEdit.dbf. */
        Building,          /**< 'BB', entries of Gbuild.dbf. */
        Race,              /**< 'RR' */
        Lord,              /**< 'LR' */
        Spell,             /**< 'SS' */
        UnitGlobal,        /**< 'UU', units implementations, entries of Gunits.dbf. */
        UnitGenerated,     /**< 'UG', runtime-generated units implementations. */
        UnitModifier,      /**< 'UM' */
        Attack,            /**< 'AA' */
        TextGlobal,        /**< 'TG', entries of Tglobal.dbf. */
        LandmarkGlobal,    /**< 'MG', entries of GLmark.dbf. */
        ItemGlobal,        /**< 'IG', entries of GItem.dbf. */
        NobleAction,       /**< 'NA', entries of Gaction.dbf. */
        DynamicUpgrade,    /**< 'DU' */
        DynamicAttack,     /**< 'DA' */
        AL,                /**< 16 for 'AL' */
        DC,                /**< 17 for 'DC' */
        AC,                /**< 18 for 'AC' */
        CampaignFile = 19, /**< 'CC' */
        CW,                /**< 20 for 'CW' */
        CO,                /**< 21 for 'CO' */
        Plan = 22,         /**< 'PN' */
        ObjectCount,       /**< 'OB', number of objects in scenario file. */
        ScenarioFile,      /**< 'SC' */
        Map,               /**< 'MP' */
        MapBlock,          /**< 'MB' */
        ScenarioInfo,      /**< 'IF' */
        SpellEffects,      /**< 'ET' */
        Fortification,     /**< 'FT', capitals and villages. */
        Player,            /**< 'PL' */
        PlayerKnownSpells, /**< 'KS' */
        Fog,               /**< 'FG' */
        PlayerBuildings,   /**< 'PB' */
        Road,              /**< 'RA' */
        Stack,             /**< 'KC' */
        Unit,              /**< 'UN' */
        Landmark,          /**< 'MM' */
        Item,              /**< 'IM' */
        Bag,               /**< 'BG' */
        Site,              /**< 'SI' */
        Ruin,              /**< 'RU' */
        Tomb,              /**< 'TB' */
        Rod,               /**< 'RD' */
        Crystal,           /**< 'CR' */
        Diplomacy,         /**< 'DP' */
        SpellCast,         /**< 'ST' */
        Location,          /**< 'LO' */
        StackTemplate,     /**< 'TM' */
        Event,             /**< 'EV' */
        StackDestroyed,    /**< 'SD' */
        TalismanCharges,   /**< 'TC' */
        MT,                /**< 52 for 'MT' */
        Mountains = 53,    /**< 'ML' */
        SubRace,           /**< 'SR' */
        BR,                /**< 55 for 'BR' */
        QuestLog = 56,     /**< 'QL' */
        TurnSummary,       /**< 'TS' */
        ScenarioVariable,  /**< 'SV' */
        Invalid = 59
    };

    static constexpr std::uint32_t invalidCategoryIndex{256};
    static constexpr std::uint32_t invalidTypeIndex{0x10000};
    static constexpr size_t idStringLength{10};

    using String = std::array<char, idStringLength + 1>;

    /** Creates empty id. */
    constexpr CMidgardID() = default;

    /** Creates id from string. */
    CMidgardID(const char* string);

    /** Creates id from parts. */
    CMidgardID(Category category, std::uint8_t categoryIndex, Type type, std::uint16_t typeIndex)
        : value{fromParts(category, categoryIndex, type, typeIndex)}
    { }

    /** Creates id from raw 32 bit value. */
    constexpr CMidgardID(std::uint32_t value)
        : value{value}
    { }

    CMidgardID(const CMidgardID& other)
        : value{other.value}
    { }

    constexpr CMidgardID& operator=(const CMidgardID& other)
    {
        if (*this == other) {
            return *this;
        }

        value = other.value;
        return *this;
    }

    constexpr bool operator==(const CMidgardID& other) const
    {
        return value == other.value;
    }

    constexpr bool operator!=(const CMidgardID& other) const
    {
        return !(*this == other);
    }

    constexpr bool operator<(const CMidgardID& other) const
    {
        return value < other.value;
    }

    constexpr bool operator<=(const CMidgardID& other) const
    {
        return value <= other.value;
    }

    Category getCategory() const;

    std::uint32_t getCategoryIndex() const;

    Type getType() const;

    std::uint32_t getTypeIndex() const;

    void toString(String& idString) const
    {
        toString(idString.data());
    }

    void toString(char* idString) const;

private:
    friend struct CMidgardIDHash;

    static std::uint32_t fromParts(Category category,
                                   std::uint8_t categoryIndex,
                                   Type type,
                                   std::uint16_t typeIndex);

    std::uint32_t value{};
};

extern const CMidgardID invalidId;
extern const CMidgardID emptyId;

struct CMidgardIDHash
{
    std::size_t operator()(const CMidgardID& id) const
    {
        // All identifiers and their 32-bit value representaions must be unique.
        // There is no reason to compute full hash
        return static_cast<std::size_t>(id.value);
    }
};

} // namespace rsg
