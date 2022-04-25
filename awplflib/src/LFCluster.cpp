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
//		File: LFCluster.cpp
//		Purpose: implementation TLFClusterItem, TLFClusterDescr, TLFClusterMaker, 
//								TLFClusterRecursive, TLFClusterTrack
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#include "_LF.h"
#include "float.h"
TLFClusterItem::TLFClusterItem(TLFDetectedItem* item)
{
	this->m_di = item;
	this->m_clusterID = -1;
}

int TLFClusterItem::GetClusterID()
{
	return this->m_clusterID;
}
void TLFClusterItem::SetClusterID(int id)
{
	this->m_clusterID = id;
}

TLFDetectedItem* TLFClusterItem::GetDi()
{
	return this->m_di;
}

TLFClusterDescr::TLFClusterDescr(int id, int blob_id)
{
	m_power = 0;
	m_cluster_id = id;
	m_blob_idx = blob_id;
}
int TLFClusterDescr::GetId()
{
	return this->m_cluster_id;
}
int TLFClusterDescr::GetPower()
{
	return m_power;
}
void TLFClusterDescr::IncPower()
{
	m_power++;
}
void TLFClusterDescr::Clear()
{
	m_power = 0;
}

int  TLFClusterDescr::GetBlobId()
{
	return m_blob_idx;
}

TLFClusterMaker::TLFClusterMaker(ILFObjectDetector* detector)
{
	memset(this->m_blobs, 0, sizeof(this->m_blobs));
	this->m_detector = detector;
	m_cluster_count = 0;
}

TLFClusterMaker::~TLFClusterMaker()
{
}

void TLFClusterMaker::MakeClusters()
{
	if (m_detector == NULL)
		return;
	this->Clustering();
	this->Painting();
	this->UpdateBlobs();
}

void TLFClusterMaker::MakeClusterList()
{
	if (m_detector == NULL)
		return;
	m_clusterItems.Clear();
	for (int i = 0; i < m_detector->GetNumItems(); i++)
	{
		if (m_detector->GetItem(i)->HasObject())
		{
			TLFClusterItem* c = new TLFClusterItem(m_detector->GetItem(i));
			m_clusterItems.Add(c);
		}
	}
}

double TLFClusterMaker::Distance(TLFClusterItem* di1, TLFClusterItem* di2)
{
	if (di1 == NULL || di2 == NULL || di1 == di2)
		return FLT_MAX;
	awpPoint p1 = di1->GetDi()->GetBounds()->Center();
	awpPoint p2 = di2->GetDi()->GetBounds()->Center();

	return sqrt((double)(p1.X - p2.X)*(p1.X-p2.X) + (double)(p1.Y - p2.Y)*(p1.Y - p2.Y));
}

int TLFClusterMaker::GetUnclustered()
{
	for (int i = 0; i < m_clusterItems.GetCount(); i++)
	{
		TLFClusterItem* ci = (TLFClusterItem*)m_clusterItems.Get(i);
		if (ci != NULL && ci->GetDi() != NULL)
		{
			if (ci->GetClusterID() == -1)
				return i;
		}
	}
	return -1;
}

int TLFClusterMaker::GetNearlestItem(awpImage* img, int idx, double& dist)
{
	if (img == NULL || img->sSizeX != img->sSizeY || img->dwType != AWP_DOUBLE || idx < 0 || idx > img->sSizeX)
		return -1;
	AWPDOUBLE* d = (AWPDOUBLE*)img->pPixels;
	d += (idx*img->sSizeX);
	int result = -1;
	for (int i = 0; i < img->sSizeX; i++)
	{
		TLFClusterItem* d1 = (TLFClusterItem*)m_clusterItems.Get(i);
		if (d1->GetClusterID() < 0 && d[i] < dist)
		{
			dist = d[i];
			result = i;
		}
	}
	return result;
}

int TLFClusterMaker::GetnearlestItemWithClusterIndex(awpImage* img, int idx, double& dist)
{
	if (img == NULL || img->sSizeX != img->sSizeY || img->dwType != AWP_DOUBLE || idx < 0 || idx > img->sSizeX)
		return -1;
	AWPDOUBLE* d = (AWPDOUBLE*)img->pPixels;
	d += (idx*img->sSizeX);
	int result = -1;
	for (int i = 0; i < img->sSizeX; i++)
	{
		TLFClusterItem* d1 = (TLFClusterItem*)m_clusterItems.Get(i);
		if (d1->GetDi()->GetClusterIdx() >= 0 && d[i] < dist)
		{
			dist = d[i];
			result = i;
		}
	}
	return result;
}

