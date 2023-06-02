/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <cstring>

struct Triangle {
    OutVertex points[3];
};

void readAttributes(Attribute& vertexAtrrib, GPUMemory& mem, VertexAttrib& attrib, uint32_t vertexId)
{
    if (attrib.type == AttributeType::EMPTY) return;

    const void* attribBuffer = mem.buffers[attrib.bufferID].data;
    unsigned char* bytePtr = (unsigned char*) attribBuffer;
    bytePtr += attrib.offset + attrib.stride * vertexId;

    switch (attrib.type)
    {
        case AttributeType::UVEC2:
            vertexAtrrib.u2 = *((glm::uvec2*)(bytePtr));
            break;
        case AttributeType::UVEC3:
            vertexAtrrib.u3 = *((glm::uvec3*)(bytePtr));
            break;
        case AttributeType::UVEC4:
            vertexAtrrib.u4 = *((glm::uvec4*)(bytePtr));
            break;
        case AttributeType::VEC4:
            vertexAtrrib.v4 = *((glm::vec4*)(bytePtr));
            break;
        case AttributeType::VEC3:
            vertexAtrrib.v3 = *((glm::vec3*)(bytePtr));
            break;
        case AttributeType::VEC2:
            vertexAtrrib.v2 = *((glm::vec2*)(bytePtr));
            break;
        case AttributeType::FLOAT:
            vertexAtrrib.v1 = *((float*)(bytePtr));
            break;
        default:
            // its UINT
            vertexAtrrib.u1 = *((uint32_t*)(bytePtr));
            break;
    }
}

uint32_t computeVertexID(GPUMemory& mem, VertexArray const& vao, uint32_t shaderInvocation)
{
    if (vao.indexBufferID < 0) return shaderInvocation;

    uint8_t* ind = (uint8_t*) mem.buffers[vao.indexBufferID].data + vao.indexOffset;
    switch (vao.indexType)
    {
    case IndexType::UINT8:
        return ((uint8_t*)ind)[shaderInvocation];
    case IndexType::UINT16:
        return ((uint16_t*)ind)[shaderInvocation];
    default:
        return ((uint32_t*)ind)[shaderInvocation];
    }      
}

void runVertexAssembly(InVertex& inVertex, GPUMemory& mem, VertexArray& vao, uint32_t shaderInvocation)
{
    inVertex.gl_VertexID = computeVertexID(mem, vao, shaderInvocation);

    for (size_t i = 0; i < maxAttributes; ++i)
    {
        readAttributes(inVertex.attributes[i], mem, vao.vertexAttrib[i], inVertex.gl_VertexID);
    }
}

Triangle primitiveAssembly(GPUMemory& mem, DrawCommand& cmd, uint32_t drawID, uint32_t triangleIndex, ShaderInterface& si, Program& prg)
{
    Triangle triangle;

    uint32_t firstVertexIndex = triangleIndex * 3;
    for (uint32_t i = firstVertexIndex; i < firstVertexIndex + 3; ++i)
    {
        InVertex inVertex;
        OutVertex outVertex;
        inVertex.gl_DrawID = drawID;

        runVertexAssembly(inVertex, mem, cmd.vao, i);

        prg.vertexShader(outVertex, inVertex, si);
        triangle.points[i % 3] = outVertex;
    }

    return triangle;
}

void perspectiveDivision(Triangle& triangle)
{
    for (size_t i = 0; i < 3; ++i)
    {
        float w = triangle.points[i].gl_Position.w;
        triangle.points[i].gl_Position.x /= w;
        triangle.points[i].gl_Position.y /= w;
        triangle.points[i].gl_Position.z /= w;
    }
}

void viewportTransformation(Triangle& triangle, uint32_t halfWidth, uint32_t halfHeight)
{
    for (size_t i = 0; i < 3; ++i)
    {
        triangle.points[i].gl_Position.x = (triangle.points[i].gl_Position.x + 1.0f) * halfWidth;
        triangle.points[i].gl_Position.y = (triangle.points[i].gl_Position.y + 1.0f) * halfHeight;
    }
}

