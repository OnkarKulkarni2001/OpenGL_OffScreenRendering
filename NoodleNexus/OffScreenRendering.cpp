#include "cFBO/cFBO_RGB_depth.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include "cBasicFlyCamera/cBasicFlyCamera.h"
#include "sMesh.h"
#include "cBasicTextureManager/cBasicTextureManager.h"

void RenderScene(
    GLuint program,
    glm::mat4 matProjection,
    glm::mat4 matView,
    float ratio,
    glm::vec3 eyeLocation);

void DrawMesh(sMesh* pCurMesh, GLuint program, bool SetTexturesFromMeshInfo = true);
sMesh* g_pFindMeshByFriendlyName(std::string theNameToFind);

extern cBasicFlyCamera* g_pFlyCamera;
extern cBasicTextureManager* g_pTextures;

void FBOInit(cFBO_RGB_depth& FBO_View) {
    std::string FBOError;
    if (!FBO_View.init(1920, 1080, FBOError))
        //    if (!FBO_WarehouseView.init(128, 64, FBOError))
        //    if (!FBO_WarehouseView.init(3840 * 2, 2180 * 2, FBOError))
    {
        std::cout << "Error: FBO.init(): " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK" << std::endl;
    }
}

void FBOBind(cFBO_RGB_depth& FBO_View, float ratio, GLuint program, glm::vec3 eyeLocation, glm::vec3 targetLocation) {
    
    //sMesh* skySphere = g_pFindMeshByFriendlyName("SkySphere");
    //skySphere->bIsVisible = false;

    glm::mat4 matProjection = glm::mat4(1.0f);
    glm::mat4 matView = glm::mat4(1.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // **************************************************
    // 
    // RENDER from the inside of the warehouse


    // Point output to the off-screen FBO
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_View.ID);

    // These will ONLY work on the default framebuffer
       // glViewport(0, 0, width, height);
       //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // FBO_WarehouseView.clearColourBuffer(0);
    FBO_View.clearBuffers(true, true);


    matProjection = glm::perspective(0.6f,
        ratio,
        0.1f,
        1000.0f);

    //glm::vec3 eyeInsideWarehouse = glm::vec3(-197.0f, 14.0f, -72.0f);
    //float xOffset = 10.0f * glm::sin((float)glfwGetTime() / 2.0f);
    //glm::vec3 atInsideWareHouse =
    //    eyeInsideWarehouse + glm::vec3(xOffset, 0.0f, 10.0f);
    
    matView = glm::lookAt(eyeLocation, targetLocation, glm::vec3(0.0f, 1.0f, 0.0f));
    glUseProgram(program);
    //std::cout << "Eye: " << eyeLocation.x << ", " << eyeLocation.y << ", " << eyeLocation.z << std::endl;
    //std::cout << "Target: " << targetLocation.x << ", " << targetLocation.y << ", " << targetLocation.z << std::endl;

    RenderScene(program, matProjection, matView, ratio, eyeLocation);
    //skySphere->bIsVisible = true;1
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderFBOTexture(sMesh* pFBOTextureMesh, sMesh* pFBOTextureMesh2, cFBO_RGB_depth& FBO_View, cFBO_RGB_depth& FBO_View2, GLuint program, glm::mat4& matProjection, glm::mat4& matView, float ratio) {

    if (pFBOTextureMesh && pFBOTextureMesh2)
    {
        pFBOTextureMesh->bIsVisible = true;
        pFBOTextureMesh2->bIsVisible = true;

        GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

        GLint matView_UL = glGetUniformLocation(program, "matView");
        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);



        // Connect texture unit #0 to the offscreen FBO
        glActiveTexture(GL_TEXTURE0);

        // The colour texture inside the FBO is just a regular colour texture.
        // There's nothing special about it.
        glBindTexture(GL_TEXTURE_2D, FBO_View.colourTexture_0_ID);
        //glBindTexture(GL_TEXTURE_2D, ::g_pTextures->getTextureIDFromName("dua-lipa-promo.bmp"));

        GLint texture01_UL = glGetUniformLocation(program, "texture00");
        glUniform1i(texture01_UL, 0);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here

        GLint texRatio_0_to_3_UL = glGetUniformLocation(program, "texRatio_0_to_3");
        glUniform4f(texRatio_0_to_3_UL,
            1.0f,
            0.0f,
            0.0f,
            0.0f);

        //GLint b_Is_FBO_Texture_UL = glGetUniformLocation(program, "b_Is_FBO_Texture");
        //GLint bUseTextureAsColour_UL = glGetUniformLocation(program, "bUseTextureAsColour");

        DrawMesh(pFBOTextureMesh, program, false);
        pFBOTextureMesh->bIsVisible = false;

        matProjection_UL = glGetUniformLocation(program, "matProjection");
        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

        matView_UL = glGetUniformLocation(program, "matView");
        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);



        // Connect texture unit #0 to the offscreen FBO
        glActiveTexture(GL_TEXTURE0);

        // The colour texture inside the FBO is just a regular colour texture.
        // There's nothing special about it.
        glBindTexture(GL_TEXTURE_2D, FBO_View2.colourTexture_0_ID);
        //glBindTexture(GL_TEXTURE_2D, ::g_pTextures->getTextureIDFromName("dua-lipa-promo.bmp"));

        texture01_UL = glGetUniformLocation(program, "texture00");
        glUniform1i(texture01_UL, 0);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here

        texRatio_0_to_3_UL = glGetUniformLocation(program, "texRatio_0_to_3");
        glUniform4f(texRatio_0_to_3_UL,
            1.0f,
            0.0f,
            0.0f,
            0.0f);

        //GLint b_Is_FBO_Texture_UL = glGetUniformLocation(program, "b_Is_FBO_Texture");
        //GLint bUseTextureAsColour_UL = glGetUniformLocation(program, "bUseTextureAsColour");
        DrawMesh(pFBOTextureMesh2, program, false);
        
        //glUniform1f(b_Is_FBO_Texture_UL, (float)GL_FALSE);
        //glUniform1f(bUseTextureAsColour_UL, (float)GL_TRUE);

        pFBOTextureMesh2->bIsVisible = false;
    }


    RenderScene(program, matProjection, matView, ratio, ::g_pFlyCamera->getEyeLocation());
}