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
//		File: LFFaceDescriptor.cpp
//		Purpose: implementation of TLFFaceImageDescriptor
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#include "_LF.h"
#pragma hdrstop

const awp2DPoint FaceGraph1[24] =
{
{-0.27954945,	0.062898626},
{0.258583241,	0.076876099},
{0.740806042,	0.05590989 },
{1.285927469,	0.020966209},
{0.202673351,	0.621997526},
{0.531143955,	0.520187746},
{0.824670877,	0.580065108},
{0.104831044,	1.104220327},
{0.943479393,	1.055299173},
{-0.496200273,	0.677907416},
{-0.384380493,	1.125186535},
{-0.272560714,	1.411724722},
{-0.027954945,	1.642353018},
{0.531143955,	1.761161534},
{1.076265382,	1.684285435},
{1.502578293,	1.362803568},
{1.6004206,	1.020355492     },
{1.628375545,	0.5590989   },
{-0.426312911,	-0.244605769},
{-0.076876099,	-0.342448076},
{0.27954945,	-0.216650824},
{0.656941207,	-0.272560714},
{0.999389283,	-0.412335438},
{1.390758513,	-0.258583241}
};

TLFFaceImageDescriptor::TLFFaceImageDescriptor()
{
    memset(m_Triangles, 0, 32*sizeof(TLFTriangle*));
    Reset();
}

TLFFaceImageDescriptor::TLFFaceImageDescriptor(const char* lpFileName)
{
      memset(m_Triangles, 0, 32*sizeof(TLFTriangle*));
      LoadFromFile(lpFileName);
}
TLFFaceImageDescriptor::~TLFFaceImageDescriptor()
{
    Reset();
}


awpPoint TLFFaceImageDescriptor::GetPoint(int index)
{
    awpPoint p;
    p.X = 0;
    p.Y = 0;
    if (index >= 0 && index < 24)
    {
		p.X = (AWPSHORT)floor(m_points[index].X + 0.5);
		p.Y = (AWPSHORT)floor(m_points[index].Y);
    }
    return p;
}
void     TLFFaceImageDescriptor::SetPoint(int index, awpPoint p)
{
    if (index >= 0 && index < 24)
    {
         m_points[index].X = p.X;
         m_points[index].Y = p.Y;
    }
}

void     TLFFaceImageDescriptor::Reset()
{
    memset(m_points, 0, sizeof(m_points));
    memcpy(this->m_fmodel, FaceGraph1, sizeof(FaceGraph1));
    m_glasses = false;
    m_bread = false;
    m_Empty = true;
    m_turn = turnNone;
    awpPoint p;
    memset(&p,0,sizeof(p));
    for (int i = 0; i < 32; i++)
    {
        if (this->m_Triangles[i] != NULL)
        {
            delete this->m_Triangles[i];
            this->m_Triangles[i] = NULL;
        }
    }

    m_roi.SetRoi(p,p);
}

TLFRoi* TLFFaceImageDescriptor::GetRoi()
{
    return &this->m_roi;
}

void TLFFaceImageDescriptor::SetEyes(awpPoint le, awpPoint re)
{
    m_roi.SetRoi(le,re);
    m_Empty = false;
}

void     TLFFaceImageDescriptor::CreateModel()
{
    awp2DPoint m_le, m_re;
    m_le.X = m_roi.GetRoi().p.X;
    m_le.Y = m_roi.GetRoi().p.Y;

    m_re.X = m_roi.GetRoi().p1.X;
    m_re.Y = m_roi.GetRoi().p1.Y;

    if (m_re.X - m_le.X == 0)
        return;

    double alfa = atan((double)(m_re.Y - m_le.Y)/(double)(m_re.X - m_le.X));
    double L = sqrt((double)(m_re.X - m_le.X)*(m_re.X - m_le.X) + (m_re.Y-m_le.Y)*(m_re.Y - m_le.Y));

    awp2DPoint p2d[25];

    for (int i = 0; i < 24; i++)
    {
       p2d[i].X =  FaceGraph1[i].X*cos(alfa) - FaceGraph1[i].Y*sin(alfa);
       p2d[i].Y =  FaceGraph1[i].X*sin(alfa) + FaceGraph1[i].Y*cos(alfa);

       m_points[i].X =   (m_le.X +  p2d[i].X*L);
       m_points[i].Y   = (m_le.Y +  p2d[i].Y*L);
    }
    m_Empty = false;
}

