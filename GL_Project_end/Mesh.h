#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "Face.h"

class Mesh
{
public:
	vector<Vertex*>	m_Vertices;
	vector<Face*>	m_Faces;
public:
	Mesh();
	Mesh(char *file)
	{
		LoadObj(file);
	}
	~Mesh();
public:
	void	LoadObj(char *file);
	void	BuildList(void);
	void	ComputeNormal(void);
	void	MoveToCenter(Vec3<double> minPos, Vec3<double> maxPos, double scale = 1.0);
public:
	void	Draw(int type = 0);
	void	DrawSolid(void);
	void	DrawWire(void);
};

#endif