// Rsm.cpp: implementation of the Rsm class.
//
//////////////////////////////////////////////////////////////////////

#include "Rsm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RSM::RSM()
{
	ntextures = 0;
	nmeshes = 0;
	tmp = 0;
}

void RSM::Save(char *filename)
{
	int fin = 0;
	FILE *fp;
	fp = fopen(filename, "wb");

	fwrite(&header, sizeof(rsm_header_t), 1, fp);

	fwrite(&ntextures, sizeof(int), 1, fp);

	fwrite(textures_name, sizeof(ro_string_t), ntextures, fp);

	for (int i = 0; i < nmeshes; i++)
	{
		meshes[i].Save(fp, (i == 0));
	}

	fwrite(&fin, sizeof(int), 1, fp);
	fwrite(&fin, sizeof(int), 1, fp);

	fclose(fp);
}

bool RSM::GRFLoad(Grf *grffile, char *filename, bool extract)
{
	int i, offset;
	unsigned char *RSMData;
	unsigned long size;

	sprintf(&modelfname[0], "%s", filename);
	strcpy(&modelfname[strlen(filename)], "\0");

	offset = 0;
	RSMData = (unsigned char *)grf_get(grffile, filename, &size, NULL);

	GrfError error;

	if (extract)
	{
		char targfile[512];
		sprintf(targfile, "C:\\Program Files\\RagnarokOffline3\\%s", filename);
		grf_extract(grffile, filename, targfile, NULL);
	}

	if (!RSMData)
		return FALSE;

	memcpy(&header, &RSMData[offset], sizeof(rsm_header_t));
	offset += sizeof(rsm_header_t);
	memcpy(&ntextures, &RSMData[offset], sizeof(int));
	offset += sizeof(int);

	textures = new GLuint[ntextures];
	alphatex = new bool[ntextures];
	textures_name = new ro_string_t[ntextures];

	memcpy(textures_name, &RSMData[offset], sizeof(ro_string_t) * ntextures);
	offset += sizeof(ro_string_t) * ntextures;

	glGenTextures(ntextures, textures);

	for (i = 0; i < ntextures; i++)
	{
		LoadGRFTexture(grffile, textures_name[i], textures[i], &alphatex[i], extract);
	}
	meshes = new RSM_Mesh[30];

	while (data_status(&offset, RSMData) != 0)
	{
		meshes[nmeshes].GRFLoad(&offset, RSMData, textures, alphatex, (nmeshes == 0));
		nmeshes++;
		if (nmeshes > 29)
			break;
	}

	if (nmeshes == 1)
		meshes[0].only = true;
	else
		meshes[0].only = false;

	father = new int[nmeshes];
	father[0] = 0;
	for (i = 0; i < nmeshes; i++)
		for (int j = 0; j < nmeshes; j++)
			if ((j != i) && (!strcmp(meshes[j].parent_name, meshes[i].name)))
				father[j] = i;

	BoundingBox();

	return true;
}

bool RSM::Load(char *ragnapath, char *filename)
{
	FILE *fp;
	int i;

	fp = fopen(filename, "rb");

	if (!fp)
	{
		return false;
	}

	fread(&header, sizeof(rsm_header_t), 1, fp);

	fread(&ntextures, sizeof(int), 1, fp);

	textures = new GLuint[ntextures];
	alphatex = new bool[ntextures];
	textures_name = new ro_string_t[ntextures];

	fread(textures_name, sizeof(ro_string_t), ntextures, fp);

	glGenTextures(ntextures, textures);

	for (i = 0; i < ntextures; i++)
	{
		LoadTexture(ragnapath, textures_name[i], textures[i], &alphatex[i]);
	}
	meshes = new RSM_Mesh[30];

	while (file_status(fp) != 0)
	{
		meshes[nmeshes].Load(fp, textures, alphatex, (nmeshes == 0));
		nmeshes++;
		if (nmeshes > 29)
			break;
	}

	if (nmeshes == 1)
		meshes[0].only = true;
	else
		meshes[0].only = false;

	fclose(fp);

	father = new int[nmeshes];
	father[0] = 0;
	for (i = 0; i < nmeshes; i++)
		for (int j = 0; j < nmeshes; j++)
			if ((j != i) && (!strcmp(meshes[j].parent_name, meshes[i].name)))
				father[j] = i;

	BoundingBox();

	return true;
}

