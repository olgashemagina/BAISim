#include "_LF.h"
/*
TLFLandmark
*/
TLFLandmark::TLFLandmark(TLFLandmarkAttr* attr, awp2DPoint point, AWPDOUBLE status)
{
	m_landmark = point;
	m_attr = attr;
	m_status = status;
}
TLFLandmark::~TLFLandmark()
{
}

TiXmlElement* TLFLandmark::SaveXML()
{
	if (m_attr == NULL)
		return NULL;

	TiXmlElement* f = new TiXmlElement(GetName());
	TLFString id = m_attr->GetID();
	f->SetAttribute("ID", id.c_str());
	f->SetDoubleAttribute("X", m_landmark.X);
	f->SetDoubleAttribute("Y", m_landmark.Y);
	f->SetDoubleAttribute("S", m_status);
	return f;
}
TLFLandmark*  TLFLandmark::LoadXML(TiXmlElement* parent, TLFLandmarkAttributes* attrs)
{
	if (parent == NULL || attrs == NULL)
		return NULL;
	const char* name = parent->Value();
	if (strcmp(name, "TLFLandmark") != 0)
		return NULL;
	const char* _id = parent->Attribute("ID");
	if (_id == NULL)
		return NULL;
	TLFString strUUID = _id;
	TLFLandmarkAttr* attr = attrs->Attribute(_id);
	if (attr == NULL)
		return NULL;
	awp2DPoint p;
	parent->QueryDoubleAttribute("X"	,&p.X);
	parent->QueryDoubleAttribute("Y",	&p.Y);
	AWPDOUBLE d = 0;
	parent->QueryDoubleAttribute("S", &d);
	return new TLFLandmark(attr, p, d);
}

const char* TLFLandmark::GetId()
{
	TLFString str = m_attr->GetID();
	return str.c_str();
}

const char* TLFLandmark::ClassName()
{
	return m_attr->ClassName();
}

int			TLFLandmark::Color()
{
	return m_attr->Color();
}

awp2DPoint TLFLandmark::landmark()
{
	return m_landmark;
}
AWPDOUBLE  TLFLandmark::x()
{
	return m_landmark.X;
}
AWPDOUBLE  TLFLandmark::y()
{
	return m_landmark.Y;
}

void TLFLandmark::SetPoint(awp2DPoint point)
{
	m_landmark = point;
}
void TLFLandmark::SetX(AWPDOUBLE x)
{
	m_landmark.X = x;
}
void TLFLandmark::SetY(AWPDOUBLE y)
{
	m_landmark.Y = y;
}
AWPDOUBLE  TLFLandmark::Status()
{
	return m_status;
}
void TLFLandmark::SetStatus(AWPDOUBLE value)
{
	m_status = value;
}

/*
	TLFLandmarkAttr
*/
TLFLandmarkAttr::TLFLandmarkAttr()
{
	UUID id;
	LF_UUID_CREATE(id)
	m_id = LFGUIDToString(&id);
	awpRGBtoWeb(0,255,0, &m_color);
	m_ClassName = "landmark";
}
TLFLandmarkAttr::TLFLandmarkAttr(TLFString& id, int color, const char* className)
{
	m_color = color;
	m_ClassName = className;
	m_id = id;
}

TLFLandmarkAttr::~TLFLandmarkAttr()
{

}

const char* TLFLandmarkAttr::ClassName()
{
	return m_ClassName.c_str();
}
void TLFLandmarkAttr::SetClassName(const char* value)
{
	m_ClassName = value;
}

int TLFLandmarkAttr::Color()
{
	return m_color;
}
void TLFLandmarkAttr::SetColor(int value)
{
	m_color = value;
}
void TLFLandmarkAttr::SetColor(AWPBYTE r, AWPBYTE g, AWPBYTE b)
{
	awpRGBtoWeb(r,g,b, &m_color);
}


const char* TLFLandmarkAttr::GetID()
{
	return m_id.c_str();
}

