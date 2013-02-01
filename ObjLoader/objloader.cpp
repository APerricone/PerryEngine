#include "objloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "model.h"
#include "log.h"

#include <time.h>

CObjLoader::~CObjLoader()
{
	std::vector<char*>::iterator i;
	for( i = m_astrMaterialNames.begin();
		 i != m_astrMaterialNames.end(); ++i)
	{
		delete (*i);
	}
}

bool CObjLoader::LoadObj(const char* i_strFilename)
{
	m_strFilename = i_strFilename;
	m_pDest = NULL;
	if( !ParseMtl() ) return false;
	if( !ParseObj() ) return false;
	return true;
}

/// OBJ
bool CObjLoader::ParseObj()
{
	int l = strlen(m_strFilename);
	char *filename = new char[l+5];
	strcpy(filename,m_strFilename);
	strcat(filename,".obj");

	m_bCurrHasNormal = false;
	m_bCurrHasTextures = false;
	ILog::Message("parsing %s...\n", filename);
	FILE *f=fopen(filename,"rt");
	if(f == NULL)
	{
		ILog::Error("file not found\n");
		return false;
	}
	fseek(f,0,SEEK_END);
	long fileSize = ftell(f);
	fseek(f,0,SEEK_SET);
	time_t currT; time(&currT);
	long sec = 0;
	while(1)
	{
		fgets(m_strCurrLine,1024,f);
		if(feof(f)) break;
		switch(m_strCurrLine[0])
		{
		case 'v': if(!ManageObj_v()) return false; break;
		case 'f': if(!ManageObj_f()) return false; break;
		case 'o': if(!ManageObj_o()) return false; break;
		case 'g': if(!ManageObj_g()) return false; break;
		case 'u': if(!ManageObj_u()) return false; break;
		}

		time_t newT;
		time(&newT);
		if( newT != currT )
		{
			currT = newT;
			sec++;
			long filePos = ftell(f);
			float pos = float(filePos) * 100 / float(fileSize);
			float total = sec * 100 / pos;

			ILog::Message("\r\t... %05.2f%% done in %i sec, estimated %.0f (%.0f to end)", 
				pos,sec,total,(total-sec));
		}
	}
	ILog::Message("\n");
	fclose(f);
	CloseMesh();
	if( m_pDest )
		m_pDest->Enable();
	return true;
}

bool CObjLoader::ManageObj_v()
{
	if(m_strCurrLine[1]==' ')
	{
		float3 t;
		sscanf(m_strCurrLine,"v %f %f %f",&t.x(),&t.y(),&t.z());
		m_af3Positions.push_back(t);
	}
	else if(m_strCurrLine[1]=='n')
	{
		float3 t;
		sscanf(m_strCurrLine,"vn %f %f %f",&t.x(),&t.y(),&t.z());
		m_af3normals.push_back(t);
	}
	else if(m_strCurrLine[1]=='t')
	{
		float2 t;
		sscanf(m_strCurrLine,"vt %f %f",&t.x(),&t.y());
		m_af2textures.push_back(t);
	}
	return true;
}

bool CObjLoader::ManageObj_o()
{
	if(m_pDest!=0)
	{
		m_pDest->Enable();
		CloseMesh();
		m_bCurrHasNormal = false;
		m_bCurrHasTextures = false;
	}
	char *name=strtok(m_strCurrLine," \r\n");
	name=strtok(NULL,"\r\n");
	m_pDest = CModel::CreateModel();
	m_pDest->SetName(name);
	m_pDest->Disable();
	ILog::Message("adding:%s\n", name);
	return true;
}

bool CObjLoader::ManageObj_g()
{
	if(m_pDest==0)
	{
		ILog::Error("found geometry definition outside object definition");
		return false;
	}
	if( m_pDest->GetLastMesh() )
	{
		CloseMesh();
		m_pDest->AddMesh();
	}
	m_bCurrHasNormal = false;
	m_bCurrHasTextures = false;

	char *name=strtok(m_strCurrLine," \r\n");
	name=strtok(NULL,"\r\n");
	//m_pDest->GetLastMesh()->SetName(name);
	return true;
}

void CObjLoader::CloseMesh()
{
	if( !m_bCurrHasNormal)
		m_oCreator.CalculateNormals();
	m_oCreator.CreateMesh(*m_pDest->GetLastMesh(),true,false,m_bCurrHasTextures);
	m_oCreator.Clear();
}

bool CObjLoader::ManageObj_u()
{
	if(strncmp(m_strCurrLine,"usemtl",6) == 0)
	{
		char *name=strtok(m_strCurrLine," \r\n");
		name=strtok(NULL,"\r\n");
		unsigned int i;
		for( i=0; i < m_astrMaterialNames.size(); ++i)
		{
			if( strcmp( name, m_astrMaterialNames[i] ) == 0 ) 
				break;
		}
		if( i == m_astrMaterialNames.size() )
		{
			ILog::Error("Unable to found material \"%s\"\n", name);
			return false;
		} 
		if( i>= m_aMaterials.size() ) 
		{
			ILog::Error("Undefined material with name \"%s\"\n", name);
			return false;
		}

		if( m_pDest->GetLastMesh() )
			CloseMesh();
		m_pDest->AddMesh();
		m_bCurrHasNormal = false;
		m_bCurrHasTextures = false;
		m_pDest->SetLastMaterial( m_aMaterials[i] );
	}
	return true;
}

