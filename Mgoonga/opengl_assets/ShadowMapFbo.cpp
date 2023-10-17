#include "stdafx.h"
#include "ShadowMapFbo.h"
#include <stdio.h>

//-------------------------------------------------------------
 ShadowMapFBO::ShadowMapFBO()
 {
		m_fbo = 0;
 }

 //-------------------------------------------------------------
 ShadowMapFBO::~ShadowMapFBO()
 {
  if (m_fbo != 0) 
	{
		glDeleteFramebuffers(1, &m_fbo);
		m_shadowMap.freeTexture();
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
		 m_shadowMap.mTextureWidth = 1024; //!?
		 m_shadowMap.mTextureHeight = 1024;//!?
		 m_cubemap = needsCubeMap;
		 m_shadowMap.makeDepthCubeMap();
		 glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap.id, 0);
		 glReadBuffer(GL_NONE); //?
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
 bool ShadowMapFBO::InitCSM(unsigned int WindowWidth, unsigned int WindowHeight, int32_t _layers)
 {
	 m_shadowMap.mTextureWidth = WindowWidth;
	 m_shadowMap.mTextureHeight = WindowHeight;

	 glGenFramebuffers(1, &m_fbo);
	 m_shadowMap.makeDepthTextureArray(_layers);
	 glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	 glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		 m_shadowMap.id, 0);
	 glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap.id, 0);

	 glDrawBuffer(GL_NONE);
	 glReadBuffer(GL_NONE);

	 GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	 if (Status != GL_FRAMEBUFFER_COMPLETE) {
		 printf("FB error, status: 0x%x\n", Status);
		 return false;

	 }
	 glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	 m_array = true;
	 return true;
 }

 //-------------------------------------------------------------
 void ShadowMapFBO::BindForWriting()
 {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
 }

 //-------------------------------------------------------------
 void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
	 glActiveTexture(TextureUnit);
	 if (m_cubemap)
		 glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap.id);
	 else if(m_array)
		 glBindTexture(GL_TEXTURE_2D_ARRAY, m_shadowMap.id);
	 else
		 glBindTexture(GL_TEXTURE_2D, m_shadowMap.id);
}

 //-------------------------------------------------------------
 Texture ShadowMapFBO::GetTexture()
 {
	 return m_shadowMap;
 }