TiXmlElement* TLFLandmarkAttr::SaveXML()
{
	TiXmlElement* f = new TiXmlElement(GetName());
	f->SetAttribute("ID", m_id.c_str());
	f->SetAttribute("ClassName", m_ClassName.c_str());
	f->SetAttribute("Color", m_color);
	return f;
}
TLFLandmarkAttr* TLFLandmarkAttr::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return NULL;
	const char* name = parent->Value();
	if (strcmp(name, "TLFLandmarkAttr") != 0)
		return NULL;
	const char* _id = parent->Attribute("ID");
	if (_id == NULL)
		return NULL;
	TLFString strUUID = _id;
	int color;
	if (parent->QueryIntAttribute("Color", &color) == TIXML_NO_ATTRIBUTE)
		return NULL;
	const char* className = parent->Attribute("ClassName");
	if (className == NULL)
		return NULL;
	return new TLFLandmarkAttr(strUUID, color, className);
}
/*
TLFLandmarkAttributes
*/
TLFLandmarkAttributes::TLFLandmarkAttributes()
{

}
TLFLandmarkAttributes::~TLFLandmarkAttributes()
{

}

void TLFLandmarkAttributes::Clear()
{
	this->m_attributes.Clear();
}
void TLFLandmarkAttributes::Append(TLFLandmarkAttr* attr)
{
	this->m_attributes.Add(attr);
}
void TLFLandmarkAttributes::Delete(int index, TLFLandmarkFiles* files)
{
	//todo: remove all points with attr UUID in the database 
	TLFLandmarkAttr* attr = Attribute(index);
	if (attr == NULL)
		return;
	if (files != NULL)
	{
		TLFString id = attr->GetID();
		for (int i = 0; i < files->Count(); i++)
		{
			TLFLandmarkFile* file = files->File(i);
			for (int j = file->Count() - 1; j >= 0; j--)
			{
				TLFLandmark* lnd = file->Landmark(j);
				TLFString lndid = lnd->GetId();
				if (id == lndid)
					file->Delete(j);
			}
		}
	}
	this->m_attributes.Delete(index);
}

TLFLandmarkAttr* TLFLandmarkAttributes::Attribute(int index)
{
	return dynamic_cast<TLFLandmarkAttr*>(m_attributes.Get(index));
}

TLFLandmarkAttr* TLFLandmarkAttributes::Attribute(const TLFString& id)
{
	TLFLandmarkAttr* result = NULL;
	for (int i = 0; i < m_attributes.GetCount(); i++)
	{
		TLFLandmarkAttr* attr = Attribute(i);
		TLFString strId = attr->GetID();
		if (id == strId)
		{
			result = attr;
			break;
		}
	}
	return result;
}

int TLFLandmarkAttributes::Count()
{
	return this->m_attributes.GetCount();
}
TLFObjectList* TLFLandmarkAttributes::GetList()
{
	return &m_attributes;
}

TiXmlElement* TLFLandmarkAttributes::SaveXML()
{
	TiXmlElement* e = new TiXmlElement(GetName());
	for (int i = 0; i < m_attributes.GetCount(); i++)
	{
		TLFLandmarkAttr* attr = Attribute(i);
		if (attr != NULL)
		{
			TiXmlElement* f = attr->SaveXML();
			if (f != NULL)
				e->LinkEndChild(f);
			else
			{
				delete e;
				return NULL;
			}
		}
		else
		{
			delete e;
			return NULL;
		}
	}
	return e;
}
TLFLandmarkAttributes* TLFLandmarkAttributes::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return NULL;
	parent = parent->FirstChildElement("TLFLandmarkAttributes");
	if (parent == NULL)
		return NULL;

	TLFLandmarkAttributes* result = new TLFLandmarkAttributes();
	for (TiXmlNode* child = parent->FirstChild("TLFLandmarkAttr"); child; child = child->NextSibling())
	{
		TiXmlElement* e = child->ToElement();
		TLFLandmarkAttr* attr = TLFLandmarkAttr::LoadXML(e);
		if (attr != NULL)
			result->Append(attr);
		else
		{
			delete result;
			return NULL;
		}

	}
	return result;
}
/*
TLFLandmarkFile
*/
TLFLandmarkFile::TLFLandmarkFile(const char* fileName)
{
	m_fileName = fileName;
}
TLFLandmarkFile::~TLFLandmarkFile()
{
}

const char* TLFLandmarkFile::FileName()
{
	return m_fileName.c_str();
}

void TLFLandmarkFile::SetFileName(const char* fileName)
{
	m_fileName = fileName;
}

int TLFLandmarkFile::Count()
{
	return m_landmarks.GetCount();
}
TLFLandmark* TLFLandmarkFile::Landmark(int index)
{
	return dynamic_cast<TLFLandmark*>(m_landmarks.Get(index));
}

