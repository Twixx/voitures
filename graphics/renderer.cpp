#include "../glm/gtx/transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#include "renderer.h"

Renderer::Renderer() : m_projection(1.0), m_camera(glm::vec3(50, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1))
{
}

Renderer::~Renderer()
{
}

void Renderer::setProjection(glm::mat4 &projection)
{
    m_projection = glm::mat4(projection) ;
}

void Renderer::setPerspective(float angle, float ratio, float near, float far)
{
    m_projection = glm::perspective(angle, ratio, near, far);
}

void Renderer::draw(Object const& object, GLenum mode)
{
    draw(object.getDrawable(), mode);
}

void Renderer::draw(Drawable const& drawable, GLenum mode)
{
    GLuint shader = drawable.getShader()->getProgramID() ;
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    glUseProgram(shader);
        glBindVertexArray(drawable.getIdVAO());
                glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
                glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(m_camera.getView()));
                glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(drawable.getModel()));
                if(drawable.getTexture())
                {
                    glBindTexture(GL_TEXTURE_2D, drawable.getTexture()->getId());
                }
                glDrawElements(GL_TRIANGLES,drawable.getIndicesNumber()*3, GL_UNSIGNED_INT, NULL);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glUseProgram(0);
}

Camera& Renderer::getCamera()
{
	return m_camera;
}
