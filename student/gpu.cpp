/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>

struct Triangle {
    OutVertex points[3];
};

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
    else if (attrib.type == AttributeType::UVEC3)
    {
        glm::uvec3 *ind = (glm::uvec3*)(bytePtr);
        vertexAtrrib->u3 = *ind;
    }
    else if (attrib.type == AttributeType::UVEC4)
    {
        glm::uvec4* ind = (glm::uvec4*)(bytePtr);
        vertexAtrrib->u4 = *ind;
    }
    else if (attrib.type == AttributeType::VEC4)
    {
        glm::vec4* ind = (glm::vec4*)(bytePtr);
        vertexAtrrib->v4 = *ind;
    }
    else if (attrib.type == AttributeType::VEC3)
    {
        glm::vec3* ind = (glm::vec3*)(bytePtr);
        vertexAtrrib->v3 = *ind;
    }
    else if (attrib.type == AttributeType::VEC2)
    {
        glm::vec2* ind = (glm::vec2*)(bytePtr);
        vertexAtrrib->v2 = *ind;
    }
    else if (attrib.type == AttributeType::FLOAT)
    {
        float* ind = (float*)(bytePtr);
        vertexAtrrib->v1 = *ind;
    }
    else if (attrib.type == AttributeType::UINT)
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

Triangle primitiveAssembly(GPUMemory& mem, DrawCommand cmd, uint32_t drawID, uint32_t triangleIndex)
{
    Program prg = mem.programs[cmd.programID];
    Triangle triangle;

    uint32_t firstVertexIndex = triangleIndex * 3;
    for (size_t i = firstVertexIndex; i < firstVertexIndex + 3; i++)
    {
        InVertex inVertex;
        OutVertex outVertex;
        inVertex.gl_DrawID = drawID;

        runVertexAssembly(&inVertex, mem, cmd.vao, i);

        ShaderInterface si;
        si.uniforms = mem.uniforms;
        si.textures = mem.textures;

        prg.vertexShader(outVertex, inVertex, si);
        triangle.points[i % 3] = outVertex;
    }

    return triangle;
}

void perspectiveDivision(Triangle* triangle)
{
    for (size_t i = 0; i < 3; i++)
    {
        float w = triangle->points[i].gl_Position.w;
        triangle->points[i].gl_Position.x /= w;
        triangle->points[i].gl_Position.y /= w;
        triangle->points[i].gl_Position.z /= w;
    }
}

void viewportTransformation(Triangle* triangle, uint32_t width, uint32_t height)
{
    for (size_t i = 0; i < 3; i++)
    {
        triangle->points[i].gl_Position.x = (triangle->points[i].gl_Position.x + 1.0f) * width * 0.5f;
        triangle->points[i].gl_Position.y = (triangle->points[i].gl_Position.y + 1.0f) * height * 0.5f;
    }
}

void loadAttributesToFragment(InFragment* inFragment, OutVertex p1, OutVertex p2, OutVertex p3, Program prg, size_t i, double l0, double l1, double l2)
{
    if (prg.vs2fs[i] == AttributeType::UINT)
    {
        inFragment->attributes[i].u1 = p1.attributes[i].u1;
    }
    else if (prg.vs2fs[i] == AttributeType::UVEC2)
    {
        inFragment->attributes[i].u2 = p1.attributes[i].u2;
    }
    else if (prg.vs2fs[i] == AttributeType::UVEC3)
    {
        inFragment->attributes[i].u3 = p1.attributes[i].u3;
    }
    else if (prg.vs2fs[i] == AttributeType::UVEC4)
    {
        inFragment->attributes[i].u4 = p1.attributes[i].u4;
    }

    // interpolate
    double s = (l0 / p1.gl_Position.w) + (l1 / p2.gl_Position.w) + (l2 / p3.gl_Position.w);
    l0 /= p1.gl_Position.w * s;
    l1 /= p2.gl_Position.w * s;
    l2 /= p3.gl_Position.w * s;

    if (prg.vs2fs[i] == AttributeType::VEC4)
    {
        for (size_t vecI = 0; vecI < 4; vecI++)
        {
            inFragment->attributes[i].v4[vecI] = (p1.attributes[i].v4[vecI] * l0) + (p2.attributes[i].v4[vecI] * l1) + (p3.attributes[i].v4[vecI] * l2);
        }
    }
    else if (prg.vs2fs[i] == AttributeType::VEC3)
    {
        for (size_t vecI = 0; vecI < 3; vecI++)
        {
            inFragment->attributes[i].v3[vecI] = (p1.attributes[i].v3[vecI] * l0) + (p2.attributes[i].v3[vecI] * l1) + (p3.attributes[i].v3[vecI] * l2);
        }
    }
    else if (prg.vs2fs[i] == AttributeType::VEC2)
    {
        for (size_t vecI = 0; vecI < 2; vecI++)
        {
            inFragment->attributes[i].v2[vecI] = (p1.attributes[i].v2[vecI] * l0) + (p2.attributes[i].v2[vecI] * l1) + (p3.attributes[i].v2[vecI] * l2);
        }
    }
    else if (prg.vs2fs[i] == AttributeType::FLOAT)
    {
        inFragment->attributes[i].v1 = 10.f;
    }
}

