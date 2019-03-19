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
	Script,
	Unknown
};

class Component {

	public:

		Component();
		Component(ComponentType type);

		virtual ~Component() = 0;

		ComponentType getType() const;

	protected:

		ComponentType m_type;
};

#endif