void     TLFFaceImageDescriptor::NormModel(awp2DPoint ole, awp2DPoint ore)
{
   awp2DPoint le,re;
   le = ole;
   re = ore;
    double alfa = -atan((re.Y - le.Y)/(re.X - le.X));
    double L = sqrt((re.X - le.X)*(re.X - le.X) + (re.Y-le.Y)*(re.Y - le.Y));
    for (int i = 0; i < 24; i++)
    {
          m_fmodel[i] = m_points[i];
    }
}
void     TLFFaceImageDescriptor::UpdateModel(int sel, awp2DPoint le, awp2DPoint re, awp2DPoint ole, awp2DPoint ore)
{
    awp2DPoint m_re = re;
    awp2DPoint m_le = le;
    awp2DPoint m_oldRe = ore;
    awp2DPoint m_oldLe = ole;

    if (m_re.X - m_le.X == 0)
        return;

    awp2DPoint p2d[25];
    double alfa0 = atan((m_re.Y - m_le.Y)/(m_re.X - m_le.X));
    double alfa1 = atan((m_oldRe.Y - m_oldLe.Y)/(m_oldRe.X - m_oldLe.X));
    double alfa = alfa0 - alfa1;
    double L0 = sqrt((m_re.X - m_le.X)*(m_re.X - m_le.X) + (m_re.Y-m_le.Y)*(m_re.Y - m_le.Y));
    double L1 = sqrt((m_oldRe.X - m_oldLe.X)*(m_oldRe.X - m_oldLe.X) + (m_oldRe.Y-m_oldLe.Y)*(m_oldRe.Y - m_oldLe.Y));
    double L = L0/L1;
    awp2DPoint c;
    if (sel == 25)
        c = m_le;
    else if (sel == 26)
        c = m_re;

    for (int i = 0; i < 24; i++)
    {
       p2d[i].X =   (m_fmodel[i].X - c.X)*cos(alfa) - (m_fmodel[i].Y - c.Y)*sin(alfa);
       p2d[i].Y =   (m_fmodel[i].X - c.X)*sin(alfa) + (m_fmodel[i].Y - c.Y)*cos(alfa);

       m_points[i].X   =   (p2d[i].X*L);
       m_points[i].Y   =   (p2d[i].Y*L);
       m_points[i].X   +=   c.X;
       m_points[i].Y   +=  c.Y;

    }
    awpPoint p,p1;
	p.X = (AWPSHORT)floor(le.X);
	p.Y = (AWPSHORT)floor(le.Y);
	p1.X = (AWPSHORT)floor(re.X);
	p1.Y = (AWPSHORT)floor(re.Y);
    this->SetEyes(p,p1);
}


