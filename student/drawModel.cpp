/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>
#include <glm/glm.hpp>
#include <xmmintrin.h>
#include <glm/gtc/type_ptr.hpp>

 ///\endcond

// Function to multiply two square matrices
void multiplyMatrices(const glm::mat4& matrix1, const glm::mat4& matrix2, glm::mat4& result) {
	__m128 row0 = _mm_loadu_ps(&matrix2[0][0]);
	__m128 row1 = _mm_loadu_ps(&matrix2[1][0]);
	__m128 row2 = _mm_loadu_ps(&matrix2[2][0]);
	__m128 row3 = _mm_loadu_ps(&matrix2[3][0]);

	for (int i = 0; i < 4; ++i) {
		__m128 a0 = _mm_set1_ps(matrix1[i][0]);
		__m128 a1 = _mm_set1_ps(matrix1[i][1]);
		__m128 a2 = _mm_set1_ps(matrix1[i][2]);
		__m128 a3 = _mm_set1_ps(matrix1[i][3]);

		__m128 mul0 = _mm_mul_ps(a0, row0);
		__m128 mul1 = _mm_mul_ps(a1, row1);
		__m128 mul2 = _mm_mul_ps(a2, row2);
		__m128 mul3 = _mm_mul_ps(a3, row3);

		__m128 sum0 = _mm_add_ps(mul0, mul1);
		__m128 sum1 = _mm_add_ps(mul2, mul3);
		__m128 resultRow = _mm_add_ps(sum0, sum1);

		_mm_storeu_ps(&result[i][0], resultRow);
	}
}

glm::vec4 multiplyMatrixVector(const glm::mat4& matrix, glm::vec4& vector)
{
	__m128 vec = _mm_loadu_ps(glm::value_ptr(vector));
	__m128 row1 = _mm_loadu_ps(glm::value_ptr(matrix[0]));
	__m128 row2 = _mm_loadu_ps(glm::value_ptr(matrix[1]));
	__m128 row3 = _mm_loadu_ps(glm::value_ptr(matrix[2]));
	__m128 row4 = _mm_loadu_ps(glm::value_ptr(matrix[3]));

	__m128 res = _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0x00), row1);
	res = _mm_add_ps(res, _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0x55), row2));
	res = _mm_add_ps(res, _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0xAA), row3));
	res = _mm_add_ps(res, _mm_mul_ps(_mm_shuffle_ps(vec, vec, 0xFF), row4));

	glm::vec4 result;
	_mm_storeu_ps(glm::value_ptr(result), res);

	return result;
}

DrawCommand createDrawCommand(Mesh& mesh)
{
	VertexArray vao;
	vao.indexBufferID = mesh.indexBufferID;
	vao.indexOffset = mesh.indexOffset;
	vao.indexType = mesh.indexType;
	vao.vertexAttrib[0] = mesh.position;
	vao.vertexAttrib[1] = mesh.normal;
	vao.vertexAttrib[2] = mesh.texCoord;

	DrawCommand draw;
	draw.backfaceCulling = !mesh.doubleSided;
	draw.nofVertices = mesh.nofIndices;
	draw.programID = 0;
	draw.vao = vao;

	return draw;
}