void TLFLandmarkFile::Delete(int index)
{
	m_landmarks.Delete(index);
}
void TLFLandmarkFile::Delete(const TLFString& strUUID)
{
	for (int i = m_landmarks.GetCount() - 1; i >= 0; i--)
	{
		TLFLandmark* lnd = Landmark(i);
		TLFString _id = lnd->GetId();
		if (strUUID == _id)
			m_landmarks.Delete(i);
	}
}

void TLFLandmarkFile::Append(TLFLandmark* landmark)
{
	if (landmark != NULL)
		m_landmarks.Add(landmark);
}

TiXmlElement* TLFLandmarkFile::SaveXML()
{
	TiXmlElement* e = new TiXmlElement(GetName());
	e->SetAttribute("FileName", m_fileName.c_str());
	for (int i = 0; i < m_landmarks.GetCount(); i++)
	{
		TLFLandmark* lnd = Landmark(i);
		if (lnd != NULL)
		{
			TiXmlElement* f = lnd->SaveXML();
			if (f != NULL)
				e->LinkEndChild(f);
			else
			{
				delete e;
				return NULL;
			}
		}
		else
		{
			delete e;
			return NULL;
		}
	}
	return e;
}
TLFLandmarkFile* TLFLandmarkFile::LoadXML(TiXmlElement* parent, TLFLandmarkAttributes* attrs)
{
	if (parent == NULL)
		return NULL;

	const char* name = parent->Value();
	if (strcmp(name, "TLFLandmarkFile") != 0)
		return NULL;
	const char* strFileName = parent->Attribute("FileName");
	if (strFileName == NULL)
		return NULL;
	TLFLandmarkFile* result = new TLFLandmarkFile(strFileName);
	for (TiXmlNode* child = parent->FirstChild("TLFLandmark"); child; child = child->NextSibling())
	{
		TiXmlElement* e = child->ToElement();
		TLFLandmark* lnd = TLFLandmark::LoadXML(e, attrs);
		if (lnd != NULL)
			result->Append(lnd);
		else
		{
			delete result;
			return NULL;
		}
	}
	return result;
}
/*
TLFLandmarkFiles
*/

TLFLandmarkFiles::TLFLandmarkFiles()
{
}
TLFLandmarkFiles::~TLFLandmarkFiles()
{
}

int TLFLandmarkFiles::Count()
{
	return m_files.GetCount();
}
char asciitolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}
int TLFLandmarkFiles::GetFileIndex(const char* fileName)
{
	if (fileName == NULL)
		return -1;
	TLFString fn = fileName;
	std::transform(fn.begin(), fn.end(), fn.begin(), asciitolower);
	for (int i = 0; i < m_files.GetCount(); i++)
	{
		TLFLandmarkFile* f = dynamic_cast<TLFLandmarkFile*>(m_files.Get(i));
		TLFString fn1 = f->FileName();
		std::transform(fn1.begin(), fn1.end(), fn1.begin(), asciitolower);
		if (fn == fn1)
			return i;
	}
	return -1;
}


TLFLandmarkFile* TLFLandmarkFiles::File(int index)
{
	return dynamic_cast<TLFLandmarkFile*>(m_files.Get(index));
}

TLFLandmarkFile* TLFLandmarkFiles::File(const char* fileName)
{
	int index = GetFileIndex(fileName);
	if (index < 0)
		return NULL;
	return File(index);
}

void TLFLandmarkFiles::Delete(int index)
{
	m_files.Delete(index);
}
void TLFLandmarkFiles::Delete(const char* fileName)
{
	int index = GetFileIndex(fileName);
	if (index < 0)
		return;
	Delete(index);
}

void TLFLandmarkFiles::Append(TLFLandmarkFile* file)
{
	m_files.Add(file);
}
void TLFLandmarkFiles::Clear()
{
	m_files.Clear();
}

TiXmlElement* TLFLandmarkFiles::SaveXML()
{
	TiXmlElement* e = new TiXmlElement(GetName());
	for (int i = 0; i < m_files.GetCount(); i++)
	{
		TLFLandmarkFile* file = File(i);
		if (file != NULL)
		{
			TiXmlElement* f = file->SaveXML();
			if (f != NULL)
				e->LinkEndChild(f);
			else
			{
				delete e;
				return NULL;
			}
		}
		else
		{
			delete e;
			return NULL;
		}
	}
	return e;

}
TLFLandmarkFiles* TLFLandmarkFiles::LoadXML(TiXmlElement* parent, TLFLandmarkAttributes* attrs)
{
	if (parent == NULL)
		return NULL;
	parent = parent->FirstChildElement("TLFLandmarkFiles");
	if (parent == NULL)
		return NULL;

	const char* name = parent->Value();
	if (strcmp(name, "TLFLandmarkFiles") != 0)
		return NULL;

	TLFLandmarkFiles* result = new TLFLandmarkFiles();
	for (TiXmlNode* child = parent->FirstChild("TLFLandmarkFile"); child; child = child->NextSibling())
	{
		TiXmlElement* e = child->ToElement();
		TLFLandmarkFile* f = TLFLandmarkFile::LoadXML(e, attrs);
		if (f != NULL)
			result->Append(f);
		else
		{
			delete result;
			return NULL;
		}
	}
	return result;
}