void RSM::BoundingBox()
{
	int i;

	meshes[0].BoundingBox();
	for (i = 1; i < nmeshes; i++)
		if (father[i] == 0)
			meshes[i].BoundingBox(&meshes[0].transf);

	for (i = 0; i < 3; i++)
	{
		box.max[i] = meshes[0].max[i];
		box.min[i] = meshes[0].min[i];
		for (int j = 1; j < nmeshes; j++)
		{
			if (father[j] == 0)
			{
				box.max[i] = MAX(meshes[j].max[i], box.max[i]);
				box.min[i] = MIN(meshes[j].min[i], box.min[i]);
			}
		}
		box.range[i] = (box.max[i] + box.min[i]) / 2.0;
	}
}

void RSM::DrawAxes(ro_position_t pos)
{
	CVector3 vBox[8];
	GLfloat Mat[16];
	GLfloat Rot[16];

	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);

	glRotatef(pos.rx, 1.0, 0.0, 0.0);
	glRotatef(pos.ry, 0.0, 1.0, 0.0);
	glRotatef(pos.rz, 0.0, 0.0, 1.0);

	glScalef(pos.sx, -pos.sy, pos.sz);

	Rot[0] = meshes[0].transf.todo[0];
	Rot[1] = meshes[0].transf.todo[1];
	Rot[2] = meshes[0].transf.todo[2];
	Rot[3] = 0.0;

	Rot[4] = meshes[0].transf.todo[3];
	Rot[5] = meshes[0].transf.todo[4];
	Rot[6] = meshes[0].transf.todo[5];
	Rot[7] = 0.0;

	Rot[8] = meshes[0].transf.todo[6];
	Rot[9] = meshes[0].transf.todo[7];
	Rot[10] = meshes[0].transf.todo[8];
	Rot[11] = 0.0;

	Rot[12] = 0.0;
	Rot[13] = 0.0;
	Rot[14] = 0.0;
	Rot[15] = 1.0;

	glScalef(meshes[0].transf.todo[19], meshes[0].transf.todo[20], meshes[0].transf.todo[21]);

	glMultMatrixf(Rot);

	if (!meshes[0].only)
	{
		glTranslatef(-box.range[0], -box.max[1], -box.range[2]);
	}
	else
	{
		glTranslatef(0.0, -box.max[1] + box.range[1], 0.0);
	}

	glRotatef(meshes[0].transf.todo[15] * 180.0 / 3.14159,
			  meshes[0].transf.todo[16], meshes[0].transf.todo[17], meshes[0].transf.todo[18]);

	if (meshes[0].only)
		glTranslatef(-box.range[0], -box.range[1], -box.range[2]);

	if (!meshes[0].only)
		glTranslatef(meshes[0].transf.todo[9], meshes[0].transf.todo[10], meshes[0].transf.todo[11]);

	// need to rotate the bounding box for some reason...
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	/*
			// Draw bounding box
			glBegin(GL_LINE_LOOP);
				glVertex3f (box.min[0],box.min[1],box.min[2]);
				glVertex3f (box.max[0],box.min[1],box.min[2]);
				glVertex3f (box.max[0],box.max[1],box.min[2]);
				glVertex3f (box.min[0],box.max[1],box.min[2]);
			glEnd();

			glBegin(GL_LINE_LOOP);
				glVertex3f (box.min[0],box.min[1],box.min[2]);
				glVertex3f (box.min[0],box.min[1],box.max[2]);
				glVertex3f (box.min[0],box.max[1],box.max[2]);
				glVertex3f (box.min[0],box.max[1],box.min[2]);
			glEnd();

			glBegin(GL_LINE_LOOP);
				glVertex3f (box.min[0],box.min[1],box.min[2]);
				glVertex3f (box.max[0],box.min[1],box.min[2]);
				glVertex3f (box.max[0],box.min[1],box.max[2]);
				glVertex3f (box.min[0],box.min[1],box.max[2]);
			glEnd();
	*/
	// draw the axes of the object
	GLfloat vcenter[3];
	GLfloat vcenterx[3], vcentery[3], vcenterz[3];
	GLfloat vin[3];

	glGetFloatv(GL_MODELVIEW_MATRIX, &Mat[0]);

	vin[0] = 0;
	vin[1] = 0;
	vin[2] = 0;
	MatrixMultVect(Mat, vin, vcenter);

	vin[0] = 10;
	vin[1] = 0;
	vin[2] = 0;
	MatrixMultVect(Mat, vin, vcenterx);

	vin[0] = 0;
	vin[1] = 10;
	vin[2] = 0;
	MatrixMultVect(Mat, vin, vcentery);

	vin[0] = 0;
	vin[1] = 0;
	vin[2] = 10;
	MatrixMultVect(Mat, vin, vcenterz);

	GLfloat savecolor[4];
	glGetFloatv(GL_CURRENT_COLOR, &savecolor[0]);

	glPushMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glLoadIdentity();

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(vcenter[0], vcenter[1], vcenter[2]);
	glVertex3f(vcenterx[0], vcenterx[1], vcenterx[2]);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(vcenter[0], vcenter[1], vcenter[2]);
	glVertex3f(vcentery[0], vcentery[1], vcentery[2]);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(vcenter[0], vcenter[1], vcenter[2]);
	glVertex3f(vcenterz[0], vcenterz[1], vcenterz[2]);

	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glPopMatrix();

	glColor4f(savecolor[0], savecolor[1], savecolor[2], savecolor[3]);

	vBox[0] = MatrixMultVect3f(Mat, box.min[0], box.min[1], box.min[2]);
	vBox[1] = MatrixMultVect3f(Mat, box.max[0], box.min[1], box.min[2]);
	vBox[2] = MatrixMultVect3f(Mat, box.max[0], box.max[1], box.min[2]);
	vBox[3] = MatrixMultVect3f(Mat, box.min[0], box.max[1], box.min[2]);
	vBox[4] = MatrixMultVect3f(Mat, box.min[0], box.min[1], box.max[2]);
	vBox[5] = MatrixMultVect3f(Mat, box.max[0], box.min[1], box.max[2]);
	vBox[6] = MatrixMultVect3f(Mat, box.max[0], box.max[1], box.max[2]);
	vBox[7] = MatrixMultVect3f(Mat, box.min[0], box.max[1], box.max[2]);

	glPushMatrix();

	glLoadIdentity();

	glBegin(GL_LINE_LOOP);
	glVertex3f(vBox[0].x, vBox[0].y, vBox[0].z);
	glVertex3f(vBox[1].x, vBox[1].y, vBox[1].z);
	glVertex3f(vBox[2].x, vBox[2].y, vBox[2].z);
	glVertex3f(vBox[3].x, vBox[3].y, vBox[3].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(vBox[4].x, vBox[4].y, vBox[4].z);
	glVertex3f(vBox[5].x, vBox[5].y, vBox[5].z);
	glVertex3f(vBox[6].x, vBox[6].y, vBox[6].z);
	glVertex3f(vBox[7].x, vBox[7].y, vBox[7].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(vBox[0].x, vBox[0].y, vBox[0].z);
	glVertex3f(vBox[4].x, vBox[4].y, vBox[4].z);
	glVertex3f(vBox[7].x, vBox[7].y, vBox[7].z);
	glVertex3f(vBox[3].x, vBox[3].y, vBox[3].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(vBox[1].x, vBox[1].y, vBox[1].z);
	glVertex3f(vBox[5].x, vBox[5].y, vBox[5].z);
	glVertex3f(vBox[6].x, vBox[6].y, vBox[6].z);
	glVertex3f(vBox[2].x, vBox[2].y, vBox[2].z);
	glEnd();

	glPopMatrix();

	glPopMatrix();
}

void RSM::Display(ro_position_t pos)
{
	CFrustum g_Frustum;

	CVector3 vBox[8];
	GLfloat Mat[16];
	GLfloat Rot[16];

	g_Frustum.CalculateFrustum();

	glPushMatrix();

	// RSW positions are map-centred; Y is negated (RO vs OpenGL convention);
	// Z offset of -10 aligns with GRFEditor's tile-zero origin.
	glTranslatef(pos.x, -pos.y, pos.z - 10.0f);

	glRotatef(-pos.rz, 0.0f, 0.0f, 1.0f);
	glRotatef(-pos.rx, 1.0f, 0.0f, 0.0f);
	glRotatef( pos.ry, 0.0f, 1.0f, 0.0f);

	glScalef(pos.sx, -pos.sy, pos.sz);

	// GRFEditor v1.x: innermost RSW transform centres the RSM geometry.
	// box.min[1] aligns the model's lowest vertex with the RSW anchor point.
	glTranslatef(-box.range[0], box.min[1], -box.range[2]);

	glGetFloatv(GL_MODELVIEW_MATRIX, &Mat[0]);

	vBox[0] = MatrixMultVect3f(Mat, box.min[0], box.min[1], box.min[2]);
	vBox[1] = MatrixMultVect3f(Mat, box.max[0], box.min[1], box.min[2]);
	vBox[2] = MatrixMultVect3f(Mat, box.max[0], box.max[1], box.min[2]);
	vBox[3] = MatrixMultVect3f(Mat, box.min[0], box.max[1], box.min[2]);
	vBox[4] = MatrixMultVect3f(Mat, box.min[0], box.min[1], box.max[2]);
	vBox[5] = MatrixMultVect3f(Mat, box.max[0], box.min[1], box.max[2]);
	vBox[6] = MatrixMultVect3f(Mat, box.max[0], box.max[1], box.max[2]);
	vBox[7] = MatrixMultVect3f(Mat, box.min[0], box.max[1], box.max[2]);

	if (g_Frustum.PointInFrustum(vBox[0].x, vBox[0].y, vBox[0].z) | g_Frustum.PointInFrustum(vBox[1].x, vBox[1].y, vBox[1].z) | g_Frustum.PointInFrustum(vBox[2].x, vBox[2].y, vBox[2].z) | g_Frustum.PointInFrustum(vBox[3].x, vBox[3].y, vBox[3].z) | g_Frustum.PointInFrustum(vBox[4].x, vBox[4].y, vBox[4].z) | g_Frustum.PointInFrustum(vBox[5].x, vBox[5].y, vBox[5].z) | g_Frustum.PointInFrustum(vBox[6].x, vBox[6].y, vBox[6].z) | g_Frustum.PointInFrustum(vBox[7].x, vBox[7].y, vBox[7].z))
	// if (  g_Frustum.PointInFrustum(pos.x, pos.y, pos.z))
	{
		glPushMatrix();

		glLoadIdentity();

		GLfloat savecolor[4];
		glGetFloatv(GL_CURRENT_COLOR, &savecolor[0]);

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex3f(vBox[0].x, vBox[0].y, vBox[0].z);
		glVertex3f(vBox[1].x, vBox[1].y, vBox[1].z);
		glVertex3f(vBox[2].x, vBox[2].y, vBox[2].z);
		glVertex3f(vBox[3].x, vBox[3].y, vBox[3].z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(vBox[4].x, vBox[4].y, vBox[4].z);
		glVertex3f(vBox[5].x, vBox[5].y, vBox[5].z);
		glVertex3f(vBox[6].x, vBox[6].y, vBox[6].z);
		glVertex3f(vBox[7].x, vBox[7].y, vBox[7].z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(vBox[0].x, vBox[0].y, vBox[0].z);
		glVertex3f(vBox[4].x, vBox[4].y, vBox[4].z);
		glVertex3f(vBox[7].x, vBox[7].y, vBox[7].z);
		glVertex3f(vBox[3].x, vBox[3].y, vBox[3].z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(vBox[1].x, vBox[1].y, vBox[1].z);
		glVertex3f(vBox[5].x, vBox[5].y, vBox[5].z);
		glVertex3f(vBox[6].x, vBox[6].y, vBox[6].z);
		glVertex3f(vBox[2].x, vBox[2].y, vBox[2].z);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);

		glColor4f(savecolor[0], savecolor[1], savecolor[2], savecolor[3]);

		glPopMatrix();
	}

	DisplayMesh(&box, 0);

	glPopMatrix();
}

void RSM::DisplayMesh(bounding_box_t *b, int n, ro_transf_t *ptransf)
{
	glPushMatrix();

	meshes[n].Display(b, ptransf);

	for (int i = 0; i < nmeshes; i++)
		if ((i != n) && (father[i] == n))
		{
			DisplayMesh((n == 0) ? b : NULL, i, &meshes[n].transf);
		}
	glPopMatrix();
}

RSM::~RSM()
{
	if (ntextures)
		delete[] textures;
}
