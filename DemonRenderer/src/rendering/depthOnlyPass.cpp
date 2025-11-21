#include "rendering/depthOnlyPass.hpp"
#include "core/application.hpp"

void DepthPass::parseScene()
{
	for (auto& actor : scene->m_actors)
	{
		if (actor.material) {
			if (actor.depthMaterial) {
				for (auto& [blockname, layout] : actor.depthMaterial->m_shader->m_UBOLayouts) {
					// Add blockname lookup
					auto UBOmapIt = m_UBOmap.find(blockname);
					if (UBOmapIt == m_UBOmap.end()) {
						// Insert lookup
						m_UBOmap.insert({blockname, layout.getBindingPoint()});
					}
					else {
						if (UBOmapIt->second != layout.getBindingPoint()) {
							spdlog::error("Error - attempting to add to UBO blocks with the same name to different binding points!");
						}
					}
			
					// Add UBO
					Application::UBOmanager.add(layout);
				}
			}
		}
	}
}
