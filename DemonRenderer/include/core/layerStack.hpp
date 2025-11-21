/** \file layerStack.hpp */
#pragma once
#include "core/layer.hpp"

/** \class LayerStack
*	\brief Class which stores unique pointers to Layers in stack.
*	Pointers are used as Layer are abstract base classes.
*/

class LayerStack {
public:
	void push(std::unique_ptr<Layer> layer); //!< Push Layer onto the stack
	template <typename T, typename... Args>
	void emplace_top(Args&&... args); //!< Push Layer onto the stack
	std::unique_ptr<Layer> pop(); //!< Pops a layer off the stack
	[[nodiscard]] std::unique_ptr<Layer>& top()  noexcept; //!< Gives access by reference to the top of the stack
	[[nodiscard]] const std::unique_ptr<Layer>& top() const noexcept; //!< Give access by const reference to the top of the stack
	template<class Fn>
	void for_each(Fn f); //!< Apply function to every element in the layer stack
	template<class Fn>
	void for_each(Fn f) const; //!< Apply const function to every element in the layer stack
private:
	std::vector<std::unique_ptr<Layer>> m_layers; //!< Underlying data structure which hold layers
};

template<class Fn>
inline void LayerStack::for_each(Fn f)
{
	std::for_each(m_layers.begin(), m_layers.end(), f);
}

template<class Fn>
inline void LayerStack::for_each(Fn f) const
{
	std::for_each(m_layers.begin(), m_layers.end(), f);
}

template <typename T, typename... Args>
inline void LayerStack::emplace_top(Args&&... args) {
	m_layers.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
}