
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
//		File: LFCommon.cpp
//		Purpose: Implements common classses: TLFObject, TLFListNode, TLFList, TLFObjectList
//
//      CopyRight 2004-2018 (c) NN-Videolab.net
//M*/
#include "_LF.h"

/*
    TLFObject
*/
TLFObject::TLFObject()
{
}
TLFObject::~TLFObject()
{
}

const char* TLFObject::GetName()
{
	return "TLFObject";
}

/*
	TLFListNode
*/

TLFListNode::TLFListNode(TLFObject* object)
{
	this->m_next = NULL;
	this->m_element = object;
}

TLFObject* TLFListNode::GetElement()
{
	return this->m_element;
}
void       TLFListNode::SetElement(TLFObject* object)
{
	this->m_element = object;
}
TLFListNode*   TLFListNode::NextNode()
{
	return this->m_next;
}

void		   TLFListNode::SetNextNode(TLFListNode* node)
{
	this->m_next = node;
}


TLFList::TLFList()
{
	m_headNode = NULL;
}

TLFList::~TLFList()
{
	Clear();
}

TLFObject* TLFList::First()
{
	if (m_headNode == NULL)
		return NULL;
	return m_headNode->GetElement();
}
TLFObject* TLFList::Last()
{
	if (m_headNode == NULL)
		return NULL;
	TLFListNode* node = m_headNode;
	while (node->NextNode() != NULL)
		node = node->NextNode();
	return node->GetElement();
}

TLFObject* TLFList::Next(TLFObject* object)
{
	if (m_headNode == NULL)
		return NULL;
	TLFListNode* node = m_headNode;
	while (node->NextNode() != NULL)
	{
		if (node->GetElement() == object)
			return node->NextNode()->GetElement();
		node = node->NextNode();
	}
	return NULL;
}
void TLFList::Clear(bool del)
{
	if (m_headNode == NULL)
		return;
	TLFListNode* newnode = m_headNode->NextNode();
	while(newnode != NULL)
	{
		TLFListNode* node = newnode;
		newnode = newnode->NextNode();
		if (del && node->GetElement() != NULL)
			delete node->GetElement();

		delete node;
	}
	delete m_headNode;
	m_headNode = NULL;
}

void TLFList::PushBack(TLFObject* object)
{
	TLFListNode* node = new TLFListNode(object);
	if (m_headNode == NULL)
	{
		m_headNode = node;
	}
	else
	{
		TLFListNode* tmp_node = m_headNode;
		while (tmp_node->NextNode() != NULL)
			tmp_node = tmp_node->NextNode();
		tmp_node->SetNextNode(node);
	}
}
void TLFList::PopBack()
{
	if (m_headNode == NULL)
		return;
	/*
		находим последний узел и удаляем его. 
		у предпоследнего узла устанавливаем 
		следующий узел в NULL
	*/
	TLFListNode* node = m_headNode;					//предпоследний узел
	TLFListNode* node1 = m_headNode->NextNode();	//последний узел
	if (node1 == NULL)
	{
		/*в списке всего один элемент - удалим его*/
		delete node;
		m_headNode = NULL;
		return;
	}
	else
	{
		/*в списке больше чем один элемент, надо найти последний 
		и предпоследний*/
		while (true)
		{
			if (node1->NextNode() == NULL)
			{
				node->SetNextNode(NULL);
				delete node1;
				return;
			}

			node = node1;				// устанавливаем предпоследний элемент
			node1 = node1->NextNode();  // устанавливаем последний элемент
		}
	}
}
TLFObject* TLFList::Pop(TLFObject* object)
{
	if (m_headNode == NULL)
		return NULL;
	if (First() == object)
	{
		// удаляем первый элемент. 
		TLFListNode* node = m_headNode->NextNode();
		delete m_headNode;
		m_headNode = node;
		if (node != NULL)
			return node->GetElement();
		else
			return NULL;
	}
	else if (Last() == object)
	{
		PopBack();
		return NULL;
	}
	else
	{
		TLFListNode* node  = m_headNode;
		TLFListNode* node1 = m_headNode->NextNode();
		if (node1 == NULL)
			return NULL;
		while (node1->NextNode() != NULL)
		{
			if (node1->GetElement() == object)
			{
				node->SetNextNode(node1->NextNode());
				delete node1;
				if (node->NextNode() != NULL)
					return node->NextNode()->GetElement();
				else
					return NULL;
			}
			node = node1;
			node1 = node1->NextNode();
		}
	}
	return NULL;
}


