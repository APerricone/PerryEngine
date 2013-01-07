#include "keymanager.h"
#include <memory.h>

#include "opengl.h"

#include <GL/glfw.h>

KeyManager::KeyManager()
{
	memset(m_abEnable,false,N_KEY);
	memset(m_abLastValue,false,N_KEY);
	memset(m_abNewValue,false,N_KEY);
}

void KeyManager::Update()
{
	for(int i=0;i<N_KEY;i++)
	{
		if( m_abEnable[i] )
		{
			m_abLastValue[i] = m_abNewValue[i];
			if( i<=GLFW_KEY_LAST )
				m_abNewValue[i] = glfwGetKey( i ) == GLFW_PRESS;
			else
				m_abNewValue[i] = false;
		}
	}
}
	