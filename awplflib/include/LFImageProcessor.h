

#ifndef _LF_IMAGE_PROCESSOR_H_
#define _LF_IMAGE_PROCESSOR_H_

/** \addtogroup LFInterfaces
*   @{
*/

/** @brief Generalized image processing interface 
*/
class ILFImageProcessor : public TLFObject
{
protected:
	/** @brief image containers 
	*/
	TLFImage m_SourceImage;
	TLFImage m_ResultImage;
	/** @brief image processing area 
	*/
	TLFRect  m_ImageRoi;
	/* вектор параметров
	*/
	TLFObjectList m_Params;
	virtual bool Process() = 0;
public:
	ILFImageProcessor();
	virtual ~ILFImageProcessor();
	/** @brief images as property 
	*/
	virtual bool SetSourceImage(TLFImage* pImage);
	virtual TLFImage* GetSourceImage();
	virtual TLFImage* GetResultImage();
	/** @brief parameters
	*/
	int GetNumParameters();
	TLFParameter* GetParameter(int index);
	bool SetParameterValue(double Value, int index);
	/** @brief image processing area as property 
	*/
	TLFRect GetRoi();
	void SetRoi(TLFRect& Rect);
};
/** @} */ 
#endif //_LF_IMAGE_PROCESSOR_H_