float triangleArea2D(float dX0, float dY0, float dX1, float dY1, float dX2, float dY2)
{
    float dArea = ((dX1 - dX0) * (dY2 - dY0) - (dX2 - dX0) * (dY1 - dY0)) / 2.0;
    return (dArea > 0.0) ? dArea : -dArea;
}

void perFragmentOperations(Frame framebuffer, OutFragment outFragment, float depth, float x, float y)
{
    x = x - 0.5f;
    y = y - 0.5f;

    int pixelPos = (x + y * framebuffer.width);

    if (depth >= framebuffer.depth[pixelPos])
    {
        //printf("DISCARD (%f, %f)\n", depth, framebuffer.depth[pixelPos]);
        // discard fragment
        return;
    }

    glm::vec4 color = outFragment.gl_FragColor;
    color.x = glm::clamp(color.x, 0.f, 1.f);    // r
    color.y = glm::clamp(color.y, 0.f, 1.f);    // g
    color.z = glm::clamp(color.z, 0.f, 1.f);    // b
    color.w = glm::clamp(color.w, 0.f, 1.f);    // alpha

    int pos = pixelPos * framebuffer.channels;

    // update depth only if alpha is > 0.5f
    if (color.w > 0.5f)
    {
        framebuffer.depth[pixelPos] = depth;
    }

    framebuffer.color[pos + 0] = (uint8_t) ((framebuffer.color[pos + 0] * (1.f - color.w)) + ((color.x * 255.f) * color.w));
    framebuffer.color[pos + 1] = (uint8_t) ((framebuffer.color[pos + 1] * (1.f - color.w)) + ((color.y * 255.f) * color.w));
    framebuffer.color[pos + 2] = (uint8_t) ((framebuffer.color[pos + 2] * (1.f - color.w)) + ((color.z * 255.f) * color.w));
    framebuffer.color[pos + 3] = (uint8_t) color.w;
}

void loadFragmentToShader(Frame frame, float x, float y, Program prg, ShaderInterface si, OutVertex p1, OutVertex p2, OutVertex p3)
{
    float area = triangleArea2D(p3.gl_Position.x, p3.gl_Position.y, p2.gl_Position.x, p2.gl_Position.y, p1.gl_Position.x, p1.gl_Position.y);
    float l0 = triangleArea2D(p3.gl_Position.x, p3.gl_Position.y, p2.gl_Position.x, p2.gl_Position.y, x, y) / area;
    float l1 = triangleArea2D(p3.gl_Position.x, p3.gl_Position.y, p1.gl_Position.x, p1.gl_Position.y, x, y) / area;
    //float l2 = triangleArea2D(p2.gl_Position.x, p2.gl_Position.y, p1.gl_Position.x, p1.gl_Position.y, x, y) / area;
    float l2 = 1.f - (l0 + l1);
    float depth = p1.gl_Position.z * l0 + p2.gl_Position.z * l1 + p3.gl_Position.z * l2;

    InFragment inFragment;
    inFragment.gl_FragCoord.x = x;
    inFragment.gl_FragCoord.y = y;
    inFragment.gl_FragCoord.z = depth;

    for (size_t i = 0; i < maxAttributes; i++)
    {
        if (prg.vs2fs[i] != AttributeType::EMPTY)
            loadAttributesToFragment(&inFragment, p1, p2, p3, prg, i, l0, l1, l2);
    }

    OutFragment outFragrament;
    prg.fragmentShader(outFragrament, inFragment, si);

    perFragmentOperations(frame, outFragrament, depth, x ,y);

}

