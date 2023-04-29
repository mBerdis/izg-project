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

void prepareNode(GPUMemory& mem, CommandBuffer& commandBuffer, Model const& model, Node* node, glm::mat4 varMat)
{
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

		if (mesh.diffuseTexture != -1)
		{
			// mesh has texture

		}

	}

	// process children
	for each (Node child in node->children)
	{
		prepareNode(mem, commandBuffer, model, &child, varMat);
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
void prepareModel(GPUMemory&mem,CommandBuffer&commandBuffer,Model const&model){
  (void)mem;
  (void)commandBuffer;
  (void)model;
  /// \todo Tato funkce připraví command buffer pro model a nastaví správně pamět grafické karty.<br>
  /// Vaším úkolem je správně projít model a vložit vykreslovací příkazy do commandBufferu.
  /// Zároveň musíte vložit do paměti textury, buffery a uniformní proměnné, které buffer command buffer využívat.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace a v testech.

  mem.programs->fragmentShader = drawModel_fragmentShader;
  mem.programs->vertexShader   = drawModel_vertexShader  ;

  // add first clear command
  ClearCommand clear;
  clear.color = glm::vec4(0.1, 0.15, 0.1, 1);
  clear.depth = 1e+11;

  commandBuffer.commands[commandBuffer.nofCommands].type = CommandType::CLEAR;
  commandBuffer.commands[commandBuffer.nofCommands].data.clearCommand = clear;
  commandBuffer.nofCommands++;

  for each (Node root in model.roots)
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
void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
  (void)outVertex;
  (void)inVertex;
  (void)si;
  /// \todo Tato funkce reprezentujte vertex shader.<br>
  /// Vaším úkolem je správně trasnformovat vrcholy modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
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
void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si){
  (void)outFragment;
  (void)inFragment;
  (void)si;
  /// \todo Tato funkce reprezentujte fragment shader.<br>
  /// Vaším úkolem je správně obarvit fragmenty a osvětlit je pomocí lambertova osvětlovacího modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel_fs]

