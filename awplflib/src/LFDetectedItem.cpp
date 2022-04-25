/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                  Locate Framework  Computer Vision Library
//
// Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the NN-Videolab.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Locate Framework  (LF) Computer Vision Library.
//		File: LFDetectedItem.cpp
//		Purpose: implementation TLFDetectedItem
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"
#define LF_DEFAULT_HEALTH 25
//-----------------------detect item --------------
TLFDetectedItem::TLFDetectedItem()
{
	this->m_bw = 10;
	this->m_bh = 10;
	this->m_Raiting = 0;
	this->m_racurs = 0;
	this->m_pRect = new TLFRect();
	this->m_type = "unknown";
	this->m_angle = 0;
	this->m_strDetectorName = "";
	this->m_hasObject = true;
	memset(&m_id, 0, sizeof(UUID));
	this->m_predictor = NULL;
	m_health = LF_DEFAULT_HEALTH;
	m_state = 1;
	m_cluster_idx = -1;
	this->m_strComment = "";
	this->m_zone = NULL;
}

TLFDetectedItem::TLFDetectedItem(awpRect* pRect,
									double raiting,
									std::string type,
									int angle,
									int racurs,
									int bw,
									int bh,
									string strDetector,
									UUID id,
									ILFPredictor* predictor) : TLFObject()
{
	this->m_pRect = new TLFRect();
	if (pRect != NULL)
		m_pRect->SetRect(*pRect);
	this->m_Raiting = raiting;
	this->m_type = type;
	this->m_angle = angle;
	this->m_racurs = racurs;
	this->m_bh = bh;
	this->m_bw = bw;
	this->m_strDetectorName = strDetector;
	this->m_hasObject = true;
	memcpy(&this->m_id, &id, sizeof(UUID));
	this->m_predictor = predictor;
	this->m_health = LF_DEFAULT_HEALTH;
	m_state = 1;
	m_cluster_idx = -1;
	this->m_strComment = "";
	this->m_zone = NULL;
}

TLFDetectedItem::TLFDetectedItem(TLFDetectedItem& item) : TLFObject()
{
	this->m_pRect = new TLFRect(*item.m_pRect);
	this->m_Raiting = item.m_Raiting;
	this->m_type = item.m_type;
	this->m_angle = item.m_angle;
	this->m_racurs = item.m_racurs;
	this->m_bh = item.m_bh;
	this->m_bw = item.m_bh;
	this->m_strDetectorName = item.m_strDetectorName;
	this->m_hasObject = true;
	memcpy(&this->m_id, &item.m_id, sizeof(UUID));
	this->m_predictor = item.m_predictor;
	this->m_health = item.m_health;
	m_state = 1;
	m_cluster_idx = item.m_cluster_idx;
	this->m_strComment = item.m_strComment;
	TLFZone* z = item.GetZone();
	if (z != NULL) {
		this->m_zone = new TLFZone(*z);
		delete z;
	}
	else
		m_zone = NULL;
}

TLFDetectedItem::TLFDetectedItem(TLFDetectedItem* item) : TLFObject()
{
	this->m_pRect = new TLFRect(*item->m_pRect);
	this->m_Raiting = item->m_Raiting;
	this->m_type = item->m_type;
	this->m_angle = item->m_angle;
	this->m_racurs = item->m_racurs;
	this->m_bh = item->m_bh;
	this->m_bw = item->m_bw;
	this->m_strDetectorName = item->m_strDetectorName;
	this->m_hasObject = item->m_hasObject;
	memcpy(&this->m_id, &item->m_id, sizeof(UUID));
	this->m_predictor = item->m_predictor;
	this->m_health = item->m_health;
	m_state = 1;
	m_cluster_idx = item->m_cluster_idx;
	this->m_strComment = item->m_strComment;
	TLFZone* z = item->GetZone();
	if (z != NULL) {
		this->m_zone = new TLFZone(*z);
		delete z;
	}
	else
		m_zone = NULL;
}

TLFDetectedItem::~TLFDetectedItem()
{
	if (this->m_pRect != NULL)
		delete m_pRect;
	if (this->m_predictor != NULL)
		delete m_predictor;
}
/*data exchange*/
TLFRect*	TLFDetectedItem::GetBounds()
{
	return this->m_pRect;
}
double		TLFDetectedItem::GetRaiting()
{
	return this->m_Raiting;
}

void		TLFDetectedItem::SetRaiting(double value)
{
	this->m_Raiting = value;
}

int  TLFDetectedItem::GetClusterIdx()
{
	return this->m_cluster_idx;
}
void TLFDetectedItem::SetClusterIdx(int value)
{
	this->m_cluster_idx = value;
}

