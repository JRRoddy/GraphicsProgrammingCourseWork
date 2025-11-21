#pragma once

#include "rendering/uniformDataTypes.hpp"

class UBOCacheBasePass {
public:
	virtual void parseScene() = 0;
	template<typename T>
	bool setCachedValue(const std::string& blockName, const std::string& uniformName, T value); //!< Set a single cached value
	void uploadCachedValues() const; //!< Upload all cached values
protected:
	std::unordered_multimap<std::string, std::pair<std::string, UniformData>> m_cache; //!< Internal storage of data being uploaded for this pass only
	std::unordered_map<std::string, size_t> m_UBOmap; //!< Maps block names to global UBO binding points
private:
	bool getLayout(size_t bindingPoint, UBOLayout& layout);
};

template<typename T>
inline bool UBOCacheBasePass::setCachedValue(const std::string& blockName, const std::string& uniformName, T value)
{
	auto bindPointIt = m_UBOmap.find(blockName);
	if (bindPointIt != m_UBOmap.end())
	{
		UBOLayout layout;
		bool gotLayout = getLayout(bindPointIt->second, layout);
		if (gotLayout) {
			for (auto& elem : layout)
			{
				if (elem.m_name == uniformName)
				{
					UniformData data;
					data.type = elem.m_dataType;
					data.enabled = true;
					data.data = value;
					std::pair<std::string, UniformData> dataPair(uniformName, data);
					bool set = false;
					for (auto& cacheData : m_cache) {
						auto& cachedBlockName = cacheData.first;
						auto& cachedUniformName = cacheData.second.first;
						if (cachedBlockName == blockName && cachedUniformName == uniformName) {
							cacheData.second.second = data;
							set = true;
						}
					}
					if (!set) m_cache.insert(std::pair<std::string, std::pair<std::string, UniformData>>(blockName, dataPair));
					return true;
				}
			}
		}
	}

	return false;
}