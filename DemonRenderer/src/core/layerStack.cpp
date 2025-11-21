/** \file layerStack.cpp */

#include "core/layerStack.hpp"

void LayerStack::push(std::unique_ptr<Layer> layer)
{
	m_layers.push_back(std::move(layer));
}

std::unique_ptr<Layer> LayerStack::pop()
{
	auto result = std::move(m_layers.back());
	m_layers.pop_back();
	return result;
}

std::unique_ptr<Layer>& LayerStack::top() noexcept
{
	return m_layers.back();
}

const std::unique_ptr<Layer>& LayerStack::top() const noexcept
{
	return m_layers.back();
}