bool TLFFaceImageDescriptor::LoadFromFile(const char* lpFileName)
{
   Reset();

   FILE* f = fopen(lpFileName, "r+t");
   if (f == NULL)
        return false;
   char header[10];
   fscanf(f, "%s", header);
   if (strcmp(header, "face24") != 0)
   {
    fclose(f);
    return false;
   }
   awpPoint pp1;
   awpPoint pp2;
   memset(&pp1, 0, sizeof(awpPoint));
   memset(&pp2, 0, sizeof(awpPoint));
   for (int i = 0; i < 24; i++)
   {
    float a,b;
    fscanf(f, "%f\t%f", &a, &b);
    m_points[i].X = a;
    m_points[i].Y = b;
   }
   float a,b,c,d;
   fscanf(f, "%f%f\n%f%f", &a, &b, &c, &d);

   pp1.X = (AWPSHORT)floor(a + 0.5);
   pp1.Y = (AWPSHORT)floor(b + 0.5);
   pp2.X = (AWPSHORT)floor(c + 0.5);
   pp2.Y = (AWPSHORT)floor(d + 0.5);
   int aa,bb;
   fscanf(f, "%d\t%d", &aa, &bb);
    m_glasses = aa!=0?true:false;
    m_bread = bb!=0?true:false;
   fscanf(f, "%d\t", &aa);
   if (aa == 0)
    m_turn = turnNone;
   else if (aa == 1)
    m_turn = turnLeft;
   else
    m_turn = turnRight;

   m_roi.SetRoi(pp1,pp2);
   fclose(f);
   m_Empty = false;

   return true;
}
void TLFFaceImageDescriptor::SaveToFile(const char* lpFileName)
{

   FILE* f = fopen(lpFileName, "w+t");
   fprintf(f, "face24\n");
   for (int i = 0; i <24;i++)
    fprintf(f, "%f\t%f\n", m_points[i].X, m_points[i].Y);

   fprintf(f, "%d\t%d\n", m_roi.GetRoi().p.X, m_roi.GetRoi().p.Y);
   fprintf(f, "%d\t%d\n", m_roi.GetRoi().p1.X, m_roi.GetRoi().p1.Y);
   fprintf (f, "%d\t%d\n", this->m_glasses, this->m_bread);
   int turn = 0;
   switch (this->m_turn)
   {
      case turnNone:
        turn = 0;
      break;
      case turnLeft:
        turn = 1;
      break;
      case turnRight:
        turn = 2;
      break;
   }
   fprintf (f, "%d\n", turn);
   fclose(f);
}
const char* PointNames[9] =
{"left-eye-left","left-eye-right", "right-eye-left", "right-eye-right", "left-nose", "center-nose", "right-nose", "left-lips", "right-lips"};

TiXmlElement* TLFFaceImageDescriptor::SaveXML(const char* lpFileName,  awpRect* faceBox)
{
	  TiXmlElement* elem = new TiXmlElement("image");
	  string _name =lpFileName;
	  string _ansi = LFChangeFileExt(_name, ".jpg");
	  elem->SetAttribute("file", _ansi.c_str());
	  TiXmlElement* elem1 = new TiXmlElement("box");

      awpRect r = this->m_roi.GetRoi().r;
      if (faceBox != NULL)
        memcpy(&r, faceBox, sizeof(awpRect));
      elem1->SetAttribute("top", r.top);
      elem1->SetAttribute("left", r.left);
      elem1->SetAttribute("width", r.right - r.left);
      elem1->SetAttribute("height", r.bottom - r.top);
      TiXmlElement* elem2 = new TiXmlElement("part");
      elem2->SetAttribute("name", "le");
      elem2->SetAttribute("x", this->m_roi.GetRoi().p.X);
      elem2->SetAttribute("y", this->m_roi.GetRoi().p.Y);
      TiXmlElement* elem3 = new TiXmlElement("part");
      elem3->SetAttribute("name", "re");
      elem3->SetAttribute("x", this->m_roi.GetRoi().p1.X);
      elem3->SetAttribute("y", this->m_roi.GetRoi().p1.Y);
      elem1->LinkEndChild(elem2);
      elem1->LinkEndChild(elem3);
      for (int i = 0; i < 24; i++)
      {
        TiXmlElement* e = new TiXmlElement("part");
        if (i  < 9 )
            e->SetAttribute("name", /*IntToStr(i).c_str()*/PointNames[i]);
        else
            e->SetAttribute("name", LFIntToStr(i).c_str());
        e->SetAttribute("x", (int)m_points[i].X);
        e->SetAttribute("y", (int)m_points[i].Y);
        elem1->LinkEndChild(e);
      }
      elem->LinkEndChild(elem1);
      return elem;
}