double TLFClusterMaker::GetNearleastItemToCluster(int clusterID, int& idx)
{
	double result = FLT_MAX;
	awpImage* img = m_matrix.GetImage();
	AWPDOUBLE* data = (AWPDOUBLE*)img->pPixels;
	for (int i = 0; i < m_clusterItems.GetCount(); i++)
	{
		TLFClusterItem* d1 = (TLFClusterItem*)m_clusterItems.Get(i);
		if (d1->GetClusterID() == clusterID)
		{
			for (int j = 0; j < m_clusterItems.GetCount(); j++)
			{
				TLFClusterItem* d2 = (TLFClusterItem*)m_clusterItems.Get(j);
				if (d2->GetClusterID() < 0)
				{
					double d = data[i*img->sSizeX + j];
					if (d < result)
					{
						result = d;
						idx = j;
						if (result < 12)
						{
							d2->SetClusterID(clusterID);
						}
					}
				}

			}
		}

	}
	return result;
}


void TLFClusterMaker::Clustering()
{
	this->MakeClusterList();
	if (this->m_clusterItems.GetCount() == 0)
		return;
	awpImage* img = NULL;
	awpCreateImage(&img, m_clusterItems.GetCount(), m_clusterItems.GetCount(), 1, AWP_DOUBLE);
	if (img != NULL)
	{
		awpFill(img, FLT_MAX);
		AWPDOUBLE* data = (AWPDOUBLE*)img->pPixels;
		for (int i = 0; i < img->sSizeY; i++)
		{
			TLFClusterItem* d1 = (TLFClusterItem*)m_clusterItems.Get(i);
			for (int j = 0; j < img->sSizeX; j++)
			{
				TLFClusterItem* d2 = (TLFClusterItem*)m_clusterItems.Get(j);
				data[i*img->sSizeX + j] = Distance(d1,d2);
			}
		}
		m_matrix.SetImage(img);
		awpReleaseImage(&img);

		int item_idx = GetUnclustered();
		m_cluster_count = 0;

		while (item_idx >= 0)
		{
			TLFClusterItem* item = (TLFClusterItem*)m_clusterItems.Get(item_idx);
			item->SetClusterID(m_cluster_count);
			while (GetNearleastItemToCluster(m_cluster_count, item_idx) < 12)
			{
				item = (TLFClusterItem*)m_clusterItems.Get(item_idx);
				item->SetClusterID(m_cluster_count);
			}
			m_cluster_count++;
			item_idx = GetUnclustered();
		}
	}
}

int  TLFClusterMaker::GetNewClusterIndex()
{
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
		if (m_blobs[i].state == 0)
			return i;
	return -1;
}

void TLFClusterMaker::Painting()
{
	if (m_cluster_count == 0)
		return;
	//////
	awpImage* _2d_histogramm = NULL;
	awpCreateImage(&_2d_histogramm, LF_NUM_CLUSTERS, m_cluster_count, 1, AWP_DOUBLE);
	AWPDOUBLE* data = (AWPDOUBLE*)_2d_histogramm->pPixels;
	if (_2d_histogramm != NULL)
	{
		for (int i = 0; i < m_clusterItems.GetCount(); i++)
		{
			TLFClusterItem* ci = (TLFClusterItem*)m_clusterItems.Get(i);
			int x, y;
			x = ci->GetDi()->GetClusterIdx();
			if (x > 0)
			{
				y = ci->GetClusterID();
				data[y*LF_NUM_CLUSTERS + x]++;
			}
		}
		//////
		for (int i = 0; i < m_clusterItems.GetCount(); i++)
		{
			TLFClusterItem* ci = (TLFClusterItem*)m_clusterItems.Get(i);
			if (ci->GetDi() != NULL && ci->GetDi()->GetClusterIdx() == -1)
			{
				AWPDOUBLE* data = (AWPDOUBLE*)_2d_histogramm->pPixels;
				data += (LF_NUM_CLUSTERS*ci->GetClusterID());
				int n = 0;
				for (int k = 0; k < LF_NUM_CLUSTERS; k++)
				{
					if (data[k] > 0)
						n++;
				}
				if (n == 0)
				{
					//assign new cluster_index 
					ci->GetDi()->SetClusterIdx(GetNewClusterIndex());
				}
				else
				{
					// find nearlest with cluster_index  
					double dist = FLT_MAX;
					ci->GetDi()->SetClusterIdx(GetnearlestItemWithClusterIndex(m_matrix.GetImage(), ci->GetClusterID(), dist));
				}
			}
		}
		awpReleaseImage(&_2d_histogramm);
	}
}

