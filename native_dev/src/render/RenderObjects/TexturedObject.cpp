#include "T4X/render/RenderObjects/TexturedObject.h"

void TexturedObject::printDebug()
{
    printf("DEBUG: info for TextureObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader.program_id);
    printf("    vao_id: %d\n", vao_id);
    printf("    vbo_id: %d\n", vbo_id);
}

bool TexturedObject::setAttribs() {
    glBindVertexArray(vao_id);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    /*
        Format for input vertices
            location: 0
            size: 2
            type: float
    */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    if (!checkGLError()) return false;
    return true;
}

bool TexturedObject::loadShaders() {
    // initialise shaders
    SourcePair texture_program[] = {
        SourcePair{"texturevert.glvs", GL_VERTEX_SHADER},
        SourcePair{"texturegeom.glgs", GL_GEOMETRY_SHADER},
        SourcePair{"texturefrag.glfs", GL_FRAGMENT_SHADER} };
    bool shader_success = shader.createProgram(&texture_program[0], 3);
    if (!shader_success)
    {
        printDebug();
        printf("Shader Failure\n");
        return false;
    }

    dimension_location = glGetUniformLocation(shader.program_id, "dimensions");
    if (dimension_location == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "dimensions", object_id);
        printDebug();
        return false;
    }

    setDimensions(10, 10);
    updateBuffers(2 * sizeof(float), &origin[0]);
    return true;
}

bool TexturedObject::setOrigin(float x, float y) {
    origin[0] = x;
    origin[1] = y;
    return updateBuffers(2 * sizeof(float), &origin[0]);
}

bool TexturedObject::setDimensions(int height, int width) {
    if (dimension_location == -1)
    {
        printf("dims not loaded render_obj=%d\n", object_id);
        printDebug();
        return false;
    }
    glUseProgram(shader.program_id); // set shader program
    glUniform2f(dimension_location, 0.1, 0.1);
}

bool TexturedObject::updateBuffers(int size, float *data)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    if (!checkGLError())
    {
        printf("[ TEXTURED OBJECT ] gl error in buffer update\n");
        printDebug();
        return false;
    }
    return true;
}

void TexturedObject::draw()
{
    glUseProgram(shader.program_id); // set shader program
    glBindVertexArray(vao_id);       // bind vertex array
    texture.activate();              // activate texture

    glDrawArrays(GL_POINTS, 0, 1); // draw the object
}

bool TexturedObject::loadTexture(const char* filename) {
    glUseProgram(shader.program_id); // set shader program
    if (!texture.setTexture(filename, GL_TEXTURE0)) {
        return false;
    }
   
    return checkGLError();
}