bool TLFFaceImageDescriptor::IsGlasses()
{
    return m_glasses;
}
bool TLFFaceImageDescriptor::IsBread()
{
    return m_bread;
}
bool TLFFaceImageDescriptor::IsTurnLeft()
{
    return m_turn == turnLeft;
}
bool TLFFaceImageDescriptor::IsTurnRight()
{
    return m_turn == turnRight;
}
bool TLFFaceImageDescriptor::IsEmpty()
{
    return m_Empty;
}
void TLFFaceImageDescriptor::SetTurnLeft(bool value)
{
    if (value)
        m_turn = turnLeft;
    else
        m_turn = turnNone;

}
void TLFFaceImageDescriptor::SetTurnRight(bool value)
{
    if (value)
        m_turn = turnRight;
    else
        m_turn = turnNone;
}
void TLFFaceImageDescriptor::SetGlasses(bool value)
{
    this->m_glasses = value;
}
void TLFFaceImageDescriptor::SetBread(bool value)
{
    this->m_bread = value;
}
// indexes
const SLFTriangle gtri[22] =
{
{0,	0,	18,	19},
{1,	0,	19,	1},
{2,	1,	19,	20},
{3,	2,	21,	22},
{4,	2,	22,	3 },
{5,	3,	22,	23},
{7,	20,	4 ,	5 },
{8,	20,	5 ,	21},
{9,	21,	5 ,	6 },
{11,	7,	4,	5 },
{12,	7,	5,	8 },
{13,	8,	5,	6 },
{14,	7,	13,	8 },
{15,	1,	20,	4 },
{16,	2,	21,	6 },
{17,	0,	1,	4 },
{18,	2,	3,	6 },
{23,	15,	8,	14},
{24,	13,	8,	14},
{25,	7,	12,	13},
{26,	7,	12,	13},
{27,	7,	11,	12} };