void loadAttributesToFragment(InFragment& inFragment, OutVertex& p1, OutVertex& p2, AttributeType type, size_t i, float l0, float l1)
{
    // dont interpolate integer attribs
    switch (type)
    {
        case AttributeType::UINT:
            inFragment.attributes[i].u1 = p1.attributes[i].u1;
            return;
        case AttributeType::UVEC2:
            inFragment.attributes[i].u2 = p1.attributes[i].u2;
            return;
        case AttributeType::UVEC3:
            inFragment.attributes[i].u3 = p1.attributes[i].u3;
            return;
        case AttributeType::UVEC4:
            inFragment.attributes[i].u4 = p1.attributes[i].u4;
            return;
        default:
            break;
    }

    // interpolate
    float s = (float) (l0 / p1.gl_Position.w) + (l1 / p2.gl_Position.w);
    l0 /= p1.gl_Position.w * s;
    l1 /= p2.gl_Position.w * s;

    switch (type)
    {
        case AttributeType::VEC4:
            for (int vecI = 0; vecI < 4; vecI++){
                inFragment.attributes[i].v4[vecI] = (float) ((p1.attributes[i].v4[vecI] * l0) + (p2.attributes[i].v4[vecI] * l1));
            }
            break;

        case AttributeType::VEC3:
            for (int vecI = 0; vecI < 3; vecI++){
                inFragment.attributes[i].v3[vecI] = (float) ((p1.attributes[i].v3[vecI] * l0) + (p2.attributes[i].v3[vecI] * l1));
            }
            break;
        case AttributeType::VEC2:
            for (int vecI = 0; vecI < 2; vecI++){
                inFragment.attributes[i].v2[vecI] = (float) ((p1.attributes[i].v2[vecI] * l0) + (p2.attributes[i].v2[vecI] * l1));
            }
            break;
        default:
            // its FLOAT
            inFragment.attributes[i].v1 = (float) ((p1.attributes[i].v1 * l0) + (p2.attributes[i].v1 * l1));
            break;
    }
}

void perFragmentOperations(Frame& framebuffer, OutFragment& outFragment, float depth, float x, float y)
{
    int pixelPos = (int)(x + y * framebuffer.width);

    if (depth >= framebuffer.depth[pixelPos])
    {
        // discard fragment
        return;
    }

    glm::vec4 color = outFragment.gl_FragColor;
    color.r = glm::clamp(color.r, 0.f, 1.f);    // r
    color.g = glm::clamp(color.g, 0.f, 1.f);    // g
    color.b = glm::clamp(color.b, 0.f, 1.f);    // b
    color.a = glm::clamp(color.a, 0.f, 1.f);    // alpha

    int pos = pixelPos * framebuffer.channels;

    // update depth only if alpha is > 0.5f
    if (color.a > 0.5f)
    {
        framebuffer.depth[pixelPos] = depth;
    }

    framebuffer.color[pos + 0] = (uint8_t) ((framebuffer.color[pos + 0] * (1.f - color.a)) + ((color.r * 255.f) * color.a));
    framebuffer.color[pos + 1] = (uint8_t) ((framebuffer.color[pos + 1] * (1.f - color.a)) + ((color.g * 255.f) * color.a));
    framebuffer.color[pos + 2] = (uint8_t) ((framebuffer.color[pos + 2] * (1.f - color.a)) + ((color.b * 255.f) * color.a));
    framebuffer.color[pos + 3] = (uint8_t) color.a;
}

void loadFragmentToShader(float x, float y, Frame& frame, Program& prg, ShaderInterface& si, OutVertex& p1, OutVertex& p2, float l0, float l1)
{
    float depth = p1.gl_Position.z * l0 + p2.gl_Position.z * l1;

    InFragment inFragment;
    inFragment.gl_FragCoord.x = x;
    inFragment.gl_FragCoord.y = y;
    inFragment.gl_FragCoord.z = depth;

    for (size_t i = 0; i < maxAttributes; i++)
    {
        if (prg.vs2fs[i] != AttributeType::EMPTY)
            loadAttributesToFragment(inFragment, p1, p2, prg.vs2fs[i], i, l0, l1);
    }

    OutFragment outFragment;
    prg.fragmentShader(outFragment, inFragment, si);

    perFragmentOperations(frame, outFragment, depth, x ,y);

}

float edgeFunction(const OutVertex& a, const OutVertex& b, const OutVertex& c)
{
    return (c.gl_Position.x - a.gl_Position.x) * (b.gl_Position.y - a.gl_Position.y) - (c.gl_Position.y - a.gl_Position.y) * (b.gl_Position.x - a.gl_Position.x);
}

bool isFacingCamera(glm::vec3& cameraPos, glm::vec3& vertexNorm)
{
    return glm::dot(vertexNorm, cameraPos) > 0;
}

