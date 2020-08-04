#include "stdafx.h"
#include "ShadowMapFbo.h"
#include <stdio.h>
#include <iostream>

//-------------------------------------------------------------
 ShadowMapFBO::ShadowMapFBO()
 {
	    m_fbo = 0;
	    //m_shadowMap = 0;
 }

 //-------------------------------------------------------------
 ShadowMapFBO::~ShadowMapFBO()
 {
	     if (m_fbo != 0) 
		 {
		   glDeleteFramebuffers(1, &m_fbo);
         }
 }

 //-------------------------------------------------------------
 bool ShadowMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight, bool needsCubeMap)
 {
	 m_shadowMap.mTextureWidth = WindowWidth;
	 m_shadowMap.mTextureHeight = WindowHeight;
	 
	 glGenFramebuffers(1, &m_fbo);
	 glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	 if (needsCubeMap)
	 {
		 m_shadowMap.makeDepthCubeMap();
		 glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap.id, 0);
	 }
	 else
	 {
		 m_shadowMap.makeDepthTexture();
		 glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
			 m_shadowMap.id, 0);
	 }

	 glDrawBuffer(GL_NONE);
	 
	 GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	  	  
	   if (Status != GL_FRAMEBUFFER_COMPLETE) {
	       printf("FB error, status: 0x%x\n", Status);
	       return false;
	  
	}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	     return true;
	 }

 //-------------------------------------------------------------
 void ShadowMapFBO::BindForWriting()
 {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
 }

 //-------------------------------------------------------------
 void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
	     glActiveTexture(TextureUnit);
	     glBindTexture(GL_TEXTURE_2D, m_shadowMap.id);
}

 //-------------------------------------------------------------
 Texture ShadowMapFBO::GetTexture()
 {
	 return m_shadowMap;
 }