void TLFClusterMaker::UpdateBlobs()
{
	// очистка массива наблюдаемых объектов. 
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		m_blobs[i].angle = 0;
		m_blobs[i].cx = 0;
		m_blobs[i].cy = 0;
		m_blobs[i].major = 0;
		m_blobs[i].minor = 0;
		m_blobs[i].mxx = 0;
		m_blobs[i].mxy = 0;
		m_blobs[i].myy = 0;
		m_blobs[i].area = 0;
	}
	//
	for (int i = 0; i < m_clusterItems.GetCount(); i++)
	{
		TLFClusterItem* ci = (TLFClusterItem*)m_clusterItems.Get(i);
		TLFDetectedItem* di = ci->GetDi();
		if (di != NULL)
		{
			int idx = di->GetClusterIdx();
			awpPoint p = di->GetBounds()->Center();
			m_blobs[idx].area += 1;
			m_blobs[idx].cx += p.X;
			m_blobs[idx].cy += p.Y;
		}
	}

	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs[i].area > 0)
		{
			m_blobs[i].cx /= m_blobs[i].area;
			m_blobs[i].cy /= m_blobs[i].area;
		}
	}

	for (int i = 0; i < m_clusterItems.GetCount(); i++)
	{
		TLFClusterItem* ci = (TLFClusterItem*)m_clusterItems.Get(i);
		TLFDetectedItem* di = ci->GetDi();
		if (di != NULL)
		{
			int idx = di->GetClusterIdx();
			awpPoint p = di->GetBounds()->Center();
			if (m_blobs[idx].area > 0)
			{
				m_blobs[idx].mxx += (p.X - m_blobs[idx].cx)*(p.X - m_blobs[idx].cx);
				m_blobs[idx].mxy += (p.Y - m_blobs[idx].cy)*(p.X - m_blobs[idx].cx);
				m_blobs[idx].myy += (p.Y - m_blobs[idx].cy)*(p.Y- m_blobs[idx].cy);
			}
		}
	}

	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs[i].area > 0)
		{
			m_blobs[i].mxx /= m_blobs[i].area;
			m_blobs[i].myy /= m_blobs[i].area;
			//	moments.mxy  /= moments.area;
			m_blobs[i].mxy = 0;
			if (m_blobs[i].mxy == 0 && m_blobs[i].myy <= m_blobs[i].mxx)
			{
				m_blobs[i].angle = -AWP_PI / 2;
				m_blobs[i].major = 4 * sqrt(m_blobs[i].mxx);
				m_blobs[i].minor = 4 * sqrt(m_blobs[i].myy);
			}
			else if (m_blobs[i].mxy == 0 && m_blobs[i].myy > m_blobs[i].mxx)
			{
				m_blobs[i].angle = 0;
				m_blobs[i].major = 4 * sqrt(m_blobs[i].myy);
				m_blobs[i].minor = 4 * sqrt(m_blobs[i].mxx);
			}
			else if (m_blobs[i].myy <= m_blobs[i].mxx)
			{
				m_blobs[i].angle = atan(-2 * m_blobs[i].mxy*m_blobs[i].myy - m_blobs[i].mxx + sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy));
				m_blobs[i].major = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx + sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));
				m_blobs[i].minor = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx - sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));

			}
			else if (m_blobs[i].myy > m_blobs[i].mxx)
			{
				m_blobs[i].angle = atan(sqrt(m_blobs[i].mxx + m_blobs[i].myy + sqrt((m_blobs[i].mxx - m_blobs[i].myy)*(m_blobs[i].mxx - m_blobs[i].myy) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)) / (-2 * m_blobs[i].mxy + 0.000001));
				m_blobs[i].major = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx + sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));
				m_blobs[i].minor = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx - sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));
			}

		}
	}

	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs[i].area > 0 && m_blobs[i].state == 0)
		{
			m_blobs[i].state = 1;

			// todo: добавить GUID
			LF_UUID_CREATE(m_blobs[i].id)
		}
		else if (m_blobs[i].area > 0 && m_blobs[i].state == 1)
		{
			m_blobs[i].state = 2;
		}
		else if (m_blobs[i].area == 0 && (m_blobs[i].state == 2 || m_blobs[i].state == 1))
		{
			m_blobs[i].state = 3;
		}
		else if (m_blobs[i].area == 0 && m_blobs[i].state == 3)
		{
			m_blobs[i].state = 0;
			LF_NULL_UUID_CREATE(m_blobs[i].id)
		}
	}
} 

int TLFClusterMaker::NumClusters()
{
	return m_cluster_count;
}


TLFClusterRecursive::TLFClusterRecursive(ILFObjectDetector* detector,  double minw, double minh, double maxw, double maxh)
{
	memset(this->m_blobs, 0, sizeof(this->m_blobs));
	this->m_detector = detector;
	this->m_cluster_count = 0;
    this->m_cluster_idx = 0;
	m_descr = NULL;
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		m_blobs[i].color = -1;
	}
	this->m_minw = minw;
	this->m_minh = minh;
	this->m_maxw = maxw;
	this->m_maxh = maxh;
}

