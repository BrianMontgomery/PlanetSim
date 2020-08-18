#include "PSIMPCH.h"
#include "VulkanVertexArray.h"
#include "Platform/Vk/FrameWork/VulkanFrameWork.h"


VulkanVertexArray::VulkanVertexArray()
{
	PSIM_PROFILE_FUNCTION();
}

VulkanVertexArray::~VulkanVertexArray()
{
	PSIM_PROFILE_FUNCTION();
}

void VulkanVertexArray::Bind() const
{
	PSIM_PROFILE_FUNCTION();

	//glBindVertexArray(m_RendererID);
}

void VulkanVertexArray::Unbind() const
{
	PSIM_PROFILE_FUNCTION();

	//glBindVertexArray(0);
}

void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	PSIM_PROFILE_FUNCTION();

	PSIM_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

	m_VertexBuffers.push_back(vertexBuffer);
}

void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	PSIM_PROFILE_FUNCTION();

	m_IndexBuffer = indexBuffer;
}

//vk::VertexInputBindingDescription
void* VulkanVertexArray::getBindingDescription()
{
	//get stride
	uint32_t stride = 0;
	for (int i = 0; i < m_VertexBuffers.size(); i++)
	{
		stride += m_VertexBuffers[i]->GetLayout().GetStride();
	}

	//create binding description
	bindingDescription = { 0, stride, vk::VertexInputRate::eVertex };

	return &bindingDescription;
}

//std::vector<vk::VertexInputAttributeDescription>
void* VulkanVertexArray::getAttributeDescriptions()
{
	attributeDescriptions.clear();
	for (int h = 0; h < m_VertexBuffers.size(); h++)
	{
		for (int i = 0; i < m_VertexBuffers[h]->GetLayout().GetElements().size(); i++)
		{
			uint32_t j = i;
			switch (m_VertexBuffers[h]->GetLayout().GetElements()[i].Type)
			{
			case ShaderDataType::Float:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Float2:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Float3:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32B32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Float4:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32B32A32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Mat3:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32B32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				attributeDescriptions.push_back({ j + 1, 0, vk::Format::eR32G32B32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				attributeDescriptions.push_back({ j + 2, 0, vk::Format::eR32G32B32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				i += 2;
				break;
			case ShaderDataType::Mat4:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32B32A32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				attributeDescriptions.push_back({ j + 1, 0, vk::Format::eR32G32B32A32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				attributeDescriptions.push_back({ j + 2, 0, vk::Format::eR32G32B32A32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				attributeDescriptions.push_back({ j + 3, 0, vk::Format::eR32G32B32A32Sfloat, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				i += 3;
				break;
			case ShaderDataType::Int:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32Sint, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Int2:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32Sint, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Int3:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32B32Sint, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Int4:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32G32B32A32Sint, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			case ShaderDataType::Bool:
				attributeDescriptions.push_back({ j, 0, vk::Format::eR32Uint, (m_VertexBuffers[h]->GetLayout().GetElements())[i].Offset });
				break;
			}
		}
	}
	return &attributeDescriptions;
}

//std::vector<vk::Buffer>
void* VulkanVertexArray::getVertexBuffersBuffers()
{
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	if (vBuffBuffers.empty() ) {
		vBuffBuffers = {};
	}
	else {
		vBuffBuffers.clear();
	}

	for (int i = 0; i < m_VertexBuffers.size(); i++)
	{
		vBuffBuffers.push_back(framework->getBufferList()->get(m_VertexBuffers[i]->getID())->getBuffer());
	}

	return &vBuffBuffers;
}

//vk::Buffer
void* VulkanVertexArray::getIndexBufferBuffer()
{
	VulkanFrameWork* framework = VulkanFrameWork::getFramework();

	iBuffBuffer = framework->getBufferList()->get(m_IndexBuffer->getID())->getBuffer();
	return &iBuffBuffer;
}

void VulkanVertexArray::cleanUp()
{
	PSIM_PROFILE_FUNCTION();

	//check if these are the last ones
	vBuffBuffers.clear();
	m_IndexBuffer.reset();
	m_VertexBuffers.clear();
}