/*
    TLFObjectList
*/
TLFObjectList::TLFObjectList()
{
    m_Count = 0;
    m_Capacity = 0;
    m_List = NULL;
    SetCapacity(0);
}
TLFObjectList::~TLFObjectList()
{
    Clear();
}

TLFObject*  TLFObjectList::Get(int index)
{
    if (index < 0 || index >= m_Count)
        return NULL;
    return m_List[index];
}

void TLFObjectList::Grow()
{
    int Delta;
    if (m_Capacity > 64)
        Delta = m_Capacity / 4;
    else if (m_Capacity > 8)
        Delta = 16;
    else
        Delta = 4;
    SetCapacity(m_Capacity + Delta);    
}

void TLFObjectList::Put(int index, TLFObject* pObject)
{
    if (index < 0 || index >= m_Count)
        return;
    if (pObject != m_List[index])
        m_List[index] = pObject;
}

void TLFObjectList::SetCapacity(int nNewCapacity)
{
    if (nNewCapacity < m_Count)
        return;
    if (nNewCapacity != m_Capacity)
    {
        m_List = (TLFObject**)realloc(m_List, nNewCapacity*sizeof(TLFObject**));
        m_Capacity = nNewCapacity;
    }
}

void TLFObjectList::SetCount(int NewCount)
{
    int i;
    if (NewCount < 0)
        return;  //DONE: here is error
    if (NewCount > m_Capacity)
        SetCapacity(NewCount);
    if (NewCount > m_Count)
        memset(&m_List[m_Count],0,(NewCount - m_Count)*sizeof(TLFObject**));
    else
        for (i = m_Count -1; i >= NewCount; i--)
        {
            Delete(i);
        }
    m_Count = NewCount;
}


int TLFObjectList::Add(TLFObject* pObject)
{
    int res = m_Count;
    if (res == m_Capacity)
        Grow();
    m_List[res] = pObject;
    m_Count++;
    return res;
}
void TLFObjectList::Clear()
{
//    SetCount(0);
	for (int i = 0; i < this->m_Count; i++)
	{
		TLFObject* tmp = Get(i);
		delete tmp;
	}
	m_Count = 0;
    SetCapacity(0);
}
void TLFObjectList::Delete(int index)
{
    if (index < 0 || index >= m_Count)
        return;
    TLFObject* tmp = Get(index);
    if (tmp != NULL)
        delete tmp;
//    m_Count--;
    if (index < m_Count)
	{
      //  MoveMemory(&m_List[index], &m_List[index +1], (m_Count - index)*sizeof(TLFObject**));
      for (int i = index+1; i <= m_Count; i++)
      {
        m_List[i-1] = Get(i);
      }
    }
    m_Count--;
}

void TLFObjectList::Exchange(int index1, int index2)
{
   TLFObject* tmp;
   if (index1 < 0 || index1 >= m_Count)
        return;
   if (index2 < 0 || index2 >= m_Count)
        return;
   tmp = m_List[index1];
   m_List[index1] = m_List[index2];
   m_List[index2] = tmp;
}
TLFObjectList* TLFObjectList::Expand()
{
    if (m_Count == m_Capacity)
        Grow();
    return this;
}
TLFObject* TLFObjectList::Extract(TLFObject* pObject)
{
    int i;
	TLFObject* res = NULL;
    i = IndexOf(pObject);
    if (i >= 0)
    {
        res = pObject;
        m_List[i] = NULL;
        Delete(i);
    }
    return res;
}

TLFObject* TLFObjectList::First()
{
    return Get(0);
}
int TLFObjectList::IndexOf(TLFObject* pObject)
{
    int res = 0;
    while (res < m_Count && m_List[res] != pObject)
        res++;
    if (res == m_Count)
        res = -1;
    return res;
}

void TLFObjectList::Insert(int index, TLFObject* pObject)
{
    if (index < 0 || index > m_Count)
        return;
    if (m_Count == m_Capacity)
		Grow();
    if (index < m_Count)
        memcpy(&m_List[index], &m_List[index +1], (m_Count - index)*sizeof(TLFObject**));
    m_List[index] = pObject;
    m_Count++;
}