void drawLine(OutVertex& v1, OutVertex& v2, Frame& frame, Program& prg, ShaderInterface& si, glm::vec2& min, glm::vec2& max)
{
    int x1 = (int) v1.gl_Position.x;
    int y1 = (int) v1.gl_Position.y;
    int x2 = (int) v2.gl_Position.x;
    int y2 = (int) v2.gl_Position.y;

    // DDA algorithm
    float dy = y2 - y1;
    float dx = x2 - x1;

    if (dx < 0)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    if (abs(dx) < abs(dy))
    {
        std::swap(dx, dy);
        float k = dy / dx;
        int y = x1;
        float ratioInc = 1.f / abs(dx);
        float l0 = 0.f;

        for (int x = y1; x <= y2; ++x)
        {
            if (!(y >= max.x || x >= max.y || y < min.x || x < min.y))
            {
                loadFragmentToShader(y, x, frame, prg, si, v1, v2, 1.f - l0, l0);
            }
            l0 += ratioInc;
            y += (int) k;
        }
    }
    else 
    {
        float k = dy / dx;
        int y = y1;
        float ratioInc = 1.f / abs(dx);
        float l0 = 0.f;

        for (int x = x1; x <= x2; ++x)
        {
            if (!(x >= max.x || y >= max.y || x < min.x || y < min.y))
            {
                loadFragmentToShader(x, y, frame, prg, si, v1, v2, l0, 1.f - l0);
            }
            l0 += ratioInc;
            y += (int) k;
        }
    } 
}

void drawBresLine(int decide, OutVertex& v1, OutVertex& v2, Frame& frame, Program& prg, ShaderInterface& si, glm::vec2& min, glm::vec2& max)
{
    int x = ceilf(v1.gl_Position.x);
    int y = ceilf(v1.gl_Position.y);
    int x2 = ceilf(v2.gl_Position.x);
    int y2 = ceilf(v2.gl_Position.y);

    float l0 = 0.f;

    bool yLonger = false;
    int incrementVal, endVal;
    int shortLen = y2 - y;
    int longLen = x2 - x;
    if (abs(shortLen) > abs(longLen)) {
        int swap = shortLen;
        shortLen = longLen;
        longLen = swap;
        yLonger = true;
    }
    endVal = longLen;
    if (longLen < 0) {
        incrementVal = -1;
        longLen = -longLen;
    }
    else incrementVal = 1;
    int decInc;
    if (longLen == 0) decInc = 0;
    else decInc = (shortLen << 16) / longLen;
    int j = 0;
    if (yLonger) {
        for (int i = 0; i != endVal; i += incrementVal) {
            x = x + (j >> 16);
            y += i;
            if (!(y >= max.x || x >= max.y || y < min.x || x < min.y))
            {
                loadFragmentToShader(x, y, frame, prg, si, v1, v2, 1.f - l0, l0);
            }
            j += decInc;
        }
    }
    else {
        for (int i = 0; i != endVal; i += incrementVal) {
            x += i;
            y = y + (j >> 16);

            if (!(y >= max.x || x >= max.y || y < min.x || x < min.y))
            {
                loadFragmentToShader(x, y, frame, prg, si, v1, v2, 1.f - l0, l0);
            }
            j += decInc;
        }
    }
}

void rasterize(Triangle& triangle, DrawCommand& cmd, ShaderInterface& si, Frame& frame, Program& prg, glm::vec3& cameraVec)
{
    // check if all points of the triangle are same
    if (triangle.points[0].gl_Position == triangle.points[1].gl_Position && triangle.points[1].gl_Position == triangle.points[2].gl_Position)
        return;

    // check if triangle is facing a camera, if not discard it.
    if (cmd.backfaceCulling && !isFacingCamera(cameraVec, triangle.points[0].attributes[1].v3))
        return;

    // check if triangle is cw or ccw
    bool clockwise = true;
    if (edgeFunction(triangle.points[0], triangle.points[1], triangle.points[2]) < 0)
        clockwise = false;

    if (clockwise && cmd.backfaceCulling)
        return;

    glm::vec2 max = glm::vec2();
    glm::vec2 min = glm::vec2();
    ////////////////////////////////////////////////////////////////
    // bounding box
    max.x = glm::min(triangle.points[0].gl_Position.x, glm::min(triangle.points[1].gl_Position.x, triangle.points[2].gl_Position.x));
    min.y = glm::min(triangle.points[0].gl_Position.y, glm::min(triangle.points[1].gl_Position.y, triangle.points[2].gl_Position.y));
    max.x = glm::max(triangle.points[0].gl_Position.x, glm::max(triangle.points[1].gl_Position.x, triangle.points[2].gl_Position.x));
    max.y = glm::max(triangle.points[0].gl_Position.y, glm::max(triangle.points[1].gl_Position.y, triangle.points[2].gl_Position.y));

    min.x = (int) glm::max(min.x, 0.f);
    min.y = (int) glm::max(min.y, 0.f);
    max.x = (float) ((int) (glm::min(max.x + 1.f, ((float) frame.width))));
    max.y = (float) ((int) (glm::min(max.y + 1.f, ((float) frame.height))));
    ////////////////////////////////////////////////////////////////

    drawBresLine(0, triangle.points[0], triangle.points[1], frame, prg, si, min, max);
    drawBresLine(0, triangle.points[0], triangle.points[2], frame, prg, si, min, max);
    drawBresLine(0, triangle.points[2], triangle.points[1], frame, prg, si, min, max);
}