void prepareNode(GPUMemory& mem, CommandBuffer& commandBuffer, Model const& model, Node const& node, glm::mat4& varMat)
{
	glm::mat4 modelMat;
	multiplyMatrices(node.modelMatrix, varMat, modelMat);

	// process self
	if (node.mesh != -1)
	{
		Mesh mesh = model.meshes[node.mesh];

		// create draw command
		DrawCommand draw = createDrawCommand(mesh);
		if (commandBuffer.nofCommands + 1 < commandBuffer.maxCommands)
		{
			commandBuffer.commands[commandBuffer.nofCommands].type = CommandType::DRAW;
			commandBuffer.commands[commandBuffer.nofCommands].data.drawCommand = draw;
			commandBuffer.nofCommands++;
		}

		int index = 10 + (commandBuffer.nofCommands - 2) * 5;
		mem.uniforms[index + 0].m4 = modelMat;
		mem.uniforms[index + 1].m4 = glm::transpose(glm::inverse(modelMat));
		mem.uniforms[index + 2].v4 = mesh.diffuseColor;
		mem.uniforms[index + 3].i1 = mesh.diffuseTexture;
		mem.uniforms[index + 4].v1 = mesh.doubleSided;
	}

	// process children
	for (Node child : node.children)
	{
		prepareNode(mem, commandBuffer, model, child, modelMat);
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
void prepareModel(GPUMemory& mem, CommandBuffer& commandBuffer, Model const& model) {
	/// \todo Tato funkce připraví command buffer pro model a nastaví správně pamět grafické karty.<br>
	/// Vaším úkolem je správně projít model a vložit vykreslovací příkazy do commandBufferu.
	/// Zároveň musíte vložit do paměti textury, buffery a uniformní proměnné, které buffer command buffer využívat.
	/// Bližší informace jsou uvedeny na hlavní stránce dokumentace a v testech. 

	for (size_t i = 0; i < model.buffers.size(); i++)
		mem.buffers[i] = model.buffers[i];

	for (size_t i = 0; i < model.textures.size(); i++)
		mem.textures[i] = model.textures[i];

	mem.programs[0].vs2fs[0] = AttributeType::VEC3;
	mem.programs[0].vs2fs[1] = AttributeType::VEC3;
	mem.programs[0].vs2fs[2] = AttributeType::VEC2;
	mem.programs[0].vs2fs[3] = AttributeType::UINT;

	mem.programs[0].fragmentShader = drawModel_fragmentShader;
	mem.programs[0].vertexShader = drawModel_vertexShader;

	// adding first clear command
	ClearCommand clear;
	clear.color = glm::vec4(0.1f, 0.15f, 0.1f, 1.f);
	clear.depth = 1e+11f;

	commandBuffer.commands[commandBuffer.nofCommands].type = CommandType::CLEAR;
	commandBuffer.commands[commandBuffer.nofCommands].data.clearCommand = clear;
	commandBuffer.nofCommands++;
	glm::mat4 mat = glm::mat4(1.f);

	for (Node root : model.roots)
	{
		prepareNode(mem, commandBuffer, model, root, mat);
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
void drawModel_vertexShader(OutVertex& outVertex, InVertex const& inVertex, ShaderInterface const& si)
{
	int index = 10 + inVertex.gl_DrawID * 5;

	// Apply vertex transformations
	glm::vec4 vertex1 = glm::vec4(inVertex.attributes[0].v3, 1.f);
	glm::vec4 vertex2 = glm::vec4(inVertex.attributes[1].v3, 0.f);

	outVertex.attributes[0].v3 = multiplyMatrixVector(si.uniforms[index].m4, vertex1);
	outVertex.attributes[1].v3 = multiplyMatrixVector(si.uniforms[index + 1].m4, vertex2);
	outVertex.attributes[2].v2 = inVertex.attributes[2].v2;
	outVertex.attributes[3].u1 = inVertex.gl_DrawID;

	glm::mat4 transformationMatrix;
	multiplyMatrices(si.uniforms[index].m4, si.uniforms[0].m4, transformationMatrix);
	outVertex.gl_Position = multiplyMatrixVector(transformationMatrix, vertex1);
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
void drawModel_fragmentShader(OutFragment& outFragment, InFragment const& inFragment, ShaderInterface const& si) 
{
	/// \todo Tato funkce reprezentujte fragment shader.<br>
	/// Vaším úkolem je správně obarvit fragmenty a osvětlit je pomocí lambertova osvětlovacího modelu.
	/// Bližší informace jsou uvedeny na hlavní stránce dokumentace.

	int index = 10 + inFragment.attributes[3].u1 * 5;
	int32_t textureID = si.uniforms[index + 3].i1;
	glm::vec3 normal = glm::normalize(inFragment.attributes[1].v3);
	glm::vec3 light = glm::normalize(si.uniforms[1].v3 - inFragment.attributes[0].v3);
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