bool CObjLoader::ManageObj_f()
{
	if( m_pDest == NULL)
	{
		ILog::Error("Found face definition outside object definition");
		return false;
	}
	if( m_pDest->GetLastMesh() == NULL) 
	{
		ILog::Error("Found face definition outside geometry definition");
		return false;
	}
	unsigned int p[3],n[3],t[3];
	bool normal = false,texCoord = false;
	if(strstr(m_strCurrLine,"/"))
	{
		if(strstr(m_strCurrLine,"//"))
		{
			normal = true;
			sscanf(m_strCurrLine,"f %i//%i %i//%i %i//%i",
				&p[0],&n[0],
				&p[1],&n[1],
				&p[2],&n[2]);
		} else
		{
			// chec for "%i/%i %i/%i %i/%i" or "%i/%i/%i %i/%i/%i %i/%i/%i"
			const char *firstSlash = strstr(m_strCurrLine,"/");
			const char *firstSpace = strstr(firstSlash," ");
			const char *secondSlash = strstr(firstSlash+1,"/");
			if( secondSlash > firstSpace )
			{
				texCoord = true;
				sscanf(m_strCurrLine,"f %i/%i %i/%i %i/%i",
					&p[0],&t[0],
					&p[1],&t[1],
					&p[2],&t[2]);
			} else
			{
				normal = true;
				texCoord = true;
				sscanf(m_strCurrLine,"f %i/%i/%i %i/%i/%i %i/%i/%i",
					&p[0],&t[0],&n[0],
					&p[1],&t[1],&n[1],
					&p[2],&t[2],&n[2]);
			}
		}
	} else
	{
		sscanf(m_strCurrLine,"f %i %i %i",&p[0],&p[1],&p[2]);
	}
	if( normal ) m_bCurrHasNormal = true;
	if( texCoord) m_bCurrHasTextures = true;
	for(int i=0;i<3;i++)
	{
		if( normal )
			m_oCreator.Normal( m_af3normals[n[i]-1] );
		if( texCoord)
			m_oCreator.Texture( m_af2textures[t[i]-1] );
		m_oCreator.Point( m_af3Positions[p[i]-1] );
	}
	return true;
}

/// MTL
bool CObjLoader::ParseMtl()
{
	int l = strlen(m_strFilename);
	char *filename = new char[l+5];
	strcpy(filename,m_strFilename);
	strcat(filename,".mtl");

	ILog::Message("parsing %s...\n", filename);
	FILE *f=fopen(filename,"rt");
	if(f == NULL)
	{
		ILog::Error("file not found\n");
		return false;
	}

	while(1)
	{
		fgets(m_strCurrLine,1024,f);
		if(feof(f)) break;
		switch(m_strCurrLine[0])
		{
		case 'K': ManageMtl_K(); break;
		case 'N': ManageMtl_N(); break;
		case 'm': ManageMtl_m(); break;
		case 'n': ManageMtl_n(); break;
		}
	}
	fclose(f);
	return true;
}

void CObjLoader::ManageMtl_K()
{
	if(m_strCurrLine[1]=='d')
	{
		sscanf(m_strCurrLine,"Kd %f %f %f",
			&m_aMaterials.back().f4Color.x(),
			&m_aMaterials.back().f4Color.y(),
			&m_aMaterials.back().f4Color.z());
	} else
	if(m_strCurrLine[1]=='s')
	{
		sscanf(m_strCurrLine,"Ks %f %f %f",
			&m_aMaterials.back().f4Specular.x(),
			&m_aMaterials.back().f4Specular.y(),
			&m_aMaterials.back().f4Specular.z());
	}
	// ignore: Ka - Ke
}

void CObjLoader::ManageMtl_N()
{
	if(m_strCurrLine[1]=='s')
	{
		float t;
		sscanf(m_strCurrLine,"Ns %f",&t);
		m_aMaterials.back().f4Specular.w() = t/ 100.f;
	}
}

void CObjLoader::ManageMtl_m()
{
	if(memcmp(m_strCurrLine,"map_K",5)==0)
	{
		if(m_strCurrLine[5] == 'd')
		{
			char *name=strtok(m_strCurrLine," \r\n");
			name=strtok(NULL,"\r\n");
			CImage *pImage = CImage::LoadFile(name);
			if( pImage )
			{
				m_aMaterials.back().glDiffuseTex = pImage->GetTexture2D();
				delete pImage;
			}
		}
		// ignore map_Ks - map_Ka - map_Ke
	}
}

void CObjLoader::ManageMtl_n()
{
	if(strncmp(m_strCurrLine,"newmtl",6) == 0)
	{
		char *name=strtok(m_strCurrLine," \r\n");
		name=strtok(NULL,"\r\n");

		int l = strlen(name);
		m_aMaterials.push_back( CMaterial::Data() );
		m_astrMaterialNames.push_back(new char[l+1]);
		strcpy(m_astrMaterialNames.back(),name);
		ILog::Message("adding:%s\n", name);
	}
}