void cutEdge(OutVertex& a, OutVertex& b, Program& prg)
{
    float t = (-a.gl_Position.w - a.gl_Position.z) / (b.gl_Position.w - a.gl_Position.w + b.gl_Position.z - a.gl_Position.z);
    
    a.gl_Position += t * (b.gl_Position - a.gl_Position);

    for (uint32_t i = 0; i < maxAttributes; i++)
    {
        switch (prg.vs2fs[i])
        {
            case AttributeType::FLOAT:
                a.attributes[i].v1 += t * (b.attributes[i].v1 - a.attributes[i].v1);
                continue;
            case AttributeType::VEC2:
                a.attributes[i].v2 += t * (b.attributes[i].v2 - a.attributes[i].v2);
                continue;
            case AttributeType::VEC3:
                a.attributes[i].v3 += t * (b.attributes[i].v3 - a.attributes[i].v3);
                continue;
            case AttributeType::VEC4:
                a.attributes[i].v4 += t * (b.attributes[i].v4 - a.attributes[i].v4);
                continue;
            case AttributeType::UINT:
                a.attributes[i].u1 += (uint32_t) t * (b.attributes[i].u1 - a.attributes[i].u1);
                continue;
            case AttributeType::UVEC2:
                a.attributes[i].u2 += (uint32_t) t * (b.attributes[i].u2 - a.attributes[i].u2);
                continue;
            case AttributeType::UVEC3:
                a.attributes[i].u3 += (uint32_t) t * (b.attributes[i].u3 - a.attributes[i].u3);
                continue;
            case AttributeType::UVEC4:
                a.attributes[i].u4 += (uint32_t) t * (b.attributes[i].u4 - a.attributes[i].u4);
                continue;
            default:
                break;
        }
    }
}

int clipping(Triangle& triangle, Triangle& secondTriangle, Program& prg)
{
    bool isInsideCameraMask[3];
    isInsideCameraMask[0] = (-triangle.points[0].gl_Position.w) > triangle.points[0].gl_Position.z;
    isInsideCameraMask[1] = (-triangle.points[1].gl_Position.w) > triangle.points[1].gl_Position.z;
    isInsideCameraMask[2] = (-triangle.points[2].gl_Position.w) > triangle.points[2].gl_Position.z;

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

    // CLIPPING NEEDED
    if (!isInsideCameraMask[0] && isInsideCameraMask[1] && isInsideCameraMask[2])
    {
        // Point 0 OK (is not inside)
        cutEdge(triangle.points[1], triangle.points[0], prg);  // change point 1
        cutEdge(triangle.points[2], triangle.points[0], prg);  // change point 2
        return 1;
    }
    else if (isInsideCameraMask[0] && !isInsideCameraMask[1] && isInsideCameraMask[2])
    {
        // Point 1 OK (is not inside)
        cutEdge(triangle.points[0], triangle.points[1], prg);  // change point 0
        cutEdge(triangle.points[2], triangle.points[1], prg);  // change point 2
        return 1;
    }
    else if (isInsideCameraMask[0] && isInsideCameraMask[1] && !isInsideCameraMask[2])
    {
        // Point 2 OK (is not inside)
        cutEdge(triangle.points[0], triangle.points[2], prg);  // change point 0
        cutEdge(triangle.points[1], triangle.points[2], prg);  // change point 1
        return 1;
    }
    else if (isInsideCameraMask[0])
    {
        // Points 1, 2 OK (not inside)
        memcpy(&secondTriangle, &triangle, sizeof(Triangle));                 // make a copy of a triangle

        cutEdge(triangle.points[0], triangle.points[1], prg);             // change point 0
        cutEdge(secondTriangle.points[0], secondTriangle.points[2], prg); // change point 0 of second triangle
        triangle.points[2] = secondTriangle.points[0];                    // change point of first triangle to second intersection
        return 2;
    }
    else if (isInsideCameraMask[1])
    {
        // Points 0, 2 OK (not inside)
        memcpy(&secondTriangle, &triangle, sizeof(Triangle));                 // make a copy of a triangle

        cutEdge(triangle.points[1], triangle.points[0], prg);              // change point 1
        cutEdge(secondTriangle.points[1], secondTriangle.points[2], prg);  // change point 1 of second triangle
        triangle.points[2] = secondTriangle.points[1];                           // change point of first triangle to second intersection
        return 2;
    }
    else
    {
        // Points 0, 1 OK (not inside)
        memcpy(&secondTriangle, &triangle, sizeof(Triangle));                 // make a copy of a triangle

        cutEdge(triangle.points[2], triangle.points[0], prg);             // change point 2
        cutEdge(secondTriangle.points[2], secondTriangle.points[1], prg); // change point 2 of second triangle
        triangle.points[1] = secondTriangle.points[2];                    // change point of first triangle to second intersection
        return 2;
    }
}