TLFClusterRecursive::~TLFClusterRecursive()
{
	if (m_descr != NULL)
		delete m_descr;
}

void TLFClusterRecursive::MakeClusters()
{
	if (this->m_detector == NULL)
		return;
	TLFTileScanner* scanner = dynamic_cast<TLFTileScanner*>(m_detector->GetScanner());
	if (scanner == NULL)
		return;
	int w = scanner->GetNumX() -2;
	int h = scanner->GetNumY() -2;

	for (int i = 0; i < this->m_clusters.GetCount(); i++)
	{
		TLFClusterDescr* cd = (TLFClusterDescr*)m_clusters.Get(i);
		cd->Clear();
	}

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			TLFDetectedItem* di = m_detector->GetItem(i*w + j);
			if (di != NULL && di->HasObject() && di->GetClusterIdx() > 0)
			{
				int idx  = di->GetClusterIdx();
				GetDescr(idx);
				if (m_descr != NULL)
					m_descr->IncPower();
				Search(j, i, idx);
			}
		}

	}
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j  < w; j++)
		{
			TLFDetectedItem* di = m_detector->GetItem(i*w + j);
			if (di != NULL && di->HasObject() && di->GetClusterIdx() == -1)
			{
                m_cluster_idx++;
				int blob_idx = this->GetBlobIndex();
				//printf("cluster id = %d \t color id = %d\n", m_cluster_idx, blob_idx);
				m_descr = new TLFClusterDescr(m_cluster_idx, blob_idx);
				Search(j, i, m_cluster_idx);
				m_clusters.Add(m_descr);
			}
		}
	}

	for (int i = 0; i < m_detector->GetNumItems(); i++)
	{
		TLFDetectedItem* di = m_detector->GetItem(i);
		if (di->HasObject() && di->GetClusterIdx() > 0)
		{
			int idx = di->GetClusterIdx();
			GetDescr(idx);
			if (m_descr && m_descr->GetPower() < 20)
				di->SetHasObject(false);
		}
	}

	for (int i = m_clusters.GetCount() - 1; i >= 0; i--)
	{
		TLFClusterDescr* cd = (TLFClusterDescr*)m_clusters.Get(i);
		//printf("id = %d \t power = %d\n", cd->GetId(), cd->GetPower());
		if (cd->GetPower() < 20)
		{

			if (cd->GetBlobId() >= 0)
				m_blobs[cd->GetBlobId()].color = -1;
			//printf("delete cluster id = %d \t power = %d\n", cd->GetId(), cd->GetPower());
			m_clusters.Delete(i);
		}
	}
	m_cluster_count = m_clusters.GetCount();
 	MakeClusterList();
 	UpdateBlobs();

}

bool TLFClusterRecursive::Search(int& x, int& y, int id)
{
	TLFTileScanner* scanner = dynamic_cast<TLFTileScanner*>(m_detector->GetScanner());
	if (scanner == NULL)
		return false;
	int w = scanner->GetNumX() -2;
	int h = scanner->GetNumY() -2;

	TLFDetectedItem* di = m_detector->GetItem(y*w + x);
	di->SetClusterIdx(id);
	if (m_descr != NULL)
		di->SetColor(m_descr->GetBlobId());
	for (int i = y - 4; i <= y + 4; i++)
	{
		if (i < 0 || i >= h)
			continue;
		for (int j = x -4; j <= x + 4; j++)
		{
			if (j < 0 || j >= w)
				continue;
			di = m_detector->GetItem(i*w + j);
			if (di != NULL && di->HasObject() && di->GetClusterIdx() == -1)
			{
				if (this->m_descr != NULL)
					m_descr->IncPower();
				Search(j, i, id);
			}
		}
	}
	return true;
}


int TLFClusterRecursive::NumClusters()
{
	return this->m_cluster_count;
}

static double GetMinMomnetsX(SLFBinaryBlob& m)
{
	double a = AWP_MIN(m.minor_p1.X, m.major_p1.X);
	double b = AWP_MIN(m.minor_p2.X, m.major_p2.X);
	return AWP_MIN(a, b);
}
static double GetMaxMomnetsX(SLFBinaryBlob& m)
{
	double a = AWP_MAX(m.minor_p1.X, m.major_p1.X);
	double b = AWP_MAX(m.minor_p2.X, m.major_p2.X);
	return AWP_MAX(a, b);
}
static double GetMinMomnetsY(SLFBinaryBlob& m)
{
	double a = AWP_MIN(m.minor_p1.Y, m.major_p1.Y);
	double b = AWP_MIN(m.minor_p2.Y, m.major_p2.Y);
	return AWP_MIN(a, b);
}
static double GetMaxMomnetsY(SLFBinaryBlob& m)
{
	double a = AWP_MAX(m.minor_p1.Y, m.major_p1.Y);
	double b = AWP_MAX(m.minor_p2.Y, m.major_p2.Y);
	return AWP_MAX(a, b);
}