//
bool TLFFaceImageDescriptor::CreateTriangles(int depth)
{
    if (this->IsEmpty())
        return false;

    for (int i = 0; i < 32; i++)
    {
        if (this->m_Triangles[i] != NULL)
        {
            delete this->m_Triangles[i];
            this->m_Triangles[i] = NULL;
        }
    }
    this->m_List.Clear();
  //  FILE* f = fopen("index.txt", "r+t");
    int d = depth;
//    int k,i2,i3;

    for (int i = 0; i < 22;i++)
    {
    //    fscanf(f,"%d\t%d\t%d\t%d\n", &k,&i1,&i2,&i3);
        m_tindex[i] = gtri[i];
        m_Triangles[i] = new TLFTriangle(m_points[gtri[i].v1], m_points[gtri[i].v2], m_points[gtri[i].v3], NULL, d, i);
        AddTriangle(m_Triangles[i]);
    }
/*
    m_Triangles[0] = new TLFTriangle(m_points[0], m_points[18], m_points[19], NULL, d, 0);
    fprintf(f,"%d\t%d\t%d\t%d\n", 0,0,18,19);
    m_Triangles[1] = new TLFTriangle(m_points[0], m_points[19], m_points[1], NULL, d, 1);
    fprintf(f,"%d\t%d\t%d\t%d\n", 1,0,19,1);
    m_Triangles[2] = new TLFTriangle(m_points[1], m_points[19], m_points[20], NULL, d, 2);
    fprintf(f,"%d\t%d\t%d\t%d\n", 2,1,19,20);
    m_Triangles[3] = new TLFTriangle(m_points[2], m_points[21], m_points[22], NULL, d, 3);
    fprintf(f,"%d\t%d\t%d\t%d\n", 3,2,21,22);
    m_Triangles[4] = new TLFTriangle(m_points[2], m_points[22], m_points[3], NULL, d, 4);
    fprintf(f,"%d\t%d\t%d\t%d\n", 4,2,22,3);
    m_Triangles[5] = new TLFTriangle(m_points[3], m_points[22], m_points[23], NULL, d, 5);
    fprintf(f,"%d\t%d\t%d\t%d\n", 5,3,22,23);
    m_Triangles[6] = new TLFTriangle(m_points[16], m_points[6], m_points[8], NULL, d, 6);
    fprintf(f,"%d\t%d\t%d\t%d\n", 6,16,6,8);
    m_Triangles[7] = new TLFTriangle(m_points[20], m_points[4], m_points[5], NULL, d, 7);
    fprintf(f,"%d\t%d\t%d\t%d\n", 7,20,4,5);
    m_Triangles[8] = new TLFTriangle(m_points[20], m_points[5], m_points[21], NULL, d, 8);
    fprintf(f,"%d\t%d\t%d\t%d\n", 8,20,5,21);
    m_Triangles[9] = new TLFTriangle(m_points[21], m_points[5], m_points[6], NULL, d, 9);
    fprintf(f,"%d\t%d\t%d\t%d\n", 9,21,5,6);
    m_Triangles[10] = new TLFTriangle(m_points[10], m_points[7], m_points[4], NULL, d, 10);
    fprintf(f,"%d\t%d\t%d\t%d\n", 10,10,7,4);
    m_Triangles[11] = new TLFTriangle(m_points[7], m_points[4], m_points[5], NULL, d, 11);
    fprintf(f,"%d\t%d\t%d\t%d\n", 11,7,4,5);
    m_Triangles[12] = new TLFTriangle(m_points[7], m_points[5], m_points[8], NULL, d, 12);
    fprintf(f,"%d\t%d\t%d\t%d\n", 12,7,5,8);
    m_Triangles[13] = new TLFTriangle(m_points[8], m_points[5], m_points[6], NULL, d, 13);
    fprintf(f,"%d\t%d\t%d\t%d\n", 13,8,5,6);
    m_Triangles[14] = new TLFTriangle(m_points[7], m_points[13], m_points[8], NULL, d, 14);
    fprintf(f,"%d\t%d\t%d\t%d\n", 14,7,13,8);
    m_Triangles[15] = new TLFTriangle(m_points[1], m_points[20], m_points[4], NULL, d, 15);
    fprintf(f,"%d\t%d\t%d\t%d\n", 15,1,20,4);
    m_Triangles[16] = new TLFTriangle(m_points[2], m_points[21], m_points[6], NULL, d, 16);
    fprintf(f,"%d\t%d\t%d\t%d\n", 16,2,21,6);
    m_Triangles[17] = new TLFTriangle(m_points[0], m_points[1], m_points[4], NULL, d, 17);
    fprintf(f,"%d\t%d\t%d\t%d\n", 17,0,1,4);
    m_Triangles[18] = new TLFTriangle(m_points[2], m_points[3], m_points[6], NULL, d, 18);
    fprintf(f,"%d\t%d\t%d\t%d\n", 18,2,3,6);
    m_Triangles[19] = new TLFTriangle(m_points[23], m_points[3], m_points[17], NULL, d, 19);
    fprintf(f,"%d\t%d\t%d\t%d\n", 19,23,3,17);
    m_Triangles[20] = new TLFTriangle(m_points[3], m_points[6], m_points[17], NULL, d, 20);
    fprintf(f,"%d\t%d\t%d\t%d\n", 20,3,6,17);
    m_Triangles[21] = new TLFTriangle(m_points[17], m_points[6], m_points[16], NULL, d, 21);
    fprintf(f,"%d\t%d\t%d\t%d\n", 21,17,6,16);
    m_Triangles[22] = new TLFTriangle(m_points[16], m_points[8], m_points[15], NULL, d, 22);
    fprintf(f,"%d\t%d\t%d\t%d\n", 2,16,8,15);
    m_Triangles[23] = new TLFTriangle(m_points[15], m_points[8], m_points[14], NULL, d, 23);
    fprintf(f,"%d\t%d\t%d\t%d\n", 23,15,8,14);
    m_Triangles[24] = new TLFTriangle(m_points[13], m_points[8], m_points[14], NULL, d, 24);
    fprintf(f,"%d\t%d\t%d\t%d\n", 24,13,8,14);
    m_Triangles[25] = new TLFTriangle(m_points[7], m_points[12], m_points[13], NULL, d, 25);
    fprintf(f,"%d\t%d\t%d\t%d\n", 25,7,12,13);
    m_Triangles[26] = new TLFTriangle(m_points[7], m_points[12], m_points[13], NULL, d, 26);
    fprintf(f,"%d\t%d\t%d\t%d\n", 26,7,12,13);
    m_Triangles[27] = new TLFTriangle(m_points[7], m_points[11], m_points[12], NULL, d, 27);
    fprintf(f,"%d\t%d\t%d\t%d\n", 27,7,11,12);
    m_Triangles[28] = new TLFTriangle(m_points[7], m_points[10], m_points[11], NULL, d, 28);
    fprintf(f,"%d\t%d\t%d\t%d\n", 28,7,10,11);
    m_Triangles[29] = new TLFTriangle(m_points[4], m_points[10], m_points[9], NULL, d, 29);
    fprintf(f,"%d\t%d\t%d\t%d\n", 29,4,10,9);
    m_Triangles[30] = new TLFTriangle(m_points[4], m_points[9], m_points[0], NULL, d, 30);
    fprintf(f,"%d\t%d\t%d\t%d\n", 30,4,9,0);
    m_Triangles[31] = new TLFTriangle(m_points[18], m_points[0], m_points[9], NULL, d, 31);
    fprintf(f,"%d\t%d\t%d\t%d\n", 31,18,0,9);
    fclose(f);

    f = fopen("index22.txt", "r+t");
    for (int i = 0; i < 32; i++)
    {
        fscanf(f, "%d\t%d\t%d\t%d\n", &m_tindex[i].idx, &m_tindex[i].v1, &m_tindex[i].v2, &m_tindex[i].v3);
    }
*/
 //   fclose(f);
    return true;
}
void  TLFFaceImageDescriptor::AddTriangle(TLFTriangle*  t)
{
   if (t == NULL)
    return;
   if (t->GetChild(0) == 0 && t->GetParent() != NULL)
   {
       this->m_List.AddTriangle(t);
   }
   else
   {
        for (int i = 0; i < 4; i++)
        {
            AddTriangle(t->GetChild(i));
        }
   }
}
/////
TLFTriangle* TLFFaceImageDescriptor::GetTriangle(int index)
{
    if (index >= 0 && index < 32)
        return m_Triangles[index];
    return NULL;
}

