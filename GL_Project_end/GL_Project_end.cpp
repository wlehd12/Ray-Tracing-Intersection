#include <iostream>
#include <math.h>
#include "GL\glut.h"
#include "Mesh.h"


#define PI 3.141592

float zoom = 15.0f;
float rotx = 0;
float roty = 0.001f;
float tx = 0;
float ty = 0;
float t;
float g_p1[3]; //ray tracing의 시작점
float g_p2[3]; //ray tracing의 도착점
int lastx = 0;
int lasty = 0;
int g_h;
int a;
int k;
int n_face[20];
unsigned char Buttons[4] = { 0 };
double u2[3];
double u3[3];

Vec3<double> g[20];
Vec3<double> orig;
Vec3<double> di;
Vec3<double> P;


Mesh *m_Mesh;

//raytracing으로 Mesh와 만났을 때 반응
bool rayTriangleIntersect(Vec3<double> &orig, Vec3<double> &dir, Vec3<double> &v0, Vec3<double> &v1, Vec3<double> &v2)
{
	Vec3<double> v0v1 = v1 - v0;
	Vec3<double> v0v2 = v2 - v0;
	// Face normal을 하여 법선 벡터 N을 구한다
	Vec3<double> N = v0v1.Cross(v0v2);
	float area2 = N.Length();

	float NdotRayDirection = N.Dot(dir);
	// 평행할 경우
	if (fabs(NdotRayDirection) == 0)
		return false;

	float d = -N.Dot(v0);
	t = -(N.Dot(orig) + d) / NdotRayDirection;

	// 세 점으로 만든 삼각형의 평면 방정식 위에 있는 점 p
	// 방향벡터 dir = 1/t * (P - orig)
	P = { (orig.x() + t * dir.x()),(orig.y() + t * dir.y()),(orig.z() + t * dir.z()) };

	Vec3<double> edge0 = v1 - v0;
	Vec3<double> edge1 = v2 - v1;
	Vec3<double> edge2 = v0 - v2;
	Vec3<double> C0 = P - v0;
	Vec3<double> C1 = P - v1;
	Vec3<double> C2 = P - v2;
	
	//외적
	Vec3<double> n1 = edge0.Cross(C0);			
	Vec3<double> n2 = edge1.Cross(C1);
	Vec3<double> n3 = edge2.Cross(C2);

	//P가 Mesh 안에 존재
	if (N.Dot(n1) < 0 || N.Dot(n2) < 0 || N.Dot(n3) < 0)
		return false;

	return true;
}

//ray tracing 의 g_p2[3]도착점,g_p1[3] 시작점 구하기
void ComputeRayDirection(int x, int y)
{
	GLdouble wx, wy, wz;		// 주어진 점의 windows coordinate
	wx = x;
	wy = g_h - y;				// opengl의 y 좌표는 화면의 아래쪽이 0, 가장 윗쪽이 g_h 값을 가진다. 
	wz = 0;

	GLint viewport[4];			// viewport matrix {0,0,width, height} 이렇게 4개의 값을 얻어냄
	GLdouble mvMatrix[16];		// modelview matrix를 저장할 4x4 행렬
	GLdouble projMatrix[16];	// projection matrix를 저장할 4x4 행렬

	glGetIntegerv(GL_VIEWPORT, viewport);			// 각각의 행렬을 얻어내는 함수
	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

	GLdouble px, py, pz;	// wx,wy,wz에 해당하는 world coordinate
	// Unprojection과정
	gluUnProject(wx, wy, wz, mvMatrix, projMatrix, viewport, &px, &py, &pz);

	// 시작점 g_p1 세팅
	g_p1[0] = px;	g_p1[1] = py;	g_p1[2] = pz;

	// 화면에 수직인 방향 성분 u vector를 얻기 위해 
	// 윈도우 좌표 (wx, wy, wz+1)인 점의 world coordinate을 구해봄
	// (즉 화면에 수직으로 1만큼 내려온 점의 좌표임)
	gluUnProject(wx, wy, wz + 1, mvMatrix, projMatrix, viewport, &px, &py, &pz);

	double u[3];				// 화면에 수직인 ray의 방향을 나타내는 vector u
	u[0] = px - g_p1[0];
	u[1] = py - g_p1[1];
	u[2] = pz - g_p1[2];


	//vector u와 수직인 벡터
	gluUnProject(wx , wy + 1, wz, mvMatrix, projMatrix, viewport, &px , &py, &pz);
	u2[0] = px - g_p1[0];
	u2[1] = py - g_p1[1];
	u2[2] = pz - g_p1[2];


	//vector u와 수직인 벡터
	gluUnProject(wx + 1 , wy , wz, mvMatrix, projMatrix, viewport, &px, &py , &pz);
	u3[0] = px - g_p1[0];
	u3[1] = py - g_p1[1]; 
	u3[2] = pz - g_p1[2];


	// 방향 vector를 normalize
	double length_u = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	u[0] /= length_u;
	u[1] /= length_u;
	u[2] /= length_u;

	orig = { g_p1[0], g_p1[1], g_p1[2] };		//ray tracing의 시작점
	di = { u[0], u[1], u[2] };					//ray tracing의 Normal vector
	a = 0;

	// 수직인 방향으로 거리 s=100 인 점의 좌표 g_p2를 계산함
	double s = 100;	//거리 값
	g_p2[0] = g_p1[0] + s * u[0];
	g_p2[1] = g_p1[1] + s * u[1];
	g_p2[2] = g_p1[2] + s * u[2];

	k = 0;
	int numFaces = m_Mesh->m_Faces.size();		//triangle의 갯수
	for (int i = 0; i < numFaces; i++)
	{
		Face *face = m_Mesh->m_Faces[i];
		Vec3<double> q[3];						// triangle의 각 꼭지점 위치
		for (int j = 0; j < 3; j++)
		{
			q[j] = face->m_Vertices[j]->m_Position;
		}

		//레이저가 Mesh 와 부딪쳤을 경우
		if (rayTriangleIntersect(orig, di, q[0], q[1], q[2])) {
			printf("\n");
			a = 1.0f;
			k++;
			g[k] = P;
			n_face[k] = i;
			printf("P : %f %f %f\n", P.x(), P.y(), P.z());
			printf("m_face number[i] : %d\n", i);
			//printf("q[0] : %f %f %f\n", q[0].x(), q[0].y(), q[0].z());
			//printf("q[1] : %f %f %f\n", q[1].x(), q[1].y(), q[1].z());
			//printf("q[2] : %f %f %f\n", q[2].x(), q[2].y(), q[2].z());
			printf("\n");
		}
		
	}
	printf("점P의 갯수 k : %d\n", k);


	//raytracing 에 부딪친 점P들 중에 시작점에 가장 가까운 점P 찾기
	if (k > 0) {		
		if (k > 1) {
			for (int p = 1; p < k; p++) {
				//ray tracing 시작점 g_p1에서 점P까지의 길이 비교
				float a1 = sqrt(pow(g[p].x() - g_p1[0], 2) + pow(g[p].y() - g_p1[1], 2) + pow(g[p].z() - g_p1[2], 2));
				float a2 = sqrt(pow(g[p + 1].x() - g_p1[0], 2) + pow(g[p + 1].y() - g_p1[1], 2) + pow(g[p + 1].z() - g_p1[2], 2));
				// 길이 최소값 구하기
				if (a1 < a2) {
					g[p + 1] = g[p];
					n_face[p + 1] = n_face[p];
				}					
			}
		}
		g_p2[0] = g[k].x();
		g_p2[1] = g[k].y();
		g_p2[2] = g[k].z();

		printf("n_face : %d\n", n_face[k]);
		printf("g[k] : %f %f %f\n", g[k].x(), g[k].y(), g[k].z());	//점P 최종값
		printf("g_p1 : %f %f %f\n", g_p1[0], g_p1[1], g_p1[2]);		// ray tracing 시작점

		printf("u : %f %f %f\n", u[0], u[1], u[2]);
		printf("u2 : %f %f %f\n", u2[0], u2[1], u2[2]);
		printf("u3 : %f %f %f\n", u3[0], u3[1], u3[2]);
	}
	printf("-----end-----\n");
}