static void DoUpdateBlobs(SLFBinaryBlob* blobs, TLFObjectList* list)
{
	TLFObjectList* m_clusterItems = list;
	SLFBinaryBlob* m_blobs = blobs;
	// очистка массива наблюдаемых объектов. 
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		blobs[i].angle = 0;
		blobs[i].cx = 0;
		blobs[i].cy = 0;
		blobs[i].sx = 0;
		blobs[i].sy = 0;
		blobs[i].w = 0;
		blobs[i].h = 0;
		blobs[i].major = 0;
		blobs[i].minor = 0;
		blobs[i].mxx = 0;
		blobs[i].mxy = 0;
		blobs[i].myy = 0;
		blobs[i].area = 0;
		if (blobs[i].state == 3)
		{
			blobs[i].color = -1;
			blobs[i].state = 0;
		}
	}
	//
	for (int i = 0; i < m_clusterItems->GetCount(); i++)
	{
		TLFClusterItem* ci = (TLFClusterItem*)m_clusterItems->Get(i);
		TLFDetectedItem* di = ci->GetDi();
		if (di != NULL)
		{
			int idx = di->GetColor();
			if (idx >= 0 && idx < LF_NUM_CLUSTERS)
			{
				awpPoint p = di->GetBounds()->Center();
				blobs[idx].area += 1;
				blobs[idx].cx += p.X;
				blobs[idx].cy += p.Y;
			}
		}
	}

	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (blobs[i].area > 0)
		{
			blobs[i].cx /= blobs[i].area;
			blobs[i].cy /= blobs[i].area;
		}
	}

	for (int i = 0; i < m_clusterItems->GetCount(); i++)
	{
		TLFClusterItem* ci = (TLFClusterItem*)m_clusterItems->Get(i);
		TLFDetectedItem* di = ci->GetDi();
		if (di != NULL)
		{
			int idx = di->GetColor();
			if (idx >= 0 && idx < LF_NUM_CLUSTERS)
			{
				awpPoint p = di->GetBounds()->Center();
				if (m_blobs[idx].area > 0)
				{
					m_blobs[idx].mxx += (p.X - m_blobs[idx].cx)*(p.X - m_blobs[idx].cx);
					m_blobs[idx].mxy += (p.Y - m_blobs[idx].cy)*(p.X - m_blobs[idx].cx);
					m_blobs[idx].myy += (p.Y - m_blobs[idx].cy)*(p.Y - m_blobs[idx].cy);
				}
			}
		}
	}

	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs[i].area > 0)
		{
			m_blobs[i].mxx /= m_blobs[i].area;
			m_blobs[i].myy /= m_blobs[i].area;
			//	moments.mxy  /= moments.area;
			m_blobs[i].mxy = 0;
			if (m_blobs[i].mxy == 0 && m_blobs[i].myy <= m_blobs[i].mxx)
			{
				m_blobs[i].angle = -AWP_PI / 2;
				m_blobs[i].major = 4 * sqrt(m_blobs[i].mxx);
				m_blobs[i].minor = 4 * sqrt(m_blobs[i].myy);

				m_blobs[i].minor_p1.X = m_blobs[i].cx;
				m_blobs[i].minor_p1.Y = m_blobs[i].cy - m_blobs[i].minor / 2;
				m_blobs[i].minor_p2.X = m_blobs[i].cx;
				m_blobs[i].minor_p2.Y = m_blobs[i].cy + m_blobs[i].minor / 2;

				m_blobs[i].major_p1.X = m_blobs[i].cx - m_blobs[i].major / 2;
				m_blobs[i].major_p1.Y = m_blobs[i].cy;
				m_blobs[i].major_p2.X = m_blobs[i].cx + m_blobs[i].major / 2;
				m_blobs[i].major_p2.Y = m_blobs[i].cy;

			}
			else if (m_blobs[i].mxy == 0 && m_blobs[i].myy > m_blobs[i].mxx)
			{
				m_blobs[i].angle = 0;
				m_blobs[i].major = 4 * sqrt(m_blobs[i].myy);
				m_blobs[i].minor = 4 * sqrt(m_blobs[i].mxx);

				m_blobs[i].minor_p1.X = m_blobs[i].cx - m_blobs[i].minor / 2;
				m_blobs[i].minor_p1.Y = m_blobs[i].cy;
				m_blobs[i].minor_p2.X = m_blobs[i].cx + m_blobs[i].minor / 2;
				m_blobs[i].minor_p2.Y = m_blobs[i].cy;

				m_blobs[i].major_p1.X = m_blobs[i].cx;
				m_blobs[i].major_p1.Y = m_blobs[i].cy - m_blobs[i].major / 2;
				m_blobs[i].major_p2.X = m_blobs[i].cx;
				m_blobs[i].major_p2.Y = m_blobs[i].cy + m_blobs[i].major / 2;
			}
			else if (m_blobs[i].myy <= m_blobs[i].mxx)
			{
				m_blobs[i].angle = atan(-2 * m_blobs[i].mxy*m_blobs[i].myy - m_blobs[i].mxx + sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy));
				m_blobs[i].major = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx + sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));
				m_blobs[i].minor = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx - sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));

				m_blobs[i].minor_p1.X = m_blobs[i].cx + 0.5*m_blobs[i].minor*cos(m_blobs[i].angle);
				m_blobs[i].minor_p1.Y = m_blobs[i].cy + 0.5*m_blobs[i].minor*sin(m_blobs[i].angle);
				m_blobs[i].minor_p2.X = m_blobs[i].cx - 0.5*m_blobs[i].minor*cos(m_blobs[i].angle);
				m_blobs[i].minor_p2.Y = m_blobs[i].cy - 0.5*m_blobs[i].minor*sin(m_blobs[i].angle);

				m_blobs[i].major_p1.X = m_blobs[i].cx + 0.5*m_blobs[i].major*cos(-AWP_PI / 2. + m_blobs[i].angle);
				m_blobs[i].major_p1.Y = m_blobs[i].cy + 0.5*m_blobs[i].major*sin(-AWP_PI / 2. + m_blobs[i].angle);
				m_blobs[i].major_p2.X = m_blobs[i].cx - 0.5*m_blobs[i].major*cos(-AWP_PI / 2. + m_blobs[i].angle);
				m_blobs[i].major_p2.Y = m_blobs[i].cy - 0.5*m_blobs[i].major*sin(-AWP_PI / 2. + m_blobs[i].angle);

			}
			else if (m_blobs[i].myy > m_blobs[i].mxx)
			{
				m_blobs[i].angle = atan(sqrt(m_blobs[i].mxx + m_blobs[i].myy + sqrt((m_blobs[i].mxx - m_blobs[i].myy)*(m_blobs[i].mxx - m_blobs[i].myy) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)) / (-2 * m_blobs[i].mxy + 0.000001));
				m_blobs[i].major = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx + sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));
				m_blobs[i].minor = sqrt(8 * (m_blobs[i].myy + m_blobs[i].mxx - sqrt((m_blobs[i].myy - m_blobs[i].mxx)*(m_blobs[i].myy - m_blobs[i].mxx) + 4 * m_blobs[i].mxy*m_blobs[i].mxy)));

				m_blobs[i].minor_p1.X = m_blobs[i].cx + 0.5*m_blobs[i].minor*cos(m_blobs[i].angle);
				m_blobs[i].minor_p1.Y = m_blobs[i].cy + 0.5*m_blobs[i].minor*sin(m_blobs[i].angle);
				m_blobs[i].minor_p2.X = m_blobs[i].cx - 0.5*m_blobs[i].minor*cos(m_blobs[i].angle);
				m_blobs[i].minor_p2.Y = m_blobs[i].cy - 0.5*m_blobs[i].minor*sin(m_blobs[i].angle);

				m_blobs[i].major_p1.X = m_blobs[i].cx + 0.5*m_blobs[i].major*cos(-AWP_PI / 2. + m_blobs[i].angle);
				m_blobs[i].major_p1.Y = m_blobs[i].cy + 0.5*m_blobs[i].major*sin(-AWP_PI / 2. + m_blobs[i].angle);
				m_blobs[i].major_p2.X = m_blobs[i].cx - 0.5*m_blobs[i].major*cos(-AWP_PI / 2. + m_blobs[i].angle);
				m_blobs[i].major_p2.Y = m_blobs[i].cy - 0.5*m_blobs[i].major*sin(-AWP_PI / 2. + m_blobs[i].angle);
			}

			m_blobs[i].sx = GetMinMomnetsX(m_blobs[i]);
			m_blobs[i].sy = GetMinMomnetsY(m_blobs[i]);
			m_blobs[i].w = GetMaxMomnetsX(m_blobs[i]) - m_blobs[i].sx;
			m_blobs[i].h = GetMaxMomnetsY(m_blobs[i]) - m_blobs[i].sy;

		}
	}
}