TLFTriangleList* TLFFaceImageDescriptor::GetList(int index)
{
    return &this->m_List;
}

SLFTriangle* TLFFaceImageDescriptor::GetTIndex()
{
    return &this->m_tindex[0];
}

void TLFFaceImageDescriptor::GetShape(awp2DPoint* shape)
{
    if (this->IsEmpty())
        return;
    for (int i = 2; i < NUM_POINTS; i++)
    {
       shape[i] = this->m_points[i-2];
    }
    shape[0].X = m_roi.GetRoi().p.X;
    shape[0].Y = m_roi.GetRoi().p.Y;
    shape[1].X = m_roi.GetRoi().p1.X;
    shape[1].Y = m_roi.GetRoi().p1.Y;
}

awpRect  TLFFaceImageDescriptor::GetFaceBoxMinMax()
{
      awpRect box;
      int minx, maxx, miny, maxy;
      awpPoint p = GetPoint(0);
      minx = p.X;
      maxx = p.X;
      miny = p.Y;
      maxy = p.Y;
      for (int i = 1; i < 24; i++)
      {
        awpPoint p;
        p = GetPoint(i);
        if (minx > p.X)
            minx = p.X;
        if (maxx < p.X)
            maxx = p.X;
        if (miny > p.Y)
            miny = p.Y;
        if (maxy < p.Y)
            maxy = p.Y;
      }
      box.left = minx;
      box.right = maxx;
      box.top = miny;
      box.bottom = maxy;
      return box;

}
awpRect  TLFFaceImageDescriptor::GetFaceBoxForPredictor(awpRect* faceRect)
{
      awpRect box;
      if (faceRect == NULL)
        box = GetFaceBoxMinMax();
      else
        memcpy(&box, faceRect, sizeof(awpRect));
      return box;
}