TLFObject* TLFObjectList::Last()
{
    return Get(m_Count -1);
}

void TLFObjectList::Move(int nCurIndex, int nNewIndex)
{
    TLFObject* tmp;
    if (nCurIndex != nNewIndex)
    {
       if (nCurIndex < 0 || nCurIndex >= m_Count)
            return;
       if (nNewIndex < 0 || nNewIndex >= m_Count)
            return;
        tmp = Get(nCurIndex);
        m_List[nCurIndex] = NULL;
        Delete(nCurIndex);
        Insert(nNewIndex, NULL);
        m_List[nNewIndex] = tmp;
    }
}

int TLFObjectList::Remove(TLFObject* pObject)
{
    int res = IndexOf(pObject);
    if (res >= 0)
        Delete(res);
    return res;
}

void TLFObjectList::Pack()
{
     int i;
     for (i = m_Count -1; i >=0; i--)
        if (m_List[i] == NULL)
              Delete(i);
}
void QuickSort(TLFObject** pList, int l, int r, TLFListSortCompare Compare)
{
    int i, j;
    void* P;
    void* T;

    do
    {
        i = l;
        j = r;
        P = pList[(l+r)>>1];
        do
        {
           while (Compare(pList[i], P) < 0)
            i++;
           while (Compare(pList[j], P) >0 )
			j--;
            if (i <= j )
            {
                T = pList[i];
                pList[i] = pList[j];
                pList[j] = (TLFObject*)T;
                i++;
                j--;
            }
        }while(i < j);
        if (l < j)
            QuickSort(pList, l,j, Compare);
        l = i;
    }while(i <= r);
}

void TLFObjectList::Sort(TLFListSortCompare Compare)
{
    if (m_List != NULL && m_Count > 0)
        QuickSort(m_List, 0, m_Count -1, Compare);
}

// property
int TLFObjectList::GetCapacity()
{
    return m_Capacity;
}

int TLFObjectList::GetCount()
{
	return m_Count;
}

TLFObject** TLFObjectList::GetList()
{
    return m_List;
}



/*-------------- семантическое описание изображения --------------------------*/
TLFSemanticImageDescriptor::TLFSemanticImageDescriptor()
{
	m_imageWidth = 0;
	m_imageHeight = 0;
}
TLFSemanticImageDescriptor::TLFSemanticImageDescriptor(awpImage* pImage)
{
	m_imageWidth = 0;
	m_imageHeight = 0;

	if (pImage != NULL)
	{
		m_imageWidth = pImage->sSizeX;
		m_imageHeight = pImage->sSizeY;
	}
}

TLFSemanticImageDescriptor::TLFSemanticImageDescriptor(int w, int h)
{
	m_imageWidth = w;
	m_imageHeight = h;
}

TLFSemanticImageDescriptor::~TLFSemanticImageDescriptor()
{
}
/*работа с изображением*/
void TLFSemanticImageDescriptor::SetImage(awpImage* pImage)
{
	if (pImage != NULL)
	{
		m_imageWidth = pImage->sSizeX;
		m_imageHeight = pImage->sSizeY;
	}
}

int TLFSemanticImageDescriptor::Width()
{
	return m_imageWidth;
}
int TLFSemanticImageDescriptor::Height()
{
	return m_imageHeight;
}

