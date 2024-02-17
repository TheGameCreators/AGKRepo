
// Bullet Physics Wrapper for App Game Kit
//Stab In The Dark Software 
//Copyright (c) 2002-2015
//http://gonefishing@stabinthedarksoftware.com 


#include "agk.h"
#include <Box2D/Common/b2Draw.h>


using namespace AGK;
GLDebugDrawer* glDebugDraw = NULL;

GLDebugDrawer::GLDebugDrawer() : m_debugMode(0)
{
	m_pShader = AGKShader::g_pShaderColor;
}

GLDebugDrawer::~GLDebugDrawer()
{
}

void	GLDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
{
	// Doesn't work on Android
	/*
	btScalar scaleFactor =  GetCurrentDynamicsWorld()->m_scaleFactor;
	btVector3 scaledFrom, scaledTo;
	scaledFrom = from * scaleFactor;
	scaledTo = to * scaleFactor;

	//agk::PlatformBindBuffer( 0 );
	//if ( !m_pShader ) return;
	//m_pShader->MakeActive();

	//btVector3 vertices[2];
	//b2Color color[2];

	//vertices[0] = scaledFrom;
	//vertices[0] = scaledTo;

	//color[0] = b2Color(fromColor.getX(), fromColor.getY(), fromColor.getZ());
	//color[1] = b2Color(toColor.getX(), toColor.getY(), toColor.getZ());

	//int vertexCount = 2;
	//GLfloat *pVertices = new GLfloat[ vertexCount*3 ];
	//GLubyte *pColor = new GLubyte[ vertexCount*4 ];
	//
	//agk::PlatformBindBuffer( 0 );

	//if ( !m_pShader ) return;
	//m_pShader->MakeActive();

	//int locPos = m_pShader->GetAttribPosition();
	//int locColor = m_pShader->GetAttribColor();


	//if ( locPos >= 0 ) m_pShader->SetAttribFloat( locPos, 3, 0, pVertices );
	//if ( locColor >= 0 ) m_pShader->SetAttribUByte( locColor, 4, 0, true, pColor );

	//agk::PlatformSetCullMode( 0 );
	//agk::PlatformSetBlendMode( 0 );
	//agk::PlatformSetDepthTest( 0 );

	//for (int i = 0; i < vertexCount; ++i)
	//{
	//	pVertices[ i*3 ] = vertices[i].getX();
	//	pVertices[ i*3 + 1 ] = vertices[i].getY();
	//	pVertices[ i*3 + 2 ] = vertices[i].getZ();

	//	pColor[ i*4 ] = (unsigned char) (color[i].r * 255);
	//	pColor[ i*4 + 1 ] = (unsigned char) (color[i].g * 255);
	//	pColor[ i*4 + 2 ] = (unsigned char) (color[i].b * 255);
	//	pColor[ i*4 + 3 ] = 255;
	//}

	//m_pShader->DrawPrimitives( AGK_LINES, 0, vertexCount );

	//agk::PlatformSetDepthTest( 1 );

	//delete [] pVertices;
	//delete [] pColor;


	//Test from this Link
	//http://sio2interactive.forumotion.net/t599-enabling-bullet-debug-draw-code-included

	  float tmp[ 6 ] = { scaledFrom.getX(), scaledFrom.getY(), scaledFrom.getZ(),
                    to.getX(), to.getY(), to.getZ() };
      
      glPushMatrix();
      {         
         //glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);         
         glVertexPointer( 3, GL_FLOAT, 0, &tmp );
         glPointSize( 5.0f );
         glDrawArrays( GL_POINTS, 0, 2 );
         glDrawArrays( GL_LINES, 0, 2 );
      }
      glPopMatrix();      
   
	*/
}



void	GLDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	drawLine(from,to,color,color);
}

void GLDebugDrawer::drawSphere (const btVector3& p, btScalar radius, const btVector3& color)
{
	// This doesn't work in OpenGL ES
	/*
	glColor4f (color.getX(), color.getY(), color.getZ(), btScalar(1.0f));
	glPushMatrix ();
	glTranslatef (p.getX(), p.getY(), p.getZ());

	int lats = 5;
	int longs = 5;

	int i, j;
	for(i = 0; i <= lats; i++) {
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
		btScalar z0  = radius*sin(lat0);
		btScalar zr0 =  radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
			btScalar x = cos(lng);
			btScalar y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}

	glPopMatrix();
	*/
}

void	GLDebugDrawer::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{
	// This doesn't work in OpenGL ES
	/*
	btScalar scaleFactor =  GetCurrentDynamicsWorld()->m_scaleFactor;
	btVector3 aScaled, bScaled, cScaled;
	aScaled = a * scaleFactor;
	bScaled = b * scaleFactor;
	cScaled = c * scaleFactor;

//	if (m_debugMode > 0)
	{
		const btVector3	n=btCross(b-a,c-a).normalized();
		glBegin(GL_TRIANGLES);		
		glColor4f(color.getX(), color.getY(), color.getZ(),alpha);
		glNormal3d(n.getX(),n.getY(),n.getZ());
		glVertex3d(aScaled.getX(),aScaled.getY(),aScaled.getZ());
		glVertex3d(bScaled.getX(),bScaled.getY(),bScaled.getZ());
		glVertex3d(cScaled.getX(),cScaled.getY(),cScaled.getZ());
		glEnd();
	}
	*/
}

void	GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;

}

void	GLDebugDrawer::draw3dText(const btVector3& location,const char* textString)
{
	// This doesn't work in OpenGL ES
	/*
	glRasterPos3f(location.x(),  location.y(),  location.z());
	*/
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void	GLDebugDrawer::reportErrorWarning(const char* warningString)
{
	printf("%s\n",warningString);
}

void	GLDebugDrawer::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
	// This doesn't work in OpenGL ES
	/*
	{
		btVector3 to=pointOnB+normalOnB*1;//distance;
		const btVector3&from = pointOnB;
		glColor4f(color.getX(), color.getY(), color.getZ(),1.f);
		//glColor4f(0,0,0,1.f);
		glBegin(GL_LINES);
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
		glEnd();

		
//		glRasterPos3f(from.x(),  from.y(),  from.z());
//		char buf[12];
//		sprintf(buf," %d",lifeTime);
		//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);


	}
	*/
}
















