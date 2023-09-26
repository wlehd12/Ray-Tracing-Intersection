#ifndef __FACE_H__
#define __FACE_H__

#pragma once
#include "Vertex.h"

class Face
{
public:
	int				m_Index;
	Vec3<double>	m_Normal;
	vector<Vertex*>	m_Vertices;
public:
	Face();
	Face(int index, Vertex *v0, Vertex *v1, Vertex * v2)
	{
		m_Index = index;
		m_Vertices.push_back(v0);
		m_Vertices.push_back(v1);
		m_Vertices.push_back(v2);
	}
	~Face();
};

#endif

