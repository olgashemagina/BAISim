//---------------------------------------------------------------------------

#ifndef Cube3DH
#define Cube3DH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
typedef struct
{
	float x;
	float y;
	float z;
}T3DPoint;

class T3DTransform
{
protected:

    float angleX;
    float angleY;
	float angleZ;

	float startX;
	float startY;

	float Zoom;

	void SetZoom(float v);

public:
	T3DTransform();
	~T3DTransform();

	TPoint GetProjection(T3DPoint);

	__property float ax = {read = angleX, write = angleX};
	__property float ay = {read = angleY, write = angleY};
	__property float az = {read = angleZ, write = angleZ};
	__property float sx = {read = startX, write = startX};
	__property float sy = {read = startY, write = startY};
	__property float zoom = {read = Zoom, write = SetZoom};
};

class TCube
{
protected:
	T3DTransform* m_t;
	T3DPoint	  m_v[8];

	T3DPoint GetVertex(int index)
	{
		return m_v[index];
	}
	void SetVertex(int index, T3DPoint v)
	{
		m_v[index] = v;
	}
	bool m_DrawText;
	TColor m_color;
public:
	TCube(T3DTransform* t);
	void Draw(TCanvas* pCanvas);
	void ConvertTo(void (*convert)(double, double, double, double&, double&, double&));
	__property bool DrawText = {read = m_DrawText, write = m_DrawText};
	__property T3DPoint vertex[int index] = {read = GetVertex, write = SetVertex};
	__property TColor color = {read = m_color, write = m_color};
};
#endif