std::string			TLFDetectedItem::GetType()
{
	return this->m_type;
}

std::string TLFDetectedItem::GetComment()
{
	return this->m_strComment;
}


void TLFDetectedItem::SetType(const char* value)
{
	m_type = value;
}

void		TLFDetectedItem::SetComment(const char* value)
{
	m_strComment = value;
}

int TLFDetectedItem::GetAngle()
{
	return this->m_angle;
}

void TLFDetectedItem::SetAngle(int value)
{
	m_angle = value;
}


int TLFDetectedItem::GetState()
{
	return this->m_state;
}

void TLFDetectedItem::SetState(int state)
{
	m_state = state;
}

int TLFDetectedItem::GetRacurs()
{
	return this->m_racurs;
}

void TLFDetectedItem::GetId(UUID& id)
{
#ifdef WIN32
	memcpy(&id, &this->m_id, sizeof(UUID));
#else
	memcpy(&id, this->m_id, sizeof(UUID));
#endif
}


void TLFDetectedItem::SetRacurs(int value)
{
	m_racurs = value;
}
int   TLFDetectedItem::GetBW()
{
	return this->m_bw;
}
int   TLFDetectedItem::GetBH()
{
	return this->m_bh;
}
TiXmlElement* TLFDetectedItem::SaveXML()
{
	if (this->m_pRect == NULL)
		return NULL;
	awpRect r = this->m_pRect->GetRect();

	TiXmlElement* elem = new TiXmlElement(this->GetName());
	elem->SetAttribute("DetectorName", this->m_strDetectorName.c_str());
	elem->SetDoubleAttribute("Raiting", this->m_Raiting);
	elem->SetAttribute("Type", this->m_type.c_str());
	elem->SetAttribute("Angle", this->m_angle);
	elem->SetAttribute("Racurs", this->m_racurs);
	elem->SetAttribute("Bw", this->m_bw);
	elem->SetAttribute("Bh", this->m_bh);
	elem->SetAttribute("left", r.left);
	elem->SetAttribute("top", r.top);
	elem->SetAttribute("right", r.right);
	elem->SetAttribute("bottom", r.bottom);
	elem->SetAttribute("Comment", m_strComment.c_str());
	TLFZone* z = GetZone();
	if (z != NULL)
	{
		z->SaveXML(elem);
		delete z;
	}

	return elem;
}

bool TLFDetectedItem::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	if (strcmp(parent->Value(), GetName()) != 0)
		return false;

	this->m_strDetectorName = parent->Attribute("DetectorName");
	double v;
	parent->QueryDoubleAttribute("Raiting", &v);
	this->m_Raiting = v;
	m_type = parent->Attribute("Type");
	parent->Attribute("Angle", &this->m_angle);
	parent->Attribute("Racurs", &this->m_racurs);
	parent->Attribute("Bw", &this->m_bw);
	parent->Attribute("Bh", &this->m_bh);
	if (parent->Attribute("Comment") != NULL)
	{
		m_strComment = parent->Attribute("Comment");
	}

	awpRect r;
	int iv;
	parent->Attribute("left", &iv);
	r.left = iv;
	parent->Attribute("top", &iv);
	r.top = iv;
	parent->Attribute("right", &iv);
	r.right = iv;
	parent->Attribute("bottom", &iv);
	r.bottom = iv;

	if (this->m_pRect == NULL)
		this->m_pRect = new TLFRect();

	this->m_pRect->SetRect(r);

	TiXmlElement* child = parent->FirstChildElement();
	if (child)
	{
		if (m_zone == NULL)
			m_zone = new TLFZone();
        m_zone->LoadXML(child);
	}

	return true;
}

void TLFDetectedItem::Resize(double factor)
{
	TLFRect* rect = this->GetBounds();
	if (rect != NULL)
	{
		awpRect  awpr = rect->GetRect();
		awpr.left = (AWPSHORT)floor(awpr.left*factor + 0.5);
		awpr.right = (AWPSHORT)floor(awpr.right*factor + 0.5);
		awpr.top = (AWPSHORT)floor(awpr.top*factor + 0.5);
		awpr.bottom = (AWPSHORT)floor(awpr.bottom*factor + 0.5);
		rect->SetRect(awpr);
	}
}

void TLFDetectedItem::Flip(int w)
{
	TLFRect* rect = this->GetBounds();
	awpRect  awpr = rect->GetRect();
	AWPSHORT l, r;
	l = w - awpr.right;
	r = w - awpr.left;
	awpr.left = l;
	awpr.right = r;

	rect->SetRect(awpr);
}