awpImage* TLFFaceImageDescriptor::GetFaceImageForPredictor(awpImage* img, awpRect* faceRect)
{

    awpRect box  = GetFaceBoxForPredictor(faceRect);
    awpRect box1 = GetFaceBoxForPredictor(NULL);

    if (faceRect == NULL)
        box = box1;
    if (faceRect != NULL)
    {
    TLFRect _box(box);
    if (_box.RectOverlap(box1) < 0.3)
        return NULL;
        }


    int w = awpRectWidth(box);
    int h = awpRectHeight(box);

      if (w < h)
      {
         w = h;
      }
      else
      {
        h = w;
      }

      awpPoint cent;
      cent.X = (box.left + box.right) / 2;
      cent.Y = (box.top + box.bottom) / 2;
      box.left = cent.X - w/ 2;
      box.right = cent.X + w / 2;
      box.top  = cent.Y - h / 2;
      box.bottom = cent.Y + h / 2;

      awpRect r;
      r.left = box.left < 0 ? 0:box.left;
      r.right = box.right > img->sSizeX?img->sSizeX: box.right;
      r.top  = box.top < 0 ? 0: box.top;
      r.bottom = box.bottom > img->sSizeY ? img->sSizeY : box.bottom;
      if (r.right - r.left == 0)
        return NULL;

      awpImage* img1 = NULL;
      awpCopyRect(img, &img1, &r);

      w = r.right - r.left;
      h = r.bottom - r.top;
	  double scale = (double)w / (double)h;
      if (w > h)
      {
        w = 256;
        h = (int)floor(w / scale + 0.5);
      }
      else
      {
        h = 256;
        w = (int)floor(h*scale);
      }

      awpResizeBilinear(img1, w, h);
      //box = r;
      //scale descriptor
      scale = (double)256 / (r.right - r.left);
      for (int i = 0; i < 24; i++)
      {
         awpPoint p = GetPoint(i);
         p.X = (AWPSHORT)floor((p.X - r.left)*scale + 0.5);
		 p.Y = (AWPSHORT)floor((p.Y - r.top)*scale + 0.5);
         SetPoint(i, p);
      }
      awpPoint p1;
      awpPoint p = m_roi.GetRoi().p;
      p1 = m_roi.GetRoi().p1;
	  p.X = (AWPSHORT)floor((p.X - r.left)*scale + 0.5);
	  p.Y = (AWPSHORT)floor((p.Y - r.top)*scale + 0.5);
	  p1.X = (AWPSHORT)floor((p1.X - r.left)*scale + 0.5);
	  p1.Y = (AWPSHORT)floor((p1.Y - r.top)*scale + 0.5);
      SetEyes(p,p1);

      return img1;
}

void TLFFaceImageDescriptor::Scale(double factor)
{
    double scale = factor;
    awpPoint pp;
    pp.X = 0;
    pp.Y = 0;
    for (int i = 0; i < 24; i++)
    {
       awpPoint p = GetPoint(i);
	   p.X = (AWPSHORT)floor((p.X - pp.X)*scale + 0.5);
	   p.Y = (AWPSHORT)floor((p.Y - pp.Y)*scale + 0.5);
       SetPoint(i, p);
    }
    awpPoint p1;
    awpPoint p = m_roi.GetRoi().p;
    p1 = m_roi.GetRoi().p1;
	p.X = (AWPSHORT)floor((p.X - pp.X)*scale + 0.5);
	p.Y = (AWPSHORT)floor((p.Y - pp.Y)*scale + 0.5);
	p1.X = (AWPSHORT)floor((p1.X - pp.X)*scale + 0.5);
	p1.Y = (AWPSHORT)floor((p1.Y - pp.Y)*scale+ 0.5);
    SetEyes(p,p1);
}
void TLFFaceImageDescriptor::Shift(awpPoint pp)
{
    double scale = 1;
    for (int i = 0; i < 24; i++)
    {
       awpPoint p = GetPoint(i);
	   p.X = (AWPSHORT)floor((p.X - pp.X)*scale + 0.5);
	   p.Y = (AWPSHORT)floor((p.Y - pp.Y)*scale + 0.5);
       SetPoint(i, p);
    }
    awpPoint p1;
    awpPoint p0 = m_roi.GetRoi().p;
    p1 = m_roi.GetRoi().p1;
	p0.X = (AWPSHORT)floor((p0.X - pp.X)*scale + 0.5);
	p0.Y = (AWPSHORT)floor((p0.Y - pp.Y)*scale + 0.5);
	p1.X = (AWPSHORT)floor((p1.X - pp.X)*scale + 0.5);
	p1.Y = (AWPSHORT)floor((p1.Y - pp.Y)*scale + 0.5);
    SetEyes(p0,p1);
}




