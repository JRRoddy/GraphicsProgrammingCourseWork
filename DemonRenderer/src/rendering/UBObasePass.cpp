#include "rendering/UBObasePass.hpp"
#include "core/application.hpp"

void UBOCacheBasePass::uploadCachedValues() const
{

	for (auto& dataPair : m_cache)
	{
		auto& blockName = dataPair.first;
		auto it = m_UBOmap.find(blockName);
		if (it == m_UBOmap.end()) {
			spdlog::error("Uploading UBO cache. Can't find block {}", blockName);
			continue;
		}
		
		auto& bindingPoint = it->second;
		auto& uniformName = dataPair.second.first;
		auto& info = dataPair.second.second;

		switch (info.type)
		{
		case GL_FLOAT:
		{
			float _float = std::get<float>(info.data);
			Application::UBOmanager.uploadUniform(bindingPoint, uniformName, &_float);
		}
		break;
		case GL_FLOAT_VEC3:
			glm::vec3 vec3 = std::get<glm::vec3>(info.data);
			Application::UBOmanager.uploadUniform(bindingPoint, uniformName, glm::value_ptr(vec3));
			break;
		case GL_FLOAT_MAT4:
			glm::mat4 mat4 = std::get<glm::mat4>(info.data);
			Application::UBOmanager.uploadUniform(bindingPoint, uniformName, glm::value_ptr(mat4));
			break;
		}
	}
}

bool UBOCacheBasePass::getLayout(size_t bindingPoint, UBOLayout& layout)
{
	bool gotLayout = Application::UBOmanager.getLayout(bindingPoint, layout);
	return gotLayout;
}