void TLFClusterRecursive::UpdateBlobs()
{
	DoUpdateBlobs(m_blobs, &m_clusterItems);

	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (this->SizeFilter(m_blobs[i]) && m_blobs[i].state == 0)
		{
			m_blobs[i].state = 4;
			m_blobs[i].wait = 0;
			LF_UUID_CREATE(m_blobs[i].id)
		}
		else if (m_blobs[i].state == 4)
		{
			if (this->SizeFilter(m_blobs[i]))
			{
				m_blobs[i].wait++;
				if (m_blobs[i].wait > 15)
				{
					m_blobs[i].wait = 0;
					m_blobs[i].state = 1;
				}
			}
			else
				m_blobs[i].state = 3;
		}
		else if (this->SizeFilter(m_blobs[i]) && m_blobs[i].state == 1)
		{
			m_blobs[i].state = 2;
		}
		else if (m_blobs[i].area == 0 && (m_blobs[i].state == 2 || m_blobs[i].state == 1))
		{
			m_blobs[i].state = 3;
		}
		else if (m_blobs[i].state == 3)
		{
			m_blobs[i].state = 0;
			LF_NULL_UUID_CREATE(m_blobs[i].id)
		}
	}

}
void TLFClusterRecursive::MakeClusterList()
{
	if (m_detector == NULL)
		return;
	m_clusterItems.Clear();
	for (int i = 0; i < m_detector->GetNumItems(); i++)
	{
		if (m_detector->GetItem(i)->HasObject())
		{
			TLFClusterItem* c = new TLFClusterItem(m_detector->GetItem(i));
			m_clusterItems.Add(c);
		}
	}
}

