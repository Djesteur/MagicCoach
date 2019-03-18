#ifndef COMPONENT_HPP
#define COMPONENT_HPP

enum class ComponentType {

	Boolean,
	Integer,
	IntegerArray,
	Word,
	WordArray,
	Target,
	TargetArray,
	Script
};

class Component {

	public:

		Component(ComponentType type);
		Component(const Component&) = delete;
		Component &operator=(const Component&) = delete;

		ComponentType getType() const;

	protected:

		ComponentType m_type;
};

#endif