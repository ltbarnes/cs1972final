#include "mesh.h"
#include "trianglemesh.h"

#include "debugprinting.h"

Mesh::Mesh()
    : m_w(0),
      m_h(0),
      m_vertMappings(NULL),
      m_mappingSize(0),
      m_vao(0),
      m_vbo(0)
{
}

Mesh::~Mesh()
{
    if (m_vertMappings)
        delete [] m_vertMappings;
    if (m_vao)
        glDeleteVertexArrays(1, &m_vao);
    if (m_vbo)
        glDeleteBuffers(1, &m_vbo);
}

void Mesh::initStrip(GLuint shader, GLuint w, GLuint h, const glm::vec3 *verts, const glm::vec3 *norms)
{
    assert(w > 1 && h > 1);

    m_w = w;
    m_h = h;
    setMappings();
    GLuint memsize = m_mappingSize * 8 * sizeof(float);
    createBuffers(shader, memsize);
    setVerts(verts, norms);
}

void Mesh::initTriangles(GLuint shader, std::vector<Tri> tris, const glm::vec3 *verts)
{
    m_mappingSize = tris.size() * 3;
    GLuint memsize = m_mappingSize * 8 * sizeof(float);
    createBuffers(shader, memsize);
    setTriangles(tris, verts);
}

void Mesh::setMappings()
{
    m_mappingSize = (m_h - 1) * 2 * m_w + 3  * (m_h - 2);
    m_vertMappings = new GLuint[m_mappingSize];

    GLuint i = 0;
    GLuint row;
    for (GLuint r = 1; r < m_h; r++)
    {
        if (r % 2 == 0)
        {
            row = (r-1) * m_w;

            if (r > 1)
                m_vertMappings[i++] = row;

            for (GLuint c = 0; c < m_w; c++)
            {
                m_vertMappings[i++] = row;
                m_vertMappings[i++] = row + m_w;
                row++;
            }

            if (r < m_h - 1)
            {
                m_vertMappings[i++] = (row - 1) + m_w;
                m_vertMappings[i++] = (row - 1) + m_w;
            }

        }
        else
        {
            row = r * m_w;

            if (r > 1)
                m_vertMappings[i++] = row;

            for (GLuint c = 0; c < m_w; c++)
            {
                m_vertMappings[i++] = row;
                m_vertMappings[i++] = row - m_w;
                row++;
            }

            if (r < m_h - 1)
            {
                m_vertMappings[i++] = (row - 1) - m_w;
                m_vertMappings[i++] = (row - 1) - m_w;
            }
        }
    }
    cout << i << " : " << m_mappingSize << endl;
}

void Mesh::setVerts(const glm::vec3 *verts, const glm::vec3 *norms)
{
    int size = m_mappingSize * 8;
    GLfloat *data = new GLfloat[size];
    int index = 0;

    for (GLuint i = 0; i < m_mappingSize; i++)
    {
        addVertex(&index, verts[m_vertMappings[i]], norms[m_vertMappings[i]], data);
    }

    fillBuffer(data, 0, m_mappingSize);

    delete [] data;
}

void Mesh::setTriangles(std::vector<Tri> tris, const glm::vec3 *verts)
{
    int size = m_mappingSize * 8;
    GLfloat *data = new GLfloat[size];
    int index = 0;

    foreach (Tri tri, tris) {
        addVertex(&index, verts[tri.a], tri.normal, data);
        addVertex(&index, verts[tri.b], tri.normal, data);
        addVertex(&index, verts[tri.c], tri.normal, data);
    }

    fillBuffer(data, 0, m_mappingSize);

    delete [] data;
}

void Mesh::fillBuffer(const float *data, int start, int count)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, start * 8 * sizeof(GLfloat), count * 8 * sizeof(GLfloat), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::onDraw(GLenum mode)
{
    glDisable(GL_CULL_FACE);
    glBindVertexArray(m_vao);
    glDrawArrays(mode, 0, m_mappingSize);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
}

void Mesh::createBuffers(GLuint shader, GLuint size)
{
    if (m_vao)
        glDeleteVertexArrays(1, &m_vao);
    if (m_vbo)
        glDeleteBuffers(1, &m_vbo);

    // Initialize the vertex array object.
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Initialize the vertex buffer object.
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);

    GLuint position = glGetAttribLocation(shader, "position");
    GLuint normal = glGetAttribLocation(shader, "normal");
    GLuint texCoord = glGetAttribLocation(shader, "texCoord");

    glEnableVertexAttribArray(position);
    glVertexAttribPointer(
        position,
        3,                   // Num coordinates per position
        GL_FLOAT,            // Type
        GL_FALSE,            // Normalized
        sizeof(GLfloat) * 8, // Stride
        (void*) 0            // Array buffer offset
    );
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(
        normal,
        3,                              // Num coordinates per normal
        GL_FLOAT,                       // Type
        GL_TRUE,                        // Normalized
        sizeof(GLfloat) * 8,            // Stride
        (void*) (sizeof(GLfloat) * 3)   // Array buffer offset
    );
    glEnableVertexAttribArray(texCoord);
    glVertexAttribPointer(
        texCoord,
        2,                              // Num coordinates per position
        GL_FLOAT,                       // Type
        GL_TRUE,                        // Normalized
        sizeof(GLfloat) * 8,            // Stride
        (void*) (sizeof(GLfloat) * 6)   // Array buffer offset
    );

    // Unbind buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Mesh::addVertex(int *i, glm::vec3 v, glm::vec3 norm, float* data)
{
    data[(*i)++] = v.x;
    data[(*i)++] = v.y;
    data[(*i)++] = v.z;
    data[(*i)++] = norm.x;
    data[(*i)++] = norm.y;
    data[(*i)++] = norm.z;
    data[(*i)++] = 0;
    data[(*i)++] = 0;
}


void Mesh::initTriangles(GLuint shader, std::vector<Tri*> tris, const glm::vec3 *verts)
{
    m_mappingSize = tris.size() * 3;
    GLuint memsize = m_mappingSize * 8 * sizeof(float);
    createBuffers(shader, memsize);
    setTriangles(tris, verts);
}

void Mesh::setTriangles(std::vector<Tri*> tris, const glm::vec3 *verts)
{
    int size = m_mappingSize * 8;
    GLfloat *data = new GLfloat[size];
    int index = 0;

    foreach (Tri* tri, tris) {
        addVertex(&index, verts[tri->a], tri->normal, data);
        addVertex(&index, verts[tri->b], tri->normal, data);
        addVertex(&index, verts[tri->c], tri->normal, data);
    }

    fillBuffer(data, 0, m_mappingSize);

    delete [] data;
}