void  TLFClusterRecursive::GetDescr(int idx)
{
	m_descr = NULL;
	for (int i = 0; i < m_clusters.GetCount(); i++)
	{
		TLFClusterDescr* cd = (TLFClusterDescr*)m_clusters.Get(i);
		if (cd->GetId() == idx)
		{
			m_descr = cd;
			return;
		}
	}
}
int TLFClusterRecursive::GetBlobIndex()
{
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs[i].color == -1)
		{
			m_blobs[i].color = i;
			return i;
		}
	}
	return -1;
}

SLFBinaryBlob* TLFClusterRecursive::GetBlobs()
{
	return this->m_blobs;
}

bool TLFClusterRecursive::SizeFilter(SLFBinaryBlob& b)
{
	if (b.area == 0)
		return false;
	if (this->m_detector == NULL)
		return false;
	TLFImage*  img = m_detector->GetImage();
	if (img == NULL)
		return false;
	awpImage* _img = img->GetImage();
	if (_img == NULL)
		return false;
	double w = (double)_img->sSizeX;
	double h = (double)_img->sSizeY;

	double aw = 100*b.w / w;
	double bh = 100*b.h / h;
	if (aw > m_minw && aw < m_maxw && bh > m_minh && bh < m_maxh)
		return true;

	return false;
}

TLFClusterTrack::TLFClusterTrack(ILFObjectDetector* detector, double minw, double minh, double maxw, double maxh) : TLFClusterRecursive(detector, minw, minh, maxw, maxh)
{
	memset(this->m_blobs_tmp, 0, sizeof(this->m_blobs_tmp));
	m_descr = NULL;
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		m_blobs_tmp[i].color = -1;
	}
	m_stack_depth = 0;
}
TLFClusterTrack::~TLFClusterTrack()
{
	if (m_descr != NULL)
	delete m_descr;
}
void TLFClusterTrack::MakeClusters()
{
	if (this->m_detector == NULL)
		return;
	TLFTileScanner* scanner = dynamic_cast<TLFTileScanner*>(m_detector->GetScanner());
	if (scanner == NULL)
		return;

	int w = scanner->GetNumX() - 1;
	int h = scanner->GetNumY() - 1;

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			TLFDetectedItem* di = m_detector->GetItem(i*w + j);
			if (di != NULL && di->HasObject() && di->GetClusterIdx() > 0)
			{
				di->SetClusterIdx(-1);
			}
		}

	}

	m_cluster_idx = 0;
	m_power.clear();
	bool res = true;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			TLFDetectedItem* di = m_detector->GetItem(i*w + j);
			if (di != NULL && di->HasObject() && di->GetClusterIdx() == -1)
			{
				if (res)
				{
					m_cluster_idx++;
					SClusterDescr d;
					d.blob_idx = 0;
					d.power = 1;
					d.cluster_id = m_cluster_idx;
					if (res)
						m_power.insert(std::pair<int, SClusterDescr>(m_cluster_idx, d));
					m_stack_depth = 0;

				}
				int x = j;
				int y = i;
				m_stack_depth = 0;
				m_x = x; 
				m_y = y;
				do
				{
					res = Search(m_x, m_y, m_cluster_idx);
					m_stack_depth = 0;
				} while (!res);
			}
		}
	}
	// clear small clusters 
	std::map<int, SClusterDescr>::iterator it = m_power.begin();
	for (int i = 0; i < m_detector->GetNumItems(); i++)
	{
		TLFDetectedItem* di = m_detector->GetItem(i);
		if (di->HasObject() && di->GetClusterIdx() > 0)
		{
			int idx = di->GetClusterIdx();
			it = m_power.find(idx);
			if (it->second.power < 10)
				di->SetHasObject(false);
		}
	}

	it = m_power.begin();
	while (it != m_power.end())
	{
		if (it->second.power < 10)
		{
			m_power.erase(it);
			it = m_power.begin();
		}
		else
		{
			++it;
		}
	}
	m_cluster_count = m_power.size();
	MakeClusterList();
	UpdateBlobs();
}

