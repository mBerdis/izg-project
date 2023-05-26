/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>

///\endcond

DrawCommand createDrawCommand(Mesh* mesh)
{
	VertexArray vao;
	vao.indexBufferID	= mesh->indexBufferID;
	vao.indexOffset		= mesh->indexOffset	 ;
	vao.indexType		= mesh->indexType	 ;
	vao.vertexAttrib[0] = mesh->position	 ;
	vao.vertexAttrib[1] = mesh->normal		 ;
	vao.vertexAttrib[2] = mesh->texCoord	 ;

	DrawCommand draw;
	draw.backfaceCulling = !mesh->doubleSided;
	draw.nofVertices	 = mesh->nofIndices ;
	draw.programID		 = 0				;
	draw.vao			 = vao				;

	return draw;
}

void prepareNode(GPUMemory& mem, CommandBuffer& commandBuffer, Model const& model, Node* node, glm::mat4& varMat)
{
	glm::mat4 modelMat = varMat * node->modelMatrix;

	// process self
	if (node->mesh != -1)
	{
		Mesh mesh = model.meshes[node->mesh];
		
		// create draw command
		DrawCommand draw = createDrawCommand(&mesh);
		if (commandBuffer.nofCommands + 1 < commandBuffer.maxCommands)
		{
			commandBuffer.commands[commandBuffer.nofCommands].type = CommandType::DRAW;
			commandBuffer.commands[commandBuffer.nofCommands].data.drawCommand = draw;
			commandBuffer.nofCommands++;
		}

		const int index = 10 + (commandBuffer.nofCommands - 2) * 5;
		mem.uniforms[index + 0].m4 = modelMat;
		mem.uniforms[index + 1].m4 = glm::transpose(glm::inverse(modelMat));
		mem.uniforms[index + 2].v4 = mesh.diffuseColor;
		mem.uniforms[index + 3].i1 = mesh.diffuseTexture;
		mem.uniforms[index + 4].v1 = mesh.doubleSided;
	}

	// process children
	for (Node child : node->children)
	{
		prepareNode(mem, commandBuffer, model, &child, modelMat);
	}
}

/**
 * @brief This function prepares model into memory and creates command buffer
 *
 * @param mem gpu memory
 * @param commandBuffer command buffer
 * @param model model structure
 */
//! [drawModel]
void prepareModel(GPUMemory&mem, CommandBuffer& commandBuffer, Model const& model){
  (void)mem;
  (void)commandBuffer;
  (void)model;
  /// \todo Tato funkce připraví command buffer pro model a nastaví správně pamět grafické karty.<br>
  /// Vaším úkolem je správně projít model a vložit vykreslovací příkazy do commandBufferu.
  /// Zároveň musíte vložit do paměti textury, buffery a uniformní proměnné, které buffer command buffer využívat.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace a v testech.
  
  int i = 0;
  for (Buffer const buf : model.buffers)
  {
	  mem.buffers[i] = buf;
	  i++;
  }
  i = 0;
  for (Texture const tex : model.textures)
  {
	  mem.textures[i] = tex;
	  i++;
  }

  mem.programs[0].vs2fs[0] = AttributeType::VEC3;
  mem.programs[0].vs2fs[1] = AttributeType::VEC3;
  mem.programs[0].vs2fs[2] = AttributeType::VEC2;
  mem.programs[0].vs2fs[3] = AttributeType::UINT;

  mem.programs[0].fragmentShader = drawModel_fragmentShader;
  mem.programs[0].vertexShader   = drawModel_vertexShader  ;

  // adding first clear command
  ClearCommand clear;
  clear.color = glm::vec4(0.1f, 0.15f, 0.1f, 1.f);
  clear.depth = 1e+11f;

  commandBuffer.commands[commandBuffer.nofCommands].type = CommandType::CLEAR;
  commandBuffer.commands[commandBuffer.nofCommands].data.clearCommand = clear;
  commandBuffer.nofCommands++;

  for (Node root : model.roots)
  {
	  prepareNode(mem, commandBuffer, model, &root, glm::mat4(1.f));
  }

}
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param si shader interface
 */
//! [drawModel_vs]
void drawModel_vertexShader(OutVertex& outVertex,InVertex const &inVertex,ShaderInterface const &si)
{
	const int index = 10 + inVertex.gl_DrawID * 5;

	// Apply vertex transformations
	const glm::vec4 vertex1 = glm::vec4(inVertex.attributes[0].v3, 1.f);
	const glm::vec4 vertex2 = glm::vec4(inVertex.attributes[1].v3, 0.f);

	outVertex.attributes[0].v3 = si.uniforms[index].m4 * vertex1;
	outVertex.attributes[1].v3 = si.uniforms[index + 1].m4 * vertex2;
	outVertex.attributes[2].v2 = inVertex.attributes[2].v2;
	outVertex.attributes[3].u1 = inVertex.gl_DrawID;

	const glm::mat4 transformationMatrix = si.uniforms[0].m4 * si.uniforms[index].m4;
	outVertex.gl_Position = transformationMatrix * vertex1;

	/*
	outVertex.attributes[0].v3 = si.uniforms[index].m4 * glm::vec4(inVertex.attributes[0].v3, 1.f);
	outVertex.attributes[1].v3 = si.uniforms[index + 1].m4 * glm::vec4(inVertex.attributes[1].v3, 0.f);
	outVertex.attributes[2].v2 = inVertex.attributes[2].v2;
	outVertex.attributes[3].u1 = inVertex.gl_DrawID;
	outVertex.gl_Position = si.uniforms[0].m4 * si.uniforms[index].m4 * glm::vec4(inVertex.attributes[0].v3, 1.f);
	*/
}
//! [drawModel_vs]

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param si shader interface
 */
//! [drawModel_fs]
void drawModel_fragmentShader(OutFragment& outFragment,InFragment const&inFragment,ShaderInterface const&si){
	(void)outFragment;
	(void)inFragment;
	(void)si;
	/// \todo Tato funkce reprezentujte fragment shader.<br>
	/// Vaším úkolem je správně obarvit fragmenty a osvětlit je pomocí lambertova osvětlovacího modelu.
	/// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
	
	const int index = 10 + inFragment.attributes[3].u1 * 5;
	int32_t textureID = si.uniforms[index + 3].i1;
	glm::vec3 normal  = glm::normalize(inFragment.attributes[1].v3);
	glm::vec3 light   = glm::normalize(si.uniforms[1].v3 - inFragment.attributes[0].v3);
	glm::vec4 dC; 

	// check if texture is set
	if (textureID == -1)
		dC = si.uniforms[index + 2].v4;
	else
		dC = read_texture(si.textures[textureID], inFragment.attributes[2].v2);

	if (si.uniforms[index + 4].v1 == 1.f && 
		glm::dot(normal, si.uniforms[2].v3 - inFragment.attributes[0].v3) < 0.0)
	{
		normal *= -1.0f;
	}

	float dF = glm::clamp(glm::dot(light, normal), 0.f, 1.f);
	glm::vec4 aL = dC * 0.2f;	// aF is 0.2
	glm::vec4 dL = dC * dF;

	outFragment.gl_FragColor.r = aL.r + dL.r;
	outFragment.gl_FragColor.g = aL.g + dL.g;
	outFragment.gl_FragColor.b = aL.b + dL.b;
	outFragment.gl_FragColor.a = dC.a;
}
//! [drawModel_fs]

