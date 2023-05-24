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

void readAttributes(Attribute* vertexAtrrib, GPUMemory& mem, VertexAttrib attrib, uint32_t vertexId)
{
    if (attrib.type == AttributeType::EMPTY) return;

    const void* attribBuffer = mem.buffers[attrib.bufferID].data;
    unsigned char* bytePtr = reinterpret_cast<unsigned char*>(const_cast<void*>(attribBuffer));
    bytePtr += attrib.offset + attrib.stride * vertexId;

    switch (attrib.type)
    {
        case AttributeType::UVEC2:
            vertexAtrrib->u2 = *((glm::uvec2*)(bytePtr));
            break;
        case AttributeType::UVEC3:
            vertexAtrrib->u3 = *((glm::uvec3*)(bytePtr));
            break;
        case AttributeType::UVEC4:
            vertexAtrrib->u4 = *((glm::uvec4*)(bytePtr));
            break;
        case AttributeType::VEC4:
            vertexAtrrib->v4 = *((glm::vec4*)(bytePtr));
            break;
        case AttributeType::VEC3:
            vertexAtrrib->v3 = *((glm::vec3*)(bytePtr));
            break;
        case AttributeType::VEC2:
            vertexAtrrib->v2 = *((glm::vec2*)(bytePtr));
            break;
        case AttributeType::FLOAT:
            vertexAtrrib->v1 = *((float*)(bytePtr));
            break;
        default:
            // its UINT
            vertexAtrrib->u1 = *((uint32_t*)(bytePtr));
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

void runVertexAssembly(InVertex* inVertex, GPUMemory& mem, VertexArray const& vao, uint32_t shaderInvocation)
{
    inVertex->gl_VertexID = computeVertexID(mem, vao, shaderInvocation);

    for (size_t i = 0; i < maxAttributes; i++)
    {
        readAttributes(&inVertex->attributes[i], mem, vao.vertexAttrib[i], inVertex->gl_VertexID);
    }
}

Triangle primitiveAssembly(GPUMemory& mem, DrawCommand cmd, uint32_t drawID, uint32_t triangleIndex)
{
    Program prg = mem.programs[cmd.programID];
    Triangle triangle;

    uint32_t firstVertexIndex = triangleIndex * 3;
    for (uint32_t i = firstVertexIndex; i < firstVertexIndex + 3; i++)
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
        triangle->points[i].gl_Position.x = ((triangle->points[i].gl_Position.x + 1.0f) / 2.f) * width;
        triangle->points[i].gl_Position.y = ((triangle->points[i].gl_Position.y + 1.0f) / 2.f) * height;
    }
}

void loadAttributesToFragment(InFragment* inFragment, OutVertex p1, OutVertex p2, OutVertex p3, Program prg, size_t i, double l0, float l1, float l2)
{
    // dont interpolate integer attribs
    switch (prg.vs2fs[i])
    {
        case AttributeType::UINT:
            inFragment->attributes[i].u1 = p1.attributes[i].u1;
            return;
        case AttributeType::UVEC2:
            inFragment->attributes[i].u2 = p1.attributes[i].u2;
            return;
        case AttributeType::UVEC3:
            inFragment->attributes[i].u3 = p1.attributes[i].u3;
            return;
        case AttributeType::UVEC4:
            inFragment->attributes[i].u4 = p1.attributes[i].u4;
            return;
        default:
            break;
    }

    // interpolate
    float s = (l0 / p1.gl_Position.w) + (l1 / p2.gl_Position.w) + (l2 / p3.gl_Position.w);
    l0 /= p1.gl_Position.w * s;
    l1 /= p2.gl_Position.w * s;
    l2 /= p3.gl_Position.w * s;

    switch (prg.vs2fs[i])
    {
        case AttributeType::VEC4:
            for (int vecI = 0; vecI < 4; vecI++){
                inFragment->attributes[i].v4[vecI] = (float) ((p1.attributes[i].v4[vecI] * l0) + (p2.attributes[i].v4[vecI] * l1) + (p3.attributes[i].v4[vecI] * l2));
            }
            break;

        case AttributeType::VEC3:
            for (int vecI = 0; vecI < 3; vecI++){
                inFragment->attributes[i].v3[vecI] = (float) ((p1.attributes[i].v3[vecI] * l0) + (p2.attributes[i].v3[vecI] * l1) + (p3.attributes[i].v3[vecI] * l2));
            }
            break;
        case AttributeType::VEC2:
            for (int vecI = 0; vecI < 2; vecI++){
                inFragment->attributes[i].v2[vecI] = (float) ((p1.attributes[i].v2[vecI] * l0) + (p2.attributes[i].v2[vecI] * l1) + (p3.attributes[i].v2[vecI] * l2));
            }
            break;
        default:
            // its FLOAT
            inFragment->attributes[i].v1 = (float) ((p1.attributes[i].v1 * l0) + (p2.attributes[i].v1 * l1) + (p3.attributes[i].v1 * l2));
            break;
    }
}

void perFragmentOperations(Frame& framebuffer, OutFragment outFragment, float depth, float x, float y)
{
    x = x - 0.5f;
    y = y - 0.5f;

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

void loadFragmentToShader(Frame frame, float x, float y, Program prg, ShaderInterface si, OutVertex p1, OutVertex p2, OutVertex p3, float area, float area2, float area3)
{
    float l0 = area2 / area;
    float l1 = area3 / area;
    float l2 = 1.f - l0 - l1;
    float depth = (float) (p1.gl_Position.z * l0 + p2.gl_Position.z * l1 + p3.gl_Position.z * l2);

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

float edgeFunction(const OutVertex& a, const OutVertex& b, const OutVertex& c)
{
    return (c.gl_Position.x - a.gl_Position.x) * (b.gl_Position.y - a.gl_Position.y) - (c.gl_Position.y - a.gl_Position.y) * (b.gl_Position.x - a.gl_Position.x);
}

bool isFacingCamera(glm::vec3& cameraPos, glm::vec3& vertexNorm)
{
    return glm::dot(vertexNorm, cameraPos) > 0;
}

void rasterize(GPUMemory& mem, Triangle* triangle, DrawCommand cmd)
{
    // check if all points of the triangle are same
    if (triangle->points[0].gl_Position == triangle->points[1].gl_Position && triangle->points[1].gl_Position == triangle->points[2].gl_Position)
        return;

    bool isFacingCam =  isFacingCamera(mem.uniforms[2].v3, triangle->points[0].attributes[1].v3) || 
                        isFacingCamera(mem.uniforms[2].v3, triangle->points[1].attributes[1].v3) || 
                        isFacingCamera(mem.uniforms[2].v3, triangle->points[2].attributes[1].v3);

    // check if triangle is facing a camera, if not discard it.
    if (!isFacingCam && cmd.backfaceCulling)
        return;

    Frame frame = mem.framebuffer;
    Program prg = mem.programs[cmd.programID];

    ////////////////////////////////////////////////////////////////
    // bounding box
    float min_x = glm::min(triangle->points[0].gl_Position.x, glm::min(triangle->points[1].gl_Position.x, triangle->points[2].gl_Position.x));
    float min_y = glm::min(triangle->points[0].gl_Position.y, glm::min(triangle->points[1].gl_Position.y, triangle->points[2].gl_Position.y));

    float max_x = glm::max(triangle->points[0].gl_Position.x, glm::max(triangle->points[1].gl_Position.x, triangle->points[2].gl_Position.x));
    float max_y = glm::max(triangle->points[0].gl_Position.y, glm::max(triangle->points[1].gl_Position.y, triangle->points[2].gl_Position.y));

    min_x = (int) (glm::max(min_x, 0.f)) + 0.5f;
    min_y = (int) (glm::max(min_y, 0.f)) + 0.5f;
    max_x = (int) (glm::min(max_x + 1.f, (float) (frame.width)));
    max_y = (int) (glm::min(max_y + 1.f, (float) (frame.height)));
    ////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////
    // point[1] - point[0]
    glm::vec2 dirVec1 = glm::vec2(triangle->points[1].gl_Position.x - triangle->points[0].gl_Position.x, triangle->points[1].gl_Position.y - triangle->points[0].gl_Position.y);
    // point[2] - point[1]
    glm::vec2 dirVec2 = glm::vec2(triangle->points[2].gl_Position.x - triangle->points[1].gl_Position.x, triangle->points[2].gl_Position.y - triangle->points[1].gl_Position.y);
    // point[0] - point[2]
    glm::vec2 dirVec3 = glm::vec2(triangle->points[0].gl_Position.x - triangle->points[2].gl_Position.x, triangle->points[0].gl_Position.y - triangle->points[2].gl_Position.y);

    // EDGE FUNCTIONS
    float E1 = ((min_y - triangle->points[0].gl_Position.y) * dirVec1.x) - ((min_x - triangle->points[0].gl_Position.x) * dirVec1.y);
    float E2 = ((min_y - triangle->points[1].gl_Position.y) * dirVec2.x) - ((min_x - triangle->points[1].gl_Position.x) * dirVec2.y);
    float E3 = ((min_y - triangle->points[2].gl_Position.y) * dirVec3.x) - ((min_x - triangle->points[2].gl_Position.x) * dirVec3.y);
    ////////////////////////////////////////////////////////////////

    // calculate area of whole triangle
    float triangleArea = abs(edgeFunction(triangle->points[0], triangle->points[1], triangle->points[2]));

    ShaderInterface si;
    si.uniforms = mem.uniforms;
    si.textures = mem.textures;

    if (cmd.backfaceCulling)
    {
        for (float y = min_y; y < max_y; y++)
        {
            float t1 = E1;
            float t2 = E2;
            float t3 = E3;
            bool insideTriangle = false;

            for (float x = min_x; x < max_x; x++)
            {
                if (t1 >= 0 && t2 > 0 && t3 >= 0)
                {
                    loadFragmentToShader(frame, x, y, prg, si, triangle->points[0], triangle->points[1], triangle->points[2], triangleArea, t2, t3);
                    insideTriangle = true;
                }
                else if (insideTriangle)
                {
                    // got out of triangle
                    break;
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
    else
    {
        for (float y = min_y; y < max_y; y++)
        {
            float t1 = E1;
            float t2 = E2;
            float t3 = E3;
            bool insideTriangle = false;
            
            for (float x = min_x; x < max_x; x++)
            {
                if (t1 <= 0 && t2 <= 0 && t3 <= 0)
                {
                    loadFragmentToShader(frame, x, y, prg, si, triangle->points[0], triangle->points[1], triangle->points[2], triangleArea, abs(t2), abs(t3));
                    insideTriangle = true;
                }
                else if (t1 >= 0 && t2 > 0 && t3 >= 0)
                {
                    loadFragmentToShader(frame, x, y, prg, si, triangle->points[0], triangle->points[1], triangle->points[2], triangleArea, t2, t3);
                    insideTriangle = true;
                }
                else if (insideTriangle)
                {
                    // didnt rasterize a current pixel, but did previous
                    break;
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
}

void cutEdge(OutVertex* a, OutVertex b, Program prg)
{
    float t = (-a->gl_Position.w - a->gl_Position.z) / (b.gl_Position.w - a->gl_Position.w + b.gl_Position.z - a->gl_Position.z);
    
    a->gl_Position = a->gl_Position + t * (b.gl_Position - a->gl_Position);

    for (uint32_t i = 0; i < maxAttributes; i++)
    {
        switch (prg.vs2fs[i])
        {
            case AttributeType::FLOAT:
                a->attributes[i].v1 = a->attributes[i].v1 + t * (b.attributes[i].v1 - a->attributes[i].v1);
                continue;
            case AttributeType::VEC2:
                a->attributes[i].v2 = a->attributes[i].v2 + t * (b.attributes[i].v2 - a->attributes[i].v2);
                continue;
            case AttributeType::VEC3:
                a->attributes[i].v3 = a->attributes[i].v3 + t * (b.attributes[i].v3 - a->attributes[i].v3);
                continue;
            case AttributeType::VEC4:
                a->attributes[i].v4 = a->attributes[i].v4 + t * (b.attributes[i].v4 - a->attributes[i].v4);
                continue;
            case AttributeType::UINT:
                a->attributes[i].u1 = (uint32_t) (a->attributes[i].u1 + t * (b.attributes[i].u1 - a->attributes[i].u1));
                continue;
            case AttributeType::UVEC2:
                a->attributes[i].u2 = a->attributes[i].u2 + (uint32_t) t * (b.attributes[i].u2 - a->attributes[i].u2);
                continue;
            case AttributeType::UVEC3:
                a->attributes[i].u3 = a->attributes[i].u3 + (uint32_t) t * (b.attributes[i].u3 - a->attributes[i].u3);
                continue;
            case AttributeType::UVEC4:
                a->attributes[i].u4 = a->attributes[i].u4 + (uint32_t)t * (b.attributes[i].u4 - a->attributes[i].u4);
                continue;
            default:
                break;
        }
    }
}

int clipping(Triangle* triangle, Triangle* secondTriangle, Program prg)
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

    // CLIPPING NEEDED
    if (!isInsideCameraMask[0] && isInsideCameraMask[1] && isInsideCameraMask[2])
    {
        // Point 0 OK (is not inside)
        cutEdge(&triangle->points[1], triangle->points[0], prg);  // change point 1
        cutEdge(&triangle->points[2], triangle->points[0], prg);  // change point 2
        return 1;
    }
    else if (isInsideCameraMask[0] && !isInsideCameraMask[1] && isInsideCameraMask[2])
    {
        // Point 1 OK (is not inside)
        cutEdge(&triangle->points[0], triangle->points[1], prg);  // change point 0
        cutEdge(&triangle->points[2], triangle->points[1], prg);  // change point 2
        return 1;
    }
    else if (isInsideCameraMask[0] && isInsideCameraMask[1] && !isInsideCameraMask[2])
    {
        // Point 2 OK (is not inside)
        cutEdge(&triangle->points[0], triangle->points[2], prg);  // change point 0
        cutEdge(&triangle->points[1], triangle->points[2], prg);  // change point 1
        return 1;
    }
    else if (isInsideCameraMask[0])
    {
        // Points 1, 2 OK (not inside)
        memcpy(secondTriangle, triangle, sizeof(Triangle));                 // make a copy of a triangle

        cutEdge(&triangle->points[0], triangle->points[1], prg);             // change point 0
        cutEdge(&secondTriangle->points[0], secondTriangle->points[2], prg); // change point 0 of second triangle
        triangle->points[2] = secondTriangle->points[0];                    // change point of first triangle to second intersection
        return 2;
    }
    else if (isInsideCameraMask[1])
    {
        // Points 0, 2 OK (not inside)
        memcpy(secondTriangle, triangle, sizeof(Triangle));                 // make a copy of a triangle

        cutEdge(&triangle->points[1], triangle->points[0], prg);             // change point 1
        cutEdge(&secondTriangle->points[1], secondTriangle->points[2], prg); // change point 1 of second triangle
        triangle->points[2] = secondTriangle->points[1];                    // change point of first triangle to second intersection
        return 2;
    }
    else
    {
        // Points 0, 1 OK (not inside)
        memcpy(secondTriangle, triangle, sizeof(Triangle));                 // make a copy of a triangle

        cutEdge(&triangle->points[2], triangle->points[0], prg);             // change point 2
        cutEdge(&secondTriangle->points[2], secondTriangle->points[1], prg); // change point 2 of second triangle
        triangle->points[1] = secondTriangle->points[2];                    // change point of first triangle to second intersection
        return 2;
    }
}

void draw(GPUMemory& mem, DrawCommand cmd, uint32_t drawID) 
{
    for (uint32_t triangleIndex = 0; triangleIndex < cmd.nofVertices / 3; triangleIndex++)
    {
        Triangle triangle = primitiveAssembly(mem, cmd, drawID, triangleIndex);
        Triangle secondTriangle;

        switch (clipping(&triangle, &secondTriangle, mem.programs[cmd.programID]))
        {
            case 0:
                // dont rasterize this triangle
                continue;
            case 1:
                // one triangle to rasterize
                break;
            case 2:
                // two triangles to rasterize
                perspectiveDivision(&secondTriangle);
                viewportTransformation(&secondTriangle, mem.framebuffer.width, mem.framebuffer.height);
                rasterize(mem, &secondTriangle, cmd);
                break;
        }

        perspectiveDivision(&triangle);
        viewportTransformation(&triangle, mem.framebuffer.width, mem.framebuffer.height);
        rasterize(mem, &triangle, cmd);
    }
}

void clear(GPUMemory& mem, ClearCommand cmd) {
    if (cmd.clearColor) {
        uint32_t combinedValue = 0;
        combinedValue |= ((uint32_t)(cmd.color.r * 255.f));
        combinedValue |= ((uint32_t)(cmd.color.g * 255.f)) << 8;
        combinedValue |= ((uint32_t)(cmd.color.b * 255.f)) << 16;
        combinedValue |= ((uint32_t)(cmd.color.a * 255.f)) << 24;

        uint32_t* arr = reinterpret_cast<uint32_t*>(mem.framebuffer.color);

        size_t colorBuffLenght = mem.framebuffer.width * mem.framebuffer.height;
        std::fill_n(arr, colorBuffLenght, combinedValue);
        /*
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
        */
    }
    if (cmd.clearDepth)
    {
        std::fill_n(mem.framebuffer.depth, mem.framebuffer.width * mem.framebuffer.height, cmd.depth);
        //size_t depthBuffLenght = mem.framebuffer.width * mem.framebuffer.height;
        //for (size_t i = 0; i < depthBuffLenght; i++)
        //{
        //    mem.framebuffer.depth[i] = cmd.depth;
        //}
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