void TLFClusterTrack::UpdateBlobs()
{
	DoUpdateBlobs(m_blobs_tmp, &m_clusterItems);
	// 1. remove blobs with status == 3
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs[i].state == 3)
		{
			m_blobs[i].state = 0;
			m_blobs[i].color = -1;
			LF_NULL_UUID_CREATE(m_blobs[i].id)
		}
	}
	//2. set status = 2 or status = 3 
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs[i].state > 0)
		{
			SLFBinaryBlob* b = NearlestBlob(m_blobs[i]);
			if (b == NULL)
			{
				m_blobs[i].state = 3;
			}
			else
			{
				m_blobs[i].state = 2;
				m_blobs[i].angle = b->angle;
				m_blobs[i].area = b->area;
				m_blobs[i].cx = b->cx;
				m_blobs[i].cy = b->cy;
				m_blobs[i].sx = b->sx;
				m_blobs[i].sy = b->sy;
				m_blobs[i].h = b->h;
				m_blobs[i].w = b->w;
				m_blobs[i].major = b->major;
				m_blobs[i].major_p1 = b->major_p1;
				m_blobs[i].major_p2 = b->major_p2;
				m_blobs[i].minor = b->minor;
				m_blobs[i].minor_p1 = b->minor_p1;
				m_blobs[i].minor_p2 = b->minor_p2;
				m_blobs[i].mxx = m_blobs[i].mxx;
				m_blobs[i].mxy = m_blobs[i].mxy;
				m_blobs[i].myy = m_blobs[i].myy;

				b->state = 0;
				memset(b, 0, sizeof(SLFBinaryBlob));
				b->color = -1;
			}
		}
	}
	//3. set status = 1
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs_tmp[i].area > 0 && this->SizeFilter(m_blobs_tmp[i]))
		{
			int idx = this->GetBlobIndex();
			if (idx > 0)
			{
				memcpy(&m_blobs[idx], &m_blobs_tmp[i], sizeof(SLFBinaryBlob));
				m_blobs[idx].state = 1;
				m_blobs[idx].color = idx + 1;
				LF_UUID_CREATE(m_blobs[idx].id)
			}
		}
	}
}
// blob coloring serach machine 
bool TLFClusterTrack::Search(int& x, int& y, int id)
{
	TLFTileScanner* scanner = dynamic_cast<TLFTileScanner*>(m_detector->GetScanner());
	if (scanner == NULL)
		return false;
	m_stack_depth++;
	//to aviod stack overfolw just find
	//number of  recursive calls
	if (m_stack_depth > 3000)
	{
		return false;
	}
	int w = scanner->GetNumX()-1;
	int h = scanner->GetNumY()-1;
	bool res = false;
	TLFDetectedItem* di = m_detector->GetItem(y*w + x);
	di->SetClusterIdx(id);
	di->SetColor(id);
	for (int i = y - 2; i <= y + 4; i++)
	{
		if (i < 0 || i > h)
			continue;
		for (int j = x - 2; j <= x + 4; j++)
		{
			if (j < 0 || j > w)
				continue;
			di = m_detector->GetItem(i*w + j);
			if (di != NULL && di->HasObject() && di->GetClusterIdx() == -1)
			{
				m_power[id].power++;
				m_x = j;
				m_y = i;
				res = Search(j, i, id);
				if (!res)
				{
					return res;
				}
			}
		}
	}
	return true;
}

static double BlobDist(SLFBinaryBlob& b1, SLFBinaryBlob& b2)
{
	return sqrt((b1.cx - b2.cx)*(b1.cx - b2.cx) + (b1.cy - b2.cy)*(b1.cy - b2.cy));
}
SLFBinaryBlob* TLFClusterTrack::NearlestBlob(SLFBinaryBlob& blob)
{
	double mind = 1000000000;
	int idx = -1;
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (m_blobs_tmp[i].area > 0)
		{
			double d = BlobDist(blob, m_blobs_tmp[i]);
			if (d < mind)
			{
				mind = d;
				idx = i;
			}
		}
	}
	if (mind < 20 && idx >= 0)
		return &m_blobs_tmp[idx];
	else
		return NULL;
}