void TLFDetectedItem::Rotate(awpPoint c, int angle)
{
	TLFRect* rect1 = this->GetBounds();
	awpRect  awpr = rect1->GetRect();

	double fi = angle*AWP_PI / 180.;

	// rotate rect 
	awpPoint p;
	int w, h;
	p.X = awpr.left;
	p.Y = awpr.top;
	w = awpr.right - awpr.left;
	h = awpr.bottom - awpr.top;
	p.X = int(c.X + (p.X - c.X)*cos(fi) + (p.Y - c.Y)*sin(fi));
	p.Y = int(c.Y - (p.X - c.X)*sin(fi) + (p.Y - c.Y)*cos(fi));
	awpr.left = p.X;
	awpr.right = p.X + w;
	awpr.top = p.Y;
	awpr.bottom = p.Y + h;
	rect1->SetRect(awpr);
}

void		TLFDetectedItem::SetBounds(awpRect& rect, int iw, int ih)
{
	if (this->m_pRect == NULL)
	{
		m_pRect = new TLFRect();
	}
	m_pRect->SetRect(rect);

	// setup zone
	awp2DPoint p1;
	awp2DPoint p2;
	p1.X = 100.0*(double)rect.left / (double)iw;
	p1.Y = 100.0*(double)rect.top / (double)ih;
	p2.X = 100.0*(double)rect.right / (double)iw;
	p2.Y = 100.0*(double)rect.bottom / (double)ih;
	TLF2DRect zr(p1,p2);
	if (m_zone != NULL)
	{
		delete m_zone;
	}

    m_zone = new TLFZone(zr);

}

TLFRect* TLFDetectedItem::Predict(ILFDetectEngine* engine)
{
	if (engine == NULL)
		return NULL;
	if (this->m_predictor == NULL)
		return NULL;
	if (this->m_state == 3) {
		return NULL;
	}
	return this->m_predictor->Predict(engine);
}

void TLFDetectedItem::Update(ILFDetectEngine* engine, TLFRect* rect)
{
	if (engine == NULL || m_predictor == NULL)
		return;

	ILFScanner* s = engine->GetScanner(0);

	if (rect == NULL && m_predictor->GetPredicted() == NULL)
		this->m_state = 3;
	else if (rect == NULL)
	{
		TLFRect* predicted = m_predictor->GetPredicted();
		if (predicted != NULL)
		{
			this->m_pRect->SetRect(predicted->GetRect());
			this->m_state = 2;
		}
		else
			this->m_state = 3;
	}
	else
	{
		TLFRect* predicted = m_predictor->GetPredicted();
		if (predicted != NULL)
		{
			this->m_pRect->SetRect(predicted->GetRect());
			m_state = 2;
		}
		else
		{
			this->m_predictor->Learn(engine, rect);
			this->m_pRect->SetRect(rect->GetRect());
			m_state = 1;
		}
	}
}

int TLFDetectedItem::GetHealth()
{
	return this->m_health;
}

void TLFDetectedItem::Delete()
{
	m_health = 0;
	m_state = 3;
}

int TLFDetectedItem::GetColor()
{
	return m_color;
}
void TLFDetectedItem::SetColor(int color)
{
	m_color = color;
}

const char*       TLFDetectedItem::GetDetectorName()
{
	return  m_strDetectorName.c_str();
}

void TLFDetectedItem::SetDetectorName(const char* lpName)
{
    m_strDetectorName = lpName;
}


bool TLFDetectedItem::HasObject()
{
	return this->m_hasObject;
}

void TLFDetectedItem::SetHasObject(bool v)
{
	this->m_hasObject = v;
	if (!m_hasObject)
	{
		m_cluster_idx = -1;
	}
}


void TLFDetectedItem::SetZone(TLFZone* zone, int w, int h)
{
	if (m_zone != NULL)
		delete m_zone;
	m_zone = new TLFZone(*zone);
	// update m_pRect with new settings
	TLF2DRect* _rect2D = m_zone->GetBounds();
	if (_rect2D != NULL)
	{
		awpRect _r;
		_r.left = floor(_rect2D->GetLeftTop().X*w / 100. + 0.5);
		_r.top = floor(_rect2D->GetLeftTop().Y*h / 100. + 0.5);

		_r.right = floor(_rect2D->GetRightBottom().X*w / 100. + 0.5);
		_r.bottom = floor(_rect2D->GetRightBottom().Y*h / 100. + 0.5);
		m_pRect->SetRect(_r);
		delete _rect2D;
	}
}

TLFZone* TLFDetectedItem::GetZone()
{
	if (m_pRect == NULL)
		return NULL;
	if (m_zone == NULL)
	{
	  //TLF2DRect _rect(m_pRect->GetRect());
	  //return new TLFZone(_rect);
	  return NULL;
	}
	else
	   return new TLFZone(*m_zone);
}
