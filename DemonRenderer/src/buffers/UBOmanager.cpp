#include "buffers/UBOmanager.hpp"

bool UBOManager::add(const UBOLayout& layout)
{
	auto itToExistingUBO = m_UBOsByBindingPoint.find(layout.getBindingPoint());
	if (itToExistingUBO == m_UBOsByBindingPoint.end()) {
		// No UBO at this binding point - add it
		m_UBOsByBindingPoint.emplace(layout.getBindingPoint(), std::make_unique<UBO>(layout));
		return true;
	}

	// Bind point is already taken
	// Flag an error message if the layout of the UBO is different to that already created
	if (layout != itToExistingUBO->second->getLayout())
		spdlog::error("UBO manager add UBO error. Attempting to add UBO at existing binding point {} with different layout.", layout.getBindingPoint());
	
	return false;
	
}

bool UBOManager::getLayout(size_t bindingPoint, UBOLayout& layout)
{
	auto it = m_UBOsByBindingPoint.find(bindingPoint);
	if(it == m_UBOsByBindingPoint.end()) return false;

	layout = it->second->getLayout();
	return true;
}