/*
TLFDBLandmarks
*/
TLFDBLandmarks::TLFDBLandmarks()
{
	m_fileName = "";
	m_attributes = new TLFLandmarkAttributes();
	m_files = new TLFLandmarkFiles();
}
TLFDBLandmarks::~TLFDBLandmarks()
{
	Close();
	if (m_attributes != NULL)
		delete m_attributes;
	if (m_files != NULL)
		delete m_files;
}
/*this function using only when create database*/
void TLFDBLandmarks::SetAttributes(TLFLandmarkAttributes& attributes)
{
	//set arributes 
	if (m_attributes == NULL)
		m_attributes = new TLFLandmarkAttributes();

	m_attributes->Clear();
	for (int i = 0; i < attributes.Count(); i++)
	{
		TLFString id = attributes.Attribute(i)->GetID();
		TLFString className = attributes.Attribute(i)->ClassName();
		int color = attributes.Attribute(i)->Color();
		TLFLandmarkAttr* attr = new TLFLandmarkAttr(id, color, className.c_str());
		m_attributes->Append(attr);
	}
}

TLFDBLandmarks* TLFDBLandmarks::CreateDatabase(TLFLandmarkAttributes& attributes, const char* fileName)
{
	TLFDBLandmarks* landmarks = new TLFDBLandmarks();
	landmarks->SetAttributes(attributes);
	landmarks->SetFileName(fileName);
	if (landmarks->Save() && landmarks->Connect(fileName))
		return landmarks;
	else
	{
		delete landmarks;
		return NULL;
	}
}

bool TLFDBLandmarks::Connect(const char* fileName)
{
	bool res = false;
	if (m_fileName != "")
		Close();
	if (res = Load(fileName))
		m_fileName = fileName;
	return res;
}
void TLFDBLandmarks::Close()
{
	if (m_fileName != "")
		Save();
	m_attributes->Clear();
	m_files->Clear();
	m_fileName = "";
}

void TLFDBLandmarks::SetFileName(const char* fileName)
{
	m_fileName = fileName;
}
bool TLFDBLandmarks::Save()
{
	if (m_fileName == "")
		return false;
	TiXmlDocument doc; 
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* parent = new TiXmlElement(GetName());

	TiXmlElement* attributes = m_attributes->SaveXML();
	if (attributes == NULL)
		return false;
	parent->LinkEndChild(attributes);
	TiXmlElement* files = m_files->SaveXML();
	if (files == NULL)
		return false;
	parent->LinkEndChild(files);
	doc.LinkEndChild(parent);
	return doc.SaveFile(m_fileName.c_str());
}
bool TLFDBLandmarks::Load(const char* fileName)
{
	TiXmlDocument doc(fileName);
	if (!doc.LoadFile())
		return false;
 
	TiXmlElement* pElem = doc.FirstChildElement(this->GetName());
	if (pElem == NULL)
		return false;
	
	m_attributes->Clear();
	m_files->Clear();

	delete m_attributes;
	delete m_files;

	m_attributes = TLFLandmarkAttributes::LoadXML(pElem);
	if (m_attributes == NULL)
	{
		m_attributes = new TLFLandmarkAttributes();
		m_files = new TLFLandmarkFiles();
		return false;
	}

	m_files = TLFLandmarkFiles::LoadXML(pElem, m_attributes);
	if (m_files == NULL)
	{
		m_files = new TLFLandmarkFiles();
		return false;
	}
		
	m_fileName = fileName;

	return true;
}

TLFLandmarkAttributes* TLFDBLandmarks::Attributes()
{
	return m_attributes;
}

TLFLandmarkFiles*	   TLFDBLandmarks::Files()
{
	return m_files;
}
const char*			   TLFDBLandmarks::FileName()
{
	return m_fileName.c_str();
}
