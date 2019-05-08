#include "CardLoader.hpp"

CardLoader::CardLoader(ComponentKeeper &keeper, EntityCreator &creator) : m_keeper{keeper},
																		  m_creator{creator} {}

Entity CardLoader::getCard(const unsigned int id)
{

	if (m_loadedCard.find(id) != m_loadedCard.end())
	{
		return m_loadedCard[id];
	}
	else
	{
		m_loadedCard.insert(std::make_pair(id, loadFromFile("filePath", id)));
	}

	return m_loadedCard[id];
}

Entity CardLoader::loadFromFile(const std::string path, const unsigned int id)
{
	Entity newCard{m_creator.newEntity()};
	m_keeper.addEntity(newCard);

	std::ifstream jsonFile(path, std::ifstream::binary);
	Json::Value root;
	jsonFile >> root;

	// Get the card values, returns null if there is no such member
	const Json::Value cardInfos = root[id];

	// Adding the name component
	std::string cardName = cardInfos["Name"].asString();
	std::shared_ptr<Component> nameComponent{newComponent(ComponentType::Word)};
	std::static_pointer_cast<WordComponent>(nameComponent)->data() = cardName;
	m_keeper.addComponent(newCard, "Name", nameComponent);

	// Adding the power component
	int cardPower = cardInfos["Power"].asInt();
	std::shared_ptr<Component> powerComponent{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(powerComponent)->data() = cardPower;
	m_keeper.addComponent(newCard, "Power", powerComponent);

	// Adding the toughness component
	int cardToughness = cardInfos["Toughness"].asInt();
	std::shared_ptr<Component> toughnessComponent{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(toughnessComponent)->data() = cardToughness;
	m_keeper.addComponent(newCard, "Toughness", toughnessComponent);

	// Adding the loyalty component
	int cardLoyalty = cardInfos["Loyalty"].asInt();
	std::shared_ptr<Component> loyaltyComponent{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(loyaltyComponent)->data() = cardLoyalty;
	m_keeper.addComponent(newCard, "Loyalty", loyaltyComponent);

	// Adding the mana cost components
	std::string tempCardManaCost = cardInfos["Mana cost"].asString();
	std::smatch match;
	int cardManaCost, cmc = 0;
	std::regex neutralCostPattern("\{[0-9]\}");
	std::regex whiteCostPattern("\{W\}");
	std::regex blueCostPattern("\{U\}");
	std::regex blackCostPattern("\{B\}");
	std::regex redCostPattern("\{R\}");
	std::regex greenCostPattern("\{G\}");
	std::shared_ptr<Component> manaCostComponent{newComponent(ComponentType::IntegerArray)};

	std::ptrdiff_t matchCount(std::distance(
		std::sregex_iterator(tempCardManaCost.begin(), tempCardManaCost.end(), neutralCostPattern),
		std::sregex_iterator()));
	for (int i = 0; i < matchCount; i++)
	{
		std::static_pointer_cast<IntegerArrayComponent>(manaCostComponent)->data().emplace_back(Mana::AnyColor);
		cmc ++;
	}

	std::ptrdiff_t matchCount(std::distance(
		std::sregex_iterator(tempCardManaCost.begin(), tempCardManaCost.end(), whiteCostPattern),
		std::sregex_iterator()));
	for (int i = 0; i < matchCount; i++)
	{
		std::static_pointer_cast<IntegerArrayComponent>(manaCostComponent)->data().emplace_back(Mana::White);
		cmc ++;
	}

	std::ptrdiff_t matchCount(std::distance(
		std::sregex_iterator(tempCardManaCost.begin(), tempCardManaCost.end(), blueCostPattern),
		std::sregex_iterator()));
	for (int i = 0; i < matchCount; i++)
	{
		std::static_pointer_cast<IntegerArrayComponent>(manaCostComponent)->data().emplace_back(Mana::Blue);
		cmc ++;
	}

	std::ptrdiff_t matchCount(std::distance(
		std::sregex_iterator(tempCardManaCost.begin(), tempCardManaCost.end(), blackCostPattern),
		std::sregex_iterator()));
	for (int i = 0; i < matchCount; i++)
	{
		std::static_pointer_cast<IntegerArrayComponent>(manaCostComponent)->data().emplace_back(Mana::Black);
		cmc ++;
	}

	std::ptrdiff_t matchCount(std::distance(
		std::sregex_iterator(tempCardManaCost.begin(), tempCardManaCost.end(), redCostPattern),
		std::sregex_iterator()));
	for (int i = 0; i < matchCount; i++)
	{
		std::static_pointer_cast<IntegerArrayComponent>(manaCostComponent)->data().emplace_back(Mana::Red);
		cmc ++;
	}

	std::ptrdiff_t matchCount(std::distance(
		std::sregex_iterator(tempCardManaCost.begin(), tempCardManaCost.end(), greenCostPattern),
		std::sregex_iterator()));
	for (int i = 0; i < matchCount; i++)
	{
		std::static_pointer_cast<IntegerArrayComponent>(manaCostComponent)->data().emplace_back(Mana::Green);
		cmc ++;
	}

	m_keeper.addComponent(newCard, "ManaCost", manaCostComponent);

	std::shared_ptr<Component> CMCComponent{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(CMCComponent)->data() = cmc;
	m_keeper.addComponent(newCard, "CMC", CMCComponent);

	// Adding the type component
	std::string tempCardType = cardInfos["Type"].asString();
	int cardType;
	std::regex landPattern("Land");
	std::regex creaturePattern("Creature");
	std::regex artifactPattern("Artifact");
	std::regex enchantmentPattern("Enchantment");
	std::regex planeswalkerPattern("Planeswalker");
	std::regex sorceryPattern("Sorcery");
	std::regex instantPattern("Instant");

	if (std::regex_search(tempCardType, match, landPattern))
		cardType = CardType::Land;
	else if (std::regex_search(tempCardType, match, creaturePattern))
		cardType = CardType::Creature;
	else if (std::regex_search(tempCardType, match, artifactPattern))
		cardType = CardType::Artifact;
	else if (std::regex_search(tempCardType, match, enchantmentPattern))
		cardType = CardType::Enchantment;
	else if (std::regex_search(tempCardType, match, planeswalkerPattern))
		cardType = CardType::Planeswalker;
	else if (std::regex_search(tempCardType, match, sorceryPattern))
		cardType = CardType::Sorcery;
	else if (std::regex_search(tempCardType, match, instantPattern))
		cardType = CardType::Instant;

	std::shared_ptr<Component> typeComponent{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(typeComponent)->data() = cardType;
	m_keeper.addComponent(newCard, "Type", typeComponent);

	// Area
	std::shared_ptr<Component> cardArea{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(cardArea)->data() = Area::Library;
	m_keeper.addComponent(newCard, "Area", cardArea);

	// Controller?
	std::shared_ptr<Component> cardController{newComponent(ComponentType::Integer)};
	m_keeper.addComponent(newCard, "Controller", cardController);
	std::static_pointer_cast<IntegerComponent>(cardController)->data() = 0;

	// isTapped
	std::shared_ptr<Component> cardTapped{newComponent(ComponentType::Boolean)};
	std::static_pointer_cast<BooleanComponent>(cardTapped)->data() = false;
	m_keeper.addComponent(newCard, "IsTapped", cardTapped);

	// summonSickness
	std::shared_ptr<Component> cardSummon{newComponent(ComponentType::Boolean)};
	std::static_pointer_cast<BooleanComponent>(cardSummon)->data() = true;
	m_keeper.addComponent(newCard, "SummonSickness", cardSummon);

	// damageTaken
	std::shared_ptr<Component> cardDamageTaken{newComponent(ComponentType::Integer)};
	std::static_pointer_cast<IntegerComponent>(cardDamageTaken)->data() = 0;
	m_keeper.addComponent(newCard, "DamageTaken", cardDamageTaken);

	return newCard;
}
