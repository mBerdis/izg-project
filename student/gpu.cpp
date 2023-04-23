/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>

void readAttributes(Attribute* vertexAtrrib, GPUMemory& mem, VertexAttrib const& attrib, uint32_t shaderInvocation, uint32_t vertexId)
{
    if (attrib.type == AttributeType::EMPTY) return;

    const void* attribBuffer = mem.buffers[attrib.bufferID].data;
    unsigned char* bytePtr = reinterpret_cast<unsigned char*>(const_cast<void*>(attribBuffer));
    bytePtr += attrib.offset + attrib.stride * vertexId;

    if (attrib.type == AttributeType::UVEC2)
    {
        glm::uvec2* ind = (glm::uvec2*)(bytePtr);
        vertexAtrrib->u2 = *ind;
    }
    if (attrib.type == AttributeType::UVEC3)
    {
        glm::uvec3 *ind = (glm::uvec3*)(bytePtr);
        vertexAtrrib->u3 = *ind;
    }
    if (attrib.type == AttributeType::UVEC4)
    {
        glm::uvec4* ind = (glm::uvec4*)(bytePtr);
        vertexAtrrib->u4 = *ind;
    }
    if (attrib.type == AttributeType::VEC4)
    {
        glm::vec4* ind = (glm::vec4*)(bytePtr);
        vertexAtrrib->v4 = *ind;
    }
    if (attrib.type == AttributeType::VEC3)
    {
        glm::vec3* ind = (glm::vec3*)(bytePtr);
        vertexAtrrib->v3 = *ind;
    }
    if (attrib.type == AttributeType::VEC2)
    {
        glm::vec2* ind = (glm::vec2*)(bytePtr);
        vertexAtrrib->v2 = *ind;
    }
    if (attrib.type == AttributeType::FLOAT)
    {
        float* ind = (float*)(bytePtr);
        vertexAtrrib->v1 = *ind;
    }
    if (attrib.type == AttributeType::UINT)
    {
        uint32_t* ind = (uint32_t*)(bytePtr);
        vertexAtrrib->u1 = *ind;
    }
}

uint32_t computeVertexID(GPUMemory& mem, VertexArray const& vao, uint32_t shaderInvocation)
{
    if (vao.indexBufferID < 0) return shaderInvocation;

    const void* indexBuffer = mem.buffers[vao.indexBufferID].data;
    unsigned char* bytePtr = reinterpret_cast<unsigned char*>(const_cast<void*>(indexBuffer));
    bytePtr += vao.indexOffset;

    if (vao.indexType == IndexType::UINT32)
    {
        uint32_t* ind = (uint32_t*)(bytePtr);
        return ind[shaderInvocation];
    }
    else if (vao.indexType == IndexType::UINT16)
    {
        uint16_t* ind = (uint16_t*)(bytePtr);
        return ind[shaderInvocation];
    }
    else if (vao.indexType == IndexType::UINT8)
    {
        uint8_t* ind = (uint8_t*)(bytePtr);
        return ind[shaderInvocation];
    }
}

void runVertexAssembly(InVertex* inVertex, GPUMemory& mem, VertexArray const& vao, uint32_t shaderInvocation)
{
    inVertex->gl_VertexID = computeVertexID(mem, vao, shaderInvocation);
    
    for (size_t i = 0; i < maxAttributes; i++)
    {
        readAttributes(&inVertex->attributes[i], mem, vao.vertexAttrib[i], shaderInvocation, inVertex->gl_VertexID);
    }
}

void draw(GPUMemory& mem, DrawCommand cmd, uint32_t drawID) 
{
    Program prg = mem.programs[cmd.programID];

    for (size_t i = 0; i < cmd.nofVertices; i++)
    {
        InVertex inVertex;
        OutVertex outVertex;
        inVertex.gl_DrawID = drawID;
        
        runVertexAssembly(&inVertex, mem, cmd.vao, i);

        ShaderInterface si;
        prg.vertexShader(outVertex, inVertex, si);
    }
}

void clear(GPUMemory& mem, ClearCommand cmd) {
    if (cmd.clearColor) {
        uint8_t red     = (uint8_t) (cmd.color.r * 255.f);
        uint8_t green   = (uint8_t) (cmd.color.g * 255.f);
        uint8_t blue    = (uint8_t) (cmd.color.b * 255.f);
        uint8_t alpha   = (uint8_t) (cmd.color.a * 255.f);

        size_t colorBuffLenght = (mem.framebuffer.width * mem.framebuffer.height) * 4;
        for (size_t i = 0; i+4 < colorBuffLenght; i+= 4)
        {
            mem.framebuffer.color[i]        = red  ;
            mem.framebuffer.color[i + 1]    = green;
            mem.framebuffer.color[i + 2]    = blue ;
            mem.framebuffer.color[i + 3]    = alpha;
        }
    }
    if (cmd.clearDepth)
    {
        size_t depthBuffLenght = mem.framebuffer.width * mem.framebuffer.height;
        for (size_t i = 0; i < depthBuffLenght; i++)
        {
            mem.framebuffer.depth[i] = cmd.depth;
        }
    }
}

//! [gpu_execute]
void gpu_execute(GPUMemory&mem,CommandBuffer &cb){
  (void)mem;
  (void)cb;
  /// \todo Tato funkce reprezentuje funkcionalitu grafické karty.<br>
  /// Měla by umět zpracovat command buffer, čistit framebuffer a kresli.<br>
  /// mem obsahuje paměť grafické karty.
  /// cb obsahuje command buffer pro zpracování.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
  
  uint32_t drawID = 0;

  for (uint32_t i = 0; i < cb.nofCommands; ++i) {
      CommandType type = cb.commands[i].type;
      CommandData data = cb.commands[i].data;
      if (type == CommandType::CLEAR)
          clear(mem, data.clearCommand);
      if (type == CommandType::DRAW)
      {
          draw(mem, data.drawCommand, drawID);
          drawID++;
      }
  }
}
//! [gpu_execute]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
  if(!texture.data)return glm::vec4(0.f);
  auto uv1 = glm::fract(uv);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  //auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  for(uint32_t c=0;c<texture.channels;++c)
    color[c] = texture.data[(pix.y*texture.width+pix.x)*texture.channels+c]/255.f;
  return color;
}

