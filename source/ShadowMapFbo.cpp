#include "stdafx.h"
#include "ShadowMapFbo.h"

 ShadowMapFBO::ShadowMapFBO()
 {
	    m_fbo = 0;
	    //m_shadowMap = 0;
 }


 ShadowMapFBO::~ShadowMapFBO()
 {
	     if (m_fbo != 0) {
		         glDeleteFramebuffers(1, &m_fbo);
		
          }
		  /*   if (m_shadowMap != 0) {
		         glDeleteFramebuffers(1, &m_shadowMap);		
	      }*/
 }

 bool ShadowMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
 {
	 m_shadowMap.mTextureWidth = WindowWidth;
	 m_shadowMap.mTextureHeight = WindowHeight;
	 // Создаем FBO 
		    glGenFramebuffers(1, &m_fbo);

	// Создаем буфер глубины 
	  glGenTextures(1, &m_shadowMap.id);
	  glBindTexture(GL_TEXTURE_2D, m_shadowMap.id);
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	  GLfloat borderColor[] = { 1.0,  1.0,  1.0,  1.0 };
	  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	   glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		                           m_shadowMap.id, 0);

		     // Отключаем запись в буфер цвета 
		     glDrawBuffer(GL_NONE);
	

		     GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	

		     if (Status != GL_FRAMEBUFFER_COMPLETE) {
		         printf("FB error, status: 0x%x\n", Status);
		         return false;
		
	}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //i added
	     return true;
	 }


 void ShadowMapFBO::BindForWriting()
 {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
 }


 void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
	     glActiveTexture(TextureUnit);
	     glBindTexture(GL_TEXTURE_2D, m_shadowMap.id);
}


 Texture ShadowMapFBO::GetTexture()
 {
	 return m_shadowMap;
 }