void rasterize(GPUMemory& mem, Triangle* triangle, DrawCommand cmd)
{
    Frame frame = mem.framebuffer;
    Program prg = mem.programs[cmd.programID];
    // bounding box
    float min_x = glm::min(triangle->points[0].gl_Position.x, glm::min(triangle->points[1].gl_Position.x, triangle->points[2].gl_Position.x));
    float min_y = glm::min(triangle->points[0].gl_Position.y, glm::min(triangle->points[1].gl_Position.y, triangle->points[2].gl_Position.y));

    float max_x = glm::max(triangle->points[0].gl_Position.x, glm::max(triangle->points[1].gl_Position.x, triangle->points[2].gl_Position.x));
    float max_y = glm::max(triangle->points[0].gl_Position.y, glm::max(triangle->points[1].gl_Position.y, triangle->points[2].gl_Position.y));

    min_x = glm::max(min_x, 0.f);
    min_y = glm::max(min_y, 0.f);
    max_x = glm::min(max_x, (float) (frame.width));
    max_y = glm::min(max_y, (float) (frame.height));

    // point[1] - point[0]
    glm::vec2 dirVec1 = glm::vec2(triangle->points[1].gl_Position.x - triangle->points[0].gl_Position.x, triangle->points[1].gl_Position.y - triangle->points[0].gl_Position.y);
    // point[2] - point[1]
    glm::vec2 dirVec2 = glm::vec2(triangle->points[2].gl_Position.x - triangle->points[1].gl_Position.x, triangle->points[2].gl_Position.y - triangle->points[1].gl_Position.y);
    // point[0] - point[2]
    glm::vec2 dirVec3 = glm::vec2(triangle->points[0].gl_Position.x - triangle->points[2].gl_Position.x, triangle->points[0].gl_Position.y - triangle->points[2].gl_Position.y);

    // E1 = (min_y - point[0].y) * dirVec1.x - (min_x - point[0].x) * dirVec1.y
    int32_t E1 = (min_y - triangle->points[0].gl_Position.y) * dirVec1.x - (min_x - triangle->points[0].gl_Position.x) * dirVec1.y;

    // E2 = (min_y - point[1].y) * dirVec2.x - (min_x - point[1].x) * dirVec2.y
    int32_t E2 = (min_y - triangle->points[1].gl_Position.y) * dirVec2.x - (min_x - triangle->points[1].gl_Position.x) * dirVec2.y;

    // E3 = (min_y - point[2].y) * dirVec3.x - (min_x - point[2].x) * dirVec3.y
    int32_t E3 = (min_y - triangle->points[2].gl_Position.y) * dirVec3.x - (min_x - triangle->points[2].gl_Position.x) * dirVec3.y;

    ShaderInterface si;
    si.uniforms = mem.uniforms;
    si.textures = mem.textures;

    for (float y = min_y + 0.5f; y < max_y; y++)
    {
        int32_t t1 = E1;
        int32_t t2 = E2;
        int32_t t3 = E3;

        for (float x = min_x + 0.5f; x < max_x; x++)
        {
            // check for CCW triangles 
            // scuffed >= because of tests
            if (t1 > 0 && t2 > 0 && t3 >= 0)
            {
                loadFragmentToShader(frame, x, y, prg, si, triangle->points[0], triangle->points[1], triangle->points[2]);
            }
            else if (!cmd.backfaceCulling)
            {
                // check for CW triangles
                if (t1 <= 0 && t2 <= 0 && t3 <= 0)
                {
                    loadFragmentToShader(frame, x, y, prg, si, triangle->points[0], triangle->points[1], triangle->points[2]);
                }
            }
                
            t1 -= dirVec1.y;
            t2 -= dirVec2.y;
            t3 -= dirVec3.y;
        }

        E1 += dirVec1.x;
        E2 += dirVec2.x;
        E3 += dirVec3.x;
    }
}

int clipping(Triangle* triangle)
{
    bool isInsideCameraMask[3];
    isInsideCameraMask[0] = (-triangle->points[0].gl_Position.w) > triangle->points[0].gl_Position.z;
    isInsideCameraMask[1] = (-triangle->points[1].gl_Position.w) > triangle->points[1].gl_Position.z;
    isInsideCameraMask[2] = (-triangle->points[2].gl_Position.w) > triangle->points[2].gl_Position.z;

    if (isInsideCameraMask[0] && isInsideCameraMask[1] && isInsideCameraMask[2])
    {
        // whole triangle is inside of camera, discard it.
        return 0;
    }
    else if (!isInsideCameraMask[0] && !isInsideCameraMask[1] && !isInsideCameraMask[2])
    {
        // whole triangle is outside of camera, no clipping needed.
        return 1;
    }
    // need to clip
    return 2;
}

void draw(GPUMemory& mem, DrawCommand cmd, uint32_t drawID) 
{
    for (size_t triangleIndex = 0; triangleIndex < cmd.nofVertices / 3; triangleIndex++)
    {
        Triangle triangle = primitiveAssembly(mem, cmd, drawID, triangleIndex);

        switch (clipping(&triangle))
        {
            case 0:
                // dont rasterize this triangle
                continue;
            case 1:
                // one triangle to rasterize
                break;
            case 2:
                // two triangles to rasterize
                break;
        }

        perspectiveDivision(&triangle);
        viewportTransformation(&triangle, mem.framebuffer.width, mem.framebuffer.height);
        rasterize(mem, &triangle, cmd);
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