/*работа с xml файлом*/
bool TLFSemanticImageDescriptor::SaveXML(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	TiXmlElement* dscr = new TiXmlElement(this->GetName());
	doc.LinkEndChild(dscr);
	dscr->SetAttribute("ImageWidth",  m_imageWidth);
	dscr->SetAttribute("ImageHeight", m_imageHeight);
	for (int i = 0; i < GetCount(); i++)
	{
		 TLFDetectedItem* di = this->GetDetectedItem(i);
		 TiXmlElement* e = di->SaveXML();
		 if (e != NULL)
			dscr->LinkEndChild(e);
	}
	return doc.SaveFile(lpFileName);
}
bool TLFSemanticImageDescriptor::LoadXML(const char* lpFileName)
{
   this->Clear();

   TiXmlDocument doc;
   if (!doc.LoadFile(lpFileName))
	return false;
   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;

   pElem = hDoc.FirstChildElement().Element();
   if (pElem == NULL)
	return false;
   const char* elem_value = pElem->Value();
   const char* this_name = GetName();
   if (strcmp(pElem->Value(), GetName()) != 0)
	return false;

   int w, h;
   pElem->QueryIntAttribute("ImageWidth", &w);
   pElem->QueryIntAttribute("ImageHeight", &h);

   for(TiXmlNode* child = pElem->FirstChild(); child; child = child->NextSibling() )
   {
		TLFDetectedItem* di = new TLFDetectedItem();
		if (strcmp(child->Value(), di->GetName()) != 0)
		{
			Clear();
			return false;
		}
		if (!di->LoadXML(child->ToElement()))
		{
			Clear();
			return false;
		}
		Add(di);
   }

   return true;
}
bool     TLFSemanticImageDescriptor::_LoadXml(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	TiXmlElement* pElem = NULL;

	pElem = parent;// ->FirstChildElement();//hDoc.FirstChildElement().Element();
	if (pElem == NULL)
		return false;

	if (strcmp(pElem->Value(), GetName()) != 0)
		return false;

	int w, h;
	pElem->QueryIntAttribute("ImageWidth", &w);
	pElem->QueryIntAttribute("ImageHeight", &h);

	for (TiXmlNode* child = pElem->FirstChild(); child; child = child->NextSibling())
	{
		TLFDetectedItem* di = new TLFDetectedItem();
		if (strcmp(child->Value(), di->GetName()) != 0)
		{
			Clear();
			return false;
		}
		if (!di->LoadXML(child->ToElement()))
		{
			Clear();
			return false;
		}
		Add(di);
	}
	return true;
}
bool	 TLFSemanticImageDescriptor::_SaveXml(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;

	TiXmlElement* dscr = new TiXmlElement(this->GetName());
	dscr->SetAttribute("ImageWidth", m_imageWidth);
	dscr->SetAttribute("ImageHeight", m_imageHeight);
	for (int i = 0; i < GetCount(); i++)
	{
		TLFDetectedItem* di = this->GetDetectedItem(i);
		TiXmlElement* e = di->SaveXML();
		if (e != NULL)
			dscr->LinkEndChild(e);
	}
	parent->LinkEndChild(dscr);

	return true;
}

TiXmlElement* TLFSemanticImageDescriptor::SaveXML()
{
	TiXmlElement* dscr = new TiXmlElement(this->GetName());
	dscr->SetAttribute("ImageWidth",  m_imageWidth);
	dscr->SetAttribute("ImageHeight", m_imageHeight);
	for (int i = 0; i < GetCount(); i++)
	{
		 TLFDetectedItem* di = this->GetDetectedItem(i);
		 TiXmlElement* e = di->SaveXML();
		 if (e != NULL)
			dscr->LinkEndChild(e);
	}
	return dscr;
}
bool TLFSemanticImageDescriptor::LoadXML(TiXmlElement* e)
{
   this->Clear();

   TiXmlElement* pElem = e;
   if (pElem == NULL)
	return false;

   if (strcmp(pElem->Value(), GetName()) != 0)
	return false;

   int w, h;
   pElem->QueryIntAttribute("ImageWidth", &w);
   pElem->QueryIntAttribute("ImageHeight", &h);

   for(TiXmlNode* child = pElem->FirstChild(); child; child = child->NextSibling() )
   {
		TLFDetectedItem* di = new TLFDetectedItem();
		if (strcmp(child->Value(), di->GetName()) != 0)
		{
			Clear();
			return false;
		}
		if (!di->LoadXML(child->ToElement()))
		{
			Clear();
			return false;
		}
		Add(di);
   }

   return true;

}

