#include "Mesh.h"
#include "GL\glut.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::LoadObj(char *file)
{
	FILE *fp;

	int index[3], id;
	char buffer[256] = { 0 };
	Vec3<double> pos, minBoundary, maxBoundary;
	minBoundary.Set(DBL_MAX);
	maxBoundary.Set(DBL_MIN);

	// Read vertex
	id = 0;
	fopen_s(&fp, file, "r");
	while (fscanf(fp, "%s %lf %lf %lf", &buffer, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (buffer[0] == 'v' && buffer[1] == NULL) {
			if (minBoundary[0] > pos[0])	minBoundary[0] = pos[0];
			if (minBoundary[1] > pos[1])	minBoundary[1] = pos[1];
			if (minBoundary[2] > pos[2])	minBoundary[2] = pos[2];
			if (maxBoundary[0] < pos[0])	maxBoundary[0] = pos[0];
			if (maxBoundary[1] < pos[1])	maxBoundary[1] = pos[1];
			if (maxBoundary[2] < pos[2])	maxBoundary[2] = pos[2];
			m_Vertices.push_back(new Vertex(id++, pos));
		}
	}
	printf("Number of vertex : %d\n", m_Vertices.size());

	// Read face
	id = 0;
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %d %d %d", &buffer, &index[0], &index[1], &index[2]) != EOF) {
		if (buffer[0] == 'f' && buffer[1] == NULL) {
			m_Faces.push_back(new Face(id++, m_Vertices[index[0] - 1], m_Vertices[index[1] - 1], m_Vertices[index[2] - 1]));
		}
	}
	printf("Number of face : %d\n", m_Faces.size());
	fclose(fp);

	MoveToCenter(minBoundary, maxBoundary, 5.0);
	BuildList();
	ComputeNormal();
}

void Mesh::BuildList(void)
{
	int numberFaces = m_Faces.size();

	for (int i = 0; i < numberFaces; i++) {
		Face *f = m_Faces[i];
		for (int j = 0; j < 3; j++) {
			Vertex *v = f->m_Vertices[j];
			v->m_NeighborFaces.push_back(f);
		}
	}
}

void Mesh::ComputeNormal(void)
{
	// Face normal
	int numberFaces = m_Faces.size();
	for (int i = 0; i < numberFaces; i++) {
		Face *f = m_Faces[i];
		Vec3<double> a = f->m_Vertices[1]->m_Position - f->m_Vertices[0]->m_Position;
		Vec3<double> b = f->m_Vertices[2]->m_Position - f->m_Vertices[0]->m_Position;
		f->m_Normal = a.Cross(b);
		f->m_Normal.Normalize();
	}

	// Vertex normal
	int numVertices = (int)m_Vertices.size();
	for (int i = 0; i < numVertices; i++) {
		Vertex *v = m_Vertices[i];
		int numNeighborFaces = (int)v->m_NeighborFaces.size();
		for (int j = 0; j < numNeighborFaces; j++) {
			v->m_Normal += v->m_NeighborFaces[j]->m_Normal;
		}
		v->m_Normal /= numNeighborFaces;
	}

}

void Mesh::MoveToCenter(Vec3<double> minPos, Vec3<double> maxPos, double scale)
{
	double longestAxis = fmax(fmax(fabs(maxPos.x() - minPos.x()), fabs(maxPos.y() - minPos.y())), fabs(maxPos.z() - minPos.z()));
	Vec3<double> center = (maxPos + minPos) / 2.0;
	Vec3<double> position;
	Vec3<double> vector;
	Vec3<double> newCenter(0.0, 0.0, 0.0);

	int numVertices = (int)m_Vertices.size();

	for (int i = 0; i < numVertices; i++) {
		Vertex *v = m_Vertices[i];
		position = v->m_Position;
		vector = position - center;
		vector /= longestAxis;
		vector *= scale;
		position = newCenter;
		position += vector;
		v->m_Position = position;
	}
}

void Mesh::DrawSolid(void)
{
	glPushMatrix();
	int numberFaces = m_Faces.size();
	for (int i = 0; i < numberFaces; i++) {
		Face *f = m_Faces[i];
		Vec3<double> p[3], n[3];
		for (int j = 0; j < 3; j++) {
			p[j] = f->m_Vertices[j]->m_Position;
			n[j] = f->m_Vertices[j]->m_Normal;
		}

		glBegin(GL_POLYGON);
		for (int j = 0; j < 3; j++) {
			glNormal3f((GLfloat)n[j].x(), (GLfloat)n[j].y(), (GLfloat)n[j].z());
			glVertex3f((GLfloat)p[j].x(), (GLfloat)p[j].y(), (GLfloat)p[j].z());
		}
		glEnd();

	}
	glPopMatrix();
}

void Mesh::DrawWire(void)
{
	glPushMatrix();
	int numberFaces = m_Faces.size();
	for (int i = 0; i < numberFaces; i++) {
		Face *f = m_Faces[i];
		Vec3<double> p[3], n[3];
		for (int j = 0; j < 3; j++) {
			p[j] = f->m_Vertices[j]->m_Position;
			n[j] = f->m_Vertices[j]->m_Normal;
		}
		glBegin(GL_LINES);
		for (int j = 0; j < 3; j++) {
			glNormal3f((GLfloat)n[j % 3].x(), (GLfloat)n[j % 3].y(), (GLfloat)n[j % 3].z());
			glVertex3f((GLfloat)p[j % 3].x(), (GLfloat)p[j % 3].y(), (GLfloat)p[j % 3].z());
			glNormal3f((GLfloat)n[(j + 1) % 3].x(), (GLfloat)n[(j + 1) % 3].y(), (GLfloat)n[(j + 1) % 3].z());
			glVertex3f((GLfloat)p[(j + 1) % 3].x(), (GLfloat)p[(j + 1) % 3].y(), (GLfloat)p[(j + 1) % 3].z());
		}
		glEnd();
	}
	glPopMatrix();
}

void Mesh::Draw(int type)
{
	if (type == 0) {
		DrawSolid();
	} else if (type == 1) {
		DrawWire();
	}
}