void Init(void)
{
	glEnable(GL_DEPTH_TEST);
}

void Lighting(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void circle(void)
{
	if (k > 0) {
		glColor3f(0.0f, 0.0f, 1.0f);
		//3차원좌표에서 원의 방정식 = r * cos(θ) * u + r * sin(θ) * (n * u) + 원의 중심
		//r = 반지름 , u는 n과 수직인 벡터, n 는 단위 법선벡터 ,(n * u)는 n과 u 모두 수직인 벡터
		glPointSize(5.0);
		glBegin(GL_POINTS);
		for (int j = 0; j < 5; j++) {
			for (int i = 0; i < 24; i++) {
				float r = j * 1000;
				GLfloat x1 = r * (u3[0] * cos(48 * PI / i) + u2[0] * sin(48 * PI / i)) + (g[k].x() - j * di.x());
				GLfloat y1 = r * (u3[0] * cos(48 * PI / i) + u2[1] * sin(48 * PI / i)) + (g[k].y() - j * di.y());
				GLfloat z1 = r * (u3[0] * cos(48 * PI / i) + u2[2] * sin(48 * PI / i)) + (g[k].z() - j * di.z());
				glVertex3f(x1, y1, z1);
			}
		}
		glEnd();
	}
}

void RenderRay(void)
{
	glLineWidth(3);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, a, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(g_p1);
	glVertex3fv(g_p2);
	glEnd();
	glPointSize(3);
	glBegin(GL_POINTS);
	glVertex3fv(g_p1);
	glVertex3fv(g_p2);
	glEnd();

}


void Rendering(void)
{
	Lighting();
	m_Mesh->Draw();
	RenderRay();
	circle();
	glDisable(GL_LIGHTING);
}

void Display(void)
{
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(0, 0, -zoom);
	glTranslatef(tx, ty, 0);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);

	Rendering();

	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	if (w == 0) {
		h = 1;
	}
	g_h = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Motion(int x, int y)
{
	int diffx = x - lastx;
	int diffy = y - lasty;
	lastx = x;
	lasty = y;

	if (Buttons[0]) {
		rotx += (float) 0.5f * diffy;
		roty += (float) 0.5f * diffx;
	}
	else if (Buttons[1]) {
		tx += (float) 0.05f * diffx;
		ty -= (float) 0.05f * diffy;
	}

	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	lastx = x;
	lasty = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		Buttons[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		Buttons[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			ComputeRayDirection(x, y);
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':		//화면 확대
		zoom -= (float)1.0f;
		break;
	case '2':		//화면 축소
		zoom += (float)1.0f;
		break;
	case 'Q':
	case 'q':
		exit(0);
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	m_Mesh = new Mesh((char*)"OBJ\\dragon.obj");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Raytracing from view");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	Init();
	glutMainLoop();
}

