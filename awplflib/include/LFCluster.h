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
//		File: LFCluster.h
//		Purpose:  Binary Clusters support
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/

#ifndef _lf_cluster_h_
#define _lf_cluster_h_
/** \defgroup LFCluster
*	Implementation of binary clusters 
*   @{
*/
#define LF_NUM_CLUSTERS 32

typedef struct tagBinaryBlob
{
	UUID   id;
	int    state;
	int    color;
	int	   wait;
	double cx;
	double cy;
	double sx;
	double sy;
	double w;
	double h;
	double mxx;
	double mxy;
	double myy;
	double major;
	double minor;
	double angle;
	double area;
	awp2DPoint major_p1;
	awp2DPoint major_p2;
	awp2DPoint minor_p1;
	awp2DPoint minor_p2;

}SLFBinaryBlob;



class TLFClusterItem : public  TLFObject
{
protected:
	int m_clusterID;
	TLFDetectedItem* m_di;
public:
	TLFClusterItem(TLFDetectedItem* item);
	int GetClusterID();
	void SetClusterID(int id);
	TLFDetectedItem* GetDi();
};
class TLFClusterDescr : public TLFObject
{
protected:
	int m_power;
	int m_cluster_id;
	int m_blob_idx; 
public:
	TLFClusterDescr(int id, int blob_id);
	int GetId();
	int GetPower();
	void IncPower();
	void Clear();
	int  GetBlobId();
};

class TLFClusterMaker : public TLFObject
{
protected:
	SLFBinaryBlob      m_blobs[LF_NUM_CLUSTERS];
	ILFObjectDetector* m_detector;
	TLFObjectList    m_clusterItems;
	int m_cluster_count;
	TLFImage  m_matrix;

	void MakeClusterList();
	void Clustering();
	void Painting();
	void UpdateBlobs();
	int  GetNewClusterIndex();
	int GetUnclustered();
	int GetNearlestItem(awpImage* img, int idx, double& dist);
	int GetnearlestItemWithClusterIndex(awpImage* img, int idx, double& dist);
	double GetNearleastItemToCluster(int clusterID, int& idx);
	double Distance(TLFClusterItem* di1, TLFClusterItem* di2);
public:
	TLFClusterMaker(ILFObjectDetector* detector);
	virtual ~TLFClusterMaker();

	void MakeClusters();
	int NumClusters();
};

typedef struct tagSClusterDescr
{
	int power;
	int cluster_id;
	int blob_idx;
}SClusterDescr;
class TLFClusterRecursive : public TLFObject
{
protected: 
	double m_minw;
	double m_minh;
	double m_maxw;
	double m_maxh;
	int    m_wait_count;
	SLFBinaryBlob      m_blobs[LF_NUM_CLUSTERS];
	ILFObjectDetector* m_detector;
	int m_cluster_count;
    int m_cluster_idx;
	TLFClusterDescr* m_descr;
	TLFObjectList    m_clusterItems;
	TLFObjectList    m_clusters;
	virtual void UpdateBlobs();
	virtual bool Search(int& x, int& y, int id);
	void MakeClusterList();
	void  GetDescr(int idx);
	int GetBlobIndex();
	bool SizeFilter(SLFBinaryBlob& b);
public: 
	TLFClusterRecursive(ILFObjectDetector* detector, double minw, double minh, double maxw, double maxh);
	virtual ~TLFClusterRecursive();

	virtual void MakeClusters();
	int NumClusters();
	SLFBinaryBlob* GetBlobs();
};

class TLFClusterTrack : public TLFClusterRecursive
{
protected:
	SLFBinaryBlob      m_blobs_tmp[LF_NUM_CLUSTERS];
	virtual void UpdateBlobs();
	virtual bool Search(int& x, int& y, int id);
	SLFBinaryBlob* NearlestBlob(SLFBinaryBlob& blob);
	std::map<int, SClusterDescr> m_power;
	std::vector<SLFBinaryBlob> m_result;
	int m_stack_depth;
	int m_x;
	int m_y;
public:
	TLFClusterTrack(ILFObjectDetector* detector, double minw, double minh, double maxw, double maxh);
	virtual ~TLFClusterTrack();
	virtual void MakeClusters();
};
/** @} */ /*  end of LFCluster group */
#endif //_lf_cluster_h_