void draw(GPUMemory& mem, DrawCommand& cmd, uint32_t drawID) 
{
    ShaderInterface si;
    si.uniforms = mem.uniforms;
    si.textures = mem.textures;

    Frame     frame     = mem.framebuffer;
    Program   prg       = mem.programs[cmd.programID];
    glm::vec3 cameraVec = mem.uniforms[2].v3;

    uint32_t halfWidth = mem.framebuffer.width >> 1;
    uint32_t halfHeight = mem.framebuffer.height >> 1;

    for (uint32_t triangleIndex = 0; triangleIndex < cmd.nofVertices / 3; triangleIndex++)
    {
        Triangle triangle = primitiveAssembly(mem, cmd, drawID, triangleIndex, si, prg);
        Triangle secondTriangle;

        switch (clipping(triangle, secondTriangle, prg))
        {
            case 0:
                // dont rasterize this triangle
                continue;
            case 1:
                // one triangle to rasterize
                break;
            case 2:
                // two triangles to rasterize
                perspectiveDivision(secondTriangle);
                viewportTransformation(secondTriangle, halfWidth, halfHeight);
                rasterize(secondTriangle, cmd, si, frame, prg, cameraVec);
                break;
        }

        perspectiveDivision(triangle);
        viewportTransformation(triangle, halfWidth, halfHeight);
        rasterize(triangle, cmd, si, frame, prg, cameraVec);
    }
}

void clear(GPUMemory& mem, ClearCommand& cmd) {
    if (cmd.clearColor) {
        uint32_t combinedValue = 0;
        combinedValue |= ((uint32_t)(cmd.color.r * 255.f));
        combinedValue |= ((uint32_t)(cmd.color.g * 255.f)) << 8;
        combinedValue |= ((uint32_t)(cmd.color.b * 255.f)) << 16;
        combinedValue |= ((uint32_t)(cmd.color.a * 255.f)) << 24;

        uint32_t* arr = reinterpret_cast<uint32_t*>(mem.framebuffer.color);

        size_t colorBuffLenght = mem.framebuffer.width * mem.framebuffer.height;
        std::fill_n(arr, colorBuffLenght, combinedValue);
    }
    if (cmd.clearDepth)
    {
        std::fill_n(mem.framebuffer.depth, mem.framebuffer.width * mem.framebuffer.height, cmd.depth);
    }
}

//! [gpu_execute]
void gpu_execute(GPUMemory& mem, CommandBuffer& cb){
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
glm::vec4 read_texture(Texture const& texture, glm::vec2 uv){
  if(!texture.data) 
      return glm::vec4(0.f);

  const glm::vec2 uv1 = glm::fract(uv);
  const glm::vec2 uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  const glm::uvec2 pix = glm::uvec2(uv2);

  //auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  uint32_t index = (pix.y * texture.width + pix.x) * texture.channels;

  for(uint8_t c = 0; c < texture.channels; ++c)
    color[c] = texture.data[index + c]/255.f;

  return color;
}

