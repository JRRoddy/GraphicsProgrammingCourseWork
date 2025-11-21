/** \file UBOmanager.hpp */
#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "rendering/uniformDataTypes.hpp"
#include "buffers/UBO.hpp"

/** \class UBOManager
*	\brief Holds UBO created when materials within a render pass are scanned. */
class UBOManager
{
public:
	UBOManager() = default; //!< Default constructor
	bool add(const UBOLayout& layout); //!< Add a UBO
	bool getLayout(size_t bindingPoint, UBOLayout& layout);
	template<typename T>
	bool uploadUniform(size_t bindingPoint, const std::string& uniformName, T value) const; //!< Uploads a single value
private:
	std::unordered_map<size_t, std::unique_ptr<UBO>> m_UBOsByBindingPoint; //!< UBOs being managed by this object};
};

template<typename T>
inline bool UBOManager::uploadUniform(size_t bindingPoint, const std::string& uniformName, T value) const
{
	auto UBOptrPair = m_UBOsByBindingPoint.find(bindingPoint);
	if (UBOptrPair != m_UBOsByBindingPoint.end())
		return UBOptrPair->second->uploadData(uniformName, (void*)(value));

	return false;
}