#ifdef LOAD_FROM_STREAM
bool TLFSemanticImageDescriptor::LoadStream(std::istream& in)
{
	TiXmlDocument doc;
	in >> doc;


	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	pElem = hDoc.FirstChildElement().Element();
	return this->_LoadXml(pElem);
}
#endif
/*работа со списком найденных объектов*/
/*редактирование*/
bool TLFSemanticImageDescriptor::AddDetectedItem(TLFDetectedItem* item)
{
   //if (this->m_Image.GetImage() == NULL)
   //     return false;
    if (item == NULL)
        return false;
    if (item->GetBounds() == NULL)
        return false;

    TLFRect* r = item->GetBounds();
    awpRect rr = r->GetRect();
	/*
    if (awpRectInImage(this->m_Image.GetImage(), &rr) !=AWP_OK)
    {
        // уменьшим прямоугольник на 10%
        int delta = (rr.right - rr.left) / 20;
        if (delta > 0)
        {
            rr.left += delta;
            rr.right -= delta;
            rr.top += delta;
            rr.bottom -= delta;
            if (awpRectInImage(this->m_Image.GetImage(), &rr) !=AWP_OK)
                return false;
            delete item;
			item = new TLFDetectedItem(&rr, 0, detUnkonwn, item->GetAngle(), item->GetRacurs(), item->GetBW(), item->GetBH());

        }
        else
            return false;
    }
	*/

    Add(new TLFDetectedItem(item));
    return true;
}
bool TLFSemanticImageDescriptor::DeleteDetectedItem(int Index)
{
    if (Index < 0 || Index >= GetCount())
		return false;
    Delete(Index);
    return true;
}

/*свойства*/
int TLFSemanticImageDescriptor::GetItemsCount()
{
    return GetCount();
}
TLFDetectedItem* TLFSemanticImageDescriptor::GetDetectedItem(int index)
{
    if (index < 0 || index >= GetCount())
        return NULL;
    return (TLFDetectedItem*)Get(index);
}

void TLFSemanticImageDescriptor::Resize(double factor)
{
    for (int i = 0; i < GetCount(); i++)
    {
        TLFDetectedItem* di = (TLFDetectedItem*)Get(i);
		di->Resize(factor);
    }
}


/*сравнение семантических описаний изображения*/
double TLFSemanticImageDescriptor::Compare(const char* lpFileName, double overlap)
{
    TLFSemanticImageDescriptor* descr = new TLFSemanticImageDescriptor();
    if (descr->LoadXML(lpFileName))
    {
        double result = this->Compare(descr, overlap);
        delete descr;
        return result;
    }
    return 0;
}
/**
	finds the number of non-matched elements from Descriptor
*/
double TLFSemanticImageDescriptor::Compare(TLFSemanticImageDescriptor* Descriptor, double overlap)
{
	int count = 0;
	if (Descriptor == NULL)
		return this->GetCount();
	if (Descriptor->GetCount() == 0)
		return this->GetCount();
	for (int i = 0; i < GetCount(); i++)
	{
		TLFDetectedItem* item1 = this->GetDetectedItem(i);
		TLFRect* rect1 = item1->GetBounds();
		if (Descriptor->Overlap(*rect1) > overlap)
			count++;
	}
	return GetCount() - count;
}

double TLFSemanticImageDescriptor::Overlap(TLFRect& rect)
{
	if (this->GetCount() == 0)
		return 0;
	double max_overlap = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		TLFDetectedItem* di = GetDetectedItem(i);
		TLFRect* di_rect = di->GetBounds();
		double v = rect.RectOverlap(*di_rect);
		if (v > max_overlap)
			max_overlap = v;
	}
	return max_overlap;
}



int TLFSemanticImageDescriptor::GetItemSize(int index)
{
	if (index < 0 || index >= GetCount())
        return 0;
    int si;
    //if (this->m_Image.GetImage() == NULL)
    //    return 0;
    si = m_imageHeight*m_imageWidth;


   TLFDetectedItem* di =  (TLFDetectedItem*)Get(index);
   if (di == NULL)
    return 0;

   TLFRect* r = di->GetBounds();
   if (r == NULL)
    return 0;
   awpRect rr = r->GetRect();

   int so = (rr.right - rr.left)*(rr.bottom-rr.top);
   double a = 100.f*(double)so / (double)si;

   if (a >= 50)
    return 1;
   else if (a <= 3)
    return 5;
   else if (a > 3 && a <= 10)
    return 4;
   else if (a > 10 && a <=20)
    return 3;
   else if (a > 20 && a < 50)
     return 2; 

   return 0;
}

int TLFSemanticImageDescriptor::GetItemPos (int index)
{
    if (index < 0 || index >= GetCount())
		return 0;
    //if (this->m_Image.GetImage() == NULL)
    //    return 0;

    TLFDetectedItem* di =  (TLFDetectedItem*)Get(index);
	if (di == NULL)
          return 0;

   TLFRect* r = di->GetBounds();
   if (r == NULL)
    return 0;
   awpRect rr = r->GetRect();

   awpPoint p;
   p.X = (rr.left + rr.right) / 2;
   p.Y = (rr.top + rr.bottom) / 2;

   int w = m_imageWidth;
   int h = m_imageHeight;

   int count = 0;
   for (int sy = 0 ; sy < h; sy += h/3)
   {
     for (int sx = 0; sx < w; sx += w/3)
     {
        count++;
        awpRect tr;
        tr.left = sx;
        tr.right = sx + w/3;
        tr.top = sy;
        tr.bottom = sy + h / 3;

        if (p.X >= sx && p.X < tr.right && p.Y >= sy && p.Y < tr.bottom)
            return count;

     }
   }

    return count;
}

void TLFSemanticImageDescriptor::Predict(ILFDetectEngine* engine)
{
	double d_factor = engine->GetResize() ? 1. / engine->GetResizeCoef() : 1;
	double u_factor = engine->GetResize() ? engine->GetResizeCoef() : 1;
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFDetectedItem* di = GetDetectedItem(i);
		di->Resize(d_factor);
		TLFRect* r = di->Predict(engine);
		di->Resize(u_factor);
	}
}

void TLFSemanticImageDescriptor::Update(ILFDetectEngine* engine,  TLFSemanticImageDescriptor* sid)
{
	if (engine == NULL || sid == NULL)
		return;
	// сюда приходит sid без предикторов. это временное состояние машины
	// здесь к элементу описания добавляется предиктор и он записывается в этот sid 
	double d_factor = engine->GetResize() ? 1. / engine->GetResizeCoef() : 1;
	double u_factor = engine->GetResize() ? engine->GetResizeCoef() : 1;
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFDetectedItem* di1 = this->GetDetectedItem(i);

		if (di1)
		{
			di1->Resize(d_factor);
			
			TLFRect* rect1 = di1->GetBounds();
			bool found = false;
			for (int j = sid->GetCount()-1; j >=0; j--)
			{
				TLFDetectedItem* di2 = sid->GetDetectedItem(j);
				if (di2)
				{
					di2->Resize(d_factor);
					TLFRect* rect2 = di2->GetBounds();
					if (rect1->RectOverlap(*rect2) > 0)
					{
						found = true;
						di1->Update(engine, rect2);
						sid->Delete(j);
					}
					else
					 di2->Resize(u_factor);
				}
			}

			if (!found)
				di1->Update(engine, NULL);

			di1->Resize(u_factor);
		}
	}

	// clear intersections
	for (int i = 0; i < GetCount(); i++)
	{
		TLFDetectedItem* d0 = this->GetDetectedItem(i);
		TLFRect* r0 = d0->GetBounds();
		double s0 = r0->Square ();
		for (int j = i+1; j < GetCount(); j++ )
		{
			TLFDetectedItem* d1 = this->GetDetectedItem(j);
			TLFRect* r1 = d1->GetBounds();
			if (r0->RectOverlap(*r1) > 0.1)
			{
				double s1 = r1->Square();
			if (s0 < s1)
					d0->SetState(3);
				else
					d1->SetState(3);
			}
		}

	}

	// remove all elements whose Health ==0
	for (int i = this->GetCount() - 1; i >= 0; i--)
	{
		TLFDetectedItem* item = this->GetDetectedItem(i);
		if (item->GetHealth() == 0 && item->GetState() == 3)
			Delete(i);
		else if (item->GetState() == 3)
        {
			item->Delete();
        }
	}
	// adding new elements
	for (int i = 0; i < sid->GetCount(); i++)
	{
		TLFDetectedItem* di = sid->GetDetectedItem(i);
		awpRect pRect = di->GetBounds()->GetRect();
		ILFPredictor* predictor = LFCreatePredictor(engine->GetPredictorName(), engine);
		UUID id;
        LF_UUID_CREATE(id);
        TLFDetectedItem* item = new TLFDetectedItem(&pRect, di->GetRaiting(), di->GetType(), di->GetAngle(), di->GetRacurs(), di->GetBW(), di->GetBH(), di->GetDetectorName(), id, predictor);
		item->Resize(d_factor);
		TLFRect* rect;
		rect = item->GetBounds();
		item->Update(engine, rect);
		item->Resize(u_factor);
		Add(item);
	}
}


